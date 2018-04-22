#ifndef __TRIE__
#define __TRIE__

typedef struct ListNode{
  int id;
  int freq;
  struct ListNode *next;
}ListNode;


typedef struct TrieNode{
  char c;
  struct TrieNode *child;
  struct TrieNode *next;
  struct ListNode *postList;
}TrieNode;

ListNode *createListNode(int id );
void insertList(ListNode *root, int id);
void cleanList(ListNode * myList);


TrieNode *createTrieNode(char c);
// void insertTrieNode(TrieNode *node, char ch);
void insertTrie(TrieNode *root, char *word,int doc_id);
ListNode* searchTrie(TrieNode *root, char* key);
void cleanTrie(TrieNode **root);
void printAll(TrieNode *root,char* word, int i);
void printWord(TrieNode* root, char* word);

#endif
