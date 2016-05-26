package android.os;

interface IIpuService
{
    void start();
    void reset();
    char read_config_byte(int offset);
    int read_config_word(int offset);
    int read_config_dword(int offset);
    void write_config_byte(int offset, char val);
    void write_config_word(int offset, int val);
    void write_config_dword(int offset, int val);
    int read_inst_ram(inout int[] buf, int count, int f_pos);
    int write_inst_ram(inout int[] buf, int count, int f_pos);    
    int read_dma_buffer(inout int[] buf, int srcOffset, int dstOffset, int count);
    int write_dma_buffer(inout int[] buf, int srcOffset, int dstOffset, int count);
}

