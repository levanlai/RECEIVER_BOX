#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"

// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 2


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD1BANDCOUNT	6
#define	BIQUAD2BANDCOUNT	6

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad4Parameters1[BIQUAD1BANDCOUNT], biquad4Parameters2[BIQUAD2BANDCOUNT];
WORD biquad4Type1[BIQUAD1BANDCOUNT], biquad4Type2[BIQUAD2BANDCOUNT];
WORD biquad4XoverType1[BIQUAD1BANDCOUNT], biquad4XoverType2[BIQUAD2BANDCOUNT];
DWORD biquad4RawFrequency1[BIQUAD1BANDCOUNT], biquad4RawFrequency2[BIQUAD2BANDCOUNT];

BiquadParameters biquad4ParamAddr1 = { biquad4Parameters1, biquad4Type1, biquad4XoverType1, biquad4RawFrequency1, BIQUAD1BANDCOUNT };
BiquadParameters biquad4ParamAddr2 = { biquad4Parameters2, biquad4Type2, biquad4XoverType2, biquad4RawFrequency2, BIQUAD2BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp4pcs[10];

WORD dsp4InitAndRoute(void)
{
	WORD dspId;

	dspId = _MixPaXT_Init();

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction4
customPreInitFunction4( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #1: Biquad
	dsp4pcs[1] = _MixPaXT_Biquad_Allocate( dspId, BIQUAD1BANDCOUNT );
	_MixPaXT_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp4pcs[1], PCS_DSP_IN | 0 );
	_MixPaXT_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp4pcs[1], PCS_NODE | 2 );

	// Process #2: Biquad
	dsp4pcs[2] = _MixPaXT_Biquad_Allocate( dspId, BIQUAD2BANDCOUNT );
	_MixPaXT_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp4pcs[2], PCS_DSP_IN | 1 );
	_MixPaXT_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp4pcs[2], PCS_NODE | 3 );

	// Process #3: LevelDetect
	dsp4pcs[3] = _MixPaXT_LevelDetect_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp4pcs[3], PCS_NODE | 2 );

	// Process #4: Compressor
	dsp4pcs[4] = _MixPaXT_Compressor_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp4pcs[4], PCS_NODE | 2 );
	_MixPaXT_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp4pcs[4], PCS_NODE | 5 );
	_MixPaXT_Compressor_ConnectLevel( dspId, dsp4pcs[4], dsp4pcs[3] );

	// Process #6: LevelDetect
	dsp4pcs[6] = _MixPaXT_LevelDetect_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp4pcs[6], PCS_NODE | 3 );

	// Process #5: Gain
	dsp4pcs[5] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp4pcs[5], PCS_NODE | 5 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp4pcs[5], PCS_NODE | 0 );

	// Process #7: Compressor
	dsp4pcs[7] = _MixPaXT_Compressor_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp4pcs[7], PCS_NODE | 3 );
	_MixPaXT_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp4pcs[7], PCS_NODE | 7 );
	_MixPaXT_Compressor_ConnectLevel( dspId, dsp4pcs[7], dsp4pcs[6] );

	// Process #8: Gain
	dsp4pcs[8] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp4pcs[8], PCS_NODE | 7 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp4pcs[8], PCS_NODE | 1 );

	// Process #9: MixN
	dsp4pcs[9] = _MixPaXT_MixN_Allocate( dspId, 2 );
	_MixPaXT_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp4pcs[9], PCS_NODE | 0 );
	_MixPaXT_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp4pcs[9], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction4
customPostInitFunction4( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn4List[NUMBEROFCOMMAND4][2]=
{
	{ 0x0100, 0x0000 }, // _MixPaXT_Biquad_OnOff
	{ 0x0101, 0x0001 }, // _MixPaXT_Biquad_InGainPhase
	{ 0x0102, 0x0002 }, // _MixPaXT_Biquad_InGainValue
	{ 0x0200, 0x0000 }, // _MixPaXT_Biquad_OnOff
	{ 0x0201, 0x0001 }, // _MixPaXT_Biquad_InGainPhase
	{ 0x0202, 0x0002 }, // _MixPaXT_Biquad_InGainValue
	{ 0x0300, 0x0033 }, // _MixPaXT_LevelDetect_Attack
	{ 0x0301, 0x0034 }, // _MixPaXT_LevelDetect_Release
	{ 0x0400, 0x0019 }, // _MixPaXT_Compressor_GetGainReduction
	{ 0x0401, 0x001A }, // _MixPaXT_Compressor_OnOff
	{ 0x0402, 0x001B }, // _MixPaXT_Compressor_Threshold
	{ 0x0403, 0x001C }, // _MixPaXT_Compressor_Ratio
	{ 0x0404, 0x001D }, // _MixPaXT_Compressor_Boost
	{ 0x0405, 0x001E }, // _MixPaXT_Compressor_BoostPhase
	{ 0x0500, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0501, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0600, 0x0033 }, // _MixPaXT_LevelDetect_Attack
	{ 0x0601, 0x0034 }, // _MixPaXT_LevelDetect_Release
	{ 0x0700, 0x0019 }, // _MixPaXT_Compressor_GetGainReduction
	{ 0x0701, 0x001A }, // _MixPaXT_Compressor_OnOff
	{ 0x0702, 0x001B }, // _MixPaXT_Compressor_Threshold
	{ 0x0703, 0x001C }, // _MixPaXT_Compressor_Ratio
	{ 0x0704, 0x001D }, // _MixPaXT_Compressor_Boost
	{ 0x0705, 0x001E }, // _MixPaXT_Compressor_BoostPhase
	{ 0x0800, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0801, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0900, 0x403A }, // _MixPaXT_MixN_GainPhase
	{ 0x091F, 0x403B } // _MixPaXT_MixN_GainValue

};

#define NB_BIQUAD_COMMAND 8
const BiquadParamsTable nrpn4BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0103, 0x4003, &biquad4ParamAddr1 },
	{ 0x0122, 0x4004, &biquad4ParamAddr1 },
	{ 0x0141, 0x4005, &biquad4ParamAddr1 },
	{ 0x0160, 0x4006, &biquad4ParamAddr1 },
	{ 0x0203, 0x4003, &biquad4ParamAddr2 },
	{ 0x0222, 0x4004, &biquad4ParamAddr2 },
	{ 0x0241, 0x4005, &biquad4ParamAddr2 },
	{ 0x0260, 0x4006, &biquad4ParamAddr2 }
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
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_MixPaXT_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_MixPaXT_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
			//Biquad
			case 0x0000: _MixPaXT_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0001: _MixPaXT_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0002: _MixPaXT_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4003: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4004: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4005: SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4006: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			//LevelDetect
			case 0x0033: _MixPaXT_LevelDetect_Attack( dspId, processId, value ); return 1;
			case 0x0034: _MixPaXT_LevelDetect_Release( dspId, processId, value ); return 1;
			//Compressor
			case 0x0019:  sendSysExMessage( value, _MixPaXT_Compressor_GetGainReduction( dspId, processId ) ); return 1;
			case 0x001A: _MixPaXT_Compressor_OnOff( dspId, processId, val8bit ); return 1;
			case 0x001B: _MixPaXT_Compressor_Threshold( dspId, processId, value ); return 1;
			case 0x001C: _MixPaXT_Compressor_Ratio( dspId, processId, value ); return 1;
			case 0x001D: _MixPaXT_Compressor_Boost( dspId, processId, value ); return 1;
			case 0x001E: _MixPaXT_Compressor_BoostPhase( dspId, processId, val8bit ); return 1;
			//Gain
			case 0x002F: _MixPaXT_Gain_Value( dspId, processId, value ); return 1;
			case 0x0030: _MixPaXT_Gain_Phase( dspId, processId, val8bit ); return 1;
			//MixN
			case 0x403A: _MixPaXT_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x403B: _MixPaXT_MixN_GainValue( dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction4
	if ( customPostNrpnFunction4( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
