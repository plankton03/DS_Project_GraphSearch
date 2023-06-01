#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define HASH_SIZE 3


/**
 * @author : Fatemeh Abdi
 * @ID : 9931089
 */


//structs
struct node {
    int vertex;
    bool isPerson;
    bool isVisited;
    struct edge *edges;
    struct hashDijList *dijList;
    struct node *next;
};

struct dist {
    struct node *node;
    double distance;
    struct dist *next;
};

struct edge {
    struct node *vertex;
    int wight;
    struct edge *next;
};

struct hashDijList {
    struct dist **distances;
};

struct hashList {
    struct node **nodes;
};

struct Graph {
    int numVertices;
    int numOfPeople;
    int *people;
    struct hashList adjLists;
};


//functions
struct hashDijList *createHashDijList();

struct dist *createDistNode(struct node *node, double distance);

struct edge *createEdge(struct node *v, int weight);

struct node *createNode(int v, bool isPerson);

struct hashList *createHashList();

struct Graph *createAGraph(int vertices);

int hashFunction(int x);

void initializeHashList(struct hashList *hashList, int v, bool isPerson);

void addDistNode(struct hashDijList *hashDijList, struct dist *dist);

struct dist *searchDistNode(struct hashDijList hashDijList, int v);

int findMinDistP(struct dist *backup[], struct Graph *graph);

void addToBackup(struct dist *backup[], struct Graph *graph, struct dist *dist, int *size);

void dijkstra(struct node *src, struct Graph *graph);

struct node *searchVertex(struct hashList hashList, int v);

void addEdge(struct Graph *graph, int sV, int dV, int weight);

void printGraph(struct Graph *graph);

void setVisited(struct Graph *graph, bool v);

struct node *getFirstNode(struct Graph *graph);

void dfsUtil(struct node *node, struct Graph *graph);

void dfs(struct Graph *graph, int vertex);

void doDijkstra(struct Graph *graph);

int absolute(int x);

double calculateAvrDistance(struct Graph *graph, struct node *node);

void printBestLocations(struct dist *allDist, int index);

void calculateBestLocation(struct Graph *graph);

void joinPerson(struct Graph *graph, int v);

void leftPerson(struct Graph *graph, int v);

void getAndExecuteCommand(struct Graph *graph);

struct Graph *initializeGraph();


int main() {

    struct Graph *graph = initializeGraph();

//    printGraph(graph);

    getAndExecuteCommand(graph);

    return 0;
}


struct hashDijList *createHashDijList() {
    struct hashDijList *hashDijList = (struct hashDijList *) malloc(sizeof(struct hashDijList));
    hashDijList->distances = malloc(HASH_SIZE * sizeof(struct dist *));

    for (int i = 0; i < HASH_SIZE; ++i) {
        hashDijList->distances[i] = NULL;
    }
    return hashDijList;
}

struct dist *createDistNode(struct node *node, double distance) {
    struct dist *dist = malloc(sizeof(struct dist));
    dist->node = node;
    dist->next = NULL;
    dist->distance = distance;
    return dist;
}

struct edge *createEdge(struct node *v, int weight) {
    struct edge *edge = malloc(sizeof(struct edge));
    edge->next = NULL;
    edge->vertex = v;
    edge->wight = weight;
    return edge;
}

struct node *createNode(int v, bool isPerson) {
    struct node *newNode = (struct node *) malloc(sizeof(struct node));
    newNode->vertex = v;
    newNode->isPerson = isPerson;
    newNode->edges = NULL;
    newNode->dijList = createHashDijList();
    newNode->next = NULL;
    return newNode;
}

struct hashList *createHashList() {
    struct hashList *hashList = (struct hashList *) malloc(sizeof(struct hashList));
    hashList->nodes = malloc(HASH_SIZE * sizeof(struct node *));

    for (int i = 0; i < HASH_SIZE; ++i) {
        hashList->nodes[i] = NULL;
    }

    return hashList;
}

struct Graph *createAGraph(int vertices) {
    struct Graph *graph = (struct Graph *) malloc(sizeof(struct Graph));
    graph->numVertices = vertices;
    graph->numOfPeople = 0;
    graph->people = malloc(graph->numVertices * sizeof(int));
    for (int i = 0; i < graph->numVertices; ++i) {
        graph->people[i] = -1;
    }
    graph->adjLists = *createHashList();

    return graph;
}

int hashFunction(int x) {
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = (x >> 16) ^ x;
    return (x % HASH_SIZE);
}

//int hashFunction(int x) {
//    return (x % HASH_SIZE);
//}


void initializeHashList(struct hashList *hashList, int v, bool isPerson) {
    struct node *newNode = createNode(v, isPerson);
    int index = hashFunction(v);

    newNode->next = hashList->nodes[index];
    hashList->nodes[index] = newNode;
}

void addDistNode(struct hashDijList *hashDijList, struct dist *dist) {
    int index = hashFunction(dist->node->vertex);
    dist->next = hashDijList->distances[index];
    hashDijList->distances[index] = dist;
}

struct dist *searchDistNode(struct hashDijList hashDijList, int v) {
    int index = hashFunction(v);
    struct dist *temp = hashDijList.distances[index];

    while (temp->node->vertex != v) {
        temp = temp->next;
    }
    return temp;
}

int findMinDistP(struct dist *backup[], struct Graph *graph) {
    int min = INT_MAX;
    int index = 0;
    for (int i = 0; i < graph->numVertices; ++i) {
        if (backup[i] == NULL)
            continue;
        if (backup[i]->distance < min) {
            index = i;
            min = (int) backup[i]->distance;
        }
    }
    return index;
}

void addToBackup(struct dist *backup[], struct Graph *graph, struct dist *dist, int *size) {
    for (int i = 0; i < graph->numVertices; ++i) {
        if (backup[i] != NULL)
            continue;
        backup[i] = dist;
        *size = *size + 1;
        break;
    }
}

void dijkstra(struct node *src, struct Graph *graph) {

    struct hashList hashList = graph->adjLists;

    struct dist *backup[graph->numVertices];
    int capOvV = 0;

    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL)
            continue;
        struct node *temp = hashList.nodes[i];

        while (temp != NULL) {
            struct dist *distNode = createDistNode(temp, INT_MAX);
            addDistNode(src->dijList, distNode);
            backup[capOvV] = distNode;
            capOvV++;
            temp = temp->next;
        }
    }

    struct dist *srcDist = searchDistNode(*(src->dijList), src->vertex);
    srcDist->distance = 0;

    while (capOvV > 0) {
        int index = findMinDistP(backup, graph);
        struct dist *u = backup[index];
        struct dist *v = NULL;
        backup[index] = NULL;
        capOvV--;

        struct edge *edge = u->node->edges;
        while (edge != NULL) {
            v = searchDistNode(*(src->dijList), edge->vertex->vertex);
            if (v->distance > u->distance + edge->wight) {
                v->distance = u->distance + edge->wight;
                addToBackup(backup, graph, v, &capOvV);
            }
            edge = edge->next;
        }
    }
}

struct node *searchVertex(struct hashList hashList, int v) {
    int index = hashFunction(v);
    struct node *temp = hashList.nodes[index];

    while (temp->vertex != v) {
        temp = temp->next;
    }

    return temp;
}

void addEdge(struct Graph *graph, int sV, int dV, int weight) {

    struct node *s = searchVertex(graph->adjLists, sV);
    struct node *d = searchVertex(graph->adjLists, dV);

    struct edge *edgeIncludeD = createEdge(d, weight);
    struct edge *edgeIncludeS = createEdge(s, weight);

    edgeIncludeD->next = s->edges;
    s->edges = edgeIncludeD;

    edgeIncludeS->next = d->edges;
    d->edges = edgeIncludeS;
}

void printGraph(struct Graph *graph) {

    struct hashList hashList = graph->adjLists;

    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL) {
            printf("\nNo Vertex in index : %d\n", i);
        } else {
            printf("-------------------------------------------------------"
                   "\n\n\nin index %d \n", i);
            struct node *node = hashList.nodes[i];

            while (node != NULL) {
                printf("\n\nVertex : %d", node->vertex);
                struct edge *edge = node->edges;
                while (edge != NULL) {
                    printf("\nEdge -> V : %d , W : %d ", edge->vertex->vertex, edge->wight);
                    edge = edge->next;
                }
                struct hashDijList *hashDijList = node->dijList;

                printf("\ndistances -> ");
                for (int j = 0; j < HASH_SIZE; ++j) {
                    if (hashDijList->distances[j] == NULL) {
                        continue;
                    }
                    struct dist *temp = hashDijList->distances[j];
                    while (temp != NULL) {
                        printf("V : %d , D : %d | ", temp->node->vertex, (int) temp->distance);
                        temp = temp->next;
                    }

                }
                printf("\n\n");

                node = node->next;
            }

            printf("\nend of index %d\n\n\n", i);
        }
    }
}

void setVisited(struct Graph *graph, bool v) {

    struct hashList hashList = graph->adjLists;
    struct node *node = NULL;
    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL)
            continue;

        node = hashList.nodes[i];

        while (node != NULL) {
            node->isVisited = v;
            node = node->next;
        }

    }
}

struct node *getFirstNode(struct Graph *graph) {

    struct hashList hashList = graph->adjLists;

    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL)
            continue;
        return hashList.nodes[i];
    }
}

void dfsUtil(struct node *node, struct Graph *graph) {

    node->isVisited = true;
    printf("%d ", node->vertex);

    struct edge *edge = node->edges;
    while (edge != NULL) {
        if (edge->vertex->isVisited == false)
            dfsUtil(edge->vertex, graph);
        edge = edge->next;
    }
}

void dfs(struct Graph *graph, int vertex) {

    setVisited(graph, false);
    struct node *firstNode = NULL;

    firstNode = searchVertex(graph->adjLists, vertex);

    printf("\n\nDFS Traversal : ");
    dfsUtil(firstNode, graph);

    printf("\n\n");
}

void doDijkstra(struct Graph *graph) {

    struct hashList hashList = graph->adjLists;

    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL) {
            continue;
        } else {
            struct node *node = hashList.nodes[i];

            while (node != NULL) {
                dijkstra(node, graph);
                node = node->next;
            }
        }
    }
}

int absolute(int x) {
    if (x > 0)
        return x;
    return -x;
}

double calculateAvrDistance(struct Graph *graph, struct node *node) {
    int distanceValue = 0;

    int disPer[graph->numOfPeople];
    int count = 0;

    for (int i = 0; i < graph->numVertices; ++i) {
        if (graph->people[i] != -1) {
            int v = graph->people[i];
            struct dist *dist = searchDistNode(*(node->dijList), v);
            disPer[count] = (int) dist->distance;
            count++;
            if (count == graph->numOfPeople)
                break;
        }
    }

    for (int j = 0; j < graph->numOfPeople; ++j) {
        for (int i = j + 1; i < graph->numOfPeople; ++i) {
            distanceValue += absolute(disPer[j] - disPer[i]);
        }
    }

    double finalAns = 1.0 * distanceValue / graph->numOfPeople;
    return finalAns;
}

void printBestLocations(struct dist *allDist, int index) {
    double min = INT_MAX;
    for (int i = 0; i < index; ++i) {
        if (allDist[i].distance < min) {
            min = allDist[i].distance;
        }
    }


    int numOfMin = 0;
    printf("\nBest locations ->  ");
    for (int i = 0; i < index; ++i) {
        if (allDist[i].distance == min) {
            printf("number %d.  %d    ", ++numOfMin, allDist[i].node->vertex);
        }
    }
//    printf("   |  mid distance : %f ",min);
    printf("\n\n");
}

void calculateBestLocation(struct Graph *graph) {

    struct dist *allDist = malloc(graph->numVertices * sizeof(struct dist));
    int indexOfAllDist = 0;

    struct hashList hashList = graph->adjLists;

    for (int i = 0; i < HASH_SIZE; ++i) {
        if (hashList.nodes[i] == NULL) {
            continue;
        } else {
            struct node *node = hashList.nodes[i];
            while (node != NULL) {
                if (node->isPerson) {
                    node = node->next;
                } else {
                    struct dist *newDistance = createDistNode(node, calculateAvrDistance(graph, node));
                    allDist[indexOfAllDist] = *newDistance;
                    indexOfAllDist++;
                    node = node->next;
                }
            }
        }
    }

    printBestLocations(allDist, indexOfAllDist);
}

void joinPerson(struct Graph *graph, int v) {

    struct node *node = searchVertex(graph->adjLists, v);
    node->isPerson = true;

    //update in graph list

    for (int i = 0; i < graph->numVertices; ++i) {
        if (graph->people[i] == -1) {
            graph->people[i] = v;
            graph->numOfPeople++;
            break;
        }
    }

    if (graph->numOfPeople >= 2)
        calculateBestLocation(graph);
}

void leftPerson(struct Graph *graph, int v) {
    struct node *node = searchVertex(graph->adjLists, v);
    node->isPerson = false;

    for (int i = 0; i < graph->numVertices; ++i) {
        if (graph->people[i] == v) {
            graph->people[i] = -1;
            graph->numOfPeople--;
            break;
        }
    }

    if (graph->numOfPeople >= 2)
        calculateBestLocation(graph);
}

void getAndExecuteCommand(struct Graph *graph) {

    scanf("\n");

    char *command = (char *) malloc(25 * sizeof(char));

    while (true) {
        gets(command);
        if (strcmp(command, "exit") == 0) {
            printf("\n\nGOOD BYE :D\n\n");
            break;
        }
        if (strcmp(command, "test") == 0) {
            dfs(graph, getFirstNode(graph)->vertex);
        } else {
            char *token = strtok(command, " ");
            int number = atoi(strtok(NULL, " "));

            if (strcmp(token, "join") == 0) {
                joinPerson(graph, number);
            } else if (strcmp(token, "left") == 0) {
                leftPerson(graph, number);
            }
        }
    }
}

struct Graph *initializeGraph() {

    int V, E;
    scanf("%d %d", &V, &E);

    int nodeNum;
    struct Graph *graph = createAGraph(V);
    for (int i = 0; i < V; ++i) {
        scanf("%d", &nodeNum);
        initializeHashList(&graph->adjLists, nodeNum, false);
    }

    int sV, dV, weight;
    for (int j = 0; j < E; ++j) {
        scanf("%d %d %d", &sV, &dV, &weight);
        addEdge(graph, sV, dV, weight);
    }
    doDijkstra(graph);
    return graph;
}