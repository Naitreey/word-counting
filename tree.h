/*-----------------tree.h--------------*/
#ifndef _TREE_H_
#define _TREE_H_

#include <stdbool.h>

/*data structure:
 * basic binary search tree (unbalanced)
 *data object components:
 * Trnode: constituting element of a tree
    	   1. item
	   2. pointer to left subtree/subnode
	   3. pointer to right subtree/subnode
 * Item: hold the informations about a word being counted
           1. pointer to word (because the length of a word is variable, we store pointer-to-word to make sure the size of item is fixed.)
           2. word count
 * Tree: a structure keep track of the root of tree and its size  	   
 	   1. pointer to root node
   	   2. node count
 */
struct word_data {
	char *pword;
	int count;
};
typedef struct word_data Item;

struct node {
	Item item;
	struct node *left;
	struct node *right;
};
typedef struct node Trnode;

struct tree {
	Trnode *root;
	int size;
};
typedef struct tree Tree;

void InitializeTree(Tree *ptree);
bool TreeIsEmpty(const Tree *ptree);
bool TreeIsFull(const Tree *ptree);
int TreeItemCount(const Tree *ptree);
Item MakeItem(const char *buffer);
bool AddItem(const Item *pitem, Tree *ptree);
bool InTree(const Item *pitem, const Tree *ptree);
bool DeleteItem(Item *pitem, Tree *ptree);
void Traverse(Tree *ptree, void (*pfunc)(Item item));
void DeleteAll(Tree *ptree);

#endif
