/////////////////////////////////////////////////////////////////////
// Carlos Hernandez 
// All rights reserved
/////////////////////////////////////////////////////////////////////


#include "heap.h"
#include "maze.h"
#include "stdio.h"
#include "include.h"
#include "math.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


short flag_success1 = 0;
short d;
long long int mazeiteration1 = 0;
long long int searches_astar1 = 0;
long long int solution_cost = 0;
unsigned long long int statexpanded1 = 0;
unsigned long long int stategenerated = 0;
unsigned long long int statexpanded1_first = 0;

float time_astar_initialize1 = 0;
float time_astar_first1 = 0;
int pruned = 0;


cell1 *tmpcell1;
cell1 *tmpcell2;
struct timeval   tv11, tv22 , tv11a, tv22a , tv11b, tv22b, tv11c, tv22c, tv11d, tv22d;  

long long int costp, timep,costpp, timepp, costo, timeo,narcs = 0;


void initialize_parameters(){
	int y,x;
	mazegoal1  = &maze1[goal];
    statpercolated2 = 0;
   
}

//------------------------------------------------------------------------------------------------------
int dijkstra()
{
    cell1 *pred;
    double arc_time = 0;
    long int cont;
    cont=0;

    emptyheap2();
    mazegoal1->d = 0;
    mazegoal1->key = 0;   
    insertheap2(mazegoal1);   

    while (topheap2() != NULL)    
    {       
        tmpcell1 = popheap2();
        cont++;
        tmpcell1->overexpanded = mazeiteration1;
       // printf("d:%f\n",tmpcell1->d);getchar();
        if(tmpcell1->id == end){
            printf(" %f \n", tmpcell1->d);
            break;
        }
       
        ++statexpanded1;
        for (d = 1; d < (pred_adjacent_table[tmpcell1->id][0]*2); d += 2)
        { 
            pred = &maze1[pred_adjacent_table[tmpcell1->id][d]];
            arc_time = pred_adjacent_table[tmpcell1->id][d+1];
            if(pred->d > tmpcell1->d + arc_time)
            {
                  pred->d = tmpcell1->d + arc_time;
                  pred->key = pred->d ;
                  insertheap2(pred);   
            }   
        }/* end for */
    } /* end while */
return(flag_success1);
}


/* ------------------------------------------------------------------------------*/



/* ------------------------------------------------------------------------------*/
void test_dijkstra()
{
    cell1 *tempcell,*previous;
    int y,x,i,aux;
    long int m;

		time_astar_first1 = 0;
		gettimeofday(&tv11d, NULL);

		//Dijkstra
		if(dijkstra())
		
		
		gettimeofday(&tv22d, NULL);
		time_astar_first1   = 1.0*(tv22d.tv_sec - tv11d.tv_sec) + 1.0*(tv22d.tv_usec - tv11d.tv_usec)/1000000.0;  		
		printf("Runtime(ms):%f statexpanded: %d\n",time_astar_first1*1000,statexpanded1);

		
return ;
}


//--------------------------------------------------------------------------------------
void call_dijkstra()
{    
    generate_maze();
    initialize_parameters();
    test_dijkstra();
return;
}
