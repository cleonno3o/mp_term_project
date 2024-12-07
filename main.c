#include "device_registers.h"
#include "clocks_and_modes.h"
// librarys
#include "segment.h"
#include "buzzer.h"
#include "step_moter.h"
#include "lcd.h"
#include "led.h"
#include "servo_moter.h"

enum STATE
{
    INIT = 0,
	IDLE,
    CAR,
    SHIP,
    EMERGENCY
};
// system
int state = INIT;
bool isRegisterd = false;
const int SHIP_TIMER = 30;

void WDOG_disable();
void LPIT0_init(uint32_t delay);
void delay_ms(volatile int ms);
void init_sys();
void _port_init();

bool is_ship_waiting();
bool check_ship();
int main(void) 
{
	init_sys();
	Segment segment;
	segment_init(
		&segment,
		PORTC,
		PTC,
		PCC_PORTC_INDEX);
	segment.delay_ms = delay_ms;

	led_set_system_green(true);
	state = IDLE;

	while (1)
	{
		switch (state)
		{
			case IDLE:
				led_car_mode();
				if (is_ship_waiting())
				{
					if (check_ship())
					{
						state = SHIP;
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
				led_ship_mode();
				// 타이머 시작
				// 만약 0이면 IDLE로 복귀
				break;
			case EMERGENCY:
				// 
				break;
			default:
				break;
		}
	}
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

void LPIT0_init(uint32_t delay)
{
	uint32_t timeout;
	/*!
	 * LPIT Clocking:
	 * ==============================
	 */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);	/* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

	/*!
	 * LPIT Initialization:
	 */
	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_EN-0: Timer chans stop in Debug mode */
									   /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
									   /* SW_RST=0: SW reset does not reset timer chans, regs */
									   /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

	timeout = delay * 40000;
	LPIT0->TMR[0].TVAL = timeout;
	LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
	/* T_EN=1: Timer channel is enabled */
	/* CHAIN=0: channel chaining is disabled */
	/* MODE=0: 32 periodic counter mode */
	/* TSOT=0: Timer decrements immediately based on restart */
	/* TSOI=0: Timer does not stop after timeout */
	/* TROT=0 Timer will not reload on trigger */
	/* TRG_SRC=0: External trigger soruce */
	/* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void delay_ms(volatile int ms)
{
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
	LPIT0_init(ms); /* Initialize PIT0 for 1 second timeout  */
	while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))
	{
	}								  /* Wait for LPIT0 CH0 Flag */
}