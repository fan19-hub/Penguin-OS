/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask=0xFF; /* IRQs 0-7  */
uint8_t slave_mask=0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
i8259_init
Description: pic initialization for both master and slave
input: none
output: none
return: none 
*/
void i8259_init(void) {
    outb(master_mask,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port
                                        //mask all of 8259A-1
    outb(slave_mask,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port
                                        //mask all of 8259A-2
    outb(ICW1,MASTER_8259_PORT);//ICW1: select 8259A-1 init
    outb(ICW2_MASTER,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port
                                        //ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27
    outb(ICW3_MASTER,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port
                                        //8259A-1 has a secondary on IRQ2
    outb(ICW4,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port

    outb(ICW1,SLAVE_8259_PORT);//ICW1: select 8259A-2 init
    outb(ICW2_SLAVE,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port
                                        //ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2F
    outb(ICW3_SLAVE,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port
                                        //8259A-2 IS a secondary on PRIMARY'S IRQ2
    outb(ICW4,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port

    enable_irq(2);//irq 2 enable slave's irq
}

/* Enable (unmask) the specified IRQ */
/*
enable_irq
Description: unmask the parameter'th bit of irq
input: irq_num --the index of irq to be unmasked
output: none
return: none 
*/
void enable_irq(uint32_t irq_num) {
    if(irq_num>=16){//16:range of irq
        return;
    }
    uint8_t bit_mask;
    if(irq_num>=8){//8: range of master's irq. This condition is for slave's irq
        irq_num=irq_num-8;//get irq in slave 
        bit_mask=1<<irq_num;//set the bit irq_num to 1
        slave_mask=(~bit_mask)&slave_mask;//set the bit irq_num to 0 and do nothing to other bits of slave_mask
        outb(slave_mask,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port
    }
    else{
        bit_mask=1<<irq_num;//set the bit irq_num to 1
        master_mask=(~bit_mask)&master_mask;//set the bit irq_num to 0 and do nothing to other bits of master_mask
        outb(master_mask,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port.
                                    //send unmasked irq to master data port
    }
}

/* Disable (mask) the specified IRQ */
//if set to 1 means irq needs to be mask
/*
disable_irq
Description: mask the parameter'th bit of irq
input: irq_num --the index of irq to be masked
output: none
return: none 
*/
void disable_irq(uint32_t irq_num) {
    if(irq_num>=16){//16:range of irq
        return;
    }
    uint8_t bit_mask;
    if(irq_num>=8){//8: range of master's irq. This condition is for slave's irq
        irq_num=irq_num-8;//get irq in slave 
        bit_mask=1<<irq_num;//set the bit irq_num to 1
        slave_mask=(bit_mask)|slave_mask;//set the bit irq_num to 1 and do nothing to other bits of slave_mask
        outb(slave_mask,SLAVE_8259_PORT+1);//SLAVE_8259_PORT+1: slave_8259 data port
    }
    else{
        bit_mask=1<<irq_num;//set the bit irq_num to 1
        master_mask=(bit_mask)|master_mask;//set the bit irq_num to 1 and do nothing to other bits of master_mask
        outb(master_mask,MASTER_8259_PORT+1);//MASTER_8259_PORT+1: master_8259 data port.
                                    //send unmasked irq to master data port
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
/*
send_eoi
Description: send eoi signal of corresponding irq to pic
input: irq_num --the index of irq
output: none
return: none 
*/
void send_eoi(uint32_t irq_num) {
    if(irq_num>=16){//16:range of irq
        return;
    }uint8_t eoi1;
    eoi1=EOI;
    if(irq_num>=8){//8: range of master's irq
        irq_num=irq_num-8;//get irq in slave 
        eoi1|=irq_num;//This gets OR'd with the interrupt number and sent out to the PIC
                      //to declare the interrupt finished
        outb(eoi1,SLAVE_8259_PORT);//SLAVE_8259_PORT+1: slave_8259 data port
        irq_num=2;//change irq to irq connectes slave 8250 port
    }   eoi1=EOI;
        eoi1|=irq_num;//This gets OR'd with the interrupt number and sent out to the PIC
                      //to declare the interrupt finished
        outb(eoi1,MASTER_8259_PORT);//MASTER_8259_PORT+1: master_8259 data port
}

