#include "device_registers.h"
#include "port.h"

void common_init()
{
    // 119: 7
	COMMON_PORTN->PCR[COMMON_EMERGENCY] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	COMMON_PTN->PDDR &= ~(1 << COMMON_EMERGENCY);
	
	// 테스트
	/*
	testSW: isRegistered반전 테스트
	*/
	COMMON_PORTN->PCR[testSW] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	COMMON_PTN->PDDR &= ~(1 << testSW);

	COMMON_PORTN->PCR[SYSTEM_STOP] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);
	COMMON_PTN->PDDR &= ~(1 << SYSTEM_STOP);
}