
  
#include <system.h>
#include <sys5000.h>
#include <libFX5000.h>
#include <trace.h>
#include <string.h>
#include <libmidi.h>
#include "uart.h"
#include "../dsp/midictrl.h"
#include "../dsp/dspDesigner.h"
#include "../lib.h"
#include "../config.h"
#include "../bk9532/bk9532.h"

//20,30,50,70,100,120,140,170,200,210,220,225,230,235,240,245,250
//WORD arr_delay[17] = {0x0,0x0590,0x10B1,0x1BD3,0x2C85,0x37A6,0x42C7,0x5379,0x642B,0x69BC,0x6F4D,0x7215,0x74DD,0x77A6,0x7A6E,0x7D36,0x7FFF};


extern MyData_t  myData;
extern WORD devices_connect;
extern WORD	iNeedSaveFlash;
extern void main_sendCmdPower();
extern WORD getValueBatery();
void delayMsec(WORD ms);
void uart_init(void)
{
	// _MIDI_Init: send ,receive được trên 31250,38400 or 57600, nếu chạy 115200 thì chỉ truyền tốc độ tối đa (57600)
	// _MIDI_InitEx: send ,receive được trên 115200
	//đọc thanh ghi CONTROLPORT thì value=0x2B36 ->MIDI1EN=1 (Bit 13 – MIDI1EN: Enable MIDI1 IN to be stored into FIFO)
	//_MIDI_Init(UART_BAUDRATE);	
	_MIDI_InitEx(UART_BAUDRATE);// Same as MIDI_Init without Baudrate limitation
	_MIDI_InitEvent(); 
}

WORD bProgramInDebugMode = 0;

#define PACK_START_CODE1         0xA5
#define PACK_START_CODE2         0x5A
#define PACK_STOP_CODE          0x16
#define PACK_USER_CODE          0xFC
#define PACK_DATA_LEN           3

WORD McFifo[0x100];		// FIFO for Micro-channel (must be power of two!)
_SYS_CCBWRAPPER Uart1Callback;

//data sending to MIDI-OUT1:
struct {
	_SYS_STREAMHEAD h;
	WORD StreamData[0x400];         // FIFO for MIDI2 out
} MIDI1OutStream;
_SYS_CCBWRAPPER MIDI1StreamFlushCallback;

format_data_t uart1_data;
WORD iDataCnt=0;

// send data to MIDI_OUT1
//SendToMidiOut1
void uart1_send_byte(WORD data)
{
	data &= 0xff;
	_Sys_StreamWriteByte(&MIDI1OutStream.h, data);     // add data to output FIFO
	do {
		//wait, try to send immediately
	} while(!_Sys_UartFlushStream(&MIDI1OutStream.h));
}
void uart1_send_cmd(WORD cmd, WORD value)
{
	//TRACE("uart1_send_cmd=%x", cmd);
	//TRACE("value=%x", value);
	 //uart1_send_byte(PACK_START_CODE1 & 0xFF);
	// uart1_send_byte(PACK_START_CODE2 & 0xFF);
	// uart1_send_byte(PACK_USER_CODE & 0xFF);
	uart1_send_byte((cmd >> 8) & 0xFF);
	uart1_send_byte((cmd) & 0xFF);
	uart1_send_byte((value >> 8) & 0xFF);
	uart1_send_byte((value) & 0xFF);
	//uart1_send_byte(PACK_STOP_CODE & 0xFF);	
}

WORD HandleUart1Data(_SYS_CCB_USER_P dummy, WORD data)
{
	//A5 5A FC 03 14 00 00 16: power off
	//TRACE("data received on UART1", data);
	//TRACE("iDataCnt=%d", iDataCnt);
	data &= 0xFF;	// mask away ID bits
	switch(iDataCnt) 
	{
		case 0:
			if (iDataCnt == 0 && data == PACK_START_CODE1) 
				iDataCnt++;
			break;
		case 1:
			if (iDataCnt == 1 && data == PACK_START_CODE2) 
				iDataCnt++;			
			else 
				iDataCnt = 0;			
			break;
		case 2:
			if (iDataCnt == 2 && data == PACK_USER_CODE)
				iDataCnt++;
			else
				iDataCnt = 0;			
			break;	
		case 3: //len
			if (iDataCnt == 3)
				iDataCnt++;
			else
				iDataCnt = 0;
		break;
		case 4://cmd
			if (iDataCnt==4) {
				uart1_data.cmd = data;
				iDataCnt++;
			}else
				iDataCnt = 0;
			break;		
		case 5: //value1
			if (iDataCnt==5) {
				uart1_data.value = data<< 8;
				iDataCnt++;
			}else
				iDataCnt = 0;
			break;
		case 6://value2
			if (iDataCnt==6) {
				uart1_data.value |= data;
				iDataCnt++;
			}else
				iDataCnt = 0;
			break;
		case 7:	
			if (iDataCnt==7 && data == PACK_STOP_CODE){
				uart_cmd_parse(uart1_data.cmd, uart1_data.value,FALSE);
			}
				iDataCnt = 0;
			break;
	}
	return 1;
}

void uartMC_init(void)
{
	//Use MC for uart2 RD/WR
	//Use MC for uart1 WR, uart1 RD use from Midi class handle, because value will be exactly
	//Must be init after Check ICBM


	bProgramInDebugMode = _Sys_DebuggerConnected();
	if (bProgramInDebugMode) {
		TRACE("Debug. No UART2", bProgramInDebugMode);
	}
	

	// initialize Micro-channel handler (serial MIDI & MPU)...
	_Sys_McSetFifo(McFifo, sizeof(McFifo));		//set FIFO for Micro-channel

	// initialize MIDI_IN...
	_Sys_CcbwInit(&Uart1Callback, HandleUart1Data, 0);
	_Sys_McSetCallbackHandler(_SYS_MCCHN_UART, &Uart1Callback); // data received on MIDI_IN
	_Sys_UartEnable(1);		// enable serial MIDI_IN port through Micro-channel FIFO

#if 1
	// initialize MIDI1 out FIFO...
	 _Sys_CcbwInit(&MIDI1StreamFlushCallback,(_SYS_CCALLBACKFUNC)_Sys_UartFlushStream, 0);
	 _Sys_StreamInit(&MIDI1OutStream.h, sizeof(MIDI1OutStream), &MIDI1StreamFlushCallback);
#endif
}

void uartMC_check_fifo(void)
{
	_Sys_McPoll(10);
}

void uart_send_cmd(WORD cmd, WORD value)
{
	//TRACE("uart_send_cmd uartnum=%d",uartnum);
	//TRACE("uart_send_cmd cmd=%d",cmd);
	//TRACE("uart_send_cmd value=%d",value);
	uart1_send_cmd(cmd,value);	
}


void uart_cmd_parse(WORD cmd, WORD value,WORD iInit)
{	
	//WORD tmp;
	DWORD valueConvert;
	
	TRACE("uart_cmd_parse cmd=%d",cmd);	
	TRACE("value=%d",value);		
		switch (cmd)
		{		
			case CMD_POWER:				
				main_sendCmdPower();	
				break;
			case CMD_BATTERY_VALUE:
				uart_send_cmd(CMD_BATTERY_VALUE, getValueBatery());
				break;			
			case CMD_PANEL_SYNC:				
				TRACE("CMD_PANEL_SYNC %x",value);
				syncDataToPanel();		
				break;				
			case CMD_MIC_1_VOL:	
				//TRACE("CMD_MIC1_VOL %d",value);
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_VOL,value,iInit,TRUE,valueConvert);
				
				break;
			case CMD_MIC_2_VOL:
				//TRACE("CMD_MIC2_VOL %d",value);
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_VOL,value,iInit,TRUE,valueConvert);
				
				break;	
			// case CMD_MIC_1_EFFECT:	
			// 	//TRACE("CMD_MIC_1_EFFECT %d",value);
			// 	valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
			// 	if(myData.Mic_Control_link)
			// 		cmd_execute(CMD_MIC_2_EFFECT,value,iInit,TRUE,valueConvert);
				
			// 	break;
			// case CMD_MIC_2_EFFECT:	
			// 	//TRACE("CMD_MIC_2_EFFECT %d",value);
			// 	valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
			// 	if(myData.Mic_Control_link)
			// 		cmd_execute(CMD_MIC_1_EFFECT,value,iInit,TRUE,valueConvert);
				
			// 	break;		
			case CMD_MIC_1_ECHO:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_ECHO,value,iInit,TRUE,valueConvert);
				
				break;	
			case CMD_MIC_2_ECHO:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_ECHO,value,iInit,TRUE,valueConvert);
				
				break;							
			case CMD_MIC_1_DELAY:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_DELAY,value,iInit,TRUE,valueConvert);
				
				break;	
			case CMD_MIC_2_DELAY:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_DELAY,value,iInit,TRUE,valueConvert);
				
				break;						 
			// case CMD_MIC_1_REVERB: 
			// 	valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
			// 	if(myData.Mic_Control_link)
			// 		cmd_execute(CMD_MIC_2_REVERB,value,iInit,TRUE,valueConvert);
				
			// 	break;
			// case CMD_MIC_2_REVERB: 
			// 	valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
			// 	if(myData.Mic_Control_link)
			// 		cmd_execute(CMD_MIC_1_REVERB,value,iInit,TRUE,valueConvert);
				
			// 	break;
			case CMD_MIC_1_REPEAT: 
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_REPEAT,value,iInit,TRUE,valueConvert);
				
				break;
			case CMD_MIC_2_REPEAT: 
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_REPEAT,value,iInit,TRUE,valueConvert);
				
				break;	
			case CMD_MIC_1_BASS:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_BASS,value,iInit,TRUE,valueConvert);
				
				break;	
			case CMD_MIC_2_BASS:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_BASS,value,iInit,TRUE,valueConvert);
				
				break;
			case CMD_MIC_1_MID:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_MID,value,iInit,TRUE,valueConvert);
				
				break;	
			case CMD_MIC_2_MID:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_MID,value,iInit,TRUE,valueConvert);
				
				break;						
			case CMD_MIC_1_TREBLE:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_2_TREBLE,value,iInit,TRUE,valueConvert);
				
				break;  
			case CMD_MIC_2_TREBLE:
				valueConvert=cmd_execute(cmd,value,iInit,FALSE,0);
				if(myData.Mic_Control_link)
					cmd_execute(CMD_MIC_1_TREBLE,value,iInit,TRUE,valueConvert);
				
				break; 
			case CMD_VOL_OUT: 
				value=checkRangeValue(cmd,value);				
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_Vol_Out=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				_FBCancel_Gain_LinearGainValue(dsp[DSP3_FBC], dsp3pcs[2], valueConvert);
				break;	
			case CMD_MIC_REVERB: 
				cmd_execute(cmd,value,iInit,FALSE,0);				
				break; 	
			case CMD_MIC_EFFECT: 
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_Effect=value;
				}

				if(myData.Mic_Effect==TURN_OFF)
				{
					_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],0, 0);
					_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],1, 0);
					
					_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[8],0, 0);
					_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[8],1, 0);

					_LiveMic_MixN_LinearGainValue(dsp[DSP4_LIVEMIC], dsp4pcs[5],0, 0);
					_LiveMic_MixN_LinearGainValue(dsp[DSP4_LIVEMIC], dsp4pcs[5],1, 0);
				}else
				{					
					_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],0, GAIN_0_SAM);
					_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],1, GAIN_0_SAM);
					
					_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[8],0, GAIN_0_SAM);
					_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[8],1, GAIN_0_SAM);

					_LiveMic_MixN_LinearGainValue(dsp[DSP4_LIVEMIC], dsp4pcs[5],0, GAIN_0_SAM);
					_LiveMic_MixN_LinearGainValue(dsp[DSP4_LIVEMIC], dsp4pcs[5],1, GAIN_0_SAM);
				}

				// if(myData.Mic_Effect==TURN_OFF)
				// {
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[6],0, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[6],1, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[7],0, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[7],1, 0);

				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[5],0, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[5],1, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[6],0, 0);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[6],1, 0);
				// }else
				// {
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[6],0, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[6],1, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[7],0, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[7],1, GAIN_0_SAM);

				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[5],0, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[5],1, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[6],0, GAIN_0_SAM);
				// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[6],1, GAIN_0_SAM);
				// }
				//_LiveMic_Effect_LoadProgram(dsp[DSP1_LIVEMIC],  value==TURN_OFF ? EFFECT_OFF:EFFECT_MonoEcho_Reverb);
				//_LiveMic_Effect_LoadProgram(dsp[DSP2_LIVEMIC],  value==TURN_OFF ? EFFECT_OFF:EFFECT_MonoEcho_Reverb);
				break;						
			case CMD_MIC_FBC:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_FBC=value;
				}
				TRACE("CMD_MIC_FBC %x",value);
				_FBCancel_Bypass( dsp[DSP3_FBC], value );	// bypass: 0=normal work, 1=bypass all notch filters
				//_FBCancel_SetMode( dsp[DSP3_FBC], value==0?FBC_OFF:FBC_ON);
			 	break;
			case CMD_CONTROL_LINK:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_Control_link=value;
				}
				break;	
			case CMD_SAVE:
				SaveFlash();
				break;	
			case CMD_RESET:
				resetFactory();
				break;
			default:
				break;
		}	
}

DWORD cmd_execute(WORD cmd, WORD value,WORD iInit,WORD iLink,DWORD valueSam)
{	
	DWORD tmp;	
	// TRACE("cmd_execute cmd=%d",cmd);	
	// TRACE("value=%d",value);	
	// TRACE("iLink=%d",iLink);	
	// TRACE("valueSam=%x",valueSam);			
	switch (cmd)
	{
		case CMD_MIC_2_VOL:	
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;		
				myData.Mic_2_Vol=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
				//TRACE("CMD_MIC_2_VOL tmp=%x",tmp);	
			_LiveMic_Gain_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[1], tmp);
			break;
		case CMD_MIC_1_VOL:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Vol=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
				//TRACE("CMD_MIC_1_VOL tmp=%x",tmp);	
			_LiveMic_Gain_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[1], tmp);			
			break;	
		// case CMD_MIC_2_EFFECT:	
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;		
		// 		myData.Mic_2_Effect=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[7],0, tmp);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[7],1, tmp);
		// 	break;
		// case CMD_MIC_1_EFFECT:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Effect=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],0, tmp);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],1, tmp);
		// 	break;		
		case CMD_MIC_2_ECHO:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_2_Echo=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			_LiveMic_Effect_EchoInputLevel(dsp[DSP1_LIVEMIC], tmp);
			break;	
		case CMD_MIC_1_ECHO:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Echo=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			_LiveMic_Effect_EchoInputLevel(dsp[DSP2_LIVEMIC], tmp);
			break;							
		case CMD_MIC_2_DELAY:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_2_Delay=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
				TRACE("CMD_MIC_2_DELAY tmp=%x",tmp);	
			_LiveMic_Effect_EchoTime(dsp[DSP1_LIVEMIC], tmp);
				//TRACE("CMD_MIC_2_DELAY value=%d",value);
			 //TRACE("delay=%x",arr_delay[value]);	
			// _LiveMic_Effect_EchoTime(dsp[DSP1_LIVEMIC], arr_delay[value]);
			break;		
		case CMD_MIC_1_DELAY:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Delay=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
				TRACE("CMD_MIC_1_DELAY tmp=%x",tmp);	
			_LiveMic_Effect_EchoTime(dsp[DSP2_LIVEMIC], tmp);
			// TRACE("CMD_MIC_1_DELAY value=%d",value);
			//  	TRACE("delay=%x",arr_delay[value]);	
			//  _LiveMic_Effect_EchoTime(dsp[DSP2_LIVEMIC], arr_delay[value]);
			break;						 
		// case CMD_MIC_2_REVERB: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Reverb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_RevLevel(dsp[DSP1_LIVEMIC], tmp);
		// 	break;
		// case CMD_MIC_1_REVERB: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Reverb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_RevLevel(dsp[DSP2_LIVEMIC], tmp);
		// 	break;
		case CMD_MIC_2_REPEAT: 
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_2_Repeat=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			_LiveMic_Effect_EchoFeedback(dsp[DSP1_LIVEMIC], tmp);
			break;
		case CMD_MIC_1_REPEAT: 
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Repeat=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			_LiveMic_Effect_EchoFeedback(dsp[DSP2_LIVEMIC], tmp);
			break;	
		case CMD_MIC_2_BASS:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_2_Bass=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP1_LIVEMIC,0x0360,tmp,FORMAT_14BIT_PRECISION);
			break;	
		case CMD_MIC_1_BASS:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Bass=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP2_LIVEMIC,0x0360,tmp,FORMAT_14BIT_PRECISION);
			break;	
		case CMD_MIC_2_MID:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_2_Mid=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP1_LIVEMIC,0x0361,tmp,FORMAT_14BIT_PRECISION);
			break;	
		case CMD_MIC_1_MID:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_1_Mid=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP2_LIVEMIC,0x0361,tmp,FORMAT_14BIT_PRECISION);
			break;						
		case CMD_MIC_2_TREBLE:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE; 
				myData.Mic_2_Treb=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP1_LIVEMIC,0x0362,tmp,FORMAT_14BIT_PRECISION);
			break;  
		case CMD_MIC_1_TREBLE:
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE; 
				myData.Mic_1_Treb=value;
			}
			if(iLink)
				tmp=valueSam;
			else
				tmp=ConvertValueToSAM((DWORD)value,cmd);
			func_SendValueToSAM(DSP2_LIVEMIC,0x0362,tmp,FORMAT_14BIT_PRECISION);
			break; 

		case CMD_MIC_REVERB: 
			value=checkRangeValue(cmd,value);
			if(!iInit)
			{
				iNeedSaveFlash=TRUE;
				myData.Mic_Reverb=value;
			}
			tmp=ConvertValueToSAM((DWORD)value,cmd);
			_LiveMic_Effect_RevLevel(dsp[DSP4_LIVEMIC], tmp);
			break;

		// case CMD_MIC_2_VOL:	
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;		
		// 		myData.Mic_2_Vol=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 		//TRACE("CMD_MIC_2_VOL tmp=%x",tmp);	
		// 	_LiveMic_Gain_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[2], tmp);
		// 	//_LiveMic_Gain_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[8], tmp);
		// 	break;
		// case CMD_MIC_1_VOL:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Vol=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 		//TRACE("CMD_MIC_1_VOL tmp=%x",tmp);	
		// 	//_LiveMic_Gain_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[1], tmp);
		// 	_LiveMic_Gain_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[4], tmp);
		// 	break;	
		// case CMD_MIC_2_EFFECT:	
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;		
		// 		myData.Mic_2_Effect=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[7],0, tmp);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP1_LIVEMIC], dsp1pcs[7],1, tmp);
		// 	break;
		// case CMD_MIC_1_EFFECT:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Effect=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],0, tmp);
		// 	_LiveMic_MixN_LinearGainValue(dsp[DSP2_LIVEMIC], dsp2pcs[8],1, tmp);
		// 	break;		
		// case CMD_MIC_2_ECHO:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Echo=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_EchoInputLevel(dsp[DSP1_LIVEMIC], tmp);
		// 	break;	
		// case CMD_MIC_1_ECHO:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Echo=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_EchoInputLevel(dsp[DSP2_LIVEMIC], tmp);
		// 	break;							
		// case CMD_MIC_2_DELAY:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Delay=value;
		// 	}
		// 	// if(iLink)
		// 	// 	tmp=valueSam;
		// 	// else
		// 	// 	tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	// 	TRACE("CMD_MIC_2_DELAY tmp=%x",tmp);	
		// 	// _LiveMic_Effect_EchoTime(dsp[DSP1_LIVEMIC], tmp);
		// 		//TRACE("CMD_MIC_2_DELAY value=%d",value);
		// 	 	TRACE("delay=%x",arr_delay[value]);	
		// 	 _LiveMic_Effect_EchoTime(dsp[DSP1_LIVEMIC], arr_delay[value]);
		// 	break;		
		// case CMD_MIC_1_DELAY:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Delay=value;
		// 	}
		// 	// if(iLink)
		// 	// 	tmp=valueSam;
		// 	// else
		// 	// 	tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	// 	TRACE("CMD_MIC_1_DELAY tmp=%x",tmp);	
		// 	// _LiveMic_Effect_EchoTime(dsp[DSP2_LIVEMIC], tmp);
		// 	TRACE("CMD_MIC_1_DELAY value=%d",value);
		// 	 	TRACE("delay=%x",arr_delay[value]);	
		// 	 _LiveMic_Effect_EchoTime(dsp[DSP2_LIVEMIC], arr_delay[value]);
		// 	break;						 
		// case CMD_MIC_2_REVERB: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Reverb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_RevLevel(dsp[DSP1_LIVEMIC], tmp);
		// 	break;
		// case CMD_MIC_1_REVERB: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Reverb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_RevLevel(dsp[DSP2_LIVEMIC], tmp);
		// 	break;
		// case CMD_MIC_2_REPEAT: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Repeat=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_EchoFeedback(dsp[DSP1_LIVEMIC], tmp);
		// 	break;
		// case CMD_MIC_1_REPEAT: 
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Repeat=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	_LiveMic_Effect_EchoFeedback(dsp[DSP2_LIVEMIC], tmp);
		// 	break;	
		// case CMD_MIC_2_BASS:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Bass=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0160,tmp,FORMAT_14BIT_PRECISION);
		// 	break;	
		// case CMD_MIC_1_BASS:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Bass=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0260,tmp,FORMAT_14BIT_PRECISION);
		// 	break;	
		// case CMD_MIC_2_MID:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_2_Mid=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0161,tmp,FORMAT_14BIT_PRECISION);
		// 	break;	
		// case CMD_MIC_1_MID:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE;
		// 		myData.Mic_1_Mid=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0261,tmp,FORMAT_14BIT_PRECISION);
		// 	break;						
		// case CMD_MIC_2_TREBLE:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE; 
		// 		myData.Mic_2_Treb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0162,tmp,FORMAT_14BIT_PRECISION);
		// 	break;  
		// case CMD_MIC_1_TREBLE:
		// 	value=checkRangeValue(cmd,value);
		// 	if(!iInit)
		// 	{
		// 		iNeedSaveFlash=TRUE; 
		// 		myData.Mic_1_Treb=value;
		// 	}
		// 	if(iLink)
		// 		tmp=valueSam;
		// 	else
		// 		tmp=ConvertValueToSAM((DWORD)value,cmd);
		// 	func_SendValueToSAM(DSP4_MIXPAXT,0x0262,tmp,FORMAT_14BIT_PRECISION);
		// 	break; 
		
		default:
			break;
	}	
	return tmp;
}

void TurnFBC(WORD value,WORD iInit)
{	
	//TRACE("TurnFBC %x",value);
	uart_cmd_parse(CMD_MIC_FBC,value,iInit);	
}

void syncDataToPanel(void)
{	
	uart_send_cmd(CMD_CONTROL_LINK, myData.Mic_Control_link);
	uart_send_cmd(CMD_MIC_1_VOL, myData.Mic_1_Vol);
	//uart_send_cmd(CMD_MIC_1_EFFECT, myData.Mic_1_Effect);
	uart_send_cmd(CMD_MIC_1_ECHO, myData.Mic_1_Echo);
	uart_send_cmd(CMD_MIC_1_DELAY, myData.Mic_1_Delay);
	//uart_send_cmd(CMD_MIC_1_REVERB, myData.Mic_1_Reverb);
	uart_send_cmd(CMD_MIC_1_REPEAT, myData.Mic_1_Repeat);
	uart_send_cmd(CMD_MIC_1_BASS, myData.Mic_1_Bass);
	uart_send_cmd(CMD_MIC_1_MID, myData.Mic_1_Mid);
	uart_send_cmd(CMD_MIC_1_TREBLE, myData.Mic_1_Treb);
	if(!myData.Mic_Control_link)
	{
		uart_send_cmd(CMD_MIC_2_VOL, myData.Mic_2_Vol);
		//uart_send_cmd(CMD_MIC_2_EFFECT, myData.Mic_2_Effect);
		uart_send_cmd(CMD_MIC_2_ECHO, myData.Mic_2_Echo);
		uart_send_cmd(CMD_MIC_2_DELAY, myData.Mic_2_Delay);
		//uart_send_cmd(CMD_MIC_2_REVERB, myData.Mic_2_Reverb);
		uart_send_cmd(CMD_MIC_2_REPEAT, myData.Mic_2_Repeat);
		uart_send_cmd(CMD_MIC_2_BASS, myData.Mic_2_Bass);
		uart_send_cmd(CMD_MIC_2_MID, myData.Mic_2_Mid);
		uart_send_cmd(CMD_MIC_2_TREBLE, myData.Mic_2_Treb);
	}
	uart_send_cmd(CMD_VOL_OUT, myData.Mic_Vol_Out);
	uart_send_cmd(CMD_MIC_REVERB, myData.Mic_Reverb);
	uart_send_cmd(CMD_MIC_EFFECT, myData.Mic_Effect);
	uart_send_cmd(CMD_MIC_FBC, myData.Mic_FBC);
	
	check_mics_connect(TRUE);
	uart_send_cmd(CMD_BATTERY_VALUE, getValueBatery());
	uart_send_cmd(CMD_PANEL_SYNC, 0);	
	//TRACE("syncDataToPanel %d",value);
	
}


