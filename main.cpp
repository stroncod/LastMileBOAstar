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
double probabilities_table[MAX_CITIES][MAX_CITIES];
double paso_matrix[max_problem_size][max_problem_size];
int succ_matrix[max_problem_size][max_problem_size];
vector<vector<Edge>> sorted_edges;
int ncities = 0;
double w;

double sorted_probs[MAX_CITIES];
int cities[MAX_CITIES];

boost::container::vector<Solution*> solutions; 

int initial_city = 0;
int generated_nodes;
int expanded_nodes;

//double city_weights[MAX_CITIES] = {0.23096777, 0.27334326, 0.06755775, 0.29685889, 0.13127233};
//double city_weights[MAX_CITIES] = {0.00989596, 0.0602574 , 0.06482666, 0.04278365, 0.01577632,
//       0.1732365 , 0.04556679, 0.07857399, 0.24014894, 0.26893381};
//double city_weights[MAX_CITIES] = {0.06974428, 0.11602551, 0.0523294 , 0.1157239 , 0.09386252,
//       0.01621567, 0.05760262, 0.08557558, 0.03268164, 0.01132345,
//       0.091636  , 0.01876605, 0.0016644 , 0.04662738, 0.1902216};
double city_weights[MAX_CITIES] = {0.00211822, 0.11540042, 0.04333332, 0.00204014, 0.0121299 ,
       0.05710817, 0.0092357 , 0.08271591, 0.05408277, 0.04493818,
       0.07292101, 0.03763544, 0.11700132, 0.03472672, 0.02216916,
       0.0418944 , 0.00552211, 0.16620586, 0.03345843, 0.04536283};
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

void read_problem2(const char *filename) {

	FILE *f;
    int y,x,num;
	double num2;
    f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
    rewind (f);

	for (y = 0; y < ncities; y++) {
      for (x = 0; x < 3; x++) {
		if (x == 1) {
			fscanf(f, "%d", &num);
			cities[y+1] = num;
		} else {
			if (x == 2) {
				fscanf(f, "%lf", &num2);
				distance_matrix[0][y+1] = num2;
			} else {
				fscanf(f, "%d", &num);
			}
		}
      }
    }

    for (y = 0; y < ncities; y++) {
      for (x = 0; x < ncities; x++) {
		fscanf(f, "%lf", &num2);
		//printf("m: %lf", num2);
		distance_matrix[y+1][x+1] = num2;
      }
	  //cin.get();
    }
    fclose (f);

	for(y = 0; y < ncities; y++) {
		distance_matrix[y+1][0] = 0.0;
	}
}

void read_probabilities(const char *filename){
	
	FILE *f;
    int y,x;
	int num;
	double num2;
    f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
    rewind (f);
    for (y = 0; y < ncities; y++) {
      for (x = 0; x < 25; x++) {
		if(x > 0) { 
			fscanf(f, "%lf", &num2);
			//printf("x>1: %lf", num2);
			probabilities_table[y+1][x] = num2; }
		else {
			fscanf(f, "%d", &num);
			//printf("x0: %d ", num);
			
		}
      }

	  //cin.get();
    }

	for (y = 0; y < 24; y++) {
		probabilities_table[0][y] = 0.0;
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
		for (size_t j = 0; j < 25; j++)
			if(min_node > probabilities_table[i][j]) min_node = probabilities_table[i][j];	
		sorted_probs[i] = min_node;
		min_node = LARGE;
	}
}

int get_time_frame(double g_t) { 
		if(g_t > 24) return trunc((g_t-24));
		return trunc(g_t); 
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
                }
                count_b++;
            }
        }   
    }
   
    val += sorted_edges[city][1].cost + sorted_edges[initial_city][1].cost;
	//printf("h time calculated %1.f \n", val);
    return val*0.5;

 }

double in_out_prob(Node_h* current, short next_city ,vector<short> visited){
	double val = 0.0; 
	int count_a=0, count_b=0;

	int last_frame = get_time_frame(current->g.first);
	
   	for(int i=0; i<ncities; i++){ // for the missing cities
   		
        if(i != next_city && !visited[i] ){ //if is not the same city and is not in the subtour
			val += distance_matrix[next_city][i]; //get full time on lex order
            //val += sorted_probs[i];
        }
    }
	int max_frame = get_time_frame(current->g.first+val); 

	double min_val = LARGE;
	if(max_frame > 24) max_frame = 24; //always gonna be zero otherwise
	for (size_t i = last_frame; i < max_frame+1; i++) {
		if(probabilities_table[next_city][i] < min_val) min_val = probabilities_table[next_city][i];
	}
	//val += sorted_probs[city] + sorted_probs[initial_city];
	//printf("h prob calculated %1.f \n", val);
	//preguntar si es la suma o es la pura wea
	return min_val;
   
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
				//h_p = in_out_prob(current,i,cities_visited);
				h_p = 0;

				g_t = current->g.first + distance_matrix[current->city][i];

				int frame = get_time_frame(g_t);

				g_p = current->g.second + probabilities_table[i][frame];//*city_weights[i];
				//printf("prob for frame %d: %.1f \n", frame, (1 - probabilities_table[i][frame]));
				//printf("the fuck is happening: %.3f %.3f \n", probabilities_table[i][frame], city_weights[i]);
				
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				//f = (((g_t+h_t)*w)*1000000+(g_p+h_p));
				f = (g_t+h_t*w)*1000000+h_t;
				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;

				
				Node_h* succ = new Node_h(i,g,h,f,current->depth+1,v,current);
				
				//cout<<"city: " <<succ->city<<endl;
				//printf("g_t, g_p: ( %.3f, %.3f ) \n", g.first, g.second);
				//printf("h_t, h_p: ( %.3f, %.3f ) \n", h.first, h.second);
				//printf("f: %.1f \n", f);
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
				//cout<<"going back"<<endl;
				g_t = current->g.first + distance_matrix[current->city][initial_city];
				int frame = get_time_frame(g_t);
				//printf(" frame: %d",frame);
				g_p = current->g.second + probabilities_table[initial_city][frame];//*city_weights[past_succ];
				//printf("the fuck is happening: %.3f %.3f \n", probabilities_table[initial_city][frame],city_weights[past_succ]);
				
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				//f = (((g_t+h_t)+w)*1000000+(g_p+h_p));
				f = (g_t+h_t*w)*1000000+h_t;

				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;
				Node_h* succ = new Node_h(initial_city,g,h,f,current->depth+1,v,current);
				
				//cout<<"city: " <<succ->city<<endl;
				//printf("g_t, g_p: ( %.3f, %.3f ) \n", g.first, g.second);
				//printf("h_t, h_p: ( %.3f, %.3f ) \n", h.first, h.second);
				//printf("f: %.1f \n", f);
				
				//print_node(succ,current_solution,cities_visited);
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(past_succ);
			}
			if(current->city != past_succ && past_succ != initial_city){


				h0_t = in_out(past_succ,cities_visited);
				//h0_p = in_out_prob(current,past_succ,cities_visited);

				h_t = max((current->h.first-distance_matrix[current->city][past_succ]) ,h0_t);
				h_p = 0;
				//h_p = max((current->h.second-probabilities_table[current->city][past_succ]) ,h0_p);
				g_t = current->g.first + distance_matrix[current->city][past_succ];
				int frame = get_time_frame(g_t);
				//printf(" frame: %d",frame);
				g_p = current->g.second + probabilities_table[past_succ][frame];//*city_weights[past_succ];
				//printf("prob for frame %d: %.1f \n", frame, (1- probabilities_table[past_succ][frame]));
				//printf("the fuck is happening: %.3f %.3f \n", probabilities_table[past_succ][frame], city_weights[past_succ]);
				//f = (((g_t+h_t)*w)*1000000+(g_p+h_p));
				f = (g_t+h_t*w)*1000000+h_t;
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				double g2_min = get_min_g2();
				if(g_p +h_p >=  g2_min) continue;
	
				Node_h* succ = new Node_h(past_succ,g,h,f,current->depth+1,v,current);
				
				//cout<<"city: " <<succ->city<<endl;
				//printf("g_t, g_p: ( %.3f, %.3f ) \n", g.first, g.second);
				//printf("h_t, h_p: ( %.3f, %.3f ) \n", h.first, h.second);
				//printf("f: %.1f \n", f);
				
				//print_node(succ,current_solution,cities_visited);
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(past_succ);
			}
		}
		
	}

	
}

int aStar(int init_city, double w, int lookahead, int start_time) {
	
	int missing_cities = ncities;
	vector<short> cities_visited;
	vector<int> current_solution;
	int iter = 0;

	ncities = 30;
	cout<<"****** DOING BOA* SEARCH ******"<<endl;
	vector<int> v;
	pair<double,double> g_init = make_pair(start_time+1,0);
	pair<double,double> h_init = make_pair(start_time+1,0);
	
	Node_h* initial_node = new Node_h(init_city,g_init,h_init,0,1,v,NULL);
	open.push(initial_node);

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while(!open.empty()) {

		Node_h* current = open.top();	
		cities_visited = fill_visited_cities(current);
		//cout<<"*****Current node****"<<endl;
		//cout<<current->city<<endl;
		//cout<<"depth: "<<current->depth<<endl;
		//print_node(current,current_solution,cities_visited);
		
		double g2_min = get_min_g2();
		
		//10 minute deadline
		if(std::chrono::steady_clock::now() - start > std::chrono::seconds(60000)) 
            return -1;
		
		if(current->g.second+0 >= g2_min) {
			printf("current g2 vs g2_min: (%.1f ,%.1f) \n", current->g.second, g2_min);
			open.pop();
			continue;
		} 

		
		if(current->depth > ncities) {

			Solution* solution = new Solution(current->g.first, current->g.second);
			solutions.push_back(solution);
			open.pop();
			//continue;
			return -1;
		}	

		closed.push_back(current);
		open.pop();
		expanded_nodes++;
		
		//Successors generation
		//cout<<"*****SUCCESSORS****"<<endl;
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
		int start_time = 0; //9 am 
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		int sol_response = aStar(initial_city,w,lookahead,start_time);
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		if(sol_response == -1){
			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);

			cout<<"Pareto Frontier (g1,g2): "<<endl;
			for (auto &sol : solutions){
				printf("\t ( %.3lf , %.2lf ) \n",sol->g1, sol->g2);
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
	ncities = 30;
	
	
	//read_problem("../problems/AdaptedFormat/51.mtsp");
	//read_problem("../problems/LastMile/20cities/20.2.tsp");
	//read_problem2("../problems/test1/30.tsp");
	//read_probabilities("../problems/test1/30.lastmile");
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