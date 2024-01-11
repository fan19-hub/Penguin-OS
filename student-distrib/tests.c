#include "tests.h"
#include "terminal.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "filesys.h"
#include "systemcall.h"
#include "rtc.h"
#define PASS 1
#define FAIL 0

// int rtc_test_flag=0;
// int get_rtc_test(){
// 	return rtc_test_flag;
// }

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


// /* Checkpoint 1 tests */

// /* IDT Test - Example
//  * 
//  * Asserts that first 10 IDT entries are not NULL
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  * Coverage: Load IDT, IDT definition
//  * Files: x86_desc.h/S
//  */
// int idt_test(){
// 	TEST_HEADER;

// 	int i;
// 	int result = PASS;
// 	for (i = 0; i < 10; ++i){
// 		if ((idt[i].offset_15_00 == NULL) && 
// 			(idt[i].offset_31_16 == NULL)){
// 			assertion_failure();
// 			result = FAIL;
// 		}
// 	}

// 	return result;
// }

// /* division_exception_test
// * Inputs: None
// * Outputs: None
// * Side Effects: Stop the program
// * Files: exception_wrap.h/exception_wrap.c
// */
int division_exception_test() {
    TEST_HEADER;
    int b = 0;
    int a = 6; // 6 is a random number 
    a = a / b;
    return FAIL;
}
// /* system call test
// * Inputs: None
// * Outputs: None
// * Side Effects: print the information that there is a system call
// * Files: exception_wrap.h/exception_wrap.c
// */

// int syscall_test() {
//     TEST_HEADER;
//     __asm__("int    $0x80"); //0x80 is the system call
//     return PASS; 
// }

// /* pointer Test 
//  * Inputs: none
//  * Outputs: none
//  * Side Effects: This test will dereference NULL, the page fault is expected
//  * Files: exception_wrap.h/exception_wrap.c
//  */
// int pointer_null_test(){
// 	TEST_HEADER;
// 	int* p = NULL;
// 	int val;
// 	val = *(p);
// 	return PASS;	
// }

// /* 
//  * bound_out
//  * INPUTS: none
//  * OUTPUTS: none
//  * SIDE EFFECTS: test the bound
//  */
// int bound_out() {
// 	TEST_HEADER;
// 	asm (
// 		"movl $2, %ebx;"
// 		"bound %ebx, LABEL;"
// 		"LABEL:;"
// 		".long 3;"
// 	);
// 	// 2, 3 are random numbers
// 	return PASS;
// }



// /* rtc_test
//  * Inputs: None
//  * Outputs: None
//  * Side Effects: Change the flag indicating whether rtc test is launched.
//  * Files: rtc.h/rtc.c
//  */
// void rtc_test(){
// 	rtc_test_flag=1;
// }



// /* Paging Test valid dereference: valid dereference test
//  * Inputs: None
//  * Outputs: PASS or OS crash
//  * Side Effects: The OS will crash if my paging has problems
//  * Files: paging.h/paging.c
//  */
// int paging_test_valid_dereference(){
// 	TEST_HEADER;
// 	int result = PASS;
// 	uint32_t dereference,check_pos;
// 	check_pos = VIDEO_MEM_BASE;
// 	dereference = *((uint32_t *)check_pos);
// 	check_pos = KERNEL_PAGE;
// 	dereference = *((uint32_t *)check_pos);
	
// 	return result;
// }

// /* Paging Test invalid dereference: invalid and valid dereference test
//  * Inputs: None
//  * Outputs: OS crash or Fail
//  * Side Effects: The OS will crash if my paging works well
//  * Files: paging.h/paging.c
//  */

// int paging_test_invalid_dereference(){
// 	TEST_HEADER;
// 	int result = FAIL;
// 	uint32_t dereference,check_pos;
// 	check_pos = VIDEO_MEM_BASE + PG_SIZE_4KB;
// 	dereference = *((uint32_t *)check_pos);
// 	check_pos = KERNEL_MEM + PG_SIZE_4MB;
// 	dereference = *((uint32_t *)check_pos);
// 	return result;
// }


// /* Paging Test value: test value in page directory and page table entry
//  *
//  * Check values in Paging
//  * Inputs: None
//  * Outputs: PASS or FAIL
//  * Side Effects: None
//  * Files: paging.h/paging.c
//  */
// int paging_test_value(){
// 	TEST_HEADER;
// 	int result = PASS;
// 	uint32_t temp_1, temp_2;
// 	if (page_directory[0] != (((uint32_t)page_table_vid) | R_W | PRESENT | ACCESS)){
// 		temp_1 = ((uint32_t)page_table_vid) | R_W | PRESENT| ACCESS ;
// 		printf("page_directory[0] = %x",page_directory[0]);
// 		printf("test_1 = %x",temp_1);
// 		printf("PDE 0 wrong\n");
// 		result = FAIL;
// 	}

// 	if (page_directory[1] != ((KERNEL_PAGE | PRESENT | PS |  GLOBAL | ACCESS | DIRTY | R_W ) & SUPERVISOR )){
// 		temp_2 =( KERNEL_PAGE | PRESENT | PS |  GLOBAL | ACCESS | DIRTY | R_W ) & SUPERVISOR ;
// 		printf("page_directory[1] = %x",page_directory[1]);
// 		printf("test_2 = %x",temp_2);
// 		printf("PDE 1 wrong\n");
// 		result = FAIL;
// 	}
// 	if (page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB] != (page_table_vid[VIDEO_MEM_BASE/PG_SIZE_4KB] | R_W | PRESENT | ACCESS ) ){
// 		printf("PTE wrong\n");
// 		result = FAIL;
// 	}
// 	return result;
// }

// // add more tests here

// /* Checkpoint 2 tests */
// /* Checkpoint 2 tests */
// /*rtc test*/
// /* rtc_read_write_test
// * Test different RTC frequencies
// * Inputs: None
// * Outputs: None
// * Side Effects: Opens the RTC and writes many different frequencies to it.
// * Files: rtc.c
// */
// int rtc_read_write_test() {

// 	TEST_HEADER;
//     uint32_t u;
//     uint32_t j;
//     int32_t val = 0;
// 	clear();
//     val += rtc_open(NULL);
//     for(u = 2; u <= 1024; u*=2) {
//         val += rtc_write( &u, sizeof(uint32_t),NULL);
//         printf("Testing: %d Hz\n", u);
//         for(j = 0; j < u; j++) {
//             val += rtc_read(NULL, NULL, NULL);
//             printf("1");
//         }
//         printf("\n");
// 		clear();
//     }
	
//     if(val == 1) {
//         return FAIL;
//     } else {
//         return PASS;
//     }


// }
// /* terminal test */
// /*terminal_read_test: terminal_read
//  *
//  * Check terminal_read
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  * Coverage: terminal_driver
//  * Files: terminal_driver.h; terminal_driver.c
//  */
// int terminal_read_test(){
// 	TEST_HEADER;
// 	int i,n;
// 	int result = PASS;
// 	int testsize = 10;
// 	//printf("The size of the terminal buffer you set is: %d\n", testsize);
// 	char buf[testsize];

// 	while(1){
// 		/* initialize the content */
// 		memset(buf,0,testsize);
		
// 		//printf("Please enter the value in keyboard buffer:\n");
// 		/* zero means do not need here */
// 		n = terminal_read(0,buf,testsize);
// 		//printf("The value in terminal buffer is: \n");
// 		for ( i = 0; i < n; i++)
// 		{
// 			putc_modf(buf[i]);
// 		}
// 		if (buf[n-1]!='\n'){
// 			putc_modf('\n');
// 		}
// 		//printf("%s\n", buf);
// 		//putc_modf('\t');
// 	}

// 	return result;
// }

// /* terminal_write_test: terminal_write
//  *
//  * Check terminal_write
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  * Coverage: terminal_driver
//  * Files: terminal_driver.h; terminal_driver.c
//  */
// int terminal_write_test(){
// 	TEST_HEADER;
// 	int result = PASS;
// 	int n;
// 	int testsize = 10;
// 	char buf[testsize];
// 	while(1){
// 		memset(buf,0,testsize);
// 		n = terminal_read(0,buf,testsize);
// 		terminal_write(0,buf,n);
// 	}


// 	return result;
// }
// /*
// NULL_POINTER
// DESCRIPTION: test the case if the NULL pointer of DENTRY is sent to read_dentry_by_name
// INPUT:none
// OUTPUT:none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int NULL_POINTER(){
// 	int result;
// 	dentry_t *dentry=NULL;//make pointer of dentry to NULL
// 	result=read_dentry_by_name("frame1.txt",dentry);
// 	if(result==-1){
// 		return PASS;
// 	}
// 	return FAIL;
// }
// /*
// TOO_LONG_NAME
// DESCRIPTION: test the case if the name of file is too long
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int TOO_LONG_NAME(){
// 	uint32_t result;
// 	dentry_t dentry;
// 	result=read_dentry_by_name("verylargetextwithverylongname.txt",&dentry);//very long name file
// 	if(result==-1){
// 		return PASS;
// 	}
// 	return FAIL;
// }
// /*
// DENTRY_INDEX_OUT
// DESCRIPTION: test the case if the index of dentry is out of range. Test read_dentry_by_index function
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int DENTRY_INDEX_OUT(){
// 	uint32_t result;
// 	dentry_t dentry;
// 	result=read_dentry_by_index(17,&dentry);//17 is just a random number bigger than the number of dentry in boot vlock
// 	if(result==-1){
// 		return PASS;
// 	}
// 	return FAIL;
// }

// /*
// VALID_FILE_OPEN
// DESCRIPTION: test the case if a correct file name is sent to file_open
// INPUT:none
// OUTPUT: print the inode number written from file_open
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_FILE_OPEN(){
// 	int result;
// 	uint32_t fd;//store inode number of file in file_open
// 	result=file_open("frame1.txt", &fd);
// 	if(result==-1){
// 		return FAIL;
// 	}
// 	printf("The inode number is %d\n",fd);//print inode number
// 	return PASS;
// }
// /*
// INVALID_FILE_OPEN
// DESCRIPTION: test the case if the NULL pointer of fd is sent to file_open
// INPUT:none
// OUTPUT: print some addition information about the failure
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int INVALID_FILE_OPEN(){
// 	int result;
// 	result=file_open("frame0.txt",NULL);//if &fd=NULL
// 	if(result==-1){
// 		return PASS;
// 	}
// 	return FAIL;
// }
// /*
// FILE_CLOSE_TEST
// DESCRIPTION: test the function of file_close
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int FILE_CLOSE_TEST(){
// 	int32_t result;
// 	uint32_t fd; 
// 	result=file_open("frame0.txt",&fd);
// 	if(result==-1){
// 		return FAIL;
// 	}
// 	result=file_close(&fd);
// 	if(result==0){
// 		return PASS;
// 	}return FAIL;
// }
// /*
// INVALID_OFFSET_READ_FILE
// DESCRIPTION: test the case if the offset of file is to large. Test file_read function
// INPUT:none
// OUTPUT: print some addition information about the failure
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int INVALID_OFFSET_READ_FILE(){
// 	int result;
// 	uint32_t fd; 
// 	char buf[200];//200 is just the number bigger than the size of frame0 and smaller than the block size
// 	result=file_open("frame0.txt",&fd);
// 	if(result==-1){
// 		printf("file_open fails\n");
// 		return FAIL;
// 	}
// 	result=file_read(&fd,50000,buf,200);//50000 is just a huge number to make sure offset is out of range
// 	                                   //200 is also a random number smaller than or equal to buffer size
// 	file_close(&fd);
// 	if(result==0){
// 		return PASS;
// 	}

// 	return FAIL;
// }

// /*
// VALID_FILE_READ
// DESCRIPTION: test the case if the correct file name is sent. Test the whole process of file read, including file_open, file_read and file_close.
// INPUT:none
// OUTPUT: print some addition information about the failure. Print the content of the file.
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// // int VALID_FILE_READ(){
// // 	// *a="grep";
// // 	int result;
// // 	uint32_t fd; 
// // 	char buf[200];//200 is just the number bigger than the size of frame0 and smaller than the block size
// // 	result=file_open("frame0.txt",&fd);
// // 	if(result==-1){
// // 		printf("file_open fails\n");
// // 		return FAIL;
// // 	}
// // 	result=file_read(&fd,0,buf,200);//200 is also a random number smaller than or equal to buffer size and bigger than size of frame0.txt
// // 	if (result!=187){//187 is the number of char in frame0.txt
// // 		printf("read file error\n");
// // 		return FAIL;
// // 	}
// // 	buf[result] = '\0';//end buf
// // 	result=file_close(&fd);
	
// // 	printf(buf);
// // 	printf("\n");
// // 	printf("file name: frame0.txt\n");
// // 	return PASS;
// // }

// // int VALID_FILE_READ(){
// // 	int result;
// // 	uint32_t fd; 
// // 	char buf[5349];//5349 is the size of ls
// // 	result=file_open("ls",&fd);
// // 	if(result==-1){
// // 		printf("file_open fails\n");
// // 		return FAIL;
// // 	}
// // 	result=file_read(&fd,0,buf,5349);//5300 is to show the data at the end
// // 	buf[result] = '\0';//end buf
// // 	int i;
// // 	for(i=0;i<5349;i++){//49 is from 5300 to 5349
// // 		putc_modf(buf[i]);
// // 	}
// // 	printf("\n");
// // 	printf("file name: ls\n");
// // 	return PASS;
// // }

// int VALID_FILE_READ(){
// 	int result;
// 	uint32_t fd; 
// 	char buf[100000];//200 is just the number bigger than the size of frame0 and smaller than the block size
// 	result=file_open("verylargetextwithverylongname.tx",&fd);
// 	if(result==-1){
// 		printf("file_open fails\n");
// 		return FAIL;
// 	}
// 	result=file_read(&fd,0,buf,100000);//200 is also a random number smaller than or equal to buffer size and bigger than size of frame0.txt
// 	buf[result] = '\0';//end buf
// 	result=file_close(&fd);
// 	int i;
// 	i=0;
// 	while(buf[i]!='\0'){
// 		putc_modf(buf[i++]);
// 	}
// 	printf(buf);
// 	printf("\n");
// 	printf("file name: verylargetextwithverylongname.tx\n");
// 	return PASS;
// }



// /*
// VALID_FILE_WRITE
// DESCRIPTION: test the function of file_write
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_FILE_WRITE(){
//    int result;
//    uint32_t fd;
//    char buf[200]; //200 is just the number bigger than the size of frame0 and smaller than the block size
//    result=file_open("frame0.txt",&fd);
//    if(result==-1){
// 	   return FAIL;
//    }
//    result=file_write(&fd,0, buf, 200);//200 is just a random number smaller than or equal to buf size
//    if(result==-1){
// 	   return PASS;
//    }return FAIL;

// }
// /*
// VALID_DIR_OPEN
// DESCRIPTION: test the case that the correct file name is sent to dir_open, the dir_open can work correctly
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_DIR_OPEN(){
// 	uint32_t fd; 
//     int32_t result;
// 	result=dir_open(&fd,"frame0.txt");
// 	if(result==-1){
// 		return FAIL;
// 	}
// 	return PASS;
// }/*
// VALID_DIR_CLOSE
// DESCRIPTION: test if dir can close correctly
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_DIR_CLOSE(){
// 	uint32_t fd; 
//     int32_t result;
// 	result=dir_open(&fd,"frame0.txt");
// 	if(result==-1){
// 		return FAIL;
// 	}
// 	//close directory
// 	result=dir_close(&fd);
// 	if(result==0){
// 	return PASS;}
// 	return FAIL;
// }
// /*
// VALID_DIRECTORY_READ
// DESCRIPTION: test if dir_read can work correctly
// INPUT:none
// OUTPUT: file names in directory
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_DIRECTORY_READ(){
// 	char buf[32];//32 is the max size of filename
// 	int32_t result;
// 	uint32_t fd; 
// 	fd=0;
// 	uint32_t n_dentry;
// 	n_dentry=get_Num_Dentry();//get the number of dentries
// 	int i;
// 	for(i = 0; i < n_dentry; i++) {//write all the files in the directory
// 		result = dir_read(&fd, buf, 32);//32 is the max size of file name
// 		if(result < 0||result>32){// 32 is the max size of file name
// 			return FAIL;
// 			}
// 		if(result == 0){
// 			break;
// 			}
		
// 		// printf(buf);
// 		// printf("\n");
// 	}
// 	return PASS;
// }
// /*
// VALID_DIRECTORY_WRITE
// DESCRIPTION: test if dir_write can work correctly
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int VALID_DIRECTORY_WRITE(){
//    int result;
//    uint32_t fd;
//    char buf[200]; //200 is just the number bigger than the size of frame0 and smaller than the block size
//    result=dir_open(&fd,"frame0.txt");
//    if(result==-1){
// 	   return FAIL;
//    }
//    result=dir_write(&fd,buf,200);//200 is just a random number smaller than or equal to buf size
//    if(result==-1){
// 	   return PASS;
//    }return FAIL;

// }
// /*
// INVALID_DIRECTORY_READ
// DESCRIPTION: test if the index of dentry out of range, will dir_read fail.
// INPUT:none
// OUTPUT: none
// RETURN: FAIL means the test fails. PASS means the test passes
// */
// int INVALID_DIRECTORY_READ(){
// 	char buf[32];//32 is the max size of filename
// 	int32_t result;
// 	uint32_t fd; 
// 	fd=0;
// 	uint32_t n_dentry;
// 	n_dentry=get_Num_Dentry();
// 	int i;
// 	for(i = 0; i < n_dentry+1; i++) {//make i out of range
// 		result = dir_read(&fd, buf, 32);//32 is the max size of file name
// 		if(result<0){
// 			return PASS;
// 		}
// 		if(result>32){
// 			return FAIL;
// 			}
// 		if(result == 0){
// 			break;
// 			}
// 	}
// 	return FAIL;
// }
/* Checkpoint 3 tests */
int write_syscall(){
	
    // int fail = 0;
	uint8_t buf[32];
	
	if (-1 != write(0, buf, 31)) {
			return FAIL;
    }return PASS;
	
}

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	clear();
	/* cp1 */
	//TEST_OUTPUT("idt_test", idt_test());
	

	/* launch your tests here */
	/* exception test */
	// TEST_OUTPUT("out of bound test",bound_out());
	// TEST_OUTPUT("division_exception_test",division_exception_test());
	//TEST_OUTPUT("syscall_test",syscall_test());
	// TEST_OUTPUT("null pointer test",pointer_null_test());
	// TEST_OUTPUT("paging_test_valid_dereference",paging_test_invalid_dereference);
	/* rtc test */
	//rtc_test(); // Use this test will fill the screen (which is correct), which will influence other testcase result.
	
	/* page tests */
	//TEST_OUTPUT("paging_test_invalid_dereference", paging_test_invalid_dereference());// Use paging_test_invalid_dereference will cause the OS crash (which is correct), which will influence other testcase result.
	//TEST_OUTPUT("paging_test_valid_dereference", paging_test_valid_dereference());
	//TEST_OUTPUT("paging_test_value", paging_test_value());

	/* cp2 */
	/* terminal test */
	//TEST_OUTPUT("terminal_read_test",terminal_read_test());
	//TEST_OUTPUT("terminal_write_test",terminal_write_test());
   
    /*filesystem test*/
	// TEST_OUTPUT("NULL_POINTER_test",NULL_POINTER());
	// TEST_OUTPUT("TOO_LONG_NAME_test",TOO_LONG_NAME());
	// TEST_OUTPUT("DENTRY_INDEX_OUT_test",DENTRY_INDEX_OUT());
	// TEST_OUTPUT("VALID_FILE_OPEN_test",VALID_FILE_OPEN());
	// TEST_OUTPUT("INVALID_FILE_OPEN_test",INVALID_FILE_OPEN());
    // TEST_OUTPUT("FILE_CLOSE_TEST",FILE_CLOSE_TEST());
    // TEST_OUTPUT("INVALID_OFFSET_READ_FILE_test",INVALID_OFFSET_READ_FILE());
	// TEST_OUTPUT("VALID_FILE_READ_test",VALID_FILE_READ());
    // TEST_OUTPUT("VALID_FILE_WRITE_test",VALID_FILE_WRITE());
    // TEST_OUTPUT("VALID_DIR_OPEN_test",VALID_DIR_OPEN());
	// TEST_OUTPUT("VALID_DIR_CLOSE_test",VALID_DIR_CLOSE());
    // TEST_OUTPUT("VALID_DIRECTORY_READ_test" ,VALID_DIRECTORY_READ());
	// TEST_OUTPUT("INVALID_DIRECTORY_READ_test",INVALID_DIRECTORY_READ());
	// TEST_OUTPUT("VALID_DIRECTORY_WRITE_test",VALID_DIRECTORY_WRITE());
	/*rtc test*/
	// TEST_OUTPUT("rtc_read_write_test",rtc_read_write_test());

	// TEST_OUTPUT("WRITE",write_syscall());
}
