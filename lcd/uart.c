
#include <system.h>
#include <sys5000.h>
#include <libFX5000.h>
#include <trace.h>
#include <libmidi.h>
#include "uart.h"
#include "../dsp/midictrl.h"
#include "../dsp/dspDesigner.h"
#include "../lib.h"
#include "../config.h"
#include "../bk9532/bk9532.h"

#ifdef AUTO_POWER_ON
WORD iFisrt_sync_Panel=TRUE;
WORD ReceiveCmdPowerValue=Turn_ON;//lai test
#else
WORD iFisrt_sync_Panel=FALSE;
WORD ReceiveCmdPowerValue=Turn_OFF;
#endif


static DWORD id_Mic_receive[BK9532_MAX_CHANNELS] = {0, 0};
static WORD OutputAudio=Output_MONO;
static WORD ModeBASS_BOOST=Turn_OFF;
static WORD volume_Master=0;
static WORD volume_Music=0;
WORD detect_Mic_Wire=Detect_unPlug;
WORD volume_Mic_Wire=0;
static WORD volume_Guitar=0;
static WORD detect_Guitar=Detect_unPlug;
////Biquad id theo thứ tự :Type,Q,F,G	
// const struct ui_map_id ui_map_list[] = {
//     //{CMD_MASTER_VOL,func_SendValueToSAM}, 
// 	{CMD_MASTER_VOL, func_SendValueToSAM}, 
// };

// WORD get_map_id_sam(WORD id)
// {
//     WORD i;
//     for (i = 0; i < sizeof(ui_map_list) / sizeof(struct ui_map_id); i++)
//     {
//         if (id == ui_map_list[i].id)
//             return ui_map_list[i].id_ref;
//     }

//     return -1;
// }

// WORD get_map_index(WORD id)
// {
//     WORD i;
//     for (i = 0; i < sizeof(ui_map_list) / sizeof(struct ui_map_id); i++)
//     {
//         if (id == ui_map_list[i].id)
//             return i;
//     }

//     return -1;
// }

void uart_init(void)
{
	// _MIDI_Init: send ,receive được trên 31250,38400 or 57600, nếu chạy 115200 thì chỉ truyền tốc độ tối đa (57600)
	// _MIDI_InitEx: send ,receive được trên 115200
	//đọc thanh ghi CONTROLPORT thì value=0x2B36 ->MIDI1EN=1 (Bit 13 – MIDI1EN: Enable MIDI1 IN to be stored into FIFO)
	//_MIDI_Init(UART_BAUDRATE);	
	_MIDI_InitEx(UART_BAUDRATE);// Same as MIDI_Init without Baudrate limitation
	_MIDI_InitEvent(); 
}

WORD nrpnItem[2] = {0, 0}; //Cmd.Data
//WORD nrpnCrcByte = 0;
WORD bGetCmdDone = 0;
//WORD cmd_OK = 0x5168, cmd_FAIL = 0x1234;
void GetNRPN3b(DWORD d)
{
	WORD ch, byte2, byte3;
	//note: giá trị data/cm thì phải theo quy luật dưới	
	//d =  code | byte1 | byte2
	//với byte<=0x7F (nếu bit8 =1 sẽ không nhận được)
	ch = (d>>16) & 0x0F;
	if (ch == 1) //B1 = cmd
	{
		nrpnItem[0] = (WORD)(d & 0xffff);
		nrpnItem[1] = 0; //reset first 
		bGetCmdDone = 1; //not ready, just got header!
	}else if (ch == 2 && bGetCmdDone == 1) 
	{
		byte2 = (d >> 8) & 0x7f;
		byte3 = (d) & 0x7f;
		nrpnItem[1] = (byte2 << 7) | byte3; 
		bGetCmdDone = 2;
	}

	//Get with 1 cmd, value =  0
	// if (ch == 0) //B0 = cmd, data = 0
	// {
	// 	nrpnItem[0] = (WORD)(d & 0xffff);
	// 	nrpnItem[1] = 0;
	// 	bGetCmdDone = 2; //done
	// }
	//Get with 1 cmd, value 2/3 bytes
	// else if (ch == 1) //B1 = cmd, Add B2 for data <= 0x3fff, Add B3 for data <= 0x7fff
	// {
	// 	nrpnItem[0] = (WORD)(d & 0xffff);
	// 	nrpnItem[1] = 0; //reset first 
	// 	bGetCmdDone = 1; //not ready, just got header!
	// }
	// else if (ch == 2 && bGetCmdDone == 1) //[2 bytes] ... B2 = data <= 0x3fff
	// {
	// 	byte2 = (d >> 8) & 0x7f;
	// 	byte3 = (d) & 0x7f;
	// 	nrpnItem[1] = (byte2 << 7) | byte3; 
	// 	bGetCmdDone = 2;
	// }
	// else if (ch == 3 && bGetCmdDone == 1) //[3 bytes] ... B3 = data <= 0x7fff
	// {
	// 	byte2 = (d >> 8) & 0x7f;
	// 	byte3 = (d) & 0x7f;
	// 	nrpnItem[1] = (1 << 14) | ((byte2 << 7) | byte3); 
	// 	bGetCmdDone = 2;
	// }
	

	TRACE("bGetCmdDone=%d", bGetCmdDone);
	
}
void ResetNRPN3b(void)
{
	nrpnItem[0] = 0;
	nrpnItem[1] = 0;
	bGetCmdDone = 0;
	//nrpnCrcByte = 0;
}

void HandleMidiEvent(DWORD midi_event)
{
	//DWORD value = 0;
	DWORD mask;
	//WORD crcByte;
	TRACE("HandleMidiEvent %x", midi_event);

	mask = (midi_event & 0xFFFF0000);
	if ((mask == 0x0BB00000) || (mask == 0x0BB10000) || (mask == 0x0BB20000) || (mask == 0x0BB30000) || (mask == 0x0BBF0000)) 
	{
		GetNRPN3b(midi_event);
		if (bGetCmdDone == 2) //send result to master <Bluetooth>
		{
			//TRACE("Done here1 Cmd=%x", nrpnItem[0]);
			//TRACE("Data%x", nrpnItem[1]);
			uart_cmd_parse(nrpnItem[0], nrpnItem[1]);
		}
	}
	if (bGetCmdDone == 2) 
	{
		//TRACE("Done here2 Cmd=%x", nrpnItem[0]);
		//TRACE("Data%x", nrpnItem[1]);
	}
}

void midi_handle_event(void)
{
	
	DWORD midi_event = 0;
	//WORD cnum,cin,ch,b1,b2,b3;
	//_MIDI_Flush();
	if(_MIDI_ReceiveEvent(&midi_event))	
	{
		HandleMidiEvent(midi_event);
		// cnum=(midi_event>>24)&0xF0; //cable number
		// cin=(midi_event>>24)&0x0F; //code index number
		// ch=(midi_event>>16)&0x0F; //MIDI channel
		// b1=(midi_event>>16)&0xFF; //MIDI byte 1 (status)
		// b2=(midi_event>>8)&0x7F; //MIDI byte 2
		// b3=midi_event&0x7F; //MIDI byte 3
		// TRACE("midi_event=%x",midi_event);
		// TRACE("cnum=%x",cnum);
		// TRACE("cin=%x",cin);
		// TRACE("ch=%x",ch);
		// TRACE("b1=%x",b1);
		// TRACE("b2=%x",b2);
		// TRACE("b3=%x",b3);
	}
	
	// WORD midi_event;
	// WORD tmp= _MIDI_Receive(&midi_event);
	// TRACE("tmp=%d",tmp);
	// if(tmp)
	// {
	// 	TRACE("midi_event=%d",midi_event);
	// }
}

void uart_send_cmd(WORD cmd, WORD value)
{
	//TRACE("uart_send_cmd cmd=%x", cmd);
	//TRACE("value=%x", value);
	_MIDI_Send((cmd >> 8) & 0xFF);
	_MIDI_Send((cmd) & 0xFF);
	_MIDI_Send((value >> 8) & 0xFF);
	_MIDI_Send((value) & 0xFF);
	_MIDI_Flush();
}

void uart_refresh(void)
{
	_MIDI_Flush();
}

WORD getStatusSyncPanel(void)
{
	return iFisrt_sync_Panel;
}
void setStatusSyncPanel(WORD value)
{
	if(iFisrt_sync_Panel!=value)
	 iFisrt_sync_Panel=value;
}
WORD getReceiveCmdPowerValue(void)
{
	return ReceiveCmdPowerValue;
}

void uart_sync_panel(void)
{
	TRACE("uart_sync_panel %d",iFisrt_sync_Panel);	
	if(!getStatusSyncPanel())
	{
		uart_send_cmd(CMD_PANEL_SYNC, cmd_OK);	
	}
}

void uart_send_ID_mic_pair(WORD bus,DWORD id)
{
	TRACE("uart_send_ID_mic_pair id=0x%x",id);
	if(bus==0)	
	{
		uart_send_cmd(CMD_PAIR_MIC_ID_1_0, id & 0xFFFF);	
		uart_send_cmd(CMD_PAIR_MIC_ID_1_1, (id >> 16) & 0xFFFF);		
	}else
	{
		uart_send_cmd(CMD_PAIR_MIC_ID_2_0, id & 0xFFFF);	
		uart_send_cmd(CMD_PAIR_MIC_ID_2_1, (id >> 16) & 0xFFFF);		
	}	
}

void set_Vol_Mic_Wire(WORD value)
{
	//TRACE("set_Vol_Mic_Wire detect=%d",value);		
	//TRACE("set_Vol_Mic_Wire value=%d",value);		
	if(detect_Mic_Wire==Detect_unPlug)
	{
		_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP1], dsp1pcs[3], 2, 0);//MixN#3,gain3
	}else
	{	
		DWORD valueConvert=ConvertValueToSAM((DWORD)value,Type_MIC_VOL);
		_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP1], dsp1pcs[3], 2, valueConvert);//MixN#3,gain3
	}
}

void set_Vol_Guitar(WORD value)
{
	//TRACE("set_Vol_Guitar detect=%d",value);		
	//TRACE("set_Vol_Guitar value=%d",value);		
	if(detect_Guitar==Detect_unPlug)
	{
		_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[16],0);
		_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[17],0);	
	}else
	{
		DWORD valueConvert=ConvertValueToSAM((DWORD)value,Type_MUSIC_VOL);				
		_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[16],valueConvert);
		_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[17],valueConvert);	
	}
}
//sub luôn ra stereo, switch mono/stereo chỉ cho music 
void uart_cmd_parse(WORD cmd, WORD value)
{	
	//WORD tmp;
	// DWORD valueConvert;
	
	// //TRACE("uart_cmd_parse %d",cmd);		
	// 	switch (cmd)
	// 	{
	// 		case CMD_PAIR_MIC_ID_1_0:
	// 		case CMD_PAIR_MIC_ID_1_1:
	// 		case CMD_PAIR_MIC_ID_1_2:
	// 		case CMD_PAIR_MIC_ID_2_0:
	// 		case CMD_PAIR_MIC_ID_2_1:
	// 		case CMD_PAIR_MIC_ID_2_2:
	// 			receive_ID_pair_fromPanel(cmd,value);				
	// 			break;	
	// 		case CMD_POWER:	
	// 		TRACE("CMD_POWER %x",value);
	// 			ReceiveCmdPowerValue=value;									
	// 			break;
	// 		case CMD_PANEL_SYNC:
	// 		TRACE("CMD_PANEL_SYNC %x",value);
	// 			iFisrt_sync_Panel=TRUE;					
	// 			break;	
	// 		// case CMD_FUNC_SYNC:
	// 		// 	TRACE("CMD_FUNC_SYNC %d",iFisrt_sync_Panel);
	// 		// 	iFisrt_sync_Panel=TRUE;
	// 		// 	func_sync_sendto_panel();
	// 		// 	break;
	// 		case CMD_FUNC_VALUE:
	// 			TRACE("CMD_FUNC_VALUE %x",value);
	// 			parse_func_value_from_panel(value);			
	// 			break;	
	// 		case CMD_MASTER_VOL:
	// 		{
	// 			volume_Master=value;
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_MASTER_VOL);
	// 			if(OutputAudio==Output_MONO)
	// 			{
	// 				TRACE("CMD_MASTER_VOL Output_MONO %d",OutputAudio);
	// 				//Mono thì gain left+right giảm -6db
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 0, GAIN_Minus_6_SAM);//MixN#22,gain 1,index=0
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 2, GAIN_Minus_6_SAM);//MixN#22,gain 3,index=2
					
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 2, GAIN_Minus_6_SAM);
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 0, GAIN_Minus_6_SAM);
	// 				//tat tieng kenh right
	// 				_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP3], dsp3pcs[25],0);//Gain#25					
	// 			}else
	// 			{
	// 				TRACE("CMD_MASTER_VOL Output_STEREO %d",OutputAudio);
	// 				//stereo thì gain của từng kênh=0, kênh khác=-116
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 0, GAIN_0_SAM);
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 2, 0);
					
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 2, GAIN_0_SAM);
	// 				_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 0, 0);

	// 				_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP3], dsp3pcs[25],valueConvert);//Gain#25
	// 			}
	// 			_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP3], dsp3pcs[17],valueConvert);//Gain#17
				
	// 			_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP3], dsp3pcs[4],valueConvert);
	// 			_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP3], dsp3pcs[18],valueConvert);
				
	// 			// _MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 0, valueConvert);//MixN#22,gain 1,index=0
	// 			// _MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 1, valueConvert);///MixN#22,gain 2,index=1
	// 			// if(OutputAudio==Output_MONO)
	// 			// {
	// 			// 	TRACE("CMD_MASTER_VOL Output_MONO %d",OutputAudio);
	// 			//  	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 2, valueConvert);//MixN#22,gain 3,index=2				
	// 			//  	//_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 0, valueConvert);//MixN#23,gain 1,index=0
	// 			// 	//tat tieng kenh right
	// 			// 	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 0, 0);//MixN#23,gain 1,index=0
	// 			// 	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 2, 0);//MixN#23,gain 3,index=2
	// 			// }else
	// 			// {				
				
	// 			// 	TRACE("CMD_MASTER_VOL Output_STEREO %d",OutputAudio);
	// 			//  	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[22], 2, 0);//MixN#22,gain 3,index=2				
	// 			//  	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 0, 0);//MixN#23,gain 1,index=0

	// 			// 	_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 2, valueConvert);//MixN#23,gain 3,index=2
	// 			// }
	// 			// _MixSPDIFOut_MixN_LinearGainValue(dsp[DSP3], dsp3pcs[23], 1, valueConvert);//MixN#23,gain 2,index=1
			
	// 		}
	// 			break;
	// 		case CMD_MUSIC_VOL:
	// 		{
	// 			volume_Music=value;
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_MUSIC_VOL);
	// 			_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[2],valueConvert);//Gain#2
	// 			_MixSPDIFOut_Gain_LinearGainValue(dsp[DSP1], dsp1pcs[8],valueConvert);//Gain#8				
	// 		}				
	// 			break;
	// 		case CMD_MUSIC_BASS:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_EQ);
	// 			func_SendValueToSAM(DSP1,0x0660,valueConvert,FORMAT_14BIT_PRECISION);//BQ#6,index0
	// 			func_SendValueToSAM(DSP1,0x0C60,valueConvert,FORMAT_14BIT_PRECISION);//BQ#12,index0
	// 			break;		
	// 		case CMD_MUSIC_TREB:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_EQ);
	// 			func_SendValueToSAM(DSP1,0x0662,valueConvert,FORMAT_14BIT_PRECISION);//BQ#6,index2
	// 			func_SendValueToSAM(DSP1,0x0C62,valueConvert,FORMAT_14BIT_PRECISION);//BQ#12,index2
	// 			break;
	// 		case CMD_MIC1_VOL:				
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_MIC_VOL);
	// 			_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP1], dsp1pcs[3], 1, valueConvert);//MixN#3,gain2
	// 			//func_SendValueToSAM(DSP1,0x0320,valueConvert,FORMAT_14BIT_PRECISION);//MixN#3,gain2
	// 			break;
	// 		case CMD_MIC2_VOL:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_MIC_VOL);
	// 			_MixSPDIFOut_MixN_LinearGainValue(dsp[DSP1], dsp1pcs[3], 0, valueConvert);//MixN#3,gain1
	// 			//func_SendValueToSAM(DSP1,0x031F,valueConvert,FORMAT_14BIT_PRECISION);//MixN#3,gain1
	// 			break;

	// 		case CMD_MIC_WIRE_VOL:	
	// 			volume_Mic_Wire=value;
	// 			set_Vol_Mic_Wire(value);				
	// 			break;
	// 		case CMD_GUITAR_VOL:
	// 			volume_Guitar=value;
	// 			set_Vol_Guitar(value);				
	// 			break;
	// 		case CMD_MIC_BASS:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_EQ);
	// 			func_SendValueToSAM(DSP4,0x0360,valueConvert,FORMAT_14BIT_PRECISION);//BQ#3,index0
	// 			break;	
	// 		case CMD_MIC_MID:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_EQ);
	// 			func_SendValueToSAM(DSP4,0x0361,valueConvert,FORMAT_14BIT_PRECISION);//BQ#3,index1
	// 			break;			
	// 		case CMD_MIC_TREB: 
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_EQ);
	// 			func_SendValueToSAM(DSP4,0x0362,valueConvert,FORMAT_14BIT_PRECISION);//BQ#3,index2
	// 			break;  
	// 		case CMD_ECHO_VOL:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_Echo);
	// 			_LiveMic_Effect_EchoInputLevel(dsp[DSP4], valueConvert);
	// 			//func_SendValueToSAM(DSP4,0x0108,valueConvert,FORMAT_14BIT_PRECISION);//Echo#1,_LiveMic_Effect_EchoInputLevel, range 0-100%
	// 			break;					
	// 		case CMD_DELAY:
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_Delay);
	// 			func_SendValueToSAM(DSP4,0x0109,valueConvert,FORMAT_14BIT_PRECISION);//Echo#1,_LiveMic_Effect_EchoTime, range:20ms-650ms
	// 			break;				 
	// 		case CMD_REVERB: 
	// 			valueConvert=ConvertValueToSAM((DWORD)value,Type_Reverb);
	// 			_LiveMic_Effect_RevLevel(dsp[DSP4], valueConvert);
	// 			//func_SendValueToSAM(DSP4,0x0102,valueConvert,FORMAT_14BIT_PRECISION);//Echo#1,_LiveMic_Effect_RevLevel, range 0-100%
	// 			break;

	// 		case CMD_BASS_BOOST:
	// 		 	_MixPaXT_Exciter_OnOff( dsp[DSP1], dsp1pcs[1], ModeBASS_BOOST );//Exciter#1
	// 		 	_MixPaXT_Exciter_OnOff( dsp[DSP1], dsp1pcs[7], ModeBASS_BOOST );//Exciter#7
	// 			break;
	// 		case CMD_SWITCH_INPUT:
	// 			changeInput(value);
	// 		break;
	// 		default:
	// 			break;
	// 	}	
}

static _SYS_TASK uart_task;
static _SYS_TASK_CALLBACK uart_task_handler =uart_task_handler_func;
#define uart_STEP_MS 1
static void uart_task_handler_func(void)
{
    uart_call_task_handler();
}

static void uart_call_task_handler()
{
	midi_handle_event();
}
void uart_register_task()
{
    _Sys_SchedRegister(&uart_task, uart_task_handler, uart_STEP_MS);
}

void  receive_ID_pair_fromPanel(WORD cmd,WORD id)
{	
    DWORD tmp=id;
    
    if(cmd==CMD_PAIR_MIC_ID_1_0 ||cmd==CMD_PAIR_MIC_ID_1_1 ||cmd==CMD_PAIR_MIC_ID_1_2 )
    {        
		DWORD id_Mic=id_Mic_receive[0];
        if(cmd==CMD_PAIR_MIC_ID_1_0)
            id_Mic_receive[0] =(DWORD)((id_Mic&0xFFFFC000)|tmp);
        else if(cmd==CMD_PAIR_MIC_ID_1_1)   
            id_Mic_receive[0] =(DWORD)((id_Mic&0xF0003FFF)|(tmp<<14));
        else
            id_Mic_receive[0] =(DWORD)((id_Mic&0x0FFFFFFF)|(tmp<<28));
		TRACE("mic1 id=%x",id_Mic_receive[0]);
    }else if(cmd==CMD_PAIR_MIC_ID_2_0 ||cmd==CMD_PAIR_MIC_ID_2_1 ||cmd==CMD_PAIR_MIC_ID_2_2 )
    {
       DWORD id_Mic=id_Mic_receive[1];
        if(cmd==CMD_PAIR_MIC_ID_2_0)
            id_Mic_receive[1] =(DWORD)((id_Mic&0xFFFFC000)|tmp);
        else if(cmd==CMD_PAIR_MIC_ID_2_1)   
            id_Mic_receive[1] =(DWORD)((id_Mic&0xF0003FFF)|(tmp<<14));
        else
            id_Mic_receive[1] =(DWORD)((id_Mic&0x0FFFFFFF)|(tmp<<28));	
		TRACE("mic2 id=0x%x",id_Mic_receive[1]);	
       
    }   
} 

DWORD  get_ID_pair_fromPanel(WORD bus)
{
	//TRACE("bus=%d",bus);
	//TRACE("id=0x%lx",id_Mic_receive[bus]);
	return id_Mic_receive[bus];
}
void changeInput(WORD value)
{
	if(value==Input_Optical)
	{

	}else
	{

	}
	
}
void changeOutputAudio(WORD value)
{	
	TRACE("changeOutputAudio %d",OutputAudio);
	TRACE("changeOutputAudio value=%d",value);
	#if USING_PANEL
		OutputAudio= (OutputAudio==Output_MONO)?Output_STEREO:Output_MONO;
		uart_send_cmd(LED_2, OutputAudio);
	#else	
		OutputAudio=value;
	#endif
	uart_cmd_parse(CMD_MASTER_VOL,volume_Master);
}

void ToogleBASS_BOOST(void)
{
	ModeBASS_BOOST= (ModeBASS_BOOST==Turn_OFF)?Turn_ON:Turn_OFF;
	TurnBASS_BOOST(ModeBASS_BOOST);	
	
}
void TurnBASS_BOOST(WORD value)
{
	ModeBASS_BOOST= value;
	TRACE("TurnBASS_BOOST %d",value);
	#if USING_PANEL
	uart_send_cmd(LED_3, ModeBASS_BOOST);
	#endif
	uart_cmd_parse(CMD_BASS_BOOST,value);	
}
void set_detect_Mic_Wire(WORD value)
{
	//TRACE("set_detect_Mic_Wire %d",value);
	if(detect_Mic_Wire!=value)
	{
		set_Vol_Mic_Wire(0);
		delayMsec(1000); 
		detect_Mic_Wire=value;
		if(detect_Mic_Wire==Detect_Plug)
			set_Vol_Mic_Wire(volume_Mic_Wire);
	}	
}
void set_detect_Guitar(WORD value)
{
	//TRACE("set_detect_Guitar %d",value);
	if(detect_Guitar!=value)
	{
		set_Vol_Guitar(0);
		delayMsec(1000); 
		detect_Guitar=value;
		if(detect_Guitar==Detect_Plug)
			set_Vol_Guitar(volume_Guitar);
	}	
}

