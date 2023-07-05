// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Macros
#define MAX_FILE_LENGTH 512
#define MAX_SYMBOLS 26 // Size of the alphabet
#define MAX_CHAR_CODE_LENGTH 10 // In theory thi should be 8


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

//create a table for the huffman codes as a global variable
struct huffmanTable huffmanTableOne[MAX_SYMBOLS];
int huff_index = 0;

// Functions
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

void printArr(int arr[], int n){
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
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

node_t* buildHuffmanEncTree(char alph[], float prob[], int size){
    // Construct a Huffman Coding Tree for the provided alphabet 
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
 
    // Pop last node
    return popMin(minHeap);
}

void printCodes(node_t* root, int arr[], int symbol_count){
 
    // Assign 0 to left edge and recur
    if (root->leftChild) {
 
        arr[symbol_count] = 0;
        printCodes(root->leftChild, arr, symbol_count + 1);
    }
 
    // Assign 1 to right edge and recur
    if (root->rightChild) {
 
        arr[symbol_count] = 1;
        printCodes(root->rightChild, arr, symbol_count + 1);
    }
 
    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) {
 
        printf("%c: ", root->character);
        printArr(arr, symbol_count);

        // Add the character and its symbol
        // to a table to use for encoding
        huffmanTableOne[huff_index].character = root->character;
        huffmanTableOne[huff_index].bitValSize = symbol_count;
        for(int y = 0; y < symbol_count; y++){
            huffmanTableOne[huff_index].bitVal[y] = arr[y];
        }
        huff_index++;

    }
}

void huffmanCodes(char alph[], float prob[], int size){
    // Construct Huffman Tree
    node_t* root = buildHuffmanEncTree(alph, prob, size);
 
    // Print Huffman codes using
    // the Huffman tree built above
    int arr[100], top = 0;
 
    printCodes(root, arr, top);  //added feature that creates Huffman Table

 
}


void printEncodedTextTable(const char* text, struct huffmanTable* huffmanTable) {
    int text_size = strlen(text);
        // For all symbols in input text
    for(int i = 0; i < text_size; i++){
        // Check for matching symbol in huffman table
        for(int j = 0; j < MAX_SYMBOLS; j++){
            // If matching char found
            if(text[i] == huffmanTable[j].character){
                printf("character: %c   Code: ",text[i]);
                // Print all the bit code values
                for(int z = 0; z < huffmanTable[j].bitValSize; z++){
                    printf("%d",huffmanTable[j].bitVal[z]);
                }
                printf("\n");
            }

        }
    }
    
} 

int* printEncodedText(const char* text, struct huffmanTable* huffmanTable) {
    int text_size = strlen(text);
    int* encodedArry = (int*)malloc((text_size) * sizeof(int));  // allocate memory for the message
    int index = 0;

        // For all symbols in input text
    for(int i = 0; i < text_size; i++){
        // Check for matching symbol in huffman table
        for(int j = 0; j < MAX_SYMBOLS; j++){
            // If matching char found
            if(text[i] == huffmanTable[j].character){
                // add all the bit code values to array
                for(int z = 0; z < huffmanTable[j].bitValSize; z++){
                    encodedArry[index] = huffmanTable[j].bitVal[z];
                    index++;
                }
            }

        }
    }
    printf("the size of the index is %d\n", index);
    printArr(encodedArry,index);

    return encodedArry;
} 

int main(){
    printf("Huffman Decoding Starting Up!\n");
    char alph[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	float prob[] = { 0.084966, 0.020720, 0.045388, 0.033844, 0.111607, 0.018121, 0.024705, 0.030034, 0.075448, 0.001965, 0.011016, 0.054893, 0.030129, 0.066544, 0.071635, 0.031671, 0.001962, 0.075809, 0.057351, 0.069509, 0.036308, 0.010074, 0.012899, 0.002902, 0.017779, 0.002722};
    int symbol_count = sizeof(alph) / sizeof(alph[0]);

    huffmanCodes(alph, prob, symbol_count);

    char text[] = {'S','O','F','T','W','A','R','E','\0'}; //always include null terminator at the end
    
    printEncodedTextTable(text, huffmanTableOne);


    int* encoded_text = printEncodedText(text,huffmanTableOne);
    free(encoded_text);



    return 0;
}