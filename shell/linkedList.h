/*
* linkedList.h
* Author: Austin Stephen
* Date: April 18, 2022
*
* COSC 3750, Homework 9
*
* Contains the declarations for the linked list 
* data structure used to manage all of the user input.
*
*/

#ifndef DATA
#define DATA

typedef struct node Node;
typedef struct word Word;

struct node {
  Node *next, *prev;
  char *command;
  int operator;
  Word *arg_list;
  int input, output, error;
  char *in_file, *outfile, *err_file; 

  ///// For maintining the state /////
  // Most of the logic is implemented in in operator checks.
  // These are special purpose flags not achived easily with operator checks.
  int pipe;
  int semicolon;
  int quiet;
  int append;
};

struct word {
  struct word *next, *prev;
  char *string;
};

/*
* Initalizes the Prev pointer to Null and this will stay Null for the 
* as this is how you tell it is the first node.
*/
extern Node* initilize();

/*
* Add an exising node to the structure by rearranging pointers
*/
extern void addNode(Node* current, char* command);

/*
* Add a word to arg_list
*/
extern void addWord(Node* current, char *word);

/*
* Prints out the entire structure with formatting for debugging.
*/
extern void printStruct(Node* toPrint);

/*
* Deletes all of the words stored in arg_list
*/

extern void deleteWords(Word* arg);

/*
* Frees all of the memory for the entire structure.
*/
extern void cleanUp(Node* current);

#endif