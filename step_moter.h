#include "device_registers.h"
// #include "S32K144.h"
#include "lpit.h"

#define STEP_PTN PTA
#define STEP_IN_1 3
#define STEP_IN_2 4
#define STEP_IN_3 5
#define STEP_IN_4 6

extern int STEP_LAST = 0;
/* TODO:
STEP_LAST 부터 시작해서 target까지 움직이는 거로 
이러면 각도를 정하는게 아니라 끝을 그냥 0 ~ 180도가 맞는듯
*/

void _step_clear()
{
    STEP_PTN->PCOR = 1 << STEP_IN_1;
    STEP_PTN->PCOR = 1 << STEP_IN_2;
    STEP_PTN->PCOR = 1 << STEP_IN_3;
    STEP_PTN->PCOR = 1 << STEP_IN_4;
}

void step_open(int ms, int angle)
{
    int target_rotate = 512 * ((double) angle / 360);
    for (int i = 0; i < target_rotate; i++)
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
    _step_clear();
}

void step_close(int ms, int angle)
{
    int target_rotate = 512 * ((double) angle / 360);
    for (int i = 0; i < target_rotate; i++)
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
    _step_clear();
}