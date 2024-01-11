/* paging.c - 
 */

#include "paging.h"
#include "lib.h"

/* checkpoint 1 */

/* paging_init
 *
 * Initializing paging
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: paging
 */
void paging_init(void){
    int i;
    for ( i = 0; i < PD_SIZE; i++)
    {
        /* initialize the page directory and page table video */
        page_directory[i] = 0|R_W;
        page_table_vid[i] = (i * PG_SIZE_4KB) | R_W;
        page_table_vidpage[i] = (i * PG_SIZE_4KB) | R_W;
    }
    
    /* mark the certain entry in the page table as "present" */
    /* certain pages: video memory page and its three terminal */
    page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB] = VIDEO_MEM_BASE| PRESENT ;
    
    paging_terimal_init();

    /* mark the certain entry in the page directory as "present" */
    /* certain pages: the entry points to the page table of video memory */
    /* and connect the first entry of page_directory to page_table_vid */
    /* page_directory[0]----------->page_table_vid------------>video memory */
    page_directory[0] =  (uint32_t)page_table_vid | PRESENT | R_W ;
    /* set the corresponding bits for the global kernel page */
    /* page_directory[1]----------->kernel page */
    page_directory[1] =  (KERNEL_PAGE | PRESENT | PS |  GLOBAL | R_W) & SUPERVISOR;
    paging_assemble_interface();

}
/* checkpoint 3 */
/* checkpoint 4 */
/* checkpoint 5 */
/* paging_terimal_init
 *
 * Initializing paging
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: paging
 */
void paging_terimal_init(){
    page_table_vid[VIDEO_MEM_T1  /PG_SIZE_4KB] = VIDEO_MEM_T1| PRESENT ;
    page_table_vid[VIDEO_MEM_T2  /PG_SIZE_4KB] = VIDEO_MEM_T2| PRESENT ;
    page_table_vid[VIDEO_MEM_T3  /PG_SIZE_4KB] = VIDEO_MEM_T3| PRESENT ;
    return;
}
/* paging_assemble_interface
 *
 * Initializing paging
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: paging
 */
void paging_assemble_interface(){
    loadPageDirectory(page_directory);
    enablePaging_4mb();
    enablePaging();
    return;
}

