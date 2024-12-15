// #include "S32K144.h"
#include "device_registers.h"
#include "lpit.h"
#include "port.h"
#define KEYPAD_NONE '^'
#define KEYPAD_NONE_MODE -1
#define KEYPAD_REGISTER_MODE 1
#define KEYPAD_DELETE_MODE 2

volatile char input;
volatile int mode;

void keypad_init()
{
    KEYPAD_PORTN->PCR[ROW_1] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[ROW_2] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[ROW_3] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[ROW_4] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[COL_1] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[COL_2] = PORT_PCR_MUX(1);
    KEYPAD_PORTN->PCR[COL_3] = PORT_PCR_MUX(1);

    KEYPAD_PTN->PDDR |= 1 << COL_1 | 1 << COL_2 | 1 << COL_3;
    KEYPAD_PTN->PDDR &= ~(1 << ROW_1);
    KEYPAD_PTN->PDDR &= ~(1 << ROW_2);
    KEYPAD_PTN->PDDR &= ~(1 << ROW_3);
    KEYPAD_PTN->PDDR &= ~(1 << ROW_4);
    input = KEYPAD_NONE;
    mode = KEYPAD_NONE_MODE;
}

char key_scan(void)
{
    int Dtime = 10000;
    char value = KEYPAD_NONE;

    KEYPAD_PTN->PSOR |= 1 << COL_1;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        value = '1'; // 1
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        value = '2'; // 4
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        value = '7'; // 7
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        value = '*'; //*
    KEYPAD_PTN->PCOR |= 1 << COL_1;

    KEYPAD_PTN->PSOR |= 1 << COL_2;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        value = '2';
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        value = '5';
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        value = '8';
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        value = '0';
    KEYPAD_PTN->PCOR |= 1 << COL_2;

    KEYPAD_PTN->PSOR |= 1 << COL_3;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        value = '3';
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        value = '6';
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        value = '9';
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        value = '#';
    KEYPAD_PTN->PCOR |= 1 << COL_3;

    return value;
}

int keypad_get_mode()
{
    if (mode == KEYPAD_NONE_MODE) 
    {
        char tmp = key_scan();
        if (tmp == '#') mode = KEYPAD_REGISTER_MODE;
        else if (tmp == '*') mode = KEYPAD_DELETE_MODE;
        else input = KEYPAD_NONE_MODE;
    }
    return mode;
}

int keypad_get_input()
{
    if (input == KEYPAD_NONE) 
    {
        char tmp = key_scan();
        if (tmp >= '0' && tmp <= '9') input = tmp;
        else input = KEYPAD_NONE;
    }
    return input;
}

void keypad_clear()
{
    input = KEYPAD_NONE;
    mode = KEYPAD_NONE_MODE;
}