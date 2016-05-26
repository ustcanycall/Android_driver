/***************************************************************************************/
// FILE NAME:   ipu.c
// VERSION:     v2.0
// DESCRIPTION: IPU device driver. IPU[31:22] = 0
//              Configuration Register: IPU[21:20] = 2'b00 SIZE = ?
//              Instruction RAM:        IPU[21:20] = 2'b01 SIZE = 32KB
//              CT DATA RAM:            IPU[21:20] = 2'b10 SIZE = 1MB
//              LT DATA RAM:            IPU[21:20] = 2'b11 SIZE = 1MB
//              DMA BUFFER:             change "mem=" of bootargs
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.04.27
/***************************************************************************************/

#include <linux/init.h>         // moudle_init(), moudle_exit()
#include <linux/module.h>       //      
#include <linux/slab.h>         // kmalloc(), kfree()
#include <linux/types.h>        // special types: dev_t, off_t, pid_t
#include <linux/fs.h>           // struct file_operations; struce inode; MINOR; MAJOR
#include <linux/ioctl.h>        // _IO() _IOR() _IOW() _IOWR()
#include <linux/cdev.h>         // struct cdev
#include <linux/semaphore.h>    // struct semaphore, up(), down()
#include <linux/version.h>      // LINUX_VERSION_CODE, KERNEL_VERSION
#include <linux/device.h>       // device_create(), class_create(), class_destroy()
#include <linux/dma-mapping.h>  // mmap()
#include <asm/uaccess.h>        // get_user() put_user() copy_from_user() copy_to_user
#include <asm/io.h>             // ioremap() iounmap() ioread*() iowrite*()
#include "ipu.h"

/***************************************************************************************/
/*                              MICRO DEFINITION                                       */
/***************************************************************************************/

// Refer to ipu.h

/***************************************************************************************/
/*                              STRUCT TYPE DECLARATION                                */
/***************************************************************************************/

struct ipu
{
    const char *        name;

    unsigned int        config_reg_length;
    phys_addr_t         config_reg_phys_addr;
    void __iomem *      config_reg_virt_addr;
    
    unsigned int        inst_ram_size;
    phys_addr_t         inst_ram_phys_addr;
    void __iomem *      inst_ram_virt_addr;

    struct semaphore    config_reg_sem;
    struct semaphore    inst_ram_sem;

    dev_t               chrdev;
    struct cdev         cdev; 
};


/***************************************************************************************/
/*                              FUNCTION DECLARATION                                   */
/***************************************************************************************/

static int ipu_open(
            struct inode * inode,
            struct file * filp
        );

static int ipu_release(
            struct inode * inode,
            struct file * filp
        );

static ssize_t ipu_read(
            struct file * filp,
            char __user * buf, 
            size_t count, 
            loff_t * f_pos
        );

static ssize_t ipu_write(
            struct file * filp,
            const char __user * buf,
            size_t count,
            loff_t * f_pos
        );

static 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
int
#else
long 
#endif
ipu_ioctl(
            #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
            struct inode *inode,
            #endif
            struct file * filp,
            unsigned int cmd,
            unsigned long arg
        );

static loff_t ipu_llseek(
            struct file * filp,
            loff_t off,
            int whence
        );

static int ipu_mmap(
            struct file * filp,
            struct vm_area_struct * vma
        );

/***************************************************************************************/
/*                              GLOBAL VARIABLE DECLARATION                            */
/***************************************************************************************/

static int ipu_major = 0;
static int ipu_minor = 0;

static struct ipu *dev = NULL;
static struct class * dev_class = NULL;
static const struct file_operations ipu_fops = {
    .owner          = THIS_MODULE,
    .open           = ipu_open,
    .release        = ipu_release,
    .read           = ipu_read,
    .write          = ipu_write,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    .ioctl          = ipu_ioctl,
#else
    .unlocked_ioctl = ipu_ioctl,
#endif
    .llseek         = ipu_llseek,
    .mmap           = ipu_mmap,
};

//void *buffer_virt_addr;
//dma_addr_t  buffer_phys_addr; 
static char buffer[1024];

/***************************************************************************************/
/*                      NEW KERNEL VERSION ABOLISHED FUNCTION                           */
/***************************************************************************************/

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25))
static void init_MUTEX(struct semaphore *sem)
{
    sema_init(sem, 1);
}
#endif

/***************************************************************************************/
/*                              FUNCTION IMPLEMENTATION                                */
/***************************************************************************************/

static int ipu_open(struct inode * inode, struct file * filp)
{
    struct ipu * dev;

    dev = container_of(inode->i_cdev, struct ipu, cdev);
    filp->private_data = dev;

    if(dev)
    {
        printk("[ipu_open]:%p\n", filp);
        return SUCCESS;
    }
    else
    {
        printk("[ipu_open]: [ERROR] Can't open IPU device!\n");
        return -EINVAL;
    }
}

static int ipu_release(struct inode *inode, struct file *filp)
{
    struct ipu * dev;

    dev = filp->private_data;

    if(dev)
    {
        printk("[ipu_release]:%p\n", filp);
        return SUCCESS;
    }
    else
    {
        printk("[ipu_release]: [ERROR] No IPU device!\n");
        return -EINVAL;
    }
}

static 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
int
#else
long 
#endif
ipu_ioctl(
        #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
        struct indoe *inode,
        #endif
        struct file *filp, 
        unsigned int cmd, unsigned long arg)
{
    int ret_value = SUCCESS;
    struct ipu *dev;
    dev = filp->private_data;

    if(!dev)
    {
        printk("[ipu_ioctl]: [ERROR] No IPU device!\n");
        return -EINVAL;
    }

    if(down_interruptible(&dev->config_reg_sem))
    {
        return -ERESTARTSYS;
    }

    switch(cmd)
    {

        /* Read BYTE */
        case RDCONFIG_BYTE:
            {
            unsigned char read_value;
            read_value = ioread8(dev->config_reg_virt_addr + (unsigned int)arg);
            printk("[ipu_ioctl]: Read CONFIG REG byte offset 0x%x, value is 0x%x\n", (unsigned int)arg, (unsigned int)read_value);
            ret_value = (unsigned char) read_value;
            break;
            }

        /* Read WORD */
        case RDCONFIG_WORD:
            {
            unsigned short read_value;
            read_value = ioread16(dev->config_reg_virt_addr + (unsigned int)arg);
            printk("[ipu_ioctl]: Read CONFIG REG word offset 0x%x, value is 0x%x\n", (unsigned int)arg, (unsigned int)read_value);
            ret_value =  (unsigned short)read_value;
            break;
            }
        /* Read DWORD  */
        case RDCONFIG_DWORD:
            {
            unsigned int read_value;
            read_value = ioread32(dev->config_reg_virt_addr + (unsigned int)arg);
            printk("[ipu_ioctl]: Read CONFIG REG dword offset 0x%x, value is 0x%x\n", (unsigned int)arg, (unsigned int)read_value);
            ret_value = (unsigned int)read_value;
            break;
            }
        /* Write BYTE */
        case WTCONFIG_BYTE:
            {
            unsigned int *offset = (unsigned int *)arg;
			unsigned char *data = (unsigned char *)((unsigned long *)arg+1);
            printk("[ipu_ioctl]: Write CONFIG REG byte offset 0x%x, value is 0x%x\n", (unsigned int) *offset, (unsigned int)*data);
            iowrite8(*data, dev->config_reg_virt_addr + *offset);
            break;
            }
        /* Write WORD */
        case WTCONFIG_WORD:
            {
            unsigned int *offset = (unsigned int *)arg;
			unsigned short *data = (unsigned short *)((unsigned long *)arg+1);
            printk("[ipu_ioctl]: Write CONFIG REG word offset 0x%x, value is 0x%x\n", (unsigned int) *offset, (unsigned int)*data);
            iowrite16(*data, dev->config_reg_virt_addr + *offset);
            break;
            }
        /* Write DWORD */
        case WTCONFIG_DWORD:
            {
            unsigned int *offset = (unsigned int *)arg;
			unsigned int *data = (unsigned int *)((unsigned long *)arg+1);
            printk("[ipu_ioctl]: Write CONFIG REG dword offset 0x%x, value is 0x%x\n", (unsigned int) *offset, (unsigned int)*data);
            iowrite32(*data, dev->config_reg_virt_addr + *offset);
            break;
            }
        /* Start */
        case START:
            // Add code here
            printk("[ipu_ioctl]: Start!\n");
            break;
        /* Reset */
        case RESET:
            // Add code here
            printk("[ipu_ioctl]: Reset!\n");
            break;
        /* Default Operatin */
        default:
            printk("[ipu_ioctl]: [ERROR] WRONG CMD!\n");
            ret_value = -EINVAL;
            break;
        }
    
    up(&dev->config_reg_sem);

    return ret_value;

}

static ssize_t ipu_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t ret_value = SUCCESS;
    struct ipu *dev;
    dev = filp->private_data;

    if(!dev)
    {
        printk("[ipu_read]: [ERROR] No IPU device!\n");
        return -EINVAL;
    }

    if(down_interruptible(&dev->inst_ram_sem))
        return -ERESTARTSYS;

    if(count == 0) 
    {
        printk("[ipu_read]: [WARNING] Read data count is zero!\n");
        goto out;
    }
    if(*f_pos > dev->inst_ram_size) 
    {
        printk("[ipu_read]: [WARNING] Read file position out of range!\n");
        goto out;
    }
    if(*f_pos + count > dev->inst_ram_size) 
    {
        count = dev->inst_ram_size - *f_pos; 
    }

//if(copy_to_user(buf, (dev->inst_ram_virt_addr + *f_pos), count))
    if(copy_to_user(buf, (buffer + *f_pos), count))
    {
        printk("[ipu_read]: [ERROR] Copy data to user failed!\n");
        ret_value = -EFAULT;
        goto out;
    }

    ret_value = count;
    printk("[ipu_read]:read %d bytes\n",ret_value);

out:
    up(&dev->inst_ram_sem);
    return ret_value;
}


static ssize_t ipu_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t ret_value = SUCCESS;
    struct ipu *dev;
    dev = filp->private_data;

    if(!dev)
    {
        printk("[ipu_write]: [ERROR] No IPU device!\n");
        return -EINVAL;
    }

    if(down_interruptible(&dev->inst_ram_sem))
        return -ERESTARTSYS;

    if(count == 0) 
    {
        printk("[ipu_write]: [WARNING] Write data count is zero!\n");
        goto out;
    }
    if(*f_pos > dev->inst_ram_size) 
    {
        printk("[ipu_write]: [WARNING] Write file position out of range!\n");
        goto out;
    }
    if(*f_pos + count > dev->inst_ram_size) 
    {
        count = dev->inst_ram_size - *f_pos; 
    }

//if(copy_from_user((dev->inst_ram_virt_addr + *f_pos), buf, count))
    if(copy_from_user((buffer + *f_pos), buf, count))
    {
        printk("[ipu_write]: [ERROR] Copy data from user failed!\n");
        ret_value = -EFAULT;
        goto out;
    }
    ret_value = count;
    printk("[ipu_write]:write %d bytes\n",ret_value);

out:
    up(&dev->inst_ram_sem);
    return ret_value;
}

static loff_t ipu_llseek(struct file *filp, loff_t off, int whence)
{
    struct ipu *dev;
    loff_t pos;

    dev = filp->private_data;
    if(!dev)
    {
        printk("[ipu_llseek]: [ERROR] No IPU device!\n");
        return -EINVAL;
    }

    pos = filp->f_pos;
    switch (whence)
    {
        /* Set f_pos */
        case 0:
            pos = off;
            break;
        /* Move f_pos forward */
        case 1:
            pos += off;
            break;
        /* More */
        case 2:
            break;
        /* Default Operation */
        default:
            return -EINVAL;
    }

    if ((pos > dev->inst_ram_size) || (pos < 0))
    {
        printk("[ipu_llseek]: [ERROR] Move file position out of range!");
        return -EINVAL;
    }
    
    printk("[ipu_llseek]: Move file position to %d", (unsigned int)pos);
    return filp->f_pos = pos;
}

static int ipu_mmap(struct file *filp, struct vm_area_struct *vma)
{
    struct ipu *dev;
    dev = filp->private_data;

    if(!dev)
    {
        printk("[ipu_mmap]: [ERROR] Can't find ipu device!\n");
        return -EINVAL;
    }

    if (vma->vm_end - vma->vm_start > DMA_BUFFER_SIZE)
    {
        printk("[ipu_mmap]: [ERROR] Mmap size too large!\n");
        return -EINVAL;
    }

    vma->vm_flags |= VM_IO;
    //vma->vm_flags |= VM_RESERVED;
    vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, DMA_BUFFER_START >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot))
    {
        printk("[ipu_mmap]: [ERROR] Can't mmap physical_address 0x%x to user space 0x%x!\n", (unsigned int)DMA_BUFFER_START, (unsigned int)vma->vm_start);
        return -EAGAIN;
    }

    printk("[ipu_mmap]: Mmap physical_address 0x%x to user space 0x%x!\n", (unsigned int)DMA_BUFFER_START, (unsigned int)vma->vm_start);
    return 0;
}


static int __init ipu_init(void)
{
    int err = -1;
    dev_t chrdev = 0;
    struct device *temp = NULL;

    printk(KERN_ALERT"[ipu_init]: Initializing IPU device!\n");

    /* Allocate char device region */
    if(ipu_major)
    {
        chrdev = MKDEV(ipu_major, ipu_minor);
        err = register_chrdev_region(chrdev, 1, IPU_NAME);
    }
    else
    {
        err = alloc_chrdev_region(&chrdev, 0, 1, IPU_NAME);
    }
    if(err < 0) 
    {
       printk(KERN_ALERT"[ipu_init]: [Error] Failed to allocate device ID for IPU!\n");
       goto fail;
    }
    ipu_major = MAJOR(chrdev);
    ipu_minor = MINOR(chrdev);

    /* Allocate ipu struct */
    dev = kmalloc(sizeof(struct ipu), GFP_KERNEL);
    if(!dev)
    {
        err = -ENOMEM;
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to allocate memory for struct ipu!\n");
        goto unregister;
    }
    
    /* Initialize ipu */
    memset(dev, 0, sizeof(struct ipu));
    dev->name = IPU_NAME;
    dev->inst_ram_phys_addr = INST_RAM_BASE_ADDRESS;
    dev->inst_ram_size = INST_RAM_SIZE;
    dev->config_reg_phys_addr = CONFIG_REG_BASE_ADDRESS;
    dev->config_reg_length = CONFIG_REG_NUM * CONFIG_REG_WIDTH;
    init_MUTEX(&(dev->config_reg_sem));
    init_MUTEX(&(dev->inst_ram_sem));

    /* Ioremap instruction ram physical address */
    dev->inst_ram_virt_addr = ioremap_nocache(dev->inst_ram_phys_addr, dev->inst_ram_size);
    if(!dev->inst_ram_virt_addr)
    {
        err = -EIO;
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to ioremap instruction ram physical address!\n");
        goto cleanup;
    }
    printk(KERN_ALERT"[ipu_init]: Ioremap instruction ram physical address 0x%x to kernel virtual address 0x%x!\n", \
            (unsigned int)dev->inst_ram_phys_addr, (unsigned int)dev->inst_ram_virt_addr);

    /* Map reg physical address */
    dev->config_reg_virt_addr = ioremap_nocache(dev->config_reg_phys_addr, dev->config_reg_length);
    if(!dev->config_reg_virt_addr)
    {
        err = -EIO;
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to ioremap configuration register physical address!\n");
        goto unmap_ram;
    }
    printk(KERN_ALERT"[ipu_init]: Ioremap configuration register physical address 0x%x to kernel virtual address 0x%x!\n", \
            (unsigned int)dev->config_reg_phys_addr, (unsigned int)dev->config_reg_virt_addr);

    /* Add IPU device to system */
    cdev_init(&(dev->cdev), &ipu_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &ipu_fops;
    err = cdev_add(&(dev->cdev), chrdev, 1);
    if(err)
    {
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to Add IPU to system!\n");
        goto unmap_reg;
    }

    /* Create ipu class under /sys/class */
    dev_class = class_create(THIS_MODULE, IPU_NAME);
    if(IS_ERR(dev_class))
    {
        err = PTR_ERR(dev_class);
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to create ipu class!\n");
        goto destroy_cdev;
    }

    /* Register your own device in sysfs, and this will cause udev to create corresponding device node */  
    temp = device_create(dev_class, NULL, chrdev, NULL, "%s", IPU_NAME);
    if(IS_ERR(temp))
    {
        err = PTR_ERR(temp);
        printk(KERN_ALERT"[ipu_init]: [Error] Failed to mount IPU to /dev/ipu!\n");
        goto destroy_class;
    }

    printk(KERN_ALERT"[ipu_init]: Succeeded to initialize ipu device.\n");

    return SUCCESS;

destroy_device:
    device_destroy(dev_class, chrdev);
destroy_class:
    class_destroy(dev_class);
destroy_cdev:
    cdev_del(&(dev->cdev));
unmap_reg:
    iounmap(dev->config_reg_virt_addr);
unmap_ram:
    iounmap(dev->inst_ram_virt_addr);
cleanup:
    kfree(dev);
unregister:
    unregister_chrdev_region(chrdev, 1);
fail:
    return err;
}

static void __exit ipu_exit(void)
{
    dev_t chrdev;
    chrdev = MKDEV(ipu_major, ipu_minor);

    printk(KERN_ALERT"[ipu_exit]: Destroying IPU device!\n");

    /* Destroy ipu class */
    if (dev_class)
    {
        device_destroy(dev_class, chrdev);
        class_destroy(dev_class);
    }

    /* Destroy ipu */
    if(dev)
    {
        cdev_del(&dev->cdev);
        iounmap(dev->config_reg_virt_addr);
        iounmap(dev->inst_ram_virt_addr);
        kfree(dev);
    }

    /* Unregister chrdev region */
    unregister_chrdev_region(chrdev, 1);

    printk(KERN_ALERT"[ipu_exit]: Succeeded to destroying IPU device.\n");

}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IPU Device Driver");

module_init(ipu_init);
module_exit(ipu_exit);




