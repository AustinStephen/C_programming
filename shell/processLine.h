/*
* processLine.c
* Author: Austin Stephen
* Date: April 18, 2022
*
* COSC 3750, Homework 8
*
* header file for processLine implementation
*
*/

#ifndef PROCESS
#define PROCESS

#include "wyscanner.h"
#include "linkedList.h"
#include "processCommand.h"

/*
* Controller processes a series of commands on a line from stdin
* Prints errors if it fails but will not end the main program
*/
extern int processLine();

#endif