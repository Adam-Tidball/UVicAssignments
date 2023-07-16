// Macros
#define MAX_FILE_LENGTH 512
#define NUM_SYMBOLS 26 // Size of the alphabet
#define MAX_CHAR_CODE_LENGTH 8 // In theory this should be 8


// Structs
typedef struct node_t {
    char character;
    float probability;
    struct node_t *leftChild, *rightChild;
} node_t;

// For a node at index "i": 
// The left child is at index "2*i+1" 
// The right child is at index "2*i+2" 
// And the parent node is at index "(i-1)//2 (floor division)"
typedef struct {
    node_t **treeArray;
    int numNodes;
    int maxNodes; 
} minHeap_t;

typedef struct huffmanTable {
    char character;
    int bitVal[MAX_CHAR_CODE_LENGTH];
    int bitValSize;
} huffmanTable;

// Functions 

/** Min Heap**/
node_t* allocateNewNode(char character, float probability){
    // Dynamically allocate memory for a new node 
    node_t* temp = (node_t*)malloc(sizeof(node_t));
    temp->leftChild = NULL;
    temp->rightChild = NULL;
    temp->character = character;
    temp->probability = probability;
    return temp;
}

minHeap_t* createMinHeap(int maxNodes){
    // Create minHeap object and allocate necessary memory  
    minHeap_t* minHeap = (minHeap_t*)malloc(sizeof(minHeap_t));
 
    minHeap->numNodes = 0;
    minHeap->maxNodes = maxNodes;
 
    minHeap->treeArray = (node_t**)malloc(minHeap->maxNodes * sizeof(node_t*));
    return minHeap;
}

void swapNodes(node_t** n1, node_t** n2){
    // Swap two nodes 
    node_t* temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

void heapify(minHeap_t* minHeap, int i){
    // Standard min-heapify  
    int minI = i;
    int leftI = 2*i+1;
    int rightI = 2*i+2;

    if (leftI < minHeap->numNodes && minHeap->treeArray[leftI]->probability < minHeap->treeArray[minI]->probability){
        minI = leftI;
    }
    
    if (rightI < minHeap->numNodes && minHeap->treeArray[rightI]->probability < minHeap->treeArray[minI]->probability){
        minI = rightI;
    }
 
    if (minI != i) {
        // Recurse 
        swapNodes(&minHeap->treeArray[minI],&minHeap->treeArray[i]);
        heapify(minHeap, minI);
    }
}

node_t* popMin(minHeap_t* minHeap){
    // Pop the min and heapify before returning
    node_t* temp = minHeap->treeArray[0];
    minHeap->treeArray[0] = minHeap->treeArray[minHeap->numNodes - 1];
 
    minHeap->numNodes--;
    heapify(minHeap, 0);
 
    return temp;
}

void insertMinHeap(minHeap_t* minHeap, node_t* newNode){
    // Insert a new node into the heap
    minHeap->numNodes++;
    int i = minHeap->numNodes - 1;
 
    while (i && newNode->probability < minHeap->treeArray[(i - 1) / 2]->probability) {
 
        minHeap->treeArray[i] = minHeap->treeArray[(i - 1) / 2];
        i = (i - 1) / 2;
    }
 
    minHeap->treeArray[i] = newNode;
}

void buildMinHeap(minHeap_t* minHeap){
    // Build heap
    int n = minHeap->numNodes - 1;
    int i;
 
    for (i = (n - 1) / 2; i >= 0; --i)
        heapify(minHeap, i);
}

int isLeaf(node_t* node){
    // Returns 1 if a node is a leaf; returns 0 otherwise
    return !(node->leftChild) && !(node->rightChild);
}

minHeap_t* initMinHeap(char alph[], float prob[], int size){
    // NOTE: If there are n symbols in our alphabet, then there are n-1 internal nodes for Huffman Coding, for a total of 2n - 1 nodes for a tree
    int numLeaves = size;
    minHeap_t* minHeap = createMinHeap(2*numLeaves - 1);
 
    for (int i = 0; i < size; i++){
        minHeap->treeArray[i] = allocateNewNode(alph[i], prob[i]);
    }
 
    minHeap->numNodes = numLeaves;
    buildMinHeap(minHeap);
 
    return minHeap;
}

/** Helper **/
void printArr(int arr[], int n){
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}

void freeMem(int* encoded){
    while(*encoded != -1){
        free(encoded);
        encoded++;
    }
    free(encoded);
}