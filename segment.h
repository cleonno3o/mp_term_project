#include "S32K144.h"
typedef struct {
    PORT_Type *PORTN;
    GPIO_Type *PTN;
    int PCC_INDEX;
    unsigned int fnd_data[10] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xDE}; // 0~9 number
    unsigned int fnd_sel[4] = {0x0100, 0x0200, 0x0400, 0x0800};

    void (*delay_ms)(volatile int ms);
    void (*segemnt_init)(Segment *segment, PORT_Type *port_type, GPIO_Type *ptn, int pcc_index, int segment_port[], int com_port[]);
    void (*set_num)(Segment *segment, int num, int pos);
} Segment;

void segment_init(Segment *segment, PORT_Type *port_type, GPIO_Type *ptn, int pcc_index, int segment_port[], int com_port[]) {
    segment->PORTN = port_type;
    segment->PTN = ptn;
    segment->PCC_INDEX = pcc_index;
    PCC->PCCn[segment->PCC_INDEX] = PCC_PCCn_CGC_MASK;
    
    // segment 포트 설정
    for (int i = 0; i < 7; i++) {
        segment->PORTN->PCR[segment_port[i]] = PORT_PCR_MUX(1);
        segment->PTN->PDDR |= 1 << segment_port[i];
    }
    // com 포트 설정
    for (int i = 0; i < 4; i++) {
        segment->PORTN->PCR[com_port[i]] = PORT_PCR_MUX(1);
        segment->PTN->PDDR |= 1 << com_port[i];
    }
}

void setNum(Segment *segment, int num, int pos)
{
    segment->PTN->PSOR = segment->fnd_sel[pos];  // ouput com port select
    segment->PTN->PCOR = 0x7F;                   // clear data output
    segment->PTN->PSOR = segment->fnd_data[num]; // set data output
    segment->delay_ms(2);
    segment->PTN->PCOR = 0xFFF; // clear all
}