#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"
#include "memorymap.h"
#include <trace.h>
// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 1


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD3BANDCOUNT	4

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad1Parameters3[BIQUAD3BANDCOUNT];
WORD biquad1Type3[BIQUAD3BANDCOUNT];
WORD biquad1XoverType3[BIQUAD3BANDCOUNT];
DWORD biquad1RawFrequency3[BIQUAD3BANDCOUNT];

BiquadParameters biquad1ParamAddr3 = { biquad1Parameters3, biquad1Type3, biquad1XoverType3, biquad1RawFrequency3, BIQUAD3BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp1pcs[11];

WORD dsp1InitAndRoute(void)
{
	WORD dspId;

	dspId = _LiveMic_Init( MEMADDR_DSPINIT01_PCS00PAR00 );

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction1
customPreInitFunction1( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #1: Gain
	dsp1pcs[1] = _LiveMic_Gain_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[1], PCS_DSP_IN | 0 );
	_LiveMic_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[1], PCS_NODE | 6 );

	// Process #2: NoiseGate
	dsp1pcs[2] = _LiveMic_NoiseGate_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, NOISEGATE_SAMPLE_IN|dsp1pcs[2], PCS_NODE | 6 );
	_LiveMic_SetProcOUT( dspId, NOISEGATE_SAMPLE_OUT|dsp1pcs[2], PCS_NODE | 7 );

	// Process #3: Biquad
	dsp1pcs[3] = _LiveMic_Biquad_Allocate( dspId, BIQUAD3BANDCOUNT );
	_LiveMic_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp1pcs[3], PCS_NODE | 7 );
	_LiveMic_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp1pcs[3], PCS_NODE | 8 );

	// Process #4: LevelDetect
	dsp1pcs[4] = _LiveMic_LevelDetect_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp1pcs[4], PCS_NODE | 8 );

	// Process #5: Compressor
	dsp1pcs[5] = _LiveMic_Compressor_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp1pcs[5], PCS_NODE | 8 );
	_LiveMic_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp1pcs[5], PCS_NODE | 10 );
	_LiveMic_Compressor_ConnectLevel( dspId, dsp1pcs[5], dsp1pcs[4] );

	// Process #6: (s)Reverb/Echo
	_LiveMic_SetProcIN( dspId, LIVEMIC_REVERB_SAMPLE_IN, PCS_NODE | 10 );
	_LiveMic_SetProcIN( dspId, LIVEMIC_ECHO_SAMPLE_IN, PCS_NODE | 10 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTL, PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTR, PCS_NODE | 3 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTL, PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTR, PCS_NODE | 1 );

	// Process #10: Gain
	dsp1pcs[10] = _LiveMic_Gain_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp1pcs[10], PCS_NODE | 10 );
	_LiveMic_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp1pcs[10], PCS_DSP_OUT | 1 );

	// Process #7: MixN
	dsp1pcs[7] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[7], PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[7], PCS_NODE | 5 );

	// Process #9: MixN
	dsp1pcs[9] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[9], PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[9], PCS_NODE | 4 );

	// Process #8: MixN
	dsp1pcs[8] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp1pcs[8], PCS_NODE | 4 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp1pcs[8], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction1
customPostInitFunction1( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn1List[NUMBEROFCOMMAND1][2]=
{
	{ 0x0100, 0x0030 }, // _LiveMic_Gain_Value
	{ 0x0101, 0x0031 }, // _LiveMic_Gain_Phase
	{ 0x0200, 0x0038 }, // _LiveMic_NoiseGate_OnOff
	{ 0x0201, 0x0039 }, // _LiveMic_NoiseGate_Threshold
	{ 0x0202, 0x003A }, // _LiveMic_NoiseGate_Attack
	{ 0x0203, 0x003B }, // _LiveMic_NoiseGate_Release
	{ 0x0204, 0x003C }, // _LiveMic_NoiseGate_InGainValue
	{ 0x0205, 0x003D }, // _LiveMic_NoiseGate_InGainPhase
	{ 0x0300, 0x0012 }, // _LiveMic_Biquad_OnOff
	{ 0x0301, 0x0013 }, // _LiveMic_Biquad_InGainPhase
	{ 0x0302, 0x0014 }, // _LiveMic_Biquad_InGainValue
	{ 0x0400, 0x0034 }, // _LiveMic_LevelDetect_Attack
	{ 0x0401, 0x0035 }, // _LiveMic_LevelDetect_Release
	{ 0x0500, 0x001F }, // _LiveMic_Compressor_GetGainReduction
	{ 0x0501, 0x0020 }, // _LiveMic_Compressor_OnOff
	{ 0x0502, 0x0021 }, // _LiveMic_Compressor_Threshold
	{ 0x0503, 0x0022 }, // _LiveMic_Compressor_Ratio
	{ 0x0504, 0x0023 }, // _LiveMic_Compressor_Boost
	{ 0x0505, 0x0024 }, // _LiveMic_Compressor_BoostPhase
	{ 0x0600, 0x0000 }, // _LiveMic_Effect_LoadProgram
	{ 0x0601, 0x0001 }, // _LiveMic_Effect_RevInputLevel
	{ 0x0602, 0x0002 }, // _LiveMic_Effect_RevLevel
	{ 0x0603, 0x0003 }, // _LiveMic_Effect_RevPreHP
	{ 0x0604, 0x0004 }, // _LiveMic_Effect_RevHDamp
	{ 0x0605, 0x0005 }, // _LiveMic_Effect_RevTime
	{ 0x0606, 0x0006 }, // _LiveMic_Effect_RevToneGain
	{ 0x0607, 0x0007 }, // _LiveMic_Effect_RevToneFreq
	{ 0x0608, 0x0008 }, // _LiveMic_Effect_EchoInputLevel
	{ 0x0609, 0x0009 }, // _LiveMic_Effect_EchoTime
	{ 0x060A, 0x000A }, // _LiveMic_Effect_LongEchoMode
	{ 0x060B, 0x000B }, // _LiveMic_Effect_EchoLDamp
	{ 0x060C, 0x000C }, // _LiveMic_Effect_EchoHDamp
	{ 0x060D, 0x000D }, // _LiveMic_Effect_EchoFeedback
	{ 0x060E, 0x000E }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x060F, 0x000F }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0610, 0x0010 }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x0611, 0x0011 }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0700, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x071F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0800, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x081F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0900, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x091F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0A00, 0x0030 }, // _LiveMic_Gain_Value
	{ 0x0A01, 0x0031 } // _LiveMic_Gain_Phase

};

#define NB_BIQUAD_COMMAND 4
const BiquadParamsTable nrpn1BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0303, 0x4015, &biquad1ParamAddr3 },
	{ 0x0322, 0x4016, &biquad1ParamAddr3 },
	{ 0x0341, 0x4017, &biquad1ParamAddr3 },
	{ 0x0360, 0x4018, &biquad1ParamAddr3 }
};

WORD dsp1NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	BiquadParameters *theBiquad;
	UpdateCoeffCallback updateCoeffFunc;
	
	#ifdef _customPreNrpnFunction1
	if ( customPreNrpnFunction1( dspId, nrpn, &value, format ) )// Do all your custom pre NRPN code into this function
		return 1;
	#endif
	
	val8bit = value>>8;
	if ( format == FORMAT_14BIT_PRECISION )
		dvalue=(DWORD)value<<(DWORD)13;
	else
		dvalue=value;
	
	i = dichotomicSearch( _cptr32(nrpn1List), 2, 0, NUMBEROFCOMMAND1, nrpn );
	
	if ((i&(1<<15)))
	{
		i &= 0x7FFF;
		if ( i == NUMBEROFCOMMAND1 ) // higher bound test
			i--;
		else
		{
			if ( nrpn < nrpn1List[i][0] )
				i--;		
		}
		if ( !(nrpn1List[i][1] & (1<<14)) )
			i=-1;	
	}
	
	if ( i != -1 )
	{
		index = nrpn - nrpn1List[i][0];
		functionId = nrpn1List[i][1];
		processId = dsp1pcs[processId];
	}
	else
	{
		i = dichotomicSearch( _cptr32(nrpn1BiquadTable), 3, 0, NB_BIQUAD_COMMAND, nrpn );
		if (i!=-1)
		{
			if ((i&(1<<15)))
			{
				i &= 0x7FFF;
				if ( i >= NB_BIQUAD_COMMAND ) // higher bound test
					i--;
				else
				{
					if ( nrpn < nrpn1BiquadTable[i].nrpn )
						i--;				
				}
				if ( !(nrpn1BiquadTable[i].functionId & (1<<14)) )
					i=-1;			
			}
			
			if ( i != -1 )
			{
				index = nrpn - nrpn1BiquadTable[i].nrpn;
				functionId = nrpn1BiquadTable[i].functionId;
				processId = dsp1pcs[processId];
				theBiquad = (BiquadParameters *)nrpn1BiquadTable[i].parametersTable;
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_LiveMic_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_LiveMic_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
			//Gain
			case 0x0030: _LiveMic_Gain_Value( dspId, processId, value ); return 1;
			case 0x0031: _LiveMic_Gain_Phase( dspId, processId, val8bit ); return 1;
			//NoiseGate
			case 0x0038: _LiveMic_NoiseGate_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0039: _LiveMic_NoiseGate_Threshold( dspId, processId, value ); return 1;
			case 0x003A: _LiveMic_NoiseGate_Attack( dspId, processId, value ); return 1;
			case 0x003B: _LiveMic_NoiseGate_Release( dspId, processId, value ); return 1;
			case 0x003C: _LiveMic_NoiseGate_InGainValue( dspId, processId, value ); return 1;
			case 0x003D: _LiveMic_NoiseGate_InGainPhase( dspId, processId, val8bit ); return 1;
			//Biquad
			case 0x0012: _LiveMic_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0013: _LiveMic_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0014: _LiveMic_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4015: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4016: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4017: 
			TRACE("processId %x",processId);
			TRACE("index %x",index);
			TRACE("Freq %x",dvalue);
			SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4018: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			//LevelDetect
			case 0x0034: _LiveMic_LevelDetect_Attack( dspId, processId, value ); return 1;
			case 0x0035: _LiveMic_LevelDetect_Release( dspId, processId, value ); return 1;
			//Compressor
			case 0x001F:  sendSysExMessage( value, _LiveMic_Compressor_GetGainReduction( dspId, processId ) ); return 1;
			case 0x0020: _LiveMic_Compressor_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0021: _LiveMic_Compressor_Threshold( dspId, processId, value ); return 1;
			case 0x0022: _LiveMic_Compressor_Ratio( dspId, processId, value ); return 1;
			case 0x0023: _LiveMic_Compressor_Boost( dspId, processId, value ); return 1;
			case 0x0024: _LiveMic_Compressor_BoostPhase( dspId, processId, val8bit ); return 1;
			//(s)Reverb/Echo
			case 0x0000: _LiveMic_Effect_LoadProgram( dspId, val8bit ); return 1;
			case 0x0001: _LiveMic_Effect_RevInputLevel( dspId, value ); return 1;
			case 0x0002: _LiveMic_Effect_RevLevel( dspId, value ); return 1;
			case 0x0003: _LiveMic_Effect_RevPreHP( dspId, value ); return 1;
			case 0x0004: _LiveMic_Effect_RevHDamp( dspId, value ); return 1;
			case 0x0005: _LiveMic_Effect_RevTime( dspId, value ); return 1;
			case 0x0006: _LiveMic_Effect_RevToneGain( dspId, value ); return 1;
			case 0x0007: _LiveMic_Effect_RevToneFreq( dspId, value ); return 1;
			case 0x0008: _LiveMic_Effect_EchoInputLevel( dspId, value ); return 1;
			case 0x0009: _LiveMic_Effect_EchoTime( dspId, value ); return 1;
			case 0x000A: _LiveMic_Effect_LongEchoMode( dspId, val8bit ); return 1;
			case 0x000B: _LiveMic_Effect_EchoLDamp( dspId, value ); return 1;
			case 0x000C: _LiveMic_Effect_EchoHDamp( dspId, value ); return 1;
			case 0x000D: _LiveMic_Effect_EchoFeedback( dspId, value ); return 1;
			case 0x000E: _LiveMic_Effect_EchoOutputLevel( dspId, 0, value ); return 1;
			case 0x000F: _LiveMic_Effect_EchoOutputPhase( dspId, 0, val8bit ); return 1;
			case 0x0010: _LiveMic_Effect_EchoOutputLevel( dspId, 1, value ); return 1;
			case 0x0011: _LiveMic_Effect_EchoOutputPhase( dspId, 1, val8bit ); return 1;
			//MixN
			case 0x4036: _LiveMic_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x4037: _LiveMic_MixN_GainValue( dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction1
	if ( customPostNrpnFunction1( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
