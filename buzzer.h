// #include "S32K144.h"
#include "device_registers.h"
#include "port.h"

void buzzer_init()
{
    BUZZER_PORTN->PCR[BUZZER] = PORT_PCR_MUX(1);
    BUZZER_PTN->PDDR |= 1 << BUZZER;
}

void buzzer_set(bool on)
{
    if (on)
        BUZZER_PTN->PSOR = 1 << BUZZER;
    else
        BUZZER_PTN->PCOR = 1 << BUZZER;
}

void buzzer_toggle()
{
    BUZZER_PTN->PTOR = 1 << BUZZER;
}