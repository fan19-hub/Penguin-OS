#ifndef _FILESYS_H
#define _FILESYS_H
#include "lib.h"
#define RESERVE_52 52     //reserved size in statistic of boot block
#define RESERVE_24 24     //reserved size in directory entry of boot block
#define MAX_NUM_DENTRY 63  //the max number of dir.entries is 63
#define MAX_NAME_LEN 32 
#define MAX_NUM_DATA_BLK 1023 //the max number of data block is (2^12-4)/(2^2)=1023
#define BLK_SIZE 4096  //4kb 
#define RTC_TYPE 0
#define DIRECTORY_TYPE 1
#define REGULAR_FILE_TYPE 2


// Directory entry defination
typedef struct{
  char file_name[32]; //32B at most; file name
  uint32_t file_type;          //4B file type
  uint32_t inode_num;          //4B //the inode number
  uint8_t reserved[RESERVE_24];//24B reserved value
}dentry_t;

// Boot block defination
typedef struct{
  uint32_t n_dentry;  //4B the number of dentries
  uint32_t n_inode;    //4B the max number of inodes
  uint32_t n_data_blk;   //4B the number of data blks
  uint8_t reserved[RESERVE_52];  //52B researved value
  dentry_t dentries[MAX_NUM_DENTRY]; //64B directory entires
}boot_blk_t;

typedef struct
{
  uint32_t length;//the length of file in byte
  uint32_t data_block_idx[MAX_NUM_DATA_BLK];//data blocks of one file with this inode
}inode_t;

typedef struct 
{
  uint8_t data[BLK_SIZE];//1024=1023+1 bytes in a data block
}data_blk_t;

// Start address Pointers
inode_t* inode_ptr;  
uint8_t* data_block_ptr;
dentry_t* dentry_array;

// Functions
//initialize filesystem
void init_filesys();
//read dentry of the name provided
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
//read dentry of the index provided
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
//read data of the file
int32_t read_data (uint32_t inode, uint32_t offset, char* buf, uint32_t length);
//write data to the file
int32_t write_data (uint32_t inode, uint32_t offset, char* buf, uint32_t length);
//init the free list
void free_list_init();
//get the free data block
int32_t get_free_datablk_index();
//get the free inode block
int32_t get_free_inode_index();
int32_t file_create(const uint8_t* fname);
//open the file
int32_t file_open(const uint8_t* fname);
//read data of the file
int32_t file_read(int32_t fd, void* buf, int32_t length);
//write to file
int32_t file_write(int32_t fd, const void* buf, int32_t length);
//close the file
int32_t file_close(int32_t fd);
int32_t file_remove(uint8_t* fanme);
//open the directory
int32_t dir_open(const uint8_t* fname);
//read file name from directory 
int32_t dir_read(int32_t fd, void* buf, int32_t length);
//write to directory
int32_t dir_write(int32_t fd, const void* buf, int32_t length);
//close directory
int32_t dir_close(int32_t fd);
//get the number of dentry
uint32_t get_Num_Dentry();
#endif
