/***************************************************************************************/
// FILE NAME:   ipu.c
// VERSION:     v1.0
// DESCRIPTION: Test IPU device driver.
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.03.29
/***************************************************************************************/

#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h>      //open()

#include <unistd.h>     //close()
#include <sys/mman.h>
#include <sys/ioctl.h>

/***************************************************************************************/
/*                              MICRO DEFINITION                                       */
/***************************************************************************************/

#define IPU_DEVICE_NAME     "/dev/ipu"
#define DMA_BUFFER_SIZE     4*1024*1024

/* Configuration Registers Operations: ioctl  */
#define MAGIC_NUM           100
#define RDCONFIG_BYTE                   _IOR(MAGIC_NUM, 1, unsigned int) 
#define RDCONFIG_WORD                   _IOR(MAGIC_NUM, 2, unsigned int) 
#define RDCONFIG_DWORD                  _IOR(MAGIC_NUM, 3, unsigned int) 
#define WTCONFIG_BYTE                   _IOW(MAGIC_NUM, 4, unsigned int*) 
#define WTCONFIG_WORD                   _IOW(MAGIC_NUM, 5, unsigned int*) 
#define WTCONFIG_DWORD                  _IOW(MAGIC_NUM, 6, unsigned int*) 
#define START                           _IO(MAGIC_NUM, 7)
#define RESET                           _IO(MAGIC_NUM, 8)

/***************************************************************************************/
/*                              MAIN CODE                                              */
/***************************************************************************************/

int main(int argc, char **argv)
{
    int fd = -1;
    int i;

    fd = open(IPU_DEVICE_NAME, O_RDWR);
    if(fd == -1)
    {
        printf("Failed to open ipu device %s.\n", IPU_DEVICE_NAME); 
        return -1;
    }

    /*void *virt_base;

	if((virt_base = mmap(0, DMA_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "Error: cannot mmap dma buffer\n");
        return -1;
    }

    unsigned int *buffer = virt_base;

    for(i = 0; i < 16; i++)
    {
        buffer[i] = i;
    }

    for(i = 0; i < 16; i++)
    {
        printf("%d\n",buffer[i]);
    }

    munmap(virt_base, DMA_BUFFER_SIZE);
    */

    ioctl(fd, START);

    unsigned int ret;
    unsigned long argu;
    for (i = 0; i < 20; i++)
    {
        argu = i * 4;
        ret = ioctl(fd, RDCONFIG_DWORD, argu);
        printf("0x%x\n", ret);
    }

    unsigned long of_val[2];
    unsigned long *arg;

    unsigned int j;
    for (j = 0; j < 20; j++)
    {
        printf("###################################\n");
        for (i = 0; i < 5; i++)
        {
            of_val[0] = j * 4;
            of_val[1] = i;
            arg = of_val;
            ioctl(fd, WTCONFIG_DWORD, arg);

            ret = ioctl(fd, RDCONFIG_DWORD, j*4);
            printf("0x%x\n", ret);
    }
    }

    close(fd);
    return 0;
}




