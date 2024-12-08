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

#define EMERGENCY_SW 7
// test
int testSW = 8;

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
const int EMERGENCY_TIMER_TH = 10;
const int STEP_DELAY = 2;
//// value
int state = INIT;
bool isRegisterd = false;
int ship_timer, emergency_timer;

void WDOG_disable();
void init_sys();
void _port_init();
void nvic_init();
void show_system_ready();
void check_around();
// CAR 상태 사용 함수
bool is_ship_exist();
bool check_ship();
void set_ship_mode();

// SHIP 상태 사용 함수
void set_car_mode();

void set_emergency_mode();

int main(void) 
{
	init_sys();
	show_system_ready();
	set_car_mode();

	while (1)
	{
		switch (state)
		{
			case CAR:
				check_around();
				break;
			case SHIP:
				check_around();
				print_4_digit(ship_timer);
				if (ship_timer == 0)
				{
					set_car_mode();
				}
				break;
			case EMERGENCY:
				if (emergency_timer == 0)
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
	state = EMERGENCY;
	emergency_timer = EMERGENCY_TIMER_TH;
	// TODO: lcd
	// TODO: led
	// TODO: buzzer
}

void set_car_mode()
{
	state = CAR;
	led_car_mode();
	step_close(STEP_DELAY);
	servo_car_mode();
}

void set_ship_mode()
{
	state = SHIP;
	ship_timer = SHIP_TIMER_TH;
	led_ship_mode();
	step_open(STEP_DELAY);
	servo_ship_mode();
}

void show_system_ready()
{
	led_set_system_green(true);
	lcd_print_msg("System Ready!\0", "BRIDGE SYSTEM\0");
}

void check_around()
{
	if (is_ship_exist())
	{
		// isRegisterd = check_ship();
		if (isRegisterd)
		{
			if (state == CAR)
				set_ship_mode();
			else if (state == SHIP)
				ship_timer = SHIP_TIMER_TH;
		}
		else
		{
			if (state == CAR)
				buzzer_set(true);
		}
	}
	else
	{
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
	nvic_init();
	lcdinit();
	delay_ms(200);
}

void _port_init()
{
	/* PORTA */
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
	// 서보 모터
	PORTA->PCR[SERVO_CAR] = PORT_PCR_MUX(2);
	PORTA->PCR[SERVO_SHIP] = PORT_PCR_MUX(2);
	// 스텝 모터
	PORTA->PCR[STEP_IN_1] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_2] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_3] = PORT_PCR_MUX(1);
	PORTA->PCR[STEP_IN_4] = PORT_PCR_MUX(1);

	PTA->PDDR |= 1 << STEP_IN_1 | 1 << STEP_IN_2 | 1 << STEP_IN_3 | 1 << STEP_IN_4;
	/* PORTC */
	// 7-Segment
	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
	for (int i = 1; i <= 11; i++)
	{
		PORTC->PCR[i] = PORT_PCR_MUX(1);
		PTC->PDDR |= 1 << i;
	}
	/* PORTD */
	// LCD: 9 ~ 15
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
	// 부저: 1
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
	PORTE->PCR[EMERGENCY_SW] = PORT_PCR_MUX(1);

	PTE->PDDR |= 1 << EMERGENCY_SW;
	
	// 기타
	PORTE->PCR[testSW] = PORT_PCR_MUX(1);

	PTE->PDDR |= 1 << testSW;
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
	// ISF비트 0 설정
	PORTE->PCR[EMERGENCY_SW] &= ~(PORT_PCR_ISF_MASK);
	PORTE->PCR[testSW] &= ~(PORT_PCR_ISF_MASK);
	if ((PORTE->ISFR & (1 << testSW)) != 0)
	{
		reason = testSW;
		isRegisterd = !isRegisterd;
	}
	else if ((PORTE->ISFR & (1 << EMERGENCY_SW)) != 0)
	{
		reason = EMERGENCY_SW;
		set_emergency_mode();
	}
	// ISF비트 1 설정
	PORTE->PCR[testSW] |= PORT_PCR_ISF_MASK;
}

void LPIT0_Ch2_IRQHandler()
{
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;
	if (state == SHIP) ship_timer--;
	else if (state == EMERGENCY) emergency_timer--;
}

void nvic_init()
{
	// PORTE IRQ 초기화
	S32_NVIC->ICPR[PORTE_IRQn / 32] |= 1 << (PORTE_IRQn % 32);
	S32_NVIC->ISER[PORTE_IRQn / 32] |= 1 << (PORTE_IRQn % 32);
	S32_NVIC->IP[PORTE_IRQn] = 0x0;

	// LPIT0 IRQ
	S32_NVIC->ICPR[LPIT0_Ch2_IRQn / 32] |= 1 << (LPIT0_Ch2_IRQn % 32);
	S32_NVIC->ISER[LPIT0_Ch2_IRQn / 32] |= 1 << (LPIT0_Ch2_IRQn % 32);
	S32_NVIC->IP[LPIT0_Ch2_IRQn] = 0x1;
}