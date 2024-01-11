#include "keyboard.h"
#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "paging.h"
#include "systemcall.h"
//#include "terminal_driver.h"

#define CTRL_PUSH 0x1D
#define CTRL_FREE 0x9D
#define RSFT_PUSH 0x36
#define RSFT_FREE 0xB6
#define LSFT_PUSH 0x2A
#define LSFT_FREE 0xAA
#define ALTK_PUSH 0x38
#define ALTK_FREE 0xB8
#define F1KY_PUSH 0x3B
#define F2KY_PUSH 0x3C
#define F3KY_PUSH 0x3D
#define TAB_PUSH 0x0F
#define UP_PUSH   0x48
#define DOWN_PUSH 0x50
#define CPSL_PUSH 0x3A

#define PUSH 1
#define FREE 0

volatile unsigned char cur_ctrl = 0;
volatile unsigned char cur_rsft = 0;
volatile unsigned char cur_lsft = 0;
volatile unsigned char cur_cpsl = 0;
volatile unsigned char cur_altk = 0;

volatile unsigned int kb_buf_idx = 0;
volatile int kb_mode = 0;

// scancode reference: //reference :http://www.osdever.net/bkerndev/Docs/keyboard.htm
unsigned char scancode[4][128] = {
    {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
        '9', '0', '-', '=', '\b',   /* Backspace */
        '\t',      /* Tab */
        'q', 'w', 'e', 'r',        /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
        0,                     /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
        '\'', '`', 0,                    /* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
        'm', ',', '.', '/', 0,     /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0  /* All other keys are undefined */
    },
    {
        0, 0, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
        '(', ')', '_', '+', '\b',       /* Backspace */
        '\t',        /* Tab */
        'Q', 'W', 'E', 'R',           /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',     /* Enter key */
        0,           /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
        '\"', '~', 0,    /* Left shift */
        '|', 'Z', 'X', 'C', 'V', 'B', 'N',   /* 49 */
        'M', '<', '>', '?', 0,    /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0  /* All other keys are undefined */
    },
    {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
        '9', '0', '-', '=', '\b',   /* Backspace */
        '\t',       /* Tab */
        'Q', 'W', 'E', 'R',         /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',     /* Enter key */
        0,       /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
        '\'', '`', 0,         /* Left shift */
        '\\', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
        'M', ',', '.', '/', 0,        /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0  /* All other keys are undefined */
    },
    {
        0, 0, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
        '(', ')', '_', '+', '\b',   /* Backspace */
        '\t',           /* Tab */
        'q', 'w', 'e', 'r',          /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',     /* Enter key */
        0,       /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
        '\"', '~', 0,      /* Left shift */
        '|', 'z', 'x', 'c', 'v', 'b', 'n',    /* 49 */
        'm', '<', '>', '?', 0,       /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0  /* All other keys are undefined */
    }};
/*
keyboard_init
Description: keyboard initialization, activing corresponding irq
input: none
output: none
return: none
*/
void keyboard_init(void)
{
    free_all();
    enable_irq(KB_IRQ); // enable interrupt of keyboard
}

/*
keyboard_handler
Description: get character from ps/2 port and display it on the screen
input: none
output: none
return: none
side effect:
*/
void keyboard_handler(void)
{
    cli();
    uint8_t input_code;
    int settler;
    input_code = inb(KB_PORT); // get scancode index from ps/2 port
    unsigned char checker, charac;
    checker = scancode[0][input_code];
    /* detect FREE a key */
    /* in this way we can ignore those FREEd qwert... */
    settler = 1;
    //TAB
    int32_t fd, cnt;
    int cnt_command=0;
    int cnt_copy=0;
    uint8_t buf[32];//32 MAX SIZE OF FILENAME
    uint8_t temp_buf[32];//32 MAX SIZE OF FILENAME
    uint8_t temp_buf2[32];//32 MAX SIZE OF FILENAME
    uint8_t kb_buf_copy[32];//32 MAX SIZE OF FILENAME
    if(input_code==TAB_PUSH){
        if (-1 == (fd = open ((uint8_t*)"."))) {
        printf ("directory open failed\n");
         sti();
      send_eoi(KB_IRQ);
        return;
    }


    int k;
    for(k=0;k<kb_buf_idx;k++){
        if(keyboard_buffer[k]==' '){
            while(k<kb_buf_idx&&keyboard_buffer[k]==' ') k++;
            break;
        }
    }


    if(k<kb_buf_idx){
        strncpy(kb_buf_copy,keyboard_buffer+k,kb_buf_idx-k);
        kb_buf_copy[kb_buf_idx-k]='\0';
    }
    else{
        strncpy(kb_buf_copy,keyboard_buffer,kb_buf_idx);
        k=0;
    }


    while (0 != (cnt = read (fd, buf, 32))) {//32 MAX SIZE OF FILENAME
        if (-1 == cnt) {
	         printf("directory entry read failed\n");
              sti();
        send_eoi(KB_IRQ);
	        return ;
	    }
	    buf[cnt] = '\0';
        if(strncmp(kb_buf_copy,buf,kb_buf_idx-k)==0){
            cnt_command++;
            if(cnt_command==2){
                 sti();
        send_eoi(KB_IRQ);
                return;
            }if(cnt_command==1){
               strncpy(temp_buf2,buf,cnt);
               temp_buf2[cnt]='\0';
               strncpy(temp_buf,buf+kb_buf_idx-k,cnt-kb_buf_idx+k);
               temp_buf[cnt-kb_buf_idx+k]='\0';
               cnt_copy=cnt;
            }
        }

    }close(fd);
    if(cnt_command==1){

        // int k;
        // for(k=kb_buf_idx;k<cnt;k++){
        //     putc_modf(temp_buf[k]);
        // }
        strncpy(keyboard_buffer+k,temp_buf2,cnt_copy);
        kb_buf_idx=k+cnt_copy;
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
        flush_tlb();
        printf(temp_buf);
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
        flush_tlb();
    }
      sti();
      send_eoi(KB_IRQ);
      return;
    }

    if(input_code==UP_PUSH){
        command_count[cur_display_terminal_id-1]--;
        if(command_count[cur_display_terminal_id-1]<0){
            command_count[cur_display_terminal_id-1]=0;
        }
        while(kb_buf_idx>0)
        {
            kb_buf_idx--;
            keyboard_buffer[kb_buf_idx]='\0';
            page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
            flush_tlb();
            putc_modf('\b');
            page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
            flush_tlb();

        }
        //uint8_t command_buf[20];//max number 20
        int size_c=strlen(command_list[cur_display_terminal_id-1][command_count[cur_display_terminal_id-1]]);
        strncpy(keyboard_buffer,command_list[cur_display_terminal_id-1][command_count[cur_display_terminal_id-1]],size_c);
        keyboard_buffer[size_c]='\0';
        kb_buf_idx=size_c;
        // command_count--;
        
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
            flush_tlb();
        printf(keyboard_buffer);
         page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
            flush_tlb();

        sti();
      send_eoi(KB_IRQ);
      return;
        
    }
     if(input_code==DOWN_PUSH){
         command_count[cur_display_terminal_id-1]++;
         while(kb_buf_idx>0)
        {
            kb_buf_idx--;
            keyboard_buffer[kb_buf_idx]='\0';
            page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
            flush_tlb();
            putc_modf('\b');
            page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
            flush_tlb();

        }
        if(command_count[cur_display_terminal_id-1]>=command_total[cur_display_terminal_id-1]){
            command_count[cur_display_terminal_id-1]=command_total[cur_display_terminal_id-1];
             keyboard_buffer[0]='\0';
             kb_buf_idx=0;
             sti();
             send_eoi(KB_IRQ);
             return;

         }
        
        //uint8_t command_buf[20];//max number 20
        int size_c=strlen(command_list[cur_display_terminal_id-1][command_count[cur_display_terminal_id-1]]);
        strncpy(keyboard_buffer,command_list[cur_display_terminal_id-1][command_count[cur_display_terminal_id-1]],size_c);
        keyboard_buffer[size_c]='\0';
        kb_buf_idx=size_c;
        // command_count++;
        // if(command_count>=command_total){
        //     command_count=command_total-1;
        // }
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
            flush_tlb();
        printf(keyboard_buffer);
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
            flush_tlb();
        sti();
      send_eoi(KB_IRQ);
      return;
    }
    if (input_code & FREE_CHECKER)
    {

        if (input_code == CTRL_FREE)
            cur_ctrl = FREE;
        if (input_code == LSFT_FREE)
            cur_lsft = FREE;
        if (input_code == RSFT_FREE)
            cur_rsft = FREE;
        if (input_code == ALTK_FREE)
            cur_altk = FREE;
        sti();
        send_eoi(KB_IRQ); // bug1: forget to use send_eoi, my keyboard don't work
        return;
    }

    /* detect FREE a key */
    if (input_code == CPSL_PUSH)
        cur_cpsl ^= PUSH;
    if (input_code == CTRL_PUSH)
        cur_ctrl = PUSH;
    if (input_code == RSFT_PUSH)
        cur_rsft = PUSH;
    if (input_code == LSFT_PUSH)
        cur_lsft = PUSH;
    if (input_code == ALTK_PUSH)
        cur_altk = PUSH;

    if (checker == '\b')
    {
        if (!kb_buf_idx)
        {
            sti();
            send_eoi(KB_IRQ);
            return;
        }
    }
    if (cur_ctrl == PUSH)
    {
        /* control + l */
        if (scancode[0][input_code] == 'l')
        {
            flush_screen();
            kb_buf_idx = 0; // clean buffer
        }
    //     if (input_code==CKY_PUSH){
      
    //     send_eoi(KB_IRQ);
    //     send_sig_info(INTERRUPT_SIG);
    //     set_handler(INTERRUPT_SIG,NULL);
        
    // }
    }
    if(cur_altk == PUSH)
    {
        switch (input_code)
        {
        case F1KY_PUSH:
            // cur_altk = FREE;
            send_eoi(KB_IRQ); // send eoi
            terminal_shift(TERMINAL_1);
            return;
        
        case F2KY_PUSH:
            // cur_altk = FREE;
            send_eoi(KB_IRQ); // send eoi
            terminal_shift(TERMINAL_2);
           
            return;

        case F3KY_PUSH:
            // cur_altk = FREE;
            send_eoi(KB_IRQ); // send eoi
            terminal_shift(TERMINAL_3);

            return;

        default:
            // cur_altk = FREE;
            send_eoi(KB_IRQ);
            return;
        }

    }
    else
    {   
        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
        flush_tlb();
        /* get the first dim of array number */
        kb_mode = cur_rsft | cur_lsft | (cur_cpsl << 1);
        charac = scancode[kb_mode][input_code];

        if (checker == '\b' && kb_buf_idx > 0)
        {
            kb_buf_idx--;
            putc_modf(charac);
        }
        else if (checker == '\n' || checker == '\r')
        { /* check enter */
            keyboard_buffer[kb_buf_idx] = charac;
            kb_buf_idx = 0; /* renew our buffer */
            if (settler)
            {
                putc_modf(charac);
            }
        }
        else if (kb_buf_idx < KB_BUF_SIZE - 1)
        { /* for enter -1 */

            if (charac == 0)
            {
                if (settler)
                {
                    if(terminal_now!=(cur_display_terminal_id-1)){
                        page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
                        flush_tlb();
                    }

                    sti();
                    send_eoi(KB_IRQ);
                    return;
                }
            } /* check undefined key */

            keyboard_buffer[kb_buf_idx] = charac;
            /* prepare for new data in keyboard buffer */
            kb_buf_idx++;
            putc_modf(charac);
        }
        if(terminal_now!=(cur_display_terminal_id-1)){
            page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT;//directory point to updated physical address
            flush_tlb();
        }
    }
    // pcb_t* cur_pcb;
    // cur_pcb=get_cur_pcb_addr();
    // if (check_pengding()==1){
    //     while(cur_pcb->pending_signals[0].signum!=-999){
    //         if(cur_pcb->pending_signals[0].signum==0||cur_pcb->pending_signals[0].signum==1||cur_pcb->pending_signals[0].signum==2){
    //              set_handler(INTERRUPT_SIG,NULL);
    //         }
    //         int i=1;
    //         while(cur_pcb->pending_signals[i].signum!=-999&&i<10){
    //                 cur_pcb->pending_signals[i-1].signum=cur_pcb->pending_signals[i].signum;
    //                 i=i+1;
    //         }
             
            
    //     }
    // }
    sti();
    send_eoi(KB_IRQ); // send eoi
} // if(kb_buf_idx--<0) kb_buf_idx=0;260

/* help function */
void free_all(void)
{
    cur_ctrl = FREE;
    cur_lsft = FREE;
    cur_rsft = FREE;
    cur_cpsl = FREE;
    cur_altk = FREE;
}
