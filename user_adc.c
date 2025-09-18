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
WORD powerStatus=Turn_OFF;
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
	_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));	//clear ADC channel address bits
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
			
			TRACE("ADC_check adc read=%d", adc_arrval[adc_cntSample]);
			TRACE("adc_curr_val=%d", adc_curr_val);
			adc_old_val=adc_curr_val;
			// if((adc_curr_val>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn1_press_value+ADC_Threshold)))
			// {
			// 	TRACE("adc_lastStatus_btn=%d", adc_lastStatus_btn);
			// 	if(adc_lastStatus_btn!=Key1_press_value)
			// 	{
			// 		TRACE("Key1_press %d", adc_curr_val);
			// 		adc_lastStatus_btn=Key1_press_value;
			// 		adc_last_Btn_time_press=0;
			// 		adc_iBtn_press_long=FALSE;
			// 	}else
			// 	{
			// 		adc_last_Btn_time_press++;
			// 		TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
			// 		if(adc_last_Btn_time_press>=TIME_POWER_PRESS && !adc_iBtn_press_long)
			// 		{						
			// 			adc_iBtn_press_long=TRUE;	
			// 			powerStatus=(powerStatus==POWER_ON)?POWER_OFF:POWER_ON;	
			// 			TRACE("Key1_press long powerStatus=%d", powerStatus);				
			// 		}
			// 	}
			// }
			if((adc_curr_val>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn1_press_value+ADC_Threshold)))
			{
				if(adc_lastStatus_btn!=Key1_press_value)
				{
					TRACE("Key1_press %d", adc_curr_val);
					adc_lastStatus_btn=Key1_press_value;
					
				}
			}else if((adc_curr_val>=(ADC_Btn2_press_value-ADC_Threshold))&&(adc_curr_val<=(ADC_Btn2_press_value+ADC_Threshold)))
			{
				if(adc_lastStatus_btn!=Key2_press_value)
				{
					TRACE("Key2_press %d", adc_curr_val);
					adc_lastStatus_btn=Key2_press_value;
					
				}
			}else
			{
				if(adc_lastStatus_btn!=0) 
				{
					TRACE("Key1_press out %d", adc_lastStatus_btn);
					if(adc_lastStatus_btn==Key1_press_value)
						changeOutputAudio(0);
					else if(adc_lastStatus_btn==Key2_press_value)	
						ToogleBASS_BOOST();
					adc_lastStatus_btn=0;
				}           
              		
            
			}
		}
	}
	
	_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));	//clear ADC channel address bits
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
}

WORD adc_get_powerStatus(void)
{
	return powerStatus;
}
WORD adc_get_value(void)
{
	return adc_curr_val;
}
