#ifndef __USER_ADC_H__
#define __USER_ADC_H__
#include <system.h>
#define SCIFPG	0x0C00	//memory page of SAM5xxx scanning interface registers
#define BRVAL_ADC	4	//address of memory mapped 10bit ADC value register
#define ADC_CHANNELS	2 	//number of used ADC channe
#define ADC_sample_num   1// 3 // lấy mẫu 3 lần thì tính trung bình adc
#define ADC_10bit    0x44800000//1024
#define V_ref        0x40533333//3.3f

#define ADC_Threshold    50

#define ADC_Btn1_press_value    250
#define ADC_Btn2_press_value    420
#define ADC_Btn3_press_value    620

#define TIME_PRESS_KEEP         10
#define TIME_PRESS_CONTINUE    5

enum {
    Key1_press_value=1,   
    Key2_press_value, 
    Key3_press_value, 
};

void adc_init(void);
void ADC_check();
WORD getValueBatery();
#endif 