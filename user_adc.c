#include <system.h>
#include <sys5000.h>
#include <trace.h>
#include <system.h>
#include <math.h>
#include "user_adc.h"
#include "config.h"
WORD adc_old_val, adc_curr_val,adc_cntSample;	//value of 8 channel ADC
WORD adc_arrval[ADC_sample_num];
WORD adc_lastStatus_btn= 0;
WORD adc_iBtn_press_long= FALSE;
WORD adc_last_Btn_time_press=0;
WORD adc_timePressKeep=0;

#define ADC_PIN (2<<11)//10b: select VIN2 pin 33
WORD getSampleAverage(WORD arr[], WORD len)
{
  WORD i = 0;
  DWORD sum = 0;
  for (i = 0; i < len; i++)
    sum += arr[i];
  return (WORD)(sum / len);
}

void adc_init(void)
{
	WORD av;
	//clear ADC channel address bits
	//Bit[12:11] – ADC_MUX_SEL //10b: select VIN2 pin 33
	_andio(CLOCK_AND_RESET_CONTROL1_PORT, ~(1 << 11)); 
    _orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
	//ADC VIN2 pin enable
	_orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 10);
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
	
	do av = _rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC
	while( av&0x8000 );	//EOC/=1? -> wait
	adc_curr_val=adc_old_val=av;	
	TRACE("adc_init %d", av);
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );
}

void ADC_check(void)
{
	WORD diff;
	adc_arrval[adc_cntSample]=_rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC, value inrange 0..0x7FC0
	//TRACE("ADC_check adc_val=%d", adc_arrval[adc_cntSample]);
	//while( adc_arrval[cntSample]&0x8000 );	//EOC/=1? -> wait
	adc_cntSample++;
	if (adc_cntSample >= ADC_sample_num)
	{
		adc_cntSample=0;
		adc_curr_val=getSampleAverage(adc_arrval, ADC_sample_num);
		
		if(adc_curr_val!=adc_old_val)
		{
			
			//TRACE("ADC_check adc read=%d", adc_arrval[adc_cntSample]);
			//TRACE("adc_curr_val=%d", adc_curr_val);
			adc_old_val=adc_curr_val;			
			if((adc_curr_val>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn1_press_value+ADC_Threshold)))
			{
				if(adc_lastStatus_btn!=Key1_press_value)
				{
					//TRACE("Key1_press %d", adc_curr_val);
					adc_lastStatus_btn=Key1_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;			
				}else
				{
					adc_last_Btn_time_press++;
					//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
					if(adc_last_Btn_time_press>=TIME_PRESS_KEEP)
					{			
						TRACE("Key1_press long %d", adc_curr_val);				
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_CONTINUE;
						adc_lastStatus_btn=0;
						Button_1_Press();
					}
				}
			}else if((adc_curr_val>=(ADC_Btn2_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn2_press_value+ADC_Threshold)))
			{
				if(adc_lastStatus_btn!=Key2_press_value)
				{
					//TRACE("Key2_press %d", adc_curr_val);
					adc_lastStatus_btn=Key2_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;						
				}else
				{
					adc_last_Btn_time_press++;
					//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
					if(adc_last_Btn_time_press>=TIME_PRESS_KEEP)
					{			
						TRACE("Key2_press long %d", adc_curr_val);				
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_CONTINUE;
						adc_lastStatus_btn=0;
						Button_2_Press();
					}
				}
			}else if((adc_curr_val>=(ADC_Btn3_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn3_press_value+ADC_Threshold)))
			{
				if(adc_lastStatus_btn!=Key3_press_value)
				{
					//TRACE("Key3_press %d", adc_curr_val);
					adc_lastStatus_btn=Key3_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;						
				}else
				{
					adc_last_Btn_time_press++;
					//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
					if(adc_last_Btn_time_press>=TIME_PRESS_KEEP)
					{			
						TRACE("Key3_press long %d", adc_curr_val);				
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_CONTINUE;
						adc_lastStatus_btn=0;
						Button_3_Press();
					}
				}
			}else
			{
				if(adc_lastStatus_btn!=0) 
				{
					TRACE("Key_press out %d", adc_lastStatus_btn);
					if(adc_lastStatus_btn==Key1_press_value)
						Button_1_Press();
					else if(adc_lastStatus_btn==Key2_press_value)	
						Button_2_Press();
					else if(adc_lastStatus_btn==Key3_press_value)	
						Button_3_Press();	
					adc_lastStatus_btn=0;
					adc_old_val=0;
				}  
			}
		}
	}
	
	//_andio( CLOCK_AND_RESET_CONTROL1_PORT, ADC_PIN);	//clear ADC channel address bits
	_andio(CLOCK_AND_RESET_CONTROL1_PORT, ~(1 << 11)); 
    _orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
}

