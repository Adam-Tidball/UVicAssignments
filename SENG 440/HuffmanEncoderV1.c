#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//define some constants used in the program
#define MAX_LENGTH 250

//data structer for the alphabet and probabilities
struct Alpha_Cell{
    char character;
    float probability;
};


int main(){

    printf("Huffman Decoding Starting Up!\n");

    struct Alpha_Cell alpha_cell;

    //open alphabet.txt file & end program if does not open
    FILE *file;
    char alphabet_input[MAX_LENGTH];

    file = fopen("alphabet.txt","r");

    if (file == NULL)
    {
        printf("Failed to open the file");
        return 1;
    }

    // Read and print each line of the file
    while (fgets(alphabet_input, MAX_LENGTH, file) != NULL) 
    {
        printf("Line being read: \"%s\"", alphabet_input);

        // Each line gets parsed and put into a nice data structure
        int i = 0;
        while(alphabet_input[i] != '\0')
        {
            if(isalpha(alphabet_input[i]))
            {
                alpha_cell.character = alphabet_input[i];
            } 
            else if (isdigit(alphabet_input[i]))
            {
                alpha_cell.probability = alphabet_input[i];
            }
            i++;
        }


    }
    printf("All lines read\n");

    //Test printing out of the data struct - FAILS HERE
    printf("Index Number [%d] character: [%s] probability: [%f]\n", 1, alpha_cell.character, alpha_cell.probability);


    // Close the file
    fclose(file);

    return 0;
}