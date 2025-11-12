#ifndef __UART_H__
#define __UART_H__

#include <system.h>

void uart_init(void);
void uart_send_cmd(WORD cmd, WORD value);
void uart_cmd_parse(WORD cmd, WORD value,WORD iInit);
void TurnFBC(WORD value,WORD iInit);
DWORD cmd_execute(WORD cmd, WORD value,WORD iInit,WORD iLink,DWORD valueSam);
#endif //__UART_H__