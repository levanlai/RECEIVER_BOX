#include <system.h>
#include <trace.h>
#include <sys5000.h>
#include "config.h"
#include "lcd/uart.h"

#define SYS_POWER_LATCH_PORT_DIR P0DIRPORT
#define SYS_POWER_LATCH_PORT_VAL P0VALPORT
#define SYS_POWER_LATCH_PIN  13

#define SYS_POWER_BUTTON_PORT_DIR P0DIRPORT
#define SYS_POWER_BUTTON_PORT_VAL P0VALPORT
#define SYS_POWER_BUTTON_PIN  0

#define SYS_CHARGE_DET_PORT_DIR     P0DIRPORT
#define SYS_CHARGE_DET_PORT_VAL     P0VALPORT
#define SYS_CHARGE_DET_PIN  11

#define SYS_PLUGIN_DET_PORT_DIR P0DIRPORT
#define SYS_PLUGIN_DET_PORT_VAL P0VALPORT
#define SYS_PLUGIN_DET_PIN  12//5

extern void bk9532_TurnLED(WORD value);
extern MyData_t  myData;
extern WORD powerState;
extern WORD cnt_SilenceDetect;
void sys_power_latch(WORD onoff)
{
    TRACE("sys_power_latch onoff=%d",onoff);
    _orio(SYS_POWER_LATCH_PORT_DIR, (1 << SYS_POWER_LATCH_PIN));
    if(onoff == 1)//power on
    {
        _orio(SYS_POWER_LATCH_PORT_VAL, (1 << SYS_POWER_LATCH_PIN));
        //bk9532_TurnLED(TRUE);
    }
    else//power off
    {
        _andio(SYS_POWER_LATCH_PORT_VAL, ~(1 << SYS_POWER_LATCH_PIN));
        //bk9532_TurnLED(FALSE);
        //_Sys_Reset();//reset trong debug không có tác dụng
        
    }
    if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0;
}

BOOL sys_power_button(void)
{
    //BOOL val;
    _andio(SYS_POWER_BUTTON_PORT_DIR, ~(1 << SYS_POWER_BUTTON_PIN));

    return (_rdio(SYS_POWER_BUTTON_PORT_VAL) >> SYS_POWER_BUTTON_PIN ) & 0x01;
}
// cắm charge:
// charge det 0
// plugin det 1

// rút charge
// charge det 1
// plugin det 0
BOOL check_charge_det(void)
{
    BOOL val;
    _andio(SYS_CHARGE_DET_PORT_DIR, ~(1 << SYS_CHARGE_DET_PIN));

    return (_rdio(SYS_CHARGE_DET_PORT_VAL) >> SYS_CHARGE_DET_PIN ) & 0x01;
}

BOOL check_plugin_det(void)
{
    BOOL val;
    _andio(SYS_PLUGIN_DET_PORT_DIR, ~(1 << SYS_PLUGIN_DET_PIN));

    return (_rdio(SYS_PLUGIN_DET_PORT_VAL) >> SYS_PLUGIN_DET_PIN ) & 0x01;
}

void Button_1_Press(WORD value)//Move UP
{
    TRACE("Button_1_Press powerState=%d",powerState);
    uart_send_cmd(CMD_MOVE, value);
    if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0; 
}
void Button_2_Press(WORD value)//Move UP
{
    TRACE("Button_2_Press powerState=%d",powerState); 
     uart_send_cmd(CMD_MOVE, value);
    if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0; 
}
void Button_3_Press()
{
    TRACE("Button_3_Press powerState=%d",powerState);
    uart_cmd_parse(CMD_RESET, 0,FALSE); 
   uart_send_cmd(CMD_RESET, 0);  
   if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0; 
}
void Button_4_Press()
{
    TRACE("Button_4_Press powerState=%d",powerState);
     uart_send_cmd(CMD_BT_DISCONNET, 0); 
    if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0;   
}
void Button_Power_Press(WORD value)
{
    TRACE("Button_Power_Press value=%d",value); 
    uart_send_cmd(CMD_SELECT_CHANGE, value);
    if(cnt_SilenceDetect!=0)
        cnt_SilenceDetect=0;
}

