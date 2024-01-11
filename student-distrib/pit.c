#include "pit.h"
#include "systemcall.h"
#include "x86_desc.h"
#include "terminal.h"
void pit_init(){
    uint16_t init_freq=82;  //set the initial frequency to be 82 HZ, the interval is about 12 miliseconds(10<12<50)
                            //82 is the factor of the base frequency 1193182. Hence we can minimize the frequency error.
    uint16_t ticks_between_interrupt=PIT_BASE_FREQ/init_freq; //The ticks between interrupts
    uint8_t lobyte=ticks_between_interrupt&0xF;    //The low 8 bits of ticks_between_interrupt 
    uint8_t hibyte=ticks_between_interrupt>>8;     //The high 8 bits of ticks_between_interrupt 
    
    //set up the mode, writing to mode/command port
    outb(PIT_MODE_VECTOR,PIT_COMMAND_PORT);  

    //write the ticks between two adjacent interrupts      
    outb(lobyte,PIT_DATA_PORT);    //write the lobyte first
    outb(hibyte,PIT_DATA_PORT);    //write the hibyte

    //enable the IRQ0 on PIC
    enable_irq(IRQ0);       
}



void pit_handler(){
    send_eoi(IRQ0);                 //0 is the irq of pit
    cli();
    pcb_t* cur_pcb;
    pcb_t* next_pcb;
    cur_pcb=get_pcb_addr(pcb_array[terminal_now]);


    //update running terminal
    terminal_now=(terminal_now+1)%3;      //3: we have 3 terminals in total     
    while(pcb_array[terminal_now]==-1){   //if next pcb doesn't exist, we skip it and check the one after it.
        terminal_now=(terminal_now+1)%3;
    }

    //update next_pcb
    next_pcb=get_pcb_addr(pcb_array[terminal_now]);
    if(next_pcb->id==cur_pcb->id){
        return;
    }
    uint32_t phy_addr=PROGRAM_START_ADDR+next_pcb->id*PAGE_4MB;//physical address of user file
    
    //map the user program to its corresponding physical address 
    page_directory[VIR_ADDR/PAGE_4MB]=(((phy_addr/PAGE_4MB)<<22)| PRESENT | PS |GLOBAL|  R_W |U_S);//directory point to updated physical address
    flush_tlb();
    if(terminal_now==(cur_display_terminal_id-1)){
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=( VIDEO_MEM_BASE| PRESENT | R_W |U_S);
        page_table_vidpage[VIDEO_MEM_BASE/PG_SIZE_4KB]=( VIDEO_MEM_BASE| PRESENT | R_W |U_S);
    }
    else{
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT| R_W |U_S;//directory point to updated physical address
        page_table_vidpage[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT| R_W |U_S;//directory point to updated physical address
    }
    flush_tlb();//update 

    //update TSS
    //TSS USER STACK->KERNEL STACK
    uint32_t esp_kernel=PROGRAM_START_ADDR-ONE_SAPCE-STACK_8KB*(next_pcb->id);   //kernel stack of user space file
    tss.esp0=esp_kernel;
    tss.ss0=KERNEL_DS;
    sti();
   
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=g" (cur_pcb->esp), "=g" (cur_pcb->ebp)
    );

    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        : /*No output*/
        : "g" (next_pcb->esp), "g" (next_pcb->ebp)
    );
}











