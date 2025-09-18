#ifndef __USER_ADC_H__
#define __USER_ADC_H__
#include <system.h>
#define SCIFPG	0x0C00	//memory page of SAM5xxx scanning interface registers
#define BRVAL_ADC	4	//address of memory mapped 10bit ADC value register
#define ADC_CHANNELS	1 	//number of used ADC channe
#define ADC_sample_num    3 // lấy mẫu 3 lần thì tính trung bình adc

#define ADC_Threshold    50
// #define ADC_Btn1_press_value    180
// #define ADC_Btn2_press_value    560
// #define ADC_Btn3_press_value    740
#define ADC_Btn1_press_value    50
#define ADC_Btn2_press_value    270
#define ADC_Btn3_press_value    430
#define ADC_Btn4_press_value    570
#define ADC_Btn5_press_value    770

#define TIME_POWER_PRESS    30//khoan 1.9s, thêm nhấp nháy nữa là 2.2s

enum {
    Key1_press_value=1,   
    Key2_press_value, 
    Key3_press_value, 
    };

void adc_init(void);
void ADC_check(void);
WORD adc_get_powerStatus(void);
WORD adc_get_value(void);
#endif 