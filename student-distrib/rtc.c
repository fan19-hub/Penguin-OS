#include "rtc.h"
#include "i8259.h"
#include "lib.h"
#include "tests.h"
#include "systemcall.h"
volatile uint32_t counter;
volatile uint32_t rtc_flg;
uint32_t rtc_ct;
uint32_t alarm_ct;
volatile uint32_t alarm_count;
/* void rtc_handler();
 * Inputs: None
 * Return Value: None
 * Function: handle the rtc interrupt by running the test_interrupts, read the RTC register C and send EOI signal */
void rtc_handler(){
    cli();             // protect the interrupt hanlding
    
    // if(get_rtc_test()){
	//     test_interrupts(); // if RTC interrupts are being received, run test interrupts
    // }

    counter=counter-1; 
    // rtc_flg=0;
    if(counter==0){
        counter=rtc_ct;        // reset counter using the current rtc's counter
        rtc_flg=1;             // enable rtc interrupt to happen

    }
    // if(alarm_count==0){
    //     alarm_count=alarm_ct;        // reset counter using the current rtc's counter
    //     // printf("alarm");
    //     // send_sig_info(ALARM_SIG);
    //     send_sig_info(4);
    // }
    // pcb_t* cur_pcb;
    // cur_pcb=get_cur_pcb_addr();
    // if (check_pengding()==1){
    //     while(cur_pcb->pending_signals[0].signum!=-999){
    //         if(cur_pcb->pending_signals[0].signum==0||cur_pcb->pending_signals[0].signum==1||cur_pcb->pending_signals[0].signum==2){
    //              set_handler(1,NULL);
    //         }
    //         int i=1;
    //         while(cur_pcb->pending_signals[i].signum!=-999&&i<10){
    //                 cur_pcb->pending_signals[i-1].signum=cur_pcb->pending_signals[i].signum;
    //                 i=i+1;
    //         }
             
            
    //     }
    // }
    // Read the RTC register C
    // so that it can continuously send interrupts
    outb(RTC_C_REG, RTC_IO_PORT0); // select RTC registerC 
    inb(RTC_IO_PORT1);		       // return data is not needed
    
	send_eoi(IRQ8);                // finish the interrupt
	sti();
}

/* void rtc_handler();
 * Inputs: None
 * Return Value: None
 * Function: initialize the RTC by turning on the bit 6 of register B and setting the lower 4 bits of register A for the frequency 
 *           Finally enable the interrupt port 8 on PIC*/
void rtc_init(){
    char prev;        
    // turns on the bit 6 of Register B
	outb(RTC_B_REG, RTC_IO_PORT0);  // Ox70 is used to specify the register index
 	prev = inb(RTC_IO_PORT1);       // read the data from 0x71
 	outb(RTC_B_REG, RTC_IO_PORT0);  // reselect the register B (Reading will automatically select the register D)
    outb(prev|mask6, RTC_IO_PORT1); // change the bit 6 of prev to be 1 and write it back. This turns on bit 6 of register B 

    // set the requency by writing to Register A
    int rate = 15;
    outb(RTC_A_REG, RTC_IO_PORT0);
    prev = inb(RTC_IO_PORT1);
    outb(RTC_A_REG, RTC_IO_PORT0);
    outb((prev & 0xf0) | rate, RTC_IO_PORT1);
    counter=RTC_FREQ_INITI/RTC_FREQ_ACTU;  //initialize the counter of rtc , set it to default_rtc_counter 
    rtc_flg=0;                              //initialize rtc flag by setting it to 0 , which disable rtc interrupt happening
    rtc_ct=counter;                         //initilaize rtc_ct by setting it to  default_rtc_counter 
 	enable_irq(IRQ8);
    rtc_rate_change(INITIAL_FREQ);          //initialize the frequency as 1024
}



/* void rtc_open();
 * Inputs: None
 * Return Value:0
 * Function: initialize the RTC ,and make the interval_ct pointer to store the number of interval
 * of rtc interval*/
int32_t rtc_open(const uint8_t* filename){
    rtc_ct=RTC_FREQ_INITI/RTC_FREQ_ACTU;
    return 0;
}




/* rtc_close
 * Inputs:  fd  - File descriptor number
 * Return Value: 0 on success, -1 on failure
 * Function: Close RTC device  */
int32_t rtc_close(int32_t fd) {
    return 0;
}
/* rtc_write
 * Inputs:  buf - new frequency for rtc
            length - length of input buf, which should be size of int32_t
            rtc_ct - the counter to record whether rtc interrupt is available
 * Return Value: 0 on success, -1 on failure
 * Function: Close RTC device  */
int32_t rtc_write(int32_t fd, const void* buf, int32_t length){
    if (buf==NULL){     //check whether the input pointer is NULL
        return -1;
    }
    if (length!=sizeof(uint32_t))
    {          //check whether the input length is qualified
        return -1;   
    }
    
    int32_t freq;
    freq=*(int32_t*)buf;
    if(freq>RTC_FREQ_INITI){
        return -1;      //The frequent should not be larger than 1024 hz
    }
    if((freq&(freq-1))!=0){      //test whether freq is  power of 2
        return -1;
    }
    cli();
    rtc_ct=RTC_FREQ_INITI/freq;
    sti();   //to prevent rtc_ct modified by other process
    return 0;
}


/* rtc_read
 * Inputs:  buf - new frequency for rtc
            length - length of inpiut buf, which should be size of int32_t
            fd - file 
 * Return Value: 0 on success, -1 on failure
 * Function: enable rtc to read the file according to frequency by using a counter flg to achieve it */
int32_t rtc_read(int32_t fd, void* buf, int32_t length){
    rtc_flg=0;        
    while(rtc_flg==0); 
    return 0;
}
/* rtc_rate_changea
 * Inputs: 
           
            freq_set - frequency that are set by user for test
 * Return Value: 0 on success, -1 on failure
 * Function: enable rtc to read the file according to frequency by using a counter flg to achieve it */
void rtc_rate_change(int32_t freq_set){
    int32_t result=0;    //initialize result , which will count log2 of frequency we set
    int32_t temp=freq_set;  //copy freq_set to temp
    char prev;
    temp=temp>>1;        
    while(temp!=0){
        result++;                //count log2 of frequency
        temp=temp>>1;
    }
    
    char rate= (RATE_DEFAULT-result+1)& MASK_RATE;      //frequency = 32768 >>(rate-1) and only get the least significant bits
                     
    if( rate < RATE_MIN){               //when rate-1<2, that is , frequency >1024, which is invalid ,then return 
        return;
    }
    cli();                               //mask other interrupt 
    outb(RTC_A_REG, RTC_IO_PORT0);         //set port index to register A
    prev = inb(RTC_IO_PORT1);               // get value of register A
    outb(RTC_A_REG, RTC_IO_PORT0);          //set port index to register A again
    outb((prev & MASK_PREV) | rate, RTC_IO_PORT1); // write new rate to A
    sti();
}
