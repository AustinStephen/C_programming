/* recursiveAdd.c
*  Author: Austin Stephen
*  Date: 3/22/2022
*
*  Recursively processes the contents of the direcory and any subdirectories.
*/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "add.c"

void recursiveAdd(char *basePath,char* archive)
{
    char path[2048];
    //char path2[2048];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
      return;

    while ((dp = readdir(dir)) != NULL){
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
      // update path
        strcat(path, "/");
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, dp->d_name);
        add(path,archive);
        recursiveAdd(path,archive);
      }
    }
    closedir(dir);
}
