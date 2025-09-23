#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"

// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 1


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD4BANDCOUNT	6

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad3Parameters4[BIQUAD4BANDCOUNT];
WORD biquad3Type4[BIQUAD4BANDCOUNT];
WORD biquad3XoverType4[BIQUAD4BANDCOUNT];
DWORD biquad3RawFrequency4[BIQUAD4BANDCOUNT];

BiquadParameters biquad3ParamAddr4 = { biquad3Parameters4, biquad3Type4, biquad3XoverType4, biquad3RawFrequency4, BIQUAD4BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp3pcs[7];

WORD dsp3InitAndRoute(void)
{
	WORD dspId;

	dspId = _MixPaXT_Init();

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction3
customPreInitFunction3( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #4: Biquad
	dsp3pcs[4] = _MixPaXT_Biquad_Allocate( dspId, BIQUAD4BANDCOUNT );
	_MixPaXT_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp3pcs[4], PCS_DSP_IN | 0 );
	_MixPaXT_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp3pcs[4], PCS_NODE | 0 );

	// Process #3: LevelDetect
	dsp3pcs[3] = _MixPaXT_LevelDetect_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp3pcs[3], PCS_NODE | 0 );

	// Process #2: Compressor
	dsp3pcs[2] = _MixPaXT_Compressor_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp3pcs[2], PCS_NODE | 0 );
	_MixPaXT_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp3pcs[2], PCS_NODE | 2 );
	_MixPaXT_Compressor_ConnectLevel( dspId, dsp3pcs[2], dsp3pcs[3] );

	// Process #5: Gain
	dsp3pcs[5] = _MixPaXT_Gain_Allocate( dspId );
	_MixPaXT_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[5], PCS_NODE | 2 );
	_MixPaXT_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[5], PCS_NODE | 3 );

	// Process #6: BusSendN
	dsp3pcs[6] = _MixPaXT_BusSendN_Allocate( dspId, 2 );
	_MixPaXT_SetProcIN( dspId, BUSSENDN_SAMPLE_IN|dsp3pcs[6], PCS_NODE | 3 );
	_MixPaXT_SetProcOUT( dspId, BUSSENDN_SAMPLE_OUT|dsp3pcs[6], PCS_DSP_OUT | 0 );

#ifdef _customPostInitFunction3
customPostInitFunction3( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn3List[NUMBEROFCOMMAND3][2]=
{
	{ 0x0200, 0x0019 }, // _MixPaXT_Compressor_GetGainReduction
	{ 0x0201, 0x001A }, // _MixPaXT_Compressor_OnOff
	{ 0x0202, 0x001B }, // _MixPaXT_Compressor_Threshold
	{ 0x0203, 0x001C }, // _MixPaXT_Compressor_Ratio
	{ 0x0204, 0x001D }, // _MixPaXT_Compressor_Boost
	{ 0x0205, 0x001E }, // _MixPaXT_Compressor_BoostPhase
	{ 0x0300, 0x0033 }, // _MixPaXT_LevelDetect_Attack
	{ 0x0301, 0x0034 }, // _MixPaXT_LevelDetect_Release
	{ 0x0400, 0x0000 }, // _MixPaXT_Biquad_OnOff
	{ 0x0401, 0x0001 }, // _MixPaXT_Biquad_InGainPhase
	{ 0x0402, 0x0002 }, // _MixPaXT_Biquad_InGainValue
	{ 0x0500, 0x002F }, // _MixPaXT_Gain_Value
	{ 0x0501, 0x0030 }, // _MixPaXT_Gain_Phase
	{ 0x0600, 0x400E }, // _MixPaXT_BusSendN_GainPhase
	{ 0x061F, 0x400F } // _MixPaXT_BusSendN_GainValue

};

#define NB_BIQUAD_COMMAND 4
const BiquadParamsTable nrpn3BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0403, 0x4003, &biquad3ParamAddr4 },
	{ 0x0422, 0x4004, &biquad3ParamAddr4 },
	{ 0x0441, 0x4005, &biquad3ParamAddr4 },
	{ 0x0460, 0x4006, &biquad3ParamAddr4 }
};

WORD dsp3NrpnHandler( WORD nrpn, WORD dspId, WORD processId, DWORD value, WORD format )
{
	
	WORD i, functionId, index = 0, val8bit;
	DWORD dvalue;
	BiquadParameters *theBiquad;
	UpdateCoeffCallback updateCoeffFunc;
	
	#ifdef _customPreNrpnFunction3
	if ( customPreNrpnFunction3( dspId, nrpn, &value, format ) )// Do all your custom pre NRPN code into this function
		return 1;
	#endif
	
	val8bit = value>>8;
	if ( format == FORMAT_14BIT_PRECISION )
		dvalue=(DWORD)value<<(DWORD)13;
	else
		dvalue=value;
	
	i = dichotomicSearch( _cptr32(nrpn3List), 2, 0, NUMBEROFCOMMAND3, nrpn );
	
	if ((i&(1<<15)))
	{
		i &= 0x7FFF;
		if ( i == NUMBEROFCOMMAND3 ) // higher bound test
			i--;
		else
		{
			if ( nrpn < nrpn3List[i][0] )
				i--;		
		}
		if ( !(nrpn3List[i][1] & (1<<14)) )
			i=-1;	
	}
	
	if ( i != -1 )
	{
		index = nrpn - nrpn3List[i][0];
		functionId = nrpn3List[i][1];
		processId = dsp3pcs[processId];
	}
	else
	{
		i = dichotomicSearch( _cptr32(nrpn3BiquadTable), 3, 0, NB_BIQUAD_COMMAND, nrpn );
		if (i!=-1)
		{
			if ((i&(1<<15)))
			{
				i &= 0x7FFF;
				if ( i >= NB_BIQUAD_COMMAND ) // higher bound test
					i--;
				else
				{
					if ( nrpn < nrpn3BiquadTable[i].nrpn )
						i--;				
				}
				if ( !(nrpn3BiquadTable[i].functionId & (1<<14)) )
					i=-1;			
			}
			
			if ( i != -1 )
			{
				index = nrpn - nrpn3BiquadTable[i].nrpn;
				functionId = nrpn3BiquadTable[i].functionId;
				processId = dsp3pcs[processId];
				theBiquad = (BiquadParameters *)nrpn3BiquadTable[i].parametersTable;
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_MixPaXT_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_MixPaXT_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
			//Compressor
			case 0x0019:  sendSysExMessage( value, _MixPaXT_Compressor_GetGainReduction( dspId, processId ) ); return 1;
			case 0x001A: _MixPaXT_Compressor_OnOff( dspId, processId, val8bit ); return 1;
			case 0x001B: _MixPaXT_Compressor_Threshold( dspId, processId, value ); return 1;
			case 0x001C: _MixPaXT_Compressor_Ratio( dspId, processId, value ); return 1;
			case 0x001D: _MixPaXT_Compressor_Boost( dspId, processId, value ); return 1;
			case 0x001E: _MixPaXT_Compressor_BoostPhase( dspId, processId, val8bit ); return 1;
			//LevelDetect
			case 0x0033: _MixPaXT_LevelDetect_Attack( dspId, processId, value ); return 1;
			case 0x0034: _MixPaXT_LevelDetect_Release( dspId, processId, value ); return 1;
			//Biquad
			case 0x0000: _MixPaXT_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0001: _MixPaXT_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0002: _MixPaXT_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4003: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4004: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4005: SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4006: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			//Gain
			case 0x002F: _MixPaXT_Gain_Value( dspId, processId, value ); return 1;
			case 0x0030: _MixPaXT_Gain_Phase( dspId, processId, val8bit ); return 1;
			//BusSendN
			case 0x400E: _MixPaXT_BusSendN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x400F: _MixPaXT_BusSendN_GainValue( dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction3
	if ( customPostNrpnFunction3( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
