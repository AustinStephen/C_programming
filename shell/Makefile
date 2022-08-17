#######
# Makefile Author: Austin Stephen
# Date: April 18, 2022
#
# COSC 3750, Homework 10
#
# My Makefile for Homework 10
######
CC=gcc
CFLAGS=-Wall -ggdb -g
RM=/bin/rm -f

.PHONY: clean all

wyshell: wyshell.c processLine.c linkedList.c processCommand.c
	${CC} ${CFLAGS} wyshell.c wyscanner.c linkedList.c processLine.c processCommand.c -o wyshell

clean:
	${RM} wyshell core.*
