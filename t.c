#include "device_registers.h"
#include "clocks_and_modes.h"

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
unsigned int fnd_data[10]={0x107C, 0x0C,0x10B4,0x109C,0xCC,0x10D8,0x10F8,0x104C,0x10FC,0x10DC};
unsigned int fnd_sel[4]={0x0100,0x0200,0x0400,0x0800};
unsigned int num=0;
unsigned int External_PIN=0; /* External_PIN:SW External input Assignment */

int segment[] = {12, 2, 3, 4, 5, 6, 7};
int com[] = {8, 9, 10, 11};
int inter[] = {11, 12, 13, 16, 17};
void PORT_init (void)
{
	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
	PTD->PDDR |= 1 << 15;
	PORTD->PCR[15] = PORT_PCR_MUX(1);
}
void WDOG_disable (void)
{
  WDOG->CNT = 0xD928C520;     /* Unlock watchdog 		*/
  WDOG->TOVAL = 0x0000FFFF;   /* Maximum timeout value 	*/
  WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}

void LPIT0_init (uint32_t delay)
{
	uint32_t timeout;
   /*!
    * LPIT Clocking:
    * ==============================
    */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

  /*!
   * LPIT Initialization:
   */
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay* 40000;
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
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

void delay_ms (volatile int ms){
   LPIT0_init(ms);           /* Initialize PIT0 for 1 second timeout  */
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
	lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void NVIC_init_IRQs(void){
	
	int PORTC_NVIC_IDX = PORTC_IRQn / 32;
	S32_NVIC->ICPR[PORTC_NVIC_IDX] |= 1<<(PORTC_IRQn % 32); // Clear any pending IRQ61
	S32_NVIC->ISER[PORTC_NVIC_IDX] |= 1<<(PORTC_IRQn % 32); // Enable IRQ61
	S32_NVIC->IP[PORTC_IRQn] =0xB; //Priority 11 of 15
}

void PORTC_IRQHandler(void){
	// Port Control Register ISF bit '0' set
	for (int i = 0; i < (int) sizeof(inter) / (int) sizeof(int); i++)
		PORTC->PCR[inter[i]] &= ~(PORT_PCR_ISF_MASK); 

	// find interrupt pin
	for (int i = 0; i < (int) sizeof(inter) / (int) sizeof(int); i++)
	{
		if (PORTC->ISFR & (1<<inter[i]) != 0)
		{
			External_PIN = i + 1;
			break;
		}
	}

	// calculate
	switch (External_PIN){
		case 1:
			num += 1;
			break;
		case 2:
			num += 2;
			break;
		case 3:
			num += 3;
			break;
		case 4:
			num *= 2;
			break;
		case 5:
			num /= 2;
			break;
		default:
			break;
	}
	External_PIN=0;
	
	// Port Control Register ISF bit '1' set
	for (int i = 0; i < (int) sizeof(inter) / (int) sizeof(int); i++)
		PORTC->PCR[inter[i]] |= (PORT_PCR_ISF_MASK); 
}

void setNum(int num, int pos)
{
    PTD->PSOR = fnd_sel[pos];	// ouput com port select
    PTD->PCOR = 0x7F;			// clear data output
    PTD->PSOR = fnd_data[num];	// set data output
    delay_ms(2);
    PTD->PCOR = 0x1FFF;			// clear all
}

int main(void)
{

	WDOG_disable();/* Disable Watchdog in case it is not done in startup code */
	PORT_init();            /* Configure ports */
	SOSC_init_8MHz();        /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	NVIC_init_IRQs(); /*Interrupt Pending, Endable, Priority Set*/
	while(1)
	{
		PTD->PCOR = 1 << 15;
		delay_ms(10);
		PTD->PSOR = 1 << 15;
	}
}
