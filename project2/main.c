#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "trie.h"
int main(){
    TrieNode *trie= createTrieNode('\0');
    insertTrie(trie, "geia1","path1",4);
    insertTrie(trie, "bye2","path2",2);
    insertTrie(trie, "lol3","path3",3);
    insertTrie(trie, "fucj4","path4",4);
    ListNode* temp=searchTrie(trie,"lol3");
    if(temp!=NULL){
      printf("found  %s\n",temp->path );
    }
    cleanTrie(&trie);
  return 0 ;
}
