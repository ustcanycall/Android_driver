/***************************************************************************************/
// FILE NAME:   ipu.cpp
// VERSION:     v1.0
// DESCRIPTION: IPU device driver in android HAL layer.
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.05.05
/***************************************************************************************/
#include <hardware/hardware.h>  // struct hw_module_t, struct hw_device_t, and corresponding operations
#include <fcntl.h>              // open()
#include <errno.h>              // all kinds of error code 
#include <sys/ioctl.h>          // ioctl()
#include <unistd.h>             // close()
#include <cutils/log.h>         // ALOGE() ALOGI()
#include <cutils/atomic.h>      // not used??
#include <sys/mman.h>           // mmap()

#include <hardware/ipu.h>       // micro definition and struct type definition

/***************************************************************************************/
/*                              MICRO DEFINITION                                       */
/***************************************************************************************/

// refer to <hardware/ipu.h>

/***************************************************************************************/
/*                              STRUCT TYPE DECLARATION                                */
/***************************************************************************************/

// refer to <hardware/ipu.h>

/***************************************************************************************/
/*                              FUNCTION DECLARATION                                   */
/***************************************************************************************/

/* openning device method for struct hw_module_methods_t */
static int ipu_device_open(
            const struct hw_module_t * module,
            const char * id,
            struct hw_device_t ** device
        );

/* closing device method for struct hw_device_t */
static int ipu_device_close(
            struct hw_device_t * device
        );

/* ipu device operations */
static void ipu_start(
            struct ipu_device_t * dev
        );

static void ipu_reset(
            struct ipu_device_t * dev
        );

static char ipu_read_config_byte(
            struct ipu_device_t * dev,
            int offset
        );

static short ipu_read_config_word(
            struct ipu_device_t * dev,
            int offset
        );

static int ipu_read_config_dword(
            struct ipu_device_t * dev,
            int offset
        );

static void ipu_write_config_byte(
            struct ipu_device_t * dev,
            int offset,
            char val
        );

static void ipu_write_config_word(
            struct ipu_device_t * dev,
            int offset,
            short val
        );

static void ipu_write_config_dword(
            struct ipu_device_t * dev,
            int offset,
            int val
        );

static int ipu_read_inst_ram(
            struct ipu_device_t * dev, 
            int *buf,
            int count,
            int f_pos 
        );

static int ipu_write_inst_ram(
            struct ipu_device_t * dev, 
            int *buf,
            int count,
            int f_pos
        );

static int ipu_mmap(
            struct ipu_device_t * dev,
            int buf_size
        );

/***************************************************************************************/
/*                              GLOBAL VARIABLE DECLARATION                            */
/***************************************************************************************/
 
static struct hw_module_methods_t ipu_module_methods = {
    open: ipu_device_open
};

/* every HAL module must be named HAL_MODULE_INFO_SYM */
struct ipu_module_t HAL_MODULE_INFO_SYM = {
    common: {
            tag:            HARDWARE_MODULE_TAG,        // hw_module_t tag must be set as HARDWARE_MODULE_TAG
            version_major:  1,            
            version_minor:  0,
            id:             IPU_HARDWARE_MODULE_ID,
            name:           MODULE_NAME,
            author:         MODULE_AUTHOR_NAME,
            methods:        &ipu_module_methods,
            }
};

/***************************************************************************************/
/*                              FUNCTION IMPLEMENTATION                                */
/***************************************************************************************/
static int ipu_device_open(const struct hw_module_t *module, const char *id, struct hw_device_t **device)
{
    ALOGI("[IPU_HAL]: Start to open IPU device.");
    if(!strcmp(id, IPU_HARDWARE_MODULE_ID))
    {
        struct ipu_device_t *dev;
        dev = (struct ipu_device_t *)malloc(sizeof(struct ipu_device_t));
        if(!dev)
        {
            ALOGE("[IPU_HAL] Failed to alloc space for ipu_device_t.");
            return -EFAULT;
        }
        memset(dev, 0, sizeof(struct ipu_device_t));

        dev->common.tag             = HARDWARE_DEVICE_TAG;              // hw_device tage must be set as HARDWARE_DEVICE_TAG
        dev->common.version         = 0;
        dev->common.module          = (hw_module_t *)module;
        dev->common.close           = ipu_device_close;
        dev->ipu_reset              = ipu_reset;
        dev->ipu_start              = ipu_start;
        dev->ipu_read_config_byte   = ipu_read_config_byte;
        dev->ipu_read_config_word   = ipu_read_config_word;
        dev->ipu_read_config_dword  = ipu_read_config_dword;
        dev->ipu_write_config_byte  = ipu_write_config_byte;
        dev->ipu_write_config_word  = ipu_write_config_word;
        dev->ipu_write_config_dword = ipu_write_config_dword;
        dev->ipu_read_inst_ram      = ipu_read_inst_ram;
        dev->ipu_write_inst_ram     = ipu_write_inst_ram;
        dev->ipu_mmap               = ipu_mmap;

        if((dev->fd = open(DEVICE_NAME, O_RDWR|O_SYNC)) == -1)
        {
            ALOGE("[IPU_HAL] Failed to open device file %s.", DEVICE_NAME);
            free(dev);
            return -EFAULT;
        }

        *device = &(dev->common);
        ALOGI("[IPU_HAL]: Open device file %s successfully.", DEVICE_NAME);
        return 0;
    }
    return -EFAULT;
}

static int ipu_device_close(struct hw_device_t *device)
{
    ALOGI("[IPU_HAL]: Start to close IPU device.");
    struct ipu_device_t *dev = (struct ipu_device_t *)device;
    if(dev)
    {
        close(dev->fd);
        free(dev);
    }
    ALOGI("[IPU_HAL]: Close device file %s successfully.", DEVICE_NAME);
    return 0;
}

static void ipu_start(struct ipu_device_t * dev)
{
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return;
    }
    ioctl(dev->fd, HAL_START, 0);
    ALOGI("[IPU_HAL]: IOCTL START successfully.");
}

static void ipu_reset(struct ipu_device_t * dev)
{
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return;
    }
    ioctl(dev->fd, HAL_RESET, 0);
    ALOGI("[IPU_HAL]: IOCTL RESET successfully.");
}

static char ipu_read_config_byte(struct ipu_device_t * dev, int offset)
{
    char ret_value;
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return 0;
    }
    ret_value = (char) ioctl(dev->fd, HAL_RDCONFIG_BYTE, offset);
    ALOGI("[IPU_HAL]: IOCTL READ CONFIG REG BYTE successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) ret_value);
    return ret_value;
}

static short ipu_read_config_word(struct ipu_device_t * dev, int offset)
{
    short ret_value;
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return 0;
    }
    ret_value = (short) ioctl(dev->fd, HAL_RDCONFIG_WORD, offset);
    ALOGI("[IPU_HAL]: IOCTL READ CONFIG REG WORD successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) ret_value);
    return ret_value;
}

static int ipu_read_config_dword(struct ipu_device_t * dev, int offset)
{
    short ret_value;
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return 0;
    }
    ret_value = (int) ioctl(dev->fd, HAL_RDCONFIG_DWORD, offset);
    ALOGI("[IPU_HAL]: IOCTL READ CONFIG REG DWORD successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) ret_value);
    return ret_value;
}

static void ipu_write_config_byte(struct ipu_device_t * dev, int offset, char val)
{
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return;
    }
    unsigned long of_va[2] = {(unsigned long)offset, (unsigned long)val};
    unsigned long *argu = of_va;
    ioctl(dev->fd, HAL_WTCONFIG_BYTE, argu);
    ALOGI("[IPU_HAL]: IOCTL WRITE CONFIG REG BYTE successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) val);
}

static void ipu_write_config_word(struct ipu_device_t * dev, int offset, short val)
{
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return;
    }
    unsigned long of_va[2] = {(unsigned long)offset, (unsigned long)val};
    unsigned long *argu = of_va;
    ioctl(dev->fd, HAL_WTCONFIG_WORD, argu);
    ALOGI("[IPU_HAL]: IOCTL WRITE CONFIG REG WORD successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) val);
}

static void ipu_write_config_dword(struct ipu_device_t * dev, int offset, int val)
{
    if (!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return;
    }
    unsigned long of_va[2] = {(unsigned long)offset, (unsigned long)val};
    unsigned long *argu = of_va;
    ioctl(dev->fd, HAL_WTCONFIG_DWORD, argu);
    ALOGI("[IPU_HAL]: IOCTL WRITE CONFIG REG DWORD successfully, Offset is 0x%x, Value is 0x%x.", (unsigned int) offset, (unsigned int) val);
}

static int ipu_read_inst_ram(struct ipu_device_t *dev, int *buf, int count, int f_pos)
{
    int ret_value;
    if(!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return -EFAULT;
    }
    
    /* set read address */
    ret_value = lseek(dev->fd, (unsigned int)f_pos, 0);
    if(ret_value == -1)
    {
        ALOGE("[IPU_HAL]: Failed to set reading address.");
        return -EFAULT;
    }

    /* read data from ram */
    ret_value = read(dev->fd, (void *)buf, (unsigned int)count);
    if(ret_value == -1)
    {
        ALOGE("[IPU_HAL]: Failed to read data.");
        return -EFAULT;
    }
    ALOGI("[IPU_HAL] Intended to read %dbytes. In fact Read out %dbytes.", count, ret_value);

    return 0;

}

static int ipu_write_inst_ram(struct ipu_device_t *dev, int *buf, int count, int f_pos)
{
    int ret_value;
    if(!dev)
    {
        ALOGE("[IPU_HAL]: Null dev pointer.");
        return -EFAULT;
    }
    
    /* set read address */
    ret_value = lseek(dev->fd, (unsigned int)f_pos, 0);

    if(ret_value == -1)
    {
        ALOGE("[IPU_HAL]: Failed to set writing address.");
        return -EFAULT;
    }

    /* read data from ram */
    ret_value = write(dev->fd, (void *)buf, (unsigned int)count);
    if(ret_value == -1)
    {
        ALOGE("[IPU_HAL]: Failed to write data.");
        return -EFAULT;
    }
    ALOGI("[IPU_HAL]: Intended to write %dbytes. In fact write in %dbytes.", count, ret_value);

    return 0;
}

static int ipu_mmap(struct ipu_device_t *dev, int buf_size)
{
    int ret_value;
    void *virt_base;
    //if((ret_value = (int)mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, 0)) == MAP_FAILED)
    virt_base = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, 0);
    if (virt_base == MAP_FAILED)
    {
        ALOGE("[IPU_HAL]: Failed mmap.");
        return -EFAULT;
    }
    ret_value = (int)virt_base;
    ALOGI("[IPU_HAL]: MMAP successfully. Return virtual address is 0x%x", (unsigned int)ret_value);
    return ret_value;
}
