/*
 * hc908_monitor_commands.h
 *
 *  Created on: 31.05.2020
 *      Author: pantec
 */

#ifndef HC908_MONITOR_COMMANDS_H_
#define HC908_MONITOR_COMMANDS_H_


typedef enum {
	eMonitorUninitialized = 0,

	eMonitorSecurity = 1,

	eMonitorReadMem = 0x4A,
	eMonitorWriteMem = 0x49,
	eMonitorIncRead = 0x1A,
	eMonitorIncWrite = 0x19,
	eMonitorReadSP = 0x0c,
	eMonitorExec =  0x28,

	eMonitorResetDevice = 'Ä', // 0xEE,		// 'Ä'
	eMonitorGetBaudRate = 'Ö', // 0xED,		// 'Ö'
} hc908_monitor_cmds_e;

/*
 * Ident		0x49	'I'
 * Quit			0x51	'Q'
 * Write		0x57	'W'
 * Read			0x52	'R'
 * Erase		0x45	'E'
 * Ack			0xFC
 *
 * */

#endif /* HC908_MONITOR_COMMANDS_H_ */
