// #include "S32K144.h"
#include "device_registers.h"

#define BUZZER 1
#define BUZZER_PTN PTE
void buzzer_set(bool on)
{
    if (on)
        BUZZER_PTN->PSOR = 1 << BUZZER;
    else
        BUZZER_PTN->PCOR = 1 << BUZZER;
}