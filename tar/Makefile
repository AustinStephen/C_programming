#######
# Makefile Author: Austin Stephen
# Date: March 22, 2022
#
# COSC 3750, Homework 6
#
# My Makefile for Homework 6
######

CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f

.PHONY: clean

wytar: extract.c recursiveAdd.c checkInput.c add.c mainHeader.h wytar.c
	${CC} ${CFLAGS} wytar.c -lm -o wytar

clean:
	${RM} wytar core.*
