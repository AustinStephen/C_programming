#######
# Makefile Author: Austin Stephen
# Date: March 2, 2022
#
# COSC 3750, Homework 5
#
# My Makefile for Homework 5
######
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f

.PHONY: clean

wyls: wyls.c
	${CC} ${CFLAGS} wyls.c -lm -o wyls

clean:
	${RM} wyls core.*
