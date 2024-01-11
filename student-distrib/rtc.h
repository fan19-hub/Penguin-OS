#ifndef _RTC
#define _RTC
#include "i8259.h"
#include "lib.h"

#define IRQ8 8
#define RTC_A_REG 0x8B
#define RTC_B_REG 0x8B
#define RTC_C_REG 0x8C
#define RTC_IO_PORT0 0x70
#define RTC_IO_PORT1 0x71
#define mask6 0x40
#define RTC_FREQ_INITI 1024
#define RTC_FREQ_ACTU 2
#define RATE_DEFAULT 15
#define MASK_RATE 0x0F
#define RATE_MIN 3
#define MASK_PREV 0xF0
#define INITIAL_FREQ 1024
void rtc_handler(); //handle the RTC interrupt
void rtc_init();    //initialize the RTC device
int32_t rtc_open(const uint8_t* filename);               //initilize the rtc and set the frequency to 2 hz 
int32_t rtc_close(int32_t fd);                  //Close RTC device
int32_t rtc_write(int32_t fd, const void* buf, int32_t length);   //write a new frequency to rtc by reading a file buffer as a pointer
int32_t rtc_read(int32_t fd, void* buf, int32_t length);    //only enable rtc interrupt to read at some specific moment according to frequency
void rtc_rate_change(int32_t freq_set); //change frequency by setting a new value frequency to poart
#endif

