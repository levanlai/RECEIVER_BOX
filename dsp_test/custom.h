// This file can be modified by user and will not be overwritten by Dream DSP Designer.
#ifndef __CUSTOM_H__
#define __CUSTOM_H__

//DSP#1 custom functions, uncomment the '#define' line where you want to add custom code, and implement the function below the '#define' in a separate C file
//#define _customPreInitFunction1
WORD customPreInitFunction1( WORD dspId );
//#define _customPostInitFunction1
WORD customPostInitFunction1( WORD dspId );
//#define _customPreNrpnFunction1
WORD customPreNrpnFunction1( WORD dspId, WORD nrpn, DWORD *pval, WORD format );
//#define _customPostNrpnFunction1
WORD customPostNrpnFunction1( WORD dspId, WORD nrpn, WORD val8bit, WORD value, DWORD dvalue );


#endif //__CUSTOM_H__