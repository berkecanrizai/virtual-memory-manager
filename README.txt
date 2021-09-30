MakeFile Commands:
make all   : compile part1.c and part2.c
make first : runs part1.out
make FIFO  : runs part2.out with -p 0
make LRU   : runs part2.ount with -p 1
make clean : remove both part1.out and part2.out


In the first part, we have two masks, PAGE_MASK and OFFSET_MASK which are both 0b1111111111. We are given the virtual page 
number that consists of virtual adress and page offset. We first get the offset,as int by taking & of logical_adress that 
is provided with the OFFSET_MASK which returns the 00..0x...x that is our offset.
Than we shift the digits of this logcal_adress by OFFSET_BITS which is 10 for this project so now we have 0...0x...x that 
represents our logical page number, we then take logical_page as logical_page & PAGE_MASK this is how these adresses are 
initialized.
We do these calculations for each of the numbers given in a while loop that copies those to buffer, those elements are con-
verted to numerics in atoi(buffer).
With these iterations we try to get physical_page with search_tlb function, this checks if given adress is already in tlb table,
if it is then that is TLB hit and we move on. If it is not, it returns -1 and we go to the pagetable where we again check if,
given adress is in there we move on however, if it is -1 than that is page fault and we need to retrieve it from the disk. 
In the case of page fault, we increment count of page_faults by 1. Initialize physical_page to free_page and increment free_page 
by 1 again. We then open the file with fopen with parameters, backing_file and read bytes. We use that pointer to get the data 
with appropiate size by passing it to fseek and fread and we get the exact part we need by calculating the offset with physical_page*
PAGE_SIZE, upon retrieving that data we add it to the pagetable[logical page]. Than file is closed with fclose(pointer). That adress
is added to TLB with add_to_tlb. 
In add_to_tlb, tlbindex counts the current entry we are in, with %TLB_SIZE we make sure we are in the array and if we don't have any
space left, it points back to the first entry of array. Each tlbentry hold both logical and physical adresses in unsigned char, they 
are initialized here. Than these are printed in the order of virtual, physical adresses and value inside.

In part2, We changed the FRAME to 256 as requested. We have a counter array that has length of page number. In this part we changed 
data of tlbentry (logical and physical) to integers. Again we have same structure for hit and page faults as the first part of the 
assignment. Again, offset and logical_pages are extracted from VPN. If there is miss and page fault, we initialize int value to 0 and 
for length of counter which is PAGES, we look at if ith space of pagetable is empty if so, and if value is zero or bigger than the int
stored in ith field of counter we change the least found index to i and value to ith element of counter. At each iteration neverthless 
of if it is hit or not, we change the value of current_logical page to current iteration so we keep track of latest and least used 
elements. Than physical_page is ith value of counter after for loop ends. This was LRU implementation, for the fifo we initialize 
physical_page to free_page and increment free_page by one. We take modulo of free_page with FRAME so that we don't exceed the maximum 
length of the array that is number FRAME. In an array, we find current physical_page in pagetable and delete it from there by changing 
it to -1. We than add next adress to the pagetable with the line, pagetable[logical_page] = physical_page than add to the tlb before 
closing the file's pointer. Again, prints are done lastly.