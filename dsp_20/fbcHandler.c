#include "fbcHandler.h"

#define _FBCancel_UpdateFBCFilters(a, b, c) _FBCancel_Biquad_UpdateCoeff( a, 0, b, c )

FBCStatus fbcStatus1;

// FBC status initialization
void FBC_InitStatus( FBCStatus *fbcStatus, WORD fbcdsp, WORD currentQ, WORD currentAttenuation )
{
	WORD i;
	
	fbcStatus->fbcdsp = fbcdsp;

	for (i=0; i<FBC_BANDNB; i++)
	{
		// Fixed point vars
		fbcStatus->q[i] = currentQ;
		fbcStatus->attenuation[i] = currentAttenuation;
		
		// Floating point vars
		fbcStatus->fbcFilters[i].f0 = 0;
		fbcStatus->fbcFilters[i].Q = FLOAT_SQ2B2;
		fbcStatus->fbcFilters[i].dBGain = 0;
		fbcStatus->fbcFilters[i].fs = GetSystemSamplingRate();
	}
	
	FBCancel_SetCurrentAttenuation( fbcStatus, currentAttenuation );
	FBCancel_SetCurrentQ( fbcStatus, currentQ );
	
}

#define FBCATTENUATION_ITEMNB 14
#define FBCQ_ITEMNB 16

int FBC_ATTENUATION_TABLE[FBCATTENUATION_ITEMNB] = { -52, -48, -44, -40, -36, -32, -28, -24, -20, -16, -12, -8, -4, 0 }; // Attenuation table	// Attenuation in dB
WORD FBC_Q_TABLE[FBCQ_ITEMNB] = { 57, 54, 51, 48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 14, 7 };	// Q table

/*
* This function allows to place automatically notch filters when an acoustic feedback occurs.
*/
void FBC_BaseLevel( FBCStatus *fbcStatus )
{
	WORD newFrequencyIndex, stable;
	_FILTER_PARAM *theFBCIIRFilter;
	_BIQUAD_COEFF iirCoeff; // Floating point
	_BIQUAD_FIXEDCOEFF icfixed; // Fixed point

	theFBCIIRFilter = fbcStatus->fbcFilters;
	
	// Check if FBC detected a new frequency (from DSP)
	if ( (newFrequencyIndex = _FBCancel_CheckForNewFrequency( fbcStatus->fbcdsp )) != -1 ) 
	{	
			// Update structure with new frequency found by FBC
			theFBCIIRFilter[newFrequencyIndex].f0 = _FBCancel_GetFloatFrequency( fbcStatus->fbcdsp, newFrequencyIndex ); // _float(_FBCancel_GetFrequency( fbcStatus->fbcdsp, newFrequencyIndex ));	
			fbcStatus->attenuation[newFrequencyIndex] = FBCancel_GetCurrentAttenuation( fbcStatus ); // attenuation = 0 (-52dB by default)
			theFBCIIRFilter[newFrequencyIndex].dBGain = _float(FBC_ATTENUATION_TABLE[fbcStatus->attenuation[newFrequencyIndex]]);		// -52dB by default (!!must be cast to int)
		
			if ( !_fgt( theFBCIIRFilter[newFrequencyIndex].f0, 0x42260000 ) )	// _float(41.500000) = 0x42260000 (ALT+F Key)
			{	// if f0 <= 41.5Hz -> Q=1/12oct
				theFBCIIRFilter[newFrequencyIndex].Q = _float( FBC_Q_TABLE[6] ); 
				fbcStatus->q[newFrequencyIndex] = 13;
			}
			else
			{	// use current Q settings
				fbcStatus->q[newFrequencyIndex] = FBCancel_GetCurrentQ( fbcStatus );							
				theFBCIIRFilter[newFrequencyIndex].Q = _float( FBC_Q_TABLE[fbcStatus->q[newFrequencyIndex]] ); 					// if f0 > 187.5 -> Q = current Q
			}

			stable = _Biquad_FltFloatCoeff( _BIQUAD_FLTPTR(_Biquad_FilterPBS), &theFBCIIRFilter[newFrequencyIndex], &iirCoeff ); // computes IIR coefficient from PBS(Hi-Q) filter type
			if (stable)
			{
				_Biquad_FloatToFixCoeff( &iirCoeff, &icfixed, _BIQUAD_FRACTBITS_1_2_21 ); // converts float coefficients into fixed point format (default 1.2.21)
				_FBCancel_UpdateFBCFilters( fbcStatus->fbcdsp, &icfixed, newFrequencyIndex );
			}		
	}
}

/* Function for loading 1 FBC notch filter (update all parameters (f0, att, Q) 
 Input:
	- fbcStatus: pointer to FBC status structure
	- frqIndex: [0..11] place where to put notch filter
	- frqValue: frequency value(in Hertz, fixed point
	- QIndex: Index of Q value -> (see FBC_Q_TABLE for Q value)
	- AttIndex: Index of Attenuation value -> (see FBC_ATTENUATION_TABLE for attenuation value)
*/
void FBC_LoadFrequency( FBCStatus *fbcStatus, WORD frqIndex, WORD frqValue, WORD QIndex, WORD AttIndex )
{
	_FILTER_PARAM *theFBCIIRFilter;
	_BIQUAD_COEFF iirCoeff;	// Floating point
	_BIQUAD_FIXEDCOEFF icfixed; // Fixed point
	WORD stable;
	
	if (frqIndex > FBC_BANDNB-1) return;
	
	_FBCancel_SetFrequency( fbcStatus->fbcdsp, frqIndex, frqValue );
	theFBCIIRFilter = fbcStatus->fbcFilters;
	theFBCIIRFilter[frqIndex].f0 = _float( frqValue );		// update 
	fbcStatus->attenuation[frqIndex] = AttIndex;
	theFBCIIRFilter[frqIndex].dBGain = _float( FBC_ATTENUATION_TABLE[AttIndex] );

	fbcStatus->q[frqIndex] = QIndex;							
	theFBCIIRFilter[frqIndex].Q = _float( FBC_Q_TABLE[QIndex] ); 
		
	stable = _Biquad_FltFloatCoeff( _BIQUAD_FLTPTR(_Biquad_FilterPBS), &theFBCIIRFilter[frqIndex], &iirCoeff ); // computes IIR coefficient from PBS(Hi-Q) filter type
	if (stable)
	{
		_Biquad_FloatToFixCoeff( &iirCoeff, &icfixed, _BIQUAD_FRACTBITS_1_2_21 ); // converts float coefficients into fixed point format (default 1.2.21)
		_FBCancel_UpdateFBCFilters( fbcStatus->fbcdsp, &icfixed, frqIndex );
	}

}

void FBCancel_SetCurrentQ( FBCStatus *fbcStatus, WORD value )
{
	if (value>=FBCQ_ITEMNB)
		value = FBCQ_ITEMNB-1;
	fbcStatus->currentQ = value;
}

WORD FBCancel_GetCurrentQ( FBCStatus *fbcStatus )
{
	return ( fbcStatus->currentQ );
}

void FBCancel_SetCurrentAttenuation( FBCStatus *fbcStatus, WORD value )
{
	if (value>=FBCATTENUATION_ITEMNB)
		value = FBCATTENUATION_ITEMNB-1;
	fbcStatus->currentAttenuation = value;
}

WORD FBCancel_GetCurrentAttenuation( FBCStatus *fbcStatus )
{
	return ( fbcStatus->currentAttenuation );
}

// Get system sampling rate and converts to floating point value
FLOAT GetSystemSamplingRate(void)
{
	return((FLOAT)_float(_Sys_GetSamplingRate()));
}

void FBCStatus_Init( void )
{
	FBC_InitStatus( &fbcStatus1, dsp[2], 0, 0 );
}

void FBC_BaseLevels( void )
{
	FBC_BaseLevel( &fbcStatus1 );
}
