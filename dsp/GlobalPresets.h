#ifndef __GLOBALPRESETS_H__
#define __GLOBALPRESETS_H__

#include <system.h>


/* Global preset header file */


void dspDesigner_LoadGlobalPreset( PTR32 global_preset_table );

// Gain preset structure.
typedef struct 
{
	WORD parameter1; // Gain
	WORD parameter2; // 180deg
} globalGainPreset;

// Biquad[5] preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // 180deg
	WORD parameter3; // Gain
	WORD parameter4; // Type1
	WORD parameter5; // Q1
	DWORD parameter6; // Freq1
	WORD parameter7; // Gain1
	WORD parameter8; // Type2
	WORD parameter9; // Q2
	DWORD parameter10; // Freq2
	WORD parameter11; // Gain2
	WORD parameter12; // Type3
	WORD parameter13; // Q3
	DWORD parameter14; // Freq3
	WORD parameter15; // Gain3
	WORD parameter16; // Type4
	WORD parameter17; // Q4
	DWORD parameter18; // Freq4
	WORD parameter19; // Gain4
	WORD parameter20; // Type5
	WORD parameter21; // Q5
	DWORD parameter22; // Freq5
	WORD parameter23; // Gain5
} globalBiquad5Preset;

// Biquad[3] preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // 180deg
	WORD parameter3; // Gain
	WORD parameter4; // Type1
	WORD parameter5; // Q1
	DWORD parameter6; // Freq1
	WORD parameter7; // Gain1
	WORD parameter8; // Type2
	WORD parameter9; // Q2
	DWORD parameter10; // Freq2
	WORD parameter11; // Gain2
	WORD parameter12; // Type3
	WORD parameter13; // Q3
	DWORD parameter14; // Freq3
	WORD parameter15; // Gain3
} globalBiquad3Preset;

// MixN[3] preset structure.
typedef struct 
{
	WORD parameter1; // 180deg1
	WORD parameter2; // 180deg2
	WORD parameter3; // 180deg3
	WORD parameter4; // Gain1
	WORD parameter5; // Gain2
	WORD parameter6; // Gain3
} globalMixN3Preset;

// MixN[2] preset structure.
typedef struct 
{
	WORD parameter1; // 180deg1
	WORD parameter2; // 180deg2
	WORD parameter3; // Gain1
	WORD parameter4; // Gain2
} globalMixN2Preset;

// FreqShifter preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // PostHP
	WORD parameter3; // Shift
	WORD parameter4; // MaxScale
	WORD parameter5; // Gain
	WORD parameter6; // 180deg
} globalFreqShifterPreset;

// NoiseGate preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // Threshold
	WORD parameter3; // Attack
	WORD parameter4; // Release
	WORD parameter5; // Gain
	WORD parameter6; // 180deg
} globalNoiseGatePreset;

// Exciter preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // Gain
	WORD parameter3; // ThresldLow
	WORD parameter4; // ThrsldHigh
	WORD parameter5; // LowContour
	WORD parameter6; // Intensity
} globalExciterPreset;

// Feedback canceller preset structure.
typedef struct 
{
	WORD parameter1; // Bypass
	WORD parameter2; // LockFilter
	WORD parameter3; // Mode
} globalFeedback_cancellerPreset;

// Compressor preset structure.
typedef struct 
{
	WORD parameter1; // ON/OFF
	WORD parameter2; // Threshold
	WORD parameter3; // Ratio
	WORD parameter4; // Boost
	WORD parameter5; // 180deg
} globalCompressorPreset;

// LevelDetect preset structure.
typedef struct 
{
	WORD parameter1; // Release
	WORD parameter2; // Attack
} globalLevelDetectPreset;

// (s)Reverb/Echo preset structure.
typedef struct 
{
	WORD parameter1; // Program
	WORD parameter2; // RevInLevel
	WORD parameter3; // RevLevel
	WORD parameter4; // RevPreHP
	WORD parameter5; // RevHDamp
	WORD parameter6; // RevTime
	WORD parameter7; // RvToneGain
	WORD parameter8; // RvToneFq
	WORD parameter9; // EchoInLvl
	WORD parameter10; // EchoTime
	WORD parameter11; // Time x2
	WORD parameter12; // EchoLDamp
	WORD parameter13; // EchoHDamp
	WORD parameter14; // EchoFeedback
	WORD parameter15; // EchoOutLvlL
	WORD parameter16; // EchoPhaseL
	WORD parameter17; // EchoOutLvlR
	WORD parameter18; // EchoPhaseR
} globalS_Reverb_EchoPreset;


// Gain preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[2]; // controls array
} globalGainControl;

// Biquad[5] preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[23]; // controls array
} globalBiquad5Control;

// Biquad[3] preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[15]; // controls array
} globalBiquad3Control;

// MixN[3] preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[6]; // controls array
} globalMixN3Control;

// MixN[2] preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[4]; // controls array
} globalMixN2Control;

// FreqShifter preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[6]; // controls array
} globalFreqShifterControl;

// NoiseGate preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[6]; // controls array
} globalNoiseGateControl;

// Exciter preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[6]; // controls array
} globalExciterControl;

// Feedback canceller preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[3]; // controls array
} globalFeedback_cancellerControl;

// Compressor preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[5]; // controls array
} globalCompressorControl;

// LevelDetect preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[2]; // controls array
} globalLevelDetectControl;

// (s)Reverb/Echo preset control structure.
typedef struct 
{
	WORD numberOfControls; // number of controls
	WORD sizeOfStruct; // size of this Structure
	WORD control[18]; // controls array
} globalS_Reverb_EchoControl;

#define	GLOBAL_GAIN_PRESET_NB	1
extern const globalGainPreset global_gain_presets[GLOBAL_GAIN_PRESET_NB];
#define	GLOBAL_BIQUAD5_PRESET_NB	2
extern const globalBiquad5Preset global_biquad5_presets[GLOBAL_BIQUAD5_PRESET_NB];
#define	GLOBAL_BIQUAD3_PRESET_NB	2
extern const globalBiquad3Preset global_biquad3_presets[GLOBAL_BIQUAD3_PRESET_NB];
#define	GLOBAL_MIXN3_PRESET_NB	1
extern const globalMixN3Preset global_mixn3_presets[GLOBAL_MIXN3_PRESET_NB];
#define	GLOBAL_MIXN2_PRESET_NB	1
extern const globalMixN2Preset global_mixn2_presets[GLOBAL_MIXN2_PRESET_NB];
#define	GLOBAL_FREQSHIFTER_PRESET_NB	1
extern const globalFreqShifterPreset global_freqshifter_presets[GLOBAL_FREQSHIFTER_PRESET_NB];
#define	GLOBAL_NOISEGATE_PRESET_NB	1
extern const globalNoiseGatePreset global_noisegate_presets[GLOBAL_NOISEGATE_PRESET_NB];
#define	GLOBAL_EXCITER_PRESET_NB	1
extern const globalExciterPreset global_exciter_presets[GLOBAL_EXCITER_PRESET_NB];
#define	GLOBAL_FEEDBACK_CANCELLER_PRESET_NB	1
extern const globalFeedback_cancellerPreset global_feedback_canceller_presets[GLOBAL_FEEDBACK_CANCELLER_PRESET_NB];
#define	GLOBAL_COMPRESSOR_PRESET_NB	2
extern const globalCompressorPreset global_compressor_presets[GLOBAL_COMPRESSOR_PRESET_NB];
#define	GLOBAL_LEVELDETECT_PRESET_NB	1
extern const globalLevelDetectPreset global_leveldetect_presets[GLOBAL_LEVELDETECT_PRESET_NB];
#define	GLOBAL_S_REVERB_ECHO_PRESET_NB	1
extern const globalS_Reverb_EchoPreset global_s_reverb_echo_presets[GLOBAL_S_REVERB_ECHO_PRESET_NB];
#define	NUMBER_OF_PRESET_TABLE	12
extern const WORD_FAR_P global_preset_table_address[NUMBER_OF_PRESET_TABLE];
extern const globalGainControl global_gain_controls;
extern const globalBiquad5Control global_biquad5_controls;
extern const globalBiquad3Control global_biquad3_controls;
extern const globalMixN3Control global_mixn3_controls;
extern const globalMixN2Control global_mixn2_controls;
extern const globalFreqShifterControl global_freqshifter_controls;
extern const globalNoiseGateControl global_noisegate_controls;
extern const globalExciterControl global_exciter_controls;
extern const globalFeedback_cancellerControl global_feedback_canceller_controls;
extern const globalCompressorControl global_compressor_controls;
extern const globalLevelDetectControl global_leveldetect_controls;
extern const globalS_Reverb_EchoControl global_s_reverb_echo_controls;
extern const WORD_FAR_P global_preset_control_table_address[NUMBER_OF_PRESET_TABLE];
#define	GLOBAL_NUMBER_OF_DSP	4
extern const DWORD globalpreset[1][54];

#endif //__GLOBALPRESETS_H__