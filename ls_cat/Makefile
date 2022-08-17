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

wy: wyls.c wycat.c
	${CC} ${CFLAGS} wyls.c -lm -o wyls
	${CC} ${CFLAGS} wycat.c -lm -o wycat
clean:
	${RM} wyls core.*
