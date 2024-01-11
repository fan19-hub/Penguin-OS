/* terminal.h
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

#define TERMINAL_BUF_SIZE 128
#define TERMINAL_1 1
#define TERMINAL_2 2
#define TERMINAL_3 3
#define KB_BUF_SIZE    128

/* cp1 */

/* return 0 */
int32_t terminal_open(const uint8_t* filename);

/* return 0 */
int32_t terminal_close(int32_t fd);

/* put data in kb buf to terminal buf */
int32_t terminal_read(int32_t fd, void * buf, int32_t num);

/* put data in terminal buf to screen */
int32_t terminal_write(int32_t fd, const void * buf, int32_t num); 

/* determine when we need to start read (detect the "\n" signal which means kb buf is ready) */
int32_t detect_enter_signal();

/* cp5 */
typedef struct terminal {
    int terminal_id;
    int cursor_x;
    int cursor_y;
    int pidarray[6];
    int cur_pid_idx;
    int kb_buf_idx_terminal;
    unsigned char keyboard_buffer_terminal[KB_BUF_SIZE];


} terminal_t;

terminal_t terminal_1;
terminal_t terminal_2;
terminal_t terminal_3;

volatile unsigned int cur_display_terminal_id;

// terminal_t initialize
void terminal_init();

void terminal_shift(int terminal_id);

void terminal_store(int terminal_id);

void terminal_load(int terminal_id);

void terminal_init_helpfunction(int terminal_id);
extern char command_list[3][50][50]; //at most 100 command stored
extern int command_total[3];
extern int command_count[3];
terminal_t* get_terminal_addr(int terminal_id);

#endif /* _TERMINAL_H */
