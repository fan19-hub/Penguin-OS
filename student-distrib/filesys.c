#include "systemcall.h"
#include "filesys.h"

// Global Variables
uint32_t Num_Dentry;//the number of dentries
uint32_t Num_Inodes;// the number of inodes
uint32_t Num_Data_Blocks;//number of data blocks
uint32_t File_Position=0;//the first index of dentry
// Free list (including the indexes of data block)
uint32_t Free_list[MAX_NUM_DATA_BLK*MAX_NUM_DENTRY];
uint32_t Free_inode_list[MAX_NUM_DENTRY];
int Free_list_len=0;
int Free_inode_list_len=0;
/*
get_Num_Dentry
Description: get the number of dentries
input: none
output: none
return: Num_Dentry --the number of dentries
*/
uint32_t get_Num_Dentry(){
    return Num_Dentry;
}

/*
init_filesys
Description: define the position of pointers 
            including inode pointer and data block pointer. 
            Also store the number of inodes,dentries and data blks
input: boot_blk_ptr -- the start of boot block
output: none
return: none
*/
void init_filesys(boot_blk_t* boot_blk_ptr){
    //store the number of inodes,dentries and data blks in global variables
    Num_Dentry=boot_blk_ptr->n_dentry;
    Num_Inodes=boot_blk_ptr->n_inode;
    Num_Data_Blocks=boot_blk_ptr->n_data_blk;
    dentry_array=boot_blk_ptr->dentries;
    // define the position of pointers 
            //including inode pointer and data block pointer. 
    inode_ptr=(inode_t*)(boot_blk_ptr+1); 
    data_block_ptr=(uint8_t*)(inode_ptr+Num_Inodes); 
    free_list_init();
}

/*
read_dentry_by_name
Description: find the dentry with the same file name as the input. Then store the information of the dentry 
            including inode pointer and data block pointer. 
input: fname --file name
       dentry --a place to store the information in the dentry in boot block
output: none
return: dentry index if succeeds; -1 if fails
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int index;
    
    // Check invalid cases
    if(fname==NULL || dentry==NULL){
        printf("Null pointer detected\n");
        return -1;
    }  
    
    if(strlen((int8_t*)fname)>MAX_NAME_LEN){
        printf("Name of the file to read is too long\n");
        return -1;
    }
    
    for(index=0;index<Num_Dentry;index++){
        if(strncmp((int8_t*)fname,(int8_t*)(dentry_array[index].file_name),MAX_NAME_LEN)==0){ 
        // returning 0 means the matching of two strings
        // only the first 32 characters matter
            read_dentry_by_index(index,dentry);            
            return index;
        }
    }
    return -1;

}

/*
read_dentry_by_index
Description: store the information of the index th dentry
            including inode pointer and data block pointer. 
input: index --the index of dentry
       dentry --a place to store the information in the dentry in boot block
output: none
return: 0 if succeeds; -1 if fails
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    
    // Check invalid cases
    if(index>get_Num_Dentry()-1){//index is unsigned int, no need to check negative value
        printf("Index of dentry out of range\n");
        return -1;
    }
    if(dentry==NULL){
        printf("Invalid dentry pointer provided\n");
        return -1;
    }
    
    // Fill the values into the dentry provided
    strncpy(dentry->file_name,dentry_array[index].file_name,MAX_NAME_LEN); //strncpy can specified the length to copy
    dentry->file_type=dentry_array[index].file_type;
    dentry->inode_num=dentry_array[index].inode_num;
    return 0;
}

/*
read_data
Description: read the file and store the data read to buf 
input: inode --the number of inode
       offset --the offset of file read
       buf    --place to store the content of file
       length -- the file length to read
output: none
return: the reading length; -1 if fails
*/
int32_t read_data(uint32_t inode, uint32_t offset, char* buf, uint32_t length){
    //check invalid conditions
    if(buf==0){//null pointer error
        printf("Null pointer error\n");
        return -1;
    }
    inode_t* cur_inode=(inode_t*)inode_ptr+inode;
    if(inode>Num_Inodes ||inode<0){
        printf("inode number not in the range\n");
        return -1; //if inode is not in the range
    }
    if(offset>cur_inode->length){
        printf("offset out of range\n");
        return 0; //0 word is read
    }
    int data_blk_inode=offset/BLK_SIZE;//the index of data blk index in inode structure
    int data_blk_index=cur_inode->data_block_idx[data_blk_inode];//the index of blk
    int offset_inblk=offset%BLK_SIZE;//the offset_inblk th word in current data blk
    int cnt_word=0;
    int i;
    //read the file and write the buf
    for(i=0;i<length;i++,buf++,offset_inblk++){
        if((offset+i)>=cur_inode->length){//out of range
            return cnt_word;
        }
        if(offset_inblk==BLK_SIZE){//if a data blk is completed
            data_blk_inode+=1;//goto next data block
            if(data_blk_inode>=MAX_NUM_DATA_BLK){//if out of range
            return cnt_word;
        }
            data_blk_index=cur_inode->data_block_idx[data_blk_inode];
            offset_inblk=0;
        }
        memcpy(buf, data_block_ptr+BLK_SIZE*data_blk_index+offset_inblk,1);//copy word in file to buf
        cnt_word++;//count the word read
    }return cnt_word;
}
/*
write_data
Description: write the file from the data in buf 
input: inode --the number of inode
       offset --the offset of file read
       buf    --place storing content of file
       length -- the file length to read
output: none
return: the wrting length; -1 if fails
*/
int32_t write_data(uint32_t inode, uint32_t offset, char* buf, uint32_t length){
    int i,j;
    int cnt_bytes=0;
    int data_blk_num;
    int cur_data_blk_index;
    data_blk_t* cur_data_blk;
    
    //check invalid conditions
    if(buf==0){//null pointer error
        printf("Null pointer error\n");
        return -1;
    }
    inode_t* cur_inode=(inode_t*)inode_ptr+inode;
    if(inode>Num_Inodes ||inode<0){
        printf("inode number not in the range\n");
        return -1; //if inode is not in the range
    }
    if(offset>cur_inode->length){
        printf("offset out of range\n");
        return 0; //0 word is read
    }
    
    //get the length, update the length item in inode
    cur_inode->length=length;
    
    //the offset
    int offset_inblk=offset%BLK_SIZE;               //the offset inside the first block to write
    int start_blk=ceil((double)(offset)/BLK_SIZE);  //the first data block to write
    
    //calc the number of data blocks needed
    data_blk_num=ceil((double)(cur_inode->length)/BLK_SIZE);

    //loop through all the data blocks listed in this inode 
    for(i=start_blk;i<data_blk_num;i++){
        cur_data_blk_index=get_free_datablk_index();
        // -1 for no more free block
        if(cur_data_blk_index!=-1){
            //add this data block index to the inode
            cur_inode->data_block_idx[i]=cur_data_blk_index;
            //get the data block pointer
            cur_data_blk=(data_blk_t*)data_block_ptr+cur_data_blk_index;
            //write the data
            int start=i==start_blk? offset_inblk:0;
            for(j=start;j<BLK_SIZE;j++,cnt_bytes++){
                if(cnt_bytes==length){              //if we are done, break
                    return cnt_bytes;
                }
                cur_data_blk->data[j]=buf[cnt_bytes]; //write one byte
            }
        }
        else{ //no more free data block
            printf("No enough space to write, %d Bytes were written\n",cnt_bytes);
            return cnt_bytes;
        }
    }
    return cnt_bytes;
}

/*
free_list_init
Description: define the position of pointers 
            including inode pointer and data block pointer. 
            Also store the number of inodes,dentries and data blks
input: boot_blk_ptr -- the start of boot block
output: none
return: none
*/
void free_list_init(){
    int i;
    int j;
    int k=0;
    int x=0;
    int data_blk_num;
    uint32_t busy_list[MAX_NUM_DATA_BLK*Num_Dentry];
    //start from the first inode (inode_ptr)
    inode_t* cur_inode=(inode_t*)inode_ptr;

    //loop through all the inodes, to find the busy data blocks
    for(i=0;i<Num_Inodes;i++,cur_inode++){  //cur_inode++ here
        if(cur_inode->length==0){           //check if the inode is occupied
            Free_inode_list[x++]=i;
            continue;
        }
        data_blk_num=ceil((double)(cur_inode->length)/BLK_SIZE);    //loop through all the data blocks listed in this inode 
        for(j=0;j<data_blk_num;j++,k++){             
            busy_list[k]=cur_inode->data_block_idx[j];
        }
    }
    int busy_list_len=k;
    k=0;

    //get the compliment set: free list
    for(i=0;i<Num_Data_Blocks;i++){
        //check for busy by looping through busy list
        int busy=0;
        for(j=0;j<busy_list_len;j++){
            if(i==busy_list[j]){
                busy=1;
                break;
            }
        }
        
        //if not busy, add it into the free list
        if(!busy){
            Free_list[k]=i;
            k++;
        }
    }
    Free_list_len=k;
    Free_inode_list_len=x;
    return;
}

/*
data_blk_t* get_free_data_blk()
Description: get the free data block and update the free list 
input: none
output: update the free list
return: The last free block, return -1 for fail
*/

int32_t get_free_datablk_index(){
    Free_list_len--;                    //pop the last data block
    if(Free_list_len<0){
        return -1;
    }
    int32_t index=(int32_t)(Free_list[Free_list_len]); //index of the last data block
    // data_blk_t* new_ptr=(data_blk_t*)data_block_ptr+index; //the pointer to 
    return index;
}

/*
int32_t get_free_inode_index()
Description: get the free inode and update the free inode list 
input: none
output: update the free inode list
return: The last free block, return -1 for fail
*/

int32_t get_free_inode_index(){
    Free_inode_list_len--;                    //pop the last data block
    if(Free_inode_list_len<0){
        return -1;
    }
    int32_t index=(int32_t)(Free_inode_list[Free_inode_list_len]); //index of the last data block
    // data_blk_t* new_ptr=(data_blk_t*)data_block_ptr+index; //the pointer to 
    return index;
}

int32_t file_create(const uint8_t* fname){
    uint32_t inode_num;
    dentry_t dentry;
    uint8_t filename[32];
    int i=0;
    for(i=0;i<strlen(fname);i++){
        if(fname[i]=='\n'){
            filename[i]='\0';
            break;
        }
        filename[i]=fname[i];
    }
    filename[i]='\0';
    //check if the dentry list is full
    if(Num_Dentry>=MAX_NUM_DENTRY){
        printf("No more dentry to hold this file\n");
        return -1;
    }
    //add the new inode num to the  dentry
    inode_num=get_free_inode_index();
    if(inode_num==-1){
        printf("No more free inode to hold this file\n");
        return -1;
    }
    dentry.inode_num=inode_num;
    //add the file name to the dentry
    uint32_t fname_length=strlen((int8_t*)filename);
    fname_length=fname_length>32?32:fname_length;
    strncpy((int8_t*)(dentry.file_name),(int8_t*)filename,fname_length);
    dentry.file_name[fname_length]='\0';
    //add the file type to the dentry
    dentry.file_type=REGULAR_FILE_TYPE;

    //add the dentry to the dentry list, update the Num_Dentry
    dentry_array[Num_Dentry++]=dentry;
    return inode_num;
}
/*
file_open
Description: get the information of the file with file name provided 
input: fname -- file name
output: none
return: 0 if succeeds; -1 if fails
*/
int32_t file_open(const uint8_t* fname){
    dentry_t dentry;
    int32_t result;
    result=read_dentry_by_name(fname,&dentry);  //get the information of  the file with file name fname
        //result==-1 Can't find it in this directory, create a new file for writing operation
    if (result==-1){
        return -1;
    }
    return 0;

}

/*
file_read
Description: read the file and store the data read to buf 
input: fd -- file descriptor, currently represents inode of the file
       offset -- the offset between the start address of file and the start address of reading
        buf    --place to store the content of file
       length -- the file length to read
output: none
return: the read length; -1 if fails
*/
int32_t file_read(int32_t fd, void* buf, int32_t length){
    //declaration
    uint32_t offset;
    int32_t bytes_read;
    pcb_t* pcb_ptr;
    uint32_t inode_num;
    file_array_t* file_array;
    
    //Null pointer check
    if(buf==0){
        return -1;
    }

    //pcb related
    pcb_ptr=get_cur_pcb_addr();           //get the pointer to current pcb       
    if(pcb_ptr==NULL){
        return -1;
    }
    file_array=&(pcb_ptr->file_array[fd]);  //get the block corresponding to fd in file_array    
    inode_num=file_array->inode;            //get the index of the dentry
    offset=file_array->file_pos;            //get the offset of the file read start address
    //read data
    bytes_read=read_data(inode_num,offset, (char*)buf, length);    //read the file and store the data read to buf 
    char* buff=(char*)buf;
    //update the pos
    file_array->file_pos+=bytes_read;      //update the current file position
    return bytes_read;
}

/*
file_write
Description: write to file 
input: fd -- file descriptor, currently represents inode of the file
       offset -- the offset between the start address of file and the start address of reading
        buf    --the content need to write to file
       length -- the file length to read
output: none
return: -1 if fails
*/
int32_t file_write(int32_t fd, const void* buf, int32_t length){
    //declaration
    uint32_t offset;
    int32_t bytes_write;
    pcb_t* pcb_ptr;
    uint32_t inode_num;
    file_array_t* file_array;
    
    //Null pointer check
    if(buf==0){
        return -1;
    }

    //pcb related
    pcb_ptr=get_cur_pcb_addr();           //get the pointer to current pcb       
    if(pcb_ptr==NULL){
        return -1;
    }
    file_array=&(pcb_ptr->file_array[fd]);  //get the block corresponding to fd in file_array    
    inode_num=file_array->inode;            //get the index of the dentry
    offset=file_array->file_pos;            //get the offset of the file read start address
    //read data
    bytes_write=write_data(inode_num,offset, (char*)buf, length);    //read the file and store the data read to buf 
    //update the pos
    file_array->file_pos+=bytes_write;      //update the current file position
    return bytes_write;
}

/*
file_close
Description: close the file
input: fd -- file descriptor, currently represents inode of the file
output: none
return: 0;
*/
int32_t file_close(int32_t fd){
    return 0;
}


/*
dir_open
Description: open directory;currently just open file of file name fname
input:  fname --file name
output: none
return: 0 if succeeds; -1 if fails
*/
int32_t dir_open(const uint8_t* fname){
    dentry_t dentry;
    int32_t result;
    //read the dentry of file and store the information
    result=read_dentry_by_name(fname,&dentry);
    return result;
}

/*
dir_read
Description: read file name in directory
input: fd -- file descriptor, currently represents inode of the file
        buf    --the place to store the file names read
       length -- the file length to read
output: none
return: the read length
*/
int32_t dir_read(int32_t fd, void* buf, int32_t length ){
    dentry_t dentry;
    int32_t result;
    int32_t l;
    uint32_t file_index;
    pcb_t* pcb_ptr;
    file_array_t* file_array;
    l=length;
    //invalid case
    if(buf==0){
        printf("NULL POINTER ERROR\n");
        return -1;
    }

    //pcb related
    pcb_ptr=get_cur_pcb_addr();           //get the pointer to current pcb       
    if(pcb_ptr==NULL){
        return -1;
    }
    file_array=&(pcb_ptr->file_array[fd]);  //get the block corresponding to fd in file_array    
    file_index=file_array->file_pos;        //get the offset of file read
    if(file_index==get_Num_Dentry()){
        return 0;
    }

    //read dentry
    result=read_dentry_by_index(file_index,&dentry);
    if(result==-1){
        return -1;
    }

    //cut the length
    if(length>MAX_NAME_LEN){//control the length of read
        l=MAX_NAME_LEN;
    }
    if(strlen(dentry.file_name)<l){//length read must be equal to file name length in dentry
        l=strlen(dentry.file_name);
    }
    
    //copy the string
    strncpy((char*)buf,dentry.file_name,l);//copy file name in dentry to buf with size<=32
    *((char*)buf+l) = '\0';

    //update pos
    file_array->file_pos+=1;      //update the current file position

    return l;
}

int32_t file_remove(uint8_t* fname ){
    dentry_t dentry;
    int index;
    if(-1==(index=read_dentry_by_name(fname,&dentry))){
        printf("File not found\n");
        return -1;
    }
    for(;index<get_Num_Dentry()-1;index++){
        dentry_array[index]=dentry_array[index+1];
    }
    Num_Dentry--;
    return 0;
}

/*
dir_write
Description: write to directory
input: fd -- file descriptor, currently represents inode of the file
        buf    --the file names to write
       length -- the file length to read
output: none
return: -1 means fail
*/
int32_t dir_write(int32_t fd, const void* buf, int32_t length) {
    return -1;
}

/*
dir_close
Description:close the directory
input: fd -- file descriptor, currently represents inode of the file
output: none
return: 0 means success
*/
int32_t dir_close(int32_t fd) {
    return 0;
}

