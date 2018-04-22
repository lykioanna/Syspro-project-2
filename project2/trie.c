#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"

ListNode *createListNode(int id){
  ListNode *mynode;
  mynode= malloc(sizeof(ListNode));
  mynode->id= id;
  mynode->freq= 1;
  mynode->next= NULL;

  return mynode;
}

void insertList(ListNode *root, int id){
  ListNode *temp= root;
  if(root == NULL ){
    printf("ERROR IN insertList\n" );
    return;
  }
  if(temp->id == id){
    temp->freq++;
    return;
  }
  while(temp->next!=NULL){
    temp=temp->next;
    if(temp->id == id){
      temp->freq++;
      return;
    }
  }
  temp->next= createListNode(id);

}


void cleanList(ListNode * myList){
  ListNode* temp = myList;
  do{
    myList = temp;
    temp = myList->next;
    free(myList);
  }while(temp!=NULL);

}

////////////////////////////////////////////////

TrieNode *createTrieNode(char ch){
  TrieNode *pNode= malloc(sizeof(TrieNode));
  pNode->c= ch;
  pNode->child= NULL;
  pNode->next= NULL;
  pNode->postList= NULL;
  return pNode;
}





////sta children   katheta-
void insertTrie(TrieNode *root, char *word,int doc_id){
  TrieNode *tempNode= root;
  int i=0;
  for (i=0; i< strlen(word); i++){
    char temp= word[i];

    if (tempNode->child == NULL){               // an einai keno
      tempNode->child = createTrieNode(word[i]);
      tempNode= tempNode->child;
    }else{
      if(tempNode->child->c > word[i]){           //bainei apo ta aristera
        TrieNode* newNode = createTrieNode(word[i]);
        newNode->next = tempNode->child;
        tempNode->child = newNode;
        tempNode = newNode;
      }else if(tempNode->child->c < word[i]){     //bainei apo ta deksia
        TrieNode* tempListNode = tempNode->child;
        int flag=0;
        while(tempListNode->next != NULL){
          if( tempListNode->next->c == word[i]){
              tempNode = tempListNode->next;
              flag=1;
              break;
          }else if (tempListNode->next->c > word[i]){ //einai megalutero to next, prepei na to dimiourgisw edw
            TrieNode* newNode= createTrieNode(word[i]);
            newNode->next = tempListNode->next;
            tempListNode->next = newNode;
            tempNode = newNode;
            flag=1;
            break;
          }else { // <
            tempListNode = tempListNode->next;
          }
        }
        if (flag==0){
          TrieNode* newNode= createTrieNode(word[i]);
          tempListNode->next = newNode;
          tempNode = newNode;
        }
      }else{
        tempNode= tempNode->child;
      }
    }
  }
  if(tempNode-> postList==NULL)
    tempNode-> postList = createListNode(doc_id);
  else
    insertList(tempNode->postList,doc_id);
}

//* giati thelw na gyrnaei dieuthinsi oxi antigrafo listas
ListNode* searchTrie(TrieNode *root, char* word){
  TrieNode *tempNode= root->child;
  ListNode *temp;
  int flag=0, i;
  for (i=0; i<strlen(word); i++){

    while (tempNode != NULL){
        if (tempNode->c == word[i]){
          printf("%c",tempNode->c );
          if (tempNode->postList !=NULL && i==strlen(word)-1){
            printf("\nWord found\n" );
            return tempNode->postList;
            temp= tempNode->postList;
          }
          tempNode= tempNode->child;
          break;
        }
        tempNode = tempNode->next;
      }
      if (tempNode== NULL){
        break;

      }
    }
    printf("\nWord not found\n");
    return NULL;
  }


void cleanTrie(TrieNode **root){
  if ((*root)->child != NULL){
    cleanTrie(&(*root)->child);
  }
  if ((*root)->next != NULL) {
    cleanTrie(&(*root)->next);
  }
  if ((*root)->postList != NULL){
    cleanList((*root)->postList);
  }
  free(*root);
}


void printWord(TrieNode* root, char* word){
  ListNode* temp;
  int i=0;
  temp= searchTrie(root, word);
  while (temp!=NULL){
    i++;
    temp= temp->next;
  }
  printf("%s %d\n",word, i);
}

void printAll(TrieNode *node,char* word, int i){
  if(node==NULL)return;
  word[i]= node->c;
  if (node->postList != NULL){
    int count=0;
    ListNode* tempNode=node->postList;
    while (tempNode!=NULL){
      count++;
      tempNode= tempNode->next;
    }
    word[i+1]= '\0';
    printf("%s  %d\n",word,count);
  }
  if (node->child!=NULL){
    printAll(node->child, word,i+1);
  }
  if (node->next!=NULL){
    printAll(node->next, word,i);
  }
}
