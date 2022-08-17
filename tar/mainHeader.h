#ifndef MAIN_H
#define MAIN_H

#include "checkInput.c"
#include "recursiveAdd.c"
#include "extract.c"

// function defintions 
int createHeader(int x);
int checkInput(char* arg);
void recursiveAdd(char *basePath, char* archive);
void add(char* file,char* archive);
void extract(char* arc);

#endif
