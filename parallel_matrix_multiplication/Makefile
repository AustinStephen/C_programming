#######
# Makefile Author: Austin Stephen
# Date: April 18, 2022
#
# COSC 3750, Homework 11
#
# My Makefile for Homework 11
######
CC=gcc
CFLAGS=-Wall -pthread -ggdb -g
RM=/bin/rm -f

.PHONY: clean all

mmult: mmult.c
	${CC} ${CFLAGS} mmult.c -o mmult 

clean:
	${RM} mmult core.*
