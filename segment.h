// #include "S32K144.h"
#include "device_registers.h"
#include "lpit.h"
#include "port.h"

void segment_init()
{
    for (int i = 0; i <= 8; i++)
    {
        SEGMENT_PORTN->PCR[i] = PORT_PCR_MUX(1);
        SEGMENT_PTN->PDDR |= 1 << i;
    }
}

void set_num (int num, int pos)
{
    unsigned int fnd_data[10]= {
    0x3F, // 0: A, B, C, D, E, F
    0x06, // 1: B, C
    0x5B, // 2: A, B, D, E, G
    0x4F, // 3: A, B, C, D, G
    0x66, // 4: B, C, F, G
    0x6D, // 5: A, C, D, F, G
    0x7D, // 6: A, C, D, E, F, G
    0x07, // 7: A, B, C
    0x7F, // 8: A, B, C, D, E, F, G
    0x6F  // 9: A, B, C, D, F, G
    };
    unsigned int fnd_sel[2]={0x80, 0x0100};

    SEGMENT_PTN->PCOR |= 0x1FF;          // clear data output
    SEGMENT_PTN->PSOR |= fnd_sel[pos];  // ouput com port select
    SEGMENT_PTN->PCOR |= 0x7F;          // clear data output
    SEGMENT_PTN->PSOR |= fnd_data[num]; // set data output
    delay_ms(2);
    SEGMENT_PTN->PCOR |= 0x1FF; // clear all
}

void print_4_digit(int num)
{
    for (int i = 1; i <= 2; i++)
    {
        set_num(num % 10, 2 - i);
        num /= 10;
    }
}