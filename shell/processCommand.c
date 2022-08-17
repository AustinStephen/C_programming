/*
* processCommand.c
* Author: Austin Stephen
* Date: April 18, 2022
*
* COSC 3750, Homework 10
*
* Contains the functions for processing a command both in the 
* background and the foregroud. 
*
* WARNING uses fork.
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "processCommand.h"

void processC(Node* toProcess){

  // Get to the start of the list of commands
  Node* tmp = toProcess; 
  while(tmp !=NULL){
    toProcess = tmp; 
    // reset to track location in pipes
    toProcess->pipe = 0;
    tmp = tmp->prev;
  }

if(toProcess->command == NULL){
  return;
}

  // Process the commands one by one 
  while(toProcess != NULL){

    // Pipes
    // tracks if a pipe is in use for this process
    int pipeOut = 0;
    int pipeIn = 0;
    
    // Check if the output/input is redirected with no file specified,
    // this implies there is a pipe to the next command or from prev.
    if(toProcess->output == 1 && toProcess->outfile == NULL){
      pipeOut =1;
      //make it use the other tmp file for the next pipe
      toProcess->next->pipe = (toProcess->pipe + 1)%2;
    }
    if(toProcess->input == 1 && toProcess->in_file == NULL){
       pipeIn =1;
    }

  //printf("%s %d",toProcess->command, toProcess->operator );

    // splits here
    int pid = fork();
    
    // if fork is a success
    if (pid<0){
      printf("Process Failed\n");
      return;
    }

    // if the running the child process
    if (pid == 0){
      
      // create the arguement list 
      
      // count the arguements, intilized to 2 for the name of the
      // comamnd to run and null terminator.
      int count = 2;

      // To loop over the arguements with
      Word* args = toProcess->arg_list;
      Word* tmp = toProcess->arg_list;

      // iterate of the arguements to count them
      // tmp is now null and args points to the end
      while(tmp !=NULL){
        args = tmp;
        tmp = tmp->next;
        count++;
      }

      // create the arg list the size of the number of arguements
      char* arg_list_exec[count];

      // set the last element to NULL
      arg_list_exec[count-1] = NULL;
      count--;
      // set first element to the commands name
      arg_list_exec[0]= toProcess->command; 

      // create the arguement list back to front
      while(count > 1){
        arg_list_exec[count-1] = args->string;
        args = args-> prev;
        count--;
      }


      // Open and map any necessary files to stdin, stdout, stderr
      int fdI = 0;
      int fdO = 0;
      int fdE = 0;
      int pipe = 0;

      // input files
      if(toProcess->in_file != NULL){
        fdI = open(toProcess->in_file,O_RDONLY);

        // test if created successfully
        if(fdI == -1){
          printf("failed to open file");
          return;
        }
        else{
          // changing stdin to point to the new location
          int rtn = dup2(fdI,STDIN_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect input");
            return;
          }
        }
      }

      // output files
      if(toProcess->outfile != NULL){
        if(toProcess->append == 0){
          // overwrite
          fdO = open(toProcess->outfile, O_WRONLY | O_CREAT | O_TRUNC, 00600);
        }
        else{
          // append
          fdO = open(toProcess->outfile,O_WRONLY | O_CREAT | O_APPEND, 00600);
        }
        // test if created successfully
        if(fdO == -1){
          printf("failed to open/create file");
          return;

        }
        else{
          // changing stdout to point to the new location
          int rtn = dup2(fdO,STDOUT_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect output");
            return;
          }
        }
      }

      // diagnostic(err) files
      // // error to output
      if(toProcess->error == 1 && toProcess->operator == REDIR_ERR_OUT){
        // changing stderr to point to the new location
          int rtn = dup2(STDOUT_FILENO,STDERR_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect output");
            return;
          }
      }

      // other error redirects

      if(toProcess->err_file != NULL){

        // Check the properties to open the file with
        if(toProcess->append == 0){
        // overwrite
        fdE = open(toProcess->err_file, O_WRONLY | O_CREAT | O_TRUNC, 00600);
        }
        else{
        // append
        fdE = open(toProcess->err_file,O_WRONLY | O_CREAT | O_APPEND, 00600);
        }
        // test if created successfully
        if(fdE == -1) {
          printf("failed to open/create file");
          return;
        }
        else{
          // changing stderr to point to the new location
          int rtn = dup2(fdE,STDERR_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect output");
            return;
          }
        }
      }

      // close any files that were opened 
      int retrn; 
      
      if(toProcess->in_file != NULL){
        retrn = close(fdI);
        if (retrn == -1){
          printf("Failed to close file\n");
          return;
        }
      }
      if(toProcess->outfile != NULL){
        retrn = close(fdO);
        if (retrn == -1){
          printf("Failed to close file\n");
          return;
        }
      }
      if(toProcess->err_file != NULL){
        retrn = close(fdE);
        if (retrn == -1){
        printf("Failed to close file\n");
        return;
        }
      }

      // Pipes
      // Alternating writes to a tmp file, notice they are opposite 
      // for read an write so it reads and writes the the correct file

      // if input is reciving from a pipe, map output of the
      // pipe to the input of the process
      if(pipeIn == 1 && toProcess->pipe == 1){
        // attempt to open the tmp file
        pipe = open("zzz",O_RDONLY);

        // test if created successfully
        if(pipe == -1){
          printf("failed to open file");
          return;
        }
        else{
          // changing stdin to point to the new location
          int rtn = dup2(pipe,STDIN_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect input");
            return;
          }
        }
      }

      // if input is reciving from a pipe, map output of the
      // pipe to the input of the process
      if(pipeIn == 1 && toProcess->pipe == 0){
        // attempt to open the tmp file
        pipe = open("zzzz",O_RDONLY);

        // test if created successfully
        if(pipe == -1){
          printf("failed to open file");
          return;
        }
        else{
          // changing stdin to point to the new location
          int rtn = dup2(pipe,STDIN_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect input");
            return;
          }
        }
      }

      if(pipeIn == 1){
        retrn = close(pipe);
        if (retrn == -1){
          printf("Failed to close file\n");
          return;
        }
      }

      // if output is sending to the pipe, map output of the
      // process to the input of the pipe
      if(pipeOut== 1 && toProcess->pipe == 0){
        // overwrite
          pipe = open("zzz", O_WRONLY | O_CREAT | O_TRUNC, 00600);
        // test if created successfully
        if(pipe == -1){
          printf("failed to open/create file");
          return;

        }
        else{
          // changing stdout to point to the new location
          int rtn = dup2(pipe,STDOUT_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect output");
            return;
          }
        }
      }

      // if output is sending to the pipe, map output of the
      // process to the input of the pipe
      if(pipeOut== 1 && toProcess->pipe == 1){
        // overwrite
          pipe = open("zzzz", O_WRONLY | O_CREAT | O_TRUNC, 00600);
        // test if created successfully
        if(pipe == -1){
          printf("failed to open/create file");
          return;

        }
        else{
          // changing stdout to point to the new location
          int rtn = dup2(pipe,STDOUT_FILENO);
          // test dup2 worked
          if(rtn == -1){
            printf("failed to redirect output");
            return;
          }
        }
      }

      if(pipeOut== 1){
        retrn = close(pipe);
        if (retrn == -1){
          printf("Failed to close file\n");
          return;
        }
      }

      // run the command
      retrn = execvp(toProcess->command,arg_list_exec);

      // test the return value of the command
      if(retrn == -1){
        printf("Command: %s failed\n", toProcess->command);
        return;
      }

      return;
    }

    // if running the parent process
    else{
      // The waiting strategy depends on backgroud or foreground process
      // testing foreground vs background 

      // backgroud process
      if(toProcess->quiet == 1){

        // Wait for the child process to terminate silently 
        if (waitpid(pid, NULL, WNOHANG) < 0) {
          perror("Failed to clean up child process \n");
        }
      }
      // foreground process, block until finished
      else{
        // Wait for the child process to terminate
        if (waitpid(pid, NULL, 0) < 0) {
          perror("Failed to clean up child process \n");
          return;
        }
      }
    }  
    // next node
    toProcess = toProcess->next;  
  } 
}