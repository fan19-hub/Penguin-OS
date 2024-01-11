#define ASM_SYS    1

.data               # data section declaration

# the jump table used for system call
syscall_jump_table: # the first item is not defined because index in eax is 1-10
    .long undefined
    .long halt
    .long execute 
    .long read
    .long write
    .long open 
    .long close
    .long getargs
    .long vidmap
    .long undefined
    .long undefined
    .long remove

.text               # code section declaration

.global syscall_handler
.global flush_tlb
.global store_esp
.global store_ebp
.global Halt_ret

# void syscall_handler();
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %eax - the system call number
#            %ebx - the first argument for system call function
#            %ecx - the second argument for system call function
#            %edx - the third argument for system call function
#   Outputs: %eax - the return value of the system call function. -1 for failure
syscall_handler:
    # Save the callee saved registers
    pushl %edi
    pushl %esi
    pushl %ebp

    # Check if call_num is valid(1-10)
    cmpl    $12, %eax           # if call_num>=11 or call_num<0 
    jae     SYSCALL_FAIL
    cmpl    $0, %eax            # if call_num==0
    je      SYSCALL_FAIL

    # System call
    pushl %edx              # third argument
    pushl %ecx              # second argument
    pushl %ebx              # first argument
    call *syscall_jump_table(, %eax, 4)   # long type has 32bit/8=4byte

    # restore caller saved registers but keeping eax 
    popl %ebx             
    popl %ecx       
    popl %edx       
    
    SYSCALL_END:
        # restore the callee saved registers
        popl %ebp
        popl %esi
        popl %edi
        iret

    SYSCALL_FAIL:
        orl $0xFFFFFFFF, %eax           # return -1 for failure
        jmp SYSCALL_END

# uint32_t store_esp();
#    Inputs: None
#    Outputs: copy esp to eax to store it
store_esp:
    movl %esp,%eax
    ret

# uint32_t store_ebp();
#    Inputs: None
#    Outputs: copy esp to eax to store it
store_ebp: # copy ebp
    movl %ebp,%eax
    ret

# void flush_tlb ();
# Interface: 
#    Inputs: None
#   Outputs: reload cr3 
flush_tlb:
    movl %cr3,%eax
    movl %eax,%cr3
    ret
