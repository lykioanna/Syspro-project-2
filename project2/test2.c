#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[]){
  char* buffer=NULL;
  int ch=0, lines=0, ret, i;
  size_t size= 512;

  DIR *d;
  struct dirent *dir;

  FILE *fp = fopen(argv[1], "r");
      if (fp == NULL){
          fprintf(stderr,"Error opening file\n");
          exit(-1);
      }

  buffer= malloc(sizeof(char) *size);


  ret = getline(&buffer, &size, fp);
  while(ret>-1) {
    if (ret<0)                            //if line ended
      break;

    buffer[ret-1]= '\0';
    printf("LINE: %s\n", buffer);

    char* doc= buffer;
    d = opendir(doc);
    if (d) {
      printf("%s\n",buffer );

      while ((dir = readdir(d)) != NULL) {
        printf("TXT NAME: %s\n", dir->d_name);
      }
      closedir(d);
    }

    ret= getline(&buffer, &size, fp);     //takes whole line
    }

  fclose(fp);
}
