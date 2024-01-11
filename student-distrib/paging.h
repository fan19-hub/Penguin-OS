/* paging.h - 
 * 
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/* define some useful "Magic Number" */

/* physical memory reference */
#define VIDEO_MEM_BASE 0xB8000
#define VIDEO_MEM_T1   0xB9000
#define VIDEO_MEM_T2   0xBA000
#define VIDEO_MEM_T3   0xBB000
#define KERNEL_MEM     0x400000
#define KERNEL_PAGE    1024<<12        // same to PG_SIZE_4MB
/* cp4 */
#define KERNAL_SPACE_BASE   0x8000000
#define USER_SPACE_BASE     0x8400000
#define VIDEO_PAGE_VADDR    0x8800000

/* size */

#define PD_SIZE        1024
#define PT_SIZE        1024
#define PG_SIZE_4KB    0x1000
#define PG_SIZE_4MB    0x1000 * 1024

/* selection bit */
    /* and to set 0 */
#define RESERVE_0      0xffffffBf       // set bit 6 in PDE_4KB: reserved (set to 0)
#define SUPERVISOR     0xfffffffB       // set bit 2 to supervisor mode to make it continue to work after paging is turned on
    /* or to set 1 */
#define PAT_PDE_4MB    0x00001000       // set bit 12 in PDE_4MB: Page table attribute index         
#define GLOBAL         0x00000100       // set bit 8: Global page
#define PAT_PTE_4KB    0x00000080       // set bit 7 in PTE: Page table attribute index
#define PS             0x00000080       // set bit 7 in PDE (otherwise PTE): Page size
#define DIRTY          0x00000040       // set bit 6 (otherwise PDE_4KB): Dirty
#define ACCESS         0x00000020       // set bit 5: accessed
#define PCD            0x00000010       // set bit 4: cache disabled
#define PWT            0x00000008       // set bit 3: user/supervisor 
#define U_S            0x00000004       // set bit 2:
#define R_W            0x00000002       // set bit 1: R_W
#define PRESENT        0x00000001       // set bit 0: present
//#define AVAIL 

/* allocate memory for PD and PTs */
uint32_t page_directory[PD_SIZE] __attribute__((aligned(PG_SIZE_4KB)));
uint32_t page_table_vid[PT_SIZE] __attribute__((aligned(PG_SIZE_4KB)));
uint32_t page_table_vidpage[PT_SIZE] __attribute__((aligned(PG_SIZE_4KB)));

//Initializing paging
void paging_init(void);
//Initializing paging_terminal
void paging_terimal_init();
//Put the address of PD to CR3
extern void loadPageDirectory(unsigned int*);
//Set the corresponding bit in CR0 to enable paging operation for 4kb page
extern void enablePaging();
//Set the corresponding bit in CR3 to enable paging operation for 4mb page
extern void enablePaging_4mb();
void paging_assemble_interface();
#endif /* _PAGING_H */
