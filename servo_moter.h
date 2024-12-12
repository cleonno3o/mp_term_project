// #include "S32K144.h"
#include "device_registers.h"
#include "lpit.h"
#include "port.h"
#define SERVO_DELAY 1000

void servo_init()
{
    SERVO_PORTN->PCR[SERVO_CAR] = PORT_PCR_MUX(2);
    SERVO_PORTN->PCR[SERVO_SHIP] = PORT_PCR_MUX(2);
}

// 서보 모터의 듀티 사이클 계산 함수
void servo_set_angle(int angle, int servo_no) {
    // 각도에 따른 듀티 사이클 값 계산 (0도 = 1000, 90도 = 2000)
    int pwm_value = 1000 + ((angle * 1000) / 180);
    if (servo_no == SERVO_CAR)
        FTM_SERVO->CONTROLS[FTM_SERVO_CAR_CH].CnV = pwm_value;
    else
        FTM_SERVO->CONTROLS[FTM_SERVO_SHIP_CH].CnV = pwm_value;
    FTM_SERVO->SC |= FTM_SC_CLKS(3);
    delay_ms(SERVO_DELAY);
}

void servo_car_mode()
{
    servo_set_angle(90, SERVO_CAR);
    servo_set_angle(0, SERVO_SHIP);
}

void servo_ship_mode()
{
    servo_set_angle(0, SERVO_CAR);
    servo_set_angle(90, SERVO_SHIP);
}

// FTM0 PWM 초기화 함수
void ftm_servo_init(void) {
    // FTM0 클럭 설정
    PCC->PCCn[FTM_SERVO_PCC_INDEX]  &= ~PCC_PCCn_CGC_MASK; // 클럭 비활성화
    PCC->PCCn[FTM_SERVO_PCC_INDEX]  |= PCC_PCCn_PCS(0b010) // 8MHz SIRCDIV1_CLK 선택
                                    | PCC_PCCn_CGC_MASK; // FTM0 클럭 활성화

    FTM_SERVO->CNTIN = 0x00;           // 카운터 초기값
    FTM_SERVO->MOD = 20000 - 1;        // 20ms 주기 (50Hz)
    FTM_SERVO->CONTROLS[FTM_SERVO_CAR_CH].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; // Edge-aligned PWM
    FTM_SERVO->CONTROLS[FTM_SERVO_SHIP_CH].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;

    // 8로 분주, PWM1활성화
    FTM_SERVO->SC |= FTM_SC_PS(3) | FTM_SC_PWMEN6_MASK | FTM_SC_PWMEN7_MASK;
}