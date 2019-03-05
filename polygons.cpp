#include <GL/glut.h>
#include <GL/gl.h>
#include "polygons.hpp"

using namespace std;

list<poligon_p> poligon_list;
list<node_p> open_list;
list<node_p> closed_list;
list<node_p> fringe_list;
list<edge_p> edges_list;
node_p start, finish;
float maxX = 0, maxY = 0;

bool compareWeight(node_p const &p1, node_p const &p2) {
    return p1->weight + p1->heuristic < p2->weight + p2->heuristic;
}

bool between(float value, float la, float lb) {
    if (la > lb) {
        return ((lb <= value) && (value <= la));
    } else {
        return ((la <= value) && (value <= lb));
    }
}

void printFringe() {
    printf("\n---- FRINGE ----\n");
    for (auto &i : fringe_list) {
        printf("%1.0f,%1.0f -> %f \n", i->point->x, i->point->y, i->weight + i->heuristic);
    }
}

void printClosed() {
    printf("\n---- CLOSED ----\n");
    for (auto &i : closed_list) {
        printf("%1.0f,%1.0f \n", i->point->x, i->point->y);
    }
}

void printPoligon(poligon_p poligon) {
    point_p iterator = poligon->start;
    switch (poligon->sides) {
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
            printf("Poligon of %d sides\n", poligon->sides);
    }
    do {
        printf("%1.0f,%1.0f \n", iterator->x, iterator->y);
        iterator = iterator->next;
    } while (iterator != poligon->start);
}

edge_p createEdge(point_p p1, point_p p2) {
    auto new_edge = (edge_p) malloc(sizeof(edge));
    if (p2->x == p1->x) {
        new_edge->m = MAX;
        new_edge->b = p1->x;
        new_edge->vertical = true;
    } else {
        new_edge->vertical = false;
        new_edge->m = (p2->y - p1->y) / ((p2->x - p1->x));
        new_edge->b = (-new_edge->m * p1->x) + p1->y;
    }
    new_edge->limit_a = p1;
    new_edge->limit_b = p2;

    return new_edge;
}

void readFile(const string &file) {
    ifstream read;
    read.open(file);
    string output;
    if (read.is_open()) {
        auto start_point = (point_p) malloc(sizeof(point));
        auto finish_point = (point_p) malloc(sizeof(point));
        read >> output;
        start_point->x = stoi(output);
        read >> output;
        start_point->y = stoi(output);
        read >> output;
        finish_point->x = stoi(output);
        read >> output;
        finish_point->y = stoi(output);
        read >> output;

        if (start_point->x > maxX)
            maxX = start_point->x;

        if (start_point->y > maxY)
            maxY = start_point->y;
        if (finish_point->x > maxX)
            maxX = finish_point->x;

        if (finish_point->y > maxY)
            maxY = finish_point->y;

        start = (node_p) malloc(sizeof(node));
        start->weight = 0;
        start->point = start_point;


        finish = (node_p) malloc(sizeof(node));
        finish->weight = MAX;
        finish->point = finish_point;
        finish->heuristic = 0;

        closed_list.push_front(finish);

        int figures = stoi(output);
        for (int i = 0; i < figures; ++i) {
            read >> output;
            auto new_pol = (poligon_p) malloc(sizeof(poligon));
            new_pol->sides = stoi(output);
            point_p vertex_before = nullptr;
            point_p first_point = nullptr;
            for (int j = 0; j < new_pol->sides; ++j) {
                read >> output;
                auto vertex = (point_p) malloc(sizeof(point));
                vertex->x = stoi(output);
                read >> output;
                vertex->y = stoi(output);

                if (vertex->x > maxX)
                    maxX = vertex->x;

                if (vertex->y > maxY)
                    maxY = vertex->y;

                if (j == 0) {
                    new_pol->start = vertex;
                    first_point = vertex;
                } else {
                    edges_list.push_back(createEdge(vertex_before, vertex));
                    vertex_before->next = vertex;
                    vertex->last = vertex_before;
                }

                if (j == new_pol->sides - 1) {
                    vertex->next = first_point;
                    first_point->last = vertex;
                    edges_list.push_back(createEdge(vertex, first_point));
                }
                vertex_before = vertex;


                auto newNode = (node_p) malloc(sizeof(node));
                newNode->point = vertex;
                closed_list.push_back(newNode);
            }
            poligon_list.push_back(new_pol);
        }

        for (auto &i : poligon_list) {
            printPoligon(i);
            printf("\n");
        }

    }
    read.close();
}

float getDistance(node_p n1, node_p n2) {
    double result = 0;
    float x1, y1, x2, y2;
    x1 = n1->point->x;
    x2 = n2->point->x;
    y1 = n1->point->y;
    y2 = n2->point->y;
    result = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
    return (float) result;
}

bool isInFigure(point_p p1, point_p p2) {
    if (p1->next == nullptr) {
        return false;
    }
    for (point_p i = p1; i->next != p1; i = i->next) {
        if (i == p2)
            return true;
    }

    return false;
}

bool isEqualToFigure(point_p p1, point_p p2) {
    if (p1->next == nullptr) {
        return false;
    }
    for (point_p i = p1; i->next != p1; i = i->next) {
        if (p2->x == i->x && p2->y == i->y)
            //if((p2->x != p1->next->x && p2->y != p1->next->y)  (p2->x != p1->last->x && p2->y != p1->last->y))
            return true;
    }
    return false;
}

bool isValid(node_p n1, node_p n2) {
    if (isInFigure(n1->point, n2->point) && (n2->point != n1->point->next && n2->point != n1->point->last)) {
        return false;
    }
    if (isEqualToFigure(n1->point, n2->point)) {
        return false;
    }
    if (isEqualToFigure(n2->point, n1->point)) {
        return false;
    }
    edge_p route = createEdge(n1->point, n2->point);
    bool res = true;
    for (auto &i : edges_list) {
        if (i->m == route->m) {
            //CHIDO
        } else {
            float colX, colY;
            if (i->vertical) {
                colX = i->b;
                colY = (colX * route->m) + route->b;
            } else if (route->vertical) {
                colX = route->b;
                colY = (colX * i->m) + i->b;
            } else {
                colX = (i->b - route->b) / (route->m - i->m);
                colY = (colX * i->m) + i->b;
            }
            if ((colX != route->limit_b->x && colY != route->limit_b->y) &&
                (colX != route->limit_a->x && colY != route->limit_a->y)) {
                if (between(colX, route->limit_a->x, route->limit_b->x) &&
                    between(colX, i->limit_a->x, i->limit_b->x) &&
                    between(colY, route->limit_a->y, route->limit_b->y) && between(colY, i->limit_a->y, i->limit_b->y))
                    res = false;
            }

            if (!res) {
                printf("%1.0f,%1.0f to %1.0f,%1.0f fails because it crashes %1.0f,%1.0f to %1.0f,%1.0f in %f, %f\n",
                       route->limit_a->x, route->limit_a->y, route->limit_b->x, route->limit_b->y,
                       i->limit_a->x, i->limit_a->y, i->limit_b->x, i->limit_b->y,
                       colX, colY);
                break;
            }
        }
    }
    return res;
}

void open(node_p parent) {
    printf("\n\nEXPLORATION OF NODE %1.0f,%1.0f WITH WEIGHT %f\n", parent->point->x, parent->point->y,
           parent->weight + parent->heuristic);
    printFringe();
    for (auto i = fringe_list.begin(); i != fringe_list.end(); i++) {
        if (isValid(parent, (*i)) && (*i)->weight > parent->weight + getDistance((*i), parent)) {
            (*i)->weight = parent->weight + getDistance((*i), parent);
            (*i)->parent = parent;
            fringe_list.sort(compareWeight);
        }
    }
    printClosed();
    list<node_p> toRemove;
    for (auto &i : closed_list) {
        if (isValid(parent, i)) {
            i->parent = parent;
            i->weight = parent->weight + getDistance(parent, i);
            i->heuristic = getDistance(i, finish);

            printf("Parent is %1.0f,%1.0f \n", parent->point->x, parent->point->y);
            printf("Child  is %1.0f,%1.0f  with %f\n", i->point->x, i->point->y, i->weight + i->heuristic);

            //fringe.push((*i));
            fringe_list.push_back(i);
            fringe_list.sort(compareWeight);
            toRemove.push_front(i);
        }
    }
    for (auto &i : toRemove) {
        closed_list.remove(i);
    }
}


void render() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLineWidth(1);
    glColor3f(0.8, 0.8, 0.8);

    for (int i = 0; i <= maxX; i++) {
        glBegin(GL_LINES);
        glVertex3f(i, 0, 0.0);
        glVertex3f(i, maxY + 1, 0.0);
        glEnd();
    }


    for (int i = 0; i <= maxY; i++) {
        glBegin(GL_LINES);
        glVertex3f(0, i, 0.0);
        glVertex3f(maxX + 1, i, 0.0);
        glEnd();
    }

    glLineWidth(1.5);
    glColor3f(1.0, 0.0, 0.0);


    for (auto &i : edges_list) {
        glBegin(GL_LINES);
        glVertex3f(i->limit_a->x, i->limit_a->y, 0.0);
        glVertex3f(i->limit_b->x, i->limit_b->y, 0.0);
        glEnd();
    }
    glLineWidth(2.5);
    glColor3f(0.0, 1.0, 0.0);
    for (node_p i = fringe_list.front(); i->parent != nullptr; i = i->parent) {
        glBegin(GL_LINES);
        glVertex3f(i->point->x, i->point->y, 0.0);
        glVertex3f(i->parent->point->x, i->parent->point->y, 0.0);
        glEnd();
    }

    glPointSize(10);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    glVertex3f(fringe_list.front()->point->x, fringe_list.front()->point->y, 0.0);
    glEnd();

    glBegin(GL_POINTS);
    glVertex3f(start->point->x, start->point->y, 0.0);
    glEnd();

    glutSwapBuffers();

}

int main(int argc, char **argv) {
    readFile(argv[1]);

    //fringe.push(start);
    fringe_list.push_back(start);
    while (fringe_list.front() != finish) {
        open(fringe_list.front());
        open_list.push_front(fringe_list.front());
        fringe_list.remove(fringe_list.front());
    }

    printf("\n\n RESULT\n");
    for (node_p i = fringe_list.front(); i != nullptr; i = i->parent) {
        printf("%1.0f,%1.0f -> %f\n", i->point->x, i->point->y, i->weight);
        printf("------------------------------\n");
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("square");

    glClearColor(0.0, 0.0, 0.0, 0.0);         // black background
    glMatrixMode(GL_PROJECTION);              // setup viewing projection
    glLoadIdentity();                           // start with identity matrix
    glOrtho(0.0, maxX + 1, 0.0, maxY + 1, -1.0, 1.0);   // setup a 10x10x2 viewing world

    glutDisplayFunc(render);

    glutMainLoop();

    return 0;
}

