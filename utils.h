#include <system.h>
#include <trace.h>
#include <sys5000.h>

#define SYS_POWER_LATCH_PORT_DIR P0DIRPORT
#define SYS_POWER_LATCH_PORT_VAL P0VALPORT
#define SYS_POWER_LATCH_PIN  13

#define SYS_POWER_BUTTON_PORT_DIR P0DIRPORT
#define SYS_POWER_BUTTON_PORT_VAL P0VALPORT
#define SYS_POWER_BUTTON_PIN  0


void sys_power_latch(WORD onoff);
BOOL sys_power_button(void);

void Button_1_Press();
void Button_2_Press();
