#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char id;
    struct Node** neighbors;  // D���m�n kom�ular�n� tutan dizi
    int neighborCount;
    struct Node* next;
    struct Node* nextQueue;
    int visited;
    struct Node* prev;
    int distance;
} Node;

typedef struct Edge {
    Node* start;
    Node* end;
    int visitCount;
} Edge;

typedef struct Graph {
    Node* nodes;
    Edge* edges;
    int nodeCount;
    int edgeCount;
} Graph;

typedef struct Queue {
    struct Node* front;
    struct Node* rear;
} Queue;

void enqueue(struct Queue* queue, struct Node* node);

struct Node* dequeue(struct Queue* queue);

int isQueueEmpty(Queue* queue);

int calcDistance(struct Node* neighbor);

void BFS(struct Node* startNode, Edge* edges, int totalEdges, struct Graph* graph);

int findEdgeIndex(Edge* edges, int totalEdges, Node* start, Node* end);


// calculateEdgeBetweenness fonksiyonu, BFS �a�r�lar�n� y�netir
void calculateEdgeBetweenness(struct Graph* graph, Edge* edges);

// D���m� kom�u listesinden kald�r�r
void removeNodeFromNeighbors(struct Node* source, struct Node* target);

// Kenar� listeden kald�r�r ve d���mlerin kom�u listelerini g�nceller
void removeEdgeWithMaxBetweenness(struct Graph* graph);

void detectCommunities(struct Graph* graph, int k, int t);

struct Node* findNodeById(struct Graph* graph, char id);

int addEdge(struct Graph* graph, struct Node* startNode, struct Node* endNode);

void initializeGraph(struct Graph* graph, char* filename);

int DFS(struct Node* node);

// DFS i�levi, bir d���m ve ba�lant�l� d���mleri yazd�r�r
int DFS_Print(struct Node* node, int communityNumber);

// Birbiriyle ba�lant�l� topluluklar� bulur ve yazd�r�r
int printConnectedCommunities(struct Graph* graph);

int main() {
	char* filename = "input1.txt";
    struct Graph socialNetwork;
    socialNetwork.nodes = NULL;
    socialNetwork.edges = NULL;
    socialNetwork.nodeCount = 0;
    socialNetwork.edgeCount = 0;
    int t, k;
    
    printf("Lutfen t degerini giriniz: ");
    scanf("%d", &t);
    printf("Lutfen k degerini giriniz: ");
    scanf("%d", &k);
    

    initializeGraph(&socialNetwork, filename);
    
    detectCommunities(&socialNetwork, k, t);
            
    printConnectedCommunities(&socialNetwork);

    return 0;
}

void enqueue(struct Queue* queue, struct Node* node) {
    if (queue->rear == NULL) {
        queue->rear = node;
        queue->front = queue->rear;
    } else {
        queue->rear->nextQueue=node;
        // Bu d���m� kuyru�un sonuna ekleyin
        queue->rear = node;
        queue->rear->nextQueue=NULL;
    }
}

struct Node* dequeue(struct Queue* queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    // Kuyruktan ��kar�lacak olan d���m� al�n
    struct Node* node = queue->front;

    // Kuyruktan ��karan d���m� kuyruktan kald�r�n
    queue->front = node->nextQueue;

    // E�er kuyruk bo�sa rear'� da NULL olarak g�ncelleyin
    if (node->nextQueue == NULL) {
        queue->rear = NULL;
    }
	node->nextQueue=NULL;
	
    return node;
}

int isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

//baslangic noduna olan uzakligi bulur
int calcDistance(struct Node* neighbor){
	struct Node* tmp = neighbor;
	int distance = 0;
	int i=0;
	while(tmp->prev != NULL){
		distance++;
		tmp = tmp->prev;
	}
	return distance;
}

void BFS(struct Node* startNode, Edge* edges, int totalEdges, struct Graph* graph){
	Queue queue;
    queue.front = NULL;
    queue.rear = NULL;

    // Ba�lang�� d���m�n� ziyaret edilmi� olarak i�aretle ve kuyru�a ekleyin
    startNode->visited = 1;
    enqueue(&queue, startNode);
    int i, j;
	startNode->prev = NULL;
    while (!isQueueEmpty(&queue)) {
        // Bir d���m� kuyruktan ��kar ve kom�ular�n� i�le
        struct Node* currentNode = dequeue(&queue);

        // Kom�ular� i�le
        for (i = 0; i < currentNode->neighborCount; ++i) {
            struct Node* neighbor = currentNode->neighbors[i];
            
            if(!neighbor->visited){
            	neighbor->prev = currentNode;
               	neighbor->visited = 1;
               	enqueue(&queue, neighbor);
               	
               	neighbor->distance = calcDistance(neighbor);
	            struct Node* tmp = neighbor;
	            printf("Shortest path from %c to %c: ",startNode->id, neighbor->id);
	            while(tmp->prev != NULL){
	              	printf("%c", tmp->id);
	               	int edgeIndex = findEdgeIndex(graph->edges, graph->edgeCount, tmp, tmp->prev);
	               	if (edgeIndex != -1) {
	                   	graph->edges[edgeIndex].visitCount++;
	               	}
	               	tmp = tmp->prev;
				}
				printf("%c", startNode->id);
				printf("\n");
			}else if(currentNode->distance + 1 <= neighbor->distance && neighbor->id != startNode->id){

		        struct Node* temp = currentNode;
		        printf("Shortest path from %c to %c: ",startNode->id, neighbor->id);
		        printf("%c", neighbor->id);
		        int edgeIndex = findEdgeIndex(graph->edges, graph->edgeCount, neighbor, currentNode);
	              	if (edgeIndex != -1) {
	                 	graph->edges[edgeIndex].visitCount++;
		           	}
	            while(temp->prev != NULL){
	              	printf("%c", temp->id);
	               	int edgeIndex = findEdgeIndex(graph->edges, graph->edgeCount, temp, temp->prev);
	              	if (edgeIndex != -1) {
	                 	graph->edges[edgeIndex].visitCount++;
		           	}
	               	temp = temp->prev;
				}
				printf("%c", startNode->id);
				printf("\n");
	                
	            
			}
			
			
        }
    }
}


int findEdgeIndex(Edge* edges, int totalEdges, Node* start, Node* end) {
	int i;
    for (i = 0; i < totalEdges; ++i) {
        if ((edges[i].start == start && edges[i].end == end) || 
            (edges[i].start == end && edges[i].end == start)) {
            return i;
        }
    }
    return -1;
}

void calculateEdgeBetweenness(struct Graph* graph, Edge* edges) {
    Node* current = graph->nodes;
    Node* tmp = graph->nodes;
    int totalEdges = graph->edgeCount, i;
    
    // Edgelerin visitCount de�erlerini s�f�rla
    for (i = 0; i < totalEdges; ++i) {
        graph->edges[i].visitCount = 0;
    }

    while (current != NULL) {
        // D���mlerin ziyaret edilmi� bayra��n� s�f�rlay�n
        current->visited = 0;
        current = current->next;
    }
    // Graf i�indeki her d���m i�in BFS'yi ger�ekle�tirin
    current = graph->nodes;

    while (current != NULL) {
    	tmp = graph->nodes;
    	while (tmp != NULL) {
	        // D���mlerin ziyaret edilmi� bayra��n� s�f�rlay�n
	        tmp->visited = 0;
	        tmp->prev = NULL;
	        tmp = tmp->next;
   		}
        BFS(current, edges, totalEdges, graph);
        current = current->next;
    }
}

// D���m� kom�u listesinden kald�r�r
void removeNodeFromNeighbors(struct Node* source, struct Node* target) {
    int i;
    for (i = 0; i < source->neighborCount; ++i) {
        if (source->neighbors[i]->id == target->id) {
            // D���m� kom�u listesinden kald�rma
            int j;
            for (j = i; j < source->neighborCount - 1; ++j) {
                source->neighbors[j] = source->neighbors[j + 1];
            }
            source->neighborCount--;
        }
    }
}

// Kenar� listeden kald�r�r ve d���mlerin kom�u listelerini g�nceller
void removeEdgeWithMaxBetweenness(struct Graph* graph) {
    if (graph->edgeCount == 0) {
        return;  // Grafi�in kenar� yoksa i�lem yapma
    }

    int maxIndex = 0;
    int maxCredit = graph->edges[0].visitCount;
    int i;
    for (i = 1; i < graph->edgeCount; ++i) {
        if (graph->edges[i].visitCount > maxCredit) {
            maxCredit = graph->edges[i].visitCount;
            maxIndex = i;
        }
    }
    for (i = 0; i < graph->edgeCount; i++){
    	if(graph->edges[i].visitCount == maxCredit){
    		maxIndex=i;
    		printf("Max= %d, Index= %d Kaldirilan: %c-%c\n", maxCredit, maxIndex, graph->edges[maxIndex].start->id, graph->edges[maxIndex].end->id);
    
		    // Se�ilen kenar� kald�rma i�lemi
		    struct Edge* removedEdge = &graph->edges[maxIndex];
		
		    // D���mlerin kom�uluk listesinden kald�rma i�lemi
		    removeNodeFromNeighbors(removedEdge->start, removedEdge->end);
		    removeNodeFromNeighbors(removedEdge->end, removedEdge->start);
		
		    // Yeni kenar listesini olu�tur
		    struct Edge* newEdges = (struct Edge*)malloc((graph->edgeCount - 1) * sizeof(struct Edge));
		
		    int newIndex = 0;
		    for (i = 0; i < graph->edgeCount; ++i) {
		        if (i != maxIndex) {
		            newEdges[newIndex] = graph->edges[i];
		            newIndex++;
		        }
		    }
		
		    free(graph->edges);
		    graph->edges = newEdges;
		    graph->edgeCount--;
		    i=0;
		}
	}
    
}

void detectCommunities(struct Graph* graph, int k, int t) {
    int i=0, j, m, flag=0;
    Edge* edges = graph->edges;
    int totalEdges = graph->edgeCount;
    // Edgelerin visitCount de�erlerini s�f�rla
    for (i = 0; i < totalEdges; ++i) {
        edges[i].visitCount = 0;
    }
    i=0;
    while(i<k && flag==0){
    	// Ara kesici de�erleri hesapla
        calculateEdgeBetweenness(graph, edges);
        
        printf("Edges:\n");
	    for (j = 0; j < graph->edgeCount; ++j) {
	        printf("%c-%c  count: %d\n", graph->edges[j].start->id, graph->edges[j].end->id, graph->edges[j].visitCount);
	    }
	    
        // En y�ksek ara kesici de�ere sahip kenar� kald�r
        removeEdgeWithMaxBetweenness(graph);
        
        int minComponentSize = printConnectedCommunities(graph); // graph'� yaz
        
        // �stenen t de�erini kar��layan ba�l� bile�en var m� kontrol et
        if (minComponentSize <= t) {
            flag = 1;
            printf("Minimum uye sayisi t'ye esit veya kucuk cikis yapiliyor.\n");
        }
        // TODO: Topluluklar� tespit et ve gerekli durum kontrol� yap
        i++;	    
	}
	if(flag != 1){
		printf("k degeri maksimum iterasyon sayisina ulasti cikis yapiliyor.\n");
	}
}

struct Node* findNodeById(struct Graph* graph, char id) {
    struct Node* current = graph->nodes;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int addEdge(struct Graph* graph, struct Node* startNode, struct Node* endNode) {
    // Eklenecek kenar�n tersi zaten graf �zerinde var m� kontrol et
    int i;
    for (i = 0; i < graph->edgeCount; ++i) {
        if ((graph->edges[i].start == startNode && graph->edges[i].end == endNode) ||
            (graph->edges[i].start == endNode && graph->edges[i].end == startNode)) {
            // Kenar�n tersi zaten var, eklemeyi yapma
            return 0;  // Ba�ar�s�zl�k durumunu belirtmek ad�na 0 d�nd�r�lebilir
        }
    }

    // Yeni kenar� olu�tur
    struct Edge newEdge;
    newEdge.start = startNode;
    newEdge.end = endNode;
    newEdge.visitCount=0;

    // Kenar� kenar listesine ekle
    graph->edges = (struct Edge*)realloc(graph->edges, (graph->edgeCount + 1) * sizeof(struct Edge));
    graph->edges[graph->edgeCount] = newEdge;
    graph->edgeCount++;

    return 1;  // Ba�ar� durumunu belirtmek ad�na 1 d�nd�r�lebilir
}

void initializeGraph(struct Graph* graph, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Dosya a��lamad�.");
        exit(EXIT_FAILURE);
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        char* newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        char startNodeId = line[0];
        char* endNodeIds = line + 2;

        struct Node* startNode = findNodeById(graph, startNodeId);
        if (startNode == NULL) {
            startNode = (struct Node*)malloc(sizeof(struct Node));
            startNode->id = startNodeId;
            startNode->neighbors = NULL;  // �lk ba�ta kom�u yok
            startNode->neighborCount = 0;
            startNode->visited = 0;
            startNode->next = graph->nodes;
            startNode->nextQueue = NULL;
            startNode->prev = NULL;
            startNode->distance=0;
            graph->nodes = startNode;
            graph->nodeCount++;
        }

        while (*endNodeIds != '\0') {
            char endNodeId = *endNodeIds;

            struct Node* endNode = findNodeById(graph, endNodeId);
            if (endNode == NULL) {
                endNode = (struct Node*)malloc(sizeof(struct Node));
                endNode->id = endNodeId;
                endNode->neighbors = NULL;  // �lk ba�ta kom�u yok
                endNode->neighborCount = 0;
                endNode->visited = 0;
                endNode->next = graph->nodes;
                endNode->nextQueue = NULL;
                endNode->prev = NULL;
                endNode->distance=0;
                graph->nodes = endNode;
                graph->nodeCount++;
            }

            // Kom�u d���mleri g�ncelle
            startNode->neighbors = (struct Node**)realloc(startNode->neighbors, (startNode->neighborCount + 1) * sizeof(struct Node*));
            startNode->neighbors[startNode->neighborCount] = endNode;
            startNode->neighborCount++;
            
            addEdge(graph, startNode, endNode);

            endNodeIds += 2;
        }
    }

    fclose(file);
}

int DFS(struct Node* node) {
	int j=1;
    printf("%c ", node->id);
    node->visited = 1;
	int i;
    for (i = 0; i < node->neighborCount; ++i) {
        if (!node->neighbors[i]->visited) {
            j+=DFS(node->neighbors[i]);
        }
    }
    return j;
}

// DFS i�levi, bir d���m ve ba�lant�l� d���mleri yazd�r�r
int DFS_Print(struct Node* node, int communityNumber) {
	int i=0;
    printf("Topluluk %d: ", communityNumber);
    i = DFS(node);
    printf("\n");
    return i;
}

// Birbiriyle ba�lant�l� topluluklar� bulur ve yazd�r�r
int printConnectedCommunities(struct Graph* graph) {
    printf("Birbiriyle baglantili topluluklar:\n");
    
    Node* current = graph->nodes;

    while (current != NULL) {
        // D���mlerin ziyaret edilmi� bayra��n� s�f�rlay�n
        current->visited = 0;
        current = current->next;
    }

    int communityNumber = 1;
	int j=999, tmp;
    // Her d���m� gezerek DFS fonksiyonunu �a��r
    struct Node* currentNode = graph->nodes;
    while (currentNode != NULL) {
        if (!currentNode->visited) {
            tmp = DFS_Print(currentNode, communityNumber);
            if(tmp<j){
            	j=tmp;
			}
            communityNumber++;
        }
        currentNode = currentNode->next;
    }
    return j;
}
