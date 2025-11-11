#ifndef __FBCHANDLER_H__
#define __FBCHANDLER_H__

#include <system.h>
#include <libFX5000.h>
#include <math.h>

extern WORD dsp[];

typedef struct FBCStatus_tag
{
	WORD fbcdsp;
	_FILTER_PARAM fbcFilters[FBC_BANDNB];
	WORD q[FBC_BANDNB];					// Q for each filter
	WORD attenuation[FBC_BANDNB];		// Att for each filter
	WORD currentQ;
	WORD currentAttenuation;
} FBCStatus;

FLOAT GetSystemSamplingRate(void);
void FBC_InitStatus( FBCStatus *fbcStatus, WORD fbcdsp, WORD currentQ, WORD currentAttenuation );
void FBC_BaseLevel( FBCStatus *fbcStatus );
void FBC_LoadFrequency( FBCStatus *fbcStatus, WORD frqIndex, WORD frqValue, WORD QIndex, WORD AttIndex );
void FBCancel_SetCurrentQ( FBCStatus *fbcStatus, WORD value );
WORD FBCancel_GetCurrentQ( FBCStatus *fbcStatus );
void FBCancel_SetCurrentAttenuation( FBCStatus *fbcStatus, WORD value );
WORD FBCancel_GetCurrentAttenuation( FBCStatus *fbcStatus );
FLOAT _FBCancel_GetFloatFrequency( WORD DSPid, WORD n );
void FBCStatus_Init( void );
void FBC_BaseLevels( void );

extern FBCStatus fbcStatus1;

#endif //__FBCHANDLER_H__