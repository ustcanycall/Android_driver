/***************************************************************************************/
// FILE NAME:   ipu.h
// VERSION:     v2.0
// DESCRIPTION: Head file of IPU device driver in android HAL layer.
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.05.05
/***************************************************************************************/

#ifndef _IPU_HAL
#define _IPU_HAL

#include <hardware/hardware.h>  // struct hw_module_t, struct hw_device_t, and corresponding operations

/***************************************************************************************/
/*                              MICRO DEFINITION                                       */
/***************************************************************************************/

/* Define module and device ID */
#define IPU_HARDWARE_MODULE_ID          "ipu"
#define IPU_HARDWARE_DEVICE_ID          "ipu"

#define DEVICE_NAME                     "/dev/ipu"
#define MODULE_NAME                     "Ipu"
#define MODULE_AUTHOR_NAME              "ustcanycall"

/* Configuration Registers Operations: ioctl  */
#define HAL_MAGIC_NUM                   100
#define HAL_RDCONFIG_BYTE               _IOR(HAL_MAGIC_NUM, 1, unsigned int) 
#define HAL_RDCONFIG_WORD               _IOR(HAL_MAGIC_NUM, 2, unsigned int) 
#define HAL_RDCONFIG_DWORD              _IOR(HAL_MAGIC_NUM, 3, unsigned int) 
#define HAL_WTCONFIG_BYTE               _IOW(HAL_MAGIC_NUM, 4, unsigned int*) 
#define HAL_WTCONFIG_WORD               _IOW(HAL_MAGIC_NUM, 5, unsigned int*) 
#define HAL_WTCONFIG_DWORD              _IOW(HAL_MAGIC_NUM, 6, unsigned int*) 
#define HAL_START                       _IO(HAL_MAGIC_NUM, 7)
#define HAL_RESET                       _IO(HAL_MAGIC_NUM, 8)

/***************************************************************************************/
/*                              STRUCT TYPE DECLARATION                                */
/***************************************************************************************/

/* hw_module_t should be packaged before being used */
struct ipu_module_t 
{
    struct hw_module_t common;
};

/* hw_device_t should be packaged before being used */
struct ipu_device_t
{
    struct hw_device_t common;
    int     fd;
    void    (*ipu_start)                (struct ipu_device_t * dev);
    void    (*ipu_reset)                (struct ipu_device_t * dev);
    char    (*ipu_read_config_byte)     (struct ipu_device_t * dev, int offset);
    short   (*ipu_read_config_word)     (struct ipu_device_t * dev, int offset);
    int     (*ipu_read_config_dword)    (struct ipu_device_t * dev, int offset);
    void    (*ipu_write_config_byte)    (struct ipu_device_t * dev, int offset, char val);
    void    (*ipu_write_config_word)    (struct ipu_device_t * dev, int offset, short val);
    void    (*ipu_write_config_dword)   (struct ipu_device_t * dev, int offset, int val);
    int     (*ipu_read_inst_ram)        (struct ipu_device_t * dev, int * buf, int count, int offset);
    int     (*ipu_write_inst_ram)       (struct ipu_device_t * dev, int * buf, int count, int offset);
    int     (*ipu_mmap)                 (struct ipu_device_t * dev, int buf_size);
    /* additional device operations here */
};

#endif

