#ifndef __UART_H__
#define __UART_H__

#include <system.h>
//typedef WORD (*ui_call_func_t)(WORD ch, WORD nrpn, DWORD v, WORD format);

// typedef struct  ui_map_id
// {
// 	WORD id;
// 	// id_ref;
//     ui_call_func_t ui_call_func	;
// }ui_map_id_t;

void uart_init(void);
void uart_send_cmd(WORD cmd, WORD value);
void uart_refresh(void);

void HandleMidiEvent(DWORD midi_event);
void midi_handle_event(void);
void uart_register_task(void);
void uart_task_handler_func(void);
WORD getStatusSyncPanel(void);
void setStatusSyncPanel(WORD value);
WORD getReceiveCmdPowerValue(void);
void uart_send_ID_mic_pair(WORD bus,DWORD id);
DWORD  get_ID_pair_fromPanel(WORD bus);
void  receive_ID_pair_fromPanel(WORD cmd,WORD id);
void uart_cmd_parse(WORD cmd, WORD value);
void changeOutputAudio(WORD value);
void ToogleBASS_BOOST(void);
void TurnBASS_BOOST(WORD value);
void set_detect_Mic_Wire(WORD value);
void set_detect_Guitar(WORD value);
#endif //__UART_H__