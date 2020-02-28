#include <iostream>
#include <cmath>

using namespace std;

typedef struct real_node {
	int nearby_sample = 0;
	int dist_nearby = 0;
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
		current = current->next;	// finding the tail
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

int hash_func(int node1, int node2, int pri, int table_size) {
	return (node1 + 1) * (node2 + 1) % pri % table_size;
}

int route_find(int startNode, int destNode, int_headerNode* route_table_i) {
	if (route_table_i->size > 0) {
		node_int *current = route_table_i->first;
		while (current->next != NULL) {
			if (current->node1 == startNode || current->node1 == destNode) {
				if (current->node2 == startNode || current->node2 == destNode) {
					return current->dist;
				}
			}
			current = current->next;	// finding the tail
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
	int min = INT_MAX, minIdx = -1;	// to the minimum index
	for (int i = 0; i < n; i++)
		if (distance[i] < min && !checked[i]) {
			min = distance[i];
			minIdx = i;
		}
	return minIdx;
}

void shortest_route(int **map, int startNode, int nodeNumbers, node* all_node, int_headerNode* route_table, int table_size, int pri) {
	int* checked = new int[nodeNumbers]();
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
				map[startNode][j] = map[startNode][minIdx] + map[minIdx][j];
			}
		}
	}

	// records nearby sample node and distance in between of minimum distance
	if (startNode == 0) {
		for (int i = 0; i < nodeNumbers; i++) {
			all_node[i].dist_nearby = map[startNode][i];
			all_node[i].nearby_sample = startNode;
			int_push(&route_table[hash_func(startNode, i, pri, table_size)], startNode, i, map[startNode][i]);
		}
	}
	else {
		for (int i = 0; i < nodeNumbers; i++) {
			if ((all_node[i].dist_nearby > map[startNode][i]) || (all_node[i].dist_nearby == map[startNode][i] && all_node[i].nearby_sample > startNode)) {
				all_node[i].dist_nearby = map[startNode][i];
				all_node[i].nearby_sample = startNode;
			}
			if (i != startNode && all_node[i].nearby_sample == i)
				continue;	// avoid record sample node to smaple node distance again
			int_push(&route_table[hash_func(startNode, i, pri, table_size)], startNode, i, map[startNode][i]);
		}
	}
	cout << "Vertex\t\tDistance from Source\n";
	for (int i = 0; i < nodeNumbers; i++)
		cout << i << "\t\t" << map[startNode][i] << endl;
}

int main() {
	int nodeNumbers = 0, linkNumbers = 0, pri = 0;
	cin >> nodeNumbers >> linkNumbers >> pri;

	node* all_node = new node[nodeNumbers]();	// all nodes in the graph

	int table_size = ceil(pow(nodeNumbers, 1.5));	// claculate
	int_headerNode* route_table = new int_headerNode[table_size]();

	int** addrDirectory = new int*[nodeNumbers]();
	for (int i = 0; i < nodeNumbers; i++) {
		addrDirectory[i] = new int[nodeNumbers]();
		route_table[i].size = 0;
		route_table[i].first = NULL;
	}
	for (int i = 0; i < linkNumbers; i++) {
		int linkID, firstNodeID, secondNodeID, linkCapacity;
		cin >> linkID >> firstNodeID >> secondNodeID >> linkCapacity;
		addrDirectory[firstNodeID][secondNodeID] = linkCapacity;
		addrDirectory[secondNodeID][firstNodeID] = linkCapacity;
	}

	int sample_needs = ceil(sqrt(nodeNumbers));
	cout << sample_needs << endl;
	shortest_route(addrDirectory, 0, nodeNumbers, all_node, route_table, table_size, pri);
	for (int i = 1; i < sample_needs; i++) {
		int temp_lonestIdx = 0, lonest_dist = 0;
		for (int j = 0; j < nodeNumbers; j++) {
			if (all_node[j].dist_nearby == 0) {
				continue;	// avoid check sample node again
			}
			else if (lonest_dist < all_node[j].dist_nearby) {
				lonest_dist = all_node[j].dist_nearby;
				temp_lonestIdx = j;
			}
		}
		shortest_route(addrDirectory, temp_lonestIdx, nodeNumbers, all_node, route_table, table_size, pri);
	}
	
	for (int i = 0; i < nodeNumbers; i++) {
		for (int j = 0; j < nodeNumbers; j++) {
			if (all_node[j].dist_nearby > 0 && addrDirectory[i][j] < all_node[i].dist_nearby && addrDirectory[i][j] != 0) {
				int_push(&route_table[hash_func(i, j, pri, table_size)], i, j, addrDirectory[i][j]);// if j is not sample node and in the circle, record this route
				addrDirectory[j][i] = 0;	// avoid records reverse direction again
			}
		}
	}

	for (int j = 0; j < nodeNumbers; j++)
		cout << endl << j << " " << all_node[j].nearby_sample << " " << all_node[j].dist_nearby << " ";
	cout << endl;
	
	int a = 0, b = 0;
	cin >> a >> b;
	cout << route_find(a, b, &route_table[hash_func(a, b, pri, table_size)]) << endl;
	if (route_find(a, b, &route_table[hash_func(a, b, pri, table_size)]) == 0) {
		int disa = 0, disb = 0;
		disa = route_find(a, all_node[b].nearby_sample, &route_table[hash_func(a, all_node[b].nearby_sample, pri, table_size)]) + all_node[b].dist_nearby;
		disb = route_find(b, all_node[a].nearby_sample, &route_table[hash_func(b, all_node[a].nearby_sample, pri, table_size)]) + all_node[a].dist_nearby;
		if (disa < disb)
			cout << disa << endl;
		else
			cout << disb << endl;
	}
	for (int i = 0; i < table_size; i++) {
		cout << "TABLE key " << i << endl;
		for (int j = 0; j < route_table[i].size; j++) {
			cout << int_get(&route_table[i], j)->node1 << " "
				 << int_get(&route_table[i], j)->node2 << " "
				 << int_get(&route_table[i], j)->dist << endl;
		}
		int_clear(&route_table[i]);
		cout << endl;
	}

	for (int i = 0; i < nodeNumbers; i++)
		delete[] addrDirectory[i];
	delete[] addrDirectory;
	delete[] all_node;
	delete[] route_table;

	return 0;
}