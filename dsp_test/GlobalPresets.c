#include "GlobalPresets.h"
#include "midictrl.h"

// Gain preset list. Table ID: 0x0
const globalGainPreset global_gain_presets[GLOBAL_GAIN_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS02
	{ 14847, 0 }
};
const WORD_FAR_P global_preset_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_gain_presets)

};
// Gain preset control list.
const globalGainControl global_gain_controls = 
{
	2, 2, 0x4000, 0x1
};
const WORD_FAR_P global_preset_control_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_gain_controls)

};
const DWORD globalpreset[1][3] = 
{
	// GlobalPresetA
	{ 
	// DSP#1
	0x10002, 0x2, 0x4 }
};
#define GLOBAL_PROCESS_ID_NBIT 8
#define GLOBAL_PRESET_ID_NBIT 12
#define GLOBAL_TABLE_ID_NBIT 12

/*-----------------------------------------------------------------------------
Global preset loader function
 Input: *global_preset_table = pointer to global preset table

-----------------------------------------------------------------------------*/
void dspDesigner_LoadGlobalPreset( PTR32 global_preset_table )
{
	WORD numberOfProcess;
	WORD processIndex, dspIndex;
	DWORD data;
	WORD processId, dspId, tableId, presetNumber;
	
	PTR32 control_table_address =  _cptr32( &global_preset_control_table_address );
	PTR32 preset_table_address = _cptr32( &global_preset_table_address );
	
	for ( dspIndex = 0; dspIndex < GLOBAL_NUMBER_OF_DSP; dspIndex++ )
	{
		data = readExtMemDword( global_preset_table );
		global_preset_table+=sizeof(DWORD);
		numberOfProcess = data & 0xFFFF;
		dspId = (data >> 16) & 0xFFFF;
		for ( processIndex = 0; processIndex < numberOfProcess; processIndex++ )
		{
			data = readExtMemDword( global_preset_table );
			global_preset_table+=sizeof(DWORD);
			processId = (data & ((1<<GLOBAL_PROCESS_ID_NBIT)-1));
			data >>= GLOBAL_PROCESS_ID_NBIT;
			presetNumber = (data & ((1<<GLOBAL_PRESET_ID_NBIT)-1));
			data >>= GLOBAL_PRESET_ID_NBIT;
			tableId = (data & ((1<<GLOBAL_TABLE_ID_NBIT)-1));

			dspDesigner_LoadPreset( dspId-1, processId, 
				_rdxtmem32Ex( control_table_address + (tableId<<1) ),
				_rdxtmem32Ex( preset_table_address + (tableId<<1) ),
				presetNumber );

		}
	}
}

