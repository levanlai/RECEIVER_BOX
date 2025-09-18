#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"
#include "memorymap.h"

// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 1


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD3BANDCOUNT	3

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad4Parameters3[BIQUAD3BANDCOUNT];
WORD biquad4Type3[BIQUAD3BANDCOUNT];
WORD biquad4XoverType3[BIQUAD3BANDCOUNT];
DWORD biquad4RawFrequency3[BIQUAD3BANDCOUNT];

BiquadParameters biquad4ParamAddr3 = { biquad4Parameters3, biquad4Type3, biquad4XoverType3, biquad4RawFrequency3, BIQUAD3BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp4pcs[8];

WORD dsp4InitAndRoute(void)
{
	WORD dspId;

	dspId = _LiveMic_Init( MEMADDR_DSPINIT04_PCS00PAR00 );

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction4
customPreInitFunction4( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #2: Gain
	dsp4pcs[2] = _LiveMic_Gain_Allocate( dspId );
	_LiveMic_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp4pcs[2], PCS_DSP_IN | 0 );
	_LiveMic_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp4pcs[2], PCS_NODE | 6 );

	// Process #1: (s)Reverb/Echo
	_LiveMic_SetProcIN( dspId, LIVEMIC_REVERB_SAMPLE_IN, PCS_NODE | 6 );
	_LiveMic_SetProcIN( dspId, LIVEMIC_ECHO_SAMPLE_IN, PCS_NODE | 6 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTL, PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_REVERB_SAMPLE_OUTR, PCS_NODE | 3 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTL, PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, LIVEMIC_ECHO_SAMPLE_OUTR, PCS_NODE | 1 );

	// Process #7: MixN
	dsp4pcs[7] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp4pcs[7], PCS_NODE | 0 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp4pcs[7], PCS_NODE | 5 );

	// Process #6: MixN
	dsp4pcs[6] = _LiveMic_MixN_Allocate( dspId, 2 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp4pcs[6], PCS_NODE | 2 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp4pcs[6], PCS_NODE | 4 );

	// Process #5: MixN
	dsp4pcs[5] = _LiveMic_MixN_Allocate( dspId, 3 );
	_LiveMic_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp4pcs[5], PCS_NODE | 4 );
	_LiveMic_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp4pcs[5], PCS_NODE | 7 );

	// Process #3: Biquad
	dsp4pcs[3] = _LiveMic_Biquad_Allocate( dspId, BIQUAD3BANDCOUNT );
	_LiveMic_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp4pcs[3], PCS_NODE | 7 );
	_LiveMic_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp4pcs[3], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction4
customPostInitFunction4( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn4List[NUMBEROFCOMMAND4][2]=
{
	{ 0x0100, 0x0000 }, // _LiveMic_Effect_LoadProgram
	{ 0x0101, 0x0001 }, // _LiveMic_Effect_RevInputLevel
	{ 0x0102, 0x0002 }, // _LiveMic_Effect_RevLevel
	{ 0x0103, 0x0003 }, // _LiveMic_Effect_RevPreHP
	{ 0x0104, 0x0004 }, // _LiveMic_Effect_RevHDamp
	{ 0x0105, 0x0005 }, // _LiveMic_Effect_RevTime
	{ 0x0106, 0x0006 }, // _LiveMic_Effect_RevToneGain
	{ 0x0107, 0x0007 }, // _LiveMic_Effect_RevToneFreq
	{ 0x0108, 0x0008 }, // _LiveMic_Effect_EchoInputLevel
	{ 0x0109, 0x0009 }, // _LiveMic_Effect_EchoTime
	{ 0x010A, 0x000A }, // _LiveMic_Effect_LongEchoMode
	{ 0x010B, 0x000B }, // _LiveMic_Effect_EchoLDamp
	{ 0x010C, 0x000C }, // _LiveMic_Effect_EchoHDamp
	{ 0x010D, 0x000D }, // _LiveMic_Effect_EchoFeedback
	{ 0x010E, 0x000E }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x010F, 0x000F }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0110, 0x0010 }, // _LiveMic_Effect_EchoOutputLevel
	{ 0x0111, 0x0011 }, // _LiveMic_Effect_EchoOutputPhase
	{ 0x0200, 0x0030 }, // _LiveMic_Gain_Value
	{ 0x0201, 0x0031 }, // _LiveMic_Gain_Phase
	{ 0x0300, 0x0012 }, // _LiveMic_Biquad_OnOff
	{ 0x0301, 0x0013 }, // _LiveMic_Biquad_InGainPhase
	{ 0x0302, 0x0014 }, // _LiveMic_Biquad_InGainValue
	{ 0x0500, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x051F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0600, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x061F, 0x4037 }, // _LiveMic_MixN_GainValue
	{ 0x0700, 0x4036 }, // _LiveMic_MixN_GainPhase
	{ 0x071F, 0x4037 } // _LiveMic_MixN_GainValue

};

#define NB_BIQUAD_COMMAND 4
const BiquadParamsTable nrpn4BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0303, 0x4015, &biquad4ParamAddr3 },
	{ 0x0322, 0x4016, &biquad4ParamAddr3 },
	{ 0x0341, 0x4017, &biquad4ParamAddr3 },
	{ 0x0360, 0x4018, &biquad4ParamAddr3 }
};

WORD dsp4NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	BiquadParameters *theBiquad;
	UpdateCoeffCallback updateCoeffFunc;
	
	#ifdef _customPreNrpnFunction4
	if ( customPreNrpnFunction4( dspId, nrpn, &value, format ) )// Do all your custom pre NRPN code into this function
		return 1;
	#endif
	
	val8bit = value>>8;
	if ( format == FORMAT_14BIT_PRECISION )
		dvalue=(DWORD)value<<(DWORD)13;
	else
		dvalue=value;
	
	i = dichotomicSearch( _cptr32(nrpn4List), 2, 0, NUMBEROFCOMMAND4, nrpn );
	
	if ((i&(1<<15)))
	{
		i &= 0x7FFF;
		if ( i == NUMBEROFCOMMAND4 ) // higher bound test
			i--;
		else
		{
			if ( nrpn < nrpn4List[i][0] )
				i--;		
		}
		if ( !(nrpn4List[i][1] & (1<<14)) )
			i=-1;	
	}
	
	if ( i != -1 )
	{
		index = nrpn - nrpn4List[i][0];
		functionId = nrpn4List[i][1];
		processId = dsp4pcs[processId];
	}
	else
	{
		i = dichotomicSearch( _cptr32(nrpn4BiquadTable), 3, 0, NB_BIQUAD_COMMAND, nrpn );
		if (i!=-1)
		{
			if ((i&(1<<15)))
			{
				i &= 0x7FFF;
				if ( i >= NB_BIQUAD_COMMAND ) // higher bound test
					i--;
				else
				{
					if ( nrpn < nrpn4BiquadTable[i].nrpn )
						i--;				
				}
				if ( !(nrpn4BiquadTable[i].functionId & (1<<14)) )
					i=-1;			
			}
			
			if ( i != -1 )
			{
				index = nrpn - nrpn4BiquadTable[i].nrpn;
				functionId = nrpn4BiquadTable[i].functionId;
				processId = dsp4pcs[processId];
				theBiquad = (BiquadParameters *)nrpn4BiquadTable[i].parametersTable;
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_LiveMic_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_LiveMic_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
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
			//Gain
			case 0x0030: _LiveMic_Gain_Value( dspId, processId, value ); return 1;
			case 0x0031: _LiveMic_Gain_Phase( dspId, processId, val8bit ); return 1;
			//MixN
			case 0x4036: _LiveMic_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x4037: _LiveMic_MixN_GainValue( dspId, processId, index, value ); return 1;
			//Biquad
			case 0x0012: _LiveMic_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0013: _LiveMic_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0014: _LiveMic_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4015: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4016: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4017: SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4018: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction4
	if ( customPostNrpnFunction4( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
