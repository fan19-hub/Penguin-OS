
#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#ifndef ASM 
   extern void pit_wrap();
   extern void keyboard_wrap();   // keyboard_wrap function will be called when keyboard is pressed
   extern void rtc_wrap();   // rtc_wrap function will be called when rtc is pressed
#endif

#endif
