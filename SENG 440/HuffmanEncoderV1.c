#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Macros
#define MAX_FILE_LENGTH 250
#define MAX_SYMBOLS 63 

// Structs
typedef struct {
    char character;
    float probability;
} alphaCell;

int parseAlphaProb(alphaCell* alphaList, FILE* fp){
    // Acquire symbols with their respective probabilities from the input file
    char alphabet_input[MAX_FILE_LENGTH];
    fgets(alphabet_input, MAX_FILE_LENGTH, fp);

    // Acquire first token from input; Splits on , and / and ' ' as delimiters
    char *tokInput = strtok(alphabet_input, ",/ ");
    alphaCell curCell;
    int i = 0;

    // Iterate through remaining tokens to populate list
    while (tokInput){
        if(isalpha(*tokInput)) {
            curCell.character = *tokInput;
            tokInput = strtok(NULL, ",/ ");
            curCell.probability = atof(tokInput);

            alphaList[i] = curCell;

            i++;
        }
        tokInput = strtok(NULL, ",/ ");
    }

    //recursive case for multiple lined files --- needs this to read from multiple lines!!
    if(fgets(alphabet_input, MAX_FILE_LENGTH, fp) != NULL){
        printf("gets here\n");
        return i + parseAlphaProb(alphaList, fp);
    }

    return i;
}


int main(){
    // Create variables and indicate the program is running
    printf("Huffman Decoding Starting Up!\n");
    alphaCell alphaList[MAX_SYMBOLS];

    // Open alphabet.txt file & end the program if it does not open
    FILE *fp;
    fp = fopen("Alphabet.txt","r");
    if (fp == NULL) {
        printf("Failed to open the file!");
        exit(-1);
    }

    // Populates list with (symbol, probability) pairs
    printf("Parsing file...\n");
    int symbol_count = parseAlphaProb(alphaList, fp);
    printf("Printing alphabet...\n");

    //close alphabet.txt file
    fclose(fp);

    //testing
    printf("symbol count: %d\n",symbol_count);


    for(int i = 0; i < symbol_count; i++){
        printf("%c %.3f\n", alphaList[i].character, alphaList[i].probability);
    }


    return 0;
}