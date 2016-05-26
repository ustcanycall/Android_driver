/* *************************************************************************************/
// FILE NAME:   IpuService.java
// VERSION:     v2.0
// DESCRIPTION: IPU service.
// AUTHOR:      ustcanycall
// CONTACT:     anycall@mail.ustc.edu.cn
// DATE:        2016.05.05
/* *************************************************************************************/

package com.android.server;

import android.content.Context;
import android.os.IIpuService;
import android.util.Slog;

public class IpuService extends IIpuService.Stub
{
    private static final String TAG = "IPUService";
    
    private static int dma_size = (1 * 1024 * 1024);

    // ipu_device_t pointer
    private int mPtr = 0;

    // address of dma buffer
    private int mAddress = 0;

    IpuService()
    {
        mPtr = init_native();
        if(mPtr == 0)
        {
            Slog.e(TAG, "Failed to initialize IPU service.");
        }

        mAddress = mmap_native(mPtr, dma_size);

        if(mAddress == 0)
        {
            Slog.e(TAG, "Failed to mmap dma memory.");
        }

    }

    public void start()
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return ;
        }
        start_native(mPtr); 
    }

    public void reset()
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return ;
        }
        reset_native(mPtr);
    }

    public char read_config_byte(int offset)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }
        return read_config_byte_native(mPtr, offset);
    }

    public int read_config_word(int offset)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }
        return (int)read_config_word_native(mPtr, offset);
    }

    public int read_config_dword(int offset)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }
        return read_config_dword_native(mPtr, offset);
    }

    public void write_config_byte(int offset, char val)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return ;
        }
        write_config_byte_native(mPtr, offset, val);
    }

    public void write_config_word(int offset, int val)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return ;
        }
        write_config_word_native(mPtr, offset, (short)val);
    }

    public void write_config_dword(int offset, int val)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return ;
        }
        write_config_dword_native(mPtr, offset, val);
    }

    public int read_inst_ram(int[] buf, int count, int f_pos)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }
        return read_inst_ram_native(mPtr, buf, count, f_pos);
    }

    public int write_inst_ram(int[] buf, int count, int f_pos)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }
        return write_inst_ram_native(mPtr, buf, count, f_pos);
    }

    public int read_dma_buffer(int[] buf, int srcOffset, int dstOffset, int count)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }

        if(mAddress == 0)
        {
            Slog.e(TAG, "IPU dma buffer not mmap.");
            return 0;
        }

        return read_dma_buffer_native(mAddress, buf, srcOffset, dstOffset, count);
    }

    public int write_dma_buffer(int[] buf, int srcOffset, int dstOffset, int count)
    {
        if(mPtr == 0)
        {
            Slog.e(TAG, "IPU service is not initialized.");
            return 0;
        }

        if(mAddress == 0)
        {
            Slog.e(TAG, "IPU dma buffer not mmap.");
            return 0;
        }

        return write_dma_buffer_native(mAddress, buf, srcOffset, dstOffset, count);
    }

    private static native int   init_native();
    private static native void  start_native(int ptr);
    private static native void  reset_native(int ptr);
    private static native char  read_config_byte_native(int ptr, int offset);
    private static native short read_config_word_native(int ptr, int offset);
    private static native int   read_config_dword_native(int ptr, int offset);
    private static native void  write_config_byte_native(int ptr, int offset, char val);
    private static native void  write_config_word_native(int ptr, int offset, short val);
    private static native void  write_config_dword_native(int ptr, int offset, int val);
    private static native int   read_inst_ram_native(int ptr, int[] buf, int count, int f_pos);
    private static native int   write_inst_ram_native(int ptr, int[] buf, int count, int f_pos);
    private static native int   read_dma_buffer_native(int address, int[] buff, int srcOffset, int dstOffset, int count);
    private static native int   write_dma_buffer_native(int address, int[] buff, int srcOffset, int dstOffset, int count);
    private static native int   mmap_native(int ptr, int size);
}

