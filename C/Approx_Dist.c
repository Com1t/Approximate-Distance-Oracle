#include <stdio.h>
#include <stdlib.h>
#include <limits.h> 
#include <math.h>

// typical node in the graph records the nearby sample node and distance 
typedef struct real_node {
	int nearby_sample;
	int dist_nearby;
} node;
// an ordinary node with val, and pointer to the next
typedef struct intNode {
	int node1;
	int node2;
	int dist;
	struct intNode *next;
} node_int;
// headerNode is a special node, in order to store the size of list 
typedef struct intHeaderNode {
	int size;
	struct intNode *first;
} int_headerNode;
// push will push an element to the tail of the list
void int_push(int_headerNode *head, int node1, int node2, int dist);
// pop will remove the last node of the list and return the value of last node
void int_pop(int_headerNode *head);
// clear, clears whole list
void int_clear(int_headerNode *head);
// to access the list like an array
node_int* int_get(const int_headerNode *head, int key);
// hash function
int hash_func(int node1, int node2, int pri, int table_size);
// find if the specific route is inside the table
int route_find(int startNode, int destNode, int_headerNode* route_table_i);
/*-------------------------------dijstra algorithm------------------------------------*/
int choose(int distance[], int n, int checked[]);
// map is the initial state of graph, sampling = 1 sample node, sampling = 0 normal node
void shortest_route(int **map, int startNode, int nodeNumbers, node* all_node, int_headerNode* route_table, int table_size, int pri, int sampling);

int main() {
	int nodeNumbers, linkNumbers, pri;
	scanf("%d %d %d", &nodeNumbers, &linkNumbers, &pri);

	node* all_node = (node*)malloc(sizeof(node)*nodeNumbers);	// all nodes in the graph
	int** addrDirectory = (int**)malloc(sizeof(int*)*nodeNumbers);
	for (int i = 0; i < nodeNumbers; i++)
		addrDirectory[i] = (int*)calloc(nodeNumbers, sizeof(int));

	int table_size = ceil(pow(nodeNumbers, 1.5));	// claculate table size
	int_headerNode* route_table = (int_headerNode*)malloc(sizeof(int_headerNode)*table_size);
	for (int i = 0; i < table_size; i++) {
		route_table[i].size = 0;
		route_table[i].first = NULL;
	}

	for (int i = 0; i < linkNumbers; i++) {
		int linkID, firstNodeID, secondNodeID, linkDist;
		scanf("%d %d %d %d", &linkID, &firstNodeID, &secondNodeID, &linkDist);
		addrDirectory[firstNodeID][secondNodeID] = linkDist;
		addrDirectory[secondNodeID][firstNodeID] = linkDist;
	}

	int sample_needs = ceil(sqrt(nodeNumbers));
	shortest_route(addrDirectory, 0, nodeNumbers, all_node, route_table, table_size, pri, 1);
	for (int i = 1; i < sample_needs; i++) {
		int temp_lonestIdx = 0;
		int lonest_dist = 0;
		// finding the lonest
		for (int j = 0; j < nodeNumbers; j++) {
			if (all_node[j].dist_nearby == 0) {
				continue;	// avoid check sample node again
			}
			else if (lonest_dist < all_node[j].dist_nearby) {
				lonest_dist = all_node[j].dist_nearby;
				temp_lonestIdx = j;
			}
		}
		shortest_route(addrDirectory, temp_lonestIdx, nodeNumbers, all_node, route_table, table_size, pri, 1);
	}
	for (int i = 0; i < nodeNumbers; i++) {
		if (all_node[i].dist_nearby == 0)
			continue;	// avoid record sample node to smaple node distance again
		else
			shortest_route(addrDirectory, i, nodeNumbers, all_node, route_table, table_size, pri, 0);
	}

	for (int i = 0; i < nodeNumbers; i++) {
		if (all_node[i].dist_nearby == 0)
			continue;	// avoid record starts with sample node again
		for (int j = 0; j < nodeNumbers; j++) {
			if (all_node[j].dist_nearby == 0)
				continue;	// avoid record node to smaple node again
			if (addrDirectory[i][j] <= all_node[i].dist_nearby && addrDirectory[i][j] != 0) {
				int_push(&route_table[hash_func(i, j, pri, table_size)], i, j, addrDirectory[i][j]);// if j is not sample node and in the circle, record this route
				addrDirectory[j][i] = 0;	// avoid records reverse direction again
			}
		}
	}
	int pairs = 0;
	for (int i = 0; i < table_size; i++)
		pairs += route_table[i].size;
	printf("%d\n", pairs);

	int queryPairs = 0;
	scanf("%d", &queryPairs);
	for (int i = 0; i < queryPairs; i++) {
		int pairID, startNode, destNode;
		scanf("%d %d %d", &pairID, &startNode, &destNode);
		int dirctDist = route_find(startNode, destNode, &route_table[hash_func(startNode, destNode, pri, table_size)]);
		if (dirctDist == 0) {
			int disS = 0, disD = 0;
			disS = route_find(startNode, all_node[destNode].nearby_sample, &route_table[hash_func(startNode, all_node[destNode].nearby_sample, pri, table_size)]) + all_node[destNode].dist_nearby;
			disD = route_find(destNode, all_node[startNode].nearby_sample, &route_table[hash_func(destNode, all_node[startNode].nearby_sample, pri, table_size)]) + all_node[startNode].dist_nearby;
			if (disS < disD)
				printf("%d %d %d %d\n", i, startNode, destNode, disS);
			else
				printf("%d %d %d %d\n", i, startNode, destNode, disD);
		}
		else {
			printf("%d %d %d %d\n", i, startNode, destNode, dirctDist);
		}
	}

	for (int i = 0; i < nodeNumbers; i++)
		free(addrDirectory[i]);
	free(addrDirectory);
	free(all_node);
	for (int i = 0; i < table_size; i++)
		int_clear(&route_table[i]);
	free(route_table);

	return 0;
}

// push will push an element to the tail of the list
void int_push(int_headerNode *head, int node1, int node2, int dist) {
	if (head->size == 0) {	// size 0 need another way to do since headerNode only contains a pointer to node
		head->first = (node_int *)malloc(sizeof(node_int));

		head->first->node1 = node1;
		head->first->node2 = node2;
		head->first->dist = dist;

		head->first->next = NULL;
	}
	else {
		node_int *current = head->first;
		while (current->next != NULL) {
			current = current->next;	// finding the tail
		}
		current->next = (node_int *)malloc(sizeof(node_int));

		current->next->node1 = node1;
		current->next->node2 = node2;
		current->next->dist = dist;

		current->next->next = NULL;
	}
	head->size++;
}
// pop will remove the last node of the list and return the value of last node
void int_pop(int_headerNode *head) {
	if (head->size == 1) {	// size 1 need another way to do since headerNode only contains a pointer to node
		free(head->first);
		head->first = NULL;
	}
	else if (head->size > 1) {
		node_int *current = head->first;
		while (current->next->next != NULL) {
			current = current->next;	// finding the element before tail
		}
		free(current->next);
		current->next = NULL;
	}
	head->size--;
}
// clear, clears whole list
void int_clear(int_headerNode *head) {
	if (head->size == 0)
		return;
	node_int *forward = head->first;
	node_int *current = head->first;
	while (current->next != NULL) {
		forward = current;
		current = current->next;	// go through the next one
		free(forward);
	}
	free(current);
	head->size = 0;
}
// to access the list like an array
node_int* int_get(const int_headerNode *head, int key) {
	node_int *current = head->first;
	for (int i = 0; i < key; i++)
		current = current->next;
	return current;
}
// hash function
int hash_func(int node1, int node2, int pri, int table_size) {
	return (node1 + 1) * (node2 + 1) % pri % table_size;
}
// find if the specific route is inside the table
int route_find(int startNode, int destNode, int_headerNode* route_table_i) {
	if (route_table_i->size > 0) {
		node_int *current = route_table_i->first;
		while (current->next != NULL) {
			if (current->node1 == startNode || current->node1 == destNode) {
				if (current->node2 == startNode || current->node2 == destNode) {
					return current->dist;
				}
			}
			current = current->next;	// go through the next one
		}
		if (current->node1 == startNode || current->node1 == destNode) {
			if (current->node2 == startNode || current->node2 == destNode) {
				return current->dist;
			}
		}
	}
	return 0;
}
/*-------------------------------dijstra algorithm------------------------------------*/
int choose(int distance[], int n, int checked[]) {
	int min = INT_MAX, minIdx = -1;	// to the minimum index, and haven't checked
	for (int i = 0; i < n; i++)
		if (distance[i] < min && !checked[i]) {
			min = distance[i];
			minIdx = i;
		}
	return minIdx;
}
// map is the initial state of graph, sampling = 1 sample node, sampling = 0 normal node
void shortest_route(int **map, int startNode, int nodeNumbers, node* all_node, int_headerNode* route_table, int table_size, int pri, int sampling) {
	int* checked = (int*)calloc(nodeNumbers, sizeof(int));
	for (int i = 0; i < nodeNumbers; i++) {
		if (map[startNode][i] == 0)
			map[startNode][i] = INT_MAX;
	}

	checked[startNode] = 1;
	map[startNode][startNode] = 0;

	for (int i = 0; i < nodeNumbers - 2; i++) {
		int minIdx = choose(map[startNode], nodeNumbers, checked);
		checked[minIdx] = 1;
		for (int j = 0; j < nodeNumbers; j++) {
			if (!checked[j] && map[startNode][minIdx] + map[minIdx][j] <= map[startNode][j] && map[minIdx][j]) {
				// the condition "map[minIdx][j]" is in order to avoid the path that doesn't exist
				map[startNode][j] = map[startNode][minIdx] + map[minIdx][j];	// if the indirect route has the smaller distance, store it
			}
		}
	}

	// records nearby sample node and distance in between of minimum distance
	if(sampling == 1){
		if (startNode == 0) {	// because the initial state of node doesn't have to compare anything
			for (int i = 0; i < nodeNumbers; i++) {
				all_node[i].dist_nearby = map[startNode][i];
				all_node[i].nearby_sample = startNode;
				if (startNode != i)
					int_push(&route_table[hash_func(startNode, i, pri, table_size)], startNode, i, map[startNode][i]);
			}
		}
		else {
			for (int i = 0; i < nodeNumbers; i++) {
				// if the distance is smaller or with smaller sample node
				if ((all_node[i].dist_nearby > map[startNode][i]) || (all_node[i].dist_nearby == map[startNode][i] && all_node[i].nearby_sample > startNode)) {
					all_node[i].dist_nearby = map[startNode][i];
					all_node[i].nearby_sample = startNode;
				}
				if (i != startNode && all_node[i].nearby_sample == i)
					continue;	// avoid record sample node to smaple node distance again
				if (startNode != i)
					int_push(&route_table[hash_func(startNode, i, pri, table_size)], startNode, i, map[startNode][i]);
			}
		}
	}
}

