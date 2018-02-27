#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct Source {
	int supply;
	int fout;    // Index to arc
};

struct Sink {
	float pd;
	float pi;
	int flow;
	int fin;     // Index to arc
};

struct Arc {
	int source;
	int sink;
	int flow;
	int nextin;
	int nextout;
};

struct HeapElement {
	int sink;
	float value;
};

// Globals: heap and its size
int n_sources;
int n_sinks;
int n_arcs;

struct Arc *Arcs;
struct Sink *Sinks;
struct Source *Sources;

struct HeapElement *garcs;        // Elements in heap array

struct HeapElement **heap_array;  // This is the heap
int n_elements;


/*
 * Initialize the heap
 */
void initialize_heap() {
	for (int i = 0; i < n_sinks; i++) {
		garcs[i].sink = i;
		garcs[i].value = Sinks[i].pd * Sinks[i].pi;
		heap_array[i] = &garcs[i];
	}
	n_elements = n_sinks;
}



void upHeap(int new_pos) {
	struct HeapElement *item = heap_array[new_pos];
	int parent = (new_pos - 1) / 2;
    // Loop while the item has not become the main root, 
    // and while its value is less than its parent
	while (new_pos != 0 && item->value > heap_array[parent]->value) {
		heap_array[new_pos] = heap_array[parent];
		new_pos = parent;
		parent = (new_pos - 1) / 2;
	}
	heap_array[new_pos] = item;
}


void downHeap(int root) {
	struct HeapElement *item = heap_array[root];
	if (n_elements == 1) return;  // No children
	int child = 2 * root + 1;
	do {
		if (child < n_elements - 1) {
			if (heap_array[child + 1]->value > heap_array[child]->value) {
                child++;
            }  // Pick smaller
		}
		if (heap_array[child]->value > item->value) { 
			heap_array[root] = heap_array[child];
			root = child;
			child = 2 * root + 1;
		}  // Swap
		else {
            break;
        }  // Found my place
	} while (child < n_elements);
	heap_array[root] = item;
}


struct HeapElement* remove_max() {
	if (n_elements == 0) {
		printf("empty heap error, can't delete \n");
	}
	n_elements--;
	struct HeapElement *temp = heap_array[0];
	if (n_elements != 0) { 
		heap_array[0] = heap_array[n_elements];
		downHeap(0);
	}  // If we didn't delete the root
	return temp;
}


void insert(struct HeapElement *entry) {
	heap_array[n_elements] = entry;
	n_elements++;
	upHeap(n_elements - 1);
}


void heapMake(){
	for (int i = n_elements / 2 - 1; i > -1; i--) {
		downHeap(i);
	}
}


int find_augmenting_source(int sink, int *pred, int *queue, int *visited) {
	int arc, arc1;
	int source;
	int sink1;
	int firstqueue;
	int lastqueue;

	firstqueue = 0;
	lastqueue = 0;
	memset(visited, 0, n_sources * sizeof(int));           // Block 0
    memset(pred, -1, (n_sources + n_sinks) * sizeof(int));  // Block -1

	// Scan all the sources connected to sink to look for supply
	arc = Sinks[sink].fin;
	while (arc >= 0) {
		source = Arcs[arc].source;
		pred[source] = arc;
		if (Sources[source].supply > 0) {
			return source;
		}
		else {
			visited[source] = 1;
			queue[lastqueue++] = source;  // Adding sources; sink indices will be shifted by n_sources
		}
		arc = Arcs[arc].nextin;
	}
    
	while (firstqueue < lastqueue) {
		source = queue[firstqueue++];
		arc = Sources[source].fout;  // These should be outgoing arcs. Need to check ones with flow
		while (arc >= 0) {
			if (Arcs[arc].flow > 0) {  // Has outgoing flow so can be part of augmentation
				sink1 = Arcs[arc].sink;
                if (pred[sink1 + n_sources] < 0) {
                    pred[sink1 + n_sources] = arc;
                    arc1 = Sinks[sink1].fin;
                    while (arc1 >= 0) {
                        source = Arcs[arc1].source;
                        if (visited[source] < 1) {
                            pred[source] = arc1;  // Positive to show it is outgoing
                            if (Sources[source].supply > 0) {
                                return source;
                            }
                            else {
                                visited[source] = 1;
                                queue[lastqueue++] = source; // Adding sources; sink indices will be shifted by n_sources
                            }
                        }
                        arc1 = Arcs[arc1].nextin;
                    }  // Loop over incoming arcs
                }
			}
			arc = Arcs[arc].nextout;
		}  // While arc
	}  // While firstqueue

	// In case of failure
	return -1;
};


/*
 * Generate input for the cs2 program
 */
void generate_CS2()
{
    FILE *fp1;
    int lastnode;
    int intvalue;
    float psurv;
    int totsupply = 0;
    int arcs_per_sink = 6;
    
    lastnode = n_sinks + n_sources +1;
    fp1 = fopen("input.dimacs", "w");
    if (fp1 == NULL){
        printf("Error: Cannot open file 'input.dimacs'!\n");
        exit(1);
    }
    fprintf(fp1, "c  This is the header for the CS2 version \n");
    fprintf(fp1, "p min %d %d \n", lastnode, n_arcs + arcs_per_sink * n_sinks);
    fprintf(fp1, "c  Here are the sources \n");
    for (int i = 0; i < n_sources; i++) {
        fprintf(fp1, "n  %d   %d \n", i + 1, Sources[i].supply);
        totsupply += Sources[i].supply;
    }
    
    // Dummy sink
    fprintf(fp1, "n  %d  %d \n", lastnode, -totsupply);

    fprintf(fp1, "c  Here are the arcs \n");
    for (int i = 0; i < n_arcs; i++) {
        fprintf(fp1,"a  %d  %d  %d  %d  %d \n",
                Arcs[i].source + 1, Arcs[i].sink + 1 + n_sources,
                0, Sources[Arcs[i].source].supply, 0);
    }
    int totarcs = n_arcs;
    
    // Now add the arcs to the dummy node: 6 per sink
    for (int i = 0; i < n_sinks; i++) {
        psurv = Sinks[i].pi;
        for (int k = 0; k < arcs_per_sink; k++){
            intvalue = (int)(10000.0 * 10000.0 * psurv * Sinks[i].pd);
            fprintf(fp1, "a %d  %d  %d  %d  %d \n",
                    n_sources + i + 1, lastnode, 0, 1, -intvalue);
            psurv *= (1.0 - Sinks[i].pd);
        }
    }
    fprintf(fp1, "c  This is the end of the file ");
    fclose(fp1);
}


int main() {
	float density = 1.0;  // Probability of arc existing
	float temp, temp1;
	int sink, n1, n2;
	int source;
	int node, arc;
	int *pred;
	int *queue;
	int *visited;

	clock_t start, end;
	double cpu_time_used;

	int total_supply;
    
    
    /*
     *  Initialize the graph
     */
    // Read input from prompt
    printf("Enter number of sources, number of sinks, density of graph: \n");
    scanf("%d %d %f", &n_sources, &n_sinks, &density);
        
    // Create arrays dynamically (to be destroyed later)
    n1 = (int) (1.2 * density * n_sources * n_sinks);
    Arcs = (struct Arc *) calloc(n1, sizeof(struct Arc));
    Sources = (struct Source *) calloc(n_sources, sizeof(struct Source));
    Sinks = (struct Sink *) calloc(n_sinks, sizeof(struct Sink));
    garcs = (struct HeapElement *) calloc(n_sinks, sizeof(struct HeapElement));
    heap_array = malloc(n_sinks * sizeof(struct HeapElement *));

    pred = calloc(n_sources + n_sinks,sizeof(int));
    queue = calloc(n_sources, sizeof(int));
    visited = calloc(n_sources, sizeof(int));
        
    // Initialize sources
    for (int i = 0; i < n_sources; i++){
        Sources[i].supply = 20;
        Sources[i].fout = -1;
    }
        
    // Initialize sinks
    float totprob = 0.0;
    for (int j = 0; j < n_sinks; j++) {
        Sinks[j].flow = 0;
        Sinks[j].fin = -1;
        Sinks[j].pi = 0.1 + 0.3 * ((float) rand()) / RAND_MAX;
        totprob += Sinks[j].pi;
        Sinks[j].pd = 0.6 + 0.3 * ((float) rand()) / RAND_MAX;
    }
    for (int j = 0; j < n_sinks; j++) {
        Sinks[j].pi /= totprob;
    }  // Normalize probabilities

    // Initialize arcs; store both sink and source sides
    n_arcs = 0;
    for (int i = 0; i < n_sources; i++){
        for (int j = 0; j < n_sinks; j++){
            temp = ((float) rand()) / RAND_MAX;
            if (temp < density){
                Arcs[n_arcs].nextin = Sinks[j].fin;
                Sinks[j].fin = n_arcs;
                Arcs[n_arcs].nextout = Sources[i].fout;
                Sources[i].fout = n_arcs;
                Arcs[n_arcs].source = i;
                Arcs[n_arcs].sink = j;
                Arcs[n_arcs].flow = 0;
                n_arcs++;
            }
        }
    }
    
    /*
     * Alternatively, you can read the graph from an input file
     */
    #if 0
    FILE *fp_input = fopen("input.txt", "r");
    if (fp_input == NULL) {
        printf("Error: Cannot find input file 'input.txt'!\n");
        exit(1);
    }
    fscanf(fp_input, "%d %d %d", &n_sources, &n_sinks, &n_arcs);
        
    // Initialize sources
    for (int i = 0; i < n_sources; i++) {
        fscanf(fp_input, "%d", &n1);
        Sources[i].supply = n1;
        Sources[i].fout = -1;
    }

    // Initialize sinks
    float pi_total = 0;
    for (int j = 0; j < n_sinks; j++) {
        Sinks[j].flow = 0;
        Sinks[j].fin = -1;
        fscanf(fp_input, "%f %f", &temp, &temp1);
        Sinks[j].pi = temp;
        Sinks[j].pd = temp1;
    }
        
    // Initialize arcs
    for (int i = 0; i < n_arcs; i++) {
        fscanf(fp_input, "%d %d", &n1, &n2);
        Arcs[i].source = n1;
        Arcs[i].sink = n2;
        Arcs[i].flow = 0;
        Arcs[i].nextin = Sinks[n2].fin;
        Sinks[n2].fin = i;
        Arcs[i].nextout = Sources[n1].fout;
        Sources[n1].fout = i;
    }

    fclose(fp_input);
    #endif
    
    // Generate input file for the cs2 program
    generate_CS2();

	// Timer
	start = clock();

	// Initialize heap
	initialize_heap();
	heapMake();

	// Now start the algorithm
	total_supply = 0;
	for (int i = 0; i < n_sources; i++) {
        total_supply += Sources[i].supply;
    }
    
	while (total_supply > 0){
		sink = heap_array[0]->sink;
		source = find_augmenting_source(sink, pred, queue, visited);
		if (source >= 0) {  // Have a source with supply
			heap_array[0]->value *= 1 - Sinks[sink].pd;
			downHeap(0);
			Sinks[sink].flow++;
			Sources[source].supply--;
			total_supply--;
            
			// Adjust augmenting path flows
			do {
				arc = pred[source];
				Arcs[arc].flow += 1;  // Augment flow on this arc from a source
				node = Arcs[arc].sink;
				if (node == sink) break;
				arc = pred[node + n_sources];
				Arcs[arc].flow -= 1;  // Decrease flow on this arc from a sink
				source = Arcs[arc].source;
			} while (node != sink);

		} else {
            // Found no augmentation.
            // No more flow can reach this sink.
            // Remove from heap.
			remove_max();
		}
	}
    
	// Timer
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	// Compute final probability of detection
	float Prob_det = 0.0;
    int tot_flow = 0;

	for (int i = 0; i < n_sinks; i++){
		temp = 1.0;
        tot_flow += Sinks[i].flow;
		for (int j = 0; j < Sinks[i].flow; j++){
			temp *= (1 - Sinks[i].pd);
		}
		Prob_det += Sinks[i].pi*(1 - temp);
	}

	printf("Sources: %d, Sinks: %d, Arcs: %d \n", n_sources, n_sinks, n_arcs);
	printf("CPU time %lf  \nFinal prob. detection %f \n", cpu_time_used, Prob_det);
    printf("Total flow assigned %d\n", tot_flow);
	n1 = 0;
	for (int i = 0; i < n_sinks; i++) {
		if (Sinks[i].flow > n1) n1 = Sinks[i].flow;
	}
	printf("Max flow into a sink: %d \n", n1);
    printf("Assignments (optional; see output.txt)\n");
	
    /*
     * Below is optional code.
     * Only use when you want to print out all the flow assignments
     * to output.txt.
     */
    #if 0
    fp_output = fopen("output.txt", "w");
	for (int i = 0; i < n_arcs; i++) {
		if (Arcs[i].flow > 0) {
			fprintf(fp_output, "Assign source %d to sink %d flow %d \n", Arcs[i].source, Arcs[i].sink, Arcs[i].flow);
		}
	}
	fclose(fp_output);
    #endif

    free(pred);
	free(visited);
	free(Arcs);
	free(Sources);
	free(Sinks);
	free(garcs);
	free(heap_array);
    
	return(1);
}
