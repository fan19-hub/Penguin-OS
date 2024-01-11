                            

#define ASM     1

#include "exception_wrap.h"       


 #define EXCEPTION_ARG(name, id)    \
.globl  name                        ;\
.align  4                           ;\
name:                               ;\
    pushfl                          ;\
    pushal                          ;\
    pushl   $id                     ;\
    call    exception_handler       ;\
    addl    $4, %esp                ;\
    popal                           ;\
    popfl                           ;\
    addl    $4, %esp                ;\
    iret 

#define EXCEPTION(name, id)         \
.globl  name                        ;\
.align  4                           ;\
name:                               ;\
    pushl   $0                      ;\
    pushfl                          ;\
    pushal                          ;\
    pushl   $id                     ;\
    call    exception_handler       ;\
    addl    $4, %esp                ;\
    popal                           ;\
    popfl                           ;\
    addl    $4, %esp                ;\
    iret




  EXCEPTION(divide_zero ,0X00);
  EXCEPTION(step_debug ,0X01);
  EXCEPTION(nmi ,0X02);
  EXCEPTION(breakpoint ,0X03);
  EXCEPTION(overflow ,0X04);
  EXCEPTION(out_bound ,0X05);
  EXCEPTION(opcode_fault ,0X06);
  EXCEPTION(coprocessor_fault ,0X07);
  EXCEPTION_ARG(double_fault ,0X08);
  EXCEPTION(segment_overrun ,0X09);
  EXCEPTION_ARG(invalid_tss ,0X0A);
  EXCEPTION_ARG(segment_not_present ,0X0B);
  EXCEPTION_ARG(stack_segment_fault ,0X0C);
  EXCEPTION_ARG(protection_fault ,0X0D);
  EXCEPTION_ARG(page_fault ,0X0E);

  EXCEPTION(floating_point ,0X10);
  EXCEPTION_ARG(alignment ,0X11);
  EXCEPTION(machine_fault ,0X12);
  EXCEPTION(SIMD_fault ,0X13);
  # hh



