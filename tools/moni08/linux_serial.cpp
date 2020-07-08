/*
 * linux_serial.cpp
 *
 *  Created on: 08.07.2020
 *      Author: pantec
 */


#undef DEBUG	// define this if assumed to be bugfree


// need some components from this library to be included first
#include "tns_util/daemonic.h"

#include "linux_serial.h"


#ifndef SERIAL_DEFAULT_BAUD
	#define	SERIAL_DEFAULT_BAUD		38400
#endif

#ifndef SERIAL_DEFAULT_TIMEOUT
	#define	SERIAL_DEFAULT_TIMEOUT	2		// 200 ms
#endif


#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>


/* Management of the serial line                                            */
/* These routines provide a set of simple primitives to read and write on   */
/* the serial line                                                          */

/* configure serial link fd      */

int key2baud[] = {
      		0,
            50,
            75,
            110,
            134,
            150,
            200,
            300,
            600,
            1200,
            1800,
            2400,
            4800,
            9600,
            19200,
            38400,
            57600,
            115200,
            230400,
            460800,
			-1,
			-1,
			-1
};


typedef struct {
	short bits;
	int mask;
} t_serbits;


t_serbits serbits[] = {
		{5, CS5},
		{6, CS6},
		{7, CS7},
		{8, CS8},
		{-1, CS8}
};

static bool non_standard_Baud_requested = false;

/*
main() {
int fd, sercmd, serstat;

sercmd = TIOCM_RTS;
fd = open("/dev/ttyS0", O_RDONLY); // Open the serial port.


// Read the RTS pin status.
ioctl(fd, TIOCMGET, &serstat);
if (serstat & TIOCM_RTS)
printf("RTS pin is set.\n");
else
printf("RTS pin is reset.\n");

getchar(); // Wait for the return key before continuing.

close(fd);
}
*/

#ifndef CMSPAR
		#define CMSPAR 010000000000
		/* mark or space (stick) parity */
		#warning "__ CMSPAR __ not defined in your old termios.h"
#endif


void setMCRbits(int fd, int bitName, int value)
{
	int status;

	if (ioctl(fd, TIOCMGET, &status)<0) {
		perror("setMCRbits(): TIOCMGET status");
//		status = 0;
	}
	if (value == 0) {
		fprintf(stderr, "Resetting the %d pin.\n", bitName);
	  	status &= ~bitName;	// Reset the RTS pin.
	} else {
		fprintf(stderr, "Setting the %d pin.\n", bitName);
		status |= bitName;	// Set the RTS pin.
	}
	if (ioctl(fd, TIOCMSET, &status)<0) {
		fprintf(stderr,"set mcr 0x%08x: %s\n",status,strerror(errno));
	}
}

int getMCRbits(int fd, int bitName)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"TIOCMGET: 0x%04x\n",TIOCMGET);
#endif
	int serstat;
	// Read the RTS pin status.
	if (ioctl(fd, TIOCMGET, &serstat)<0) {
		perror("getMCRbits() TIOCMGET serstat");
	}
	if (serstat & bitName)
		return 1;
//		printf("RTS pin is set.\n");
//	else
//		printf("RTS pin is reset.\n");
	return 0;
}


void setRTS(int fd, int v)
{
	setMCRbits(fd, TIOCM_RTS, v);
}

int getRTS(int fd)
{
	return getMCRbits(fd, TIOCM_RTS);
}

void setDTR(int fd, int v)
{
	setMCRbits(fd, TIOCM_DTR, v);
}

int getDTR(int fd)
{
	return getMCRbits(fd, TIOCM_DTR);
}

void toggleDTR(int fd)
{
	if (getDTR(fd) > 0) {
		setDTR(fd, 0);
	} else {
		setDTR(fd, 1);
	}
}

void toggleRTS(int fd)
{
	if (getRTS(fd) > 0) {
		setRTS(fd, 0);
	} else {
		setRTS(fd, 1);
	}
}

static bool bGotFirstTermFlags = false;
static struct termios first_term_flags;

int tcgetattr_(int fd, struct termios *term)
{
	int retval = tcgetattr(fd, term);
	if (! bGotFirstTermFlags) {
		if (retval == 0) {
			memcpy(&first_term_flags, term, sizeof(struct termios) );
			bGotFirstTermFlags = true;
		} else {
			perror("tcgetattr_(), cant store first attributes");
		}
	}
	return retval;
}

int restore_all_term_flags(int fd)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	if (!bGotFirstTermFlags) {
		fprintf(stderr,"term flags not yet saved\n");
		return -1;
	}
	int retval = tcsetattr(fd, TCSANOW, &first_term_flags);
	if (retval != 0) {
		perror("restore_all_term_flags(), cant restore attributes found @ start");
	}
	return retval;
}

int restore_and_close(int fd)
{
	int retval = -1;

	if (fd >= 0) {  // safety precaution
		retval = restore_all_term_flags(fd);
	}

	close(fd);
	bGotFirstTermFlags = false;

	return retval;
}



bool tty_serial_timeout_set (int fd, int dsec)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	struct termios term;
	if (tcgetattr_(fd,&term)!=0) {
		perror("tty_serial_timeout_set(), getting attributes");
		return false;
	}

	if (dsec == 0) {
		term.c_cc[VMIN] = 1; //wait for at least one character (never times out)
	} else {
		term.c_cc[VMIN] = 0; //return after timeout, even with nothing
	}
	term.c_cc[VTIME] = dsec; //try reading for this amount of time (in deciseconds)

	if (tcsetattr(fd,TCSANOW,&term)!=0) {
		perror("tty_serial_timeout_set(), setting attributes");
		return false;
	}
	return true;
}


int read_serial_flag(int fd, int flag)
{
        struct termios term;
        if (tcgetattr_(fd,&term) != 0) {
                EPRINTF("serial_flags(), getting attributes, %s\n", strerror(errno));
                return -1;
        }
        switch(flag) {
                case IXON:
                        return ((term.c_iflag & flag)>0) ? 1 : 0;

                case CRTSCTS:
                case CLOCAL:
                        return ((term.c_cflag & flag)>0) ? 1 : 0;
        }
        return -1;
}


bool serial_flags(int fd, int flag, int onOff)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
        struct termios term;
        if (tcgetattr_(fd,&term) != 0) {
                fprintf(stderr,"serial_flags(), getting attributes, %s\n", strerror(errno));
                return false;
        }
        switch(flag) {
                case IXON:
                        if (onOff > 0) {
                                set(term.c_iflag, flag);
                        } else {
                                clear(term.c_iflag, flag);
                        }
                        break;

                case CRTSCTS:
                case CLOCAL:
                        if (onOff > 0) {
                                set(term.c_cflag, flag);
                        } else {
                                clear(term.c_cflag, flag);
                        }
                        break;
        }
        if (tcsetattr(fd,TCSANOW,&term)!=0) {
                perror("serial_configure(), setting attributes");
                return false;
        }
        return true;
}


int parity_key(int parity)
{
	int result = 0;
	switch(parity) {
		case 'm':
		case 'M':
			result |= CMSPAR;
			break;

		case 's':
		case 'S':
			result |= (PARENB | CMSPAR);
			break;

		case 'N':
		case 'n':
		case '0':
		default:
			break;

		case 'o':
		case 'O':
		case '1':
			result |= (PARODD | PARENB);		// parity must be enabled also
			break;

		case 'e':
		case 'E':
		case '2':
			result |= PARENB;
			break;
	}
	return result;
}

bool serial_configure_par(int fd, int parity)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	struct termios term;
	if (tcgetattr_(fd,&term) != 0) {
		fprintf(stderr,"serial_configure_par(), getting attributes, %s\n", strerror(errno));
		return false;
	}

	clear(term.c_cflag,PARENB|PARODD|CMSPAR);     /* Control modes */
	set(term.c_cflag, parity_key(parity));

	if (tcsetattr(fd,TCSANOW,&term)!=0) {
		perror("serial_configure_par(), setting attributes");
		return false;
	}
	return true;
}

bool serial_configure_modem(int fd, int xonxoff)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	struct termios term;
	if (tcgetattr_(fd,&term) != 0) {
		fprintf(stderr,"serial_configure_modem(), getting attributes, %s\n", strerror(errno));
		return false;
	}

	clear(term.c_iflag, IXON|IXOFF|IXANY);
	if (xonxoff > 0) {
		set(term.c_iflag, IXON|IXOFF);
	} else {
		set(term.c_iflag, IXANY);
	}

	if (tcsetattr(fd,TCSANOW,&term)!=0) {
		perror("serial_configure_modem(), setting attributes");
		return false;
	}
	return true;
}


long requestedSpeedKey(int real_baud)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
 	switch(real_baud) {
		case 50:	return B50;
		case 75:	return B75; break;
		case 110:	return B110; break;
		case 134:	return B134; break;
		case 150:	return B150; break;
		case 200:	return B200; break;
		case 300:	return B300; break;
		case 600:	return B600; break;
		case 1200:	return B1200; break;
		case 4800:	return B4800; break;
		case 9600:	return B9600; break;
		case 19200:	return B19200; break;
		case 38400:	return B38400; break;
		case 57600:	return B57600;
		case 115200: return B115200;
		case 230400: return B230400;
		case 460800: return B460800;
		default: return -1;
 	}
 	return -2;
}

bool setCustomSerialSpeed(int realBaud, char *serialDeviceName)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	char cmd[256];

	int custom_divider = 115200 / realBaud;

	sprintf(cmd,"(setserial %s divisor %d && setserial %s spd_cust)"
			,serialDeviceName, custom_divider, serialDeviceName);

	int ret = system(cmd);
	if (ret != 0) {
		EPRINTF("setserial %s failed, using non-standard baudrate %d, assuming you have set custom divider %d\n", serialDeviceName, realBaud, custom_divider);
		return false;
	}
	return true;
}

#if 0
// clipboard junk
		default:
//#ifdef LINUX
			{
				// speedkey = CBAUDEX | 15;

				non_standard_Baud_requested = true;

				char sfileName[80];
				char *serialDeviceName = fileNameFromDesc(fd, sfileName);
				if (serialDeviceName == NULL) {
					// error
					EPRINTF("could not get filename for serial device %d, using non-standard baudrate %d, assuming you have set custom divider %d\n", fd, speed, custom_divider);
					return false;
				} else {

				}

				speedkey = B38400;
//				set(term.c_cflag, CIBAUD);
			}
			break;
//#else
//			return false;
//#endif
	}
}
	return true;
}

#endif



bool serial_configure_ext(int fd, int speed, int bits, int stop, int parity, bool hwflow)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
	struct termios term;
	static long	 speedkey;

	speedkey = requestedSpeedKey(speed);
	if ((speedkey == B38400) && non_standard_Baud_requested) {
		// warn, could not revert back
		EPRINTF("non-standard baudrate was in effect, assuming you have reset to spd_normal");
	}
	if (speedkey < 0) {
		non_standard_Baud_requested = true;
		speedkey = B38400;
		int custom_divider = 115200 / speed;
		// warn, need to have setserial before open
		EPRINTF("using non-standard baudrate %d, assuming you have set custom divider %d", speed, custom_divider);
	}

	if (tcgetattr_(fd,&term) != 0) {
		EPRINTF("serial_configure_ext(), getting attributes, %s", strerror(errno));
		return false;
	}

	unsigned int i = cfgetispeed(&term);

	fprintf(stderr,"i: 0x0%x\n",i);
	if (i < (sizeof(key2baud)/sizeof(int))) {
		int j = key2baud[i];
		fprintf(stderr,"speed change from %d ",j);
	}

#if 1
	if (cfsetospeed(&term, speedkey) < 0) {
		perror("serial_configure(), cfsetospeed:");
	}
	if (cfsetispeed(&term, speedkey) < 0) {  /* Speed */
		perror("serial_configure(), cfsetispeed:");
	}
#endif
#if 0
	if (cfsetspeed(&term, speedkey) < 0) {
		perror("serial_configure(), cfsetspeed:");
	}
#endif
/*
	if (cfsetospeed(&term,speedkey) < 0) {
		perror("serial_configure(), cfsetospeed:");
	}
*/
//	i = CIBAUD;

	i = cfgetispeed(&term);
	fprintf(stderr,"i: %d\n",i);
	if (i < (sizeof(key2baud)/sizeof(int))) {
		fprintf(stderr,"to %d Baud\n",key2baud[i]);
	}
/*
	i = cfgetospeed(&term);
	fprintf(stderr,"ospeed i: %d\n",i);
	if (i < (sizeof(key2baud)/sizeof(int))) {
		fprintf(stderr,"to %d Baud\n",key2baud[i]);
	}
*/
	/* Input modes */
	clear(term.c_iflag, IGNPAR|INPCK|ISTRIP|ICRNL|INLCR|IXON|IXOFF);
	set(term.c_iflag, IGNBRK|BRKINT|IXANY);

	clear(term.c_oflag, OPOST);    /* Output modes */
	set(term.c_oflag, 0);

/*
	CRTSCTS
  	CLOCAL
*/
	int sizekey = CS8;
	int j = 0;
	while (serbits[j].bits != -1) {
		if (serbits[j].bits == bits) {
			sizekey = serbits[j].mask;
			break;
		}
		j++;
	}

	if (stop > 0) {
		sizekey |= CSTOPB;
	}

	if (hwflow) {
		sizekey |= CRTSCTS;
	}

	sizekey |= parity_key(parity);

	clear(term.c_cflag, CSIZE|PARENB|CSTOPB|PARODD|CRTSCTS|CMSPAR);     /* Control modes */
	set(term.c_cflag, sizekey|CREAD|CLOCAL);
                                               /* Line modes */
	clear(term.c_lflag,ISIG|ICANON|XCASE|ECHO|IEXTEN);
	set(term.c_lflag,0);   /* MIN and TIME */

	term.c_cc[VMIN]  = 0;
	term.c_cc[VTIME] = SERIAL_DEFAULT_TIMEOUT;

	if (tcsetattr(fd, TCSANOW, &term)!=0) {
		perror("serial_configure_ext(), setting attributes");
		return false;
	}
	return true;
}


bool serial_configure(int fd, int speed, int bits)
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
//	char ci[256];
//	strcpy(ci, copyright_mod);
	return serial_configure_ext(fd, speed, bits, 0, 'n', 0);
}


int serial_open(char *portname) /* open serial link on portname */
{
#ifdef WITH_DEBUG_TRACE
	fprintf(stderr,"%s\n",__func__);
#endif
  int fd;

  if ((fd = open(portname,O_RDWR|O_EXCL|O_NONBLOCK|O_NOCTTY)) > 0)
    {
     serial_configure(fd, SERIAL_DEFAULT_BAUD, 8);
     return(fd);
    }
   else return 0;
}

/* Read until and including \n */
/* Terminate char * with \0    */
int serial_readline(int  fd,char buffer[],int size)
{
  int  nrd,rsize;
  char c;

  rsize = 0;
  do
  {
    nrd = read(fd,&c,1);

//    fprintf(stderr,"read 1 byte [%c]\n",c);

    if (nrd < 0) break;
    buffer[rsize] = c;
    rsize += nrd;
  } while (nrd==1 && rsize < size && c != '\n');

  buffer[rsize] = '\0';

#ifdef DEBUG
  fprintf(stderr,"serial_readline: [%s]\n",buffer);
#endif


  if (nrd>=0) return(rsize);
  else
  {
    perror("serial_readline()");
    return(nrd);
  }
}



unsigned long buffer_delay(unsigned long nBytes, unsigned long Baud, unsigned long iSerialBits, unsigned long iSerialStop, char iSerialParity)
{
	unsigned long _blockFillDelay = 0;

	unsigned long nBits = iSerialBits;	// care for 5,6,7,8 bit data modes
	nBits += iSerialStop;		// count extra
	if (tolower(iSerialParity) != 'n') {
		nBits += 1UL;
	}
	// int uSleep = (iBlockRecv * 1000000 * nBits) / Baud;    // 		chars  /  (byte per second)
	_blockFillDelay = 1000000UL;
	_blockFillDelay *= nBytes;
	_blockFillDelay *= nBits;
	_blockFillDelay /= Baud;

	return _blockFillDelay;
}



