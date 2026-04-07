#include <system.h>
#include <sys5000.h>
#include <trace.h>
#include <system.h>
#include <math.h>
#include "user_adc.h"
#include "config.h"
WORD adc_old_val[ADC_CHANNELS], adc_curr_val[ADC_CHANNELS],adc_cntSample;	//value of 8 channel ADC
DWORD adc_Average=0;
WORD adc_lastStatus_btn=0;
WORD adc_chn=0;
WORD adc_sample=0;
//WORD adc_iBtn_press_long= FALSE;
WORD adc_last_Btn_time_press=0;
WORD adc_timePressKeep=0;
WORD adc_btn3_pressOK=FALSE;

extern WORD powerState;

// WORD getSampleAverage(WORD arr[], WORD len)
// {
//   WORD i = 0;
//   DWORD sum = 0;
//   for (i = 0; i < len; i++)
//     sum += arr[i];
//   return (WORD)(sum / len);
// }

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
	WORD adc_arrval=_rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC, value inrange 0..0x7FC0
	//TRACE("ADC_check adc_val=%d", adc_arrval);
	//while( adc_arrval[cntSample]&0x8000 );	//EOC/=1? -> wait
	if(adc_chn==ADC_BTN /*&& powerState==TURN_ON*/)
	{
		adc_curr_val[adc_chn]=adc_arrval;
		if(adc_curr_val[adc_chn]!=adc_old_val[adc_chn])
		{
			adc_old_val[adc_chn]=adc_curr_val[adc_chn];	
			//TRACE("adc_Btn=%d", adc_curr_val[adc_chn]);
			if((adc_curr_val[adc_chn]>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn1_press_value+ADC_Threshold)))
			{
				//TRACE("1 %d", adc_lastStatus_btn);
				if(adc_lastStatus_btn==0)
				{
					TRACE("Key1_press %d", adc_curr_val[adc_chn]);
					adc_lastStatus_btn=Key1_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;	
					Button_1_Press(MOVE_DOWN);		
				}else
				{
					if(adc_lastStatus_btn==Key1_press_value)
					{
						adc_last_Btn_time_press++;
						TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
						if(adc_last_Btn_time_press>=adc_timePressKeep)
						{			
							TRACE("Key1_press long %d", adc_curr_val[adc_chn]);				
							adc_last_Btn_time_press=0;	
							adc_timePressKeep=TIME_PRESS_CONTINUE;
							Button_1_Press(MOVE_DOWN_HOLD);
						}
					}
				}
			}else if((adc_curr_val[adc_chn]>=(ADC_Btn2_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn2_press_value+ADC_Threshold)))
			{
				//TRACE("2 %d", adc_lastStatus_btn);
				if(adc_lastStatus_btn==0)
				{
					TRACE("Key2_press %d", adc_curr_val[adc_chn]);
					adc_lastStatus_btn=Key2_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;	
					Button_2_Press(MOVE_UP);					
				}else
				{
					if(adc_lastStatus_btn==Key2_press_value)
					{
						adc_last_Btn_time_press++;
						//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
						if(adc_last_Btn_time_press>=adc_timePressKeep)
						{			
							TRACE("Key2_press long %d", adc_curr_val[adc_chn]);				
							adc_last_Btn_time_press=0;	
							adc_timePressKeep=TIME_PRESS_CONTINUE;
							Button_2_Press(MOVE_UP_HOLD);
						}
					}
				}
			}else if((adc_curr_val[adc_chn]>=(ADC_Btn3_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn3_press_value+ADC_Threshold)))
			{
				//TRACE("3 %d", adc_lastStatus_btn);
				if(adc_lastStatus_btn==0)
				{
					TRACE("Key3_press %d", adc_curr_val[adc_chn]);
					adc_lastStatus_btn=Key3_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;
					adc_btn3_pressOK=FALSE;	
					//Button_3_Press(MOVE_UP);				
				}else
				{
					if(adc_lastStatus_btn==Key3_press_value && !adc_btn3_pressOK)
					{
						adc_last_Btn_time_press++;
						//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
						if(adc_last_Btn_time_press>=adc_timePressKeep)
						{			
							TRACE("Key3_press long %d", adc_curr_val[adc_chn]);				
							adc_last_Btn_time_press=0;	
							adc_timePressKeep=TIME_PRESS_CONTINUE;
							adc_btn3_pressOK=TRUE;
							Button_3_Press(MOVE_UP_HOLD);								
						}
					}
				}
			}else if((adc_curr_val[adc_chn]>=(ADC_Btn4_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn4_press_value+ADC_Threshold)))
			{
				//TRACE("4 %d", adc_lastStatus_btn);
				if(adc_lastStatus_btn==0)
				{
					TRACE("Key4_press %d", adc_curr_val[adc_chn]);//nhấn giữ 2 nút 1+2
					adc_lastStatus_btn=Key4_press_value;
					adc_last_Btn_time_press=0;	
					adc_timePressKeep=TIME_PRESS_KEEP;
					Button_4_Press();					
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
					
					if(adc_lastStatus_btn==Key3_press_value && !adc_btn3_pressOK)
						Button_3_Press(MOVE_UP);	
					adc_lastStatus_btn=0;
					adc_old_val[adc_chn]=0;
				}  
			}
		}
	}else if(adc_chn==ADC_BATERY)
	{
		if(adc_curr_val[adc_chn]==0)
		{
			adc_curr_val[adc_chn]=adc_arrval;
			adc_old_val[adc_chn]=adc_arrval;
			adc_Average=0;
		}
		adc_cntSample+=1;
		adc_Average +=adc_arrval;
		if (adc_cntSample >= ADC_sample_Bat_num)
		{			
			adc_curr_val[adc_chn]=(WORD)(adc_Average/ADC_sample_Bat_num);			
			if(adc_curr_val[adc_chn]!=adc_old_val[adc_chn])
			{
				adc_old_val[adc_chn]=adc_curr_val[adc_chn];	
			}
			adc_cntSample=0;
			adc_Average=0;
		}

		// if (adc_cntSample >= ADC_sample_Bat_num)
		// {	
		// 	WORD i,j,temp,skipCount,validSamples;	
		// 	DWORD sum;	
		// 	// Sắp xếp mảng (Thuật toán Bubble Sort đơn giản)
		// 	for (i = 0; i < ADC_sample_Bat_num - 1; i++) {
		// 		for (j = i + 1; j < ADC_sample_Bat_num; j++) {
		// 			if (rawADC[i] > rawADC[j]) {
		// 				temp = rawADC[i];
		// 				rawADC[i] = rawADC[j];
		// 				rawADC[j] = temp;
		// 			}
		// 		}
		// 	}
			
		// 	// Loại bỏ nhiễu: Bỏ 25% giá trị nhỏ nhất và 25% giá trị lớn nhất
		// 	sum = 0;
		// 	skipCount = ADC_sample_Bat_num / 4; 
		// 	validSamples = ADC_sample_Bat_num - (skipCount * 2);
			
		// 	for (i = skipCount; i < ADC_sample_Bat_num - skipCount; i++) {
		// 		sum += rawADC[i];
		// 	}

		// 	adc_curr_val[adc_chn]=(WORD)(sum/validSamples);	
		// 	adc_cntSample=0;
		// }
		//TRACE("adc_cntSample=%d", adc_cntSample);
		//TRACE("adc_curr_val=%d", adc_curr_val[adc_chn]);
	}

	
	// if(powerState==TURN_OFF)
	// 	adc_chn=ADC_BATERY;
	// else
	{
		if( ++adc_chn==ADC_CHANNELS ) adc_chn=ADC_BATERY;	//select next adc channel
	}			
	_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));
	if(adc_chn==ADC_BTN)
		_orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
}

// void ADC_check()
// {	
// 	adc_arrval[adc_chn][adc_cntSample[adc_chn]]=_rdxtmem( SCIFPG, BRVAL_ADC );	//read ADC, value inrange 0..0x7FC0
// 	//TRACE("ADC_check adc_val=%d", adc_arrval[adc_cntSample]);
// 	//while( adc_arrval[cntSample]&0x8000 );	//EOC/=1? -> wait
// 	adc_cntSample[adc_chn]++;
// 	if(adc_chn==ADC_BATERY)
// 	{
// 		if(adc_curr_val[adc_chn]==0)
// 		{
// 			adc_curr_val[adc_chn]=adc_arrval[adc_chn];
// 			adc_old_val[adc_chn]=adc_arrval[adc_chn];
// 		}
// 		adc_sample=ADC_sample_Bat_num;
// 	}else
// 		adc_sample=ADC_sample_num;
// 	if (adc_cntSample[adc_chn] >= adc_sample)
// 	{
// 		adc_cntSample[adc_chn]=0;
// 		adc_curr_val[adc_chn]=getSampleAverage(adc_arrval[adc_chn], adc_sample);
		
// 		if(adc_curr_val[adc_chn]!=adc_old_val[adc_chn])
// 		{
			
// 			//TRACE("ADC_check adc read=%d", adc_arrval[adc_cntSample]);
// 			//TRACE("adc_curr_val=%d", adc_curr_val[adc_chn]);
// 			adc_old_val[adc_chn]=adc_curr_val[adc_chn];	
// 			if(adc_chn==ADC_BTN && powerState==TURN_ON)
// 			{
// 				//TRACE("adc_Btn=%d", adc_curr_val[adc_chn]);
// 				if((adc_curr_val[adc_chn]>=(ADC_Btn1_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn1_press_value+ADC_Threshold)))
// 				{
// 					//TRACE("1 %d", adc_lastStatus_btn);
// 					if(adc_lastStatus_btn==0)
// 					{
// 						TRACE("Key1_press %d", adc_curr_val[adc_chn]);
// 						adc_lastStatus_btn=Key1_press_value;
// 						adc_last_Btn_time_press=0;	
// 						adc_timePressKeep=TIME_PRESS_KEEP;	
// 						Button_1_Press(MOVE_DOWN);		
// 					}else
// 					{
// 						if(adc_lastStatus_btn==Key1_press_value)
// 						{
// 							adc_last_Btn_time_press++;
// 							TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
// 							if(adc_last_Btn_time_press>=adc_timePressKeep)
// 							{			
// 								TRACE("Key1_press long %d", adc_curr_val[adc_chn]);				
// 								adc_last_Btn_time_press=0;	
// 								adc_timePressKeep=TIME_PRESS_CONTINUE;
// 								Button_1_Press(MOVE_DOWN_HOLD);
// 							}
// 						}
// 					}
// 				}else if((adc_curr_val[adc_chn]>=(ADC_Btn2_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn2_press_value+ADC_Threshold)))
// 				{
// 					//TRACE("2 %d", adc_lastStatus_btn);
// 					if(adc_lastStatus_btn==0)
// 					{
// 						TRACE("Key2_press %d", adc_curr_val[adc_chn]);
// 						adc_lastStatus_btn=Key2_press_value;
// 						adc_last_Btn_time_press=0;	
// 						adc_timePressKeep=TIME_PRESS_KEEP;	
// 						Button_2_Press(MOVE_UP);					
// 					}else
// 					{
// 						if(adc_lastStatus_btn==Key2_press_value)
// 						{
// 							adc_last_Btn_time_press++;
// 							//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
// 							if(adc_last_Btn_time_press>=adc_timePressKeep)
// 							{			
// 								TRACE("Key2_press long %d", adc_curr_val[adc_chn]);				
// 								adc_last_Btn_time_press=0;	
// 								adc_timePressKeep=TIME_PRESS_CONTINUE;
// 								Button_2_Press(MOVE_UP_HOLD);
// 							}
// 						}
// 					}
// 				}else if((adc_curr_val[adc_chn]>=(ADC_Btn3_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn3_press_value+ADC_Threshold)))
// 				{
// 					//TRACE("3 %d", adc_lastStatus_btn);
// 					if(adc_lastStatus_btn==0)
// 					{
// 						TRACE("Key3_press %d", adc_curr_val[adc_chn]);
// 						adc_lastStatus_btn=Key3_press_value;
// 						adc_last_Btn_time_press=0;	
// 						adc_timePressKeep=TIME_PRESS_KEEP;
// 						adc_btn3_pressOK=FALSE;	
// 						//Button_3_Press(MOVE_UP);				
// 					}else
// 					{
// 						if(adc_lastStatus_btn==Key3_press_value && !adc_btn3_pressOK)
// 						{
// 							adc_last_Btn_time_press++;
// 							//TRACE("adc_last_Btn_time_press=%d", adc_last_Btn_time_press);
// 							if(adc_last_Btn_time_press>=adc_timePressKeep)
// 							{			
// 								TRACE("Key3_press long %d", adc_curr_val[adc_chn]);				
// 								adc_last_Btn_time_press=0;	
// 								adc_timePressKeep=TIME_PRESS_CONTINUE;
// 								adc_btn3_pressOK=TRUE;
// 								Button_3_Press(MOVE_UP_HOLD);								
// 							}
// 						}
// 					}
// 				}else if((adc_curr_val[adc_chn]>=(ADC_Btn4_press_value-ADC_Threshold))&&(adc_curr_val[adc_chn]<=(ADC_Btn4_press_value+ADC_Threshold)))
// 				{
// 					//TRACE("4 %d", adc_lastStatus_btn);
// 					if(adc_lastStatus_btn==0)
// 					{
// 						TRACE("Key4_press %d", adc_curr_val[adc_chn]);//nhấn giữ 2 nút 1+2
// 						adc_lastStatus_btn=Key4_press_value;
// 						adc_last_Btn_time_press=0;	
// 						adc_timePressKeep=TIME_PRESS_KEEP;
// 						Button_4_Press();					
// 					}
// 				}else
// 				{
// 					if(adc_lastStatus_btn!=0) 
// 					{
// 						TRACE("Key_press out %d", adc_lastStatus_btn);
// 						// if(adc_lastStatus_btn==Key1_press_value)
// 						// 	Button_1_Press();
// 						// else if(adc_lastStatus_btn==Key2_press_value)	
// 						// 	Button_2_Press();
// 						// else if(adc_lastStatus_btn==Key3_press_value)	
// 						// 	Button_3_Press();
						
// 						if(adc_lastStatus_btn==Key3_press_value && !adc_btn3_pressOK)
// 							Button_3_Press(MOVE_UP);	
// 						adc_lastStatus_btn=0;
// 						adc_old_val[adc_chn]=0;
// 					}  
// 				}
// 			}else if(adc_chn==ADC_BATERY)
// 			{
// 				//TRACE("adc_Batery=%d", adc_curr_val[adc_chn]);
// 				//checkVolBatery(adc_curr_val[adc_chn]);
// 			}			
// 		}
// 	}
	
// 	if(powerState==TURN_OFF)
// 		adc_chn=ADC_BATERY;
// 	else
// 	{
// 		if( ++adc_chn==ADC_CHANNELS ) adc_chn=ADC_BATERY;	//select next adc channel
// 	}			
// 	_andio( CLOCK_AND_RESET_CONTROL1_PORT, ~(3<<11));
// 	if(adc_chn==ADC_BTN)
// 		_orio(CLOCK_AND_RESET_CONTROL1_PORT, 1 << 12);
// 	_wrxtmem( SCIFPG, BRVAL_ADC, 0 );	//trigger first ADC conversion
// }

WORD getValueBatery()
{
	//TRACE("adc_Batery=%d", adc_curr_val[ADC_BATERY]);
	return adc_curr_val[ADC_BATERY];
}