#include <iostream>
#include <cstdint> //or <stdint.h>
#include <limits>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/any.hpp>
#include <boost/range/adaptor/map.hpp>
#include <random> 
#include <boost/container/vector.hpp>
//#include <boost/container/stable_vector.hpp>
//#include <boost/container/static_vector.hpp>
//#include <boost/array.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lockfree/stack.hpp>
#include <stdexcept>
#include <chrono>
#include <functional>

#include "node.hpp"


#define max_problem_size 1000
#define LARGE 100000000
#define MAX_CITIES 1002


using namespace std;
using namespace boost::heap;

double cities_coor[max_problem_size][2];
double distance_matrix[MAX_CITIES][MAX_CITIES];
double probabilities_table[MAX_CITIES][5];
double paso_matrix[max_problem_size][max_problem_size];
int succ_matrix[max_problem_size][max_problem_size];
vector<vector<Edge>> sorted_edges;
int ncities = 0;
double w;

double sorted_probs[MAX_CITIES];

boost::container::vector<Solution*> solutions; 

int initial_city = 0;
int generated_nodes;
int expanded_nodes;

void read_problem(const char *filename) {

    FILE *f;
    int y,x,num;
    f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
    rewind (f);
    for (y = 0; y < ncities; y++) {
      for (x = 0; x < 3; x++) {
		fscanf(f, "%d", &num);
		if (x > 0) cities_coor[y][x-1] = num;
      }
    }
    fclose (f);
}

void read_probabilities(const char *filename){
	
	FILE *f;
    int y,x;
	double num;
    f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
    rewind (f);
    for (y = 0; y < ncities; y++) {
      for (x = 0; x < 24; x++) {
		fscanf(f, "%lf", &num);
		probabilities_table[y][x] = num;
      }
    }
    fclose (f);
}

void distance_matrix_caculation() {
  int y,y1;

  for (y = 0; y < ncities; y++) {
    for (y1 = 0; y1 < ncities; y1++) {
      if (y != y1) distance_matrix[y][y1] = sqrt(pow((cities_coor[y1][1]-cities_coor[y][1]),2) + pow((cities_coor[y1][0]-cities_coor[y][0]),2));
      
      else distance_matrix[y][y1] =0;


      //Se inicializa matrices para ordenar sucesores
      paso_matrix[y][y1] = distance_matrix[y][y1];
      succ_matrix[y][y1] = y1;
     
    }
  }

}

void succ_matrix_caculation() {
  int y,x,j,aux1,aux2;
  for (y = 0; y < ncities; y++) {
    for (x = 0; x < ncities; x++) {
      for (j = 0; j < ncities-1; j++) {
        if (paso_matrix[y][j] > paso_matrix[y][j+1]) {
          aux1 = paso_matrix[y][j];
          aux2 = succ_matrix[y][j];
          paso_matrix[y][j] = paso_matrix[y][j+1];
          succ_matrix[y][j] = succ_matrix[y][j+1];
          paso_matrix[y][j+1] = aux1;
          succ_matrix[y][j+1] = aux2;
        }
      }
    }
  }
}


vector<int> create_solution(Node_h* candidate) {

	vector<int> solution;
	solution.push_back(candidate->city);
	Node_h* parent = candidate->father;
	while(parent != NULL) {
		solution.push_back(parent->city);
		parent = parent->father;
	}

	return solution;
}

vector<short> fill_visited_cities(Node_h* candidate) {

	vector<short> cities(ncities);
	cities[candidate->city] = 1;
	cities[initial_city] = 1;
	Node_h* parent = candidate->father;
	while(parent != NULL) {

		cities[parent->city]= 1;
		parent = parent->father;
	}
	return cities;
}



void sort_edges() {

	vector<double> v;
	vector<Edge> edges;

	for(unsigned i = 0; i < ncities; ++i) {
	 	for(unsigned j = 0; j < ncities; ++j) {
	 		v.push_back(distance_matrix[i][j]);

	 	}
	 	for( auto k: sort_indexes(v) ){
	 		Edge edge = Edge(v[k],k); 		
	 		edges.push_back(edge);
	 	}
	 	sorted_edges.push_back(edges);
	 	v.clear();
	 	edges.clear();
	 } 

 }

void get_min_probs(){
	double min_node = LARGE;
	for (size_t i = 0; i < ncities; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			if(min_node > probabilities_table[i][j]) min_node = probabilities_table[i][j];
		}
		
		sorted_probs[i] = min_node;
		min_node = LARGE;
	}
	
}

double in_out(short city, vector<short> visited ) {

   double val = 0.0; 
   int count_a=0, count_b=0;
   for(int i=0; i<ncities; i++){ // for the missing cities
   		
        if(i != city && !visited[i] ){ //if is not the same city and is not in the subtour
            int count_a = count_b = 0;
            while(count_a < 2){
            	//only for edges who are not connected to an interior node; initial node is never interior
                if(!visited[sorted_edges[i][count_b].to_city]  || sorted_edges[i][count_b].to_city==initial_city ){
                	if(sorted_edges[i][count_b].to_city != i) { //eliminate edges with the same nodes in both sides
                    	val += sorted_edges[i][count_b].cost;
                    	count_a++;
                	}
                    //cout << " - Sumando " << cf[i] << " nodo " << min_edge[cf[i]][count_b].nodo1 << " - Cost: " << min_edge[cf[i]][count_b].cost  << endl;
                }
                count_b++;
                //cin.get();
            }
        }   
    }
   
    val += sorted_edges[city][1].cost + sorted_edges[initial_city][1].cost;
	//printf("h time calculated %1.f \n", val);
    return val*0.5;

 }

double in_out_prob(short city, vector<short> visited){
	double val = 0.0; 
	int count_a=0, count_b=0;
   	for(int i=0; i<ncities; i++){ // for the missing cities
   		
        if(i != city && !visited[i] ){ //if is not the same city and is not in the subtour
            val += sorted_probs[i];
        }   
    }
	val += sorted_probs[city] + sorted_probs[initial_city];
	//printf("h prob calculated %1.f \n", val);
	return val;
   
}


int get_time_frame(double g_t) { 
		if(g_t > 1440) return trunc((g_t-1440)/60);
		return trunc(g_t/60); 
	}

double get_min_g2() {
	if(solutions.empty()) return LARGE;
	int i = (solutions.size()-1);
	return solutions[i]->g2;
}

void get_successors(Node_h* current, vector<short> cities_visited){

	vector<int> v;
	double h_t,h_p,g_t,g_p,f,h0_t,h0_p;
	pair<double,double> g;
	pair<double,double> h;
	if(current->father == NULL) {
		for(short i=0; i<ncities; i++){ 
			if(i != initial_city) {

				h_t = in_out(i,cities_visited);
				h_p = in_out_prob(i,cities_visited);

				g_t = current->g.first + distance_matrix[current->city][i];
				int frame = get_time_frame(g_t);
				g_p = current->g.second + (1 - probabilities_table[i][frame]);
				//printf("prob for frame %d: %.1f \n", frame, (1 - probabilities_table[i][frame]));
				f = ((g_t+h_t)*w)*1000000+(g_p+h_p);
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;

				
				Node_h* succ = new Node_h(i,g,h,f,current->depth+1,v,current);
				cout<<"city: " <<succ->city<<endl;
				
				printf("g_t, g_p: ( %.1f, %.1f ) \n", g.first, g.second);
				printf("h_t, h_p: ( %.1f, %.1f ) \n", h.first, h.second);
				printf("f: %.1f \n", f);
				//cout<<i<<" ";
				//print_node(succ,current_solution,cities_visited);
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(i);
			}
		}
	} else {

		for(auto &past_succ: current->father->succs){
			if(current->depth == ncities){
				//when all cities are visited
				h_t = 0.0;
				h_p = 0.0;
				cout<<"going back"<<endl;
				g_t = current->g.first + distance_matrix[current->city][initial_city];
				int frame = get_time_frame(g_t);
				g_p = current->g.second + (1- probabilities_table[initial_city][frame]);
				//printf("prob for frame %d: %.1f \n", frame, (1- probabilities_table[initial_city][frame]));
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				f = ((g_t+h_t)*w)*1000000+(g_p+h_p);
				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;
				Node_h* succ = new Node_h(initial_city,g,h,f,current->depth+1,v,current);
				cout<<"city: " <<succ->city<<endl;
				printf("g_t, g_p: ( %.1f, %.1f ) \n", g.first, g.second);
				printf("h_t, h_p: ( %.1f, %.1f ) \n", h.first, h.second);
				printf("f: %.1f \n", f);
				//cout<<i<<" ";
				//print_node(succ,current_solution,cities_visited);
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(past_succ);
			}
			if(current->city != past_succ && past_succ != initial_city){


				h0_t = in_out(past_succ,cities_visited);
				h0_p = in_out_prob(past_succ,cities_visited);

				h_t = max((current->h.first-distance_matrix[current->city][past_succ]) ,h0_t);
				h_p = max((current->h.second-probabilities_table[current->city][past_succ]) ,h0_p);
				g_t = current->g.first + distance_matrix[current->city][past_succ];
				int frame = get_time_frame(g_t);
				g_p = current->g.second + (1- probabilities_table[past_succ][frame]);
				//printf("prob for frame %d: %.1f \n", frame, (1- probabilities_table[past_succ][frame]));
				f = ((g_t+h_t)*w)*1000000+(g_p+h_p);
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				double g2_min = get_min_g2();
				if(g_p +h_p >=  g2_min) continue;
	
				Node_h* succ = new Node_h(past_succ,g,h,f,current->depth+1,v,current);
				
				cout<<"city: " <<succ->city<<endl;
				printf("g_t, g_p: ( %.1f, %.1f ) \n", g.first, g.second);
				printf("h_t, h_p: ( %.1f, %.1f ) \n", h.first, h.second);
				printf("f: %.1f \n", f);
				//cout<<i<<" ";
				//print_node(succ,current_solution,cities_visited);
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(past_succ);
			}
		}
		
	}

	
}

int aStar(int init_city, double w, int lookahead) {
	
	int missing_cities = ncities;
	vector<short> cities_visited;
	vector<int> current_solution;
	int iter = 0;

	cout<<"****** DOING BOA* SEARCH ******"<<endl;
	vector<int> v;
	pair<double,double> g_init = make_pair(0,0);
	pair<double,double> h_init = make_pair(0,0);
	
	Node_h* initial_node = new Node_h(init_city,g_init,h_init,0,1,v,NULL);
	open.push(initial_node);

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while(!open.empty()) {

		Node_h* current = open.top();	
		cities_visited = fill_visited_cities(current);
		cout<<"*****Current node****"<<endl;
		cout<<current->city<<endl;
		//cout<<"depth: "<<current->depth<<endl;
		//print_node(current,current_solution,cities_visited);
		
		double g2_min = get_min_g2();
		
		//10 minute deadline
		if(std::chrono::steady_clock::now() - start > std::chrono::seconds(600)) 
            return -1;
		
		if(current->g.second+0 >= g2_min) {
			printf("current g2 vs g2_min: (%.1f ,%.1f) \n", current->g.second, g2_min);
			open.pop();
			continue;
		} 

		
		if(current->depth > ncities) {
			//generate solution report
			//current_solution = create_solution(current);
			//cout<<"Best solution find (time): " <<current->g<<endl;
			//printf("Solution pair found (time,prob): (%.1f ,%.1f) \n", current->g.first, current->g.second);
			Solution* solution = new Solution(current->g.first, current->g.second);
			solutions.push_back(solution);
			open.pop();
			continue;
		}	

		closed.push_back(current);
		open.pop();
		expanded_nodes++;
		
		//Successors generation
		cout<<"*****SUCCESSORS****"<<endl;
		get_successors(current,cities_visited);
		iter++;
		//cin.get();	
	}
	return -1;
}


void search_driver(int lookahead, double w) {
	
	//add to change search algorithm

		if(lookahead == 0){

			lookahead = LARGE;
		}
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		int sol_response = aStar(initial_city,w,lookahead);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		if(sol_response == -1){
			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);

			cout<<"Pareto Frontier (g1,g2): "<<endl;
			for (auto &sol : solutions){
				printf("\t ( %d , %.1lf ) \n",sol->g1, sol->g2);
			}
			
			cout<<"Resolution Time: "<<time_span.count()<<endl;
			cout<<"Expanded Nodes: "<<expanded_nodes<<endl;
			cout<<"Generated Nodes: "<<generated_nodes<<endl;
		} else {
			cout<<"No solution found!"<<endl;
		}
		cout<<"****** ENDING BOA* SEARCH ******"<<endl;
		open.clear();
		closed.clear();
		open_map.clear();
		expanded_nodes=0;
		generated_nodes=0;
	
	
}

int main(int argc, char const *argv[])
{
	w = 1.0;
	int lookahead = 0;
	ncities = 20;
	read_problem("../problems/LastMile/20.3.tsp");
	read_probabilities("../problems/LastMile/20.3.lastmile");
	distance_matrix_caculation();
	
	/*
	int values[16] = {0,40,50,150,40,0,120,20,50,120,0,30,150,20,30,0};
	int k = 0;
	for(unsigned i = 0; i < ncities; ++i) {
		for(unsigned j = 0; j < ncities; ++j) {

			distance_matrix[i][j] = values[k];
			k++;
		}
	}*/

	succ_matrix_caculation();
	sort_edges();
	get_min_probs();

	expanded_nodes = 0;
	generated_nodes = 0;
	search_driver(lookahead,w);

	return 0;
}