#include "GlobalPresets.h"
#include "midictrl.h"

// (s)Reverb/Echo preset list. Table ID: 0x0
const globalS_Reverb_EchoPreset global_s_reverb_echo_presets[GLOBAL_S_REVERB_ECHO_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS01
	{ 12, 8192, 8192, 1251, 8192, 8192, 8192, 8192, 8192, 10981, 0, 8192, 8192, 11468, 8192, 127, 8192, 127 }
};
// Gain preset list. Table ID: 0x1
const globalGainPreset global_gain_presets[GLOBAL_GAIN_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS02
	{ 14847, 0 }
};
// NoiseGate preset list. Table ID: 0x2
const globalNoiseGatePreset global_noisegate_presets[GLOBAL_NOISEGATE_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS03
	{ 0, 15615, 8192, 8192, 14847, 0 }
};
// FreqShifter preset list. Table ID: 0x3
const globalFreqShifterPreset global_freqshifter_presets[GLOBAL_FREQSHIFTER_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS04
	{ 0, 0, 8192, 0, 16383, 0 }
};
// MixN[2] preset list. Table ID: 0x4
const globalMixN2Preset global_mixn2_presets[GLOBAL_MIXN2_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS05
	{ 0, 0, 14847, 14847 }
};
// MixN[3] preset list. Table ID: 0x5
const globalMixN3Preset global_mixn3_presets[GLOBAL_MIXN3_PRESET_NB] = 
{
	// GlobalPresetA_DSP01-PCS07
	{ 0, 0, 0, 14847, 14847, 14847 }
};
// Feedback canceller preset list. Table ID: 0x6
const globalFeedback_cancellerPreset global_feedback_canceller_presets[GLOBAL_FEEDBACK_CANCELLER_PRESET_NB] = 
{
	// GlobalPresetA_DSP03-PCS01
	{ 1, 0, 0 }
};
// Biquad[6] preset list. Table ID: 0x7
const globalBiquad6Preset global_biquad6_presets[GLOBAL_BIQUAD6_PRESET_NB] = 
{
	// GlobalPresetA_DSP04-PCS01
	{ 1, 0, 14847, 7, 955, 1228800, 64, 1, 955, 6144000, 64, 6, 955, 20480000, 64, 1, 955, 81956, 64, 1, 955, 81968, 64, 1, 955, 81980, 64 }
};
// LevelDetect preset list. Table ID: 0x8
const globalLevelDetectPreset global_leveldetect_presets[GLOBAL_LEVELDETECT_PRESET_NB] = 
{
	// GlobalPresetA_DSP04-PCS03
	{ 8192, 8192 }
};
// Compressor preset list. Table ID: 0x9
const globalCompressorPreset global_compressor_presets[GLOBAL_COMPRESSOR_PRESET_NB] = 
{
	// GlobalPresetA_DSP04-PCS04
	{ 0, 15615, 8192, 0, 0 }
};
// Delay preset list. Table ID: 0xa
const globalDelayPreset global_delay_presets[GLOBAL_DELAY_PRESET_NB] = 
{
	// GlobalPresetA_DSP04-PCS0A
	{ 0, 0, 14847, 0, 1 }
};
const WORD_FAR_P global_preset_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_s_reverb_echo_presets), 
	FARPTR((const WORD*)&global_gain_presets), FARPTR((const WORD*)&global_noisegate_presets), FARPTR((const WORD*)&global_freqshifter_presets), FARPTR((const WORD*)&global_mixn2_presets), 
	FARPTR((const WORD*)&global_mixn3_presets), FARPTR((const WORD*)&global_feedback_canceller_presets), FARPTR((const WORD*)&global_biquad6_presets), FARPTR((const WORD*)&global_leveldetect_presets), 
	FARPTR((const WORD*)&global_compressor_presets), FARPTR((const WORD*)&global_delay_presets)
};
// (s)Reverb/Echo preset control list.
const globalS_Reverb_EchoControl global_s_reverb_echo_controls = 
{
	18, 18, 0x0, 0x4001, 0x4002, 0x4003, 0x4004, 0x4005, 0x4006, 0x4007, 0x4008, 0x4009, 0xa, 0x400b, 0x400c, 0x400d, 0x400e, 0xf, 0x4010, 0x11
};
// Gain preset control list.
const globalGainControl global_gain_controls = 
{
	2, 2, 0x4000, 0x1
};
// NoiseGate preset control list.
const globalNoiseGateControl global_noisegate_controls = 
{
	6, 6, 0x0, 0x4001, 0x4002, 0x4003, 0x4004, 0x5
};
// FreqShifter preset control list.
const globalFreqShifterControl global_freqshifter_controls = 
{
	6, 6, 0x0, 0x1, 0x4002, 0x4003, 0x4004, 0x5
};
// MixN[2] preset control list.
const globalMixN2Control global_mixn2_controls = 
{
	4, 4, 0x0, 0x1, 0x401f, 0x4020
};
// MixN[3] preset control list.
const globalMixN3Control global_mixn3_controls = 
{
	6, 6, 0x0, 0x1, 0x2, 0x401f, 0x4020, 0x4021
};
// Feedback canceller preset control list.
const globalFeedback_cancellerControl global_feedback_canceller_controls = 
{
	3, 3, 0x0, 0x1, 0x3
};
// Biquad[6] preset control list.
const globalBiquad6Control global_biquad6_controls = 
{
	0x801b, 33, 0x0, 0x1, 0x4002, 0x3, 0x4022, 0x8041, 0x60, 0x4, 0x4023, 0x8042, 0x61, 0x5, 0x4024, 0x8043, 0x62, 0x6, 0x4025, 0x8044, 0x63, 0x7, 0x4026, 0x8045, 0x64, 0x8, 0x4027, 0x8046, 0x65
};
// LevelDetect preset control list.
const globalLevelDetectControl global_leveldetect_controls = 
{
	2, 2, 0x4000, 0x4001
};
// Compressor preset control list.
const globalCompressorControl global_compressor_controls = 
{
	5, 5, 0x1, 0x4002, 0x4003, 0x4004, 0x5
};
// Delay preset control list.
const globalDelayControl global_delay_controls = 
{
	5, 6, 0x0, 0x8001, 0x4002, 0x3, 0xf00d
};
const WORD_FAR_P global_preset_control_table_address[NUMBER_OF_PRESET_TABLE] = 
{
	FARPTR((const WORD*)&global_s_reverb_echo_controls), 
	FARPTR((const WORD*)&global_gain_controls), FARPTR((const WORD*)&global_noisegate_controls), FARPTR((const WORD*)&global_freqshifter_controls), FARPTR((const WORD*)&global_mixn2_controls), 
	FARPTR((const WORD*)&global_mixn3_controls), FARPTR((const WORD*)&global_feedback_canceller_controls), FARPTR((const WORD*)&global_biquad6_controls), FARPTR((const WORD*)&global_leveldetect_controls), 
	FARPTR((const WORD*)&global_compressor_controls), FARPTR((const WORD*)&global_delay_controls)
};
const DWORD globalpreset[1][33] = 
{
	// GlobalPresetA
	{ 
	// DSP#1
	0x10008, 0x1, 0x100002, 0x200003, 0x300004, 0x400005, 0x400006, 0x500007, 0x100008,
	// DSP#2
	0x20008, 0x100001, 0x200002, 0x300003, 0x100004, 0x5, 0x400006, 0x400007, 0x500008,
	// DSP#3
	0x30002, 0x600001, 0x100002,
	// DSP#4
	0x4000b, 0x700001, 0x700002, 0x800003, 0x900004, 0x100005, 0x800006, 0x900007, 0x100008, 0x400009, 0xa0000a, 0xa0000b }
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

