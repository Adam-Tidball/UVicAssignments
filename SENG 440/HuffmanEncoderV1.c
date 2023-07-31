// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include "HuffmanHeader.h"
#include <stdint.h>
#include <math.h>



void printCodesBitified(node_t* root, unsigned char* buff, int bit_count, huffCode_t* asciiToHuffman) {
    if (root->leftChild) {                                         // Assign 0 for left edge and recur
        buff[bit_count] = 0;
        printCodesBitified(root->leftChild, 
        buff, bit_count + 1, 
        asciiToHuffman);
    }
 
    if (root->rightChild) {                                        // Assign 1 for right edge and recur
        buff[bit_count] = 1;
        printCodesBitified(root->rightChild, 
        buff, bit_count + 1, 
        asciiToHuffman);
    }
 

    if (isLeaf(root)) {                                            // If this is a leaf node, then it contains a symbol
        unsigned char ascii_index = (unsigned char) root->symbol;  // Cast the symbol to unisgned char
        asciiToHuffman[ascii_index].symbol = root->symbol;         // Add the symbol and, 
        asciiToHuffman[ascii_index].code_length = bit_count;       // the length of its encoding to the table

        for (int i = 0; i < bit_count; i++) {
            asciiToHuffman[ascii_index].bitcode +=  buff[i] * (1U << i);  // Represent the encoding with the right amount of bits
        }
    } 
}


void generatehuffmanCodesBitified(node_t* root, huffCode_t* asciiToHuffman) {
    // Initialize each symbol in the table such that its index matches its ascii encoding (i.e. asciiToHuffman[65].symbol = 'A')
    unsigned char buff[128];
    for(int i = 0; i < MAX_SYMBOLS; i++){
        asciiToHuffman[i].symbol = (unsigned char) i;
        asciiToHuffman[i].bitcode = 0;
    }
    
    printCodesBitified(root, buff, 0, asciiToHuffman);
}


void printSymbolEncodingBitified(unsigned char* symbols, int symbol_count,  huffCode_t* asciiToHuffman) {
    unsigned char symbol;
    char *control_characters[] = {"NUL","SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", 
                                  "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", 
                                  "DC1", "DC2","DC3", "DC4", "NAK", "SYN", "ETB", "CAN", 
                                  "EM", "SUB", "ESC", "FS", "GS", "RS", "US"};

    for (size_t i = 0; i < MAX_SYMBOLS; i++) {
        symbol = asciiToHuffman[i].symbol;
        if (symbolIndex(symbols, symbol_count, symbol) != -1) {
            // Print symbol
            if(symbol < 8 || (symbol > 15 && symbol < 25) || symbol == 26 || symbol == 27){
                printf("Symbol: %s  Encoding: ", control_characters[(unsigned char) symbol]);
            }
            else if((symbol > 7 && symbol < 16) || symbol == 25 ||(symbol > 27 && symbol < 32)){
                printf("Symbol: %s    Encoding: ", control_characters[(unsigned char) symbol]);
            }
            else if(symbol == 32){
                printf("Symbol: space Encoding: ");
            }
            else if(symbol == 127){
                printf("Symbol: DEL   Encoding: ");
            }
            else{
                printf("Symbol: %c     Encoding: ", symbol);
            }

            unsigned char code[MAX_CODE_LENGTH] = {0};

            
            for (size_t j = 0; j < asciiToHuffman[i].code_length; j++) {
                if (asciiToHuffman[i].bitcode & (1U << j)) {
                    code[j] = '1';
                }
                else {
                    code[j] = '0';
                }

            }
            printf("%s\n", code);
        }
    }

}


int generateEncodedFileBitified(const char* sample_filename, const char* encoded_filename, huffCode_t* asciiToHuffman) {
    unsigned char sample[MAX_FILE_LENGTH];                    // Contains orginal data
    unsigned short encodedSample[MAX_ENCODING_LENGTH] = {0};  // Initialized at zero so only 1s have to be set 
    unsigned int ascii_index;                                 // Contains the ASCII code of the current symbol
    unsigned int cur_Index = 0;                               // # of shorts in compressed data
    unsigned int sample_size;                                 // # of symbols in original data
    unsigned int bit_position = 0;                            // Keeps track of bit position
    unsigned int bit_count = 0;                               // # of bits in compressed data
    FILE* fp_sample = fopen(sample_filename, "r");            // Points to the original file
    FILE* fp_compressed = fopen(encoded_filename, "wb");      // Poitns to the compressed file
    assert(fp_sample != NULL);
    assert(fp_compressed != NULL);
   
    sample_size = fread(sample, sizeof(unsigned char), MAX_FILE_LENGTH/sizeof(unsigned char), fp_sample);  // Retrieve original data

    for (size_t i = 0; i < sample_size; i++) {                                      // Iterate 1 symbol per loop iteration
        ascii_index = (unsigned char) sample[i];                                    // Get ASCII code for current symbol
            for (size_t j = 0; j < asciiToHuffman[ascii_index].code_length; j++) {  // Iterate through code length
                if (asciiToHuffman[ascii_index].bitcode & (1U << j)) {              // If a bit is set in the bitcode, then
                    encodedSample[cur_Index] |= (1U << bit_position);               // Set the matching bit in the encoding 
                }
                bit_count++;
                bit_position++;                                                     // Increment bit position
                if(bit_position >= 16) {                                            // If bit_postion == 16
                    bit_position = 0;                                               // Reset bit_position
                    cur_Index++;                                                    // Move to next short
                }
            }
    }

    fwrite(&bit_count, sizeof(unsigned int), 1,  fp_compressed);                  // First 4 bytes of the compressed file contain the compressed size in bits (necessary for decoding last symbol)
    fwrite(encodedSample, sizeof(unsigned short), cur_Index + 1,  fp_compressed);     // Rest of file contains encoding - including incomplete index

    fclose(fp_sample);
    fclose(fp_compressed);

    return bit_count;  // Returns # of encoded bits
}

// Huffman decoding
void brute_decode_text(int* encodedText, huffCode_t* asciiToHuffman) {
    // Get encoded Text Length
    printf("\n");
    int encoded_text_length = 0;
    for(int x = 0; encodedText[x] != -1; x++) {
        printf("%d",encodedText[x]);
        encoded_text_length = x;
    }
    printf("\n test \n");    
    
    
    // Find longest code length  
    int code_length = 0;
    for(int j = 0; j < MAX_SYMBOLS; j++){
        if(code_length < asciiToHuffman[j].code_length){
            // add all the bit code values to array
            code_length = asciiToHuffman[j].code_length;
        }
    }

    // Test
    printf("longest code: %d", code_length);

    // For every symbol code pair in the table
    // check if the code exists in the code_length section
    //while(encodedText[0] != -1){
        for(int y = 0; y < MAX_SYMBOLS; y++){
            for(int i = 0; i < code_length; i++){
                if(asciiToHuffman[y].bitcode != encodedText[i]){ // if no match
                    break;
                } else if(asciiToHuffman[y].code_length == i){ // if match char & length
                    printf("CHAR FOUND: %c", asciiToHuffman[y].symbol);
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

void treeDecodingBitByBit(char* huf_filename, char* decompressed_filename, node_t* root) {
    node_t* cur = root;                                         // Cursor for traversing Huffman tree
    unsigned short compressed[MAX_ENCODING_LENGTH];             // Contains compressed data
    unsigned char decompressed[MAX_FILE_LENGTH] = {0};          // Contains decompressed data
    unsigned int compressed_bits = 0;                           // # of bits in compressed data
    unsigned int cur_Index = 0;                                 // Indexes the current short
    unsigned int num_symbols = 0;                               // Counts # of decompressed symbols
    unsigned int bit_position = 0;                              // Keeps track of bit position
    FILE* fp_compressed= fopen(huf_filename, "rb");             // Points to the compressed file
    FILE* fp_decompressed = fopen(decompressed_filename, "w");  // Points to the decompressed file, or creates it if it doesn't exist
    assert(fp_compressed != NULL);
    assert(fp_decompressed != NULL);
    
    
    fread(&compressed_bits, sizeof(unsigned int), 1, fp_compressed);           // Retrieve the # of compressed bits
    fread(compressed, sizeof(unsigned short), MAX_ENCODING_LENGTH/sizeof(unsigned short), fp_compressed);  // Retrieve compressed data 

    for (size_t i = 0; i < compressed_bits; i++) {               // Iterate through 1 encoded bit per loop iteration
        if (!isLeaf(cur)) {
            if (compressed[cur_Index] & (1U << bit_position)) {  //
                cur = cur->rightChild;
            }
            else {
                cur = cur->leftChild;
            }
            bit_position++;
            if (bit_position >= 16) {
                bit_position = 0;
                cur_Index++;
            }
        }
        else {
            decompressed[num_symbols] = cur->symbol;
            num_symbols++;
            cur = root;
            i--;
        }
    }

    if (isLeaf(cur)) {
        decompressed[num_symbols] = cur->symbol;
        num_symbols++;
    }

    fwrite(decompressed, sizeof(unsigned char), num_symbols, fp_decompressed);
    fclose(fp_compressed);
    fclose(fp_decompressed);
}

// LUT
void lutPopulate(node_t* root,  unsigned char* buff, unsigned int bit_count, unsigned int longest_code_exp, lut** all_luts, unsigned int* lut_num, unsigned int* row_count) {

    // Search for leaf to the left
    if (root->leftChild) {
 
        buff[bit_count] = 0;
        lutPopulate(root->leftChild, buff,  bit_count + 1, longest_code_exp, all_luts, lut_num, row_count);
    }
 
    // Search for leaf to the right
    if (root->rightChild) {
 
        buff[bit_count] = 1;
        lutPopulate(root->rightChild, buff, bit_count + 1, longest_code_exp, all_luts, lut_num, row_count);
    }
 
    // If this is a leaf node, then it contains a symbol
    if (isLeaf(root)) {

        // The buffer is the start of the index for the symbol and code_length 
        unsigned int bit_val = 0;        
        for (int i = 0; i < bit_count; i++) {
            bit_val <<= 1;
            if (buff[i]  == 1) {
                bit_val |= 1;
            } 
            else if(buff[i] == 0) {
                bit_val |= 0;
            } 
            else {
                printf("Tried to convert non 1 or 0 to bit!\n");
                exit(1);
            }
        }

        // Still need to fill in all possible combos of bits after the buffer code
        unsigned int min_bit_val = bit_val;
        unsigned int max_bit_val = bit_val;
        unsigned int bit_dif = longest_code_exp - bit_count;

        for(int i = 0; i < bit_dif; i++) {
            min_bit_val <<= 1;
            min_bit_val |= 0;
            max_bit_val <<= 1;
            max_bit_val |= 1;
        } 

        while(min_bit_val <= max_bit_val) {
            all_luts[*lut_num][min_bit_val - ((*lut_num) * (MAX_ROWS_PER_TABLE))].symbol = root->symbol;
            all_luts[*lut_num][min_bit_val - ((*lut_num) * (MAX_ROWS_PER_TABLE))].code_length = bit_count;
            (*row_count) = (*row_count) + 1;
            if ((*row_count) == MAX_ROWS_PER_TABLE) {
                (*lut_num) = (*lut_num) + 1;
                (*row_count) = 0;
            }
            min_bit_val += 1;

            // Testing 1
            // printf("For the table: %d and index: %d The ", *lut_num, (min_bit_val-1) - ((*lut_num) * MAX_ROWS_PER_TABLE) );
            // printf("Symbol \"%c\" was reached\n", root->symbol);
        }

        // Testing 2
        // printf("\nThe main bit_val is: ");
        // for (int i = longest_code_exp - 1; i >= 0; i--) {
        // unsigned int mask = 1u << i;    // Sets ith bit to 1 and rest to 0 for unsigned int literal
        // putchar((bit_val & mask) ? '1' : '0');
        // }
        // printf("  and the symbol is: %c", root->symbol);
        // printf("\nWith the extra zero this is number: %u\n", bit_val);

    }
}

void lutCreation(node_t* root, unsigned alpha_size, lut** all_luts, unsigned int* tables_needed, unsigned int longest_code_exp){
    // TESTING	
    printf("\n\nalpha size is: %d\n", alpha_size);	
    printf("Current mem val being allocated: %ld\n",(sizeof(lut)*(MAX_ROWS_PER_TABLE)));	
    printf("value for the largest code exponenet: %d\n", longest_code_exp);

    // Create the largest possible index	
    unsigned int max_index = (1u << longest_code_exp) - 1;	
    printf("value from the largest index is: %d\n", max_index);  	
    // Determine how many LUTS are needed	
    (*tables_needed) = (max_index / MAX_ROWS_PER_TABLE) + 1;  	
    printf("value for tables needed is: %d\n", *tables_needed);  	
   	
    // Allocate memory for required tables 	
    for(int i = 0; i < (*tables_needed); i++){	
        lut* mem_per_table = malloc(sizeof(lut)*(MAX_ROWS_PER_TABLE));	
        if (mem_per_table == NULL){	
            printf("Failed to allocate memory for look up table");	
            exit(1);	
        }	
        all_luts[i] = mem_per_table;	
    }	
    // Populate the tables with the symbols from the tree	
    unsigned char buff[100];	
    unsigned int lut_num = 0;	
    unsigned int row_counter = 0;	

    printf("done in lutCreation\n");

    lutPopulate(root, buff , 0, longest_code_exp, all_luts, &lut_num, &row_counter);	

    printf("done in lutPopulate\n");
    
}

void lutFreeAll( lut** all_luts, unsigned int* tables_needed){	
    // Interates over all possible tables and frees memory	
    for(int i = 0; i < (*tables_needed); i++){	
        free(all_luts[i]);	
    }	
}

void lutDecoding(char* huf_filename, char* decoded_filename, lut** all_luts, const int barrel_shifter) {
    FILE* fp_compressed = fopen(huf_filename, "rb");
    if(fp_compressed == NULL){
        printf("Failed to open encoded file!\n");
        exit(1);
    }

        FILE* fp_decompressed = fopen(decoded_filename, "a");
    if(fp_decompressed == NULL){
        printf("Failed to open decoded file!\n");
        exit(1);
    }

    char str[] = "\n\nLUT Decoding Below:\n";
    fwrite(str, sizeof(str) - sizeof(char), 1, fp_decompressed);

    // Go through the encoded file by barrel_shifter length sections (longest code length)
    // and search each table for that index
    unsigned int compressed_bits = 0;       // # of bits in compressed data
    fread(&compressed_bits, sizeof(unsigned int), 1, fp_compressed);  
    printf("\ncompressed bits: %d\n",compressed_bits);
    unsigned short encoded[compressed_bits];
    fread(encoded, sizeof(unsigned short), compressed_bits/sizeof(unsigned short), fp_compressed);

    //TESTING
    int x = 0;
    int y = 0;
    long long temp = 0;
    int buf = compressed_bits / 16;
    printf("\n");
    for (int i = 0; i < compressed_bits; i++){
        temp << 1;
        if(encoded[x] & (1U << (i + (16 * x)) )){       // "i + (16 * x)" used to skip over two empty bytes
            temp |= 1;
            printf("1");
        } else {
            temp |= 0;
            printf("0");
        }
        y++;
        if(y >= 16){
            y = 0;
            x++;
        }
    }



    printf("\n\nBarrel shifter size: %d\n", barrel_shifter);


    int decoded_bit_count = 0;
    int cur_index = 0;
    int bit_pos = 0;
    while(decoded_bit_count < compressed_bits){     // Iterate through until all bits are decoded
    
    // Get barrel shifter size section of encoded message     
        unsigned int encoded_section = 0;       
        printf("barrel: ");
        for (int i = decoded_bit_count; i < barrel_shifter + decoded_bit_count; i++) {
            //encoded_section << 1;
            if (encoded[cur_index] & (1U << (i + (16 * cur_index)))) {
                encoded_section |= 1u << ((barrel_shifter - 1) - (i - decoded_bit_count));
                printf("1");
            } else {
                encoded_section |= 0 << ((barrel_shifter - 1) - (i - decoded_bit_count));
                printf("0");
            }
            bit_pos++;
            if(bit_pos >= 16){
                bit_pos = 0;
                cur_index++;
            }
        }



        // Find the index that is the same value as the encoded_section
        // use all_luts[i][x] and the MAX_ROWS_PER_TABLE variable
        unsigned int table_index = 0;
        unsigned int table_num = 0;
        table_index = encoded_section % MAX_ROWS_PER_TABLE;
        table_num = encoded_section / MAX_ROWS_PER_TABLE;
        printf("   barrel value: %d   barrel index: %d     ",encoded_section, cur_index);
        printf("   table[%d][%d] searched   ", table_num, table_index);

        //Set to the code_length at that index and print the symbol
        unsigned int temp_bit_count = all_luts[table_num][table_index].code_length;
        unsigned char symbol = all_luts[table_num][table_index].symbol;

        fwrite(&symbol, sizeof(unsigned char), 1, fp_decompressed);


        // Testing
        // printf("Checking table %d at index %d \n", table_num, table_index);
        // printf("The code length is: %d and the character is: ",temp_bit_count);
        // printf("%c",all_luts[table_num][table_index].symbol);

        bit_pos = bit_pos - (barrel_shifter - temp_bit_count);      // update the bit position to account for not decoded bits
        if (bit_pos < 0) {      // if the bit_pos is negative we need to go back to previous index
            cur_index--;
            bit_pos = 16 + bit_pos;
        }
        
        decoded_bit_count = decoded_bit_count + temp_bit_count;

        printf("    code found: %c  code length: %d   total bits decoded: %d",symbol, temp_bit_count, decoded_bit_count);

        // Shift the encoded message the code length of the symbol
        //encoded >> temp_bit_count;
        // for(int i = 0; i < (compressed_bits - decoded_bit_count); i++){
        //     encoded[i] = encoded[i + temp_bit_count];
        // }

        //printf("\n barrel index: %d\n\n", encoded_section);

        printf("\n");
        
    }

    //fwrite(decoded, sizeof(unsigned char), index, fp_decompressed);
    fclose(fp_compressed);
    fclose(fp_decompressed);
}

int prologue(const char* original_filename, unsigned char* symbols, float* probabilities, unsigned* sample_size) {
    printf("%s\n", original_filename);
    FILE* fp = fopen(original_filename, "r");  // File pointer
    if (fp == NULL){
        printf("Failed to open file!\nExiting Program!\n");
        exit(1);
    }
    unsigned char cur_symbol;                // Holds the current symbol 
    unsigned symbol_count = 0;               // # of unique ASCII values
    unsigned frequencies[MAX_SYMBOLS];       // Frequencies of each symbol
    unsigned char sample[MAX_FILE_LENGTH];   // Buffer for sample text
    assert(fp != NULL);
    *sample_size = fread(sample, sizeof(unsigned char), MAX_FILE_LENGTH, fp);  // Retrieve sample text

    for (int i = 0; i < *sample_size; i++) {                                    // Iterates through sample text 1 char at a time
        cur_symbol = sample[i];
        if (symbol_count > MAX_SYMBOLS) {                                       // If # of symbols exceeds MAX_SYMBOLS, 
            printf("Symbol count exceeded!\n");                                 // then abort
            exit(1);
        }
        else if (symbolIndex(symbols, symbol_count, cur_symbol) == -1) {        // Else if a new symbol is encountered,
            symbols[symbol_count] = cur_symbol;                                 // then store it,  and increment 
            frequencies[symbol_count] = 1;                                      // initialize its frequency,
            symbol_count++;                                                     // and increment symbol_count
        }
        else {                                                                  // Else retrieve index for cur_symbol,
            frequencies[symbolIndex(symbols, symbol_count, cur_symbol)]  += 1;  // and incrememt its frequency
        }
    }
    
    double inv_sample_size = *sample_size;  
    inv_sample_size = 1/inv_sample_size;  // Precomputed for efficiency
    
    for (int i = 0; i < symbol_count; i++) {
        probabilities[i] = frequencies[i]*inv_sample_size;
    }

    fclose(fp);
    return symbol_count;
}

int main(int argc, char* argv[]) {
    /******************************** Variables ********************************/
    char input_filename[MAX_FILENAME_LENGTH];     // Filename provided by user input
    char original_filename[MAX_PATH_LENGTH];      // Filename for original file
    char compressed_filename[MAX_PATH_LENGTH];    // Filename for compressed file
    char decompressed_filename[MAX_PATH_LENGTH];  // Filename for decompressed file
    unsigned char symbols[MAX_SYMBOLS];           // Every unique ASCII symbol in the sample (unsorted)
    float probabilities[MAX_SYMBOLS];             // Corresponding probability for each symbol
    unsigned sample_size;                         // Size of the sample in bits
    unsigned compressed_size;                     // Size of the encoded sample in bits
    unsigned symbol_count;                        // Size of the alphabet
    node_t* huffmanRoot;                          // Root node of the huffman tree
    huffCode_t asciiToHuffman[MAX_SYMBOLS];       // Huffman code of each symbol is stored at the index of its ascii encoding
    lut* all_luts[MAX_TABLES];                    // An array to store all the look up tables
    unsigned tables_needed = 0;                   // Used to keep track of the LUTs needed for an alphabet

    /******************************** Handle User Input ********************************/
    if (argc == 1) {
        fprintf(stderr, "Too few arguments!\nusage: %s <filename>\n", argv[0]);
        exit(1);
    }
    else if (argc == 2 && strlen(argv[1]) < 64) { 
        strncpy(input_filename, argv[1], 63);
    }
    else if (strlen(argv[1]) >= 64) {
        fprintf(stderr, "Filename is too long!\nusage: %s <filename>\n", argv[0]);
        exit(1);
    }
    else {
        fprintf(stderr, "Too many arguments!\nusage: %s <filename>\n", argv[0]);
        exit(1);
    }

    /******************************** Initialization ********************************/
    if (input_filename != NULL) {
        // Assign directory paths
        strncpy(original_filename, "original/", MAX_PATH_LENGTH - 1);
        strncpy(compressed_filename, "compressed/", MAX_PATH_LENGTH - 1);
        strncpy(decompressed_filename, "decompressed/", MAX_PATH_LENGTH - 1);

        // Null-terminate the destination buffers
        original_filename[MAX_PATH_LENGTH - 1] = '\0';
        compressed_filename[MAX_PATH_LENGTH - 1] = '\0';
        decompressed_filename[MAX_PATH_LENGTH - 1] = '\0';

        // Retrieve filename without file extension
        char stripped_filename[MAX_FILENAME_LENGTH];
        strncpy(stripped_filename, input_filename, MAX_FILENAME_LENGTH - 1);
        strtok(stripped_filename, ".");

        // Append filename to file paths
        strncat(original_filename, stripped_filename, MAX_PATH_LENGTH - strlen(original_filename) - 1);
        strncat(compressed_filename, stripped_filename, MAX_PATH_LENGTH - strlen(compressed_filename) - 1);
        strncat(decompressed_filename, stripped_filename, MAX_PATH_LENGTH - strlen(decompressed_filename) - 1);

        // Append file extension
        strncat(original_filename, ".txt", MAX_PATH_LENGTH - strlen(original_filename) - 1);
        strncat(compressed_filename, ".huf", MAX_PATH_LENGTH - strlen(compressed_filename) - 1);
        strncat(decompressed_filename, ".txt", MAX_PATH_LENGTH - strlen(decompressed_filename) - 1);
    }
    else {
        fprintf(stderr, "Error retrieving filename!\nusage: %s <filename>\n", argv[0]);
        exit(1);
    }
    

    symbol_count = prologue(original_filename, symbols,  // Retrieves sample text and size, determines symbols 
                   probabilities, &sample_size);         // and their probabilities, and return symbol_count
    
    
    /******************************** Encoding ********************************/
    huffmanRoot = buildHuffmanEncTree(symbols,                           // Construct a Huffman encoding tree 
                                      probabilities,                     // for the given symbol and probability pairs
                                      symbol_count);                     
    
    generatehuffmanCodesBitified(huffmanRoot,                            // Recursively traverse the tree, generate the huffman code for every leaf's symbol,
                                 asciiToHuffman);                        // and populate the encoding table
                                                                
    printSymbolEncodingBitified(symbols,                                 // Print the huffman encoding of every symbol in the alphabet
                                symbol_count, 
                                asciiToHuffman);

    compressed_size = generateEncodedFileBitified(original_filename,     // Perform huffman encoding on the sample text and, 
                                                  compressed_filename,   // store the result in a .huf file
                                                  asciiToHuffman);  

    /******************************** Decoding ********************************/
    treeDecodingBitByBit(compressed_filename, decompressed_filename, huffmanRoot);  // Decode the compressed file in a bit-by-bit manner
    //exit(0);    

    unsigned code_max_bits = 0;
    code_max_bits = maxTreeDepth(huffmanRoot);

    lutCreation(huffmanRoot, symbol_count, all_luts, &tables_needed, code_max_bits);

    lutDecoding(compressed_filename, decompressed_filename, all_luts, code_max_bits);

    //testing
    for(int t = 0; t < 2; t++){
        for(int g = 0; g < 2; g++){
            printf("look up table from all_luts[%d][%d] vals: %c and %d\n", t, g, all_luts[t][g].symbol, all_luts[t][g].code_length);
        }
    }

    lutFreeAll(all_luts, &tables_needed);
    freeHuffmanTree(huffmanRoot);

    /******************************** Prints Statements ********************************/
    printf("Sample Size: %d bits\n", sample_size*8);
    printf("Symbol Count: %d\n", symbol_count);
    printf("Compressed Size: %d bits\n", compressed_size);

    return 0;
}