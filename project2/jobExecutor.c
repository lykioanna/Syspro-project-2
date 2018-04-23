#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "Arguments.h"
#include "trie.h"
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

        TrieNode *trie= createTrieNode('\0'); /////////////////////////////////////////////

        for(i = 0 ; i < child_numdocs ; i++){    //gia osa paths prepei na diavasei
          while (read ( in , msgbuf , BUFSIZE +1) <=0);
          printf("PATH :%s  %d \n",msgbuf,id );
          my_paths[i] = malloc(sizeof(char)*strlen(msgbuf));
          strcpy(my_paths[i],msgbuf);
          write(out,"NextPath",strlen("NextPath")+1);

          //open .txt files
          char* doc= my_paths[i];
          char *txtname;
          txtname= malloc(sizeof(char)*size+1);
          d = opendir(doc);
          if (d) {
            //printf("%s\n",buffer );

            while ((dir = readdir(d)) != NULL) {
              if (strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")){
                strcpy(txtname, dir->d_name);
                printf("TXT NAME: %s\n", txtname);

                /////edo prospathw na anoiksw ta arxeiakia/////
                char* pathtxt;
                pathtxt= malloc(sizeof(char)*size);
                strcpy(pathtxt, my_paths[i]);
                strcat(pathtxt,txtname);
                //EXW STO PATH TXT KATHE PATH GIA TA arxeiakia
                int linestxt=0, ch2;
                FILE *fp2 = fopen(pathtxt, "r");
                    if (fp2 == NULL){
                        fprintf(stderr,"Error opening file\n");
                        exit(-1);
                    }

                while(!feof(fp2)){
                  ch2 = fgetc(fp2);
                  if(ch2 == '\n'){
                    linestxt++;
                  }
                }
                printf("LINES %d\n",linestxt);

                char **arraytxt, *token;
                int ret2, k;
                arraytxt = malloc(sizeof(char*)*linestxt);    //just to save the lines

                fseek(fp2, 0, SEEK_SET); //go to the top of file
                for(k=0; k<linestxt; k++){
                  ret2 = getline(&buffer, &size, fp2);
                    if (ret2<0)                            //if line ended
                      break;

                    buffer[ret2-1]= '\0' ;
                    arraytxt[k] = malloc(sizeof(char )* strlen(buffer)+1);
                    strcpy(arraytxt[k], buffer);
                    //printf("DES EDO %s\n",arraytxt[k]);

                    token= strtok(arraytxt[k]," ");
                    //printf("TOKEN1: %s\n", token);
                    while (token != NULL){
                      //printf("TOKEN %s\n",token);
                      //INSERT TRIE KTL
                      token= strtok(NULL," ");
                    }
                }
              }//END OF IF . ..
            }//END OF WHILE
            closedir(d);
          }
        }


        while(1){
          if (  read ( in, msgbuf, BUFSIZE +1) > 0){
            if(!strcmp(msgbuf,"/exit")){
              printf("DES EDO\n" );
              break;
            }
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

  // for(i = 0 ; i <w ; i++){
  //   if (( nwrite = write ( out_father[i] ,"exit" , strlen("exit")+1) ) == -1){
  //     perror ( " Error in Writing 2" ) ;
  //     exit (2) ;
  //   }
  // }
  for( j= 0 ; j < lines ; j++){
    free(array[j]);
  }
  free(array);

////////////////////////////////////////////////////////////////////////
  fp = stdin;
  char* tempWord;
  while (!feof(fp)){
    ret = getline(&buffer, &size, fp);      //reading from stdin
    if (ret<0)
      break;
    buffer[ret-1]= '\0';

    tempWord = strtok(buffer," ");
    if(!strcmp(buffer,"/exit")){
      for(i=0 ; i<w ; i++){
        if (( nwrite = write ( out_father[i] ,"/exit" , strlen("/exit")+1) ) == -1){
          perror ("Error in Writing 5");
          exit (2) ;
        }
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
        break;
      }
    }else if(!strcmp(buffer,"/search")){

    }
    return 0;
  }



////////////////////////////////////////////////////////////////////////

  // /* Wait for children to exit. */
  // int status;
  // pid_t pid;
  // for(i = 0  ; i < w ; i++){
  //   printf("Waiting: %d\n",waitpid(pids[i],&status,0));
  // }
  //
  // for(i = 0 ; i < w ; i++){
  //   sprintf(Input_parent, "Input%d", i);
  //   sprintf(Output_parent,"Output%d",i);
  //   if (remove(Input_parent) != 0)
  //     printf("Unable to delete the file");
  //   if (remove(Output_parent) != 0)
  //     printf("Unable to delete the file");
  // }
  // free(doc);
  // free(buffer);
}
