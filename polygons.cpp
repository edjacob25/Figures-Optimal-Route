#include "polygons.hpp"

using namespace std;

struct CompareWeight {
    bool operator()(node_p const & p1, node_p const & p2) {
        // return "true" if "p1" is ordered before "p2", for example:
        return p1->weight + p1->heuristic > p2->weight + p2->heuristic;
    }
};

bool compareWeight(node_p const & p1, node_p const & p2){
	return p1->weight + p1->heuristic < p2->weight + p2->heuristic;
}

bool between(float value, float la, float lb){
	if(la > lb){
		return ((lb<=value)&&(value<=la));
	}else{
		return ((la<=value)&&(value<=lb));
	}
}

//priority_queue<node_p, vector<node_p>, CompareWeight> fringe;
list<poligon_p> poligon_list;
list<node_p> open_list;
list<node_p> closed_list;
list<node_p> fringe_list;
list<edge_p> edges_list;                                                        
node_p start, finish;

void printFringe(){
	printf("\n---- FRINGE ----\n");
	for(list<node_p>::iterator i = fringe_list.begin(); i!= fringe_list.end(); i++){
        printf("%f,%f -> %f \n", (*i)->point->x, (*i)->point->y, (*i)->weight + (*i)->heuristic);
    }
}

void printClosed(){
	printf("\n---- CLOSED ----\n");
	for(list<node_p>::iterator i = closed_list.begin(); i!= closed_list.end(); i++){
        printf("%f,%f \n", (*i)->point->x, (*i)->point->y);
    }
}

void printPoligon(poligon_p poligon){
	point_p iterator = poligon->start;
	switch (poligon->sides){
		case 3:
			printf("Triangle\n");
			break;

		case 4:
			printf("Rectangle\n");
			break;

		case 5:
			printf("Pentagon\n");
			break;
		default:
			printf("Poligon of %d sides\n",poligon->sides);	
	}
	do{
		printf("x: %f, y: %f \n",iterator->x, iterator->y );
		iterator = iterator->next;
	}while(iterator != poligon->start);
}

edge_p createEdge(point_p p1, point_p p2){
	edge_p new_edge = (edge_p) malloc(sizeof(edge));
	if (p2->x == p1->x ){
		new_edge->m = MAX;
		new_edge->b = p1->x;
		//printf("%f %f\n",p1->x, p2->x );
		new_edge->vertical = true;
	}
	else{
		new_edge->vertical = false;
		new_edge->m = (float) (p2->y - p1->y) / ((p2->x - p1->x));
		new_edge->b = (-new_edge->m * p1->x) + p1->y;
	}
    //printf("%f - %f / ");
	//printf("%f - %f / %f - %f\n", p2->y, p1->y, p2->x, p1->x);
	//printf("m = %f, b = %f\n",new_edge->m, new_edge->b);
    new_edge->limit_a = p1;
	new_edge->limit_b = p2;

	return new_edge;
}

// FIX going between figures
// create vertex->before and if the node is in the same poligon, only check it if its next or before

// NEEDS FIX
// if a node from other figure is exactly at the same point of one of the oposite sides of a figure, it will pass throught the figure
// requeue after changing a value
// check why 0,4 can't go to 1,9


void readFile(string file){
	ifstream read;
	read.open(file);
	string output;
	if (read.is_open())
	{
		point_p start_point = (point_p) malloc(sizeof(point));
		point_p finish_point = (point_p) malloc(sizeof(point));
		read >> output;
		start_point->x = stoi(output);
		read >> output;
		start_point->y = stoi(output);
		read >> output;
		finish_point->x = stoi(output);
		read >> output;
		finish_point->y = stoi(output);read >> output;

		start =(node_p) malloc(sizeof(node));
		start->weight = 0;
		start->point = start_point;


		finish =(node_p) malloc(sizeof(node));
		finish->weight = MAX;
		finish->point = finish_point;
		finish->heuristic = 0;

		closed_list.push_front(finish);

		int figures = stoi(output);
		for (int i = 0; i < figures; ++i)
		{
			read >> output;
			poligon_p new_pol = (poligon_p) malloc(sizeof(poligon));
			new_pol->sides = stoi(output);
			point_p vertex_before = NULL;
			point_p first_point; 
			for (int i = 0; i < new_pol->sides; ++i)
			{
				read >> output;
				point_p vertex = (point_p) malloc(sizeof(point));
				vertex->x = stoi(output);
				read >> output;
				vertex->y = stoi(output);

				if (i == 0)
				{
					new_pol->start = vertex;
					first_point = vertex;
				}
				else{
				 	edges_list.push_back(createEdge(vertex_before, vertex));
					vertex_before->next = vertex;
					vertex->last = vertex_before;
				}

				if(i == new_pol->sides - 1){
					vertex->next = first_point;
					first_point->last = vertex;
					edges_list.push_back(createEdge(vertex, first_point));
				}
				vertex_before = vertex;
			

				node_p newNode =(node_p) malloc(sizeof(node));
				newNode->point = vertex;
				closed_list.push_back(newNode);
			}
			poligon_list.push_back(new_pol);
		}

		for(list<poligon_p>::iterator i = poligon_list.begin(); i!= poligon_list.end(); i++){
        	printPoligon((*i));
        	printf("\n");
    	}

	}
	read.close();
}

float getDistance(node_p n1, node_p n2){
	float result = 0;
	int x1, y1, x2, y2;
	x1 = n1->point->x;
	x2 = n2->point->x;
	y1 = n1->point->y;
	y2 = n2->point->y;
	result = sqrt(pow((x2-x1),2) + pow((y2-y1),2));
	return result;
}

bool isInFigure(point_p p1, point_p p2){
	//printf("is in figure\n");
	if(p1->next == NULL){
		//printf("Estoy en start o finish\n");
		return false;
	}
	for (point_p i = p1; i->next != p1; i=i->next)
	{
		//printf("aaa %d, %d\n",i->x,i->y);
		if (i == p2)
			return true;
	}

	return false;
}

bool isValid(node_p n1, node_p n2){
	if (isInFigure(n1->point, n2->point) && (n2->point != n1->point->next || n2->point != n1->point->last)){
		return false;
	}
	edge_p route = createEdge(n1->point, n2->point);
	bool res = true;
	for(list<edge_p>::iterator i = edges_list.begin(); i!= edges_list.end(); i++){
        if ((*i)->m == route->m){
        	//CHIDO
        }
        else{
        	float colX, colY;
        	if ((*i)->vertical == true){
        		colX = (*i)->b;
        		colY = (colX * route->m) + route->b;
        		//printf("guardada vertical\n");
        	}
        	else if(route->vertical == true){
        		colX = route->b;
        		colY = (colX * (*i)->m) + (*i)->b;
        		//printf("calculada vertical %f\n", route->vertical);
        	}
        	else{
        		colX = (float) ((*i)->b - route->b) / (route->m - (*i)->m);
        		colY = (colX * (*i)->m) + (*i)->b;
        		//printf("normal\n");
        	}
        	//printf("%f, %f to %f, %f crosses %f, %f to %f, %f in %f, %f\n", (*i)->limit_a->x,(*i)->limit_a->y,(*i)->limit_b->x,(*i)->limit_b->y,  route->limit_a->x,route->limit_a->y,route->limit_b->x,route->limit_b->y, colX, colY);
        	//printf("y = %fx + %f, y = %fx + %f \n",(*i)->b,(*i)->m, route->b, route->m);
        	if ((colX != route->limit_b->x && colY != route->limit_b->y) && (colX != route->limit_a->x && colY != route->limit_a->y)){
        		//printf("%f, %f to %f, %f crosses %f, %f to %f, %f in %f, %f\n", route->limit_a->x,route->limit_a->y,route->limit_b->x,route->limit_b->y, (*i)->limit_a->x,(*i)->limit_a->y,(*i)->limit_b->x,(*i)->limit_b->y, colX, colY);
        		//printf("choque esquina\n");
        		//printf("%f,%f - %f,%f\n",colX,colY,(float) route->limit_b->x, (float)route->limit_b->y);
        		if (between(colX, route->limit_a->x, route->limit_b->x) && between(colX, (*i)->limit_a->x, (*i)->limit_b->x))
        			res = false; 
        	}

        	if (res == false){
        		printf("%f, %f to %f, %f fails because it crashes %f, %f to %f, %f in %f, %f\n", 
        			route->limit_a->x,route->limit_a->y,route->limit_b->x,route->limit_b->y, 
        			(*i)->limit_a->x,(*i)->limit_a->y,(*i)->limit_b->x,(*i)->limit_b->y, 
        			colX, colY);
        		break;
        	}
        }
    }
    return res;
}

void open(node_p parent){
	printf("\n\nEXPLORATION OF NODE %f,%f WITH WEIGHT %f\n",parent->point->x, parent->point->y, parent->weight + parent->heuristic );
	printFringe();
	for(list<node_p>::iterator i = fringe_list.begin(); i!= fringe_list.end(); i++){
        if (isValid(parent, (*i)) && (*i)->weight > parent->weight + getDistance((*i), parent)){
        	(*i)->weight = parent->weight + getDistance((*i), parent);
        	fringe_list.sort(compareWeight);
        }
    }
    printClosed();
    list<node_p> toRemove;
   	for(list<node_p>::iterator i = closed_list.begin(); i!= closed_list.end(); i++){
   		if (isValid(parent, (*i))){
   			(*i)->parent = parent;
        	(*i)->weight = parent->weight + getDistance(parent, (*i));
        	(*i)->heuristic = getDistance((*i), finish);

        	printf("Parent is %f, %f \n", parent->point->x, parent->point->y);
   			printf("Child  is %f, %f  with %f\n", (*i)->point->x, (*i)->point->y, (*i)->weight + (*i)->heuristic);

        	//fringe.push((*i));
        	fringe_list.push_back((*i));
        	fringe_list.sort(compareWeight);
        	toRemove.push_front((*i));
        }
    }
    for(list<node_p>::iterator i = toRemove.begin(); i!= toRemove.end(); i++){
    	closed_list.remove((*i));
    }
}

int main(int argc, char const *argv[])
{
	readFile(argv[1]);

	//fringe.push(start);
	fringe_list.push_back(start);
	while (fringe_list.front() != finish){
		open(fringe_list.front());
		open_list.push_front(fringe_list.front());
		fringe_list.remove(fringe_list.front());
	}

	printf("\n\n RESULT\n");
	for (node_p i = fringe_list.front(); i != NULL; i = i->parent){
		printf("%f, %f -> %f\n", i->point->x, i->point->y, i->weight);
		printf("------------------------------\n");
	}


	/*for (int i = 0; i < 10; ++i)
	{
		node_p temp =(node_p) malloc(sizeof(node));
		temp->weight = i;
		fringe.push(temp);
	}
	node_p temp =(node_p) malloc(sizeof(node));
	temp->weight = -3;
	fringe.push(temp);
	for (int i = 0; i < 11; ++i)
	{
		printf("%f\n",fringe.top()->weight);
		fringe.p|op();
	}*/
	
	return 0;
}

