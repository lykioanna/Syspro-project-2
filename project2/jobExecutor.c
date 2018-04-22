#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Arguments.h"
#include "trie.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#define BUFSIZE 1024


int main(int argc,char* argv[]){

  //Variables
  int i, w=0, numdocs=0, docsdiv=0, divflag=0;
  char *doc, *buffer;

  int nwrite;

  doc=buffer=NULL;
  read_args(&doc, &w, argc, argv);

  if (doc==NULL || w==0){
    fprintf(stderr, "Please, give parameters\n");
    exit(-1);
  }

  int in_father[w];
  int out_father[w];
  DIR *d;
  struct dirent *dir;
  d = opendir(doc);
  pid_t pids[w];
  int ch=0, lines=0, ret;
  size_t size= 512;

////////////////////////////////////////////////////////////////////////
  FILE *fp = fopen(doc, "r");
      if (fp == NULL){
          fprintf(stderr,"Error opening file\n");
          exit(-1);
      }

  while(!feof(fp)){
    ch = fgetc(fp);
    if(ch == '\n'){
      lines++;
    }
  }
  //printf("LINES %d\n",lines );

  buffer= malloc(sizeof(char) *size);     //just to read
  char **array;
  array = malloc(sizeof(char*)*lines);    //just to save the lines

  fseek(fp, 0, SEEK_SET); //go to the top of file

  for (i=0; i<lines; i++){
    ret = getline(&buffer, &size, fp);
    //while(ret>-1) {
      if (ret<0)                            //if line ended
        break;

      buffer[ret-1]= '\0' ;
      array[i] = malloc(sizeof(char )* strlen(buffer)+1);
      strcpy(array[i], buffer);
      //printf("Array's: %s \n",array[i]);

      // char* doc= buffer;
      // d = opendir(doc);
      // if (d) {
      //   printf("%s\n",buffer );
      //
      //   while ((dir = readdir(d)) != NULL) {
      //     printf("TXT NAME: %s\n", dir->d_name);
      //   }
      //   closedir(d);
      // }

      //ret= getline(&buffer, &size, fp);     //takes whole line
      //}
  }
  fclose(fp);
////////////////////////////////////////////////////////////////////////
numdocs= lines/w;     // posa paths tha parei o kathenas
if (lines%w != 0){
  docsdiv= lines%w;   //an perisseuei kapoio to pairnei o prwtos
}
////////////////////////////////////////////////////////////////////////


  char Input_parent[24];
  char Output_parent[24];

  /* Start CHILDREN. */
  for (i = 0; i < w; i++) {
    sprintf(Input_parent, "Input%d", i);
    sprintf(Output_parent,"Output%d",i);

    //make these pipes
    if(mkfifo(Input_parent,0666)==-1){
        perror("Fail to make named pipe\n");
        exit(1);}

    if(mkfifo(Output_parent,0666)==-1){
        perror("Fail to make named pipe\n");
        exit(1);}


    if ((pids[i] = fork()) < 0) {
      fprintf(stderr, "Wrong in fork\n");
      exit(0);

    } else if (pids[i] == 0){
////////////////////////////////////////////////////////////////////////
        //CHILD//
        char msgbuf[BUFSIZE];
        int in,out,child_numdocs=numdocs,id=i, linestxt=0;
        if(id < docsdiv){
          child_numdocs++;
        }
        printf("i am child %d  with docs %d\n",id,child_numdocs);
        char** my_paths;
        my_paths = malloc(sizeof(char*)*child_numdocs);

        //open these pipes
        if((out=open(Input_parent,O_WRONLY))==-1){
          perror("Fail to open the named Output pipe\n");
          exit(1);
        }
        if((in=open(Output_parent,O_RDONLY|O_NONBLOCK))==-1){
            perror("Fail to open the named Input pipe\n");
            exit(1);
        }

        for(i = 0 ; i < child_numdocs ; i++){    //gia osa paths prepei na diavasei
          while (read ( in , msgbuf , BUFSIZE +1) <=0);
          printf("PATH :%s  %d \n",msgbuf,id );
          my_paths[i] = malloc(sizeof(char)*strlen(msgbuf));
          strcpy(my_paths[i],msgbuf);
          write(out,"NextPath",strlen("NextPath")+1);

          //open .txt files
          char *txtname, *token;
          int j, tmpr;
          //txtname= malloc(sizeof(char)*size);

          char* doctxt;
          strcpy(doctxt,my_paths[i]);
          d = opendir(doctxt);
          if (d) {
            //printf("%s\n",buffer );

            while ((dir = readdir(d)) != NULL) {
              if (strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")){
                printf("TXT NAME: %s\n", dir->d_name);
                //////////////////////////////////
                FILE *tmpfile = fopen(dir->d_name, "r");
                  if (tmpfile == NULL){
                      fprintf(stderr,"Error opening file\n");
                      exit(-1);
                  }

                while(!feof(tmpfile)){
                  ch = fgetc(tmpfile);
                  if(ch == '\n'){
                    linestxt++;
                  }
                }//endofwhile(feof)

                for( j=0; j<linestxt; j++){
                  tmpr= getline(&buffer, &size, tmpfile);     //takes whole line
                  if (tmpr<0)                            //if line ended
                    break;

                  buffer[ret-1]= '\0';
                  token= strtok(buffer, " ");
                  while (buffer!= "\n"){
                    printf("STRTOKED %s\n",token );
                    token= strtok(NULL," ");
                  }
                }

              }
            }//fclose(tmpfile);
            closedir(d);
          }
        }

        while(1){
          if (  read ( in , msgbuf , BUFSIZE +1) > 0) {
            if(!strcmp(msgbuf,"exit"))break;
          }
        }

        for(i=0 ; i < child_numdocs ; i++){
          free(my_paths[i]);
        }
        free(my_paths);
        printf("child finished\n" );
        return 0;
      }
////////////////////////////////////////////////////////////////////////


      //FATHER
      //open the named pipes for each child
      if((in_father[i]=open(Input_parent,O_RDONLY|O_NONBLOCK))==-1){
          perror("Fail to open the named pipe_in\n");
          exit(1);
      }
      if((out_father[i]=open(Output_parent,O_WRONLY))==-1){
          perror("Fail to open the named pipe_out\n");
          exit(1);
      }

  }//end of for

  int counter=0,counter_rec=0,tmpchild=0,j;
  char tmp[100],msgbuf[BUFSIZE],strnumdocs[15];
  sprintf(strnumdocs,"%d", numdocs);

  for( i=0 ; i < lines-docsdiv ; i++){      //isarithma paths
    write(out_father[tmpchild],array[i],strlen(array[i])+1);
    tmpchild++;
    if(tmpchild==w){
      tmpchild=0;
      while(counter_rec<w){                 //ola ta paidia apantoun oti diavasan ta paths
        for(j = 0 ; j < w ; j++){
          if(read(in_father[j],msgbuf,BUFSIZE)>0){
            if(!strcmp(msgbuf,"NextPath"))counter_rec++;
          }
        }
      }
      counter_rec = 0;
    } //end of if(tmpchild==w)
  }
  //moirase to upoloipo
  counter_rec = tmpchild = 0;
  if(docsdiv>0){
    for(i = lines-docsdiv ; i < lines ; i++){ //ta paths pou perisepsan
      write(out_father[tmpchild],array[i],strlen(array[i])+1);
      tmpchild++;
    }
    while(1){
      for(i = 0 ; i < docsdiv ; i++){        //kai edw apantoun ta paidia oti diavasan to upoloipa paths
        if(read(in_father[i],msgbuf,BUFSIZE)>0){
          if(!strcmp(msgbuf,"NextPath")){
            counter_rec++;
          }
        }
      }
      if(counter_rec == docsdiv)break;
    }
  }

  for(i = 0 ; i <w ; i++){
    if (( nwrite = write ( out_father[i] ,"exit" , strlen("exit")+1) ) == -1){
      perror ( " Error in Writing 2" ) ;
      exit (2) ;
    }
  }
  for( j= 0 ; j < lines ; j++){
    free(array[j]);
  }
  free(array);

////////////////////////////////////////////////////////////////////////

  /* Wait for children to exit. */
  int status;
  pid_t pid;
  for(i = 0  ; i < w ; i++){
    printf("Waiting: %d\n",waitpid(pids[i],&status,0));
  }

  for(i = 0 ; i < w ; i++){
    sprintf(Input_parent, "Input%d", i);
    sprintf(Output_parent,"Output%d",i);
    if (remove(Input_parent) != 0)
      printf("Unable to delete the file");
    if (remove(Output_parent) != 0)
      printf("Unable to delete the file");
  }
  free(doc);
  free(buffer);
}
