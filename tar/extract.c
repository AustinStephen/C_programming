/* extract.c
*  Author: Austin Stephen
*  Date: 3/22/2022
*
*  extracts an arhcive by creating the directories and files
*/
#include <stdlib.h>
#include <math.h>

void extract(char* arc){
  // information for the current archive element
  int file = 0; // is file indicator
  int link = 0; // is link indicator
  int dir =0; // is dir indicator
  int header = 1; // processing header indicator
  int size =0; // # of bytes of file content to read
  int i = 0; // tracks location in header
  int test =0;
  // intialize
  char name[100]; // name
  memset(name,0,100);
  char mode[8]; // permissions
  memset(mode,0,8);
  char chksum[8]; // checksum
  memset(chksum,0,8);
  char linkname[100]; // linkname
  memset(linkname,0,100);
  char sizeA[12]; // size
  memset(sizeA,0,8);

  // open the archive
  FILE *fileP = fopen(arc,"r");
  
  if( fileP == NULL){
    perror(arc);
    return;
  }
  else{
    // read file 1 character at a time and store information
    char c = fgetc(fileP);
    while (c != EOF){
      // if(test == 8|| test ==9)
        //printf("At char: %d : %c\n", i , c);
      // path
      if(i < 99)
        name[i] = c;
      //mode
      else if ( i > 99 && i < 108)
        mode[i-100] = c;
      //chksum
      else if (i > 149 && i < 156)
        chksum[i-150] = c;
      //typeflag
      else if ( i == 156){
        if ( c == 48)
          file = 1;
        else if (c == '2')
          link = 1;
        else if (c == '5'){
          dir = 1;
        }
      //   else 
      //     printf("Typeflag is %c and this is not recognized \n", c);
        }
      // linkname
      else if( i>156 && i < 257){
        linkname[i-157] = c;
      }
      // size
      else if (i > 123 && i < 136){
        sizeA[i-124] = c;
      }
      
      // test for end of header
      if ( i == 499){
        // if(name[0] == '\0')
        //   return;
        header =0;
        // convert to decimal
        int tmp = atoi(sizeA);
        int count = 0;
        while(tmp > 0 ){
          size += (tmp%10) * pow(8,count);
          count= count +1;
          tmp/=10;
        }
        // printf("Mode: %s \n", mode);
        // printf("Name: %s \n", name);
        // printf("chksum %s \n", chksum);
        // printf("linkname %s \n", linkname);
        // printf("typeflag %d,  %d,  %d\n",file,link,dir );
        // printf("size %d \ncat ", size);
      }
      // create file, dir or lnk
      if(header == 0){

        // create a file
        if( file == 1){
          //create a file
          FILE * P;
          P = fopen(name,"a");
          //check if the file could be created or opened
          if (P != NULL){
            // add to the file
            if(i != 499){ 
              fputc(c,P);
              size = size -1;
            }
            fclose(P);
          }
          else{
            printf("Error: Could not open: %s",name);
            return;
          }
        }
        // create a directory 
        else if( dir == 1){
          //printf("Making a dir %s ", name);
          int res = mkdir(name, 0700);
          if(res != 0){
            printf("Cannot make directory: %s \n", name);
            perror("mkdir");
            return;
          }

        }
        // create a link
        else if(link == 1){
          //printf("Making a link %s",name);
          int res = symlink(linkname,name);
          if (res == -1){
            printf("Error: Could not make link %s", linkname);
            return;
          }
        }

        // reset for the next header
        if (size == 0){
          // indicate where we are
          // printf("Start over");
          // if a file was created set its permissions
          if(file == 1){
            int tmp = strtol(mode,0,8);
            chmod(name,tmp);
          }
          // reset the counter, indicators, and buffers
          //printf("resetting to check next header");
          i = -1; // will get 1 added after exiting this checks
          header = 1;
          memset(name, 0, 100);
          memset(chksum, 0, 8);
          memset(mode, 0, 8);
          memset(sizeA,0,8);
          memset(linkname,0,8);
          file = 0;
          link = 0;
          dir =0;
          test= test +1;
        }
      }

      // incement i, get next char
      i += 1;
      c = fgetc(fileP);
    }
  // close archive
  fclose(fileP);
  }

}
