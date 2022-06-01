/////////////////////////////////////////////////////////////////////
// Carlos Hernandez
// All rights reserved
/////////////////////////////////////////////////////////////////////

#ifndef INCLUDEH
#define INCLUDEH

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define MAXNODES 4000000
#define MAXNEIGH 45

unsigned int adjacent_table[MAXNODES][MAXNEIGH];
unsigned int pred_adjacent_table[MAXNODES][MAXNEIGH];
long long int coordinates[MAXNODES][2];
int goal,start, end; 

int TNodes;
long long int last_state;

//_________________________________________________________________________________________________________
#define LARGE  1000000000
#define BASE   1000000

long int statpercolated2;
char aentrada[81];
char aentrada1[81];


#define max(x,y) ( (x) > (y) ? (x) : (y) )
#define min(x,y) ( (x) < (y) ? (x) : (y) )

#endif
