// #include "S32K144.h"
#include "device_registers.h"
#include "lpit.h"
#define KEYPAD_PTN PTB
#define COL_1 12
#define COL_2 14
#define COL_3 15
#define ROW_1 0
#define ROW_2 1
#define ROW_3 2
#define ROW_4 3
#define KEYPAD_NONE '^'

char KeyScan(void)
{
    int Dtime = 10000;
    char Kbuff = KEYPAD_NONE;

    KEYPAD_PTN->PSOR |= 1 << COL_1;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        Kbuff = '1'; // 1
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        Kbuff = '2'; // 4
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        Kbuff = '7'; // 7
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        Kbuff = '*'; //*
    KEYPAD_PTN->PCOR |= 1 << COL_1;

    KEYPAD_PTN->PSOR |= 1 << COL_2;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        Kbuff = '2';
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        Kbuff = '5';
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        Kbuff = '8';
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        Kbuff = '0';
    KEYPAD_PTN->PCOR |= 1 << COL_2;

    KEYPAD_PTN->PSOR |= 1 << COL_3;
    delay_100ns(Dtime);
    if (KEYPAD_PTN->PDIR & (1 << ROW_1))
        Kbuff = '3';
    if (KEYPAD_PTN->PDIR & (1 << ROW_2))
        Kbuff = '6';
    if (KEYPAD_PTN->PDIR & (1 << ROW_3))
        Kbuff = '9';
    if (KEYPAD_PTN->PDIR & (1 << ROW_4))
        Kbuff = '#';
    KEYPAD_PTN->PCOR |= 1 << COL_3;

    return Kbuff;
}