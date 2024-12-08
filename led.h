#include "S32K144.h"
#define LED_PTN PTE
#define LED_SYSTEM_GREEN 2
#define LED_CAR_GREEN 3
#define LED_CAR_RED 4
#define LED_SHIP_GREEN 5
#define LED_SHIP_RED 6

void led_set_system_green(bool on)
{
    if (on)
        LED_PTN->PSOR = 1 << LED_SYSTEM_GREEN;
    else
        LED_PTN->PCOR = 1 << LED_SYSTEM_GREEN;
}

void led_set_car_green(bool on)
{
    if (on)
        LED_PTN->PSOR = 1 << LED_CAR_GREEN;
    else
        LED_PTN->PCOR = 1 << LED_CAR_GREEN;
}

void led_set_car_red(bool on)
{
    if (on)
        LED_PTN->PSOR = 1 << LED_CAR_RED;
    else
        LED_PTN->PCOR = 1 << LED_CAR_RED;
}

void led_set_ship_green(bool on)
{
    if (on)
        LED_PTN->PSOR = 1 << LED_SHIP_GREEN;
    else
        LED_PTN->PCOR = 1 << LED_SHIP_GREEN;
}

void led_set_ship_red(bool on)
{
    if (on)
        LED_PTN->PSOR = 1 << LED_SHIP_RED;
    else
        LED_PTN->PCOR = 1 << LED_SHIP_RED;
}

void led_car_mode()
{
    led_set_car_green(true);
    led_set_car_red(false);
    led_set_ship_green(false);
    led_set_ship_red(true);
}

void led_ship_mode()
{
    led_set_car_green(false);
    led_set_car_red(true);
    led_set_ship_green(true);
    led_set_ship_red(false);
}

void led_emergency_mode()
{
    led_set_car_red(true);
    led_set_car_green(false);
    led_set_ship_red(true);
    led_set_ship_green(false);
}

void led_toggle_all()
{
    LED_PTN->PTOR |= 1 << LED_CAR_RED |
                    1 << LED_CAR_GREEN |
                    1 << LED_SHIP_RED |
                    1 << LED_SHIP_GREEN;
}