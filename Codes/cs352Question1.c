/**
 * CS352 - PROJECT QUESTION NUMBER 1
 * Eren Ali Aslangiray
 * Mehmet Enis Isgören
 * Mahmut Sami Özmen
 */


//PLEASE RUN THE CODE IN TERMINAL WITH THIS CALL: ./cs352 addresses.txt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>

//This is the part where we define the simulation properities:
#define FRAME_SIZE 256        //Frame size
#define TOTAL_NUMBER_OF_FRAMES 256  //Total number of frames
#define ADDRESS_MASK  0xFFFF  //Hide other bits to see only the address
#define OFFSET_MASK  0xFF //Hide other bits to see only the offset
#define TLB_SIZE 16       //Size of TLB
#define PAGE_TABLE_SIZE 256  //Page table size
#define CHUNK               256 // Number of bytes to read
#define BUFFER_SIZE         10 // Number of char's to read in each row



//This is the part where we setup our arrays:
int pageTableNumbers[PAGE_TABLE_SIZE];  //Array to hold the page numbers in the page table
int pageTableFrames[PAGE_TABLE_SIZE];   //Array to hold the frame numbers in the page table
int TLBPageNumber[TLB_SIZE];  //Array to hold the page numbers in the TLB
int TLBFrameNumber[TLB_SIZE]; //Array to hold the frame numbers in the TLB
int physicalMemory[TOTAL_NUMBER_OF_FRAMES][FRAME_SIZE]; //Physical memory 2D array (Table)

//Our counter integers for all the calculations needed:
int pageFaults = 0;   // Counter for page faults
int TLBHits = 0;      // Counter for TLB hits
int firstAvailableFrame = 0;  // Counter for the first available frame
int firstAvailablePageTableNumber = 0;  // Counter for the first available page table entry
int numberOfTLBEntries = 0;             // Counter for the number of entries in the TLB

// Input file and backing store (Note you have to run the code in terminal as "./cs352 addresses.txt" you can change input file while calling the program at terminal.
FILE    *address_file;
FILE    *backing_store;

// Defining the readed input file values to char and int.
char    address[BUFFER_SIZE];
int     logical_address;

// Defining the readed backing store file values to signed char.
signed char     buffer[CHUNK];
// The value of the byte (signed char) in memory
signed char     value;

// Names for the functions used below
void getPage(int address);
void readFromStore(int pageNumber);
void insertIntoTLB(int pageNumber, int frameNumber);

// Main opens necessary files and calls on getPage for every entry in the addresses file
int main(int argc, char *argv[])
{
    // Perform basic error checking
    if (argc != 2) {
        fprintf(stderr,"Usage: ./a.out [input file]\n");
        return -1;
    }
    
    // Open the backing store
    backing_store = fopen("BACKING_STORE.bin", "rb");
    // Open the input file that contains the logical addresses
    address_file = fopen(argv[1], "r");
    
    int numberOfTranslatedAddresses = 0;
    // Read through the input file and output each logical address
    while ( fgets(address, BUFFER_SIZE, address_file) != NULL) {
        logical_address = atoi(address);
        
        // Get the physical address and value stored at that address
        getPage(logical_address);
        numberOfTranslatedAddresses++;  // Increment the number of translated addresses
    }
    
    // Calculate and print out the stats
    double pfRate = pageFaults / (double)numberOfTranslatedAddresses;
    double TLBRate = TLBHits / (double)numberOfTranslatedAddresses;
    printf("Page Fault Rate = %.3f\n",pfRate);
    printf("TLB Hit Rate = %.3f\n", TLBRate);
    
    // Close the input file and backing store and end the operation.
    fclose(address_file);
    fclose(backing_store);
    
    return 0;
}

// This function is to take the logical address and obtain the physical address and value stored at that address.
void getPage(int logical_address){
    
    // This obtains the page number and offset from the logical address
    int pageNumber = ((logical_address & ADDRESS_MASK)>>8);
    int offset = (logical_address & OFFSET_MASK);
    
    // First try to get page from TLB
    int frameNumber = -1; //Initialized to -1 to tell if it's valid.
    
    int i;  //Look through TLB for a match
    for(i = 0; i < TLB_SIZE; i++){
        if(TLBPageNumber[i] == pageNumber){   //If the TLB index is equal to the page number
            frameNumber = TLBFrameNumber[i];  //Then the frame number is extracted
                TLBHits++;                // And the TLBHit counter is incremented
        }
    }
    
    //If the frameNumber was not found
    if(frameNumber == -1){
        int i;   //Look the contents of the page table
        for(i = 0; i < firstAvailablePageTableNumber; i++){
            if(pageTableNumbers[i] == pageNumber){         // If the page is found in those contents
                frameNumber = pageTableFrames[i];          // Extract the frameNumber from its copy array
            }
        }
        if(frameNumber == -1){                     // If the page is not found in those contents
            readFromStore(pageNumber);             // Page fault, call to readFromStore to get the frame into physical memory and the page table
            pageFaults++;                          // Increment the number of page faults
            frameNumber = firstAvailableFrame - 1;  // And set the frameNumber to the current firstAvailableFrame index
        }
    }
    
    insertIntoTLB(pageNumber, frameNumber);  // Call to function to insert the page number and frame number into the TLB
    value = physicalMemory[frameNumber][offset];  // Frame number and offset used to get the signed value stored at that address
   printf("frame number: %d\n", frameNumber);
printf("offset: %d\n", offset); 
    // Output print of the virtual address, physical address and value of the signed char to the console.
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, (frameNumber << 8) | offset, value);
}

// Function to insert a page number and frame number into the TLB with a First in Firs Out (FIFO) replacement
void insertIntoTLB(int pageNumber, int frameNumber){
    
    int i;  // If it's already in the TLB, break the func.
    for(i = 0; i < numberOfTLBEntries; i++){
        if(TLBPageNumber[i] == pageNumber){
            break;
        }
    }
    
    // If the number of entries is equal to the index
    if(i == numberOfTLBEntries){
        if(numberOfTLBEntries < TLB_SIZE){  // And the TLB still has room in it
            TLBPageNumber[numberOfTLBEntries] = pageNumber;    // Insert the page and frame
            TLBFrameNumber[numberOfTLBEntries] = frameNumber;
        }
        else{                                            // Otherwise move everything over
            for(i = 0; i < TLB_SIZE - 1; i++){
                TLBPageNumber[i] = TLBPageNumber[i + 1];
                TLBFrameNumber[i] = TLBFrameNumber[i + 1];
            }
            TLBPageNumber[numberOfTLBEntries-1] = pageNumber;  // And insert the page and frame on the end
            TLBFrameNumber[numberOfTLBEntries-1] = frameNumber;
        }        
    }
    // If the index is not equal to the number of entries
    else{
        for(i = i; i < numberOfTLBEntries - 1; i++){      // Iterate through up to one less than the number of entries
            TLBPageNumber[i] = TLBPageNumber[i + 1];      // Move everything over in the arrays
            TLBFrameNumber[i] = TLBFrameNumber[i + 1];
        }
        if(numberOfTLBEntries < TLB_SIZE){                // If there is still room in the array, put the page and frame on the end
            TLBPageNumber[numberOfTLBEntries] = pageNumber;
            TLBFrameNumber[numberOfTLBEntries] = frameNumber;
        }
        else{                                             // Otherwise put the page and frame on the number of entries - 1
            TLBPageNumber[numberOfTLBEntries-1] = pageNumber;
            TLBFrameNumber[numberOfTLBEntries-1] = frameNumber;
        }
    }
    if(numberOfTLBEntries < TLB_SIZE){                    // If there is still room in the arrays, increment the number of entries
        numberOfTLBEntries++;
    }    
}
// Function to read from the backing store and bring the frame into physical memory and the page table
void readFromStore(int pageNumber){
    // First seek to byte CHUNK in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    if (fseek(backing_store, pageNumber * CHUNK, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
    }
    
    // Now read CHUNK bytes from the backing store to the buffer
    if (fread(buffer, sizeof(signed char), CHUNK, backing_store) == 0) {
        fprintf(stderr, "Error reading from backing store\n");        
    }
    
    // Load the bits into the first available frame in the physical memory 2D array
    int i;
    for(i = 0; i < CHUNK; i++){
        physicalMemory[firstAvailableFrame][i] = buffer[i];
    }
    
    // And then load the frame number into the page table in the first available frame
    pageTableNumbers[firstAvailablePageTableNumber] = pageNumber;
    pageTableFrames[firstAvailablePageTableNumber] = firstAvailableFrame;

    // Increment the counters that track the next available frames
    firstAvailableFrame++;
    firstAvailablePageTableNumber++;
}
