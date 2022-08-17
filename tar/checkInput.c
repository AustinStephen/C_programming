/* checkInput.c
*  Author: Austin Stephen
*  Date: 3/22/2022
*
*  Helper function that returns 1 if the input is a -f, 2
*  if it is an -x, 3 if it is an -c, 4 if is a directory,
*  5 if it is a link, 6 if it should be treated as a file.
*  If the input is preceeded by a - but is not a supported
*  option an error is printed.
*
*
*/
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int checkInput(char* input){

  // check if the input is an option 
  if(input[0] == '-'){
    // check if option is valid 
    if(strlen(input) >= 2){
      if(input[1] == 'c')
        return 1;
      if(input[1] == 'x')
        return 2;
      if(input[1] == 'f')
        return 3;
      else{
        printf("Error: ");
        printf(input);
        printf(" is not a valid option \n");
        return 5;
      }
    }
  }else
  return 4;

return 5;
}
