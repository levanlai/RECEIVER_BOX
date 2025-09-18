#include <system.h>
#include <sys5000.h>
#include <DreamBoards.h>
#include <libFX5000.h>
#include <trace.h>
#include <math.h>
#include "config.h"
#include "dsp/midictrl.h"

WORD Sys_func_value_cur   = 0x0000;
WORD Sys_func_value_tmp   = 0x0000;

//https://resource.heltec.cn/utils/hf
#define FLOAT_20        0x41a00000
#define FLOAT_12        0x41400000
#define FLOAT_0x7FFF    0x46fffe00

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

DWORD func_calRangeLinearGainValue(DWORD value)
{
    //Converts linearValue parameter[0..0x7FFF] to 20*log10(linearValue/0x8000)+12. 
    //linearValue is the gain value, 0x7FFF=+12dB .. 0x5A9D=+9dB .. 0x4026=+6dB, .. 0x2000=0dB, .. 0x1000=-6dB, .. 0=-Inf 
    // vd: 20*log10(linearValue/0x8000)+12=-3
    // ->log10(linearValue/0x8000)=(-3-12)/20
    // ->linearValue/0x8000=10^((-3-12)/20)
    // ->linearValue=0x8000*10^((-3-12)/20)
    //log⁡(a)=b có nghĩa là 10^b=a
    DWORD range;
    	
    if(value==-6)   range = 0x1000;
	else if(value==-3)   range = 0x16C3;
    else if(value==0)   range = 0x2000;
	else if(value==3)   range = 0x2D6A;
    else if(value==6)   range = 0x4026;
    else if(value==9)   range = 0x5A9D;
    else if(value==12)   range = 0x7FFF;
    else
    {
        //tmp=_fmul(_float(0x7FFF),_fpow(_float(10),_fdiv(_fsub(_float(value),_float(12)),_float(20))));
        //tmp=_fmul(_float(0x7FFF),_fpow10(_fdiv(_fsub(_float(value),_float(12)),_float(20))));
        FLOAT tmp=_fmul(FLOAT_0x7FFF,_fpow10(_fdiv(_fsub(_float(value),FLOAT_12),FLOAT_20)));
        //TRACE("tmp=%x",tmp); 
        range=_ftol(tmp);  
        //TRACE("range=%x",range); 
        //-6->range=0x101D
        //12->range=0x7FFF
    }
    
    return range;
}

FLOAT log10(FLOAT x) {
    return _fdiv(_flog(x), FLOAT_LOG10);
}

DWORD func_convertvalueToSam(DWORD value,DWORD midcircle,DWORD maxcircle,DWORD middB,DWORD maxdB)
{
    //nếu midcicle=0 thì bỏ qua mid, sẽ tuyến tính từ 0->max
    //Converts linearValue parameter[0..0x7FFF] to 20*log10(linearValue/0x8000)+12. 
    //linearValue is the gain value, 0x7FFF=+12dB .. 0x5A9D=+9dB .. 0x4026=+6dB, .. 0x2000=0dB, .. 0x1000=-6dB, .. 0=-Inf 
    //float value = (float)(20 * Math.log10(((i * range) / maxPercent) / 0x8000) + 12.0);
    
    DWORD samvalue;       
    //FLOAT tmp=_fadd(_fmul(FLOAT_20,log10(_fdiv(FLOAT_0x1000,FLOAT_0x7FFF))),FLOAT_12);
    // TRACE("tmp=%x",tmp); 
    // samvalue=_ftol(tmp);    
    //TRACE("samvalue=%x",samvalue);  
    
    TRACE("func_convertvalueToSam value=%d",value);   
    TRACE("midcircle=%d",midcircle);   
    TRACE("maxcircle=%d",maxcircle);  
     TRACE("middB=%d",middB);   
    TRACE("maxdB=%d",maxdB);  
    if(midcircle!=CIRCLE_NO_MID_POS && value<=midcircle)
    {
        DWORD midRange=func_calRangeLinearGainValue(middB);
        samvalue=(value * midRange) / midcircle;
        TRACE("<=midcircle samvalue=%x",samvalue); 
    }else
    {
        DWORD maxRange=func_calRangeLinearGainValue(maxdB);
        samvalue=(value * maxRange) / maxcircle;
        TRACE(">midcircle samvalue=%x",samvalue);
    }
    return samvalue;
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

// WORD get_Sys_func_avails()
// {
//     return SYSTEM_FUNC_AVAILS;    
// }

void get_Sys_func_value()
{
    WORD check_bit=0;
    check_bit=bk9532_mic_is_connected(0)|bk9532_mic_is_connected(1);
    Sys_func_value_tmp=setValueAtOffset(Sys_func_value_tmp,SYSTEM_FUNC_MIC_STATE_OFFS,check_bit);  
    //TRACE("get_Sys_func_value =%x",Sys_func_value_tmp);
    //TRACE("get_Sys_func_value check_bit=%x",check_bit); 
}

void parse_func_value_from_panel(WORD value)
{
    if(Sys_func_value_cur!=value)
    {        
        WORD valueCur=getBitValueAtOffset(Sys_func_value_cur,SYSTEM_FUNC_FBX_STATE_OFFS);
        WORD valueReceive=getBitValueAtOffset(value,SYSTEM_FUNC_FBX_STATE_OFFS);
        TRACE("parse_func_value_from_panel value=%x",Sys_func_value_cur);
        if(valueCur!=valueReceive)
        {
            TRACE("FBX change %d",valueReceive);
        }
        valueCur=getBitValueAtOffset(Sys_func_value_cur,SYSTEM_FUNC_BASSB_STATE_OFFS);
        valueReceive=getBitValueAtOffset(value,SYSTEM_FUNC_BASSB_STATE_OFFS);
        if(valueCur!=valueReceive)
        {
            TRACE("BASSB change %d",valueReceive);
            TurnBASS_BOOST(valueReceive);
        }        
        valueCur=getBitValueAtOffset(Sys_func_value_cur,SYSTEM_FUNC_ENHANCER_STATE_OFFS);
        valueReceive=getBitValueAtOffset(value,SYSTEM_FUNC_ENHANCER_STATE_OFFS);
        if(valueCur!=valueReceive)
        {
            TRACE("ENHANCER change %d",valueReceive);
        }
        valueCur=getBitValueAtOffset(Sys_func_value_cur,SYSTEM_FUNC_Power48v_OFFS);
        valueReceive=getBitValueAtOffset(value,SYSTEM_FUNC_Power48v_OFFS);
        if(valueCur!=valueReceive)
        {
            TRACE("Power48v change %d",valueReceive);
            set_Power48v_value(valueReceive);
        }
        valueCur=getBitValueAtOffset(Sys_func_value_cur,SYSTEM_FUNC_OUTPUT_AUDIO_MODE);
        valueReceive=getBitValueAtOffset(value,SYSTEM_FUNC_OUTPUT_AUDIO_MODE);
        //TRACE("output valueCur=%d",valueCur);
         //TRACE("output valueReceive=%d",valueReceive);
        if(valueCur!=valueReceive)
        {
            TRACE("OutputAudio change %d",valueReceive);
            changeOutputAudio(valueReceive);
        }
        Sys_func_value_cur=Sys_func_value_tmp=value;
    }
}

void update_func_value_to_panel(void)
{
    WORD tmp=getStatusSyncPanel();
    //TRACE("update_func_value_to_panel %d",tmp); 
    if(getStatusSyncPanel())
    {
        get_Sys_func_value();
        //TRACE("update_func_value_to_panel tmp=%x",Sys_func_value_tmp); 
        //TRACE("Sys_func_value=%x",Sys_func_value_cur); 
        if(Sys_func_value_cur!=Sys_func_value_tmp)    
        {
            Sys_func_value_cur=Sys_func_value_tmp;
            uart_send_cmd(CMD_FUNC_VALUE, Sys_func_value_cur);
        }         
    }
	
}

// void func_sync_sendto_panel(void)
// {
//     if(getStatusSyncPanel())
//     {
//         WORD data=get_Sys_func_avails();
//         TRACE("Sys_func_avails %x",data);        
//         uart_send_cmd(CMD_FUNC_ENABLE, data);
//         update_func_value_to_panel();
//     }
// }

WORD func_SendValueToSAM(WORD ch, WORD nrpn, DWORD v, WORD format)
{	
	WORD tmp=dspDesigner_HandleNRPN(ch,nrpn,v,format);	
	//TRACE("func_SendValueToSAM %d",tmp);
	return tmp;
}

// DWORD ConvertValueToSAM(DWORD value,WORD type) 
// {    
//   	DWORD valueConvert;
//     DWORD valueToSAM;

//     TRACE("ConvertValueToSAM value=%d",value);
//     TRACE("type=%d",type);
//     if(type==Type_EQ)
//    	{
//         if(value<50)//giá trị từ -12db->0db
//         {
//             valueConvert=EQ_GAIN_MIN+(value*(EQ_GAIN_MID-EQ_GAIN_MIN)/ 50);
//             valueToSAM=((EQ_GAIN_MID_SAM+2*valueConvert)<<8)&0x0FFFF;
//         }else//giá trị từ 0db->12db
//         {
//             valueConvert=EQ_GAIN_MID+((value-50)*(EQ_GAIN_MAX-EQ_GAIN_MID)/ 50);
//             valueToSAM=((EQ_GAIN_MID_SAM+2*valueConvert)<<8)&0x0FFFF;
//         } 
//     }else if(type==Type_MASTER_VOL)
//     {
//         //func_convertvalueToSam(value,(DWORD)50,(DWORD)100,(DWORD)0,(DWORD)6);
        
//         if(value==0)
//             valueToSAM=GAIN_MIN_SAM;
//         else if(value<5)    
//              valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
//         else if(value<50)//giá trị từ -20db->(-3)db
//         {
//             valueConvert=GAIN_MIN_start+(value*(GAIN_MID-GAIN_MIN_start)/ 50);
//             if(GAIN_MID<0)
//                 valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//             else    
//                 valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }else//giá trị từ -3db->5db
//         {
//             valueConvert=GAIN_MID+((value-50)*(GAIN_MASTER_MAX-GAIN_MID)/ 50);
//              if(GAIN_MID<0)
//                 valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//             else  
//                 valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }        
//     }else if(type==Type_MUSIC_VOL)
//     {
//         if(value==0)
//             valueToSAM=GAIN_MIN_SAM;
//         else if(value<5)    
//              valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
//         else if(value<50)//giá trị từ -20db->(-3)db
//         {
//             valueConvert=GAIN_MIN_start+(value*(GAIN_MID-GAIN_MIN_start)/ 50);
//             if(GAIN_MID<0)
//                 valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//             else    
//                 valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }else//giá trị từ -3db->3db
//         {
//             valueConvert=GAIN_MID+((value-50)*(GAIN_MAX-GAIN_MID)/ 50);
//              if(GAIN_MID<0)
//                 valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//             else  
//                 valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }        
//     }else if(type==Type_MIC_VOL)
//     {
//          if(value==0)
//             valueToSAM=GAIN_MIN_SAM;
//         else if(value<5)    
//             valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
//         else if(value<50)//giá trị từ -20db->0db
//         {
//             valueConvert=GAIN_MIN_start+(value*(GAIN_MIC_MID-GAIN_MIN_start)/ 50);
//             valueToSAM=(((GAIN_MIC_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }else//giá trị từ 0db->6db
//         {
//                 valueConvert=GAIN_MIC_MID+((value-50)*(GAIN_MIC_MAX-GAIN_MIC_MID)/ 50);
//                 valueToSAM=(((GAIN_MIC_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
//         }        
//     }else if(type==Type_Echo)
//     {
//         if(value<50)//giá trị từ 0->50
//         {
//             valueConvert = (value * (GAIN_ECHO_MID_SAM - GAIN_ECHO_MIN_SAM)) / 50 + GAIN_ECHO_MIN_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;            
//         }else   //giá trị từ 50->70 
//         {
//              valueConvert = ((value-50) * (GAIN_ECHO_MAX_SAM - GAIN_ECHO_MID_SAM)) / 50 + GAIN_ECHO_MID_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;            
//         }
//     }else if(type==Type_Delay)
//     {
//         if(value<50)//giá trị từ 45->75
//         {
//              valueConvert = (value * (GAIN_DELAY_MID_SAM - GAIN_DELAY_MIN_SAM)) / 50 + GAIN_DELAY_MIN_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB; 
//         }else   //giá trị từ 75->100
//         {
//              valueConvert = ((value-50) * (GAIN_DELAY_MAX_SAM - GAIN_DELAY_MID_SAM)) / 50 + GAIN_DELAY_MID_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB; 
//         }
//     }else if(type==Type_Reverb)
//     {
//         if(value<50)//giá trị từ 0->30
//         {
//              valueConvert = (value * (GAIN_REV_MID_SAM - GAIN_REV_MIN_SAM)) / 50 + GAIN_REV_MIN_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;    
//         }else   //giá trị từ 30->60
//         {
//               valueConvert = ((value-50) * (GAIN_REV_MAX_SAM - GAIN_REV_MID_SAM)) / 50 + GAIN_REV_MID_SAM;
//             valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;    
//         }
//     }
//     TRACE("valueConvert %d",valueConvert);
//     TRACE("valueToSAM %x",valueToSAM);
//   return valueToSAM;
// }
DWORD ConvertValueToSAM(DWORD value,WORD type) 
{    
  	DWORD valueConvert;
    DWORD valueToSAM;

    TRACE("ConvertValueToSAM value=%d",value);
    TRACE("type=%d",type);
    if(value<CIRCLE_NO_MID_POS)
        value=CIRCLE_NO_MID_POS;
    if(value>CIRCLE_MAX_POS)
        value=CIRCLE_MAX_POS;    
    if(type==Type_EQ)
   	{
        if(value<50)//giá trị từ -12db->0db
        {
            valueConvert=EQ_GAIN_MIN+(value*(EQ_GAIN_MID-EQ_GAIN_MIN)/ 50);
            valueToSAM=((EQ_GAIN_MID_SAM+2*valueConvert)<<8)&0x0FFFF;
        }else//giá trị từ 0db->12db
        {
            valueConvert=EQ_GAIN_MID+((value-50)*(EQ_GAIN_MAX-EQ_GAIN_MID)/ 50);
            valueToSAM=((EQ_GAIN_MID_SAM+2*valueConvert)<<8)&0x0FFFF;
        } 
    }else if(type==Type_MASTER_VOL)
    {
        valueToSAM=func_convertvalueToSam(value,(DWORD)CIRCLE_MID_POS,(DWORD)CIRCLE_MAX_POS,(DWORD)GAIN_MID,(DWORD)GAIN_MASTER_MAX);
        
        // if(value==0)
        //     valueToSAM=GAIN_MIN_SAM;
        // else if(value<5)    
        //      valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
        // else if(value<50)//giá trị từ -20db->(-3)db
        // {
        //     valueConvert=GAIN_MIN_start+(value*(GAIN_MID-GAIN_MIN_start)/ 50);
        //     if(GAIN_MID<0)
        //         valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        //     else    
        //         valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;

        // }else//giá trị từ -3db->5db
        // {
        //     valueConvert=GAIN_MID+((value-50)*(GAIN_MASTER_MAX-GAIN_MID)/ 50);
        //      if(GAIN_MID<0)
        //         valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        //     else  
        //         valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        // }        
    }else if(type==Type_MUSIC_VOL)
    {
        valueToSAM=func_convertvalueToSam(value,(DWORD)CIRCLE_MID_POS,(DWORD)CIRCLE_MAX_POS,(DWORD)GAIN_MID,(DWORD)GAIN_MAX);
        // if(value==0)
        //     valueToSAM=GAIN_MIN_SAM;
        // else if(value<5)    
        //      valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
        // else if(value<50)//giá trị từ -20db->(-3)db
        // {
        //     valueConvert=GAIN_MIN_start+(value*(GAIN_MID-GAIN_MIN_start)/ 50);
        //     if(GAIN_MID<0)
        //         valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        //     else    
        //         valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        // }else//giá trị từ -3db->3db
        // {
        //     valueConvert=GAIN_MID+((value-50)*(GAIN_MAX-GAIN_MID)/ 50);
        //      if(GAIN_MID<0)
        //         valueToSAM=(((GAIN_MID_SAM-GAIN_MID+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        //     else  
        //         valueToSAM=(((GAIN_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        // }        
    }else if(type==Type_MIC_VOL)
    {
		valueToSAM=func_convertvalueToSam(value,(DWORD)CIRCLE_MID_POS,(DWORD)CIRCLE_MAX_POS,(DWORD)GAIN_MIC_MID,(DWORD)GAIN_MIC_MAX);
        //  if(value==0)
        //     valueToSAM=GAIN_MIN_SAM;
        // else if(value<5)    
        //     valueToSAM=((GAIN_MIN_start_SAM<<8)&0x0FFFF)|GAIN_LSB;
        // else if(value<50)//giá trị từ -20db->0db
        // {
        //     valueConvert=GAIN_MIN_start+(value*(GAIN_MIC_MID-GAIN_MIN_start)/ 50);
        //     valueToSAM=(((GAIN_MIC_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        // }else//giá trị từ 0db->6db
        // {
        //         valueConvert=GAIN_MIC_MID+((value-50)*(GAIN_MIC_MAX-GAIN_MIC_MID)/ 50);
        //         valueToSAM=(((GAIN_MIC_MID_SAM+valueConvert)<<8)&0x0FFFF)|GAIN_LSB;
        // }        
    }else if(type==Type_Echo)
    {
		valueToSAM=func_convertvalueToSam(value,(DWORD)CIRCLE_MID_POS,(DWORD)CIRCLE_MAX_POS,(DWORD)GAIN_ECHO_MID,(DWORD)GAIN_ECHO_MAX);
        // if(value<50)//giá trị từ 0->50
        // {
        //     valueConvert = (value * (GAIN_ECHO_MID_SAM - GAIN_ECHO_MIN_SAM)) / 50 + GAIN_ECHO_MIN_SAM;
        //     valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;            
        // }else   //giá trị từ 50->70 
        // {
        //      valueConvert = ((value-50) * (GAIN_ECHO_MAX_SAM - GAIN_ECHO_MID_SAM)) / 50 + GAIN_ECHO_MID_SAM;
        //     valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;            
        // }
    }else if(type==Type_Delay)
    {
        if(value<50)//giá trị từ 45->75
        {
             valueConvert = (value * (GAIN_DELAY_MID_SAM - GAIN_DELAY_MIN_SAM)) / 50 + GAIN_DELAY_MIN_SAM;
            valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB; 
        }else   //giá trị từ 75->100
        {
             valueConvert = ((value-50) * (GAIN_DELAY_MAX_SAM - GAIN_DELAY_MID_SAM)) / 50 + GAIN_DELAY_MID_SAM;
            valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB; 
        }
    }else if(type==Type_Reverb)
    {
		valueToSAM=func_convertvalueToSam(value,(DWORD)CIRCLE_MID_POS,(DWORD)CIRCLE_MAX_POS,(DWORD)GAIN_REV_MID,(DWORD)GAIN_REV_MAX);
        // if(value<50)//giá trị từ 0->30
        // {
        //      valueConvert = (value * (GAIN_REV_MID_SAM - GAIN_REV_MIN_SAM)) / 50 + GAIN_REV_MIN_SAM;
        //     valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;    
        // }else   //giá trị từ 30->60
        // {
        //       valueConvert = ((value-50) * (GAIN_REV_MAX_SAM - GAIN_REV_MID_SAM)) / 50 + GAIN_REV_MID_SAM;
        //     valueToSAM=((valueConvert<<8)&0x0FFFF)|GAIN_LSB;    
        // }
    }
    TRACE("valueToSAM %x",valueToSAM);
    return valueToSAM;
}

