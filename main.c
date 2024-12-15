#include "device_registers.h"
#include "clocks_and_modes.h"
//#include "S32K144.h"
// librarys
#include "port.h"
#include "segment.h"
#include "buzzer.h"
#include "step_moter.h"
//#include "lcd.h"
#include "led.h"
#include "servo_moter.h"
#include "lpit.h"
//#include "keypad.h"
#include "LPUART.h"
bool prev_car = true;
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
	bool system_stop;
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
	.system_stop = false,

	.SHIP_TIMER_TH = 10,
	.EMERGENCY_TIMER_TH = 5,
	.STEP_DELAY = 2
};

void WDOG_disable();
void init_sys();
void _port_init();
void nvic_init();
void show_system_ready();
void check_around();
// CAR 
bool is_ship_exist();
bool check_ship();
void set_ship_mode();

// SHIP
void set_car_mode();

void set_emergency_mode();

int main(void) 
{
	init_sys();
    led_set_car_green(false);
    led_set_car_red(false);
    led_set_ship_green(false);
    led_set_ship_red(false);
    show_system_ready();
	set_car_mode();
	while (1)
	{
		//if (system.system_stop) return 0;
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
				print_4_digit(system.emergency_timer);
				if (system.emergency_timer == 0)
				{
					set_car_mode();
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

void set_emergency_mode()
{
	system.state = EMERGENCY;
	system.emergency_timer = system.EMERGENCY_TIMER_TH;
	step_clear();
	servo_car_mode();
	step_close(system.STEP_DELAY);
    
	// TODO: lcd
	// TODO: led
	// TODO: buzzer
}

void set_car_mode()
{
	int prev_state = system.state;
	system.state = CAR;
	led_car_mode();
	if (prev_state != EMERGENCY)
		step_close(system.STEP_DELAY);
	servo_car_mode();
}

void set_ship_mode()
{
	system.state = SHIP;
	system.ship_timer = system.SHIP_TIMER_TH;
	led_ship_mode();
    buzzer_set(false);
	step_open(system.STEP_DELAY);
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
		// system.isRegisterd = check_ship();
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
	// TODO: 배가 지나가고 있거나 앞에있으면 있는 거임
	return true;
}

bool check_ship()
{
	return false;
	// while (system.from_raspberry_pi == BLANK) {}
	// if (system.from_raspberry_pi == VALID)
	// {
	// 	system.from_raspberry_pi = BLANK;
	// 	return true;
	// }
	// else if (system.from_raspberry_pi == INVALID)
	// {
	// 	system.from_raspberry_pi = BLANK;
	// 	return false;
	// }
}

void init_sys() 
{
	WDOG_disable();		   /* Disable Watchdog in case it is not done in startup code */
	SOSC_init_8MHz();	   /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();	   /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Initclocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	SystemCoreClockUpdate();
	delay_ms(20);
	_port_init();
//	lcdinit();
	delay_ms(200);
	ftm_servo_init();
	nvic_init();
}

void _port_init()
{
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;

	PCC->PCCn[PCC_PORTD_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_PCS(0x001);
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

	servo_init();
	step_init();
	segment_init();
	PCC->PCCn[PCC_FTM2_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM2_INDEX] |= (PCC_PCCn_PCS(1) | PCC_PCCn_CGC_MASK); // Clock = 80MHz

	buzzer_init();
	led_init();
    // 119: 7
	PORTE->PCR[EMERGENCY_SW] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	PTE->PDDR &= ~(1 << EMERGENCY_SW);
	
	// 테스트
	/*
	testSW: isRegistered반전 테스트
	*/
	PORTE->PCR[testSW] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	PTE->PDDR &= ~(1 << testSW);

	PORTE->PCR[SYSTEM_STOP] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	PTE->PDDR &= ~(1 << SYSTEM_STOP);
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
	// ISF비트 0 초기화
	PORTE->PCR[EMERGENCY_SW] &= ~(PORT_PCR_ISF_MASK);
	PORTE->PCR[testSW] &= ~(PORT_PCR_ISF_MASK);
	PORTE->PCR[SYSTEM_STOP] &= ~(PORT_PCR_ISF_MASK);
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
	else if ((PORTE->ISFR & (1 << SYSTEM_STOP)) != 0)
	{
		system.system_stop = true;
	}
	// ISF비트 1 초기화
	PORTE->PCR[EMERGENCY_SW] |= PORT_PCR_ISF_MASK;
	PORTE->PCR[testSW] |= PORT_PCR_ISF_MASK;
	PORTE->PCR[SYSTEM_STOP] |= PORT_PCR_ISF_MASK;
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
	{

		led_toggle_all();
//		buzzer_toggle();
	}
}

void LPUART1_RxTx_IRQHandler()
{
	if (LPUART1->STAT & LPUART_STAT_RDRF_MASK) {
        // 수신 데이터 처리
        system.from_raspberry_pi = LPUART1->DATA;
    }
    if (LPUART1->STAT & LPUART_STAT_TDRE_MASK) {
        // 송신 데이터 처리
        LPUART1->DATA = system.to_raspberry_pi;
    }
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

	S32_NVIC->ICPR[LPIT0_Ch3_IRQn / 32] |= 1 << (LPIT0_Ch3_IRQn % 32);
	S32_NVIC->ISER[LPIT0_Ch3_IRQn / 32] |= 1 << (LPIT0_Ch3_IRQn % 32);
	S32_NVIC->IP[LPIT0_Ch3_IRQn] = 0xB;

	// LPUART IRQ
	S32_NVIC->ICPR[LPUART1_RxTx_IRQn / 32] |= 1 << (LPUART1_RxTx_IRQn % 32);
	S32_NVIC->ISER[LPUART1_RxTx_IRQn / 32] |= 1 << (LPUART1_RxTx_IRQn % 32);
	S32_NVIC->IP[LPUART1_RxTx_IRQn] = 0xB;
}