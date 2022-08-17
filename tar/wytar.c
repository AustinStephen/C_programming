/* wytar.c
*  Author: Austin Stephen
*  Date: 3/22/2022
*
*  main function for wytar.
*/
#include "mainHeader.h"

int main(int argc, char **argv)
{
  int create = 0; // indicator
  int extractF = 0; // indicator
  int fileArg = 0; // track file location

    // Process first potential 4 arguements
    for (int i = 1; i < argc; i++){

      // Watching first 4 arguements for options
      if(i<=4){

      // checking the arguement for an option
      int arg_type = checkInput(argv[i]);

      // if it was given a bad option exit
      if(arg_type == 7){
        return 0;
      }

      // making sure -f option is followed by a file and processing
      if(arg_type == 3){
        if ( argc <= i+1 ){
          printf("Error: -f must be followed by a filename A \n");
          return 0;
        }
        else if(checkInput(argv[i + 1]) != 4){
          printf("Error: -f must be followed by a filename B \n");
          i += 1;
          return 0;
        }
        else{
          i += 1;
          fileArg = i;
        }
      }

      // add a file to the archive 
      if(arg_type == 4 && create ==1){
        
        // Adds the inital element to the archive 
        add(argv[i],argv[fileArg]);
        // recursively add the remaining elements
        recursiveAdd(argv[i],argv[fileArg]);

      }

      // setting create or extract archive
      if(arg_type == 1)
        create = 1;
      if(arg_type == 2)
        extractF = 1;
    }
  }

  // if both create and extract were set or neither 
  if(create ==1 && extractF == 1){
    printf("Error: Both -c and -x were set \n");
    return 0;
  }
  else if (create ==0 && extractF == 0){
    printf("Error: Neither -c and -x were set \n");
    return 0;
  }

  // if -f was not given 
  if(fileArg ==0){
    printf("A file must be given with the -f option \n");
    return 0;
  }
  // if extracting an archive
  if(extractF == 1){
   extract(argv[fileArg]);
  }

  // process potential arguements more than the first 4
  for (int i = 1; i < argc; i++){
    // Not looking at first 4 arguements 
    if( i > 4){

      // if creating archivec
      if(create == 1){
        // Adds the inital element to the archive 
        add(argv[i],argv[fileArg]);
        // recursively add the remaining elements
        recursiveAdd(argv[i],argv[fileArg]);
      }
    }
  }

  // add the 2 512 byte blocks to the end of the archive
  if(create == 1){
    char buffer[512];
    memset(buffer, 0, 512);

    // attempt to open archive
    FILE *arcP = fopen(argv[fileArg],"a");
    // If the archive can be opened
    if(arcP != NULL){
      // write ending twice
      if (fwrite(&buffer, sizeof(buffer),1,arcP) != 1){
        printf("Error: write to %s  failed", argv[fileArg]);
        return 1;
      }
      if (fwrite(&buffer, sizeof(buffer),1,arcP) != 1){
        printf("Error: write to %s  failed", argv[fileArg]);
        return 1;
      }
    }
  }
  return 0;
}
