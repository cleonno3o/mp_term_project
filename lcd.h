
#include "lpit0.h"

#define BASE 9
#define EN 4
#define RW 5
#define RS 6
#define BF 3

void lcdEN();
void lcdNEN();
void lcdinput(uint16_t);
void lcdinit();

void lcdEN(void)
{
    PTD->PSOR |= 1 << (BASE + EN);
}

void lcdNEN(void)
{
    PTD->PCOR |= 1 << (BASE + EN);
}

void lcdinit(void)
{

    lcdinput(0x28); // lcd function set #1
    delay_100ns(70000);

    lcdinput(0x28); // lcd function set #2
    delay_100ns(70000);

    lcdinput(0x28); // lcd function set #3
    delay_100ns(70000);

    lcdinput(0x08); // lcd off
    delay_100ns(700);

    lcdinput(0x01); // lcd clear
    delay_100ns(20000);

    lcdinput(0x06); // lcd modeset
    delay_100ns(700);

    lcdinput(0x0F); // lcd display,cursor
    delay_100ns(20000);

    lcdinput(0x80); // dd ram address
    delay_100ns(700);

    lcdinput(0x02); // return home
    delay_100ns(20000);
}

void lcdinput(uint16_t data)
{
    uint16_t data1 = ((uint16_t)data & 0xF0) << (BASE - 4); // D7 ~ 4
    uint16_t data2 = (((uint16_t)data & 0x0F) << BASE);     // D3 ~ 0
    PTD->PCOR |= 1 << (BASE + RW);

    // data1
    lcdEN();
    PTD->PCOR |= 0xF << BASE;
    PTD->PSOR |= data1;
    delay_100ns(5);

    lcdNEN();

    // clear before 2nd transmission
    delay_100ns(2);
    PTD->PCOR |= 0xF << BASE;
    delay_100ns(30);

    // data2
    lcdEN();
    PTD->PCOR |= 0xF << BASE;
    PTD->PSOR |= data2;
    delay_100ns(5);

    lcdNEN();

    // clear before next order
    delay_100ns(2);
    PTD->PCOR |= 0xF << BASE;
    PTD->PCOR |= ((1 << (BASE + RS)) + (0xF << BASE));
    delay_100ns(30);
}

void lcdcharinput(char data)
{
    uint16_t data1 = ((uint16_t)data & 0xF0) << (BASE - 4); // D7 ~ 4
    uint16_t data2 = (((uint16_t)data & 0x0F) << BASE);     // D3 ~ 0

    PTD->PSOR |= 1 << (BASE + RS);
    // data1
    lcdEN();
    PTD->PCOR |= (0xF << BASE);
    PTD->PSOR |= data1;
    delay_100ns(3);

    lcdNEN();

    // clear before 2nd transmission
    delay_100ns(2);
    PTD->PCOR |= 0xF << BASE;
    delay_100ns(30);

    // data2
    lcdEN();
    PTD->PCOR |= 0xF << BASE;
    PTD->PSOR |= data2;
    delay_100ns(30);

    lcdNEN();

    // clear before next order
    delay_100ns(2);
    PTD->PCOR |= 0xF << BASE;
    PTD->PCOR |= ((1 << (BASE + RS)) + (0xF << BASE));
    delay_100ns(30);
}

void lcd_print_msg(char *msg1, char *msg2)
{
    int i = 0;
    // lcdinput(0x80);
    while(msg1[i] != '\0')
    {
        lcdcharinput(msg1[i]);
        delay_ms(800);
        i++;
    }

    lcdinput(0x80 + 0x40);
    i = 0;
    while (msg2[i] != '\0')
    {
        lcdcharinput(msg2[i]);
        delay_ms(800);
        i++;
    }

    delay_ms(2000);
    lcdinput(0x08); // lcd display off
    delay_ms(400);
    lcdinput(0x01); // Clear display
    delay_ms(200);
}