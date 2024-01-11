/* Reference: https://wiki.osdev.org/Setting_Up_Paging */

/* checkpoint 1 */

/* loadPageDirectory
 *
 * Put the address of PD to CR3
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
.global loadPageDirectory
loadPageDirectory:
pushl %ebp
movl %esp, %ebp
movl 8(%esp), %eax
movl %eax, %cr3
movl %ebp, %esp
popl %ebp
ret

/* enablePaging
 *
 * Set the corresponding bit in CR0 to enable paging operation for 4kb page
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
.global enablePaging
enablePaging:
pushl %ebp
movl %esp, %ebp
movl %cr0, %eax
orl $0x80000000, %eax
movl %eax, %cr0
movl %ebp, %esp
popl %ebp
ret

/* enablePaging_4mb:
 *
 * Set the corresponding bit in CR3 to enable paging operation for 4mb page
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
# .text
.global enablePaging_4mb
enablePaging_4mb:
pushl %ebp
movl %esp, %ebp
movl %cr4, %eax
orl $0x00000010, %eax
movl %eax, %cr4
movl %ebp, %esp
popl %ebp
ret

/* checkpoint 3 */
