#ifndef PORT_H_
#define PORT_H_

#define SERVO_PTN PTA
#define SERVO_PORTN PORTA

#define STEP_PTN PTA
#define STEP_PORTN PORTA

#define KEYPAD_PTN PTB
#define KEYPAD_PORTN PORTB

#define SEGMENT_PTN PTD
#define SEGMENT_PORTN PORTD

#define BUZZER_PTN PTE
#define BUZZER_PORTN PORTE

#define LED_PTN PTE
#define LED_PORTN PORTE

// A
// TODO: 12로 고쳐보기 -> PTN, FTM, PCC, CONTROLS 번호 모두 변경적용
// TODO: 최적 DELAY 확인
#define SERVO_CAR 12
#define SERVO_SHIP 13

#define STEP_IN_1 0
#define STEP_IN_2 1
#define STEP_IN_3 2
#define STEP_IN_4 3
// B
#define COL_1 12
#define COL_2 14
#define COL_3 15
#define ROW_1 0
#define ROW_2 1
#define ROW_3 2
#define ROW_4 3
// C

// D

// E
#define BUZZER 1

#define LED_SYSTEM_GREEN 2
#define LED_CAR_GREEN 3
#define LED_CAR_RED 9
#define LED_SHIP_GREEN 5
#define LED_SHIP_RED 6

#define EMERGENCY_SW 7

//// test
#define testSW 8
#define SYSTEM_STOP 10

// FTM
#define FTM_SERVO FTM1
#define FTM_SERVO_PCC_INDEX PCC_FTM1_INDEX
#define FTM_SERVO_CAR_CH 6
#define FTM_SERVO_SHIP_CH 7

#endif