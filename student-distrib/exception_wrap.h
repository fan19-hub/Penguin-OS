#ifndef _EXCEPTION_WRAP_H
#define _EXCEPTION_WRAP_H


#ifndef ASM
// read the volume3 referenc();stand what each exception code do
 extern void divide_zero();  //this function will be called when corresponding error happened
 extern void step_debug();  //this function will be called when corresponding error happened
 extern void nmi();  //this function will be called when corresponding error happened
 extern void breakpoint();  //this function will be called when corresponding error happened
 extern void overflow();  //this function will be called when corresponding error happened
 extern void out_bound(); //this function will be called when corresponding error happened
 extern void opcode_fault(); //this function will be called when corresponding error happened
 extern void coprocessor_fault(); //this function will be called when corresponding error happened
 extern void double_fault(); //this function will be called when corresponding error happened
 extern void segment_overrun(); //this function will be called when corresponding error happened
 extern void invalid_tss(); //this function will be called when corresponding error happened
 extern void segment_not_present(); //this function will be called when corresponding error happened
 extern void stack_segment_fault(); //this function will be called when corresponding error happened
 extern void protection_fault(); //this function will be called when corresponding error happened
 extern void page_fault(); //this function will be called when corresponding error happened

 extern void floating_point(); //this function will be called when corresponding error happened
 extern void alignment(); //this function will be called when corresponding error happened
 extern void machine_fault(); //this function will be called when corresponding error happened
 extern void SIMD_fault(); //this function will be called when corresponding error happened
 #endif  //ASM

 #endif 
