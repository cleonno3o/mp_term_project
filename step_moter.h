#include "S32K144.h"
#include "lpit0.h"

#define STEP_PTN PTA
#define STEP_IN_1 3
#define STEP_IN_2 4
#define STEP_IN_3 5
#define STEP_IN_4 6

void step_open(int ms)
{
    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; i < 4; i++)
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
}

void step_close(int ms)
{
    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; i < 4; i++)
        {
            if (j == 3)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 2)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PSOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PCOR = 1 << STEP_IN_4;
            }
            else if (j == 1)
            {
                STEP_PTN->PCOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PSOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            else if (j == 0)
            {
                STEP_PTN->PSOR = 1 << STEP_IN_1;
                STEP_PTN->PCOR = 1 << STEP_IN_2;
                STEP_PTN->PCOR = 1 << STEP_IN_3;
                STEP_PTN->PSOR = 1 << STEP_IN_4;
            }
            delay_ms(ms);
        }
    }
}