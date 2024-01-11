#ifndef IDT_H
#define IDT_H
#include "systemcall.h"
#include "lib.h"
#ifndef ASM
#define SYSCALL (0X80)                //the base of system call is 0x80
#define NUM_EXCEPT (20)                //The number of exception that exists  
#define KEYBOARD_VECTOR (0x21)        //The interrupt id for keyboard is 0x21
#define RTC_VECTOR (0x28)         //The interrupt id forrtc is 0x21
#define PIT_VECTOR  (0x20)
#define INVALID_TSS 0x0a   //acording to volume3 , the exception code .
#define SEGMENT_NOT_PRESENT  0x0b
#define STACK_SEGMENT_FAULT  0x0c
#define PROTECTION_FAULT  0x0d
#define PAGE_FAULT 0x0e
// typedef struct x86_regs  x86_regs ;
struct x86_register{
   
    uint32_t esi;  
    uint32_t edi;   //The following registers are what will be on the stack when calling pushal 
    uint32_t ebp;
    uint32_t esp;
    uint32_t edx;
    uint32_t ebx; 
    uint32_t ecx;
    uint32_t eax;
};
#define DIV_ZERO 5
#define SEGFAULT 1
void idt_init(void);
extern void exception_handler(uint32_t id,uint32_t flag,struct x86_register regs,uint32_t error_code);


#endif /*ASM*/
#endif /*IDT_H*/



