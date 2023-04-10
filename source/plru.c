/*


Tree based Pseudo-Least Recently Used (LRU) algorithm implementation.

plru.c

Author: Walid Ghandour
Date: March 2023

 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "plruTree.h"

int main (int argc, const char * argv[])  {

  // we pass in the first argument the name of the input file

  if (argc < 2) {
        fprintf(stderr, "usage: %s file: it takes one arg: name of the input file.\n", argv[0]);
        return EXIT_FAILURE; //1;
  }

  FILE *in_file  = fopen(argv[1], "r"); // read only

  // test for file not existing.
  if (! in_file ) 
  {  
    fprintf(stderr, "error: could not open textfile: %s\n", argv[1]);

    exit(-1); // return EXIT_FAILURE;
   }

 
  char line[500];
  const char *delim = "\t\n";
  char *token, *inTag, *inData;
  struct node* lruRootParse;
   
  while (fgets(line, sizeof(line), in_file)) {

    token = strtok(line,delim);
    if (token == NULL)
      break;

    if(! strcmp(token, "associativity" )){  // we provide the associativity to construct the tree
      if ( token != NULL){
	token = strtok(NULL,delim);
	lruRootParse = createTree(atoi(token));
      }
    }

    else if(!strcmp(token , "insertLine" )){// we insert a line in the tree, add a line to a given block in the cache, block index
      //is not handled yet.
      if ( token != NULL){
	inTag = strtok(NULL,delim);

	if ( inTag != NULL){
	  inData = strtok(NULL,delim);

	  insertLine(lruRootParse, atoi(inTag),  atoi(inData));   
	}

      }
      
    }

    else if(!strcmp(token , "accessLine" )){ 
     if ( token != NULL){
	inTag = strtok(NULL,delim);
	
	accessLine(lruRootParse, atoi(inTag));
     }
   
    }

    else if(!strcmp(token , "findLine" )){ 
      if ( token != NULL) {
	inTag = strtok(NULL,delim);

	findLine(lruRootParse, atoi(inTag));
      }
     
    }
    
    else if(!strcmp(token , "invalidateLineTag" )){ 
      if ( token != NULL) {
	inTag = strtok(NULL,delim);

	invalidateLineTag(lruRootParse, atoi(inTag));
      }
     
    }
    
    else if(!strcmp(token , "findPLRU" )){ 
	findPLRU(lruRootParse);
    }

    else if(!strcmp(token , "preorderTreeTraversal" )){
	preorderTreeTraversal(lruRootParse);
    }     

  }
 
   fclose(in_file);
}
