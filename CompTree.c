/*
 *  CompTree.c implements a command line utility which demonstrates use of
 *  a C program as a code generator.  In this case, the code generated is
 *  valid C math expressions which are sent to the console as text.  Each
 *  expression is made by randomly populating a composite tree structure.
 *  Operators come from the 1st input text file, while operands come from
 *  the 2nd input text file.  Object oriented techniques are used to
 *  simplify coding.
 *  
 *  This C program was developed to be posted on my web site, at
 *  http://www.williamsonic.com/CompTree, to illustrate Composite Tree
 *  techniques, which are useful for compiler and parser testing.
 *  Created by Mark Williamsen on 10/26/2010.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "CompTree.h"

/* global variables */
FILE *brnchFile = NULL;
FILE *leafFile = NULL;

/* ------------------------------------------------------------------------- */
/* main entry point */
int main(int argc, const char*argv[])
{
	/* local loop index */
	int i = 0;
	
	/* check command line args */
	if (argc < 3)
	{
		fprintf(stderr, "Usage: CompTree brnch.txt leaf.txt\n"
		"Built: " __DATE__ ".\n");
		return EXIT_FAILURE;
	}
		
	/* open input files */
	brnchFile = fopen(argv[1], "r");
	leafFile  = fopen(argv[2], "r");
	if (!brnchFile || !leafFile)
	{
		fprintf(stderr, "Failed to open input files.\n");
		return EXIT_FAILURE;
	}
	
	/* send 25 random valid expressions to the console */
	for (i = 0; i < 25; i++)
	{
		int nodeCount = 0;
		int theDepth = 0;
		int maxDepth = 0;

		/* start the ball rolling with a branch node */
		nodeStruct *listHead = calloc(1, sizeof(nodeStruct));
		listHead->theType = branch;
		
		/* randomly populate a tree structure */
		populate(listHead, &theDepth, &nodeCount);
		
		/* express the tree as text, on the console */
		printf("what = ");
		express(listHead, &maxDepth);
		printf("; /* %d %d */\n", nodeCount, maxDepth);
		
		/* free memory used recursively */
		release(listHead, &nodeCount);
		assert(!nodeCount);		
	}
	
	/* close disk files */
	if (brnchFile){fclose(brnchFile);}
	if (leafFile) {fclose(leafFile);}
	
	return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------- */
/* ask node to populate itself randomly */
/* additional layers are randomly created beneath */
void populate(nodeStruct *theNodePtr, int *theDepthPtr, int *nodeCountPtr)
{
	int low = 0;
	int high = 0;
	int numOps = 0;
	nodeStruct *newNode = NULL;
	nodeStruct *nextNode = NULL;
	
	/* just checking */
	assert(theNodePtr);

	/* select different behavior depending on node type */
	switch(theNodePtr->theType)
	{
		case branch:
			/* count depth, fill fields for branch node */
			*theDepthPtr += 1;
			if (ferror(brnchFile))
				{fprintf(stderr, "failed reading branch input file.");}
			fscanf(brnchFile, "%s %s %s %d %d", theNodePtr->pre, theNodePtr->inter, theNodePtr->post, &low, &high);
			if (feof(brnchFile))
			{
				rewind(brnchFile);
				fscanf(brnchFile, "%s %s %s %d %d", theNodePtr->pre, theNodePtr->inter, theNodePtr->post, &low, &high);
			}
			
			/* decide randomly how many operands, in the range from low to high */
			numOps = (rand() % (high - low + 1)) + low;
			while(numOps--)
			{
				/* reserve memory for new node */
				newNode = calloc(1, sizeof(nodeStruct));

				/* determine what kind of node, with odds arbitrarily set to 50/50 */
				/* leaf to branch ratio, nodeCount, and theDepth are tunable parameters */
				if ((rand() < RAND_MAX / 2) && (*nodeCountPtr < 20) && (*theDepthPtr < 7))
					{newNode->theType = branch;}
				else
					{newNode->theType = leaf;}
					 
				/* append to list */
				if (nextNode)
					{nextNode->next = newNode;}
				else
					{theNodePtr->list = newNode;}
				nextNode = newNode;
				
				/* populate new node from input files */
				populate(newNode, theDepthPtr, nodeCountPtr);				
			}
			*theDepthPtr -= 1;
			break;
			
		case leaf:
			/* count nodes, fill fields for leaf node */
			*nodeCountPtr += 1;
			theNodePtr->myDepth = *theDepthPtr;
			if (ferror(leafFile)){fprintf(stderr, "failed reading leaf input file.");}
			fscanf(leafFile, "%s", theNodePtr->name);
			if (feof(leafFile))
			{
				rewind(leafFile);
				fscanf(leafFile, "%s", theNodePtr->name);
			}
			break;
		
		default: /* should never reach here */
			assert(!1);
			break;
	}
}
  
/* ------------------------------------------------------------------------- */
/* ask node to express itself as text */
/* call all subordinate nodes recursively */
void express(nodeStruct *theNodePtr, int *maxDepthPtr)
{
	/* local list iterator */
	nodeStruct *aNode = NULL;
	
	/* just checking */
	assert(theNodePtr);
	
	/* handle node types */
	switch (theNodePtr->theType)
	{
		/* branch nodes have a list of subordinates */
		case branch:
			if (*(theNodePtr->pre) != '.'){printf("%s",theNodePtr->pre);}
			
			/* iterate over list */
			for (aNode = theNodePtr->list; aNode; aNode = aNode->next)
			{
				express(aNode, maxDepthPtr);
				if ((aNode->next) && (*(theNodePtr->inter) != '.'))
					{printf("%s", theNodePtr->inter);}
			};
			if (*(theNodePtr->post) != '.'){printf("%s",theNodePtr->post);}
			break;
			
		/* no subordinates for leaf nodes */
		case leaf:
			printf("%s",theNodePtr->name);
			if (theNodePtr->myDepth > *maxDepthPtr)
				{*maxDepthPtr = theNodePtr->myDepth;}
			break;
			
		default: /* should never reach here */
			assert(!2);
			break;
	}
}

/* ------------------------------------------------------------------------- */
/* ask node to delete subordinates, then itself */
void release(nodeStruct *theNodePtr, int *nodeCountPtr)
{
	/* local list iterators */
	nodeStruct *aNode = NULL;
	nodeStruct *nextNode = NULL;
	
	/* just checking */
	assert(theNodePtr);
	
	/* handle different node types */
	switch (theNodePtr->theType)
	{
		/* branch nodes have a list of subordinates */
		case branch:
			aNode = theNodePtr->list;
			while(aNode)
			{
				/* extra step added to avoid using deleted pointer */
				nextNode = aNode->next;
				release(aNode, nodeCountPtr);
				aNode = nextNode;
			}
			break;
			
		/* handle terminal nodes here */
		case leaf:
			*nodeCountPtr -= 1;
			break;
			
		default: /* should never reach here */
			assert(!3);
			break;
	}
	
	/* always free this */
	free(theNodePtr);
}

/* ------------------------------------------------------------------------- */
/* following is some example output from this program, compiled here as dead code */
void junk(void)
{
	/* variable names taken from operand text file */
	short what;
	int mon;
	long tue;
	unsigned short wed;
	unsigned int thu;
	unsigned long fri;
	char sat;
	signed char sun;

/* comment following each line gives the leaf count and branch depth for the expression */
what = ((((mon&&tue)*wed)-(thu/(fri%sat%(sun+mon+tue^wed))/thu)-fri*sat*sun&&mon/tue/wed%thu%fri&&sat-sun)+mon+tue); /* 23 7 */
what = ((((wed&&thu)*(fri/sat))-((sun-mon+tue^wed^thu&&fri&&sat*sun/mon/tue*wed)%thu%fri))+sat^sun%((mon-tue)+wed+thu)%fri+sat)^sun; /* 26 7 */
what = (mon*(tue&&wed&&thu)*fri); /* 5 2 */
what = ((sat%(sun-mon-tue&&wed*thu+fri%sat/sun/mon^tue)%(wed+(((thu&&fri&&sat)*sun)-mon))^tue)/(((wed+thu-fri^sat)%sun)/mon)); /* 24 7 */
what = tue%wed/thu^((fri-sat)+sun)/(mon*(tue&&wed))&&thu&&fri*sat; /* 12 6 */
what = (sun/mon/(tue%wed%(thu^fri))); /* 6 3 */
what = sat+sun; /* 2 1 */
what = mon-tue-wed*thu&&fri/sat/sun; /* 7 4 */
what = ((mon-(tue*(wed&&(thu/fri/sat))*sun)-mon)+(tue%wed%(thu^fri))+sat+sun)^mon*tue*wed&&thu/fri%sat-sun^mon%tue; /* 23 7 */
what = (wed+((((thu/fri/sat)&&sun&&mon)*tue*wed)-((thu^fri+sat+sun^mon*tue*wed-thu)%fri)-sat&&sun/mon^tue^wed%thu%fri/sat)+sun)^mon^tue; /* 28 7 */
what = (wed+thu+fri); /* 3 1 */
what = (sat-(sun*mon*(tue&&((wed%thu%(fri^sat^sun))/mon-tue*wed-thu+fri+sat&&sun/mon)))); /* 17 7 */
what = tue/(wed+thu)^(fri-sat)^(((sun/mon)&&tue)*((wed+thu^fri)%sat*sun*mon-tue&&wed-thu/fri%sat)*sun)%mon%tue/wed; /* 23 7 */
what = ((thu-fri)+sat)^(sun*(mon&&tue&&(wed/(thu%(fri^sat^sun)%mon+tue))))^wed/thu%fri%sat&&(sun+mon+tue)^wed&&thu*fri*sat-sun-mon%tue; /* 27 7 */
what = (((wed&&thu)*(((fri+sat-sun-mon^tue*wed&&thu&&fri^sat/sun%mon%tue)%wed^thu)/((((fri&&sat&&sun)*mon*tue)-wed-thu)+fri)/sat))-sun); /* 26 7 */
what = (mon/((tue&&wed*thu-fri%sat/sun^mon^tue-wed+(thu+(fri-(sat*sun)-(mon&&tue))+wed)^(thu/((fri+sat^sun^mon)%tue%wed)/thu))%fri%sat)); /* 27 7 */
what = sun&&mon*tue/wed%thu%fri^(sat+sun)-mon-tue; /* 10 6 */
what = ((wed*(thu&&fri&&sat)*sun)-(mon/((tue-wed+thu+fri^sat)%sun*mon%tue%wed/thu/fri&&sat&&sun^mon^tue*wed%thu)/fri)-sat); /* 25 7 */
what = (sun+(mon-tue-(wed*thu*(fri&&sat)))+(sun/(mon%tue%(wed^thu*fri*sat-sun+mon+tue&&wed/thu&&fri^sat))/sun)); /* 22 7 */
what = mon%tue%wed; /* 3 1 */
what = (thu+fri)^sat^sun; /* 4 2 */
what = ((mon*((((tue^wed+thu+fri)%sat)/sun*mon&&tue-wed%thu/fri^sat-((sun-mon-tue)+wed)/(thu*fri*sat))&&sun&&mon))-tue); /* 23 7 */
what = ((wed/thu/(fri%(sat^sun)))&&mon+tue&&wed-thu); /* 9 4 */
what = fri&&sat^sun^mon%tue/wed*thu; /* 7 5 */
what = (((fri*((sat/(sun%mon))&&tue&&wed)*(thu^fri*sat*sun-mon&&tue+wed/thu+fri^sat^sun%(mon+tue)))-(wed-thu-fri)-sat)+sun); /* 24 7 */

}
