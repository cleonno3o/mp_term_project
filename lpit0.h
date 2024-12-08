#include "S32K144.h"
#ifndef LPIT0_H_
#define LPIT0_H_
void LPIT0_init(uint32_t delay)
{
    uint32_t timeout;
    /*!
     * LPIT Clocking:
     * ==============================
     */
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_EN-0: Timer chans stop in Debug mode */

    // Ch 0: delayms, Ch 1: lcd timer, Ch2: 1sec interrupt, Ch3: 0.5 sec intterrupt
    timeout = delay * 40000;
    LPIT0->TMR[0].TVAL = timeout;
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;

    LPIT0->TMR[2].TVAL = 40000000;
    LPIT0->TMR[2].TCTRL = LPIT_TMR_TCTRL_T_EN_MASK;

    LPIT0->TMR[3].TVAL = 20000000;
    LPIT0->TMR[3].TCTRL = LPIT_TMR_TCTRL_T_EN_MASK;
}

void delay_ms(volatile int ms)
{
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
    LPIT0_init(ms);                   /* Initialize PIT0 for 1 second timeout  */
    while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))
    {
    } /* Wait for LPIT0 CH0 Flag */
}

void delay_100ns(uint32_t ns)
{
    uint32_t timeout_ns;
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    // Clock Src = 6 (SPLL2_DIV2_CLK)
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; // Enable clk to LPIT0 regs

    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

    timeout_ns = ns * 4;

    LPIT0->TMR[1].TVAL = timeout_ns;
    LPIT0->TMR[1].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;

    while (0 == (LPIT0->MSR & LPIT_MSR_TIF1_MASK))
    {
    } /* Wait for LPIT0 CH0 Flag */
    LPIT0->MSR |= LPIT_MSR_TIF1_MASK; /* Clear LPIT0 timer flag 0 */
}
#endif