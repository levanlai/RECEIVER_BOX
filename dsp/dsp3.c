#include <system.h>
#include <libFX5000.h>
#include "dspDesigner.h"
#include "custom.h"
#ifndef	_SKIP_DDD_NRPN_CTRL
#include "midictrl.h"
#endif	// _SKIP_DDD_NRPN_CTRL
#include "BiquadCtrl.h"

// Biquad(s) - define, variable, ... 
#define BIQUAD_ITEMCOUNT 4


#define NUMBER_OF_BIQUAD_EXTRAFUNCTION 4

#define	BIQUAD3BANDCOUNT	5
#define	BIQUAD15BANDCOUNT	5
#define	BIQUAD16BANDCOUNT	3
#define	BIQUAD2BANDCOUNT	3

#ifndef	_SKIP_DDD_NRPN_CTRL


_FILTER_PARAM biquad3Parameters3[BIQUAD3BANDCOUNT], biquad3Parameters15[BIQUAD15BANDCOUNT], biquad3Parameters16[BIQUAD16BANDCOUNT], biquad3Parameters2[BIQUAD2BANDCOUNT];
WORD biquad3Type3[BIQUAD3BANDCOUNT], biquad3Type15[BIQUAD15BANDCOUNT], biquad3Type16[BIQUAD16BANDCOUNT], biquad3Type2[BIQUAD2BANDCOUNT];
WORD biquad3XoverType3[BIQUAD3BANDCOUNT], biquad3XoverType15[BIQUAD15BANDCOUNT], biquad3XoverType16[BIQUAD16BANDCOUNT], biquad3XoverType2[BIQUAD2BANDCOUNT];
DWORD biquad3RawFrequency3[BIQUAD3BANDCOUNT], biquad3RawFrequency15[BIQUAD15BANDCOUNT], biquad3RawFrequency16[BIQUAD16BANDCOUNT], biquad3RawFrequency2[BIQUAD2BANDCOUNT];

BiquadParameters biquad3ParamAddr3 = { biquad3Parameters3, biquad3Type3, biquad3XoverType3, biquad3RawFrequency3, BIQUAD3BANDCOUNT };
BiquadParameters biquad3ParamAddr15 = { biquad3Parameters15, biquad3Type15, biquad3XoverType15, biquad3RawFrequency15, BIQUAD15BANDCOUNT };
BiquadParameters biquad3ParamAddr16 = { biquad3Parameters16, biquad3Type16, biquad3XoverType16, biquad3RawFrequency16, BIQUAD16BANDCOUNT };
BiquadParameters biquad3ParamAddr2 = { biquad3Parameters2, biquad3Type2, biquad3XoverType2, biquad3RawFrequency2, BIQUAD2BANDCOUNT };
#endif	// _SKIP_DDD_NRPN_CTRL

WORD dsp3pcs[28];

WORD dsp3InitAndRoute(void)
{
	WORD dspId;

	dspId = _MixSPDIFOut_Init( _BOARD_SPDIF_OUTPORT_PIN );

	if ( dspId == -1 ) return 0;

#ifdef _customPreInitFunction3
customPreInitFunction3( dspId );// Do all your custom pre initialization code into this function
#endif

	// Process #6: Gain
	dsp3pcs[6] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[6], PCS_DSP_IN | 0 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[6], PCS_NODE | 10 );

	// Process #7: Gain
	dsp3pcs[7] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[7], PCS_DSP_IN | 1 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[7], PCS_NODE | 11 );

	// Process #8: Gain
	dsp3pcs[8] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[8], PCS_DSP_IN | 2 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[8], PCS_NODE | 12 );

	// Process #14: LevelDetect
	dsp3pcs[14] = _MixSPDIFOut_LevelDetect_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp3pcs[14], PCS_NODE | 10 );

	// Process #9: Compressor
	dsp3pcs[9] = _MixSPDIFOut_Compressor_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp3pcs[9], PCS_NODE | 10 );
	_MixSPDIFOut_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp3pcs[9], PCS_NODE | 1 );
	_MixSPDIFOut_Compressor_ConnectLevel( dspId, dsp3pcs[9], dsp3pcs[14] );

	// Process #13: LevelDetect
	dsp3pcs[13] = _MixSPDIFOut_LevelDetect_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp3pcs[13], PCS_NODE | 11 );

	// Process #10: Compressor
	dsp3pcs[10] = _MixSPDIFOut_Compressor_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp3pcs[10], PCS_NODE | 11 );
	_MixSPDIFOut_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp3pcs[10], PCS_NODE | 0 );
	_MixSPDIFOut_Compressor_ConnectLevel( dspId, dsp3pcs[10], dsp3pcs[13] );

	// Process #12: LevelDetect
	dsp3pcs[12] = _MixSPDIFOut_LevelDetect_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, LEVELDETECT_SAMPLE_IN|dsp3pcs[12], PCS_NODE | 12 );

	// Process #11: Compressor
	dsp3pcs[11] = _MixSPDIFOut_Compressor_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, COMPRESSOR_SAMPLE_IN|dsp3pcs[11], PCS_NODE | 12 );
	_MixSPDIFOut_SetProcOUT( dspId, COMPRESSOR_SAMPLE_OUT|dsp3pcs[11], PCS_NODE | 2 );
	_MixSPDIFOut_Compressor_ConnectLevel( dspId, dsp3pcs[11], dsp3pcs[12] );

	// Process #26: Gain
	dsp3pcs[26] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[26], PCS_NODE | 1 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[26], PCS_NODE | 6 );

	// Process #24: Gain
	dsp3pcs[24] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[24], PCS_NODE | 1 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[24], PCS_NODE | 9 );

	// Process #22: MixN
	dsp3pcs[22] = _MixSPDIFOut_MixN_Allocate( dspId, 3 );
	_MixSPDIFOut_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp3pcs[22], PCS_NODE | 0 );
	_MixSPDIFOut_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp3pcs[22], PCS_NODE | 16 );

	// Process #23: MixN
	dsp3pcs[23] = _MixSPDIFOut_MixN_Allocate( dspId, 3 );
	_MixSPDIFOut_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp3pcs[23], PCS_NODE | 0 );
	_MixSPDIFOut_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp3pcs[23], PCS_NODE | 17 );

	// Process #21: Gain
	dsp3pcs[21] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[21], PCS_NODE | 0 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[21], PCS_NODE | 8 );

	// Process #27: Gain
	dsp3pcs[27] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[27], PCS_NODE | 2 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[27], PCS_NODE | 7 );

	// Process #5: MixN
	dsp3pcs[5] = _MixSPDIFOut_MixN_Allocate( dspId, 2 );
	_MixSPDIFOut_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp3pcs[5], PCS_NODE | 6 );
	_MixSPDIFOut_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp3pcs[5], PCS_NODE | 18 );

	// Process #1: MixN
	dsp3pcs[1] = _MixSPDIFOut_MixN_Allocate( dspId, 2 );
	_MixSPDIFOut_SetProcIN( dspId, MIXN_SAMPLE_IN|dsp3pcs[1], PCS_NODE | 8 );
	_MixSPDIFOut_SetProcOUT( dspId, MIXN_SAMPLE_OUT|dsp3pcs[1], PCS_NODE | 19 );

	// Process #19: (s)SPDIF-Out
	_MixSPDIFOut_SetProcIN( dspId, SPDIFOUT_SAMPLE_INL, PCS_NODE | 16 );
	_MixSPDIFOut_SetProcIN( dspId, SPDIFOUT_SAMPLE_INR, PCS_NODE | 17 );

	// Process #3: Biquad
	dsp3pcs[3] = _MixSPDIFOut_Biquad_Allocate( dspId, BIQUAD3BANDCOUNT );
	_MixSPDIFOut_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp3pcs[3], PCS_NODE | 16 );
	_MixSPDIFOut_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp3pcs[3], PCS_NODE | 20 );

	// Process #15: Biquad
	dsp3pcs[15] = _MixSPDIFOut_Biquad_Allocate( dspId, BIQUAD15BANDCOUNT );
	_MixSPDIFOut_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp3pcs[15], PCS_NODE | 17 );
	_MixSPDIFOut_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp3pcs[15], PCS_NODE | 21 );

	// Process #16: Biquad
	dsp3pcs[16] = _MixSPDIFOut_Biquad_Allocate( dspId, BIQUAD16BANDCOUNT );
	_MixSPDIFOut_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp3pcs[16], PCS_NODE | 18 );
	_MixSPDIFOut_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp3pcs[16], PCS_NODE | 22 );

	// Process #2: Biquad
	dsp3pcs[2] = _MixSPDIFOut_Biquad_Allocate( dspId, BIQUAD2BANDCOUNT );
	_MixSPDIFOut_SetProcIN( dspId, BIQUAD_SAMPLE_IN|dsp3pcs[2], PCS_NODE | 19 );
	_MixSPDIFOut_SetProcOUT( dspId, BIQUAD_SAMPLE_OUT|dsp3pcs[2], PCS_NODE | 23 );

	// Process #17: Gain
	dsp3pcs[17] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[17], PCS_NODE | 20 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[17], PCS_DSP_OUT | 0 );

	// Process #25: Gain
	dsp3pcs[25] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[25], PCS_NODE | 21 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[25], PCS_DSP_OUT | 1 );

	// Process #18: Gain
	dsp3pcs[18] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[18], PCS_NODE | 22 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[18], PCS_DSP_OUT | 3 );

	// Process #4: Gain
	dsp3pcs[4] = _MixSPDIFOut_Gain_Allocate( dspId );
	_MixSPDIFOut_SetProcIN( dspId, GAIN_SAMPLE_IN|dsp3pcs[4], PCS_NODE | 23 );
	_MixSPDIFOut_SetProcOUT( dspId, GAIN_SAMPLE_OUT|dsp3pcs[4], PCS_DSP_OUT | 2 );

#ifdef _customPostInitFunction3
customPostInitFunction3( dspId );// Do all your custom post initialization code into this function
#endif

	return( dspId );
}


#ifndef	_SKIP_DDD_NRPN_CTRL

const WORD nrpn3List[NUMBEROFCOMMAND3][2]=
{
	{ 0x0100, 0x401F }, // _MixSPDIFOut_MixN_GainPhase
	{ 0x011F, 0x4020 }, // _MixSPDIFOut_MixN_GainValue
	{ 0x0200, 0x0000 }, // _MixSPDIFOut_Biquad_OnOff
	{ 0x0201, 0x0001 }, // _MixSPDIFOut_Biquad_InGainPhase
	{ 0x0202, 0x0002 }, // _MixSPDIFOut_Biquad_InGainValue
	{ 0x0300, 0x0000 }, // _MixSPDIFOut_Biquad_OnOff
	{ 0x0301, 0x0001 }, // _MixSPDIFOut_Biquad_InGainPhase
	{ 0x0302, 0x0002 }, // _MixSPDIFOut_Biquad_InGainValue
	{ 0x0400, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x0401, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x0500, 0x401F }, // _MixSPDIFOut_MixN_GainPhase
	{ 0x051F, 0x4020 }, // _MixSPDIFOut_MixN_GainValue
	{ 0x0600, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x0601, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x0700, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x0701, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x0800, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x0801, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x0900, 0x0009 }, // _MixSPDIFOut_Compressor_GetGainReduction
	{ 0x0901, 0x000A }, // _MixSPDIFOut_Compressor_OnOff
	{ 0x0902, 0x000B }, // _MixSPDIFOut_Compressor_Threshold
	{ 0x0903, 0x000C }, // _MixSPDIFOut_Compressor_Ratio
	{ 0x0904, 0x000D }, // _MixSPDIFOut_Compressor_Boost
	{ 0x0905, 0x000E }, // _MixSPDIFOut_Compressor_BoostPhase
	{ 0x0A00, 0x0009 }, // _MixSPDIFOut_Compressor_GetGainReduction
	{ 0x0A01, 0x000A }, // _MixSPDIFOut_Compressor_OnOff
	{ 0x0A02, 0x000B }, // _MixSPDIFOut_Compressor_Threshold
	{ 0x0A03, 0x000C }, // _MixSPDIFOut_Compressor_Ratio
	{ 0x0A04, 0x000D }, // _MixSPDIFOut_Compressor_Boost
	{ 0x0A05, 0x000E }, // _MixSPDIFOut_Compressor_BoostPhase
	{ 0x0B00, 0x0009 }, // _MixSPDIFOut_Compressor_GetGainReduction
	{ 0x0B01, 0x000A }, // _MixSPDIFOut_Compressor_OnOff
	{ 0x0B02, 0x000B }, // _MixSPDIFOut_Compressor_Threshold
	{ 0x0B03, 0x000C }, // _MixSPDIFOut_Compressor_Ratio
	{ 0x0B04, 0x000D }, // _MixSPDIFOut_Compressor_Boost
	{ 0x0B05, 0x000E }, // _MixSPDIFOut_Compressor_BoostPhase
	{ 0x0C00, 0x001D }, // _MixSPDIFOut_LevelDetect_Release
	{ 0x0C01, 0x001E }, // _MixSPDIFOut_LevelDetect_Attack
	{ 0x0D00, 0x001D }, // _MixSPDIFOut_LevelDetect_Release
	{ 0x0D01, 0x001E }, // _MixSPDIFOut_LevelDetect_Attack
	{ 0x0E00, 0x001D }, // _MixSPDIFOut_LevelDetect_Release
	{ 0x0E01, 0x001E }, // _MixSPDIFOut_LevelDetect_Attack
	{ 0x0F00, 0x0000 }, // _MixSPDIFOut_Biquad_OnOff
	{ 0x0F01, 0x0001 }, // _MixSPDIFOut_Biquad_InGainPhase
	{ 0x0F02, 0x0002 }, // _MixSPDIFOut_Biquad_InGainValue
	{ 0x1000, 0x0000 }, // _MixSPDIFOut_Biquad_OnOff
	{ 0x1001, 0x0001 }, // _MixSPDIFOut_Biquad_InGainPhase
	{ 0x1002, 0x0002 }, // _MixSPDIFOut_Biquad_InGainValue
	{ 0x1100, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1101, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1200, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1201, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1500, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1501, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1600, 0x401F }, // _MixSPDIFOut_MixN_GainPhase
	{ 0x161F, 0x4020 }, // _MixSPDIFOut_MixN_GainValue
	{ 0x1700, 0x401F }, // _MixSPDIFOut_MixN_GainPhase
	{ 0x171F, 0x4020 }, // _MixSPDIFOut_MixN_GainValue
	{ 0x1800, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1801, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1900, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1901, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1A00, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1A01, 0x001A }, // _MixSPDIFOut_Gain_Phase
	{ 0x1B00, 0x0019 }, // _MixSPDIFOut_Gain_Value
	{ 0x1B01, 0x001A } // _MixSPDIFOut_Gain_Phase

};

#define NB_BIQUAD_COMMAND 16
const BiquadParamsTable nrpn3BiquadTable[NB_BIQUAD_COMMAND] = 
{
	{ 0x0203, 0x4003, &biquad3ParamAddr2 },
	{ 0x0222, 0x4004, &biquad3ParamAddr2 },
	{ 0x0241, 0x4005, &biquad3ParamAddr2 },
	{ 0x0260, 0x4006, &biquad3ParamAddr2 },
	{ 0x0303, 0x4003, &biquad3ParamAddr3 },
	{ 0x0322, 0x4004, &biquad3ParamAddr3 },
	{ 0x0341, 0x4005, &biquad3ParamAddr3 },
	{ 0x0360, 0x4006, &biquad3ParamAddr3 },
	{ 0x0F03, 0x4003, &biquad3ParamAddr15 },
	{ 0x0F22, 0x4004, &biquad3ParamAddr15 },
	{ 0x0F41, 0x4005, &biquad3ParamAddr15 },
	{ 0x0F60, 0x4006, &biquad3ParamAddr15 },
	{ 0x1003, 0x4003, &biquad3ParamAddr16 },
	{ 0x1022, 0x4004, &biquad3ParamAddr16 },
	{ 0x1041, 0x4005, &biquad3ParamAddr16 },
	{ 0x1060, 0x4006, &biquad3ParamAddr16 }
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
				updateCoeffFunc.BIQUAD_UpdateCoeffFuncPtr = _cptr32( &_MixSPDIFOut_Biquad_UpdateCoeff );
				updateCoeffFunc.BIQUAD_FlatFuncPtr = _cptr32( &_MixSPDIFOut_Biquad_Flat );
			}
		
		}
	
	}
	
	if (i != -1)
	{
		switch (functionId)
		{
			//Gain
			case 0x0019: _MixSPDIFOut_Gain_Value( dspId, processId, value ); return 1;
			case 0x001A: _MixSPDIFOut_Gain_Phase( dspId, processId, val8bit ); return 1;
			//Compressor
			case 0x0009:  sendSysExMessage( value, _MixSPDIFOut_Compressor_GetGainReduction( dspId, processId ) ); return 1;
			case 0x000A: _MixSPDIFOut_Compressor_OnOff( dspId, processId, val8bit ); return 1;
			case 0x000B: _MixSPDIFOut_Compressor_Threshold( dspId, processId, value ); return 1;
			case 0x000C: _MixSPDIFOut_Compressor_Ratio( dspId, processId, value ); return 1;
			case 0x000D: _MixSPDIFOut_Compressor_Boost( dspId, processId, value ); return 1;
			case 0x000E: _MixSPDIFOut_Compressor_BoostPhase( dspId, processId, val8bit ); return 1;
			//LevelDetect
			case 0x001D: _MixSPDIFOut_LevelDetect_Release( dspId, processId, value ); return 1;
			case 0x001E: _MixSPDIFOut_LevelDetect_Attack( dspId, processId, value ); return 1;
			//MixN
			case 0x401F: _MixSPDIFOut_MixN_GainPhase( dspId, processId, index, val8bit ); return 1;
			case 0x4020: _MixSPDIFOut_MixN_GainValue( dspId, processId, index, value ); return 1;
			//Biquad
			case 0x0000: _MixSPDIFOut_Biquad_OnOff( dspId, processId, val8bit ); return 1;
			case 0x0001: _MixSPDIFOut_Biquad_InGainPhase( dspId, processId, val8bit ); return 1;
			case 0x0002: _MixSPDIFOut_Biquad_InGainValue( dspId, processId, value ); return 1;
			case 0x4003: SetFilterType( &updateCoeffFunc, theBiquad, dspId, processId, index, val8bit ); return 1;
			case 0x4004: SetFilterQ( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
			case 0x4005: SetFilterFreq( &updateCoeffFunc, theBiquad, dspId, processId, index, dvalue ); return 1;
			case 0x4006: SetFilterGain( &updateCoeffFunc, theBiquad, dspId, processId, index, value ); return 1;
		
		}
	
	}
	
	#ifdef _customPostNrpnFunction3
	if ( customPostNrpnFunction3( dspId, nrpn, val8bit, value, dvalue ) )// Do all your custom post NRPN code into this function
		return 1;
	#endif
	return 0;

}
#endif	// _SKIP_DDD_NRPN_CTRL
