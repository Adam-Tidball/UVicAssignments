// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Macros
#define MAX_FILE_LENGTH 512
#define MAX_SYMBOLS 63 


// Structs
typedef struct node_t {
    char character;
    float probability;
    struct node_t *leftChild, *rightChild;
} node_t;


int main(){
    printf("Huffman Decoding Starting Up!\n");
    char arr[] = {'0','1'};
    

    return 0;
}