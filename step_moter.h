#include "device_registers.h"
// #include "S32K144.h"
#include "lpit.h"
#include "port.h"

#define STEP_MAX 128

extern int STEP_LAST = 0;
/* TODO:
STEP_LAST 부터 시작해서 target까지 움직이는 거로 
이러면 각도를 정하는게 아니라 끝을 그냥 0 ~ 180도가 맞는듯
*/

void step_init()
{
    STEP_PORTN->PCR[STEP_IN_1] = PORT_PCR_MUX(1);
    STEP_PORTN->PCR[STEP_IN_2] = PORT_PCR_MUX(1);
    STEP_PORTN->PCR[STEP_IN_3] = PORT_PCR_MUX(1);
    STEP_PORTN->PCR[STEP_IN_4] = PORT_PCR_MUX(1);

    STEP_PTN->PDDR |= 1 << STEP_IN_1 | 1 << STEP_IN_2 | 1 << STEP_IN_3 | 1 << STEP_IN_4;
}

void step_clear()
{
    STEP_PTN->PCOR = 1 << STEP_IN_1;
    STEP_PTN->PCOR = 1 << STEP_IN_2;
    STEP_PTN->PCOR = 1 << STEP_IN_3;
    STEP_PTN->PCOR = 1 << STEP_IN_4;
}

void step_open(int ms)
{
    // int target_rotate = 512 * ((double) angle / 360);
    for (STEP_LAST; STEP_LAST < STEP_MAX; STEP_LAST++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == 0)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 1)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 2)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            else if (j == 3)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            delay_ms(ms);
        }
    }
    step_clear();
}

void step_close(int ms)
{
    // int target_rotate = 512 * ((double) angle / 360);
    for (STEP_LAST; STEP_LAST > 0; STEP_LAST--)
    {
        for (int j = 3; j >= 0; j--)
        {
            if (j == 0)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 1)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 2)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            else if (j == 3)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            delay_ms(ms);
        }
    }
    step_clear();
}