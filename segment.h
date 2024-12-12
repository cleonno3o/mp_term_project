// #include "S32K144.h"
#include "device_registers.h"
#include "lpit.h"
#include "port.h"

void segment_init()
{
    for (int i = 1; i <= 11; i++)
    {
        SEGMENT_PORTN->PCR[i] = PORT_PCR_MUX(1);
        SEGMENT_PTN->PDDR |= 1 << i;
    }
}

void set_num (int num, int pos)
{
    unsigned int fnd_data[10]={0x7E, 0x0C,0xB6,0x9E,0xCC,0xDA,0xFA,0x4E,0xFE,0xDE};// 0~9 number
    unsigned int fnd_sel[4]={0x0100,0x0200,0x0400,0x0800};

    SEGMENT_PTN->PCOR |= 0xFF;          // clear data output
    SEGMENT_PTN->PSOR |= fnd_sel[pos];  // ouput com port select
    SEGMENT_PTN->PCOR |= 0xFF;          // clear data output
    SEGMENT_PTN->PSOR |= fnd_data[num]; // set data output
    delay_ms(2);
    SEGMENT_PTN->PCOR |= 0xFFF; // clear all
}

void print_4_digit(int num)
{
    for (int i = 1; i <= 4; i++)
    {
        set_num(num % 10, 4 - i);
        num /= 10;
    }
}