#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"
#include "systemcall.h"
#define KB_PORT 0x60   //the port of ps/2, according to https://wiki.osdev.org/%228042%22_PS/2_Controller
#define KB_IRQ 1  //irq1
#define FREE_CHECKER 0x80
//#define size_sc 58 //size of scancodes
#define KB_BUF_SIZE    128

extern volatile unsigned int kb_buf_idx;

unsigned char keyboard_buffer[KB_BUF_SIZE];
#define CKY_PUSH 0x2E 
#define INTERRUPT_SIG 2
//keyboard initialization
void keyboard_init(void);
//show typed character on the screen
void keyboard_handler(void);
void free_all(void);
#endif


