/*
 *  CompTree.h is the header for a command line utility which demonstrates
 *  use of a C program as a code generator.  In this case, the code generated
 *  is valid C math expressions which are sent to the console as text.  Each
 *  expression is made by randomly populating a composite tree structure.
 *  Operators come from the 1st input text file, while operands come from
 *  the 2nd input text file.  Object oriented techniques are used to
 *  simplify coding.
 *  
 *  This C program was developed to be posted on my web site, at
 *  http://www.williamsonic.com/CompTree, to illustrate Composite Tree
 *  techniques, which are useful for compiler and parser testing.
 *  Created by Mark Williamsen on 10/26/2010.
 *
 */

enum nodeType
{
	branch,
	leaf
};

typedef struct node nodeStruct;

/* composite tree is made up of node structs, arranged in linked lists */
struct node
{
	/* data members common to all nodes */
	enum nodeType theType;
	nodeStruct *next;	/* list at my level */
	
	/* data members for branch node */
	nodeStruct *list;	/* list beneath my level */
	char pre[32];
	char inter[32];
	char post[32];
	
	/* data members for leaf node */
	char name[32];
	int  myDepth;
};

/* method members which operate on node structs */
void populate(nodeStruct *theNodePtr, int *theDepthPtr, int *nodeCountPtr);
void express (nodeStruct *theNodePtr, int *maxDepthPtr);
void release (nodeStruct *theNodePtr, int *nodeCountPtr);
