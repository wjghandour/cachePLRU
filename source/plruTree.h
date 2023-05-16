/*


Tree based Pseudo-Least Recently Used (LRU) algorithm implementation.

plruTree.h

Author: Walid Ghandour
Date: March 2023

 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


struct node {
  //enum nodeType {internal=0, leaf=1} type; //  no nead for it since we have leaf , keep commented here and below in case
  // we use it later on
  bool rootNode; // true for root
  bool PLRUStat; // for internal nodes
  bool valid;  // for leaf
  bool leaf;
  bool rightChild;
  bool leftChild;
  int data; // for leaf
  uint32_t index;
  uint16_t level;
  uint64_t tag;
  struct node* left;
  struct node* right;
  struct node* parent;
};


struct node* generateTree(int level, int depth);

struct node* createTree(int associativity);

uint32_t getIndex(struct node* node);

uint64_t getTag(struct node* node);

int getData(struct node* node);

int findPLRU(struct node* root);

void promotePMRU(struct node* inputNode);

struct node* preorderTreeTraversal(struct node* root);

struct node* preorderLRUTraversal(struct node* root);

struct node* findLeaf(struct node* root, int index);

struct node*  findLine(struct node* root, uint64_t tag);

struct node* replaceLine(struct node* root, uint64_t tag,  int data, int index);

bool insertLine(struct node* root, uint64_t tag, int data);

bool invalidateLineIndex(struct node* root, int lineIndex);

bool invalidateLineTag(struct node* root, uint64_t tag);

void setTestLog(FILE* testlog);
