/***************************************************************************************/
// FILE NAME:   ipu.h
// VERSION:     v2.0
// DESCRIPTION: IPU device driver head file.
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.05.05
/***************************************************************************************/
#ifndef _IPU_H
#define _IPU_H

#include <asm/io.h>

/***************************************************************************************/
/*                              MICRO DEFINITION                                       */
/***************************************************************************************/

/* Some Helpful MICROS */
#define IPU_NAME                        "ipu"
#define SUCCESS                         0
// IPU_BASE[31:22] = 0;
// IPU_BASE[21:20] = 2'b00 => Configuration Register
// IPU_BASE[21:20] = 2'b01 => Instruction RAM  IPU_BASE[19:15] = 0, SIZE = 32KB
// IPU_BASE[21:20] = 2'b10 => CT DATA RAM   SIZE = 1MB   
// IPU_BASE[21:20] = 2'b11 => LT DATA RAM   SIZE = 1MB
#define IPU_BASE_ADDRESS                0x43000000 

/* Configuraton Registers INFO */
#define CONFIG_REG_BASE_ADDRESS         IPU_BASE_ADDRESS + 0x00000000
#define CONFIG_REG_NUM                  2
#define CONFIG_REG_WIDTH                4 

/* Configuration Registers Operations: ioctl  */
#define MAGIC_NUM                       100
//#define MAGIC_RDCONFIG_BYTE             MAGIC_NUM + 1
//#define MAGIC_RDCONFIG_WORD             MAGIC_NUM + 2
//#define MAGIC_RDCONFIG_DWORD            MAGIC_NUM + 3
//#define MAGIC_WTCONFIG_BYTE             MAGIC_NUM + 4
//#define MAGIC_WTCONFIG_WORD             MAGIC_NUM + 5
//#define MAGIC_WTCONFIG_DWORD            MAGIC_NUM + 6



#define RDCONFIG_BYTE                   _IOR(MAGIC_NUM, 1, unsigned int) 
#define RDCONFIG_WORD                   _IOR(MAGIC_NUM, 2, unsigned int) 
#define RDCONFIG_DWORD                  _IOR(MAGIC_NUM, 3, unsigned int) 
#define WTCONFIG_BYTE                   _IOW(MAGIC_NUM, 4, unsigned int*) 
#define WTCONFIG_WORD                   _IOW(MAGIC_NUM, 5, unsigned int*) 
#define WTCONFIG_DWORD                  _IOW(MAGIC_NUM, 6, unsigned int*) 
#define START                           _IO(MAGIC_NUM, 7)
#define RESET                           _IO(MAGIC_NUM, 8)

//#define RDCONFIG_BYTE                   _IOWR('y', MAGIC_RDCONFIG_BYTE, 0x0f) 
//#define RDCONFIG_WORD                   _IOWR('y', MAGIC_RDCONFIG_WORD, 0x0f) 
//#define RDCONFIG_DWORD                  _IOWR('y', MAGIC_RDCONFIG_DWORD, 0x0f) 
//#define WTCONFIG_BYTE                   _IOWR('y', MAGIC_WTCONFIG_BYTE, 0x0f) 
//#define WTCONFIG_WORD                   _IOWR('y', MAGIC_WTCONFIG_WORD, 0x0f) 
//#define WTCONFIG_DWORD                  _IOWR('y', MAGIC_WTCONFIG_DWORD, 0x0f) 

/* Instruction RAM Info */
#define INST_RAM_BASE_ADDRESS           IPU_BASE_ADDRESS + 0x00100000
#define INST_RAM_SIZE                   (32 * 1024)

/* CT DATA RAM Info */
#define CTDATA_RAM_BASE_ADDRESS         IPU_BASE_ADDRESS + 0x00200000
#define CTDATA_RAM_SIZE                 (1 * 1024 * 1024)

/* LT DATA RAM Info */
#define LTDATA_RAM_BASE_ADDRESS         IPU_BASE_ADDRESS + 0x00300000
#define LTDATA_RAM_SIZE                 (1 * 1024 * 1024)

/* Reserved Memory Info*/
#define DMA_BUFFER_START                0x1e000000
#define DMA_BUFFER_SIZE                 (32 * 1024 * 1024)

#endif 
