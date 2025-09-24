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
#define SYS_PLUGIN_DET_PIN  5

extern void bk9532_TurnLED(WORD value);
extern MyData_t  myData;
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
        bk9532_TurnLED(FALSE);
        _Sys_Reset();//reset trong debug không có tác dụng
        
    }
}

BOOL sys_power_button(void)
{
    BOOL val;
    _andio(SYS_POWER_BUTTON_PORT_DIR, ~(1 << SYS_POWER_BUTTON_PIN));

    return (_rdio(SYS_POWER_BUTTON_PORT_VAL) >> SYS_POWER_BUTTON_PIN ) & 0x01;
}
// cắm charge:
// charge det 0
// plugin det 0

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

void Button_1_Press()//Move UP
{
    //TRACE("Button_1_Press=%d",onoff);    
    uart_send_cmd(CMD_MOVE, MOVE_UP);
}
void Button_2_Press()//Move UP
{
    //TRACE("Button_2_Press=%d",onoff);
    uart_send_cmd(CMD_MOVE, MOVE_DOWN);
}
WORD testChange=0;
void Button_3_Press()
{
    //TRACE("Button_3_Press=%d",onoff);
    testChange++;
    if(testChange>16)
    testChange=0;
    uart_send_cmd(CMD_SET_CHANGE, testChange);
    // uart_send_cmd(CMD_RESET_FACTORY, 0);
}
void Button_Power_Press()
{
    //TRACE("Button_Power_Press=%d",onoff);
    uart_send_cmd(CMD_SELECT_CHANGE, 0);
}

