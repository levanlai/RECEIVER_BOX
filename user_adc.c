#include <system.h>
#include <sys5000.h>
#include <trace.h>
#include <system.h>
#include <math.h>
#include "user_adc.h"
#include "config.h"
WORD adc_old_val[ADC_CHANNELS], adc_curr_val[ADC_CHANNELS],adc_cntSample[ADC_CHANNELS];	//value of 8 channel ADC
WORD adc_arrval[ADC_CHANNELS][ADC_sample_num];
WORD adc_lastStatus_btn;
WORD adc_chn;
WORD adc_iBtn_press_long= FALSE;
WORD adc_last_Btn_time_press=0;
WORD adc_timePressKeep=0;
WORD adc_btn3_pressOK=FALSE;

extern WORD powerState;

WORD getSampleAverage(WORD arr[], WORD len)
{
  WORD i = 0;
  DWORD sum = 0;
  for (i = 0; i < len; i++)
    sum += arr[i];
  return (WORD)(sum / len);
}

FLOAT checkVolBatery(WORD value)
{  
//   FLOAT tmp=_float(value);
//    FLOAT tmp1=V_ref;
//     FLOAT tmp2=_fmul(tmp,tmp1);
//    TRACE("tmp=%f",tmp);
//     TRACE("tmp1=%f",tmp1); 
// 	TRACE("tmp2=%f",tmp2); 

  FLOAT VBat=_fdiv(_fmul(_fdiv(_fmul(_float(value),V_ref),ADC_10bit),_float(140)),_float(30));
  //VBat = (value * V_ref / ADC_10bit) * 140 / 30;
   //TRACE("value=%d",value);
   //TRACE("VBat=%f",VBat); 
  return VBat;
}

void adc_init(void)
{
	// WORD av;
	// //clear ADC channel address bits
	// //Bit[12:11] – ADC_MUX_SEL //10b: select VIN2 pin 33
	// _andio(CLOCK_AND_RESET_CONTROL1_PORT, ~(1 << 11)); 
    // _orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
	// //ADC VIN2 pin enable
	// _orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 10);
	// _wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
	
	// do av = _rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC
	// while( av&0x8000 );	//EOC/=1? -> wait
	// adc_curr_val[ADC_BTN]=adc_old_val[ADC_BTN]=av;	
	// TRACE("adc_init %d", av);
	// _wrxtmem( SCIFPG, BRVAL_ADC, 0 );

	//init adc_val...
	 _orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 10);	//enable VIN2 on SAM5504
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
	while(adc_chn!=ADC_CHANNELS)			//read all ADC channels...
	{	
		WORD av;
		do av = _rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC
		while( av&0x8000 );	//EOC/=1? -> wait
		adc_curr_val[adc_chn]=adc_old_val[adc_chn]=av;	
		TRACE("adc_init %d", av);
		adc_chn++;							//next ADC channel
		_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));	//clear ADC channel address bits
		if(adc_chn==ADC_BTN)
			_orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);//select VIN2
		_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger new ADC conversion
	}
	adc_chn=0;
}

void ADC_check()
{	
	adc_arrval[adc_chn][adc_cntSample[adc_chn]]=_rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC, value inrange 0..0x7FC0
	//TRACE("ADC_check adc_val=%d", adc_arrval[adc_cntSample]);
	//while( adc_arrval[cntSample]&0x8000 );	//EOC/=1? -> wait
	adc_cntSample[adc_chn]++;
	if (adc_cntSample[adc_chn] >= ADC_sample_num)
	{
		adc_cntSample[adc_chn]=0;
		adc_curr_val[adc_chn]=getSampleAverage(adc_arrval[adc_chn], ADC_sample_num);
		
		if(adc_curr_val[adc_chn]!=adc_old_val[adc_chn])
		{
			
			//TRACE("ADC_check adc read=%d", adc_arrval[adc_cntSample]);
			//TRACE("adc_curr_val=%d", adc_curr_val[adc_chn]);
			adc_old_val[adc_chn]=adc_curr_val[adc_chn];	
			if(adc_chn==ADC_BTN && powerState==POWER_ON)
			{
				//TRACE("adc_Btn=%d", adc_curr_val[adc_chn]);
				if((adc_curr_val[adc_chn]>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn1_press_value+ADC_Threshold)))
				{
					if(adc_lastStatus_btn!=Key1_press_value)
					{
						//TRACE("Key1_press %d", adc_curr_val);
						adc_lastStatus_btn=Key1_press_value;
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_KEEP;	
						Button_1_Press();		
					}else
					{
						adc_last_Btn_time_press++;
						TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
						if(adc_last_Btn_time_press>=adc_timePressKeep)
						{			
							TRACE("Key1_press long %d", adc_curr_val);				
							adc_last_Btn_time_press=0;	
							adc_timePressKeep=TIME_PRESS_CONTINUE;
							Button_1_Press();
						}
					}
				}else if((adc_curr_val[adc_chn]>=(ADC_Btn2_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn2_press_value+ADC_Threshold)))
				{
					if(adc_lastStatus_btn!=Key2_press_value)
					{
						//TRACE("Key2_press %d", adc_curr_val);
						adc_lastStatus_btn=Key2_press_value;
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_KEEP;	
						Button_2_Press();					
					}else
					{
						adc_last_Btn_time_press++;
						//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
						if(adc_last_Btn_time_press>=adc_timePressKeep)
						{			
							TRACE("Key2_press long %d", adc_curr_val);				
							adc_last_Btn_time_press=0;	
							adc_timePressKeep=TIME_PRESS_CONTINUE;
							Button_2_Press();
						}
					}
				}else if((adc_curr_val[adc_chn]>=(ADC_Btn3_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn3_press_value+ADC_Threshold)))
				{
					if(adc_lastStatus_btn!=Key3_press_value)
					{
						TRACE("Key3_press %d", adc_curr_val);
						adc_lastStatus_btn=Key3_press_value;
						adc_last_Btn_time_press=0;	
						adc_timePressKeep=TIME_PRESS_KEEP;
						adc_btn3_pressOK=FALSE;
						//Button_3_Press();						
					}else
					{
						if(!adc_btn3_pressOK)
						{
							adc_last_Btn_time_press++;
							//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
							if(adc_last_Btn_time_press>=adc_timePressKeep)
							{			
								TRACE("Key3_press long %d", adc_curr_val);				
								adc_last_Btn_time_press=0;	
								adc_timePressKeep=TIME_PRESS_CONTINUE;
								adc_btn3_pressOK=TRUE;
								Button_3_Press();								
							}
						}
					}
				}else
				{
					if(adc_lastStatus_btn!=0) 
					{
						TRACE("Key_press out %d", adc_lastStatus_btn);
						// if(adc_lastStatus_btn==Key1_press_value)
						// 	Button_1_Press();
						// else if(adc_lastStatus_btn==Key2_press_value)	
						// 	Button_2_Press();
						// else if(adc_lastStatus_btn==Key3_press_value)	
						// 	Button_3_Press();	
						adc_lastStatus_btn=0;
						adc_old_val[adc_chn]=0;
					}  
				}
			}else if(adc_chn==ADC_BATERY)
			{
				//TRACE("adc_Batery=%d", adc_curr_val[adc_chn]);
				//checkVolBatery(adc_curr_val[adc_chn]);
			}			
		}
	}
	
	if( ++adc_chn==ADC_CHANNELS ) adc_chn=0;	//select next adc channel
	_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));
	if(adc_chn==ADC_BTN)
		_orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
}

WORD getValueBatery()
{
	return adc_curr_val[ADC_BATERY];
}