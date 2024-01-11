/* terminal_driver.c -
 */

#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "paging.h"
#include "systemcall.h"
#include "pit.h"
#include "x86_desc.h"
#define TERMINAL_BUF_SIZE 128
static int enter_position;
char command_list[3][50][50]; //at most 100 command stored
int command_total[3];
int command_count[3];
/* terminal_open - initializes terminal stuff (or nothing), return 0
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage:
 */
int32_t terminal_open(const uint8_t *filename)
{
    return 0;
}

/* terminal_close - clears any terminal specific variables (or do nothing), return 0
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage:
 */
int32_t terminal_close(int32_t fd)
{
    return 0;
}

/* terminal_read - put the value in kb buf to terminal buffer
 *
 * Inputs: None
 * Outputs: return number of bytes read
 * Side Effects: None
 * Coverage:
 */
int32_t terminal_read(int32_t fd, void *buf, int32_t num)
{
    /* null case check for pointer */
    int settler = 1; 
    if (buf == NULL)
        return -1;
    cli();
    uint32_t real_read_number;

    if (num > TERMINAL_BUF_SIZE && settler)
        real_read_number = TERMINAL_BUF_SIZE;
    else
        real_read_number = num;

    /* infinite while loop*/
    /* Once '\n' is detected, the keyboard buffer would be written to buf */
    while (settler)
    {
        cli();
        if (detect_enter_signal()&&(cur_display_terminal_id-1)==terminal_now)
        {
            switch(cur_display_terminal_id-1){
    case 0 : 
        strncpy(command_list[0][command_total[0]],keyboard_buffer,strlen(keyboard_buffer)-1);
        command_list[0][command_total[0]][strlen(keyboard_buffer)-1]='\0';
        break;
    case 1:
        strncpy(command_list[1][command_total[1]],keyboard_buffer,strlen(keyboard_buffer)-1);
        command_list[1][command_total[1]][strlen(keyboard_buffer)-1]='\0';
        break;
    case 2:
        strncpy(command_list[2][command_total[2]],keyboard_buffer,strlen(keyboard_buffer)-1);
        command_list[2][command_total[2]][strlen(keyboard_buffer)-1]='\0';
        break;

    }
    
    // command_list[command_total][strlen(command)]='\0';
    command_total[cur_display_terminal_id-1]++;
    command_count[cur_display_terminal_id-1]=command_total[cur_display_terminal_id-1];
            /* find final real read number */
            if (real_read_number > enter_position)
                real_read_number = enter_position + 1;

            /* copy the data */
            if (settler > 0){(void)strncpy((int8_t *)buf, (int8_t *)keyboard_buffer, real_read_number);}
            if (settler== 1){memset((void *)keyboard_buffer, 0, (uint32_t)KB_BUF_SIZE);}
            /* reset keyboard memory */
            
            break;
        }
        sti();
    }

    sti();
    return real_read_number;
}

/* Terminal_write - write the value in buf to screen
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage:
 */
int32_t terminal_write(int32_t fd, const void *buf, int32_t num)
{
    /* null case check for terminal */
    if (!buf){
        printf("NULL pointer invalid");
        return -1;
    }
    cli();
    uint32_t i = 0;
    uint8_t character;
    
    if (strncmp("391OS> ",((int8_t *)buf),7)==0){
        putc_modf('\n');
    }
    
    while (i < num){
        character = ((int8_t *)buf)[i];
        
        if (terminal_now == cur_display_terminal_id-1){
            putc_modf(character);
        }else{
            putc_backstage(character);
        }
        
        i++;
    }

    if (num < 0){
        printf("no sense");
    }
    /* to make the data in ternimal easier to read (formation) */
    // if (((int8_t*)buf)[num-1]!='\n'){
    // 	putc_modf('\n');
    // }
    sti();
    return num;
}

/* detect_enter_signal - detect \n to start terminal read
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage:
 */
int32_t detect_enter_signal()
{
    int i = 0;
    unsigned char temp;
    while (i < KB_BUF_SIZE)
    {
        temp = keyboard_buffer[i];
        switch (temp)
        {
        case '\n' : 
            enter_position = i;
            return 1;
            break;
        case '\r' : 
            enter_position = i;
            return 1;
            break;
        default:
            break;
        }
        i++;
    }
    
    /* fail */
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

terminal_t* get_terminal_addr(int terminal_id){
    switch (terminal_id)
    {
    case TERMINAL_1:
        return &terminal_1;
        break;
    case TERMINAL_2:
        return &terminal_2;
        break;
    case TERMINAL_3:
        return &terminal_3;
        break;
    default:
        return &terminal_1;
        break;
    }
}

void terminal_init(){

    cur_display_terminal_id = TERMINAL_1;

    terminal_1.terminal_id = TERMINAL_1;
    pcb_array[0] = 0;
    //terminal_init_helpfunction(TERMINAL_1);
    terminal_store(terminal_1.terminal_id);
    
    terminal_2.terminal_id = TERMINAL_2;
    pcb_array[1] = -1;
    //terminal_init_helpfunction(TERMINAL_2);
    terminal_store(terminal_2.terminal_id);

    terminal_3.terminal_id = TERMINAL_3;
    pcb_array[2] = -1;
    //terminal_init_helpfunction(TERMINAL_3);
    terminal_store(terminal_3.terminal_id);

}

void terminal_init_helpfunction(terminal_id){
    get_terminal_addr(terminal_id)->cursor_x = 0;
    get_terminal_addr(terminal_id)->cursor_y = 0;
}

void terminal_shift(terminal_id){
    cli();
    // If the execute fails, we use these value to switch back
    int terminal_last=terminal_now;
    int cur_display_last=cur_display_terminal_id;

    // No need to switch to itself, return
    if (terminal_id == cur_display_terminal_id) {
        sti();
        return;}

    // To support the memory copy in terminal_store and teminal_load, we need to force the parallel mapping
    page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=VIDEO_MEM_BASE | PRESENT| R_W |U_S;
    flush_tlb();

    /* store the cur terminal information */
    terminal_store(cur_display_terminal_id);
    cur_display_terminal_id = terminal_id; //update the current displayed terminal id
    
    /* switch the terminal */
    switch (cur_display_terminal_id)
    {
        case TERMINAL_1:
            terminal_load(cur_display_terminal_id); // load the corresponding video page
            break;
        case TERMINAL_2:
            if (pcb_array[1] == -1)
            {
                clear();
                pcb_t* cur_pcb=get_pcb_addr(pcb_array[terminal_now]);
                asm volatile(
                    "movl %%esp, %%eax;"
                    "movl %%ebp, %%ebx;"
                    : "=a" (cur_pcb->esp), "=b" (cur_pcb->ebp)
                );

                terminal_now=cur_display_terminal_id-1;
                kb_buf_idx=0;
                keyboard_buffer[0]='\0';
                //check the result of execute
                if(-1==execute((uint8_t*)"shell")){
                    terminal_now=terminal_last;
                    cur_display_terminal_id = cur_display_last;
                    terminal_load(cur_display_terminal_id);
                }
                break;
            }
            terminal_load(cur_display_terminal_id);
            break;
        case TERMINAL_3:
            if (pcb_array[2] == -1)
            {
                clear();
                pcb_t* cur_pcb=get_pcb_addr(pcb_array[terminal_now]);
                asm volatile(
                    "movl %%esp, %%eax;"
                    "movl %%ebp, %%ebx;"
                    : "=a" (cur_pcb->esp), "=b" (cur_pcb->ebp)
                );
                
                terminal_now=cur_display_terminal_id-1;
                kb_buf_idx=0;
                keyboard_buffer[0]='\0';
                //check the result of execute
                if(-1==execute((uint8_t*)"shell")){
                    terminal_now=terminal_last;
                    cur_display_terminal_id = cur_display_last;
                    terminal_load(cur_display_terminal_id);
                }
                break;
            }
            terminal_load(cur_display_terminal_id);
            break;
        default:
            break;
    }
    // send_sig_info(5);
    page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB]=(VIDEO_MEM_T1+terminal_now*PG_SIZE_4KB)| PRESENT| R_W |U_S;
    flush_tlb();
    sti();
}

void terminal_store(terminal_id){
    cli();
    
    /* get addr */
    terminal_t* store_terminal_addr;
    store_terminal_addr = get_terminal_addr(terminal_id);
    /* store the cursor position for this terminal */
    store_terminal_addr->cursor_x = get_cursor_x();
    store_terminal_addr->cursor_y = get_cursor_y();
    /* store the keyboard_information: kbid, kbbf */
    store_terminal_addr->kb_buf_idx_terminal= kb_buf_idx;
    memcpy(store_terminal_addr->keyboard_buffer_terminal, keyboard_buffer, KB_BUF_SIZE);
    /* store vidmem to the terminal */
    memcpy((void*)  VIDEO_MEM_BASE + terminal_id*PG_SIZE_4KB, (void*) VIDEO_MEM_BASE,  PG_SIZE_4KB);
    sti();
}

void terminal_load(terminal_id){
    cli();
    /* get addr */
    terminal_t* load_terminal_addr;
    load_terminal_addr = get_terminal_addr(terminal_id);
    /* load the cursor position for this terminal */
    change_cursor_x(load_terminal_addr->cursor_x);
    change_cursor_y(load_terminal_addr->cursor_y);
    new_cursor(load_terminal_addr->cursor_x, load_terminal_addr->cursor_y);
    /* load the keyboard_information: kbid, kbbf */
    kb_buf_idx = load_terminal_addr->kb_buf_idx_terminal;
    memcpy(keyboard_buffer, load_terminal_addr->keyboard_buffer_terminal, KB_BUF_SIZE);
    /* load the terminal to vidmem */
    memcpy((void*) VIDEO_MEM_BASE, (void*)  VIDEO_MEM_BASE + terminal_id*PG_SIZE_4KB,  PG_SIZE_4KB);
    sti();
}
