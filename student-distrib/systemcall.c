#include "systemcall.h"
#include "paging.h"
#include "x86_desc.h"

int id_array[6]={0,0,0,0,0,0};//6 process allowed
int parent_array[3]={0,-1,-1};//3 terminals
int32_t pcb_array[3]={0,-1,-1};
int terminal_now=0;
uint8_t ELF[4]={0x7f, 0x45, 0x4c, 0x46};//the first 4 chars in executable files.
// file operation table
file_opt_table_t stdin_opt_table={terminal_read, NULL, terminal_open, terminal_close}; //the file operation table of stdin
file_opt_table_t stdout_opt_table={terminal_read, terminal_write, NULL, terminal_close};  //the file operation table of stdout
file_opt_table_t RTC_opt_table={rtc_read, rtc_write, rtc_open, rtc_close};//the file operation table of rtc
file_opt_table_t DIR_opt_table={dir_read, dir_write, dir_open, dir_close};//the file operation table of directory
file_opt_table_t REG_FILE_opt_table={file_read, file_write, file_open, file_close};////the file operation table of file

/*
get_cur_pcb_addr
Descriptor:call get_pcb_addr(pcb_id) to get the address of current pcb
Input:none
Output:none
Return: get_pcb_addr(pcb_id)
*/
pcb_t* get_cur_pcb_addr(){
    return get_pcb_addr(pcb_array[terminal_now]);
}

/*
get_pcb_addr
Descriptor:get the address of current pcb
Input:none
Output:none
Return: pcb --the address of current pcb (type pcb)
Side effect: read memory
*/
pcb_t* get_pcb_addr(uint32_t id){
    return (pcb_t*)(PROGRAM_START_ADDR-(id+1)*STACK_8KB);
}
/*
pcb_initialize
Descriptor:initialize pcb
Input:pcb--pcb pointer 
Output:none
Return:none
*/
void pcb_initialize(pcb_t *pcb){
    int i;
    //set registers to 0
    pcb->esp_ret=0;
    pcb->ebp_ret=0;
    pcb->eip_user=0;
    pcb->esp_user=0;
    pcb->ebp=0;
    pcb->esp=0;
    //set id to 0
    pcb->parent_id=0; 
    pcb->id=0;

   //initialize items in file_array
   for (i=0;i<PCB_SIZE;i++){
       
        pcb->file_array[i].flg=0;    //mark this entry not used
        pcb->file_array[i].file_pos=0;  //not begin to read file 
        pcb->file_array[i].inode=0;    
        pcb->file_array[i].table_ptr=NULL; //file_operation table doesn't point anything
    //initialzie inode
        
    }
    pcb->file_array[0].table_ptr =&stdin_opt_table;//initialize stdin
    pcb->file_array[0].flg=1;//busy
    pcb->file_array[1].table_ptr =&stdout_opt_table;//initialize stdout
    pcb->file_array[1].flg=1;//busy


}

/*
execute
description: for executable file, it can switch privilege from 0 to 3 to execute the user level file
input:  command --command include user file name and some arguments
output: page mapping. Load file into virtual memory
return: -1 =fails. 0 = succeed
*/
int32_t execute(const uint8_t*  command){
    int i,u;
    char file_name[MAX_NAME_LEN];//store modified command
    uint32_t file_len=strlen((char*)command);//file length
    char arg[MAX_ARG_LEN];//file argument
    char arg_echo[1000000]; //1000000 is just a very big number
    /////////////////////
    // parse the args ///
    ////////////////////

    u=0;
    i=0;
    cli();
    //invalid length
    if(file_len==0){
        return -1;
    }
    //copy command to filename 
    while(i<file_len){
        //if blank appears first
        if(i==0&&command[i]==' '){
            while(i<file_len&&command[i]==' '){
                i++;
            }if(i==file_len){//if only blank in command
                return -1;
            }
        }
        //if blank appears after words
        if(command[i]==' '){
            break;
        }
        //copy
        file_name[u++]=command[i++];
        
    }
    file_name[u]='\0'; 

    int j;
    u=0;
    for(j=0;j<MAX_ARG_LEN;j++){//initialize every char in arg to \0
            arg[j]='\0';
        }
    //copy argument in command to arg
    int flg_echo=0;
    if(i!=file_len){
       if(command[i]==' '){
           while(i<file_len&&command[i]==' '){
               i++;
           }if(i<file_len){//if only blank in command
           if(command[i]=='"'){
               strncpy(arg_echo,command+i+1,strlen(command)-i-2);
               arg_echo[strlen(command)-i-2]='\0';
               flg_echo=1;
           }else{
                while(u<MAX_ARG_LEN && i<file_len && command[i]!=' '){
                    arg[u++]=command[i++];
                }
            }}
       }
    }

    ////////////////////////
    //check for executable//
    ////////////////////////

    dentry_t dentry;
    //invalid read name
    if(read_dentry_by_name((uint8_t*)file_name,&dentry)==-1){
        return -1;
    }

    int32_t inode_n=dentry.inode_num;//get inode number of current dentry
    uint8_t buf[5]; //size should be bigger than 4
    int32_t re=read_data(inode_n,0,(char*)buf,4);//read the first 4 bytes to check
    if(re==-1){
        return -1;
    }
    //executable or not
    if(buf[0]!=ELF[0]||buf[1]!=ELF[1]||buf[2]!=ELF[2]||buf[3]!=ELF[3]){
        return -1;
    }

    ////////////////
    //page mapping//
    ////////////////

    int id=-1;//id is the current id of user file. Related to its physical address and pcb location
    for(i=0;i<6;i++){//6 is the max number of process
        if(id_array[i]==0){//free
           id=i;
           id_array[i]=1;//busy
           break;
        }
    }
    //error:no free space for current program.
    if(id==-1){
        return -1;
    }
   
    
    uint32_t phy_addr=PROGRAM_START_ADDR+id*PAGE_4MB;//physical address of user file
    page_directory[VIR_ADDR/PAGE_4MB]=(((phy_addr/PAGE_4MB)<<22)| PRESENT | PS |GLOBAL|  R_W |U_S);//directory point to updated physical address
    flush_tlb();//update 
    
   /////////////////////////////////////////
   // Load file into memory---virtual addr//
   /////////////////////////////////////////

    inode_t* inode= (inode_t*)(inode_ptr + inode_n);//inode of current user file
    read_data(inode_n,0,(char*)PROGIMG_ADDR,inode->length);  //copy the entire memory to the program img address.

   ////////////////
   //create pcb////
   ////////////////

    pcb_t *pcb;//create new pcb
    pcb=get_pcb_addr(id);//put it into the address
    pcb_initialize(pcb);//initialize pcb
    pcb->id=id; //set id
    pcb_array[cur_display_terminal_id-1]=id;  //set id
    if(parent_array[cur_display_terminal_id-1]==-1){
        pcb->parent_id=pcb->id;
    }
    else{pcb->parent_id=parent_array[cur_display_terminal_id-1];} //set parent id
    parent_array[cur_display_terminal_id-1]=pcb->id;//set parent id
    if(flg_echo==0){
    strncpy((int8_t*)pcb->arg,(int8_t*)arg,MAX_ARG_LEN);//copy parsed argument to pcb
     }
    else{
    strncpy((int8_t*)pcb->arg,(int8_t*)arg_echo,MAX_ARG_LEN);
    }

   //////////////////////////////
   //Prepare for context switch//
   //////////////////////////////

    uint8_t* buf2;
    buf2= (uint8_t* )PROGIMG_ADDR; //store program img to buf2
    pcb->eip_user=(*(uint32_t*)(buf2+24));//24 is the start byte of content for eip 24-27 bytes
    uint32_t esp_kernel=PROGRAM_START_ADDR-ONE_SAPCE-STACK_8KB*pcb->id;   //kernel stack of user space file
    
    //update TSS
    //TSS USER STACK->KERNEL STACK
    tss.esp0=esp_kernel;
    tss.ss0=KERNEL_DS;

    uint32_t esp_user=VIR_ADDR+PAGE_4MB-ONE_SAPCE; //process' stack point in user space  
    pcb->esp_user=esp_user;
    pcb->esp_ret=store_esp();//store esp
    pcb->ebp_ret=store_ebp();//store ebp

    //////////////////////////////////////
    sti();
    asm volatile("\
        xorl %%eax,%%eax; \
        movw %w3,%%ax; \
        movw %%ax,%%ds; \
        pushl %%eax ;\
        pushl %1 ;\
        pushfl ;\
        popl %%eax ;\
        orl $0x0200,%%eax ;\
        pushl %%eax ;\
        xorl %%eax,%%eax ;\
        movw %w2,%%ax ;\
        pushl %%eax ;\
        pushl %0 ;\
        iret ;\
        RETURN:;\
        "
    :
    :"g"(pcb->eip_user),"g"(pcb->esp_user),"g"(USER_CS),"g"(USER_DS)
    : "%eax"
    );
  
    return 0;
}

//halt
//Description: halt the current process and return to execute
//input: status--process status
//output: page mapping
//return: -1 fails. 
int32_t halt (uint8_t status){
     int i=0;
    int cur_id, signal;
    uint32_t cur_esp, cur_eip, offset, addr_for_pt, ex_ebp, ex_esp;
    pcb_t* cur_pcb_addr = get_cur_pcb_addr();//get current pcb
    pcb_t* par_pcb_addr = get_pcb_addr(cur_pcb_addr->parent_id);//get parent pcb
    cur_id = cur_pcb_addr->id;//current id
    signal = 0;
    //if current id =0, set signal to 1
    if (cur_id==cur_pcb_addr->parent_id){
        cur_eip = cur_pcb_addr->eip_user;
        cur_esp = cur_pcb_addr->esp_user;
        signal = 1;
    }
   //if signal=1, go back to shell
    if (signal == 1){
        asm volatile("\
        xorl %%eax,%%eax; \
        movw %w3,%%ax; \
        movw %%ax,%%ds; \
        pushl %%eax ;\
        pushl %1 ;\
        pushfl ;\
        popl %%eax ;\
        orl $0x0200,%%eax ;\
        pushl %%eax ;\
        xorl %%eax,%%eax ;\
        movw %w2,%%ax ;\
        pushl %%eax ;\
        pushl %0 ;\
        iret ;\
        "
    :
    :"g"(cur_eip),"g"(cur_esp),"g"(USER_CS),"g"(USER_DS)
    : "%eax"
    );}
    
    /* Update pcb structure value */
    pcb_array[terminal_now] = cur_pcb_addr->parent_id;
    parent_array[terminal_now]= par_pcb_addr->parent_id;
    if(pcb_array[terminal_now] == cur_pcb_addr->id){
        id_array[terminal_now] = 1;
    }else{
        id_array[cur_pcb_addr->id] = 0;
    }
   
    offset = cur_pcb_addr->parent_id + OFFSET;
    addr_for_pt = PAGE_4MB * offset;
    ex_ebp = cur_pcb_addr->ebp_ret;//ebp of execute
    ex_esp = cur_pcb_addr->esp_ret;//esp of execute

    /* close fd */  
    while(i<PCB_SIZE){
        cur_pcb_addr->file_array[i].flg = 0;
        i++;
    }
    
    /* restore paging */
    
    page_directory[USER_ID] = (addr_for_pt) | PRESENT | R_W | U_S | PS;
    flush_tlb();

    /* TSS */
    tss.ss0 = KERNEL_DS;  //give kernel stack to ss0
    tss.esp0 = KERNEL_BASE - (par_pcb_addr->id * EIGHT_KB) - 4; /* stack pointer for 4 */

    /* jump to execute return */
    // pcb_array[terminal_now]=par_pcb_addr->id;
    //   if(terminal_now==0){
    //     pcb_array[0]=&par_pcb_addr;
    // }if(terminal_now==1){
    //     pcb_array[1]=&par_pcb_addr;
    // }if(terminal_now==2){
    //     pcb_array[2]=&par_pcb_addr;
    // }
 asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "jmp RETURN;"
        : /* no outputs */
        : "g" (ex_esp), "g" (ex_ebp)
        : "%eax"
    );
    return -1;//fails

}




/*
name: open
function:
according to the type of the dentry to decide whether it is regular file, rtc and directory,
, then store index inode to the pcb's array_t.
input : --filename
output : fd-the last busy block in pcb's array  or -1 if fails
       
*/
int32_t open (const uint8_t* fname){
    int32_t fd;
    int32_t return_value;
    file_array_t* file_array;
    dentry_t dentry;
    int8_t filename[32];
    
    //get the current pcb
    pcb_t* pcb_ptr=get_cur_pcb_addr();
    if(pcb_ptr==NULL){                          //check the null ptr
        printf("PCB Error: Null pointer detected\n");
        return -1;
    }

    //check the empty filename
    if(fname==NULL || strlen((int8_t*)fname)==0){  //check the null ptr or the empty file name
        printf("System call open error: Empty filename\n");
        return -1;
    }
    int i=0;
    for(i=0;i<strlen(fname);i++){
        if(fname[i]=='\n'){
            filename[i]='\0';
            break;
        }
        filename[i]=fname[i];
    }
    filename[i]='\0';
    //find a FREE FD
    for(fd=FIRST_FILE_FD;fd<PCB_SIZE;fd++){
        file_array=&(pcb_ptr->file_array[fd]);  //get the file block corresponding to the fd
        if(file_array->flg==FD_BUSY){
            if(fd==PCB_SIZE-1){                 //if the last one is still busy, get failure
                printf("PCB Error: No more free fd in file_array for a new file\n");
                return -1;
            }
            continue;
        }
        else{                                   //find a block which is not busy
            break;
        }
    }

    //read the dentry by name
    return_value=read_dentry_by_name(filename,&dentry);
    if(return_value==-1){
        file_array->table_ptr=&REG_FILE_opt_table;
        file_array->file_pos=0;
        file_array->inode=-1;
        file_array->flg=FD_BUSY;
        strcpy(file_array->name,filename);
        return fd;
    }

    //process the opening based on the file_type
    switch (dentry.file_type){
        case RTC_TYPE:
            file_array->table_ptr=&RTC_opt_table;
            break;
        case DIRECTORY_TYPE:
            file_array->table_ptr=&DIR_opt_table;
            break;
        case REGULAR_FILE_TYPE:
            file_array->table_ptr=&REG_FILE_opt_table;
            break;
        default:
            printf("File type Error: Unrecognized type %d\n",dentry.file_type);
            return -1;
    }

    file_array->file_pos=0;     //Initially the file position is 0
    file_array->flg=FD_BUSY;    
    file_array->inode=dentry.inode_num;  //give the file inode index to file_array

    //open
    if(file_array->table_ptr->open(filename)!=-1){
        return fd;
    }
    return -1;
}

/*
name: close
function: close all files 
input: fd -- the file need to be close
outuput :  0 if successful , -1 if fails
*/
int32_t close (int32_t fd){
    pcb_t* pcb_ptr;
    file_array_t* file_array;
   
    //check if the numbers are valid
    if( fd<FIRST_FILE_FD || fd>=PCB_SIZE){    
        return -1;
    }

    //get the current pcb
    pcb_ptr=get_cur_pcb_addr();       
    if(pcb_ptr==NULL){
        return -1;
    }

    file_array=&(pcb_ptr->file_array[fd]); //get the block corresponding to fd in file_array
    if(file_array->flg==0){
        return -1;
    }
    //Clear the data inside the block in file_array 
    file_array->flg=!FD_BUSY;   //The file is free
    file_array->file_pos=0;     //The default value of file position is zero
    file_array->inode=0;        //The inode number is zero
    
    if(file_array->table_ptr->close(fd)!=-1){
        return 0;
    }
    return -1;
}
/*
function name : read the file according to fd and store it into buf
input : fd, buf,nbytes  .fd is index of pcb stored in file_array. buf is to store the fd's content
nbytes is the number of bytes to be read
output : the bumber of words it read. -1 if fails

*/
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    sti();
    //declaration
    pcb_t* pcb_ptr;
    int32_t return_value;
    file_array_t* file_array;
    //some checks   
    if(buf==NULL){                          //check if buf is a null pointer
        return -1;
    }          
    if( nbytes<0 || fd<0 || fd>=PCB_SIZE ||fd==1){  //check if the numbers are valid
        return -1;
    }
    
    //pcb related
    pcb_ptr=get_cur_pcb_addr();           //get the pointer to current pcb       
    if(pcb_ptr==NULL){
        return -1;
    }
    file_array=&(pcb_ptr->file_array[fd]);  //get the block corresponding to fd in file_array    
    
    if(file_array->inode==-1){
        close(fd);
        printf("File not found\n");
        return -1;
    }
    //if it is busy, we will apply the read function in its file operation table
    if(file_array->flg==FD_BUSY){  
        // if(NULL==file_array->table_ptr->read){
        //     return -1;
        // }  
        return_value=file_array->table_ptr->read(fd,buf,nbytes);
        
        return return_value;
    }
    else{
        return -1; // It is free, so we get failure.
    }
}
/*
function_name: write
input: fd--index of file in file array. buf -- the content to write into file . nbbytes-numebr of 
bytes to write 
output:the written word's number or -1 if fails
function: to write buf into file accoridng to fd
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    sti();
    //declaration
    pcb_t* pcb_ptr;
    int32_t return_value;
    file_array_t* file_array;

    //some checks   
    if(buf==NULL){                          //check if buf is a null pointer
        return -1;
    }          
    if( nbytes<0 || fd<=0 || fd>=PCB_SIZE ){  //check if the numbers are valid
        return -1;
    }
    
    //pcb related
    pcb_ptr=get_cur_pcb_addr();           //get the pointer to current pcb       
    if(pcb_ptr==NULL){
        return -1;
    }                   
    file_array=&(pcb_ptr->file_array[fd]);  //get the block corresponding to fd in file_array    
    if(file_array->inode==-1){
        file_array->inode=file_create(file_array->name);
    }
    //if it is busy, we will apply the read function in its file operation table
    if(file_array->flg==FD_BUSY){ 
        // if(NULL==file_array->table_ptr->write){
        //     return -1;
        // }
        return_value=file_array->table_ptr->write(fd,buf,nbytes);
        if(return_value!=-1){
            file_array->file_pos+=return_value;      //update the current file position
        }
        return return_value;
    }
    else{
        return -1; // It is free, so we get failure.
    }
}

/*
function_name: getargs
input: buf -- buffer to store the argument 
       nbytes -- the number of bytes to copy 
output: 0 for success
        -1 for failure
function: get the argument stored in the pcb
*/
int32_t getargs (uint8_t* buf, int32_t nbytes){
    //declaration
    pcb_t* pcb_ptr;
    char* arg;
    //invalid cases
    if(buf==NULL){//pointer is NULL
        printf("In getargs(), Null pointer detected");
        return -1;
    }
    if(nbytes<0){//invalid nbytes
        printf("In getargs(), Invalid nbytes");
    }

    //pcb_related
    pcb_ptr=get_cur_pcb_addr();//get current pcb address
    arg=pcb_ptr->arg;               //get the argument
    if(arg==NULL || strlen(arg)==0){//check if the argument is valid
        printf("In getargs(), Empty argument passed");
        return -1;
    }

    nbytes=nbytes>MAX_ARG_LEN? MAX_ARG_LEN:nbytes;       //if nbytes is bigger than the maximum length of argument, cut it off.
    strncpy((int8_t*)buf,(int8_t*)arg,(uint32_t)nbytes); //copy nbytes of the argument to buf
    return 0;
}

/*
function_name: vidmap
input: screen_start--pointer point to the virtual memory last accessed
output:return 0 -- success
       return 1 -- invalid position
function: change the pointer of screen_start
*/
int32_t vidmap (uint8_t** screen_start){
    /* check invalid virtual address pointer */
    /* should not be null, should in userspace */
    if( screen_start == NULL||  (uint32_t)screen_start < KERNAL_SPACE_BASE || (uint32_t)screen_start > USER_SPACE_BASE ) return -1;
        
    /* map virtual memory  VIR_ADDR+PAGE_4MB*5+VIDEO_MEM_BASE to VIDEO_MEM_BASE */
    /* new page table */
    page_table_vidpage[VIDEO_MEM_BASE/PG_SIZE_4KB]=( VIDEO_MEM_BASE| PRESENT | R_W |U_S);
    /* use index 37 as the entry we want to use in PD */
    page_directory[(VIR_ADDR+PAGE_4MB*5)/PAGE_4MB]=((uint32_t)page_table_vidpage|PRESENT | R_W |U_S);//virtual memory point to the start point of vedio memory table
    
    
    flush_tlb();
    
    /*set it point to the video page in virtual mem*/
    *screen_start=(uint8_t*)(VIR_ADDR+PAGE_4MB*5+VIDEO_MEM_BASE);
    return 0;    
    
}


/*
name: undefined
function: the system calls that are not impelmented yet
input : none
output : always fail, return -1     
*/

int32_t undefined(){
    return -1;
}

int32_t remove(uint8_t* filename){
    int32_t res=file_remove(filename);
    return res;
}


void send_sig_info(int signal){
    
    pcb_t* cur_pcb;
    cur_pcb=get_cur_pcb_addr();
    if (signal>5||signal<1){
        return ;
    }
    // int a=0;
    // for (int i=0;i<signal;i++){
    //     a=a<<signal;
    // }
    // if(a&cur_pcb->block==1){
    //     return ; //block it and return
    // }
    else{
    int i=0;
    while(cur_pcb->pending_signals[i].signum!=-999){
        i=i+1;
    }
        cur_pcb->pending_signals[i].signum=signal;
      
       return ;
    }

}
void kill(int signum){
    halt(255);
    return;
}
int check_pengding(void){
    pcb_t* cur_pcb;
    cur_pcb=get_cur_pcb_addr();
    if(cur_pcb->pending_signals[0].signum==-999){
        return 0;
    }
    else{
        return 1;
    }
}
int32_t set_handler(int32_t signum, void* handler_address){
      pcb_t* cur_pcb;
      cur_pcb=get_cur_pcb_addr();
      if(handler_address==NULL){
          if(signum==0||signum==1||signum==2){
              cur_pcb->func=&kill;
              (*cur_pcb->func)(signum);
          }
          return -1;
      }
      else{
          cur_pcb->func=handler_address;
          (*cur_pcb->func)(signum);
          return -1;
      }

}

int32_t sigreturn(void){
//    asm volatile(
//         "movl $1, %%eax;"

//         :
//         : 
//         : "%eax"
//     );
    return -1;
}

