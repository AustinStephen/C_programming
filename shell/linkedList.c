/*
* linkedList.c
* Author: Austin Stephen
* Date: April 18, 2022
*
* COSC 3750, Homework 9
*
* contains the helper function for the linkedList used to
* manage all of the tokenized user input.
*
*/

#include <stdio.h>
#include <stdlib.h>

#include "linkedList.h"

Node* initilize(){
  
  // create an empty Node
  Node* linkedList;
  linkedList = calloc(1,sizeof(Node));

  // if calloc worked correctly
  if(linkedList != NULL){
    // Currently only 1 node
    linkedList->next = NULL;
    
    // Will always stay Null
    linkedList->prev = NULL;

    // Set the inital word to null
    linkedList->arg_list = NULL;


    // The rest will default to Null
    linkedList->command = NULL;
    linkedList->operator = 0;
    linkedList->input = 0;
    linkedList->output = 0;
    linkedList->error = 0;
    linkedList->in_file = NULL;
    linkedList->outfile = NULL;
    linkedList->err_file = NULL;

    // for maintining the state
    linkedList->pipe =0;
    linkedList->semicolon =0;
    linkedList->quiet=0;
    linkedList->append=0;
    }
    else{
      perror("calloc failed");
    }
  

  return(linkedList);

}

void addNode(Node* current, char* command){

    // create an empty Node
  Node* newNode;
  newNode = calloc(1,sizeof(Node));

  // if calloc suceeded
  if (newNode != 0 ){
    // walk to the last element in the list and 
    // store it in lastElm to be appended to
    Node* tmp = current;
    Node* lastElm = tmp;
    while (tmp != NULL){
      lastElm = tmp;
      tmp = tmp->next;
    }

    // prev points to old word
    newNode->prev = lastElm;
    // last word points to new word
    lastElm->next = newNode;
    // string is new word
    newNode->command = command;
    // no next word yet
    newNode->next = NULL;
  }
  else{
    perror("calloc failed");
  }

}

void addWord(Node* current, char *word){
  // create an empty Node
  Word* newWord;
  newWord = calloc(1,sizeof(Word));

  // if calloc suceeded
  if (newWord != 0 ){

    // if there is a word in arg_list add to it,
    // otherwise make the first one by assigning one to arg_list
    if(current->arg_list != NULL){
      
      // walkt to the last element in the list and 
      // store it in lastElm to be appended to
      Word* tmp = current->arg_list;
      Word* lastElm = tmp;
      while (tmp != NULL){
        lastElm = tmp;
        tmp = tmp->next;
      }
      
      // prev points to old word
      newWord->prev = lastElm;
      // last word points to new word
      lastElm->next = newWord;
      // string is new word
      newWord->string = word;
      // no next word yet
      newWord->next = NULL;
    }
    else{
      // no previous word yet this is the head
      newWord->prev = NULL;
      // string is new word
      newWord->string = word;
      // no next word yet
      newWord->next = NULL;
      // Current arg_list should point to this word now
      current->arg_list = newWord;
    }
  }
  else{
    perror("calloc failed");
  }
}

void printStruct(Node* toPrint){
  
  while(toPrint != NULL){
    printf("\nPrinting Node : \n");
    printf("command: %s \n", toPrint->command);
    printf("input: %d \n", toPrint->input);
    printf("output: %d \n", toPrint->output);
    printf("error: %d \n", toPrint->error);

    // Print the whole arg_list
    // moves backwords because we start at the end of the list
    Word* current = toPrint->arg_list;
    while (current != NULL){
      printf("Word(s): %s \n" , current->string);
      current = current->next;
    }
    printf("operator: %d \n", toPrint->operator);
    printf("in_file: %s \n", toPrint->in_file);
    printf("out_file: %s \n", toPrint->outfile);
    printf("err_file: %s \n", toPrint->err_file);

    toPrint = toPrint->prev;
  }
}

// FIX wont delete last node
void deleteWords(Word* arg){
  
  Word* p = arg;
  Word* tmp;
  while (p != NULL){
    if (p->string != NULL) free(p->string);
    tmp = p;
    p = p->next;
    free(tmp);
  }
}

// ADD need to iterate over and free all nodes
void cleanUp(Node* current){
  
  Node* p = current;
  Node* tmp;
  while (p != NULL){
    deleteWords(p->arg_list);
    if (p->command != NULL) free(p->command);
    if (p->in_file != NULL) free(p->in_file);
    if (p->outfile != NULL) free(p->outfile);
    if (p->err_file != NULL) free(p->err_file);
    tmp = p;
    p = p->prev;
    free(tmp);
  }
}
