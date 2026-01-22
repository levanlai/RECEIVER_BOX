#ifndef _DOLIB_H  
#define _DOLIB_H

void SysVarInit(void);
void setBit(WORD Reg, WORD bitPosition);
void clearBit(WORD Reg, WORD bitPosition);
WORD func_SendValueToSAM(WORD ch, WORD nrpn, DWORD v, WORD format);
DWORD ConvertValueToSAM(DWORD value,WORD type) ;
void parse_func_value_from_panel(WORD value);
void check_mics_connect(WORD iSend);
void SaveFlash(void);
void resetFactory(void);
DWORD cpu_swap_edian(DWORD i);
WORD CRC8_Array(DWORD data,WORD len);
//void parseDataFromMic(DWORD usrdata);
WORD checkRangeValue(WORD cmd,int value);
WORD checkCMDGainEQ(WORD cmd);
WORD checkCMD_HPF(WORD cmd);
#endif