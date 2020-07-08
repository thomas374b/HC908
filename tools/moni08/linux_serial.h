/*
 * linux_serial.h
 *
 *  Created on: 08.07.2020
 *      Author: pantec
 */

#ifndef LINUX_SERIAL_H_
#define LINUX_SERIAL_H_


#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <termio.h>

/*---------------------------------------------------------------------------*/
/* Management of the serial line                                             */
/*---------------------------------------------------------------------------*/
/* These routines provide a set of simple primitives to read and write on    */
/* the serial line                                                           */
/*---------------------------------------------------------------------------*/

#ifndef SERIALPARSER_H_
	#define clear(var,mask)         var &= (~(mask))
	#define set(var,mask)           var |= (mask)
	#define let(var,mask)
	#define unused(var,mask)
#endif


// #define	serial_close(fd)                close(fd)
#define	serial_read(fd,buffer,size)     read(fd,buffer,size)
#define	serial_write(fd,buffer,size)    write(fd,buffer,size)

#if  __GNUC__
extern int      serial_open(char portname[]);

// extern bool		serial_configure(int fd, int speed, int bits);
// extern bool 	serial_configure_ext(int fd, int speed, int bits, int stop, int parity);

extern int 		restore_all_term_flags(int fd);
extern int		restore_and_close(int fd);


extern bool		serial_configure(int fd, int speed, int bits);
extern bool		serial_configure_ext(int fd, int speed, int bits, int stop, int parity, bool hwflow);
extern bool 	serial_configure_par(int fd, int parity);
extern bool		serial_configure_modem(int fd, int xonxoff);

extern int      serial_readline(int fd,char buffer[],int size);
extern int      serial_readexactly(int fd,char buffer[],int size);
extern void     serial_drain(int fd, bool verbose);
extern bool		tty_serial_timeout_set(int fd, int dsec);
extern bool 	serial_flags(int fd, int flag, int onOff);
extern int 		read_serial_flag(int fd, int flag);

#ifndef WITHOUT_RTS_DTR

extern void setRTS(int fd, int v);
extern int getRTS(int fd);
extern void setDTR(int fd, int v);
extern int getDTR(int fd);
extern void toggleDTR(int fd);
extern void toggleRTS(int fd);

extern void setMCRbits(int fd, int bitName, int value);
extern int getMCRbits(int fd, int bitName);

#endif

// calculate time to receive nBytes @ given baudrate in µs
extern unsigned long buffer_delay(unsigned long nBytes, unsigned long Baud, unsigned long iSerialBits, unsigned long iSerialStop, char iSerialParity);

// return -1 if baudrate is non-standard (i.e. requires a custom divisor)
extern long requestedSpeedKey(int real_baud);

// on Linux only! call system("setserial ...") to configure custom baudrate divisor
extern bool setCustomSerialSpeed(int realBaud, char *serialDeviceName);

#endif


#endif /* LINUX_SERIAL_H_ */
