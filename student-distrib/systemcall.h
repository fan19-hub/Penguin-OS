#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H
#include "lib.h"
#include "terminal.h"
#include "filesys.h"
#include "rtc.h"
#define PCB_SIZE 8                  //the size of pcb
#define PROGIMG_ADDR 0X08048000     //program img address 
#define PROGRAM_START_ADDR 0x800000 //the start of physical address for user level file
#define STACK_8KB 0x2000            // stack size in kernel page
#define PAGE_4MB 0x400000           // page size (4MB)
#define KERNEL_BASE 0x800000        //base address of kernel page
#define VIR_ADDR 0x08000000         //start of virtual address 
#define ONE_SAPCE 4                 //4 bytes one address
#define FD_BUSY 1                   //fd is busy
#define FIRST_FILE_FD 2             //the start FD of operations other than stdin/stdout
#define OFFSET 2   
#define USER_ID 32                  //the index of page directory for user
#define EIGHT_KB 0x2000             // stack size in kernel page
#define MAX_ARG_LEN 100             //the max length of argument. 100 is just a big number.
//The structures are defined here
//operation table declaration
typedef struct {
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
}file_opt_table_t;          //operation table declaration

//file block declaration
typedef struct{
    file_opt_table_t* table_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flg;           //1=busy
    char name[32];
}file_array_t;              //file block declaration

typedef struct{
    int signum;
}sigqueue_t;

//pcb declaration
typedef struct{
    uint32_t ebp;
    uint32_t esp;
    uint32_t esp_ret;
    uint32_t ebp_ret;
    uint32_t eip_user;
    uint32_t esp_user;
    uint32_t parent_id;
    uint32_t id;
    void (*func) (int signum);
    sigqueue_t pending_signals[20];
    // int (*set_signal)(int pid, int signal);
    uint32_t block; 
    char arg[100];  //store file argument
    file_array_t file_array[PCB_SIZE];
}pcb_t;                     //pcb declaration

extern int32_t pcb_array[3];      //record the 3 current pcb
extern int terminal_now;
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);
extern void send_sig_info(int signal);
extern int check_pengding(void);
void kill(int signum);
//operations tables for stdin, stdout, RTC, directory and regular file
file_opt_table_t stdin_opt_table; 
file_opt_table_t stdout_opt_table; 
file_opt_table_t RTC_opt_table;
file_opt_table_t DIR_opt_table;
file_opt_table_t REG_FILE_opt_table;

//system call functions
//read operation
int32_t read (int32_t fd, void* buf, int32_t nbytes); 
//write operation
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
//open operation
int32_t open (const uint8_t* filename);
//close operation
int32_t close (int32_t fd);
//halt operation
int32_t halt (uint8_t status);
//execute operation
int32_t execute(const uint8_t* command);
//getargs operation 
int32_t getargs (uint8_t* buf, int32_t nbytes);
//vidmap operation
int32_t vidmap (uint8_t** screen_start);
//undefined operations
int32_t undefined();
int32_t remove(uint8_t* filename);

//assembly functions
#ifndef ASM_SYS
//syscall handler
extern void syscall_handler();
//update tlb
extern void flush_tlb();
//store esp
extern uint32_t store_esp();
//store ebp
extern uint32_t store_ebp();
#endif

//pcb related functions
//get current pcb address
pcb_t* get_pcb_addr(uint32_t id);
//get current pcb address, which will call the above function
pcb_t* get_cur_pcb_addr();
//pcb initialization
void pcb_initialize(pcb_t *pcb);
#endif


