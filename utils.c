#include <system.h>
#include <trace.h>
#include <sys5000.h>

#define SYS_POWER_LATCH_PORT_DIR P0DIRPORT
#define SYS_POWER_LATCH_PORT_VAL P0VALPORT
#define SYS_POWER_LATCH_PIN  13

#define SYS_POWER_BUTTON_PORT_DIR P0DIRPORT
#define SYS_POWER_BUTTON_PORT_VAL P0VALPORT
#define SYS_POWER_BUTTON_PIN  0


void sys_power_latch(WORD onoff)
{
    TRACE("sys_power_latch onoff=%d",onoff);
    _orio(SYS_POWER_LATCH_PORT_DIR, (1 << SYS_POWER_LATCH_PIN));
    if(onoff == 1)
    {
        _orio(SYS_POWER_LATCH_PORT_VAL, (1 << SYS_POWER_LATCH_PIN));
    }
    else
    {
        _andio(SYS_POWER_LATCH_PORT_VAL, ~(1 << SYS_POWER_LATCH_PIN));
    }
}

BOOL sys_power_button(void)
{
    BOOL val;
    _andio(SYS_POWER_BUTTON_PORT_DIR, ~(1 << SYS_POWER_BUTTON_PORT_DIR));

    return (_rdio(SYS_POWER_BUTTON_PORT_VAL) >> SYS_POWER_BUTTON_PIN ) & 0x01;
}
