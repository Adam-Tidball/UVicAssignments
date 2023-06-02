#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//define constant for maximum alphabet file size
#define MAX_FILE_LENGTH 250
#define MAX_SYMBOLS 63 //requires 6 bits of space for max symbol key 

//data structer for the alphabet and probabilities
struct Alpha_Cell{
    char character[MAX_SYMBOLS]; //this data struct only allows for char symbols
    float probability[MAX_SYMBOLS];
};


int main(){

    // Create variables and indicate the program is running
    printf("Huffman Decoding Starting Up!\n");
    struct Alpha_Cell alpha_cell;
    char alphabet_input[MAX_FILE_LENGTH];

    // Open alphabet.txt file & end the program if it does not open
    FILE *file;
    file = fopen("alphabet.txt","r");
    if (file == NULL)
    {
        printf("Failed to open the file");
        return 1;
    }

    // Read and print each line of the file
    while (fgets(alphabet_input, MAX_FILE_LENGTH, file) != NULL) 
    {
        printf("Line being read: \"%s\"", alphabet_input);

        // Each line gets parsed and put into the data structure
        int i = 0;
        int symbol_count = 0;
        while(alphabet_input[i] != '\0')
        {
            
            if(isalpha(alphabet_input[i]))
            {
                alpha_cell.character[symbol_count] = alphabet_input[i];
                i++;
            } 
            else if (isdigit(alphabet_input[i]))
            {
                int x = 0;
                char tmp_num[10];
                while(isdigit(alphabet_input[i+x]) || ('.' == alphabet_input[i+x]) && x < 10)
                {
                    tmp_num[x] = alphabet_input[i+x];
                    x++;
                }
                alpha_cell.probability[symbol_count] = atoi(tmp_num);
                i = i + x;
            } 
            else 
            {
                i++;
            }
            symbol_count++;
        }
        

    }
    printf("All lines read\n");

    // Close the file
    fclose(file);


    //print out the data from the data structure
    printf("\nData from the data structure:\n");
    int y = 0;
    while(alpha_cell.character[y] != NULL)
    {
        printf("Character for index %d is: %c\n",y, alpha_cell.character[y]);
        printf("Probability for index %d is: %f\n",y,alpha_cell.probability[y]);
        y++;
    }

    return 0;
}