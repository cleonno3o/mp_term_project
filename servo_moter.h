#include "device_registers.h"
#include "clocks_and_modes.h"

// 서보 모터의 듀티 사이클 계산 함수
void set_servo_angle(int angle) {
    // 각도에 따른 듀티 사이클 값 계산 (0도 = 2000, 90도 = 4000)
    int pwm_value = 2000 + (angle * 2000) / 90;
    FTM0->CONTROLS[1].CnV = pwm_value;  // FTM0 채널 1에 듀티 사이클 설정
}

// // FTM0 PWM 초기화 함수
// void FTM0_PWM_Init(void) {
//     // FTM0 클럭 설정
//     PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;  // 클럭 비활성화
//     PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b010) // 8MHz SIRCDIV1_CLK 선택
//                               | PCC_PCCn_CGC_MASK;   // FTM0 클럭 활성화

//     // FTM0 초기화
//     FTM0->MODE |= FTM_MODE_WPDIS_MASK;  // Write Protection 비활성화
//     FTM0->SC = 0x00;                   // 타이머 비활성화
//     FTM0->CNTIN = 0x00;                // 카운터 초기값
//     FTM0->MOD = 40000 - 1;             // 20ms 주기 (50Hz)
//     FTM0->CONTROLS[1].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; // Edge-aligned PWM

//     // FTM0 클럭 소스 설정
//     FTM0->SC = FTM_SC_PS(7) | FTM_SC_CLKS(1);  // Prescaler = 128, System Clock 사용
// }

// // PORT 초기화 함수
// void PORT_Init(void) {
//     // PTA1 핀을 PWM 출력으로 설정
//     PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;  // PORTA 클럭 활성화
//     PORTA->PCR[1] = PORT_PCR_MUX(2);                 // PTA1을 FTM0_CH1로 설정
// }

// int main(void) {
//     WDOG_disable();            // Watchdog 비활성화
//     SOSC_init_8MHz();          // 시스템 클럭 초기화
//     SPLL_init_160MHz();
//     NormalRUNmode_80MHz();

//     PORT_Init();               // 포트 초기화
//     FTM0_PWM_Init();           // FTM0 PWM 초기화

//     while (1) {
//         set_servo_angle(0);    // 0도 설정
//         for (volatile int i = 0; i < 1000000; i++);  // 딜레이

//         set_servo_angle(90);   // 90도 설정
//         for (volatile int i = 0; i < 1000000; i++);  // 딜레이
//     }

//     return 0;
}
