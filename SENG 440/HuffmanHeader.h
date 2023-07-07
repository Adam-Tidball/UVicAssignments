// Macros
#define MAX_FILE_LENGTH 512
#define MAX_SYMBOLS 26 // Size of the alphabet
#define MAX_CHAR_CODE_LENGTH 8 // In theory thi should be 8


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
// struct huffmanTable huffmanTableOne[MAX_SYMBOLS];
// int huff_index = 0;
