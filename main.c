#include "device_registers.h"
#include "clocks_and_modes.h"
#include "S32K144.h"
// librarys
#include "segment.h"
#include "buzzer.h"
//#include "step_moter.h"
//#include "lcd.h"
#include "led.h"
#include "servo_moter.h"
#include "lpit0.h"
//#include "LPUART.h"

#define EMERGENCY_SW 7

// test
int testSW = 8;

enum STATE
{
    INIT = 0,
	CAR,
    SHIP,
    EMERGENCY
};

enum RESULT
{
	BLANK = 'B',
	VALID = 'V',
	INVALID = 'I'
};

enum AROUND
{
	NOT_EXIST = 'N',
	EXIST = 'Y'
};

// system
typedef struct {
	//// value
	int state;
	bool isRegisterd;
	int ship_timer, emergency_timer;
	char from_raspberry_pi;
	char to_raspberry_pi;
	//// constant
	const int SHIP_TIMER_TH;
	const int EMERGENCY_TIMER_TH;
	const int STEP_DELAY;
} System;

System system = {
	.state = INIT,
	.isRegisterd = false,
	.ship_timer = 0, 
	.emergency_timer = 0,
	.from_raspberry_pi = BLANK,
	.to_raspberry_pi = NOT_EXIST,
	
	.SHIP_TIMER_TH = 5,
	.EMERGENCY_TIMER_TH = 5,
	.STEP_DELAY = 2
};

void WDOG_disable();
void init_sys();
void _port_init();
void nvic_init();
void show_system_ready();
void check_around();
// CAR ���� ��� �Լ�
bool is_ship_exist();
bool check_ship();
void set_ship_mode();

// SHIP ���� ��� �Լ�
void set_car_mode();

void set_emergency_mode();
int main(void) 
{
	init_sys();
    led_set_car_green(true);
    led_set_car_red(true);
    led_set_ship_green(true);
    led_set_ship_red(true);
	show_system_ready();
	set_car_mode();
    buzzer_set(true);

	while (1)
	{
		switch (system.state)
		{
			case CAR:
				check_around();
				break;
			case SHIP:
				check_around();
				print_4_digit(system.ship_timer);
				if (system.ship_timer == 0)
				{
					set_car_mode();
				}
				break;
			case EMERGENCY:
				if (system.emergency_timer == 0)
				{
					set_car_mode();
				}
				break;
			default:
				break;
		}
	}
}

void set_emergency_mode()
{
	system.state = EMERGENCY;
	system.emergency_timer = system.EMERGENCY_TIMER_TH;
	// TODO: lcd
	// TODO: led
	// TODO: buzzer
}

void set_car_mode()
{
	int prev_state = system.state;
	system.state = CAR;
	led_car_mode();
//	if (prev_state != EMERGENCY)
//		step_close(system.STEP_DELAY);
	servo_car_mode();
}

void set_ship_mode()
{
	system.state = SHIP;
	system.ship_timer = system.SHIP_TIMER_TH;
	led_ship_mode();
    buzzer_set(false);
//	step_open(system.STEP_DELAY);
	servo_ship_mode();
}

void show_system_ready()
{
	led_set_system_green(true);
//	lcd_print_msg("System Ready!\0", "BRIDGE SYSTEM\0");
}

void check_around()
{
	if (is_ship_exist())
	{
		system.to_raspberry_pi = EXIST;
		// isRegisterd = check_ship();
        // temp
        //if (system.state == SHIP) system.isRegisterd = false;
		if (system.isRegisterd)
		{
			if (system.state == CAR)
				set_ship_mode();
			else if (system.state == SHIP)
				system.ship_timer = system.SHIP_TIMER_TH;
		}
		else
		{
			if (system.state == CAR)
            {
                buzzer_set(true);
                //system.isRegisterd = true;
            }

		}
	}
	else
	{
		system.to_raspberry_pi = NOT_EXIST;
		buzzer_set(false);
	}
}

bool is_ship_exist()
{
	// TODO: �谡 �������� �ְų� �տ������� �ִ� ����
	return true;
}

bool check_ship()
{
	return false;
}

void init_sys() 
{
	WDOG_disable();		   /* Disable Watchdog in case it is not done in startup code */
	SOSC_init_8MHz();	   /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();	   /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Initclocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	SystemCoreClockUpdate();
//	delay_ms(20);
	_port_init();
    ftm_servo_init();
	nvic_init();
//	lcdinit();
//	delay_ms(200);
}

void _port_init()
{
    /* PORTA */
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
	// ���� ����
	PORTA->PCR[SERVO_CAR] = PORT_PCR_MUX(2);
	PORTA->PCR[SERVO_SHIP] = PORT_PCR_MUX(2);
    /* PORTC */
	// 7-Segment
	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
	for (int i = 1; i <= 11; i++)
	{
		PORTC->PCR[i] = PORT_PCR_MUX(1);
		PTC->PDDR |= 1 << i;
	}
	/* PORTE */
	PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;
    // ����: 1
	PORTE->PCR[BUZZER] = PORT_PCR_MUX(1);
	PTE->PDDR |= 1 << BUZZER;
	// LED: 2 ~ 6
	PORTE->PCR[LED_SYSTEM_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_CAR_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_CAR_RED] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_SHIP_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_SHIP_RED] = PORT_PCR_MUX(1);

	PTE->PDDR |= 1 << LED_SYSTEM_GREEN | 
				1 << LED_CAR_GREEN | 1 << LED_CAR_RED | 
				1 << LED_SHIP_GREEN | 1 << LED_SHIP_RED;
    // 119: 7
	PORTE->PCR[EMERGENCY_SW] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);

	PTE->PDDR &= ~(1 << EMERGENCY_SW);
	
	// ��Ÿ
	PORTE->PCR[testSW] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);

	PTE->PDDR &= ~(1 << testSW);
}

void WDOG_disable(void) 
{
	WDOG->CNT = 0xD928C520;	  /* Unlock watchdog 		*/
	WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value 	*/
	WDOG->CS = 0x00002100;	  /* Disable watchdog 		*/
}

// Interrupts
void PORTE_IRQHandler()
{
	int reason = -1;
	// ISF��Ʈ 0 ����
	PORTE->PCR[EMERGENCY_SW] &= ~(PORT_PCR_ISF_MASK);
	PORTE->PCR[testSW] &= ~(PORT_PCR_ISF_MASK);
	if ((PORTE->ISFR & (1 << testSW)) != 0)
	{
		reason = testSW;
		system.isRegisterd = !system.isRegisterd;
	}
	else if ((PORTE->ISFR & (1 << EMERGENCY_SW)) != 0)
	{
		reason = EMERGENCY_SW;
		set_emergency_mode();
	}
	// ISF��Ʈ 1 ����
	PORTE->PCR[testSW] |= PORT_PCR_ISF_MASK;
}

void LPIT0_Ch2_IRQHandler()
{
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;
	if (system.state == SHIP) system.ship_timer--;
	else if (system.state == EMERGENCY) system.emergency_timer--;
}

void LPIT0_Ch3_IRQHandler()
{
	LPIT0->MSR |= LPIT_MSR_TIF3_MASK;
	if (system.state == EMERGENCY)
		led_toggle_all();
}

void nvic_init()
{
	// PORTE IRQ
	S32_NVIC->ICPR[PORTE_IRQn / 32] |= 1 << (PORTE_IRQn % 32);
	S32_NVIC->ISER[PORTE_IRQn / 32] |= 1 << (PORTE_IRQn % 32);
	S32_NVIC->IP[PORTE_IRQn] = 0x0A;
	
	// LPIT0 IRQ
	S32_NVIC->ICPR[LPIT0_Ch2_IRQn / 32] |= 1 << (LPIT0_Ch2_IRQn % 32);
	S32_NVIC->ISER[LPIT0_Ch2_IRQn / 32] |= 1 << (LPIT0_Ch2_IRQn % 32);
	S32_NVIC->IP[LPIT0_Ch2_IRQn] = 0xB;
}