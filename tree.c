/*-----------------tree.c--------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

/*the structure used by SeekItem().
  1. `parent' points to the parent node of the node containing the `item'
  2. `current` points to the node containing the `item'
 */
struct find {
	Trnode * parent;
	Trnode * current;
};
typedef struct find Find;

/*------------local function prototypes--------------*/

static Find SeekItem(const Item *pitem, const Tree *ptree);
static Trnode * MakeNode(const Item *pitem);
static void AddNode(Trnode *new_node, Trnode *root);
static bool ToLeft(const Item *pitem_node, const Item *pitem_root);
static bool ToRight(const Item *pitem_node, const Item *pitem_root);
static void DeleteNode(Trnode **ppnode);
static void InOrder(Trnode *pnode, void (*pfunc)(Item item));
static void DeleteAllNodes(Trnode * pnode);

/*------------definitions of tree package interface functions---------*/

void InitializeTree(Tree *ptree)
{
	ptree->root=NULL;
	ptree->size=0;
}

bool TreeIsEmpty(const Tree *ptree)
{
	return ptree->size==0;
}

//bool TreeIsFull(const Tree *ptree)
//{
//	return ptree->size==MSIZE;
//}

int TreeItemCount(const Tree *ptree)
{
	return ptree->size;
}

/* description: construct an `item' object from the input word in `buffer'
 * return value: the constructed `item'*/
Item MakeItem(const char *buffer)
{
	int size=strlen(buffer);
	Item item;
	/*to save space, we allocate the exact amount of space needed to store the word. Then store its address in Item*/
	item.pword=(char *)malloc(size+1);
	strcpy(item.pword, buffer);
	item.count=1;
	return item;
}

/*description: add item to tree.
 *return value: `true' if successful.
 *note: firstly seek item in tree (SeekItem()). If it's in tree, count++.
        If it's not in tree, create a node with this item (MakeNode()), then add this node to tree (AddNode()). */
bool AddItem(const Item *pitem, Tree *ptree)
{
	Trnode *new_node;
	Find result=SeekItem(pitem, ptree);
	if( result.current!=NULL )//if the item is already in tree, count++
	{
		(result.current->item.count)++;
		return true;
	}
	else			  //otherwise create a node and add the node to tree
	{
		new_node=MakeNode(pitem);
		if( new_node==NULL )
		{
			fputs("couldn't make node.\n", stderr);
			return false;
		}
		if( ptree->root==NULL )//make node the root node of tree
		{
			ptree->root=new_node;
		}
		else		       //find place to add node
		{
			AddNode(new_node, ptree->root);
		}
		ptree->size++;
		return true;
	}
}

bool InTree(const Item *pitem, const Tree *ptree)
{
	return ((SeekItem(pitem, ptree)).current)!=NULL;
}

/* description: delete `item' in `tree'
 * return value: true if successful
 * note: to delete an item, the containing node has to be deleted. Then the left and right subtrees of it and its parent node has to be connected. These details are dealt with in DeleteNode()*/
bool DeleteItem(Item *pitem, Tree *ptree)
{
	Find result=SeekItem(pitem, ptree);
	if( result.current==NULL )//didn't find the item
	{
		return false;
	}
	else if( result.parent==NULL )//current!=NULL and parent==NULL, means the item is at root node
	{
		DeleteNode(&ptree->root);
	}
	/*we must determine in which subtree of parent node contains the item/the node, because we need that pointer to connect the rest of subtree after the node is deleted. And to modify the value of the pointer, we need to pass the address of the pointer*/
	else if( result.current==result.parent->left )
	{
		DeleteNode(&result.parent->left);
	}
	else
	{
		DeleteNode(&result.parent->right);
	}
	ptree->size--;
	return true;
}

/* description: traverse tree and apply `pfunc' to each item.
 * note: the traversing is performed recursively by InOrder()*/
void Traverse(Tree *ptree, void (*pfunc)(Item item))
{
	if( ptree->root!=NULL )
	{
		InOrder(ptree->root, pfunc);
	}
}

/* description: delete tree, reset everything
 * note: deletion of all nodes are handled in DeleteAllNodes()*/
void DeleteAll(Tree *ptree)
{
	if( ptree->root!=NULL )
	{
		DeleteAllNodes(ptree->root);
	}
	ptree->root=NULL;
	ptree->size=0;
}

/*-------------------local functions-------------------*/

//the function can also be implemented recursively
static Find SeekItem(const Item *pitem, const Tree *ptree)
{
	Find result;
	result.parent=NULL;
	result.current=ptree->root;

	if( result.current==NULL )//if root doesn't exist
	{
		return result;
	}

	/*determine to left or to right at every node,
	 * if neither way to go, the node contains the item to be found*/
	while( result.current!=NULL )
	{
		if( ToLeft(pitem, &(result.current->item)) )
		{
			result.parent=result.current;
			result.current=result.current->left;
		}
		else if( ToRight(pitem, &(result.current->item)) )
		{
			result.parent=result.current;
			result.current=result.current->right;
		}
		else
			break;
	}
	return result;
}

static Trnode * MakeNode(const Item *pitem)
{
	Trnode *new_node=(Trnode *)malloc(sizeof(Trnode));
	if( new_node!=NULL )
	{
		new_node->item=*pitem;
		new_node->left=NULL;
		new_node->right=NULL;
	}
	return new_node;
}

/*recursively find correct place for adding node*/
static void AddNode(Trnode *new_node, Trnode *root)
{
	if( ToLeft(&new_node->item, &root->item) )
	{
		if( root->left==NULL )
		{
			root->left=new_node;
		}
		else
		{
			AddNode(new_node, root->left);
		}
	}
	else if( ToRight(&new_node->item, &root->item) )
	{
		if( root->right==NULL )
		{
			root->right=new_node;
		}
		else
		{
			AddNode(new_node, root->right);
		}
	}
	else
	{
		fputs("location error in AddNote.\n", stderr);
	}
}

//if items are word-dominated, use strcmp()
static bool ToLeft(const Item *pitem_new, const Item *pitem_root)
{
	if( strcmp(pitem_new->pword, pitem_root->pword)<0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

static bool ToRight(const Item *pitem_new, const Item *pitem_root)
{
	if( strcmp(pitem_new->pword, pitem_root->pword)>0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*if the node to be deleted doesn't have left subtree, attach its right subtree to the parent node, replacing the node to be deleted. Vice versa. Then free memory of both `pword' and the node.
 *if the node to be deleted has two subtrees, attach its left subtree to parent node, find lowest node in left subtree to attach the right subtree. Then free memory of both `pword' and the node.*/
static void DeleteNode(Trnode **ppnode)
{
	Trnode *temp;
	if( (*ppnode)->left==NULL )
	{
		temp=*ppnode;
		(*ppnode)=(*ppnode)->right;
		free(temp->item.pword);
		free(temp);
	}
	else if( (*ppnode)->right==NULL )
	{
		temp=*ppnode;
		*ppnode=(*ppnode)->left;
		free(temp->item.pword);
		free(temp);
	}
	else
	{
		temp=(*ppnode)->left;
		//find the lowest node in left subtree to attach the right node
		while( temp->right!=NULL )
		{
			temp=temp->right;
		}
		temp->right=(*ppnode)->right;
		temp=*ppnode;
		*ppnode=(*ppnode)->left;
		free(temp);
	}
}

/*apply `pfun' recursively in the order of left-item-right, bottom-up*/
static void InOrder(Trnode *pnode, void (*pfunc)(Item item))
{
	if( pnode->left!=NULL )
	{
		InOrder(pnode->left, pfunc);
	}
	(*pfunc)(pnode->item);
	if( pnode->right!=NULL )
	{
		InOrder(pnode->right, pfunc);
	}
}

/*similar with InOrder(), but the left and right subtree should be visited and freed before item, otherwise will lose track of right subtree*/
static void DeleteAllNodes(Trnode * pnode)
{
	if( pnode->left!=NULL )
	{
		DeleteAllNodes(pnode->left);
	}
	if( pnode->right!=NULL )
	{
		DeleteAllNodes(pnode->right);
	}
	//before freeing node, we should firstly free space occuped by word.
	free(pnode->item.pword);
	free(pnode);
}

