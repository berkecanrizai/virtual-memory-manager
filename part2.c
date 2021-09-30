/**
 * virtmem.c
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 1024
#define PAGE_MASK 0b1111111111


#define FRAME 256

#define PAGE_SIZE 1024
#define OFFSET_BITS 10
#define OFFSET_MASK 0b1111111111

#define MEMORY_SIZE PAGES * PAGE_SIZE

 // Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

struct tlbentry {
    int logical;
    int physical;
};

// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];
// number of inserts into TLB that have been completed. Use as tlbindex % TLB_SIZE for the index of the next TLB line to use.
int tlbindex = 0;

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
int pagetable[PAGES];

signed char main_memory[MEMORY_SIZE];

// Pointer to memory mapped backing file
signed char* backing;

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(int logical_page) {
    /* TODO */
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].logical == logical_page) {
            return tlb[i].physical;
        }
    }
    return -1;
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping (FIFO replacement). */
void add_to_tlb(int logical, int physical) {
    /* TODO */
    tlb[tlbindex % TLB_SIZE].logical = logical;
    tlb[tlbindex % TLB_SIZE].physical = physical;
    tlbindex++;
}

int p, least, value, time;
int counter[PAGES];


void initValues(const char* argv[], int i) {
    if (strcmp(argv[i], "-p") == 0) {
        if (argv[i + 1] != NULL) {
            p = atoi(argv[i + 1]);
        } //initialize optional arguement
    }
}

int main(int argc, const char* argv[])
{

    if (argc < 3) {
        fprintf(stderr, "Usage ./virtmem backingstore input\n");
        exit(1);
    }

    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            initValues(argv, i);
        }
    }

    const char* backing_filename = argv[1];
    int backing_fd = open(backing_filename, O_RDONLY);
    backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0);

    const char* input_filename = argv[2];
    FILE* input_fp = fopen(input_filename, "r");

    // Fill page table entries with -1 for initially empty table.
    int i;
    for (i = 0; i < PAGES; i++) {
        pagetable[i] = -1;
        counter[i] = 0; // initialize counter array to use with LRU algorithm
    }

    // Character buffer for reading lines of input file.
    char buffer[BUFFER_SIZE];

    // Data we need to keep track of to compute stats at end.
    int total_addresses = 0;
    int tlb_hits = 0;
    int page_faults = 0;

    // Number of the next unallocated physical page in main memory
    int free_page = 0;

    while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
        total_addresses++;
        int logical_address = atoi(buffer);

        /* TODO */
        // Calculate the page offset and logical page number from logical_address */
        int offset = logical_address & OFFSET_MASK;
        int logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;


        time++;
        if (p == 1) { //if using lru 
            counter[logical_page] = time;
        }

        int physical_page = search_tlb(logical_page);
        // TLB hit
        if (physical_page != -1) {
            tlb_hits++;
            // TLB miss
        }
        else {
            physical_page = pagetable[logical_page];
            // Page fault
            if (physical_page == -1) {
                /* TODO */
                page_faults++;

                if (p) {//LRU
                    if (page_faults <= FRAME) {
                        physical_page = page_faults - 1;
                    }
                    else {
                        value = 0;

                        for (int i = 0; i<PAGES; i++) {
                            if (pagetable[i] != -1) { //if that field is not empty
                                if (value == 0 || value > counter[i]) {
                                    least = i;
                                    value = counter[i];
                                }
                            }
                        }

                        physical_page = pagetable[least];
                    }
                }
                else {//FIFO algorithm
                    physical_page = free_page;
                    free_page++;
                    free_page = free_page % FRAME; // get them one by one from start
                }
                // open the file
                FILE* ptr = fopen(backing_filename, "rb");

                fseek(ptr, PAGE_SIZE * logical_page, SEEK_SET);

                fread(main_memory + physical_page * PAGE_SIZE, sizeof(signed char), PAGE_SIZE, ptr);

                for (int i = 0; i < PAGES; i++) {
                    if (pagetable[i] == physical_page) { //change the entry of physical_page to -1 so we delete it
                        pagetable[i] = -1;
                    }
                }
                pagetable[logical_page] = physical_page;
                fclose(ptr);
            }
            add_to_tlb(logical_page, physical_page); //add page to tlb table
        }

        int physical_address = (physical_page << OFFSET_BITS) | offset;
        signed char value = main_memory[physical_page * PAGE_SIZE + offset];

        printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }

    printf("Number of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
    printf("TLB Hits = %d\n", tlb_hits);
    printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));

    return 0;
}