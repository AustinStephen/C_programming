/* add.c
*  Author: Austin Stephen
*  Date: 3/22/2022
*
*  creates a header,then adds it an its contents to the archive
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include "tarHeader.h"

void add(char* file, char* archive){
  struct posix_header header;
  int BLOCKSIZE = 512;

  // Check the archive is not in the tar
  if(strcmp (file, archive) == 0){
    printf("Error: cannot have the target in the archive: %s %s",file,archive);
    return;
  }

  // Attempt to open the file or link
  struct stat buffer;
  int success = lstat(file,&buffer);

  // If opened correctly
  if(success == 0){

    //Name
    memset(header.name, 0, 100);
    char buffer1[100] = "";
    // adds '\0'
    strcat( buffer1, file);
    sprintf(header.name, buffer1);

    //Mode
    memset(header.mode, 0, 8 );
    int mode = buffer.st_mode; 
    // bit mask
    mode = (mode & ~S_IFMT);
    sprintf(header.mode,"%07o",mode);

    //uid
    memset(header.uid, 0, 8 );
    sprintf(header.uid, "%07o", buffer.st_uid);

    //gid
    memset(header.gid, 0, 8 );
    sprintf(header.gid, "%07o", buffer.st_gid);

    //size
    memset(header.size, 0, 12 );
    if(S_ISLNK(buffer.st_mode))
      sprintf(header.size, "%011o",0);
    else if (S_ISDIR(buffer.st_mode))
      sprintf(header.size, "%011o",0);
    else
      sprintf(header.size, "%011lo", buffer.st_size);

    //mtime
    memset(header.mtime, 0, 12 );
    sprintf(header.mtime, "%011lo", buffer.st_mtime);
    
    
    //typeflag
    if(S_ISLNK(buffer.st_mode))
      header.typeflag = SYMTYPE;
    else if (S_ISDIR(buffer.st_mode))
      header.typeflag = DIRTYPE;
    else
      header.typeflag = REGTYPE;
    
    // linkname
    memset(header.linkname, 0, 100);
    if(S_ISLNK(buffer.st_mode)){
      char linkBuffer[1024];
      ssize_t linkLength;
      if ((linkLength = readlink(file, linkBuffer, sizeof linkBuffer)) == -1){
        perror("readlink");
        }
      else{
        // Make sure that the buffer is terminated as a string
        linkBuffer[linkLength] = '\0';
        if(linkLength < 100){
          sprintf(header.linkname, linkBuffer);
        }
      }
    }
    else
    {
      // nothing if not a link
      sprintf(header.linkname, " ");
    }
    
    // magic
    memset(header.magic,0,6);
    sprintf(header.magic, "%s", TMAGIC);
    
    // version
    sprintf(header.version, "%s", TVERSION);
    
    // uname
    memset(header.uname, 0, 32 );
    struct passwd *pwd;
    if( (pwd =getpwuid(buffer.st_uid)) != NULL)
      sprintf(header.uname,"%s",pwd->pw_name);
    else
      sprintf(header.uname,"%s"," ");
    
    // gname
    memset(header.gname, 0, 32 );
    struct group *grp;
    if ( (grp = getgrgid(buffer.st_gid)) != NULL)
      sprintf(header.gname,"%s",grp->gr_name);
    else
      sprintf(header.gname,"%s"," ");
    
    // devmajor
    memset(header.devmajor, 0, 8 );
    sprintf(header.devmajor,"%s","0000000\0");
    
    // devminor
    memset(header.devminor, 0, 8 );
    sprintf(header.devminor,"%s","0000000\0");
    
    // prefix
    memset(header.prefix, 0,155);
    sprintf(header.devminor,"%s","\0");

    // checksum
    memset(header.chksum, ' ', 8 );
    unsigned int total = 0;
    for(int i = 0; i < 155; i++){
      // name
      if (i <100)
        total += header.name[i];
      // mode
      if (i < 8)
        total += header.mode[i];
      // uid
      if ( i < 8)
        total += header.uid[i];
      // gid
      if (i < 8)
        total += header.gid[i];
      // size
      if (i < 12)
        total += header.size[i];
      // mtime
      if (i < 12)
        total += header.mtime[i];
      // chksum
      if (i < 8)
        total += header.chksum[i];
      // typeflag
      if (i == 1)
        total += header.typeflag;
      // linkanme
      if (i < 100)
        total += header.linkname[i];
      // magic
      if (i < 6)
        total += header.magic[i];
      // version
      if (i < 2)
        total += header.version[i];
      // uname
      if (i < 32)
        total += header.uname[i];
      // gname
      if (i < 32)
        total += header.gname[i];
      // devmajor
      if (i < 8)
        //total += header.devmajor;
      // devminor
      if (i < 8)
        total += header.devminor[i];
      // prefix
      if ( i < 155)
       total += header.prefix[i];
    }
    sprintf(header.chksum, "%07o", total);

    // Testing it was set correctly
    // printf("\n");
    // printf("name: %s \n", header.name);
    // printf("mode: %s \n", header.mode);
    // printf("uid: %s \n", header.uid);
    // printf("gid: %s \n",header.gid);
    // printf("size: %s \n",header.size);
    // printf("mtime: %s \n",header.mtime);
    // printf("check sum: %s \n",header.chksum);
    // printf("typeFlag: %d \n", header.typeflag);
    // printf("linkname: %s \n", header.linkname);
    // printf("magic: %s \n", header.magic);
    // printf("version: %s \n", header.version);
    // printf("uname: %s \n", header.uname);
    // printf("gname: %s \n", header.gname);
    // printf("devmajor: %s \n", header.devmajor);
    // printf("devminor: %s \n", header.devminor);
    // printf("prefix: %s \n",header.prefix);

    //Write to archive
    FILE *arcP = fopen(archive,"a");

    // If the archive can be opened
    if(arcP != NULL){
      // write header
      if( fwrite(&header, sizeof(header),1,arcP) != 1){
        printf("Error: write to %s  failed", archive);
        return;
      }

      // write file contents if not a link or dir
      if(! S_ISLNK(buffer.st_mode)){
        FILE *fileP = fopen(file,"r");

        // open file
        if( fileP == NULL){
        perror(file);
        return;
        }
        else{
          // write file contents to archive 
          char buffer[BLOCKSIZE];
          size_t bytes;
          memset(buffer, 0,BLOCKSIZE);
          while (0 < (bytes = fread(buffer, 1, BLOCKSIZE, fileP))){
            fwrite(buffer, 1, bytes, arcP); // change bytes for a 512
            memset(buffer, 0,BLOCKSIZE); // reset to zero pad
          }
        // close file 
        fclose(fileP);
        }
      }
    //close archive
    fclose(arcP);  
    }
    else{
      printf("Error: cannot create archive %s", archive);
      return;
  }
}
else{
printf("Error: cannot open file, %s to read", file);
return;
}
}
