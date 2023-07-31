#ifndef _HUFFMAN_HEADER_
#define _HUFFMAN_HEADER_
// Macros
#define MAX_FILE_LENGTH 25000      // Constraint for input file
#define MAX_ENCODING_LENGTH 25000  // Constraint for encoded file
#define MAX_PATH_LENGTH 256        // Constraint for file path length
#define MAX_FILENAME_LENGTH 64     // Constraint for filename 
#define MAX_SYMBOLS 128            // ASCII (Set to 256 for Extended ASCII)
#define MAX_CODE_LENGTH 16         // Should be a multiple of 8
#define MAX_TABLES 655             // For Max symbols min table size is over 50
#define MAX_ROWS_PER_TABLE 50       // Sets the maximum number of rows per LUT

// Structs
typedef struct node_t {
    unsigned char symbol;
    float probability;
    struct node_t *leftChild, *rightChild;
} node_t;

typedef struct {
    node_t **treeArray;
    int numNodes;
    int maxNodes; 
} minHeap_t;

typedef struct {
    unsigned char symbol;
    unsigned short bitcode;
    int code_length;
} huffCode_t;

typedef struct {
    unsigned char symbol;
    int code_length;
} lut;

// Functions 

/** Min Heap**/
node_t* allocateNewNode(unsigned char symbol, float probability){
    // Dynamically allocate memory for a new node 
    node_t* temp = (node_t*)malloc(sizeof(node_t));
    // printf("Memory Allocated: %p\n", temp);
    temp->leftChild = NULL;
    temp->rightChild = NULL;
    temp->symbol = symbol;
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

minHeap_t* initMinHeap(unsigned char alph[], float prob[], int size){
    // NOTE: If there are n symbols in our alphabet, 
    // then there are n-1 internal nodes for Huffman Coding, for a total of 2n - 1 nodes for a tree
    int numLeaves = size;
    minHeap_t* minHeap = createMinHeap(2*numLeaves - 1);
 
    for (int i = 0; i < size; i++){
        minHeap->treeArray[i] = allocateNewNode(alph[i], prob[i]);
    }
 
    minHeap->numNodes = numLeaves;
    buildMinHeap(minHeap);  // TODO: Get rid of this function call as it's only used once
 
    return minHeap;
}

/** Huffman Encoding**/

node_t* buildHuffmanEncTree(unsigned char alph[], float prob[], int size){
    node_t *left, *right, *top;
 
    // Create, initialize, and build the min heap 
    minHeap_t* minHeap = initMinHeap(alph, prob, size);
 
    // Iterate while size of heap doesn't become 1
    while (!(minHeap->numNodes == 1)) {
 
        // Extract the two minimum probability nodes
        left = popMin(minHeap);
        right = popMin(minHeap);

        // Create new node: Internal nodes identified by '~', set child pointers 
        top = allocateNewNode('~', left->probability + right->probability);

        top->leftChild = left;
        top->rightChild = right;
 
        insertMinHeap(minHeap, top);
    }

    minHeap_t* lost_soul = minHeap;
    node_t* root = popMin(minHeap);

    // Release the no longer needed min-heap and its followers, from the callous chains of existence that bind us
    free(lost_soul->treeArray);
    free(lost_soul);

    // Return the root of the Huffman tree
    return root;
}


/** Helper **/
void printArr(int arr[], int n){
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}

int symbolIndex(unsigned char* symbols, int symbol_count, unsigned char symbol){
    // If a symbol exists in symbols, return its index
    // Otherwise return -1
    for (int i = 0; i < symbol_count; i++){
            if (symbols[i] == symbol){
                return i;
            }
        }

    return -1;
}

void stripFilename(char* filename, char* destination) {
    char strip[64];
    char* tok;
    strncpy(strip, filename, 96);
    tok = strtok(strip, ".");
    strncpy(destination, tok, 96);
}

void freeHuffmanTree(node_t* node) {
    if (node == NULL) {
        return;
    }

    freeHuffmanTree(node->leftChild);
    freeHuffmanTree(node->rightChild);
    
    free(node);
}

int maxTreeDepth(node_t* root) {
    if (root == NULL) {
        return 0;
    } else {
        int leftDepth = maxTreeDepth(root->leftChild);
        int rightDepth = maxTreeDepth(root->rightChild);
        int max = (leftDepth > rightDepth) ? leftDepth : rightDepth;
        return max + 1;
    }
}

#endif