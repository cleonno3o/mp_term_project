#include "device_registers.h"
#include "clocks_and_modes.h"
// librarys
#include "segment.h"
#include "buzzer.h"
#include "step_moter.h"
#include "lcd.h"
#include "led.h"
#include "servo_moter.h"
#include "lpit0.h"

enum STATE
{
    INIT = 0,
	CAR,
    CAR,
    SHIP,
    EMERGENCY
};
// system
//// constant
const int SHIP_TIMER_TH = 30;
const int STEP_DELAY = 2;
//// value
int state = INIT;
bool isRegisterd = false;
int ship_timer;

void WDOG_disable();
void init_sys();
void _port_init();

// CAR 상태 사용 함수
bool is_ship_waiting();
bool check_ship();
void set_ship_mode();

// SHIP 상태 사용 함수
void set_car_mode();

void set_emergency_mode();

int main(void) 
{
	init_sys();
	Segment segment;
	segment_init(
		&segment,
		PORTC,
		PTC,
		PCC_PORTC_INDEX);

	led_set_system_green(true);
	set_car_mode();

	while (1)
	{
		switch (state)
		{
			case CAR:
				if (is_ship_waiting())
				{
					if (check_ship())
					{
						set_ship_mode();
					}
					else
					{
						buzzer_set(true);
					}
				}
				else
				{
					buzzer_set(false);
				}
				break;
			case SHIP:
				if (ship_timer == 0)
				{
					set_car_mode();
				}
				break;
			case EMERGENCY:
				// 
				break;
			default:
				break;
		}
	}
}

void set_emergency_mode()
{

}

void set_car_mode()
{
	state = CAR;
	led_car_mode();
	step_close(STEP_DELAY);
}

void set_ship_mode()
{
	state = SHIP;
	ship_timer = SHIP_TIMER_TH;
	led_ship_mode();
	step_open(STEP_DELAY);
}

bool is_ship_waiting()
{
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
	delay_ms(20);
	_port_init();
	lcdinit();
	delay_ms(200);
}

void _port_init()
{
	/* PORTA */
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
	// 서보 모터
	
	// 스텝 모터
	PORTA->PCR[STEP_IN_1] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_2] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_3] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_4] = PORT_PCR_MUX(1);
	/* PORTC */
	// 7-Segment
	
	/* PORTD */
	// LCD
	PCC->PCCn[PCC_PORTD_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_PCS(0x001);
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

	PCC->PCCn[PCC_FTM2_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM2_INDEX] |= (PCC_PCCn_PCS(1) | PCC_PCCn_CGC_MASK); // Clock = 80MHz

	PTD->PDDR |= 0xFE00;
	for (int i = 9; i <= 15; i++)
		PORTD->PCR[i] = PORT_PCR_MUX(1);
	
	/* PORTE */
	PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;
	// 부저
	PORTE->PCR[BUZZER] = PORT_PCR_MUX(1);
	// LED
	PORTE->PCR[LED_SYSTEM_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_CAR_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_CAR_RED] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_SHIP_GREEN] = PORT_PCR_MUX(1);
	PORTE->PCR[LED_SHIP_RED] = PORT_PCR_MUX(1);
}

void WDOG_disable(void) 
{
	WDOG->CNT = 0xD928C520;	  /* Unlock watchdog 		*/
	WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value 	*/
	WDOG->CS = 0x00002100;	  /* Disable watchdog 		*/
}

// Interrupts
void LPIT0_Ch2_IRQHandler()
{
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;
	if (state == SHIP) ship_timer--;
}