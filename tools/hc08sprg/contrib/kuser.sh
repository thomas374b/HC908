#!/bin/bash
#
#  kills the program that is using my TTY (aka COM PORT)
#
#  this scripts ensures that only one program is using the serial port
#  works only on linux because of contents of /proc filesystem
#
#

if [ "$1" = "" ]
then
	echo "no argument given" >&2
	exit 0
fi

PID=`ls -altr /proc/*/fd/* 2>/dev/null | grep "$1" | cut -f3 -d/`

if [ "$PID" = "" ]
then
	echo "no user found" >&2
	exit 0
fi

kill -15 $PID
