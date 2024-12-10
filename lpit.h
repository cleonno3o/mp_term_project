#ifndef LPIT0_H_
#define LPIT0_H_
void lpit_init(uint32_t delay);
void LPIT0_init(uint32_t delay);
void delay_ms(volatile int ms);
void delay_100ns(uint32_t ns);
void delay_us(volatile int us);
#endif