/* reference: https://wiki.osdev.org/Pit */ 

/* The ports of the PIT
I/O port     Usage
0x40         Channel 0 data port (read/write)
0x41         Channel 1 data port (read/write)
0x42         Channel 2 data port (read/write)
0x43         Mode/Command register (write only, a read is ignored)
*/

/* Set up the mode
Bits         Usage
6 and 7      Select channel :
                0 0 = Channel 0
                0 1 = Channel 1
                1 0 = Channel 2
                1 1 = Read-back command (8254 only)
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
#ifndef _PIT_H
#define _PIT_H
#include "lib.h"
#include "i8259.h"
#include "paging.h"
#define TERMINAL_NUMBER 3
#define PIT_MODE_VECTOR 0x36  // 00: channel0 accesses the PIC; 11: use lobyte/hibyte because we have 16 bit to write; 011: mode3; 0:binary mode 
#define PIT_COMMAND_PORT 0x43       // Only one command port 0x43
#define PIT_DATA_PORT 0x40          // The data port for channel 0
#define PIT_BASE_FREQ 1193182       // 1193182 Hz from the document
#define IRQ0 0
void pit_init();
void pit_handler();
#endif


