#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "exception_wrap.h"
#include "keyboard.h"
#include "rtc.h"
#include "interrupt_handler.h"
#include "systemcall.h"





static char* exceptions_name[]={  //according to volume 3, write all the interrupt name
 "divide_zero",
 "step_debug",
 "nmi",
 "breakpoint",
 "overflow",
 "out_bound",
 "opcode_fault",
 "coprocessor_fault",
 "double_fault",
 "segment_overrun",
 "invalid_tss",
 "segment_not_present",
 "stack_segment_fault",
 "protection_fault",
 "page_fault",
 "reserved",
 "floating point",
 "alignment",
 "machine_fault",
 "SIMD_fault"
};
//  static char* syscall_name[]={ 
//    "halt" ,
//    "excute" ,
//    "read" ,
//    " write",
//    "open" ,
//    "close", 
//    "getargs", 
//    "vidmap" ,
//    "set_handler", 
//    "sigreturn"};
/* idt_init
 *
 * initilize IDT table and set up each entry
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void idt_init(void){
    unsigned int u;
    for(u=0;u<NUM_VEC;u++){       
        
        if((u<NUM_EXCEPT)&&(u!=0xF)){  //exception 0x15 is not present
            idt[u].present=0x1;
            idt[u].reserved3=0x0;
            idt[u].dpl=0x0;  
        }
        else{
            idt[u].present=0x0;
        }
        idt[u].seg_selector=KERNEL_CS;
        idt[u].dpl=0x0;       //set priviledge to 3 when it is system calls to switch to user-code level
        idt[u].reserved4=0x0;
        //set to 1 when it is  interrupts 
        idt[u].reserved2=0x1;
        idt[u].reserved1=0x1;
        idt[u].reserved0=0x0;
        idt[u].size=0x1;    // because it is 32bit always
    
    
    }      

  SET_IDT_ENTRY(idt[0],divide_zero);  //set present entry to have offset 
  SET_IDT_ENTRY(idt[1],step_debug );
  SET_IDT_ENTRY(idt[2],nmi );
  SET_IDT_ENTRY(idt[3],breakpoint );
  SET_IDT_ENTRY(idt[4],overflow );
  SET_IDT_ENTRY(idt[5],out_bound );
  SET_IDT_ENTRY(idt[6],opcode_fault );
  SET_IDT_ENTRY(idt[7],coprocessor_fault );
  SET_IDT_ENTRY(idt[8],double_fault );
  SET_IDT_ENTRY(idt[9],segment_overrun );
  SET_IDT_ENTRY(idt[10],invalid_tss );
  SET_IDT_ENTRY(idt[11],segment_not_present );
  SET_IDT_ENTRY(idt[12],stack_segment_fault );
  SET_IDT_ENTRY(idt[13],protection_fault );
  SET_IDT_ENTRY(idt[14],page_fault );
  SET_IDT_ENTRY(idt[16],floating_point );
  SET_IDT_ENTRY(idt[17],alignment );
  SET_IDT_ENTRY(idt[18],machine_fault );
  SET_IDT_ENTRY(idt[19],SIMD_fault );
  
  SET_IDT_ENTRY(idt[RTC_VECTOR],rtc_wrap);   //set present entry to have offset 
  SET_IDT_ENTRY(idt[SYSCALL], syscall_handler);
  SET_IDT_ENTRY(idt[KEYBOARD_VECTOR],keyboard_wrap);
  SET_IDT_ENTRY(idt[PIT_VECTOR],pit_wrap);
  SET_IDT_ENTRY(idt[KEYBOARD_VECTOR],keyboard_wrap);
  
  
  idt[KEYBOARD_VECTOR].present=0x1;  // make this idt entry present
  idt[KEYBOARD_VECTOR].reserved3=0x1;  //mark it is an interrupt
  idt[KEYBOARD_VECTOR].dpl=0x1;
  
  idt[PIT_VECTOR].present=0x1;  // make this idt entry present
  idt[PIT_VECTOR].reserved3=0x1;   //mark it is an interrupt
  idt[PIT_VECTOR].dpl=0x0; 
  
  
  idt[RTC_VECTOR].present=0x1;  // make this idt entry present
  idt[RTC_VECTOR].reserved3=0x1;   //mark it is an interrupt
  idt[RTC_VECTOR].dpl=0x0; 
  
  idt[SYSCALL].present=0x1; // make this idt entry present
  idt[SYSCALL].dpl=0x3;      // change privilege level to 3
  idt[SYSCALL].reserved3=0x0;   
  
  lidt(idt_desc_ptr);       //initiize the IDT register
    
}
//The following registers are what will be on the stack when calling pushal 
/*exception_handler
 *
 * use this  to make all exception functions
 * Inputs: uint32_t id,uint32_t flag,struct x86_register regs,uint32_t error_code
 * Outputs: None
 * Side Effects: print message of exceptions
 */

void exception_handler(uint32_t id,uint32_t flag,struct x86_register regs,uint32_t error_code){
    
    
        cli(); 
        //   clear();
        if(id<NUM_EXCEPT){
            printf(" The exception  :%s ",exceptions_name[id]," happened\n"); //print all the registers
        }
        else{
            printf("\n  A system call happens\n ");
        }
        printf(" EAX: 0x%#x, EBX: 0x%#x \n ",regs.eax,regs.ebx); 
        printf(" ECX: 0x%#x, EDX: 0x%#x \n",regs.ecx,regs.edx);
        printf(" ESI: 0x%#x, EDI: 0x%#x \n ",regs.esi,regs.edi);
        printf(" EBP: 0x%#x, ESP: 0x%#x \n ",regs.ebp,regs.esp);
        printf(" The EFLAGS is : %#x \n",flag); //print flags

        if(id==INVALID_TSS||id==SEGMENT_NOT_PRESENT||id==STACK_SEGMENT_FAULT||id==PROTECTION_FAULT){
           (error_code&0x1)? printf(" external\n"):printf(" internal\n");
           (error_code&0x2)?(printf(" IDT\n")):((error_code&0x4)?printf(" LDT\n"):printf(" GDT\n"));
           printf(" selector index: 0x%#x \n", error_code>>3);
        }
        else if(id==PAGE_FAULT){  //when it is fault_page, analyze the error_code 
            (error_code&0x1)?printf("\n This exception was due to a not-present page \n") :printf("\n  This exception was due to an access rights violation \n or the use of a reserved bit \n");
            (error_code&0x2)?printf("\n It is write caused the exception\n") :printf("\n It is read caused the exception\n");
            (error_code&0x04)? printf("\n The processor was executing at user mode \n ") :  printf("\n The processor was executing at supervisor mode\n ");
            (error_code&0x08)?printf ("\n The fault was caused by reserved bits set to 1 in a page directory\n") : printf("\n The fault was not caused by reserved bit violation \n");
        }
        // if(id==DIV_ZERO){
        //     printf("divide_zero happen,should send signal");
        //     send_sig_info(DIV_ZERO);
        //     printf("divide_zero finish");
        // }else{
        //     send_sig_info(SEGFAULT);
        // }
       sti();
        halt(255); //  stop the process 
      
}

