/*
 * LPUART.c              Copyright NXP 2016
 * Description: LPUART functions
 * 2015 Sept 28 Kushal Shaw - original version AN5213;
 * 2016 Mar 17  O Romero - ported to S32K144;
 *
 */

//#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "S32K144.h"

void LPUART1_init(void)  /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
    PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;    /* Ensure clk disabled for config */
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0x001)    /* Clock Src= 1 (SOSCDIV2_CLK) */
                               |  PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */

    LPUART1->BAUD = 0x0F000034;  /* Initialize for 9600 baud, 1 stop: */

    LPUART1->CTRL=0x000C0000;    /* Enable transmitter & receiver, no parity, 8 bit char: */
}

void LPUART1_transmit_char(char send) {    /* Function to Transmit single Char */
    while((LPUART1->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
                                   /* Wait for transmit buffer to be empty */
    LPUART1->DATA=send;              /* Send data */
}

void LPUART1_transmit_string(char data_string[])  {  /* Function to Transmit whole string */
    uint32_t i=0;
    while(data_string[i] != '\0')  
    {           /* Send chars one at a time */
        LPUART1_transmit_char(data_string[i]);
        i++;
    }
}

char LPUART1_receive_char(void) {    /* Function to Receive single Char */
    static char receive;
    while((LPUART1->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
                                     /* Wait for received buffer to be full */
    receive = LPUART1->DATA;            /* Read received data*/
    return receive;
}

char LPUART1_receive_and_echo_char(void)  {  /* Function to echo received char back */
    char send = LPUART1_receive_char();        /* Receive Char */
    LPUART1_transmit_char(send);               /* Transmit same char back to the sender */
    LPUART1_transmit_char('\n');               /* New line */
    LPUART1_transmit_char('\r');               /* Return */
    return  send;
}
