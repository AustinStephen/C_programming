/*
* wycat.c
* Author: Austin Stephen
* Date: Feb 22, 2022
*
* Cosc 3750, Homework 04
*
* This is a simple version of the utility designed to direct the 
* contents of a file or standard in to standard out.
*
*/

#include<stdio.h>
#include<string.h>

int main (int argc, char **argv)
{

int const BUFFER_SIZE = 4096;
int readSize = 0;
int writeSize = 0;


  // No arguements
  if (argc == 1)
  {
  char buffer[4096] = "";
  readSize = BUFFER_SIZE; 

    // loop until all input is handled 
    while(readSize == BUFFER_SIZE)
    {
    // read
    readSize = fread(buffer,1,BUFFER_SIZE,stdin);

    //write
    writeSize = fwrite(buffer,readSize,1,stdout); 
      
      // Check write suceeded
      if((writeSize * readSize) != readSize){
      perror("write error");
      }  
    }
  }

  // 1 or more arguements
  for (int i=1; i < argc ; i++)
  {
  char buffer[4096] = "";
  
    // "-" arguement
    if ( strcmp(argv[i],"-") == 0 )
    {
    readSize = BUFFER_SIZE; 

      // loop until all input is handled 
      while(readSize == BUFFER_SIZE)
      {
      // read
      readSize = fread(buffer,1,BUFFER_SIZE,stdin);

      //write
      writeSize = fwrite(buffer,readSize,1,stdout); 
      
        // Check write suceeded
        if((writeSize * readSize) != readSize){
        perror("write error");
        }

      }
    }  

    // All other arguements
    else
    {
    char buffer[4096] = "";

    // open file  
    char *file = argv[i];
    FILE *infile = fopen(file,"r");

	// If the file cannot be found
      if(infile == NULL)
      {
      perror(file);
      }
      // File is found
      else
      {
      readSize = BUFFER_SIZE; 

        // loop until all input is handled 
        while(readSize == BUFFER_SIZE)
        {
        // read
        readSize = fread(buffer,1,BUFFER_SIZE,infile);

        //write
        writeSize = fwrite(buffer,readSize,1,stdout); 
      
          // Check write suceeded
          if((writeSize * readSize) != readSize){
          perror("write error");
          fclose(infile);

          }
        }
	    // Whole file was not read
        if ( ferror(infile) )
	    {
        perror(file);
        fclose(infile);

        }
        // Whole file was read
        else
        {
        fclose(infile);
        }
	  }
    }
  }
}