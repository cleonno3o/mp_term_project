#include "device_registers.h"
#include "clocks_and_modes.h"
// librarys
#include "segment.h"
#include "buzzer.h"
#include "step_moter.h"
#include "lcd.h"
#include "servo_moter.h"

enum STATE
{
    IDLE = 0,
    CAR,
    SHIP,
    EMERGENCY
};

int state = IDLE;

void WDOG_disable();
void LPIT0_init(uint32_t delay);
void delay_ms(volatile int ms);
void init_sys();

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
    switch (state)
	{
	case IDLE:
		while (1) 
		{
			for (int i = 0; i < 4; i++)
			{
				set_num(&segment, i, i);
			}
		}
		break;
	case CAR:
		break;

	case SHIP:
		break;
	case EMERGENCY:
		break;
	default:
		break;
	}
}

void init_sys() 
{
	WDOG_disable();		   /* Disable Watchdog in case it is not done in startup code */
	SOSC_init_8MHz();	   /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();	   /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
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
	LPIT0_init(ms); /* Initialize PIT0 for 1 second timeout  */
	while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))
	{
	}								  /* Wait for LPIT0 CH0 Flag */
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}