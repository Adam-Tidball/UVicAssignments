// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "HuffmanHeader.h"




void brute_decode_text( int* encoded_text, struct huffmanTable* huffmanTable)
{
    // Basic tests
    printf("\n");
    for(int x = 0; x < 35; x++){
        printf("%d",encoded_text[x]);
    }
    printf("\n");

    // Find longest code length  
    int code_length = 0;
    for(int j = 0; j < MAX_SYMBOLS; j++){
        if(code_length < huffmanTable[j].bitValSize){
            // add all the bit code values to array
            code_length = huffmanTable[j].bitValSize;
        }
    }

    // Look at max length sized sections of encoded text 
    int size_to_remove = 0;
    for(int i = 0; i < code_length; i++){
        //check the length and the value
        if(huffmanTable[i].bitValSize == i){
            if(huffmanTable[i].bitVal[i] == encoded_text[i]){
                size_to_remove = i;

            }
        }

    }



}


// Draft Functions below 

// void create_LUTs(int num_tables, struct huffmanTable* huffmanTable){


// }

// void decode_text(node_t* root, int* encoded_text){
//     //pass in the root of the EncTree and encode the text
//     // int text_length = 0;
//     // for(int i = 0; encoded_text[i] != NULL; i++){
//     //     text_length = i;
//     // }
//     // printf("the text length is: %d \n", text_length);
// }