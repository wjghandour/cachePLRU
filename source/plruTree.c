/*


Tree based Pseudo-Least Recently Used (LRU) algorithm implementation.

plruTree.c

Author: Walid Ghandour
Date: March 2023

 */

#include "plruTree.h"


//THIS WILL CAUSE US A PROBLEM IF WE GENERATE MORE THAN ONE TREE SINCE IT IS USED BY ALL OF THEM
// THE SOLUTION IS TO RESET IT BEFORE THE generation of a new tree or when we create the root of a new tree
//another approach
// is to increment the index for each inserted node and then we sort the tree to have appropriate index order
int internalIndex[5]={0, 1, 3, 7, 31};
int leafIndex = 0;

// Only used when -DTEST or -DTEST2, must be initialized with setTestLog
static FILE *testlog_file;
#define log_printf(...) (testlog_file != NULL ? fprintf(testlog_file, __VA_ARGS__): 0)
#ifdef TEST
#define test_printf(...) log_printf(__VA_ARGS__)
#else
#define test_printf(...) ((void)0)
#endif
#ifdef TEST2
#define test2_printf(...) log_printf(__VA_ARGS__)
#else
#define test2_printf(...) ((void)0)
#endif

// Create a tree for a given cache associativity
struct node* generateTree(int level, int depth) { 

  if (level ==  depth) return NULL;

  if (level == 0){
    internalIndex[0]=0;
    internalIndex[1]=1;
    internalIndex[2]=3;
    internalIndex[3]=7;
    internalIndex[4]=31;
    leafIndex = 0;
  }

  
  struct node* newNode = malloc(sizeof(struct node));
  //  newNode->type = internal;
  newNode->PLRUStat = false;
  newNode->index = internalIndex[level]++;
  newNode->level = level;
  newNode->valid = false;
  newNode->leaf = false;
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->rootNode = false;
  newNode->rightChild = false;
  newNode->leftChild = false;
  newNode->data = 0;
  newNode->tag = 0;

  // separate it if from the above if, make it alone and not with initialization for index; this is intended to set the root flag
  if (level == 0) {
    newNode->rootNode = true;
    newNode->parent = NULL;
  }
  
  
  if (level + 1  < depth ) {
    newNode->left = generateTree(level + 1, depth); //you need to either verify that generate is not returning null or stop at depth -1
    newNode->left->parent=newNode;
    newNode->left->leftChild=true;
    
    newNode->right = generateTree(level + 1, depth);
    newNode->right->rightChild=true;
    newNode->right->parent=newNode;
  }
  
  if (level + 1 == depth){
    // here we create leaf nodes.
    newNode->left = malloc(sizeof(struct node));
    newNode->left->rootNode = false;
    newNode->left->PLRUStat = false;
    //    newNode->left->type=leaf;
    newNode->left->valid=false;
    newNode->left->leaf=true;
    newNode->left->leftChild=true;
    newNode->left->rightChild=false;
    newNode->left->data = 0;
    newNode->left->level = level + 1;
    newNode->left->index=leafIndex++;
    newNode->left->parent=newNode;
    newNode->left->left = NULL;
    newNode->left->right = NULL;
    
    newNode->right = malloc(sizeof(struct node));
    newNode->right->rootNode = false;
    newNode->right->PLRUStat = false;
    //    newNode->right->type=leaf;
    newNode->right->valid=false;
    newNode->right->leaf=true;
    newNode->right->leftChild=false;
    newNode->right->rightChild=true;
    newNode->right->data = 0;
    newNode->right->level = level + 1;
    newNode->right->index=leafIndex++;
    newNode->right->parent=newNode;
     newNode->right->left = NULL;
    newNode->right->right = NULL;
    
  }

  return newNode;
}


struct node* createTree(int associativity) {
  int depth = log2(associativity);
  return generateTree(0, depth);
}

uint32_t getIndex(struct node* node) {
    return node->index;
}

uint64_t getTag(struct node* node) {
    return node->tag;
}

int getData(struct node* node) {
    return node->data;
}

//find pseudo LRU block
int findPLRU(struct node* root){
  struct node* p = root;

  while (!p->leaf){
    if (p->PLRUStat){  // intially PLRUStat is set to false for all internal nodes.
      p = p->right;
    }
    else {
      p = p->left;
    }
  }
  test_printf("findPLRU:  PLRU leaf node:  index, level and tag: %d %d %ld\n" , p->index,   p->level, p->tag);
  return p->index; // we can also return here the node if needed
  
}

// promote block to PMRU
//this function is called every time
//we insert a new line
//or a line is accessed (hit)
void promotePMRU(struct node* inputNode){  //promotedNode

  struct node* promotedNode = inputNode;
  // promotePMRU takes as argument a leaf node
  //and promote it to MRU
  
  while(!(promotedNode->rootNode) && (promotedNode != NULL)){ // the first condition is enough, no need for the NULL comparison

    if (promotedNode->leftChild){//(node is left child)
      promotedNode->parent->PLRUStat = 1;
    }
    else {
      promotedNode->parent->PLRUStat = 0;
    }

    promotedNode = promotedNode->parent; 
  }

  return;
}

//Tree traversal
//we use this to verify the tree structure, and
// to print nodes: indices, levels
struct node* preorderTreeTraversal(struct node* root) {

  if (root == NULL) {
    return NULL;
  }

  if (root->leaf) {
    if (!root->valid) {
      test2_printf("Tree traversal: empty leaf node: level = %d,  index = %d.  \n", root->level, root->index);
    } else {
      test2_printf("Tree traversal: leaf node with valid data: level = %d,  index = %d, data = %d.  \n", root->level, root->index, root->data);
    }
  }
  else {

    test2_printf("Tree traversal: internal node: level = %d,  index = %d.  \n", root->level, root->index);

    struct node*  returnedNode;
    returnedNode = preorderTreeTraversal(root->left); 
    if (returnedNode != NULL){

      return returnedNode;
    }

    returnedNode = preorderTreeTraversal(root->right); 
    if (returnedNode != NULL){

      return returnedNode;
    }
 }

  return NULL;
}

// Preorder LRU traversal
// the intention is to try to find an empty line in the set
// if no empty line is found, null is returned
struct node* preorderLRUTraversal(struct node* root) {  
  if (root == NULL) return NULL;
  if (root->leaf) {
    if (!root->valid) {
      return root;
    }
  }
  else {
    struct node*  returnedNode;
    returnedNode = preorderLRUTraversal(root->left); 
    if (returnedNode != NULL){

      return returnedNode;
    }

    returnedNode = preorderLRUTraversal(root->right); 
    if (returnedNode != NULL){

      return returnedNode;
    }
 }

  return NULL;
}

//findLeaf
// this function returns the leaf node at a specific index
// it is used when this leaf is the PLRU and we need to replace it
struct node* findLeaf(struct node* root, int index) {
  if (root == NULL) return NULL;
  if (root->leaf) {
    if (root->index == index) {
      return root;
    } else {
      return NULL;
    }
  }
  struct node*  returnedNode;
  returnedNode = findLeaf(root->left, index);
  if (returnedNode != NULL){

    return returnedNode;
  }

  
  returnedNode = findLeaf(root->right, index);
  if (returnedNode != NULL){

    return returnedNode;
  }

  return NULL;
}

// This function is used to check if the line is in the cache
// it returns the found leaf node so that we can update its data
struct node*  findLine(struct node* root, uint64_t tag) {  //NOTE; THIS NEEDS TO BE UPDATED TO ACCESS USING SET INDEX AND TAG
  if (root == NULL) return NULL;
  if (root->leaf) {
    if (root->valid && root->tag == tag) {

      test_printf("findLine: line corresponding to tag  %ld found in the cache.\n", tag);

      return root;
    }
  }
  else {

    struct node*  returnedNode;
    returnedNode = findLine(root->left, tag);
    if (returnedNode != NULL){  
      return returnedNode;
    }

    returnedNode = findLine(root->right, tag);
    if (returnedNode != NULL){
      return returnedNode;
    }
  }
  
  return NULL;
}

//replace a line with another one
// we select a line at a given index
//we  update the data and tag fields, and set the valid bit
struct node* replaceLine(struct node* root, uint64_t tag,  int data, int index) {

  struct node* selectedLeaf = findLeaf(root, index);
  selectedLeaf->tag = tag;
  selectedLeaf->data = data;
  selectedLeaf->valid = true;

  test_printf("replaceLine: replace line at index %d, by a line  with tag %ld and data %d.\n", index, tag, data);

  return selectedLeaf;
}


// insert a line in a set
// we need to check first if the set is full
// if it is not full, find the first availabe leaf
// insert the new line
// promote the inserted line
bool insertLine(struct node* root, uint64_t tag, int data) {

  struct node* searchedNode = findLine(root, tag);

  if (searchedNode != NULL){
    searchedNode->data = data;
    test_printf("insertLine: line with tag %ld is already in the cache, we update it with the new data %d\n", tag, data);
    promotePMRU(searchedNode);
  }
  else {

    test_printf("insertLine: line with tag %ld is not in the cache\n", tag);
    
    struct node* tmp = preorderLRUTraversal(root);

    if (tmp == NULL){// the set is full

      test_printf("insertLine: tree is full, we need to evict PLRU to insert new line: tag %ld, data %d. \n", tag, data);
    
      int tmpIndex = findPLRU(root);
    
      struct node* insertedNode = replaceLine(root, tag, data, tmpIndex);
      promotePMRU(insertedNode);
    
    }
    else {

      test_printf("insertLine: there is empty place to insert the new line: tag %ld, data %d. \n", tag, data);
      
      tmp->valid = true;
      tmp->tag = tag;
      tmp->data = data;
      promotePMRU(tmp);
    
    }

    return false;
  }
  
  return true;
}


//invalidate function
bool invalidateLineIndex(struct node* root, int lineIndex) { 
  if (root == NULL) return false ;
  if (root->leaf) {
    if (root->index == lineIndex) {
      //test_printf("we have invalidated the line with index %d \n", lineIndex);
      root->valid = false;
      return true;
    }
  }
  else {

    bool status = false;
    
    status = invalidateLineIndex(root->left, lineIndex);
    if (status){
      return status;
    }
    status = invalidateLineIndex(root->right, lineIndex);
    if (status){
      return status;
    }
  }
  
  return false;
}

//it invalidates a given line in a set that corresponds to a specific tag
bool invalidateLineTag(struct node* root, uint64_t tag) { 
  if (root == NULL) return false ;
  if (root->leaf) {
    if (root->tag == tag) {
      test_printf("invalidateLineTag: we have invalidated the line with tag %ld.\n", tag);
      root->valid = false;
      return true;
    }
  }
  else {

    bool status = false;
    
    status = invalidateLineTag(root->left, tag);
    if (status){
      return status;
    }
    status = invalidateLineTag(root->right, tag);
    if (status){
      return status;
    }
  }
  return false;
}

void setTestLog(FILE *testlog) {
    testlog_file = testlog;
}
