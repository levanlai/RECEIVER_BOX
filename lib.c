 #include <system.h>
#include <sys5000.h>
#include <DreamBoards.h>
#include <libFX5000.h>
#include <trace.h>
#include <math.h>
#include "config.h"
#include "lib.h"
#include "dsp/midictrl.h"
#include "nvs/pms.h"
#include "lcd/uart.h"

MyData_t  myData;
WORD devices_connect=0;
WORD devices_connect_tmp=0;
WORD iNeedSaveFlash=FALSE;
extern void delayMsec(WORD ms);
//https://resource.heltec.cn/utils/hf
#define FLOAT_2         0x40000000
#define FLOAT_10        0x41200000
#define FLOAT_12        0x41400000
#define FLOAT_20        0x41a00000
#define FLOAT_100       0x42c80000
#define FLOAT_650       0x44228000
#define FLOAT_250       0x437a0000
#define FLOAT_250_0x2EBA    0x463ae800
#define FLOAT_0x7FFE    0x46fffc00
#define FLOAT_0x7FFF    0x46fffe00

#define MYDATA_FLASH_ID  0xF005

void SysVarInit(void)
{
    int rc ;
    rc=pms_get_bufs(MYDATA_FLASH_ID,(WORD *)&myData,sizeof(myData)/sizeof(WORD));
    TRACE("SysVarInit rc=%d",rc);
    if(rc==-1)
    {        
        myData.Mic_1_Vol=UI_VALUE_MID;  
        myData.Mic_1_Effect=UI_VALUE_MID;           
        myData.Mic_1_Bass=UI_VALUE_MID;    
        myData.Mic_1_Mid=UI_VALUE_MID;       
        myData.Mic_1_Treb=UI_VALUE_MID;
        myData.Mic_1_Echo=UI_VALUE_MID;
        myData.Mic_1_Delay=UI_VALUE_MID;
        myData.Mic_1_Reverb=UI_VALUE_MID;
        myData.Mic_1_Repeat=UI_VALUE_MID;

        myData.Mic_2_Vol=UI_VALUE_MID;
        myData.Mic_2_Effect=UI_VALUE_MID;        
        myData.Mic_2_Bass=UI_VALUE_MID;  
        myData.Mic_2_Mid=UI_VALUE_MID;             
        myData.Mic_2_Treb=UI_VALUE_MID;
        myData.Mic_2_Echo=UI_VALUE_MID;
        myData.Mic_2_Delay=UI_VALUE_MID;
        myData.Mic_2_Reverb=UI_VALUE_MID;
        myData.Mic_2_Repeat=UI_VALUE_MID;

        myData.Mic_Vol_Out=UI_VALUE_MID;
        myData.Mic_FBC=FBC_OFF;        
        myData.Mic_Effect=TURN_ON;
        myData.Mic_Control_link=TURN_ON;
        pms_set_bufs(MYDATA_FLASH_ID,(WORD *)&myData,sizeof(struct MyData));
    }      
    uart_cmd_parse(CMD_VOL_OUT,myData.Mic_Vol_Out,TRUE); 
   uart_cmd_parse(CMD_MIC_EFFECT,myData.Mic_Effect,TRUE);
   uart_cmd_parse(CMD_MIC_FBC,myData.Mic_FBC,TRUE);

    uart_cmd_parse(CMD_MIC_1_VOL,myData.Mic_1_Vol,TRUE);
    uart_cmd_parse(CMD_MIC_1_EFFECT,myData.Mic_1_Effect,TRUE);
    uart_cmd_parse(CMD_MIC_1_ECHO,myData.Mic_1_Echo,TRUE);
    //uart_cmd_parse(CMD_MIC_1_DELAY,myData.Mic_1_Delay,TRUE);
    uart_cmd_parse(CMD_MIC_1_REVERB,myData.Mic_1_Reverb,TRUE);
    uart_cmd_parse(CMD_MIC_1_REPEAT,myData.Mic_1_Repeat,TRUE);
    uart_cmd_parse(CMD_MIC_1_BASS,myData.Mic_1_Bass,TRUE);
    uart_cmd_parse(CMD_MIC_1_MID,myData.Mic_1_Mid,TRUE);
    uart_cmd_parse(CMD_MIC_1_TREBLE,myData.Mic_1_Treb,TRUE);
    uart_cmd_parse(CMD_MIC_1_DELAY,myData.Mic_1_Delay,TRUE);
    if(!myData.Mic_Control_link)
    {
        uart_cmd_parse(CMD_MIC_2_VOL,myData.Mic_2_Vol,TRUE);
        uart_cmd_parse(CMD_MIC_2_EFFECT,myData.Mic_2_Effect,TRUE);
        uart_cmd_parse(CMD_MIC_2_ECHO,myData.Mic_2_Echo,TRUE);
       // uart_cmd_parse(CMD_MIC_2_DELAY,myData.Mic_2_Delay,TRUE);
        uart_cmd_parse(CMD_MIC_2_REVERB,myData.Mic_2_Reverb,TRUE);
        uart_cmd_parse(CMD_MIC_2_REPEAT,myData.Mic_2_Repeat,TRUE);
        uart_cmd_parse(CMD_MIC_2_BASS,myData.Mic_2_Bass,TRUE);
        uart_cmd_parse(CMD_MIC_2_MID,myData.Mic_2_Mid,TRUE);
        uart_cmd_parse(CMD_MIC_2_TREBLE,myData.Mic_2_Treb,TRUE);
        uart_cmd_parse(CMD_MIC_2_DELAY,myData.Mic_2_Delay,TRUE);
    }
    
   
}

void checkSaveFlash(void)
{
    if(iNeedSaveFlash)
    {
        iNeedSaveFlash=FALSE;
        pms_set_bufs(MYDATA_FLASH_ID,(WORD *)&myData,sizeof(struct MyData));
    }
}

void SaveFlash(void)
{
    pms_set_bufs(MYDATA_FLASH_ID,(WORD *)&myData,sizeof(struct MyData));    
}

void resetFactory(void)
{
    int rc ;
    rc=pms_delete_bufs(MYDATA_FLASH_ID);
    TRACE("resetFactory rc=%d",rc);
    delayMsec(100);
    SysVarInit();
}

void setBit(WORD Reg, WORD bitPosition)
{
	WORD value = (0xFFFF & (1 << bitPosition));
	_orio(Reg, value);
}

void clearBit(WORD Reg, WORD bitPosition)
{
	WORD value = (0xFFFF & (~(1 << bitPosition)));
	_andio(Reg, value);
}

FLOAT func_calRangeLinearGainValue(FLOAT value)
{
    //Converts linearValue parameter[0..0x7FFF] to 20*log10(linearValue/0x7FFF)+12. 
    //linearValue is the gain value, 0x7FFF=+12dB .. 0x5A9D=+9dB .. 0x4026=+6dB, .. 0x2000=0dB, .. 0x1000=-6dB, .. 0=-Inf 
    // vd: 20*log10(linearValue/0x7FFF)+12=-3
    // ->log10(linearValue/0x7FFF)=(-3-12)/20
    // ->linearValue/0x7FFF=10^((-3-12)/20)
    // ->linearValue=0x7FFF*10^((-3-12)/20)
    //log⁡(a)=b có nghĩa là 10^b=a
    FLOAT result;
    //TRACE("value=%f",value); 	
    //TRACE("value=%x",value); 
    // if(value==-6)   range = 0x1000;
	// else if(value==-3)   range = 0x16C3;
    // else if(value==0)   range = 0x2000;
	// else if(value==3)   range = 0x2D6A;
    // else if(value==6)   range = 0x4026;
    // else if(value==9)   range = 0x5A9D;
    // else if(value==12)   range = 0x7FFF;
    // else
    {
       FLOAT tmp=_fmul(FLOAT_0x7FFF,_fpow10(_fdiv(_fsub(_fdiv(value,FLOAT_10),FLOAT_12),FLOAT_20)));
        //FLOAT tmp=0x7FFF*_fpow10((value-12)/20);
         //TRACE("tmp=%x",tmp); 
        result=_ftol(tmp);  
        //TRACE("result=%x",result); 
        //-6->range=0x101D
    }    
    return result;
}
FLOAT func_convertEQToSam(FLOAT value)
{
    FLOAT result,resultEnd;    
    //valueToSAM=((EQ_GAIN_MID_SAM+2*valueConvert)<<8)&0x0FFFF;
    FLOAT tmp=_fdiv(_fmul(value,FLOAT_2),FLOAT_10);
    result=_ftol(tmp); 
    resultEnd= ((EQ_GAIN_MID_SAM+result)<<8)&0x0FFFF;
    //TRACE("func_convertEQToSam resultEnd=%x",resultEnd); 
    //TRACE("func_convertEQToSam result=%x",result);  
    return resultEnd;
}
FLOAT func_convertEchoToSam(FLOAT value)
{
    FLOAT result;
    //in range 0..0x7FFE = 0...100%
    //->linearValue=value/100*0x7FFF
    FLOAT tmp=_fmul(_fdiv(_fdiv(value,FLOAT_10),FLOAT_100),FLOAT_0x7FFF);
    result=_ftol(tmp);  
    //TRACE("func_convertEchoToSam value=%x",value);  
    return result;
}
FLOAT func_convertDelayToSam(FLOAT value)
{
    FLOAT result;
    //in range 0..0x7FFE = 20ms...650ms
    //WORD minDelay=20;
    //WORD maxDelay=650;
    //FLOAT result=(value-minDelay)*0x7FFE/(maxDelay-minDelay);
    FLOAT tmp=_fdiv(_fmul(_fsub(_fdiv(value,FLOAT_10),FLOAT_20),FLOAT_0x7FFE),_fsub(FLOAT_650,FLOAT_20));
    //FLOAT tmp=_fdiv(_fmul(_fsub(_fdiv(value,FLOAT_10),FLOAT_20),FLOAT_250_0x2EBA),_fsub(FLOAT_250,FLOAT_20));
    result=_ftol(tmp);  
    //TRACE("func_convertDelayToSam value=%d",value);  
    return result;
}

FLOAT log10(FLOAT x) {
    return _fdiv(_flog(x), FLOAT_LOG10);
}
// Hàm nội suy tuyến tính giữa 2 điểm
FLOAT linearInterpolate(DWORD x, DWORD x0, DWORD x1, DWORD y0, DWORD y1) {
    DWORD result;    
    FLOAT tmp;    
    if (x1 == x0) return _float(y0); // tránh chia cho 0
    tmp=_fadd(_float(y0) ,_fmul(_fdiv(_fsub(_float(x),_float(x0)) , _fsub(_float(x1),_float(x0))), _fsub(_float(y1),_float(y0))));
    result=_ftol(tmp);    
    //TRACE("linearInterpolate tmp=%x",tmp);    
   // TRACE("result=%d",result);   
     return tmp;
    //return y0 + ((x - x0) / (x1 - x0)) * (y1 - y0);
}
// Hàm chuyển đổi value thành dB dựa trên 3 đoạn min-mid-max
FLOAT convertInRange(DWORD value, DWORD min, DWORD mid, DWORD max, DWORD minDb, DWORD midDb, DWORD maxDb) {
    // Nếu có 2 điểm:
    // x0,y0
    // x1,y1
    // Và nếu biết giá trị x nằm trong khoảng [x0,x1], ta muốn tìm giá trị y tương ứng, thì công thức là:
    // y=y0+((x-x0)/(x1-x0))*(y1-y0)
    //khi người dùng nhập một giá trị value trong khoảng [min, max], chương trình sẽ:
    //Nếu value nằm trong [min, mid], nội suy từ (min → mid) tới (minDb → midDb)
    //Nếu value nằm trong [mid, max], nội suy từ (mid → max) tới (midDb → maxDb)
    //TRACE("convertInRange value=%d",value);          
    // Giới hạn value trong [min, max]
    if (value <= min)
    {
        if(minDb==(DWORD)GAIN_MIN_START)
            return _float(GAIN_MIN);
        else 
            return _float(minDb);        
    } 
    if (value >= max) return _float(maxDb);

    if (value <= mid) {
        // Nội suy từ min -> mid
        return linearInterpolate(value, min, mid, minDb, midDb);
    } else {
        // Nội suy từ mid -> max
        return linearInterpolate(value, mid, max, midDb, maxDb);
    }
}

WORD setValueAtOffset(WORD num, WORD offset, WORD value) {
    WORD mask;
    // Kiểm tra xem offset có hợp lệ không
    if (offset < 0 || offset >= 16) {
        return num;
    }    
    // Kiểm tra giá trị value có hợp lệ không
    if (value != 0 && value != 1) {
       return num;
    }
    
    // Tạo một số uint16 với bit 1 tại vị trí offset và toàn bộ các bit còn lại là 0
    mask = 1 << offset;
    
    // Đặt giá trị bit tại vị trí offset
    if (value == 0) {
        // Nếu value là 0, đặt bit tại vị trí offset thành 0
        num &= ~mask;
    } else {
        // Nếu value là 1, đặt bit tại vị trí offset thành 1
        num |= mask;
    }
    return num;
}

WORD getBitValueAtOffset(WORD num, WORD offset) {
    return (num >> offset) & 1;
}

// void check_devices_connect(void)
// {
//     WORD check_bit=bk9532_mic_is_connected(0)|bk9532_mic_is_connected(1);
//     devices_connect_tmp=setValueAtOffset(devices_connect_tmp,mic_status_offset,check_bit);  
//     //TRACE("check_devices_connect tmp=%x",devices_connect_tmp);
//     //TRACE("devices_connect=%x",devices_connect); 
//     if(devices_connect_tmp!=devices_connect)
//     {
//         TRACE("check_devices_connect# =%x",devices_connect_tmp);
//         devices_connect=devices_connect_tmp;
//         uart_send_cmd(UART1,CMD_DEVICES_CONNECT, devices_connect);
//     }    
// }

void check_mics_connect(WORD iSend)
{
    devices_connect_tmp=(bk9532_mic_is_connected(1)<<1)|bk9532_mic_is_connected(0);
    //TRACE("check_devices_connect tmp=%x",devices_connect_tmp);
    //TRACE("devices_connect=%x",devices_connect); 
    if(devices_connect_tmp!=devices_connect || iSend)
    {
        TRACE("check_mics_connect# =%x",devices_connect_tmp);
        devices_connect=devices_connect_tmp;
        uart_send_cmd(CMD_DEVICES_CONNECT, devices_connect);
    }    
}

WORD func_SendValueToSAM(WORD ch, WORD nrpn, DWORD v, WORD format)
{	
	WORD tmp=dspDesigner_HandleNRPN(ch,nrpn,v,format);	
	//TRACE("func_SendValueToSAM %d",tmp);
	return tmp;
}

DWORD ConvertValueToSAM(DWORD value,WORD cmd) 
{    
  	FLOAT valueConvert;
    DWORD valueToSAM;

    //TRACE("ConvertValueToSAM value=%d",value);
    //TRACE("cmd=%d",cmd);
    // if(value<UI_VALUE_MIN)
    //     value=UI_VALUE_MIN;
    // if(value>UI_VALUE_MAX)
    //     value=UI_VALUE_MAX;    
    if(checkCMDGainEQ(cmd))
   	{
        valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)EQ_GAIN_MIN,(DWORD)EQ_GAIN_MID,(DWORD)EQ_GAIN_MAX);
        valueToSAM=func_convertEQToSam(valueConvert);        
    }else if(cmd==CMD_MIC_1_ECHO||cmd==CMD_MIC_2_ECHO)
    {
        valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_ECHO_MIN,(DWORD)UI_MIC_ECHO_MID,(DWORD)UI_MIC_ECHO_MAX);
        valueToSAM=func_convertEchoToSam(valueConvert);
    }else if(cmd==CMD_MIC_1_REVERB ||cmd==CMD_MIC_2_REVERB)
    {
        valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_REVERB_MIN,(DWORD)UI_MIC_REVERB_MID,(DWORD)UI_MIC_REVERB_MAX);
        valueToSAM=func_convertEchoToSam(valueConvert);
    }
    else if(cmd==CMD_MIC_1_DELAY ||cmd==CMD_MIC_2_DELAY)
    {
        valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_DELAY_MIN,(DWORD)UI_MIC_DELAY_MID,(DWORD)UI_MIC_DELAY_MAX);
        valueToSAM=func_convertDelayToSam(valueConvert);
    }else if(cmd==CMD_MIC_1_REPEAT ||cmd==CMD_MIC_2_REPEAT)
    {
        valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_REPEAT_MIN,(DWORD)UI_MIC_REPEAT_MID,(DWORD)UI_MIC_REPEAT_MAX);
        valueToSAM=func_convertEchoToSam(valueConvert);
    }else
    {       
        if(cmd==CMD_VOL_OUT)
            valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_OUT_VOLUME_MIN,(DWORD)UI_MIC_VOLUME_MID,(DWORD)UI_MIC_VOLUME_MAX);
        else if(cmd==CMD_MIC_1_VOL ||cmd==CMD_MIC_2_VOL ||cmd==CMD_MIC_1_EFFECT ||cmd==CMD_MIC_2_EFFECT)
            valueConvert=convertInRange(value,(DWORD)UI_VALUE_MIN,(DWORD)UI_VALUE_MID,(DWORD)UI_VALUE_MAX,(DWORD)UI_MIC_VOLUME_MIN,(DWORD)UI_MIC_VOLUME_MID,(DWORD)UI_MIC_VOLUME_MAX);
        
         valueToSAM=func_calRangeLinearGainValue(valueConvert);
    } 
    
    //TRACE("valueToSAM %x",valueToSAM);
    return valueToSAM;
}

/** Convert little endian */
DWORD cpu_swap_edian(DWORD i)
{
	DWORD ui, ret;
	ui = i;
	ret = ui >> 24;
	ret |= (ui >> 8) & 0x0000ff00;
	ret |= (ui << 8) & 0x00ff0000;
	ret |= (ui << 24);
	return ret;
}

WORD CRC8_Array(DWORD data,WORD len)
{
    WORD crc=0;
    WORD cnt,i,tmp;
    
    for (cnt = 0; cnt < len; cnt++)
    {
        tmp=(data>>(cnt*8)) & 0xff;
       // TRACE("CRC8_Array buf[%x]\n",tmp);
        crc ^= tmp;
        for (i = 0; i < 8; i++)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ 0x07;
			}
			else
			{
				crc <<= 1;
			}
		}
    }    
	 return crc&0x0ff;
}
WORD checkRangeValue(WORD cmd,int value)
{
    WORD maxRange=0;
    // if(cmd== CMD_EFFECT)
    //     maxRange=UI_VALUE_EFFECT_MAX;
    // else
        maxRange=UI_VALUE_MAX;
        
    if(value<UI_VALUE_MIN)
        value=UI_VALUE_MIN;
    if(value>maxRange)
        value=maxRange;   
    return value;
    // switch (cmd)
    // {
    // case MODE_VOL_MIC:        
    // case MODE_ECHO:
    // case MODE_DELAY:
    // case MODE_REVERB:    
    //     if(value<UI_VALUE_MIN)
    //         value=UI_VALUE_MIN;
    //     if(value>UI_VALUE_MAX)
    //         value=UI_VALUE_MAX;   
    //     break;        
    // default:
    //     break;
    // }
}
// WORD stt_old=0xff;
// void parseDataFromMic(DWORD usrdata)
// {
//     WORD cmd=usrdata & 0x0ff;
//     WORD value=(usrdata>>8) & 0x0ff;
//     WORD stt=(usrdata>>16) & 0x0ff;
//     if(stt_old!=stt && cmd>=CMD_MIC_1_VOL && cmd<=CMD_RESET_FORM_MIC)
//     {
//         int tmp=0;
//         stt_old=stt;    
//         switch (cmd)
//         {
//         case CMD_MIC_1_VOL:
//             tmp=myData.Mic_1_Vol;            
//             break;
//         case CMD_MIC_2_VOL:
//             tmp=myData.Mic_2_Vol;            
//             break;    
//         case CMD_MIC_1_ECHO:
//             tmp=myData.Mic_1_Echo; 
//             break;
//         case CMD_MIC_2_ECHO:
//             tmp=myData.Mic_2_Echo; 
//             break;    
//         case CMD_MIC_1_DELAY:
//             tmp=myData.Mic_1_Delay; 
//             break;
//         case CMD_MIC_2_DELAY:
//             tmp=myData.Mic_2_Delay; 
//             break;    
//         case CMD_MIC_1_REVERB:
//             tmp=myData.Mic_1_Reverb; 
//             break;
//         case CMD_MIC_2_REVERB:
//             tmp=myData.Mic_2_Reverb; 
//             break;        
//         case CMD_MIC_1_BASS:
//             tmp=myData.Mic_1_Bass; 
//             break;
//         case CMD_MIC_2_BASS:
//             tmp=myData.Mic_2_Bass; 
//             break;        
//         case CMD_MIC_1_TREBLE:
//             tmp=myData.Mic_1_Treb; 
//             break;    
//         case CMD_MIC_2_TREBLE:
//             tmp=myData.Mic_2_Treb; 
//             break; 
//         // case CMD_EFFECT:
//         //     tmp=myData.Effect; 
//         //     break;  
//         case CMD_RESET_FORM_MIC:           
//             break;                           
//         default:
//             break;
//         }
//         if(cmd!=CMD_RESET_FORM_MIC)
//         {
//             if(value==MOVE_UP)
//                 tmp++;
//             else if(value==MOVE_DOWN)
//                 tmp--;
//            tmp=checkRangeValue(cmd,tmp);           
//         }  
        
//         uart_cmd_parse(cmd,tmp,FALSE);
//         uart_send_cmd(cmd,tmp);
//     }
// }

WORD checkCMDGainEQ(WORD cmd)
{
  WORD result=FALSE;
  if(cmd==CMD_MIC_1_BASS || cmd==CMD_MIC_1_TREBLE || cmd==CMD_MIC_1_MID
   ||cmd==CMD_MIC_2_BASS || cmd==CMD_MIC_2_TREBLE || cmd==CMD_MIC_2_MID)
   result=TRUE;
   return result;
}