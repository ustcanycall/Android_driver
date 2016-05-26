/***************************************************************************************/
// FILE NAME:   com_android_server_IpuService.cpp
// VERSION:     v2.0
// DESCRIPTION: JNI implementation IPU service
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.05.05
/***************************************************************************************/
#define LOG_TAG "IPUServiceJNI"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"


#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/ipu.h>

#include <stdio.h>

namespace android
{

    /***************************************************************************************/
    /*                             JNI METHODS OF HARDWARE ACCESS SERVICE                  */
    /***************************************************************************************/
    /* JNI method of ioctl start operation */
    static void IPU_start(JNIEnv *env, jobject clazz, jint ptr)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return ;
        }
        
        ALOGI("[IPU_JNI]: IOCTL Start Operation.");
        
        device->ipu_start(device);
    }

    /* JNI method of ioctl reset operation */
    static void IPU_reset(JNIEnv *env, jobject clazz, jint ptr)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return ;
        }
        
        ALOGI("[IPU_JNI]: IOCTL Reset Operation.");
        
        device->ipu_reset(device);
    }
        
    /* JNI method of ioctl read config reg byte operation */
    static jchar IPU_read_config_byte(JNIEnv *env, jobject clazz, jint ptr, jint offset)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return 0;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Read config reg byte Operation.");
        
        char ret_value;
        ret_value = device->ipu_read_config_byte(device, offset);

        ALOGI("[IPU_JNI]: IOCTL Read config reg byte Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)ret_value);
        return ret_value;
    }

    /* JNI method of ioctl read config reg word operation */
    static jshort IPU_read_config_word(JNIEnv *env, jobject clazz, jint ptr, jint offset)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return 0;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Read config reg word Operation.");
        
        short ret_value;
        ret_value = device->ipu_read_config_word(device, offset);

        ALOGI("[IPU_JNI]: IOCTL Read config reg word Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)ret_value);
        return ret_value;
    }

    /* JNI method of ioctl read config reg dword operation */
    static jint IPU_read_config_dword(JNIEnv *env, jobject clazz, jint ptr, jint offset)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return 0;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Read config reg dword Operation.");
        
        int ret_value;
        ret_value = device->ipu_read_config_dword(device, offset);

        ALOGI("[IPU_JNI]: IOCTL Read config reg dword Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)ret_value);
        return ret_value;
    }

    /* JNI method of ioctl write config reg byte operation */
    static void IPU_write_config_byte(JNIEnv *env, jobject clazz, jint ptr, jint offset, jchar val)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return ;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Write config reg byte Operation.");
        device->ipu_write_config_byte(device, offset, val);
        ALOGI("[IPU_JNI]: IOCTL Write config reg byte Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)val);
    }

    /* JNI method of ioctl write config reg word operation */
    static void IPU_write_config_word(JNIEnv *env, jobject clazz, jint ptr, jint offset, jshort val)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return ;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Write config reg word Operation.");
        device->ipu_write_config_word(device, offset, val);
        ALOGI("[IPU_JNI]: IOCTL Write config reg word Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)val);
    }

    /* JNI method of ioctl write config reg dword operation */
    static void IPU_write_config_dword(JNIEnv *env, jobject clazz, jint ptr, jint offset, jint val)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return ;
        }
         
        ALOGI("[IPU_JNI]: IOCTL Write config reg dword Operation.");
        device->ipu_write_config_dword(device, offset, val);
        ALOGI("[IPU_JNI]: IOCTL Write config reg dword Successfully. Offset is 0x%x, Value is 0x%x", (int)offset, (int)val);
    }

    /* JNI method of read inst_ram operation */
    static jint IPU_resd_inst_ram(JNIEnv *env, jobject clazz, jint ptr, jintArray buf, jint count, jint f_pos)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return -1;
        }
         
        ALOGI("[IPU_JNI]: Read inst_ram operation.");
        
        int ret_value;
        /* Can not directly operate buf array, use GetXXXArrayEelements and ReleaseXXXArrayElements */
        jint *buf_copy = env->GetIntArrayElements(buf, 0);
        ret_value = device->ipu_read_inst_ram(device, buf_copy, count, f_pos);
        env->ReleaseIntArrayElements(buf, buf_copy, 0);

        return ret_value;
    }

    /* JNI method of write inst_ram operation */
    static jint IPU_write_inst_ram(JNIEnv *env, jobject clazz, jint ptr, jintArray buf, jint count, jint f_pos)
    {
        /* convert ptr to struct ipu_device_t */
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return -1;
        }
         
        ALOGI("[IPU_JNI]: Write inst_ram operation.");
        
        int ret_value;
        /* Can not directly operate buf array, use GetXXXArrayEelements and ReleaseXXXArrayElements */
        jint *buf_copy = env->GetIntArrayElements(buf, 0);
        ret_value = device->ipu_write_inst_ram(device, buf_copy, count, f_pos);
        env->ReleaseIntArrayElements(buf, buf_copy, 0);
        return ret_value;
    
    }

    /* JNI method of read dma buffer operation */
    static jint IPU_read_dma_buffer(JNIEnv *env, jobject clazz, jint address, jintArray buf, jint srcOffset, jint dstOffset, jint count)
    {
        if(!address)
        {
            ALOGE("[IPU_JNI]: IPU dma buffer not mmap.");
            return -1;
        }
         
        ALOGI("[IPU_JNI]: Write dma_buffer operation.");
        
        env->SetIntArrayRegion(buf, dstOffset, count, (const jint*)address + srcOffset);

        return count;
    
    }

    /* JNI method of write dma buffer operation */
    static jint IPU_write_dma_buffer(JNIEnv *env, jobject clazz, jint address, jintArray buf, jint srcOffset, jint dstOffset, jint count)
    {
        if(!address)
        {
            ALOGE("[IPU_JNI]: IPU dma buffer not mmap.");
            return -1;
        }
         
        ALOGI("[IPU_JNI]: Write dma_buffer operation.");
        
        env->GetIntArrayRegion(buf, srcOffset, count, (jint*)address + dstOffset);

        return count;
    
    }
    static jint IPU_mmap(JNIEnv *env, jobject clazz, jint ptr, jint size)
    {
        ipu_device_t *device = (ipu_device_t*) ptr;
        if(!device)
        {
            ALOGE("[IPU_JNI]: Device IPU is not open.");
            return -1;
        }

        ALOGI("[IPU_JNI]: MMAP operation.");

        int ret_value;
        ret_value = device->ipu_mmap(device, size);
        return ret_value;
    }

    /***************************************************************************************/
    /*                              OPEN VIRTUAL DEVICE OF HAL LAYER                       */
    /***************************************************************************************/
    /* Open virtual device in HAL layer */
    static inline int IPU_device_open(const hw_module_t *module, struct ipu_device_t **device)
    {
        return module->methods->open(module, IPU_HARDWARE_DEVICE_ID, (struct hw_device_t**)device);
    }

    /* Initialize virtual device */
    static jint IPU_init(JNIEnv *env, jclass clazz)
    {
        ipu_module_t *module;
        ipu_device_t *device;

        ALOGI("[IPU_JNI]: Initialize HAL stub IPU......");

        /* load module of HAL layer */
        if (hw_get_module(IPU_HARDWARE_MODULE_ID, (const struct  hw_module_t**)&module) ==0)
        {
            ALOGI("[IPU_JNI]: Device IPU found.");
            /* open device in HAL layer */
            if(IPU_device_open(&(module->common), &device) == 0)
            {
                ALOGI("[IPU_JNI]: Device IPU is open.");
                /* convert struct ipu_device_t to jint and return */
                IPU_start(env, clazz, (jint)device);

                return (jint)device;
            }

            ALOGE("[IPU_JNI]: Failed to open device IPU.");

            return 0;
        }
        
        ALOGE("[IPU_JNI]: Failed to get HAL stub IPU.");
        return 0;
    }


    /***************************************************************************************/
    /*                              JNI METHODS TABLE                                      */
    /***************************************************************************************/
    static const JNINativeMethod method_table[] = {
        {"init_native",                 "()I",          (void*)IPU_init},
        {"start_native",                "(I)V",         (void*)IPU_start},
        {"reset_native",                "(I)V",         (void*)IPU_reset},
        {"read_config_byte_native",     "(II)C",        (void*)IPU_read_config_byte},
        {"read_config_word_native",     "(II)S",        (void*)IPU_read_config_word},
        {"read_config_dword_native",    "(II)I",        (void*)IPU_read_config_dword},
        {"write_config_byte_native",    "(IIC)V",       (void*)IPU_write_config_byte},
        {"write_config_word_native",    "(IIS)V",       (void*)IPU_write_config_word},
        {"write_config_dword_native",   "(III)V",       (void*)IPU_write_config_dword},
        {"read_inst_ram_native",        "(I[III)I",     (void*)IPU_resd_inst_ram},
        {"write_inst_ram_native",       "(I[III)I",     (void*)IPU_write_inst_ram},
        {"read_dma_buffer_native",      "(I[IIII)I",    (void*)IPU_read_dma_buffer},
        {"write_dma_buffer_native",     "(I[IIII)I",    (void*)IPU_write_dma_buffer},
        {"mmap_native",                 "(II)I",        (void*)IPU_mmap},
    };

    /***************************************************************************************/
    /*                              REGISTER JNI METHODS                                   */
    /***************************************************************************************/
    int register_android_server_IpuService(JNIEnv *env)
    {
       return jniRegisterNativeMethods(env, "com/android/server/IpuService", method_table, NELEM(method_table));
    }
}; 










