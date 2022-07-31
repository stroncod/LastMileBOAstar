/////////////////////////////////////////////////////////////////////
// Xiaoxun Sun & Sven Koenig @ USC 2009
// All rights reserved
/////////////////////////////////////////////////////////////////////


#ifndef MAZEH
#define MAZEH

#include "include.h"


double sqr(double num);

struct cell1;
typedef struct cell1 cell1;

struct cell1
{
  long long int id;
  double d;
  double key;
  int iteration;
  int heapindex;
  int overexpanded;
};



struct cell2;
typedef struct cell2 cell2;

struct cell2
{
//  long long int id;
  int n_state;
  int g1;
  int g2;
  double key3;
//  int iteration;
  int heapindex3;
//  int overexpanded;
  cell2 *searchtree;
};

cell1 *maze1;

int   **mazeh;
cell1 * generate_anchor;
cell1 * generate_end;
cell1 *mazestart1, *mazegoal1;
cell2 *startstate, *goalstate;




int mazeiteration, searchiteration;
int RUN;


#endif
