
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

extern MyData_t  myData;
extern WORD devices_connect;
extern WORD	iNeedSaveFlash;
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
	TRACE("uart1_send_cmd=%x", cmd);
	TRACE("value=%x", value);
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
	TRACE("uart_send_cmd cmd=%d",cmd);
	TRACE("uart_send_cmd value=%d",value);
	uart1_send_cmd(cmd,value);	
}


void uart_cmd_parse(WORD cmd, WORD value,WORD iInit)
{	
	//WORD tmp;
	DWORD valueConvert;
	
	TRACE("uart_cmd_parse cmd=%d",cmd);	
	TRACE("uart_cmd_parse value=%d",value);		
		switch (cmd)
		{			
			case CMD_PANEL_SYNC:				
				TRACE("CMD_PANEL_SYNC %x",value);
				syncDataToPanel();		
				break;	
			case CMD_MIC_VOL:	
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;		
					myData.Mic_Vol=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				_MixPaXT_Gain_LinearGainValue(dsp[DSP1_MIXPAXT], dsp1pcs[4],valueConvert);
				break;
			// case CMD_MIC1_VOL:	
			// 	//TRACE("CMD_MIC1_VOL %d",value);
			// 	if(!iInit)
			// 	{
			// 		iNeedSaveFlash=TRUE;		
			// 		myData.Mic1_Vol=value;
			// 	}
			// 	valueConvert=ConvertValueToSAM((DWORD)value,cmd);
			// 	_MixPaXT_MixN_LinearGainValue(dsp[MIXPAXT], dsp1pcs[3], 1, valueConvert);//MixN#3,gain2
			// 	break;
			// case CMD_MIC2_VOL:
			// 	//TRACE("CMD_MIC2_VOL %d",value);
			// 	if(!iInit)
			// 	{
			// 		iNeedSaveFlash=TRUE;
			// 		myData.Mic2_Vol=value;
			// 	}
			// 	valueConvert=ConvertValueToSAM((DWORD)value,cmd);
			// 	_MixPaXT_MixN_LinearGainValue(dsp[MIXPAXT], dsp1pcs[3], 0, valueConvert);//MixN#3,gain1
			// 	break;			
			case CMD_MIC_BASS:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_Bass=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				func_SendValueToSAM(DSP3_MIXPAXT,0x0460,valueConvert,FORMAT_14BIT_PRECISION);
				break;	
			case CMD_MIC_MID:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Mic_Mid=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				func_SendValueToSAM(DSP3_MIXPAXT,0x0461,valueConvert,FORMAT_14BIT_PRECISION);
				break;			
			case CMD_MIC_TREB:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE; 
					myData.Mic_Treb=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				func_SendValueToSAM(DSP3_MIXPAXT,0x0462,valueConvert,FORMAT_14BIT_PRECISION);
				break;  
			case CMD_ECHO:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Echo_Vol=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				_LiveMic_Effect_EchoInputLevel(dsp[DSP4_LIVEMIC], valueConvert);
				break;					
			case CMD_DELAY:
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Delay=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				_LiveMic_Effect_EchoTime(dsp[DSP4_LIVEMIC], valueConvert);
				break;				 
			case CMD_REVERB: 
				if(!iInit)
				{
					iNeedSaveFlash=TRUE;
					myData.Reverb=value;
				}
				valueConvert=ConvertValueToSAM((DWORD)value,cmd);
				_LiveMic_Effect_RevLevel(dsp[DSP4_LIVEMIC], valueConvert);
				break;			
			case CMD_MIC_FBC:
				if(!iInit)
					iNeedSaveFlash=TRUE;
				_FBCancel_Bypass( dsp[DSP2_FBC], value );	
				//_FBCancel_SetMode( dsp[DSP2_FBC], value==0?FBC_OFF:FBC_ON);		
				 	
				break;
			
			default:
				break;
		}	
}

void TurnFBC(WORD value,WORD iInit)
{	
	//TRACE("TurnFBC %x",value);
	uart_cmd_parse(CMD_MIC_FBC,value,iInit);	
}

void syncDataToPanel(void)
{	
	uart_send_cmd(CMD_MIC_VOL, myData.Mic_Vol);
	uart_send_cmd(CMD_MIC_BASS, myData.Mic_Bass);
	uart_send_cmd(CMD_MIC_TREB, myData.Mic_Treb);
	uart_send_cmd(CMD_MIC_MID, myData.Mic_Mid);
	uart_send_cmd(CMD_ECHO, myData.Echo_Vol);
	uart_send_cmd(CMD_DELAY, myData.Delay);
	uart_send_cmd(CMD_REVERB, myData.Reverb);
	uart_send_cmd(CMD_MIC_FBC, myData.Mic_FBC);
	check_mics_connect(TRUE);
	uart_send_cmd(CMD_PANEL_SYNC, 0);	
	//TRACE("syncDataToPanel %d",value);

}

