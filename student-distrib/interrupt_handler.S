
#define ASM


/* make assembly linkage of interrupt */

#define INT_HANDLER(name,int_handler)   \
.global name                    ;\
name:       pushal                      ;\
call int_handler                ;\
popal                       ;\
iret                        ;\


INT_HANDLER(pit_wrap, pit_handler)
INT_HANDLER(keyboard_wrap, keyboard_handler)
INT_HANDLER(rtc_wrap, rtc_handler)

