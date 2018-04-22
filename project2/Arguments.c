#include <stdio.h>
#include <string.h>
#include <stdlib.h>




//Arguments Function
void read_args(char** docfile,int* workers,int argc,char* argv[]){
  int i;
  for(i=0 ; i< argc ; i++){
    if(!strcmp(argv[i],"-d")){
      *docfile= malloc(sizeof(char)*(strlen(argv[i+1])+1));
      strcpy(*docfile, argv[i+1]);
    }
    if(!strcmp(argv[i],"-w")){
      *workers=atoi(argv[i+1]);
    }
  }
}
