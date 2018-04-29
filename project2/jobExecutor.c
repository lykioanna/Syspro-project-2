#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
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
  pid_t pids[w];
  int ch=0, lines=0, ret;
  size_t size= 1024;

////////////////////////////////////////////////////////////////////////////////
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
  buffer= malloc(sizeof(char) *size);     //just to read
  char **array;
  array = malloc(sizeof(char*)*lines);    //just to save the lines

  fseek(fp, 0, SEEK_SET);                 //go to the top of file

  for (i=0; i<lines; i++){
    ret = getline(&buffer, &size, fp);
      if (ret<0)                          //if line ended
        break;

      buffer[ret-1]= '\0' ;
      array[i] = malloc(sizeof(char )* strlen(buffer)+1);
      strcpy(array[i], buffer);

  }
  fclose(fp);
////////////////////////////////////////////////////////////////////////////////
numdocs= lines/w;                         // posa paths tha parei o kathenas
if (lines%w != 0){
  docsdiv= lines%w;                       //an perisseuoun
}
////////////////////////////////////////////////////////////////////////////////

if(numdocs == 0){
    printf("Some kids does not take paths (num of kids:)%d\n",(w-lines) );
    w = lines;
}

  char Input_parent[24];
  char Output_parent[24];

// Start CHILDREN. //
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
////////////////////////////////////////////////////////////////////////////////

//CHILD//
        char msgbuf[BUFSIZE];
        int in,out,child_numdocs=numdocs,id=i, linestxt=0;
        int cwords=0, clines=0,cbyte=0;
        if(id < docsdiv){
          child_numdocs++;
        }
        if(child_numdocs==0)return 0;
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


        TrieNode *trie= createTrieNode('\0');

        for(i = 0 ; i < child_numdocs ; i++){    //gia osa paths prepei na diavasei
          while (read ( in , msgbuf , BUFSIZE +1) <=0);
          my_paths[i] = malloc(sizeof(char)*strlen(msgbuf)+1);
          strcpy(my_paths[i],msgbuf);
          write(out,"NextPath",strlen("NextPath")+1);

          //open .txt files
          char* doc= my_paths[i];
          char *txtname;
          txtname= malloc(sizeof(char)*size+1);
          d = opendir(doc);
          if (d) {

            while ((dir = readdir(d)) != NULL) {
              if (strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")){
                strcpy(txtname, dir->d_name);

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
                    clines++;
                  }
                }
                //GIA KATHE ARXEIAKI//

                char **arraytxt, *token;
                int ret2, k;
                arraytxt = malloc(sizeof(char*)*linestxt);    //just to save the lines

                fseek(fp2, 0, SEEK_SET);                      //go to the top of file
                for(k=0; k<linestxt; k++){
                  ret2 = getline(&buffer, &size, fp2);
                    if (ret2<0)                               //if line ended
                      break;

                    buffer[ret2-1]= '\0' ;
                    arraytxt[k] = malloc(sizeof(char )* strlen(buffer)+1);
                    strcpy(arraytxt[k], buffer);

                    token= strtok(arraytxt[k]," ");
                    while (token != NULL){
                      insertTrie(trie, token,pathtxt, (k+1));
                      cwords++;
                      cbyte+=strlen(token);
                      token= strtok(NULL," ");
                    }
                    free(arraytxt[k]);
                }
                fclose(fp2);
                free(arraytxt);
                free(pathtxt);
              }//END OF IF . .
            }//END OF WHILE
            closedir(d);
          }
        }


////////////////////////////////////////////////////////////////////////////////
        char *logtxt = malloc(sizeof(char)*strlen("./Log/Worker_.txt")+10);
        sprintf(logtxt,"./Log/Worker_%d.txt",id);
        FILE *f3 = fopen(logtxt, "w");
        if (f3 == NULL){
          fprintf(stderr,"Error opening file f3\n");
          exit(-1);
        }
////////////////////////////////////////////////////////////////////////////////
        char* tempWord;
        char* token2, *words;
        char strsend[128];
        time_t t;
        time(&t);
        int ret3,k;
        words= malloc(sizeof(char)*size);
        token2= malloc(sizeof(char)*size);

        ListNode *result;
        while(1){
          if (  read ( in, msgbuf, BUFSIZE +1) > 0){
            tempWord = strtok(msgbuf," ");

            if(!strcmp(tempWord,"/exit")){
              break;
          }else if(!strcmp(tempWord,"/search")){
              FILE * fp2;
              tempWord = strtok(NULL, "");
              strcpy(words, tempWord);
              token2=strtok(words," ");
              while (token2!=NULL) {
                result= searchTrie(trie, token2);
                while(result!=NULL){
                    fprintf(f3, "%s : /search : %s : %s\n",ctime(&t), token2, result->path);
                    fp2 = fopen(result->path,"r");
                    for(k=1; k<=result->id; k++){
                      ret3 = getline(&buffer, &size, fp2);
                      if (ret3<0) break;
                      if(k == result->id){
                        printf("Word: %s Line: %d Path: %s Whole line: %s\n",token2,result->id,result->path,buffer);
                        break;
                      }
                    }
                    fclose(fp2);
                    result= result->next;
                }
                token2= strtok(NULL," ");
              }
              write(out,"SearchOk",strlen("SearchOk")+1);
            }else if(!strcmp(tempWord,"/wc")){
              //printf("Cwords %d Lines %d\n",cwords,clines);
              sprintf(strsend,"%d-%d-%d\n", cwords, clines,cbyte);
              write(out,strsend,strlen(strsend)+1);


            }
          }
        }

        for(i=0 ; i < child_numdocs ; i++){
          free(my_paths[i]);
        }
        cleanTrie(&trie);
        free(words);
        free(token2);
        free(logtxt);
        free(my_paths);
        fclose(f3);
        //printf("child finished\n" );
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
      for(i = 0 ; i < docsdiv ; i++){        //apantoun ta paidia
        if(read(in_father[i],msgbuf,BUFSIZE)>0){
          if(!strcmp(msgbuf,"NextPath")){
            counter_rec++;
          }
        }
      }
      if(counter_rec == docsdiv)break;
    }
  }

  for( j= 0 ; j < lines ; j++){
    free(array[j]);
  }
  free(array);

////////////////////////////////////////////////////////////////////////////////
  fp = stdin;
  char* tempCommand,*tempWord;
  char* test, *token3;
  int Totalwords=0, Totallines=0,Totalbytes=0;
  while (1){
    ret = getline(&buffer, &size, fp);      //reading from stdin
    if (ret<0)break;
    if(buffer==NULL){
        printf("wrong in input\n" );
        continue ;
    }
    buffer[ret-1]= '\0';
    tempCommand = malloc(sizeof(char)*strlen(buffer)+1);
    strcpy(tempCommand,buffer);
    tempWord = strtok(tempCommand," ");

    if(!strcmp(tempWord,"/search")){
      for( i=0; i<w; i++){
        if ((nwrite = write ( out_father[i] ,buffer , strlen(buffer)+1) ) == -1){
            perror ("Error in Writing 6");
            exit (2) ;
        }
      }
      counter_rec= 0 ;
      while(1){
          for(i = 0 ; i < w ; i++){        //kai edw apantoun ta paidia oti diavasan to upoloipa paths
              if(read(in_father[i],msgbuf,BUFSIZE)>0){
                  if(!strcmp(msgbuf,"SearchOk")){
                      counter_rec++;
                  }
              }
          }
          if(counter_rec == w)break;
      }

    }else if(!strcmp(buffer,"/wc")){
      for(i=0 ; i<w ; i++){
        if (( nwrite = write ( out_father[i] ,buffer , strlen(buffer)+1) ) == -1){
            perror ("Error in Writing 7");
            exit (2);
        }
      }
      counter_rec= 0 ;
      while (1) {
        for(i = 0 ; i < w ; i++){
          if(read(in_father[i],msgbuf,BUFSIZE)>0){
            counter_rec++;
            token3 = strtok(msgbuf,"-");
            Totallines += atoi(token3);
            token3 = strtok(NULL,"-");
            Totalwords += atoi(token3);
            token3 = strtok(NULL,"-");
            Totalbytes += atoi(token3);
          }
        }
        if(counter_rec == w)break;
      }
      printf("Total lines: %d words: %d and Bytes: %d\n",Totallines,Totalwords,Totalbytes );
      Totallines = Totalwords = Totalbytes = 0 ;
    }else if(!strcmp(buffer,"/exit")){
        free(tempCommand);
        for(i=0 ; i<w ; i++){
            if (( nwrite = write ( out_father[i] ,"/exit" , strlen("/exit")+1) ) == -1){
                perror ("Error in Writing 5");
                exit (2) ;
            }
        }
        break;
    }else{
        printf("Wrong Command\n" );
        free(tempCommand);
        continue ;
    }
    free(tempCommand);
  }

  int status;
  // Wait for children to exit. //
  for(i = 0  ; i < w ; i++){
      printf("Finished: %d\n",waitpid(pids[i],&status,0));
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
  return 0;

}
