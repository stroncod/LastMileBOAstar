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
int rcities = 0;
int instance = 0;
int time_length;
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

void read_problem3(const char *filename) {

	FILE *f;
    int y,x,num;
	long long int num2;
	long int _,__;
    f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
    rewind (f);

	fscanf(f, "%*[^\n]");
		
    for (y = 0; y < ncities; y++) {
      for (x = 0; x < ncities; x++) {
        //printf("%d %d\n", y,x);
        if(x != y) {

            fscanf(f, "%ld %ld %lld",&_, &__ ,&num2);

			distance_matrix[y][x] = (num2 == -1) ? LARGE : ((double) num2);

        } else {
            distance_matrix[y][x] = LARGE;
        }
      }
	  //cin.get();
    }
    fclose (f);

}

void read_times(const char *filename){
	FILE *f;
	int node1, node2, arc_val;
	f = fopen(filename,"r");
	if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
	rewind (f);
	fscanf(f, "%d");
	for (int i = 0; i < ncities; i++) {
		for (int j = 0; j < ncities; j++) {
			if(i != j) {
				fscanf(f, "%d %d %d", &node1, &node2, &arc_val);
				//distance_matrix[i][j] = (arc_val == -1) ? LARGE : ((double) arc_val/3600);
				distance_matrix[i][j] = (arc_val == -1) ? LARGE : ((double) arc_val/60);
			} else { distance_matrix[i][j] = LARGE; }
		}
	}
	fclose (f);
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

void read_rejection_rates(const char *filename){
	FILE *f;
	f = fopen(filename,"r");
    if (f==NULL){printf( "No se puede abrir el fichero.\n" );}
	double rates;
    rewind (f);
	for (int i = 0; i < ncities; i++) {
		for(int j= 0; j< time_length; ++j) {
			fscanf(f, "%lf", &rates);
			//printf(" %lf",rates);
			probabilities_table[i][j] = rates;
		}
		//printf("\n");
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
		//printf("g_t: %lf\n", g_t);
		if(g_t > 24) return trunc(g_t - 24*trunc(g_t/24));
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

                	    if(sorted_edges[i][count_b].cost >= LARGE) continue;
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
   		
        if(i != next_city && !visited[i] && distance_matrix[next_city][i] < LARGE ){ //if is not the same city and is not in the subtour
			val += distance_matrix[next_city][i]; //get full time on lex order
        }
    }
	int max_frame = get_time_frame(current->g.first+val); 

	double sum_min = 0.0;

	//if(max_frame > 24) max_frame = 24; //always gonna be zero otherwise
	// if(max_frame > 24) return LARGE; // Keep it in the fr
	
	if (max_frame <= last_frame){
		// over 24
		int days = trunc((current->g.first+val)/24);
		for(int i =0; i <ncities; ++i){
			double min_val = LARGE;
			for(int j = last_frame; j < 24*days; j++){
				if(probabilities_table[i][j] < min_val) 
					min_val = probabilities_table[i][j];
			}
			for(int k =0;  k < max_frame; ++k){
				if(probabilities_table[i][k] < min_val) 
					min_val = probabilities_table[i][k];
			}
			sum_min+=min_val;
		}
	} else {
		for(int i=0; i<ncities; i++){
			double min_val = LARGE;
			for (int j = last_frame; j < max_frame+1; j++) {
				if(probabilities_table[i][j] < min_val) 
					min_val = probabilities_table[i][j];
			}
			sum_min+=min_val;
		}
	}
		
	
	/*
	for(int i=0; i<ncities; i++){
		double min_val = LARGE;
		for (int j = last_frame; j < max_frame+1; j++) {
			if(probabilities_table[i][j] < min_val) 
				min_val = probabilities_table[i][j];
		}
		sum_min+=min_val;
	}*/
	return sum_min;
   
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
				
				g_t = current->g.first + distance_matrix[current->city][i];
				
				if(g_t >= LARGE) continue;


				h_t = in_out(i,cities_visited);

				int frame = get_time_frame(g_t);
				// printf("time frame=%d\n",frame);
				
				h_p = in_out_prob(current,i,cities_visited);
				

				g_p = current->g.second + probabilities_table[i][frame];
				//printf("current=%lf  + new_rate=%lf \n", current->g.second,probabilities_table[i][frame]);
				
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);

				f = (g_t+h_t*w)*1000000+(g_p+h_p);
				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;

				
				Node_h* succ = new Node_h(i,g,h,f,current->depth+1,v,current);
				//printf("city %d g_t, g_p: ( %.3f, %.3f ) h_t, h_p: ( %.3f, %.3f ) \n", 
				//		succ->city, g.first, g.second, h.first, h.second);

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
				if(g_t >= LARGE) continue;
                
				int frame = get_time_frame(g_t);
				//printf(" frame: %d",frame);
				g_p = current->g.second + probabilities_table[current->city][frame];
				
				
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				//f = (((g_t+h_t)+w)*1000000+(g_p+h_p));
				f = (g_t+h_t*w)*1000000+(g_p+h_p);

				double g2_min = get_min_g2();
				if(g_p +h_p >= g2_min) continue;
				Node_h* succ = new Node_h(initial_city,g,h,f,current->depth+1,v,current);
				//printf("city %d g_t, g_p: ( %.3f, %.3f ) h_t, h_p: ( %.3f, %.3f ) \n", 
				//		succ->city, g.first, g.second, h.first, h.second);
				
				generated_nodes++;
				open.push(succ);
				current->succs.push_back(past_succ);
			}
			if(current->city != past_succ && past_succ != initial_city){

				g_t = current->g.first + distance_matrix[current->city][past_succ];
				//printf("%f\n",g_t);
				if(g_t >= LARGE) continue;
				h0_t = in_out(past_succ,cities_visited);
				h0_p = in_out_prob(current,past_succ,cities_visited);
				
				
				h_t = max((current->h.first-distance_matrix[current->city][past_succ]) ,h0_t);
				h_p = max((current->h.second-probabilities_table[current->city][past_succ]) ,h0_p);
				
				// printf("h0_p:%lf, h_p:%lf\n", h0_p, h_p);
				
				int frame = get_time_frame(g_t);

				g_p = current->g.second + probabilities_table[past_succ][frame];

				f = (g_t+h_t*w)*1000000+(g_p+h_p);
				g = make_pair(g_t,g_p);
				h = make_pair(h_t,h_p);
				double g2_min = get_min_g2();
				// printf("g_p:%lf + h_p:%lf >= g2min:%lf \n",g_p, h_p, g2_min);
				if(g_p +h_p >=  g2_min) continue;
	
				Node_h* succ = new Node_h(past_succ,g,h,f,current->depth+1,v,current);
				//printf("city %d g_t, g_p: ( %.3f, %.3f ) h_t, h_p: ( %.3f, %.3f ) \n", 
				//		succ->city, g.first, g.second, h.first, h.second);

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

	ncities = rcities;
	//cout<<"****** DOING BOA* SEARCH ******"<<endl;
	vector<int> v;
	pair<double,double> g_init = make_pair(start_time,0);
	pair<double,double> h_init = make_pair(start_time,0);
	
	Node_h* initial_node = new Node_h(init_city,g_init,h_init,0,1,v,NULL);
	open.push(initial_node);

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while(!open.empty()) {

		Node_h* current = open.top();	
		if(current->g.first >= LARGE) continue;
		cities_visited = fill_visited_cities(current);

		//printf("city %d g_t:%f g_p:%f h_t:%f h_p:%f f:%f  depth:%d\n", current->city, current->g.first,
        //                current->g.second, current->h.first, current->h.second, current->f, current->depth);
		
		double g2_min = get_min_g2();
		
		//10 minute deadline
		if(std::chrono::steady_clock::now() - start > std::chrono::seconds(180)) 
            return -1;
		
		//pareto dominance prunning
		if(current->g.second+current->h.second >= g2_min) {
			//printf("current g2 vs g2_min: (%.1f ,%.1f) \n", current->g.second, g2_min);
			open.pop();
			continue;
		} 

		//sanity check
		//if(generated_nodes % 1000 == 0){
		//	printf("%ld,%d,%d!", solutions.size(), expanded_nodes, generated_nodes);
		//}
		// Solution branches
		if(current->depth > ncities) {

			Solution* solution = new Solution(current->g.first, current->g.second);
			solutions.push_back(solution);
			/*
			printf("Solution found N=%lu : ( %.6lf , %.6lf ); expanded: %d; generated: %d\n",
					solutions.size(),
					current->g.first,
					current->g.second,
					expanded_nodes,
					generated_nodes);
			*/
			open.pop();
			continue;
			//cin.get();
			//return -1;
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

		if(solutions.size() > 0){
			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);
			/*
			cout<<"Pareto Frontier (g1,g2): "<<endl;
			for (auto &sol : solutions){
				printf("\t ( %.3lf , %.2lf ) \n",sol->g1, sol->g2);
			}*/
			//cout<<"Frontier size: "<<solutions.size()<<endl;
			//cout<<"Resolution Time: "<<time_span.count()<<endl;
			//cout<<"Expanded Nodes: "<<expanded_nodes<<endl;
			//cout<<"Generated Nodes: "<<generated_nodes<<endl;
			//Python script output
			printf("%ld,%f,%d,%d\n", solutions.size(), time_span.count(), expanded_nodes, generated_nodes);
		} else {
			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);
			//cout<<"No solution found!"<<endl;
			//Python script output
			printf("No solution found!%ld,%f,%d,%d\n", solutions.size(), time_span.count(), expanded_nodes, generated_nodes);
		}
		//cout<<"****** ENDING BOA* SEARCH ******"<<endl;
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
	time_length = 24;
	ncities = atoi(argv[1]);
	rcities = atoi(argv[2]);
	const char* t_instance = argv[3];
	const char* rr_instance = argv[4];

	//read_problem3(t_instance);
	read_times(t_instance);
	read_rejection_rates(rr_instance);
	//read_probabilities(rr_instace);

	//Python script output
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