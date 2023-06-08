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

    // Acquire first token from input; Splits on , and / as delimiters 
    char *tokInput = strtok(alphabet_input, ",/");
    alphaCell curCell;
    int i = 0;

    // Iterate through remaining tokens to populate list
    while (tokInput){
        if(isalpha(*tokInput)) {
            curCell.character = *tokInput;
            tokInput = strtok(NULL, ",/");
            curCell.probability = atof(tokInput);

            alphaList[i] = curCell;
            i++;
        }

        tokInput = strtok(NULL, ",/");
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

    for(int i = 0; i < symbol_count; i++){
        printf("%c %.3f\n", alphaList[i].character, alphaList[i].probability);
    }


/*
    // Read and print each line of the file
    while (fgets(alphabet_input, MAX_FILE_LENGTH, file) != NULL) 
    {
        printf("Line being read: \"%s\"", alphabet_input);

        // Each line gets parsed and put into the data structure
        int i = 0;
        while(alphabet_input[i] != '\0')
        {
            if(isalpha(alphabet_input[i]))
            {
                alpha_cell.character[symbol_count] = alphabet_input[i];
                symbol_count++;
            } 
            else if (isdigit(alphabet_input[i]))
            {
                int x = 0;
                char tmp_numS[10];
                while(isdigit(alphabet_input[i+x]) || ('.' == alphabet_input[i+x]))
                {
                    tmp_numS[x] = alphabet_input[i+x];
                    x++;
                }
                alpha_cell.probability[prob_count] = strtof(tmp_numS, NULL);
                printf("WHAT THE FYUCK IS RHIS %s\n",strtof(tmp_numS, NULL));
                prob_count++;
                i = i + x;
            } 
            i++;
        }
        

    }
    printf("All lines read\n");

    // Close the file
    fclose(file);


    //print out the data from the data structure
    printf("\nData from the data structure:\n");
    int y = 0;
    while(6 > y)
    {
        printf("Character for index %d is: %c\n",y, alpha_cell.character[y]);
        printf("Probability for index %d is: %f\n",y,alpha_cell.probability[y]);
        y++;
    }
*/
    return 0;
}