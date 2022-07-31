/////////////////////////////////////////////////////////////////////
// Carlos Hernandez
// All rights reserved
/////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "include.h"
#include "heap.h"
#include "maze.h"
#include "dijkstra.h"
#include "math.h"


#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void read_adjacent_table(const char *filename)
{
    FILE *f, *f1;
    int i,j,ori,dest,dist,t;
    f = fopen(filename,"r");
    char what, line[512];
	int num_arcs = 0,maxi = 0,prom = 0;
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );exit( 1 );}
	fscanf(f,"%d %d",&TNodes, &num_arcs);
	//printf("TNodes:%d num_arcs: %d\n",TNodes,num_arcs);
	//getchar();
	fscanf(f,"\n");
    
    for (i = 0; i < TNodes; i++)
		adjacent_table[i][0] = 0;

    for (i = 0; i < num_arcs; i++)
    {
		fscanf(f,"%d %d %d\n",&ori,&dest,&t);
		adjacent_table[ori-1][0]++;
		adjacent_table[ori-1][adjacent_table[ori-1][0] * 2 - 1] = dest-1;
		adjacent_table[ori-1][adjacent_table[ori-1][0] * 2] = t;

		pred_adjacent_table[dest-1][0]++;
		pred_adjacent_table[dest-1][pred_adjacent_table[dest-1][0] * 2 - 1] = ori-1;
		pred_adjacent_table[dest-1][pred_adjacent_table[dest-1][0] * 2] = t;

    }
    fclose (f);
}



void generate_maze()
{
	int y,x;
	read_adjacent_table(aentrada);
	
    if (maze1 == NULL){ 
		maze1 = (cell1 *)calloc(TNodes , sizeof(cell1));
		for (y = 0; y < TNodes; ++y)
		{
			maze1[y].id = y;
			maze1[y].overexpanded = 0;
			maze1[y].iteration = 0;
			maze1[y].d = LARGE;
		}
	}
}

/*----------------------------------------------------------------------------------*/  
int main(int argc, char** argv)
{
strcpy(aentrada,argv[1]);
//start = atoi(argv[2])-1;
goal = atoi(argv[2])-1;
end = atoi(argv[3])-1;
call_dijkstra();
}
