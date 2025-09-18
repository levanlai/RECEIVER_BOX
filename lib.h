#ifndef _DOLIB_H  
#define _DOLIB_H

void setBit(WORD Reg, WORD bitPosition);
void clearBit(WORD Reg, WORD bitPosition);
WORD func_SendValueToSAM(WORD ch, WORD nrpn, DWORD v, WORD format);
DWORD ConvertValueToSAM(DWORD value,WORD type) ;
//void func_sync_sendto_panel(void);
void update_func_value_to_panel(void);
//DWORD func_convertvalueToSam(DWORD value,DWORD midcicle,DWORD maxcicle,DWORD middB,DWORD maxdB);
void parse_func_value_from_panel(WORD value);
#endif