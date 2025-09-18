#include <system.h>
#include "midictrl.h"

// Include files needed for handling biquad functions
#include <math.h>
#include "dspDesigner.h"
#include "BiquadCtrl.h"


DWORD rescaleMidiData ( WORD control, DWORD data )
{
	switch (control>>14)
	{
		case 0: data &= 0x7FFF;
			data<<=8; 
			break;
		case 1: data &= 0x7FFF;
			data<<=1; 
			break;
	}
	return data;
}

/*-----------------------------------------------------------------------------
 Preset loader function
 Input:	channel = DSP# -1
		process = Process ID
		*presetControls = pointer to preset controls table
		*presetTable = pointer to preset table
		presetNumber = preset# to load (0..PRESET_NB-1)
------------------------------------------------------------------------------*/
void dspDesigner_LoadPreset( WORD channel, WORD process, PTR32 presetControls, PTR32 presetTable, WORD presetNumber )
{
	PTR32 presetArrayPointer;
	PTR32 presetDataDouble;
	WORD controlIndex;
	WORD presetStructSize, controlCount, control;
	WORD preset_count_biquad;

	preset_count_biquad = _rdxtmemEx( presetControls + 0 );
	controlCount = preset_count_biquad & 0x7FFF;
	presetStructSize= _rdxtmemEx( presetControls + 1 );
	presetArrayPointer = presetTable;
	presetArrayPointer += presetStructSize*presetNumber;
	biquadSetLoadingPreset( preset_count_biquad >> 15 );
	for ( controlIndex = 2; controlIndex < controlCount+2; controlIndex++ )
	{
		control = _rdxtmemEx( presetControls + controlIndex );
		if ( control == 0xF00D ) continue; // Delay preset extra data
		if ( (control>>14) > 1 )
		{
			presetDataDouble = presetArrayPointer;
			dspDesigner_HandleNRPN( channel, control&0xFF | (process&0x3F)<<8, rescaleMidiData( control, readExtMemDword( presetDataDouble ) ), FORMAT_28BIT_PRECISION );
			presetArrayPointer += sizeof(DWORD);
		}
		else
		{
			dspDesigner_HandleNRPN( channel, control&0xFF | (process&0x3F)<<8, rescaleMidiData( control, _rdxtmemEx( presetArrayPointer ) ), FORMAT_28BIT_PRECISION );
			presetArrayPointer++;
		}
	}
	biquadSetLoadingPreset( 0 );
}
