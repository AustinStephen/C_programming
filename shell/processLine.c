/*
* processLine.c
* Author: Austin Stephen
* Date: April 18, 2022
*
* COSC 3750, Homework 10
*
* gets a line from stdin and processes the tokens.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "processLine.h"


int processLine(){

  // The types of tokens
  // char *tokens[]={ "QUOTE_ERROR", "ERROR_CHAR", "SYSTEM_ERROR",
  //               "EOL", "REDIR_OUT", "REDIR_IN", "APPEND_OUT",
  //               "REDIR_ERR", "APPEND_ERR", "REDIR_ERR_OUT", "SEMICOLON",
  //               "PIPE", "AMP", "WORD" };

  // initalize the linked list
  Node* system = initilize();

  // indicate the prompt it ready
  printf("%s","$> ");
  
  // Buffer stores 1 line from stdin
  char* iBuffer;
  size_t ibuffSize = 2048;
  size_t sizeRead;
  iBuffer = (char *) calloc(ibuffSize, sizeof(char));

  // failed calloc for buffer    
  if (iBuffer == NULL)
    perror("Error: Could not allocate memory");

  // Read one line stdin
  sizeRead = getline (&iBuffer, &ibuffSize, stdin);

  // failed getline 
  if (sizeRead == -1) {
    return 1;
  }
  
  // tokenize the input line
  int rtn=parse_line(iBuffer);

  // iterating over tokens in the input string until the end of the line
  while(rtn !=  EOL){ 

    // deep copy
    char * word = malloc(strlen(lexeme) + 1); 
    strcpy(word, lexeme);
    
    switch(rtn) {
      
      // Word is either a command, arguement or file to redirect:
      // -- if a command is added system will point to the NEW node.
      // -- if arg is added it will be added to the current arg list
      // -- if file its name is stored in the correct parameter

      case WORD:
        // Testing if the next word is a command arguement, or file

        // Test if it is the first word in the input
        if(system->command == NULL){
          system->command = word;
        }
        // Test if the word was preceeded by a pipe
        else if(system->pipe == 1)
        {
          addNode(system,word);
          system->pipe = 0;
          system = system->next;
          system->input =1;
        }
        // Test if the word was preceeded by a semicolon
        else if(system->semicolon == 1)
        {
          addNode(system,word);
          system->semicolon = 0;
          system = system->next;
        }
        // If the operator is an input redirect then assign the in_file 
        // to the word and reset the operator.
        else if(system->operator == REDIR_IN )
        {
          system->in_file = word;
          system->operator = 0;
        }
        // If the operator is an output redirect then assign the out_file 
        // to the word and reset the operator.
        else if(system->operator == REDIR_OUT || system->operator == APPEND_OUT) 
        {
          system->outfile = word;
          system->operator = 0;
        }
        // If the operator is an error redirect then assign the out_file 
        // to the word and reset the operator.
        else if(system->operator == REDIR_ERR || system->operator == APPEND_ERR)
        {
          system->err_file = word;
          system->operator = 0;
        }
        // AMP must end a command arguement string, cannot be between arguements
        // Therefore, if it is seen followed by a word throw an error
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        else{
          addWord(system, word);
        }
        break;

      ////// Error tokens //////////

      // clear and exit but keep reading new lines
      case ERROR_CHAR:
        printf("error character: %d\n",rtn);
        free(word);
        free(iBuffer);
        cleanUp(system);
        return 0;
      // perror and terminate the whole program
      case SYSTEM_ERROR:
        perror("system error:");
        return 1;
      // clear and exit but keep reading new lines
      case QUOTE_ERROR:
        printf("quote error: %d\n",rtn);
        free(word);
        free(iBuffer);
        cleanUp(system);
        return 0;

      ////// Special tokenss ///////

      // Test if allowed, then set if so
      case REDIR_OUT:
        // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another op is directing out and this operator is directing out
        else if (system -> output == 1){
          printf("Ambigious Output redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // otherwise set the operator
        else{
          system->operator = REDIR_OUT;
          system->output = 1;
        }
        break;


      // Test if allowed, then set if so
      case REDIR_IN:
        // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // something else is directing input in 
        else if (system -> input == 1){
          printf("Ambigious Input redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // if there is a duplicate append out
        else if (system-> operator == REDIR_IN ){
          printf("Ambigious Input redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // otherwise set the operator
        else{
          system->operator = REDIR_IN;
          system->input = 1;
        }
        break;


      // Test if allowed, then set if so
      case APPEND_OUT:
        // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another op is directing out and this operator is directing out
        else if (system -> output == 1){
          printf("Ambigious Output redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
       // otherwise set the operator
        else{
          system->operator = APPEND_OUT;
          system->output = 1; 

          // Inidcates the file stored in out_file shoud be appended to
          system->append = 1;
        }
        break;


      // Test if allowed, then set if so
      case APPEND_ERR:
        // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another error op is directing out and since 
        // this operator is directing out. Error
        else if (system -> error == 1){
          printf("Ambigious Error redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command  HERE\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
       // otherwise set the operator
        else{
          system->operator = APPEND_ERR;
          system->error = 1; 

          // Inidcates the file stored in out_file shoud be appended to
          system->append = 1;
        }
        break;
      

      // Test if allowed, then set if so
      case REDIR_ERR_OUT:
        // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another error op is directing out and since
        // this operator is directing out
        else if (system -> error == 1){
          printf("Ambigious Error redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
       // otherwise set the operator
        else{
          system->operator = REDIR_ERR_OUT;
          printf("%d",STDOUT_FILENO );
          system->error = 1; 
        }
        break;


      // Test if allowed, then set if so
      case REDIR_ERR:
                // if there is no command
        if(system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another op is directing out and this operator is directing out
        else if (system -> error == 1){
          printf("Ambigious Error redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // A semicolon was the last operator because the new comand has
        // not been encounted yet
        else if (system -> operator == SEMICOLON){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // AMP must end a command arguement string, cannot be redirected
        else if (system -> operator == AMP){
          printf("& must end a command\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
       // otherwise set the operator
        else{
          system->operator = APPEND_ERR;
          system->error = 1;
        }
        break;
      

      // Mark next word is a command, set the operator
      case SEMICOLON:
        system->semicolon = 1;
        system->operator = SEMICOLON;
        break;


      // Test it is valid, mark next word is a command, set the operator
      case PIPE:
        system->pipe = 1;
        // check if the current operator is a pipe
        if ( system->operator == PIPE){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // check if the current operator is &
        else if (system-> operator == AMP){
          printf("& must end a command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // check if there is no command
        else if ( system->command == NULL){
          printf("Invalid Null Command \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        else if ( system->operator == SEMICOLON){
          printf("Unexpected token before | \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // Another op is directing out and this operator is directing out
        else if (system -> output == 1){
          printf("Ambigious Output redirection \n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        else{
        system->operator = PIPE;
        system->output = 1;
        }
        break;


      // Indicate which commands should be run quietly 
      case AMP:
        if(system->operator == SEMICOLON){
          printf("& without the accompanying commmand\n");
          free(word);
          free(iBuffer);
          cleanUp(system);
          return 0;
        }
        // mark current command to be quiet
        system->quiet = 1;

        // Checking if the input to this command was recived via a pipe
        // in that case the prior commands need run quietly as well.
        // Achived by back propagating over nodes until the operator is
        // not a pipe at each node the operator is assinged.

        // so we don't obviscate system and can keep processing the next commands
        Node *tmp = system -> prev;
        // if( tmp != NULL) tmp ->quiet = 1;
        
        // iterate over previous commands
        while(tmp != NULL && tmp->operator == PIPE){
          //printf("checking operator for command: %s \n", tmp->command);
          tmp->quiet = 1;
          tmp = tmp->prev;
        }

        // set the current operator 
        system->operator = AMP;
        break;

      default:
        printf("Problem");

    free(word);
    }
    rtn=parse_line(NULL);
  }

  // test if a redirect was the last tokens
  if(system->operator == REDIR_OUT && system->arg_list == NULL){
    printf("Missing Name for Redirect\n");
  }
  // test if appending redirect was the last tokens
  else if (system->operator == APPEND_OUT && system->arg_list == NULL){
    printf("Missing Name for Redirect\n");
  }
  // test if appending redirect was the last tokens
  else if (system->operator == REDIR_IN && system->arg_list == NULL){
    printf("Missing Name for Redirect\n");
  }
  // test if pipe was the last tokens
  else if (system->operator == PIPE && system->next == NULL){
    printf("Invalid Null Command\n");
  }
  // test if AMP was the last token and there was not command
  else if (system->operator == AMP && system->command == NULL){
    printf("& with no accompanying command\n");
  }
  else{
  // Process the commands in the input
  //printStruct(system);
  processC(system);
  remove("zzz");
  remove("zzzz");
  }

  // Free the input string
  free(iBuffer);

  // free all of the memory in the main data structure
  cleanUp(system);

return 0;
}
