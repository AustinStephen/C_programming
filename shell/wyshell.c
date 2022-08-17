/*
* wyshell.c
* Author: Austin Stephen
* Date: April 10, 2022
*
* COSC 3750, Homework 8
*
* Controller file for the entire command line 
* implementation up to homework 8.
*
* Acknowledgement of resources 
* https://www.studymite.com/blog/strings-in-c
* 
*/
#include "processLine.h"

int main(int argc,char **argv){

  // loop forever processing the user input
  while(1){

    // subroutine process one line of input.
    int rtn = processLine();
    if(rtn == 1) return 1;
  }
  return 0;
}