/* wyls.c
* Author:Austin Stephen
* Date: March 2, 2022
*
* Cosc 3750, Homework 5
*
* My psudocode for homework 5 making wyls utility.
*
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>
#include <stdlib.h>
#include <regex.h>


// Helper function that gets and prints formated information about a file
// humanReadable indicates how to print filesize
// IdValues indicates if it will print the user and group ID or name
int printStandard(char *file, int humanReadable, int IdValues)
{
   // opening the file
   struct stat buffer;
   int success = stat(file,&buffer);
   int link = lstat(file,&buffer);
   if( (success == 0) & (link == 0) )
   {
      //File type
      if(S_ISDIR(buffer.st_mode))
         printf("d");
      else if (S_ISLNK(buffer.st_mode))
         printf("l");
      else
         printf("-");

   // Password
      // User
      if( buffer.st_mode & S_IRUSR )
         printf("r");
      else
         printf("-");
      if( buffer.st_mode & S_IWUSR )
         printf("w");
      else
         printf("-");
      if( buffer.st_mode & S_IXUSR )
         printf("x");
      else
         printf("-");

      // Group
      if( buffer.st_mode & S_IRGRP )
         printf("r");
      else
         printf("-");
      if( buffer.st_mode & S_IWGRP )
         printf("w");
      else
         printf("-");
      if( buffer.st_mode & S_IXGRP )
         printf("x");
      else
         printf("-");

      // World
      if( buffer.st_mode & S_IROTH )
         printf("r");
      else
         printf("-");
      if( buffer.st_mode & S_IWOTH )
         printf("w");
      else
         printf("-");
      if( buffer.st_mode & S_IXOTH )
         printf("x ");
      else
         printf("- ");

      // Searches for username by id
      struct passwd *pwd;
      pwd = getpwuid(buffer.st_uid);

      // Checks input flag print ID or name
      if( IdValues == 0)
      printf("%8s ", pwd->pw_name);
      else if( IdValues == 1)
      printf(" %8d", buffer.st_uid);

      // Searches for groupname by id
      struct group *grp;
      grp = getgrgid(buffer.st_gid);

      // Checks input flag to print ID or name
      if( IdValues == 0)
      printf("%8s ", grp->gr_name);
      else if( IdValues == 1)
      printf("%6d ", buffer.st_gid);

      // Size
      int size = buffer.st_size;
      // normal
      if( humanReadable == 0)
      {
      printf("%8d ",size);
      }
      // humanReadable 
      else if (humanReadable == 1)
      {
         float sizeModified = (float)size;
         // Bytes
         if ( sizeModified < 1024){
            printf("%8g ",sizeModified);
         }
         // K
         else if ( sizeModified < 1048576 ){
            sizeModified = sizeModified/1024;
            sizeModified = floor(10 * sizeModified)/10; // sets precision
            printf("%7gK ",sizeModified);
         }
         // Mb
         else if (sizeModified < 1073741824){
            sizeModified = sizeModified/1048576;
            sizeModified = floor(10 * sizeModified)/10; // sets precision
            printf("%7gM ",sizeModified);
         }
         // Gb
         else{
            sizeModified = sizeModified/1073741824;
            sizeModified = floor(10 * sizeModified)/10; // sets precision
            printf("%7.1fG ",sizeModified);
         }
      }

      // Date
      char dateFile[20];

      // time tools
      time_t rawtime;
      struct tm *info;
      time( &rawtime );
      info = localtime( &rawtime );

      // getting base time
      int base= info->tm_year; // years since 1900
      base = 365*base; // days since 1900
      base = base + info->tm_yday; // adding days this year

      // getting file time
      struct tm *info2 = localtime( &(buffer.st_mtime) );
      int offset = info2->tm_year;
      offset = 365*offset; // days since 1900
      offset = offset + info2->tm_yday; // adding days this year

      // if newer than 180 days
      if(base - offset < 180){
      strftime(dateFile, 20, "%b %d %H:%M", localtime(&(buffer.st_mtime)));
      printf("%s ",dateFile);
      dateFile[0]=0;
      }
      else{
      strftime(dateFile, 20, "%b %d %Y", localtime(&(buffer.st_mtime)));
      printf("%s  ", dateFile);
      dateFile[0]=0;
      }
      // File path
      if (S_ISLNK(buffer.st_mode)){
         char linkBuffer[1024];
         ssize_t linkLength;
         if ((linkLength = readlink(file, linkBuffer, sizeof linkBuffer)) == -1){
           perror("readlink");
         }
         else{
           // Make sure that the buffer is terminated as a string
           linkBuffer[linkLength] = '\0';
           printf("%s -> %s\n", file, linkBuffer);
        }
      }
     else
         printf("%s\n",file);
   }

   else // Checks if the file can be opened
   {
    char option = file[0];
     if((option == '-') & (humanReadable == 0) & (IdValues ==0))
     {
        printf("%s",file);
        printf( " is not a valid option \n");
        return 1;
     }
    else
     {
        FILE *fileTest;
        fileTest = fopen(file,"r");
        if (fileTest == NULL)
        {
        perror(file);
        }
        else
        fclose(fileTest);
     }
   }
return 0;
}

// Helper function that handels comparisions for option interpretation

int match(const char *string, char *pattern)
{
   int status;
   regex_t mainVar;


    if (regcomp(&mainVar, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        return 0;
    }
    status = regexec(&mainVar, string, (size_t) 0, NULL, 0);
    regfree(&mainVar);
    if (status != 0) {
        return 0;
    }
    return 1;
}


int main(int argc, char **argv)
{
   struct dirent *de; // used for directories
   int i=1; // keeps track of place in the arguements
   int n =0; // keeps track if n option was used
   int h =0; // keeps track if h option was used
   int filePass=0; // keeps track if any files were passed as arguements
   int  compare_res =0;

   // The following conditionals check for options in the first 2 arguements
   // requires a lot of manual checking to behave correctly with replicates
   if(argc >1)
   {
      // checking for -n
      compare_res = match(argv[1],"^-[n]{1,}$");
      if( compare_res == 1)
      {
       n =1;
       i=2;
      }
      // checking for -h
      compare_res =  match(argv[1],"^-[h]{1,}$");
      if( compare_res  == 1)
      {
       h=1;
       i=2;
      }
     // checking for -hn
      compare_res = match(argv[1],"^-[h]{1,}[n]{1,}$");
      if( compare_res == 1)
       {
        h=1;
        n=1;
        i=2;
       }
       // checking for -nh
       compare_res =match(argv[1],"^-[n]{1,}[h]{1,}$");
       if( compare_res == 1)
       {
        h=1;
        n=1;
        i=2;
       }
   }

   // Still checking for options
   if(argc > 2)
    {
      // checking for -n
      compare_res = match(argv[2],"^-[n]{1,}$");
      if( (compare_res == 1) & (n==0) )
      {
       n =1;
       i=3;
      }
      // checking for -h
      compare_res =  match(argv[2],"^-[h]{1,}$");
      if( (compare_res  == 1) & (h ==0) )
      {
       h=1;
       i=3;
      }
      // checking for -hn
      compare_res = match(argv[2],"^-[h]{1,}[n]{1,}$");
      if( compare_res == 1)
       {
        h=1;
        n=1;
        i=3;
       }
       // checking for -nh
       compare_res =match(argv[2],"^-[n]{1,}[h]{1,}$");
       if( compare_res == 1)
       {
        h=1;
        n=1;
        i=3;
       }

    }

   // i can start at 1,2 or 3 depending what options were passed
   while ( i < argc ){
      filePass =1;
      int exit = 0;
      struct stat dirTest; // To test if directory
      int result = stat(argv[i],&dirTest); // To test if directory

      // Checks if the arguement is a directory
      if((result == 0) &&S_ISDIR(dirTest.st_mode)){
         DIR *dr = opendir(argv[i]);

         // reads all arguments in the directory
         while ((de = readdir(dr)) != NULL){
             if( (strcmp(de->d_name,".") !=0) & (strcmp(de->d_name,"..") != 0 )){
                char *relativePath;

               // Getting the last element of the argument
               int size = strlen(argv[i]);
               char *hold = argv[i];
               char lastElement =hold[size-1];

               // Checking if the user used a "/" to make a clean concatenation
               if ( lastElement != '/') {
               //building a relative path and adding /
               relativePath = malloc(strlen(argv[i])+strlen(de->d_name) + 1);
               strcpy(relativePath,argv[i]);
               strcat(relativePath,"/");
               strcat(relativePath,de->d_name);
               }

               // "/" was given by user building relative path
               else{
               relativePath = malloc(strlen(argv[i])+strlen(de->d_name));
               strcpy(relativePath,argv[i]);
               strcat(relativePath,de->d_name);
               }

               // call print with the full path
               exit = printStandard(relativePath,h,n);
               if ( exit == 1){
               return 0;
               }
            }
          }
        closedir(dr);
        }
        // entered if the file was not directory
        else{
         exit = printStandard(argv[i],h,n); //passes the next file
         if ( exit == 1)
         {
          return 0;
         }

        }
       i++;
      }

      if(filePass == 0) // enter if no file arguements were given
      {
      DIR *dr = opendir(".");

          while ((de = readdir(dr)) != NULL) // reads all arguments in the directory
          {
              if( (strcmp(de->d_name,".") !=0) & (strcmp(de->d_name,"..") != 0 ))
              {
                printStandard(de->d_name,h,n);
              }
          }
      closedir(dr);
      }
}
