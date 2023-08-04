// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdint.h>
#include "HuffmanHeader.h"


/***** Min Heap *****/
node_t* allocateNewNode(uint_fast8_t symbol, float probability) {
    // Dynamically allocate memory for a new node 
    node_t* temp = (node_t*)malloc(sizeof(node_t));
    assert(temp != NULL);
    // printf("Memory Allocated: %p\n", temp);
    temp->leftChild = NULL;
    temp->rightChild = NULL;
    temp->symbol = symbol;
    temp->probability = probability;
    return temp;
}

void swapNodes(node_t** n1, node_t** n2) {
    // Swap two nodes 
    node_t* temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

inline uint_fast32_t isLeaf(node_t* node) {
    // Returns 1 if a node is a leaf; returns 0 otherwise
    return !(node->leftChild) && !(node->rightChild);
}

void heapify(minHeap_t* minHeap, uint_fast32_t i) {
    // Standard min-heapify  
    uint_fast32_t minI = i;
    uint_fast32_t leftI = 2*i+1;
    uint_fast32_t rightI = 2*i+2;

    if (leftI < minHeap->numNodes && minHeap->treeArray[leftI]->probability < minHeap->treeArray[minI]->probability) {
        minI = leftI;
    }
    
    if (rightI < minHeap->numNodes && minHeap->treeArray[rightI]->probability < minHeap->treeArray[minI]->probability) {
        minI = rightI;
    }
 
    if (minI != i) {
        // Recurse 
        swapNodes(&minHeap->treeArray[minI],&minHeap->treeArray[i]);
        heapify(minHeap, minI);
    }
}

node_t* popMin(minHeap_t* minHeap) {
    // Pop the min and heapify before returning
    node_t* temp = minHeap->treeArray[0];
    minHeap->treeArray[0] = minHeap->treeArray[minHeap->numNodes - 1];
 
    minHeap->numNodes--;
    heapify(minHeap, 0);
 
    return temp;
}

void insertMinHeap(minHeap_t* minHeap, node_t* newNode) {
    // Insert a new node into the heap
    minHeap->numNodes++;
    uint_fast32_t i = minHeap->numNodes - 1;
 
    while (i && newNode->probability < minHeap->treeArray[(i - 1) / 2]->probability) {
 
        minHeap->treeArray[i] = minHeap->treeArray[(i - 1) / 2];
        i = (i - 1) / 2;
    }
 
    minHeap->treeArray[i] = newNode;
}

minHeap_t* createMinHeap(uint_fast32_t maxNodes) {
    // Create minHeap object and allocate necessary memory  
    minHeap_t* minHeap = (minHeap_t*)malloc(sizeof(minHeap_t));
    assert(minHeap != NULL);
 
    minHeap->numNodes = 0;
    minHeap->maxNodes = maxNodes;
    
    minHeap->treeArray = (node_t**)malloc(minHeap->maxNodes * sizeof(node_t*));
    assert(minHeap->treeArray != NULL);
    return minHeap;
}

minHeap_t* initMinHeap(uint_fast8_t alph[], float prob[], uint_fast32_t size) {
    // NOTE: If there are n symbols in our alphabet, 
    // then there are n-1 internal nodes for Huffman Coding, for a total of 2n - 1 nodes for a tree
    uint_fast32_t numLeaves = size;
    minHeap_t* minHeap = createMinHeap(2*numLeaves - 1);
    
    uint_fast32_t i;
    for (i = 0; i < size; i++){
        minHeap->treeArray[i] = allocateNewNode(alph[i], prob[i]);
    }
 
    minHeap->numNodes = numLeaves;

    // Finish building min-heap by calling heapify on a subset of nodes
    // NOTE: j is signed to avoid undefined behavior when j == 0
    int_fast32_t j;
    for (j = (numLeaves - 2)/2; j >= 0; j--) {
        heapify(minHeap, j);
    }
 
    return minHeap;
}

/***** Huffman Encoding *****/
node_t* buildHuffmanEncTree(uint_fast8_t alph[], float prob[], uint_fast32_t size) {
    node_t *left, *right, *top;
 
    // Create, initialize, and build the min heap 
    minHeap_t* minHeap = initMinHeap(alph, prob, size);
 
    // Iterate while size of heap doesn't become 1
    while (!(minHeap->numNodes == 1)) {
 
        // Extract the two minimum probability nodes
        left = popMin(minHeap);
        right = popMin(minHeap);

        // Create new node: Internal nodes identified by '~', set probability to the sum of its children, set child pointers 
        top = allocateNewNode('~', left->probability + right->probability);

        top->leftChild = left;
        top->rightChild = right;
 
        insertMinHeap(minHeap, top);
    }

    minHeap_t* lost_soul = minHeap;
    node_t* root = popMin(minHeap);

    free(lost_soul->treeArray);
    free(lost_soul);

    // Return the root of the Huffman tree
    return root;
}

void printCodesBitified(node_t* root, uint_fast8_t* buff, uint_fast32_t bit_count, huffCode_t* asciiToHuffman) {
    // Assign 0 for left edge and recur
    if (root->leftChild) {
        buff[bit_count] = 0;
        printCodesBitified(root->leftChild, 
        buff, bit_count + 1, 
        asciiToHuffman);
    }

    // Assign 1 for right edge and recur
    if (root->rightChild) {
        buff[bit_count] = 1;
        printCodesBitified(root->rightChild, 
        buff, bit_count + 1, 
        asciiToHuffman);
    }
 
    // If this is a leaf node, then it contains a symbol
    if (isLeaf(root)) {                                          
        uint_fast8_t ascii_index = root->symbol;                 // Index the table at the symbol's acsii code and,
        asciiToHuffman[ascii_index].code_length = bit_count;     // store the length of its encoding in the table
	    uint_fast32_t i;
        for (i = 0; i < bit_count; i++) {
            asciiToHuffman[ascii_index].bitcode +=  buff[i] * (1U << i);  // Represent the encoding with the right amount of bits
        }
    } 
}

void generatehuffmanCodesBitified(node_t* root, huffCode_t* asciiToHuffman) {
    // Initialize each symbol in the table such that its index matches its ascii encoding (i.e. asciiToHuffman[65].symbol = 'A')
    uint_fast8_t buff[128];
    uint_fast32_t i;
    for(i = 0; i < MAX_SYMBOLS; i++){
        asciiToHuffman[i].symbol = i;
        asciiToHuffman[i].bitcode = 0;
        asciiToHuffman[i].code_length = 0;
    }
    
    printCodesBitified(root, buff, 0, asciiToHuffman);
}

void generateEncodedFileBitified(const char* sample_filename, const char* encoded_filename, huffCode_t* asciiToHuffman) {
    uint_fast8_t sample[MAX_FILE_LENGTH];                      // Contains orginal data
    uint_fast16_t encodedSample[MAX_COMPRESSED_LENGTH] = {0};  // Initialized at zero so only 1s have to be set 
    uint_fast8_t ascii_index;                                  // Contains the ASCII code of the current symbol
    uint_fast32_t cur_Index = 0;                               // # of shorts in compressed data
    uint_fast32_t sample_size;                                 // # of symbols in original data
    uint_fast32_t bit_position = 0;                            // Keeps track of bit position
    uint_fast32_t bit_count = 0;                               // # of bits in compressed data
    FILE* fp_sample = fopen(sample_filename, "r");             // Points to the original file
    FILE* fp_compressed = fopen(encoded_filename, "wb");       // Poitns to the compressed file
    assert(fp_sample != NULL);
    assert(fp_compressed != NULL);
   
    sample_size = fread(sample, sizeof(uint_fast8_t), MAX_FILE_LENGTH/sizeof(uint_fast8_t), fp_sample);  // Retrieve original data
    uint_fast32_t i;
    for (i = 0; i < sample_size; i++) {  // Iterate 1 symbol per loop iteration
        ascii_index = sample[i];         // Get ASCII code for current symbol
        uint_fast32_t j;    
	    for (j = 0; j < asciiToHuffman[ascii_index].code_length; j++) {  // Iterate through code length
                if (asciiToHuffman[ascii_index].bitcode & (1U << j)) {   // If a bit is set in the bitcode, then
                    encodedSample[cur_Index] |= (1U << bit_position);    // set the matching bit in the encoding 
                }
                bit_count++;              // Increment bit count
                bit_position++;           // Increment bit position
                if(bit_position >= 16) {  // If bit_postion == 16
                    bit_position = 0;     // Reset bit_position
                    cur_Index++;          // Move to next short
                }
            }
    }

    fwrite(&bit_count, sizeof(uint_fast32_t), 1,  fp_compressed);                 // First 4 bytes of the compressed file contain the compressed size in bits
    fwrite(encodedSample, sizeof(uint_fast16_t), cur_Index + 1,  fp_compressed);  // Rest of file contains encoding

    fclose(fp_sample);
    fclose(fp_compressed);
}

/***** LUT *****/
void lutPopulate(node_t* root,  uint_fast8_t* buff, uint_fast32_t bit_count, uint_fast32_t longest_code_exp, lut** all_luts, uint_fast32_t* lut_num, uint_fast32_t* row_count) {
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
        uint_fast32_t bit_val = 0;        
        uint_fast32_t i;
        for (i = 0; i < bit_count; i++) {
            bit_val <<= 1;
            if (buff[i]  == 1) {
                bit_val |= 1;
            } 
            else if(buff[i] == 0) {
                bit_val |= 0;
            }  
        }

        // Still need to fill in all possible combos of bits after the buffer code
        uint_fast32_t min_bit_val = bit_val;
        uint_fast32_t max_bit_val = bit_val;
        uint_fast32_t bit_dif = longest_code_exp - bit_count;
        uint_fast32_t j;
        for(j = 0; j < bit_dif; j++) {
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

void lutCreation(node_t* root, lut** all_luts, uint_fast32_t* tables_needed, uint_fast32_t longest_code_exp) {
    // Create the largest possible index	
    uint_fast32_t max_index = (1u << longest_code_exp) - 1;	
    // Determine how many LUTS are needed	
    (*tables_needed) = (max_index / MAX_ROWS_PER_TABLE) + 1;  	
   	
    // Allocate memory for required tables 	
    uint_fast32_t i;
    for(i = 0; i < (*tables_needed); i++){	
        lut* mem_per_table = malloc(sizeof(lut)*(MAX_ROWS_PER_TABLE));	
        assert(mem_per_table != NULL);
        all_luts[i] = mem_per_table;	
    }	
    // Populate the tables with the symbols from the tree	
    uint_fast8_t buff[100];	
    uint_fast32_t lut_num = 0;	
    uint_fast32_t row_counter = 0;	

    lutPopulate(root, buff , 0, longest_code_exp, all_luts, &lut_num, &row_counter);
}

/***** Huffman Decoding *****/
void treeDecodingBitByBit(char* huf_filename, char* decompressed_filename, node_t* root) {
    node_t* cur = root;                                         // Cursor for traversing Huffman tree
    uint_fast16_t compressed[MAX_COMPRESSED_LENGTH];            // Contains compressed data
    uint_fast8_t decompressed[MAX_FILE_LENGTH] = {0};           // Contains decompressed data
    uint_fast32_t compressed_bits = 0;                          // # of bits in compressed data
    uint_fast32_t cur_Index = 0;                                // Indexes the current short
    uint_fast32_t num_symbols = 0;                              // Counts # of decompressed symbols
    uint_fast32_t bit_position = 0;                             // Keeps track of bit position
    FILE* fp_compressed= fopen(huf_filename, "rb");             // Points to the compressed file
    FILE* fp_decompressed = fopen(decompressed_filename, "w");  // Points to the decompressed file, or creates it if it doesn't exist
    assert(fp_compressed != NULL);
    assert(fp_decompressed != NULL);
    
    fread(&compressed_bits, sizeof(uint_fast32_t), 1, fp_compressed);                                      // Retrieve # of compressed bits
    fread(compressed, sizeof(uint_fast16_t), MAX_COMPRESSED_LENGTH/sizeof(uint_fast16_t), fp_compressed);  // Retrieve compressed data 
    uint_fast32_t i;
    for (i = 0; i < compressed_bits; i++) {  // Iterate through 1 compressed bit per loop iteration

        if (!isLeaf(cur)) {                                      // If the current node is not a leaf,
            if (compressed[cur_Index] & (1U << bit_position)) {  // and if the current compressed bit is a 1,
                cur = cur->rightChild;                           // then traverse the right edge of the current node
            }
            else {                                               // Else, we know the current compressed bit is a 0,
                cur = cur->leftChild;                            // so traverse the left edge of the current node
            }

            bit_position++;            // Increment the bit position for the current word
            if (bit_position >= 16) {  // If bit position exceeds word-length,
                bit_position = 0;      // then reset bit position,
                cur_Index++;           // and move on to the next word
            }
        }
        else {                                        // Else, we know the current node is a leaf and we have sucessfully decoded a symbol,
            decompressed[num_symbols] = cur->symbol;  // so store the symbol in the decompressed array,
            num_symbols++;                            // increment the decompressed symbol count,
            cur = root;                               // set the current node back to the root of the huffman tree,
            i--;                                      // and decrement the loop counter, since 1 dedicated loop iteration is required to decode a symbol (Otherwise we skip 1 encoded bit after each symbol decompression)
        }
    }

    // Decompress the last symbol, if it exists
    if (isLeaf(cur)) {
        decompressed[num_symbols] = cur->symbol;
        num_symbols++;
    }

    fwrite(decompressed, sizeof(uint_fast8_t), num_symbols, fp_decompressed);
    fclose(fp_compressed);
    fclose(fp_decompressed);
}

void lutDecoding(const char* huf_filename, const char* decompressedLUT_filename, lut** all_luts, const int barrel_shifter) {
    FILE* fp_compressed = fopen(huf_filename, "rb");
    FILE* fp_decompressed = fopen(decompressedLUT_filename, "wb");
    uint_fast32_t compressed_bits = 0;       // # of bits in compressed data
    uint_fast16_t compressed[MAX_COMPRESSED_LENGTH];
    uint_fast8_t decompressed[MAX_FILE_LENGTH];

    assert(fp_compressed != NULL);
    assert(fp_decompressed != NULL);
    
    // Go through the compressed file by barrel_shifter length sections (longest code length)
    // and search each table for that index
    
    fread(&compressed_bits, sizeof(uint_fast32_t), 1, fp_compressed);  
    fread(compressed, sizeof(uint_fast16_t), MAX_COMPRESSED_LENGTH/sizeof(uint_fast16_t), fp_compressed);
    
    uint_fast32_t decoded_bit_count = 0;
    uint_fast32_t decoded_symbol_count = 0;
    uint_fast32_t cur_index = 0;
    int_fast32_t bit_pos = 0;
    while(decoded_bit_count < compressed_bits) {     // Iterate through until all bits are decoded
    
    // Get barrel shifter size section of compressed message     
        uint_fast32_t compressed_section = 0;       
        uint_fast32_t j;
        for (j = decoded_bit_count; j < barrel_shifter + decoded_bit_count; j++) {

            if (compressed[cur_index] & (1U << bit_pos )) {
                compressed_section |= 1u << ((barrel_shifter - 1) - (j - decoded_bit_count));
            }
            bit_pos++;
            if(bit_pos >= 16){
                bit_pos = 0;
                cur_index++;
            }
        }

        // Find the index that is the same value as the compressed_section
        // use all_luts[i][x] and the MAX_ROWS_PER_TABLE variable
        uint_fast32_t table_index = 0;
        uint_fast32_t table_num = 0;

        // TODO: Posible optimization by inlining assembly division and retrieving result and remainder registers from one division operation
        table_index = compressed_section % MAX_ROWS_PER_TABLE;
        table_num = compressed_section / MAX_ROWS_PER_TABLE;

        // Set to the code_length at that index and print the symbol
        uint_fast32_t temp_bit_count = all_luts[table_num][table_index].code_length;
        uint_fast8_t symbol = all_luts[table_num][table_index].symbol;

        decompressed[decoded_symbol_count] = symbol;
        decoded_symbol_count++;

        bit_pos -= (barrel_shifter - temp_bit_count);      // update the bit position to account for not decoded bits
        if (bit_pos < 0) {                                 // if the bit_pos is negative we need to go back to previous index
            cur_index--;
            bit_pos = 16 + bit_pos;
        }
        
        decoded_bit_count = decoded_bit_count + temp_bit_count;
    }

    fwrite(decompressed, sizeof(uint_fast8_t), decoded_symbol_count, fp_decompressed);
    fclose(fp_compressed);
    fclose(fp_decompressed);
}

/***** Misc Helpers *****/
void printArr(int arr[], int n) {
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}

int symbolIndex(uint_fast8_t* symbols, int symbol_count, uint_fast8_t symbol) {
    // If a symbol exists in symbols, return its index
    // Otherwise return -1
    int i;
    for (i = 0; i < symbol_count; i++) {
            if (symbols[i] == symbol) {
                return i;
            }
        }

    return -1;
}

int prologue(const char* original_filename, uint_fast8_t* symbols, float* probabilities, uint_fast32_t* sample_size) {
    FILE* fp = fopen(original_filename, "r");  // File pointer
    uint_fast8_t cur_symbol;                // Holds the current symbol 
    uint_fast32_t symbol_count = 0;               // # of unique ASCII values
    uint_fast32_t frequencies[MAX_SYMBOLS];       // Frequencies of each symbol
    uint_fast8_t sample[MAX_FILE_LENGTH];   // Buffer for sample text
    assert(fp != NULL);
    *sample_size = fread(sample, sizeof(uint_fast8_t), MAX_FILE_LENGTH, fp);  // Retrieve sample text
    uint_fast32_t i;
    for (i = 0; i < *sample_size; i++) {                                    // Iterates through sample text 1 char at a time
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
    uint_fast32_t j;
    for (j = 0; j < symbol_count; j++) {
        probabilities[j] = frequencies[j]*inv_sample_size;
    }

    fclose(fp);
    return symbol_count;
}

void printSymbolEncodingBitified(huffCode_t* asciiToHuffman) {
    uint_fast8_t symbol;
    char *control_characters[] = {
        "NUL","SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", 
        "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", 
        "DC1", "DC2","DC3", "DC4", "NAK", "SYN", "ETB", "CAN", 
        "EM", "SUB", "ESC", "FS", "GS", "RS", "US"};

    uint_fast32_t i;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        if (asciiToHuffman[i].code_length > 0) {
            // Print symbol appropriately 
            symbol = asciiToHuffman[i].symbol;
            if (symbol < 8 || (symbol > 15 && symbol < 25) || symbol == 26 || symbol == 27) {
                printf("Symbol: %s   Encoding: ", control_characters[(uint_fast8_t) symbol]);
            }
            else if ((symbol > 7 && symbol < 16) || symbol == 25 ||(symbol > 27 && symbol < 32)) {
                printf("Symbol: %s    Encoding: ", control_characters[(uint_fast8_t) symbol]);
            }
            else if (symbol == 32) {
                printf("Symbol: space Encoding: ");
            }
            else if (symbol == 127) {
                printf("Symbol: DEL   Encoding: ");
            }
            else {
                printf("Symbol: %c     Encoding: ", symbol);
            }

            uint_fast8_t code[MAX_CODE_LENGTH] = {0};
            uint_fast32_t j;
            for (j = 0; j < asciiToHuffman[i].code_length; j++) {
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

int maxTreeDepth(node_t* root) {
    if (root == NULL) {
        return 0;
    } 
    else {
        int leftDepth = maxTreeDepth(root->leftChild);
        int rightDepth = maxTreeDepth(root->rightChild);
        int max = (leftDepth > rightDepth) ? leftDepth : rightDepth;
        return max + 1;
    }
}

void freeHuffmanTree(node_t* node) {
    if (node == NULL) {
        return;
    }

    freeHuffmanTree(node->leftChild);
    freeHuffmanTree(node->rightChild);
    
    free(node);
}

inline void lutFreeAll( lut** all_luts, uint_fast32_t* tables_needed) {
        // Interates over all possible tables and frees memory	
    uint_fast32_t i;
    for(i = 0; i < (*tables_needed); i++){	
        free(all_luts[i]);	
    }	
}


int main(int argc, char* argv[]) {
    /******************************** Variables ********************************/
    char input_filename[MAX_FILENAME_LENGTH];        // Filename provided by user input
    char original_filename[MAX_PATH_LENGTH];         // Filename for original file
    char compressed_filename[MAX_PATH_LENGTH];       // Filename for compressed file
    char decompressed_filename[MAX_PATH_LENGTH];     // Filename for decompressed file (Bitified)
    char decompressedLUT_filename[MAX_PATH_LENGTH];  // Filename for decompressed file (LUT)
    uint_fast8_t symbols[MAX_SYMBOLS];               // Every unique ASCII symbol in the sample (unsorted)
    float probabilities[MAX_SYMBOLS];                // Corresponding probability for each symbol
    uint_fast32_t sample_size;                       // Size of the sample in bits
    uint_fast32_t symbol_count;                      // Size of the alphabet
    node_t* huffmanRoot;                             // Root node of the huffman tree
    huffCode_t asciiToHuffman[MAX_SYMBOLS];          // Huffman code of each symbol is stored at the index of its ascii encoding
    lut* all_luts[MAX_TABLES];                       // An array to store all the look up tables
    uint_fast32_t tables_needed = 0;                 // Used to keep track of the LUTs needed for an alphabet

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
        strncpy(decompressedLUT_filename, "decompressed/", MAX_PATH_LENGTH - 1);

        // Null-terminate the destination buffers
        original_filename[MAX_PATH_LENGTH - 1] = '\0';
        compressed_filename[MAX_PATH_LENGTH - 1] = '\0';
        decompressed_filename[MAX_PATH_LENGTH - 1] = '\0';
        decompressedLUT_filename[MAX_PATH_LENGTH - 1] = '\0';

        // Retrieve filename without file extension
        char stripped_filename[MAX_FILENAME_LENGTH];
        strncpy(stripped_filename, input_filename, MAX_FILENAME_LENGTH - 1);
        strtok(stripped_filename, ".");

        // Append filename to file paths
        strncat(original_filename, stripped_filename, MAX_PATH_LENGTH - strlen(original_filename) - 1);
        strncat(compressed_filename, stripped_filename, MAX_PATH_LENGTH - strlen(compressed_filename) - 1);
        strncat(decompressed_filename, stripped_filename, MAX_PATH_LENGTH - strlen(decompressed_filename) - 1);
        strncat(decompressedLUT_filename, stripped_filename, MAX_PATH_LENGTH - strlen(decompressedLUT_filename) - 1);

        // Append file extension
        strncat(original_filename, ".txt", MAX_PATH_LENGTH - strlen(original_filename) - 1);
        strncat(compressed_filename, ".huf", MAX_PATH_LENGTH - strlen(compressed_filename) - 1);
        strncat(decompressed_filename, ".txt", MAX_PATH_LENGTH - strlen(decompressed_filename) - 1);
        strncat(decompressedLUT_filename, "LUT.txt", MAX_PATH_LENGTH - strlen(decompressed_filename) - 1);
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
                                                                
    printSymbolEncodingBitified(asciiToHuffman);                       // Print the huffman encoding of every symbol in the alphabet 
                                
    

    generateEncodedFileBitified(original_filename,     // Perform huffman encoding on the sample text and, 
                                compressed_filename,   // store the result in a .huf file
                                asciiToHuffman);  

    /******************************** Decoding ********************************/
    treeDecodingBitByBit(compressed_filename, decompressed_filename, huffmanRoot);  // Decode the compressed file in a bit-by-bit manner  

    uint_fast32_t code_max_bits = 0;
    code_max_bits = maxTreeDepth(huffmanRoot);

    lutCreation(huffmanRoot, all_luts, &tables_needed, code_max_bits);

    lutDecoding(compressed_filename, decompressedLUT_filename, all_luts, code_max_bits);

    lutFreeAll(all_luts, &tables_needed);
    freeHuffmanTree(huffmanRoot);

    /******************************** Prints Statements ********************************/
    // printf("Sample Size: %d bits\n", sample_size*8);
    // printf("Symbol Count: %d\n", symbol_count);
    // printf("Compressed Size: %d bits\n", compressed_size);

    return 0;
}