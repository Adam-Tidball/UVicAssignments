// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "HuffmanHeader.h"

//create a table for the huffman codes as a global variable
struct huffmanTable huffmanTableOne[NUM_SYMBOLS];
int huff_index = 0;


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

void huffmanCodes(node_t* root){
    // Print Huffman codes
    int arr[100], top = 0;
 
    printCodes(root, arr, top);  //added feature that creates Huffman Table
}


void printEncodedTextTable(const char* text, struct huffmanTable* huffmanTable) {
    int text_size = strlen(text);
        // For all symbols in input text
    for(int i = 0; i < text_size; i++){
        // Check for matching symbol in huffman table
        for(int j = 0; j < NUM_SYMBOLS; j++){
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
    // Note: A message fully composed of Qs would have a longer encoding than text_size
    int index = 0;

        // For all symbols in input text
    for(int i = 0; i < text_size; i++){
        // Check for matching symbol in huffman table
        for(int j = 0; j < NUM_SYMBOLS; j++){
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
    encodedArry[index] = -1; //make the last value -1
    printf("the size of the index is %d\n", index);
    printArr(encodedArry, index);

    return encodedArry;
}

// Huffman decoding
void brute_decode_text( int* encodedText, struct huffmanTable* huffmanTable)
{
    // Get encoded Text Length
    printf("\n");
    int encoded_text_length = 0;
    for(int x = 0; encodedText[x] != -1; x++){
        printf("%d",encodedText[x]);
        encoded_text_length = x;
    }
    printf("\n test \n");    
    
    
    // Find longest code length  
    int code_length = 0;
    for(int j = 0; j < NUM_SYMBOLS; j++){
        if(code_length < huffmanTable[j].bitValSize){
            // add all the bit code values to array
            code_length = huffmanTable[j].bitValSize;
        }
    }

    // Test
    printf("longest code: %d", code_length);

    // For every symbol code pair in the table
    // check if the code exists in the code_length section
    //while(encodedText[0] != -1){
        for(int y = 0; y < NUM_SYMBOLS; y++){
            for(int i = 0; i < code_length; i++){
                if(huffmanTable[y].bitVal[i] != encodedText[i]){ // if no match
                    break;
                } else if(huffmanTable[y].bitValSize == i){ // if match char & length
                    printf("CHAR FOUND: %c", huffmanTable[y].character);
                    //char found so remove the code
                    for(int d = 0; (d+i) <= encoded_text_length; d++){
                        encodedText[d] = encodedText[d+i];
                    }
                } else { // if match char
                    //printf("match\n");
                }
            }    
        }
    //}
}

void tree_decodingRaw(int* encoded, node_t* root){
    node_t* cur = root;

    while (*encoded != -1){
        if(!isLeaf(cur)){
            if(*encoded == 0){
                cur = cur->leftChild;
            }
            else if(*encoded == 1){
                cur = cur->rightChild;
            }
            encoded++;
        }
        else{
            printf("%c", cur->character);
            cur = root;
        }
    }
    printf("%c\n", cur->character);

}

void tree_decodingV1(int* encoded, node_t* root){
    node_t* cur = root;

    while (*encoded != -1){
        if(!isLeaf(cur)){
            if(*encoded == 0){
                cur = cur->leftChild;
            }
            else if(*encoded == 1){
                cur = cur->rightChild;
            }
            encoded++;
        }
        else{
            printf("%c", cur->character);
            if(*encoded == 0){
                cur = root->leftChild;
                encoded++;
            }
            else if (*encoded == 1){
               cur = root->rightChild; 
               encoded++;
            }
        }
    }
    printf("%c\n", cur->character);

}


int main(){
    printf("Huffman Decoding Starting Up!\n");
    char alph[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	float prob[] = { 0.084966, 0.020720, 0.045388, 0.033844, 0.111607, 0.018121, 0.024705, 0.030034, 0.075448, 0.001965, 0.011016, 0.054893, 0.030129, 0.066544, 0.071635, 0.031671, 0.001962, 0.075809, 0.057351, 0.069509, 0.036308, 0.010074, 0.012899, 0.002902, 0.017779, 0.002722};
    int symbol_count = sizeof(alph) / sizeof(alph[0]);
    clock_t start, end;

    node_t* root = buildHuffmanEncTree(alph, prob, symbol_count);

    huffmanCodes(root);
    
    char text[] = {
    'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E', 'I', 'S', 'A', 'N', 'A', 'W', 'E', 'I', 'N', 'S', 'P', 'I', 'R', 'I', 'N', 'G', 'C', 'R', 'E', 'A', 'T', 'I', 'O', 'N', 'T', 'H', 'A', 'T', 'H', 'A', 'S', 'R', 'E', 'V', 'O', 'L', 'U', 'T', 'I', 'O', 'N', 'I', 'Z', 'E', 'D', 'T', 'H', 'E', 'W', 'O', 'R', 'L', 'D', 'A', 'S', 'W', 'E', 'K', 'N', 'O', 'W', 'I', 'T', 'I', 'T', 'S', 'S', 'H', 'E', 'E', 'R', 'V', 'E', 'R', 'S', 'A', 'T', 'I', 'L', 'I', 'T', 'Y', 'A', 'N', 'D', 'P', 'O', 'W', 'E', 'R', 'A', 'R', 'E', 'N', 'O', 'T', 'H', 'I', 'N', 'G', 'S', 'H', 'O', 'R', 'T', 'O', 'F', 'R', 'E', 'M', 'A', 'R', 'K', 'A', 'B', 'L', 'E', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E', 'E', 'M', 'P', 'O', 'W', 'E', 'R', 'S', 'U', 'S', 'T', 'O', 'A', 'C', 'C', 'O', 'M', 'P', 'L', 'I', 'S', 'H', 'I', 'N', 'C', 'R', 'E', 'D', 'I', 'B', 'L', 'E', 'F', 'E', 'A', 'T', 'S', 'E', 'N', 'A', 'B', 'L', 'I', 'N', 'G', 'U', 'S', 'T', 'O', 'S', 'T', 'R', 'E', 'A', 'M', 'L', 'I', 'N', 'E', 'C', 'O', 'M', 'P', 'L', 'E', 'X', 'T', 'A', 'S', 'K', 'S', 'S', 'O', 'L', 'V', 'E', 'I', 'N', 'T', 'R', 'I', 'C', 'A', 'T', 'E', 'P', 'R', 'O', 'B', 'L', 'E', 'M', 'S', 'A', 'N', 'D', 'U', 'N', 'L', 'E', 'A', 'S', 'H', 'O', 'U', 'R', 'C', 'R', 'E', 'A', 'T', 'I', 'V', 'I', 'T', 'Y', 'I', 'T', 'H', 'A', 'S', 'T', 'R', 'A', 'N', 'S', 'F', 'O', 'R', 'M', 'E', 'D', 'I', 'N', 'D', 'U', 'S', 'T', 'R', 'I', 'E', 'S', 'A', 'C', 'C', 'E', 'L', 'E', 'R', 'A', 'T', 'E', 'D', 'S', 'C', 'I', 'E', 'N', 'T', 'I', 'F', 'I', 'C', 'A', 'D', 'V', 'A', 'N', 'C', 'E', 'M', 'E', 'N', 'T', 'S', 'A', 'N', 'D', 'E', 'N', 'H', 'A', 'N', 'C', 'E', 'D', 'C', 'O', 'M', 'M', 'U', 'N', 'I', 'C', 'A', 'T', 'I', 'O', 'N', 'O', 'N', 'A', 'G', 'L', 'O', 'B', 'A', 'L', 'S', 'C', 'A', 'L', 'E', '.', '\0'
};


    printEncodedTextTable(alph, huffmanTableOne); //current only creates one LUT (possible to use more)

    int* encoded_text = printEncodedText(text, huffmanTableOne);

    // Decoding 
    printf("\nStart of decoding:\n");

    start = clock();
    tree_decodingRaw(encoded_text, root);
    end = clock();
    printf("Time Elapsed Raw: %ld\n", end - start);

    start = clock();
    tree_decodingV1(encoded_text, root);
    end = clock();
    printf("Time Elapsed V1: %ld\n", end - start);
    

    freeMem(encoded_text);
    return 0;
}