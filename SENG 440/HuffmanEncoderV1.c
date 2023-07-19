// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "HuffmanHeader.h"



void printCodes(node_t* root, unsigned char* buff, int bit_count, huffCode_t* asciiToHuffman){
 
    // Assign 0 to left edge and recur
    if (root->leftChild) {
 
        buff[bit_count] = 0;
        printCodes(root->leftChild, buff, bit_count + 1, asciiToHuffman);
    }
 
    // Assign 1 to right edge and recur
    if (root->rightChild) {
 
        buff[bit_count] = 1;
        printCodes(root->rightChild, buff, bit_count + 1, asciiToHuffman);
    }
 
    // If this is a leaf node, then
    // it contains a symbol
    if (isLeaf(root)) {
        int ascii_index = (int) root->symbol;
        // Unsigned char bit_cursor = 0b10000000;
        // Add the symbol and its encoding to a table
        asciiToHuffman[ascii_index].symbol = root->symbol;
        asciiToHuffman[ascii_index].code_length = bit_count;

        // USED FOR TESTING - represents bits of the Huffman code as chars
        for (int i = 0; i < bit_count; i++) {
            asciiToHuffman[ascii_index].code[i] = buff[i];
        }
        

        // V2 - represents bits of the Huffman code as bits
        for (int i = 0; i < bit_count; i++) {
            asciiToHuffman[ascii_index].bitCode <<= 1;
            if (buff[i]  == 1) {
                asciiToHuffman[ascii_index].bitCode |= 1;
            } 
            else if(buff[i] == 0) {
                asciiToHuffman[ascii_index].bitCode |= 0;
            } 
            else {
                printf("Tried to convert non 1 or 0 to bit!\n");
                exit(1);
            }
        }

        // TESTING - prints the symbols and their bit codes 
        printf("The bit code for symbol %c is: ",asciiToHuffman[ascii_index].symbol);
        for (int i = bit_count - 1; i >= 0; i--) {
        unsigned int mask = 1u << i;    // Sets ith bit to 1 and rest to 0 for unsigned int literal
        putchar((asciiToHuffman[ascii_index].bitCode & mask) ? '1' : '0');
        }
        printf("\n");

        // for (int i = 0; i < MAX_CODE_LENGTH/8;) {
        //     if (buff[i] == 1){
        //         asciiToHuffman[ascii_index].code[i] |= bit_cursor;
        //         bit_cursor >>= 1;
        //     }
        //     if (bit_cursor == 0){ // Out of bits
        //         bit_cursor = 0b10000000;
        //         i++;
        //     }
        // }
    }
}

void generatehuffmanCodes(node_t* root, huffCode_t* asciiToHuffman){
    unsigned char buff[100];
    // Initialize each symbol such that its index matches to its ascii code (i.e. asciiToHuffman[65] = 'A')
    for(int i = 0; i < MAX_SYMBOLS; i++){
        asciiToHuffman[i].symbol = (unsigned char) i;
        memset(asciiToHuffman->code, 0, MAX_CODE_LENGTH);
    }
    
    printCodes(root, buff, 0, asciiToHuffman);
}


void printSymbolEncoding(unsigned char* symbols, int symbol_count,  huffCode_t* asciiToHuffman){
    char symbol;
    char *control_characters[] = {"NUL","SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", 
                                  "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", 
                                  "DC1", "DC2","DC3", "DC4", "NAK", "SYN", "ETB", "CAN", 
                                  "EM", "SUB", "ESC", "FS", "GS", "RS", "US"};

    for(int i = 0; i < MAX_SYMBOLS; i++){
        symbol = asciiToHuffman[i].symbol;
        if(symbolIndex(symbols, symbol_count, symbol) != -1){
            // Print symbol
            if(symbol < 8 || (symbol > 15 && symbol < 25) || symbol == 26 || symbol == 27){
                printf("Symbol: %s   Encoding: ", control_characters[(int) symbol]);
            }
            else if((symbol > 7 && symbol < 16) || symbol == 25 ||(symbol > 27 && symbol < 32)){
                printf("Symbol: %s    Encoding: ", control_characters[(int) symbol]);
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
            // Print huffman code
            for(int j = 0; j < asciiToHuffman[i].code_length; j++){
                printf("%d", asciiToHuffman[i].code[j]);
            }
            printf("\n");
        }
    }

}

void bitify(char* code, char* compressed){
    // Parameters: Huffman code to be bitified; compressed string reference
    // Returns: # of bits written
    //char arr[] = {0b10000000, 0b01000000, 0b00100000};

};

int generateEncodedFile(const char* sample_filename, const char* encoded_filename, huffCode_t* asciiToHuffman){
    // Sample file
    FILE* fp_sample = fopen(sample_filename, "r");
    if(fp_sample == NULL){
        printf("Failed to open sample file!\n");
        exit(1);
    }

    // Compressed file
    FILE* fp_compressed = fopen(encoded_filename, "w");
    if(fp_compressed == NULL){
        printf("Failed to create encoding file!\n");
        exit(1);
    }
   
    // Variables
    unsigned char sample[MAX_FILE_LENGTH];
    unsigned char encodedSample[MAX_ENCODING_LENGTH]; 
    int index;
    int bitstoWrite = 0;
    int sample_size = fread(sample, sizeof(unsigned char), MAX_FILE_LENGTH, fp_sample);
    
    for(int i = 0; i < sample_size; i++){
        index = (int) sample[i];
        for(int j = 0; j < asciiToHuffman[index].code_length; j++){
            encodedSample[bitstoWrite] = asciiToHuffman[index].code[j];
            bitstoWrite++;
        }
    }

    int huff_sample_size = fwrite(encodedSample, sizeof(unsigned char), bitstoWrite,  fp_compressed);
    
    fclose(fp_sample);
    fclose(fp_compressed);

    return huff_sample_size;
}

// Huffman decoding
void brute_decode_text(int* encodedText, huffCode_t* asciiToHuffman)
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
                if(asciiToHuffman[y].code[i] != encodedText[i]){ // if no match
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

void treeDecodingBitByBit(char* huf_filename, char* decoded_filename, node_t* root, const int size){
    FILE* fpEncoded = fopen(huf_filename, "r");
    if(fpEncoded == NULL){
        printf("Failed to open encoded file!\n");
        exit(1);
    }
    
    node_t* cur = root;
    unsigned char encoded[size];
    unsigned char decoded[size/8];
    int index = 0;

    FILE* fpDecoded = fopen(decoded_filename, "w");
    if(fpDecoded == NULL){
        printf("Failed to create decoded file!\n");
        exit(1);
    }

    char str[] = "Tree Decoding Below:\n";
    fwrite(str, sizeof(str) - sizeof(char), 1, fpDecoded); // Print str but not '\0'

    fread(encoded, sizeof(unsigned char), size, fpEncoded);
    for(int i = 0; i < size;){
        if(!isLeaf(cur)){
            if(encoded[i] == (unsigned char) 0){
                cur = cur->leftChild;
            }
            else if(encoded[i] == (unsigned char) 1){
                cur = cur->rightChild;
            }
            i++;
        }
        else{
            decoded[index] = cur->symbol;
            index++;
            cur = root;
        }
    }
    decoded[index] = cur->symbol;
    index++;
    
    fwrite(decoded, sizeof(unsigned char), index, fpDecoded);
    fclose(fpEncoded);
    fclose(fpDecoded);
}


void lutPopulate(node_t* root,  unsigned char* buff, unsigned int bit_count, unsigned int longest_code_exp, lut** all_luts, unsigned int* lut_num, unsigned int* row_count){

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

        }

    }

}



int lutCreation(node_t* root, int alpha_size, lut** all_luts, unsigned int* tables_needed){
    // Start with making one large table
    unsigned int table_size = 1 << alpha_size; // Equivalent to 2^alpha_size

    // TESTING
    printf("\n\nalpha size is: %d\n", alpha_size);
    printf("Table size is: %d\n",table_size);
    printf("Current mem val being allocated: %d\n",(sizeof(lut)*(MAX_ROWS_PER_TABLE)));



    // Find the longest possible code length
    unsigned int longest_code = 1;
    int longest_code_exp = 0;
    while( longest_code <= alpha_size){
        longest_code <<= 1;
        longest_code_exp++;    
    }

    // Create the largest possible index
    unsigned int max_index = (1u << longest_code_exp) - 1;
    printf("\n\nvalue from the largest index is: %d\n\n", max_index);  

    // Determine how many LUTS are needed
    (*tables_needed) = (max_index / MAX_ROWS_PER_TABLE) + 1;  
    printf("\n\nvalue for tables needed is: %d\n\n", *tables_needed);  
   

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
    lutPopulate(root, buff , 0, longest_code_exp, all_luts, &lut_num, &row_counter);

    return longest_code_exp;

}

void lutFreeAll( lut** all_luts, unsigned int* tables_needed){
    // Interates over all possible tables and frees memory
    for(int i = 0; i < (*tables_needed); i++){
        free(all_luts[i]);
    }
}

void lutDecoding(char* huf_filename, char* decoded_filename, lut** all_luts, const int barrel_shifter, const int size){
    FILE* fpEncoded = fopen(huf_filename, "r");
    if(fpEncoded == NULL){
        printf("Failed to open encoded file!\n");
        exit(1);
    }

        FILE* fpDecoded = fopen(decoded_filename, "a");
    if(fpDecoded == NULL){
        printf("Failed to create decoded file!\n");
        exit(1);
    }

    char str[] = "\n\nLUT Decoding Below:\n";
    fwrite(str, sizeof(str) - sizeof(char), 1, fpDecoded);

    // Go through the encoded file by barrel_shifter length sections 
    // and search each table for that index
    unsigned char encoded[size];
    fread(encoded, sizeof(unsigned char), size, fpEncoded);


    printf("\n TESTING \n");
    for(int i = 0; i < size; i++) {
        printf("%u", encoded[i]);
    }
    printf("\n ");

    int decoded_bit_count = 0;

    while(decoded_bit_count < size){
        unsigned int encoded_section = 0;
        // Get barrel shifter section of encoded message     
        for (int i = 0; i < barrel_shifter; i++) {
            encoded_section <<= 1;
            if (encoded[i]  == 1) {
                encoded_section |= 1;
            } 
            else if(encoded[i] == 0) {
                encoded_section |= 0;
            } 
            else {
                printf("Tried to convert non 1 or 0 to bit!\n");
                exit(1);
            }
        }

        // Find the index that is the same value as the encoded_section
        // use all_luts[i][x] and the MAX_ROWS_PER_TABLE thing
        unsigned int table_index = 0;
        unsigned int table_num = 0;
        table_index = encoded_section % MAX_ROWS_PER_TABLE;
        table_num = encoded_section / MAX_ROWS_PER_TABLE;

        //Set to the code_length at that index and print the symbol
        unsigned int temp_bit_count = all_luts[table_num][table_index].code_length;
        unsigned char symbol = all_luts[table_num][table_index].symbol;

        fwrite(&symbol, sizeof(unsigned char), 1, fpDecoded);

        decoded_bit_count = decoded_bit_count + temp_bit_count;

        // Shift the encoded message the code length of the symbol
        for(int i = 0; i < (size - decoded_bit_count); i++){
            encoded[i] = encoded[i + temp_bit_count];
        }
        
    }

    fclose(fpEncoded);
    fclose(fpDecoded);
}

int prologue(const char* sample_filename, unsigned char* symbols, float* probabilities, int* sample_size){
    FILE* fp = fopen(sample_filename, "r");  // File pointer
    unsigned char cur_symbol;                // Holds the current symbol 
    int symbol_count = 0;                    // # of unique ASCII values
    int frequencies[MAX_SYMBOLS];            // Frequencies of each symbol
    unsigned char sample[MAX_FILE_LENGTH];   // Buffer for sample text

    *sample_size = fread(sample, sizeof(unsigned char), MAX_FILE_LENGTH, fp);  // Retrieve sample text

    for (int i = 0; i < *sample_size; i++){
        cur_symbol = sample[i];
        if (symbol_count > MAX_SYMBOLS){                                       // If MAX
            printf("Symbol count exceeded!\nAbort to avoid overflow!\n");
            exit(1);
        }                                                                       //
        else if(symbolIndex(symbols, symbol_count, cur_symbol) == -1){          // If Symbol Doesnt Exist
            symbols[symbol_count] = cur_symbol;
            frequencies[symbol_count] = 1;
            symbol_count++;
        }
        else{                                                                   // 
            frequencies[symbolIndex(symbols, symbol_count, cur_symbol)]  += 1;  // If Symbol Exist, Increment Symbols Index
        }
    }
    
    float inv_sample_size = *sample_size;
    // Precomputed for efficiency 
    inv_sample_size = 1/inv_sample_size;
    for (int i = 0; i < symbol_count; i++){
        probabilities[i] = frequencies[i]*inv_sample_size;
    }

    fclose(fp);
    return symbol_count;
}


int main(){
    /******************************** Variables ********************************/
    char sample_filename[] = "sample3.txt";  // Filename for sample
    char huf_filename[64];                   // Filename for encoded sample
    char decoded_filename[64];               // Filename for encoded sample
    unsigned char symbols[MAX_SYMBOLS];      // Every unique ASCII symbol in the sample (unsorted)
    float probabilities[MAX_SYMBOLS];        // Corresponding probability for each symbol
    int sample_size;                         // Size of the sample in bits
    int huff_sample_size;                    // Size of the encoded sample in bits
    int symbol_count;                        // Size of the alphabet
    node_t* huffmanRoot;                     // Root node of the huffman tree
    huffCode_t asciiToHuffman[MAX_SYMBOLS];  // Huffman code of each symbol is stored at the index of its ascii encoding
    clock_t start, end;                      // Used for benchmarking
    lut* all_luts[MAX_TABLES];               // An array to store all the look up tables
    unsigned int tables_needed = 0;             // Used to keep track of the LUTs needed for an alphabet

    /******************************** Initialization ********************************/
    stripFilename(sample_filename, huf_filename);      // Retrieve filename
    stripFilename(sample_filename, decoded_filename);  // Retrieve filename
    strncat(huf_filename, "_compressed.huf", 16);      // Create encoded filename
    strncat(decoded_filename, "_decoded.txt", 14);     // Create decoded filename 

    symbol_count = prologue(sample_filename, symbols,  // Retrieves sample text and size, determines symbols 
                   probabilities, &sample_size);       // and their probabilities, and return symbol_count
    

    /******************************** Encoding ********************************/
    huffmanRoot = buildHuffmanEncTree(symbols,               // Construct a Huffman encoding tree for the alphabet
                   probabilities, symbol_count);             // with a min-heap implementaion 
    
    generatehuffmanCodes(huffmanRoot, asciiToHuffman);       // Recursively traverse the tree, generate the huffman code for every leaf's symbol,
                                                             // and populate the encoding table

    huff_sample_size = generateEncodedFile(sample_filename,  // Generate the huffman encoding of the sample,
                       huf_filename, asciiToHuffman);        // output it to a new file, and return the size

    
    
    /******************************** Decoding ********************************/
    start = clock();
    treeDecodingBitByBit(huf_filename, decoded_filename, huffmanRoot, huff_sample_size);  // No optimizations
    end = clock();

    unsigned int max_code_bits = 0;
    max_code_bits = lutCreation(huffmanRoot, symbol_count, all_luts, &tables_needed);

    lutDecoding(huf_filename, decoded_filename, all_luts, max_code_bits, huff_sample_size);

    //testing
    for(int t = 0; t < 2; t++){
        for(int g = 0; g < MAX_ROWS_PER_TABLE; g++){
            printf("look up table from all_luts[%d][%d] vals: %c and %d\n", t, g, all_luts[t][g].symbol, all_luts[t][g].code_length);
        }
    }

    lutFreeAll(all_luts, &tables_needed);

    /******************************** Prints Statements ********************************/
    printSymbolEncoding(symbols, symbol_count, asciiToHuffman);
    printf("Sample Size: %d bits\n", sample_size*8);
    printf("Symbol Count: %d\n", symbol_count);
    printf("Encoded Sample Size: %d bits\n", huff_sample_size);
    printf("Time Elapsed Raw: %ld\n", end - start);

    return 0;
}