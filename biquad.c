#include <system.h>
#include <libFX5000.h>
#include <trace.h>
#include "biquad.h"
#include "lib.h"
#include "dsp/dspDesigner.h"
#include "config.h"

extern MyData_t  myData;
//============================ Variables ==========================
//_FILTER_PEQ filterParamsMike[PEQ_BANDS_MAX_MIKE];
//============================ Funtions ==========================
WORD get_dsp_from_EQ(WORD peqIdx)
{
	WORD DSPid = 0;
	switch(peqIdx)
	{
		case PEQ_BLOCK_MIKE:
			DSPid = dsp[DSP4_LIVEMIC];
			break;
		default:
			DSPid = 0;
			break;
	}
	return DSPid;
}
WORD DSPexisted(WORD dsp)
{
	return (((dsp) >= 0x30 && (dsp) <= 0x3F) ? 1 : 0);
}

_FILTER_PEQ* filterParamsItem(WORD iBlock)
{
	_FILTER_PEQ* filter;
	switch(iBlock)
	{
		case PEQ_BLOCK_MIKE: filter = myData.filterParamsMike; break;
	}
	return filter;
}

void biquad_init(void)
{
	biquad_init_filter(PEQ_BLOCK_MIKE, PEQ_BANDS_MAX_MIKE);
}

void biquad_parse_info(WORD cmd, WORD value, WORD *band, WORD *iFreq, WORD *iQ, WORD *iGain, WORD *iBiqType, WORD peqIdx)
{
	switch(peqIdx)
	{
		case PEQ_BLOCK_MIKE:
			// if (biquad_band_value(cmd, value, BIQUAD_CMD_MIKE1_F, band, iFreq)) return;
			// else if (biquad_band_value(cmd, value, BIQUAD_CMD_MIKE1_Q, band, iQ)) return;
			// else if (biquad_band_value(cmd, value, BIQUAD_CMD_MIKE1_G, band, iGain)) return;
			// else if (biquad_band_value(cmd, value, BIQUAD_CMD_MIKE1_T, band, iBiqType)) return;

			if (biquad_band_value(cmd, value, CMD_MIKE_F_0, band, iFreq)) return;
			else if (biquad_band_value(cmd, value,CMD_MIKE_Q_0 , band, iQ)) return;
			else if (biquad_band_value(cmd, value, CMD_MIKE_G_0, band, iGain)) return;
			else if (biquad_band_value(cmd, value, CMD_MIKE_T_0, band, iBiqType)) return;
			break;
	}
}

WORD biquad_cmd_EQ(WORD cmd, WORD value)
{
	WORD band = MAX16B, Freq = MAX16B, Q = MAX16B, Gain = MAX16B, BiqType = MAX16B;
	WORD peqIdx, result, DSPid;
	//TRACE("biquad_cmd_EQ cmd=%x",cmd);	
	//TRACE("value=%x",value);		
	for(peqIdx = 0; peqIdx < PEQ_BLOCK_MAX; peqIdx++) {
		biquad_parse_info(cmd, value, &band, &Freq, &Q, &Gain, &BiqType, peqIdx);
		if (Freq != MAX16B) {
			if (Freq < 20) Freq = 20;
			else if (Freq >= 20000) Freq = 19999; 
			break;
		}
		else if (Q != MAX16B) { //1..100 ~ 100..100,00 [0.03 ... 100]
			if (Q < 4) Q = 4;
			else if (Q > 9999) Q = 9999; 
			break;
		}
		else if (Gain != MAX16B) { //0~1000 = 0..500..1000 = 0.0~50.0~100.0 = -50.0dB~0.0dB~50dB
			if (Gain > 1000) Gain = 1000; //1000 = 500 + 500 = +50dB max
			break;
		}
		else if (BiqType != MAX16B) { //0~1000 = 0..500..1000 = 0.0~50.0~100.0 = -50.0dB~0.0dB~50dB
			if (BiqType > BIQ_NOTCH) BiqType = BIQ_PEAK; 
			break;
		}
	}
	if (band == MAX16B && Freq == MAX16B && Q == MAX16B && Gain == MAX16B && BiqType == MAX16B) {
		//TRACE("return here=%x",band);		
		return CMD_NOT_EXISTED;
	}
	TRACE("band=%x",band);	
	//Set Filter
	DSPid = get_dsp_from_EQ(peqIdx);		
	if (DSPexisted(DSPid))
	{
		switch(peqIdx)
		{
			case PEQ_BLOCK_MIKE:
				result = biquad_set_filter(DSPid, dsp4pcs[2], peqIdx, PEQ_BANDS_MAX_MIKE, band, Freq, Q, Gain, BiqType, DSP_LIVEMIC); 
				break;
			default:
				result = CMD_ERROR_CODE;
				break;
		}
	}
	return result;
}

//=============================== lib Biquad ======================================================
#if DBG_FILTER_BIQUAD
void biq_print(_FILTER_PARAM theFBCIIRFilter, _FILTER_PEQ curBandFilter, WORD iBlock, WORD iPos)
{
	DWORD dbgValue;

	dbgValue = ((DWORD)iBlock << 16) | iPos;
	TRACE("iPos Item..._%x", dbgValue);

	dbgValue = _ftol(theFBCIIRFilter.f0);
	TRACE("Biq.Freq_%d", dbgValue);

	//dbgValue = curBandFilter.f0;
	//TRACE("curFreq_%d", dbgValue);
	
	dbgValue = _ftol(_fmul(theFBCIIRFilter.Q, 0x41200000));
	TRACE("Q_%d", dbgValue);

	//dbgValue = curBandFilter.Q;
	//TRACE("curQ_%d", dbgValue);

	dbgValue = _ftol(_fmul(theFBCIIRFilter.dBGain, 0x41200000));
	TRACE("Gain_%d", dbgValue);

	//dbgValue = curBandFilter.dBGain;
	//TRACE("curGain_%d", dbgValue);

	//TRACE("Type_%d", type);
	dbgValue = curBandFilter.biqType;
	TRACE("curType_%d", dbgValue);
}
#endif

_FILTER_PEQ filterGetUpdateItem(WORD iBlock, WORD iPos, WORD freq, WORD q, WORD gain, WORD biq)
{
	_FILTER_PEQ *filter = filterParamsItem(iBlock);
	_FILTER_PEQ band;
	WORD INVALID_WORD_VALUE = 0xFFF0;

	//Get out
	band.f0 = (freq < INVALID_WORD_VALUE) ? freq : filter[iPos].f0;
	band.Q = (q < INVALID_WORD_VALUE) ? q : filter[iPos].Q;
	band.dBGain = (gain < INVALID_WORD_VALUE) ? gain : filter[iPos].dBGain;
	band.biqType = (biq < INVALID_WORD_VALUE) ? biq : filter[iPos].biqType;

	//Update again
	if (filter[iPos].f0 != band.f0) filter[iPos].f0 = band.f0;
	if (filter[iPos].Q != band.Q) filter[iPos].Q = band.Q;
	if (filter[iPos].dBGain != band.dBGain) filter[iPos].dBGain = band.dBGain;
	if (filter[iPos].biqType != band.biqType) filter[iPos].biqType = band.biqType;

	return band;
}

WORD filterConvertItem(_FILTER_PEQ filter, _FILTER_PARAM *iirFilter)
{
	int gainOffset;
	if (filter.f0 == MAX16B || filter.Q == MAX16B || filter.dBGain == MAX16B || filter.biqType == MAX16B) //not enough info
	{
		return 0;
	}
	else
	{
		iirFilter->f0 = _float(filter.f0);
		iirFilter->fs = ((FLOAT)_float(_Sys_GetSamplingRate())); //very important for stable system.
		iirFilter->Q = _fdiv(_float(filter.Q), 0x42C80000); // = 100.000000, 0.03 ... 100,
		gainOffset = filter.dBGain - 500;
		iirFilter->dBGain = _fdiv(_float(gainOffset), 0x41200000);// 10.000000

		//TRACE("gainOffset_%d", gainOffset);
		return 1;
	}
}

void biquad_init_filter(WORD iBlock, WORD maxBands)
{
	WORD i;
	_FILTER_PEQ* filter = filterParamsItem(iBlock);
	//WORD array[7] = {250, 500, 1000, 2000, 4000, 8000, 16000};
	//WORD array[4] = {500, 2000, 5000, 10000};
	WORD array[5] = {500, 1000, 3000, 6000, 12000};
	for(i = 0; i < maxBands; i++)
	{
		filter[i].f0 =array[i];
		filter[i].Q = 70;
		filter[i].dBGain = 500;
		filter[i].biqType = BIQ_PEAK;
		// filter[i].f0 = MAX16B;
		// filter[i].Q = MAX16B;
		// filter[i].dBGain = MAX16B;
		// filter[i].biqType = MAX16B;
#if 0
		TRACE("f0_%x", filter[i].f0);
		TRACE("Q_%x", filter[i].Q);
		TRACE("dbGain_%x", filter[i].dBGain);
		TRACE("biquadType_%x", filter[i].biqType);
#endif
	}
}

WORD biquad_set_filter(WORD DSPid, WORD PROCid, WORD iBlock, WORD bandsMax, WORD band, WORD freq, WORD q, WORD gain, WORD biqType, WORD dspType)
{
	_FILTER_PARAM thePEQIIRFilter;
	_FILTER_PEQ curBandFilter;
	_BIQUAD_COEFF iirCoeff;	// Floating point
	_BIQUAD_FIXEDCOEFF icfixed; // Fixed point
	WORD stable = 0, result = CMD_ERROR_CODE;
	WORD _FRACTBITS_ = _BIQUAD_FRACTBITS_1_2_21;

	//TRACE(">>> _SetFilter band", band);
	if (band >= bandsMax) {
#if DBG_SYSTEM_INFO
		TRACE("SetFilter band overflow%d", band);
#endif
		return CMD_ERROR_CODE;
	}
	if (iBlock >= PEQ_BLOCK_MAX) {
#if DBG_SYSTEM_INFO
		TRACE("SetFilter block overflow%d", iBlock);
#endif
		return CMD_ERROR_CODE;
	}
	if (PROCid == MAX16B) 
	{
		return CMD_ERROR_CODE;
	}

	//Get data
	curBandFilter = filterGetUpdateItem(iBlock, band, freq, q, gain, biqType);
	if (!filterConvertItem(curBandFilter, &thePEQIIRFilter)) 
	{
		//TRACE("SetFilter not enough info", iBlock);
		return CMD_BYPASSED; //exit if not enough info
	}

	//Check and correct if params fail
	_Biquad_CheckParameters(&thePEQIIRFilter);

#if DBG_FILTER_BIQUAD
	biq_print(thePEQIIRFilter, curBandFilter, iBlock, band);
#endif

	//Select Filter to push data
	switch(curBandFilter.biqType)
	{
		case BIQ_LOWSHELF: //Low Shelf Filter => get half RIGHT   ___/###
			_Biquad_FilterLS1(&thePEQIIRFilter, &iirCoeff);// Computes 1st Order High Shelf Filter, not use Q, more safe than 2nd [FINAL CHOSEN]
			break;
		case BIQ_HIGHSHELF: //High Shelf Filter => get half LEFT    ###\___
			_Biquad_FilterHS1(&thePEQIIRFilter, &iirCoeff);// Computes 1st Order High Shelf Filter, not use Q, more safe than 2nd [FINAL CHOSEN]
			break;
		case BIQ_HIGHPASS: //High Pass Filter => get half RIGHT   ___/###
			_Biquad_FilterHP2(&thePEQIIRFilter, &iirCoeff); // Computes 2st Order High Pass Filter, test OK [FINAL CHOSEN]
			break;
		case BIQ_LOWPASS: //Low Pass Filter => get half LEFT    ###\___
			_Biquad_FilterLP2(&thePEQIIRFilter, &iirCoeff); // Computes 2st Order Low Pass Filter, test OK [FINAL CHOSEN]
			break;
		case BIQ_PEAK: //Peak, band
			_Biquad_FilterPEQ(&thePEQIIRFilter, &iirCoeff); // PEQ (Peaking/Notching) Bell Filter, upto -50dB
			break;
		case BIQ_BANDPASS: //BANDPASS
			_Biquad_FilterBP2(&thePEQIIRFilter, &iirCoeff); // 2nd Order Band Pass Filter
			break;				
		case BIQ_NOTCH: //Low Pass Filter => get half LEFT    ###\___
			//_Biquad_FilterPBS(&thePEQIIRFilter, &iirCoeff); // Use for Notch with Mike
			_Biquad_FltFloatCoeff(_BIQUAD_FLTPTR(_Biquad_FilterPBS), &thePEQIIRFilter, &iirCoeff); 
			break;
		default:
			//TRACE("def biqType", curBandFilter.biqType);
			_Biquad_FilterPEQ(&thePEQIIRFilter, &iirCoeff); // PEQ (Peaking/Notching) Bell Filter
			break;
	}
	stable = _Biquad_CheckStability(&iirCoeff);
	if (stable != 0) {
		_Biquad_FloatToFixCoeff(&iirCoeff, &icfixed, _FRACTBITS_); // converts float coefficients into fixed point format (default 1.2.21)
		result = CMD_DONE; //OK
		switch (dspType)
		{
			case DSP_MIXPA: _MixPA_Biquad_UpdateCoeff(DSPid, PROCid, &icfixed, band); break;
			case DSP_LIVEMIC: _LiveMic_Biquad_UpdateCoeff(DSPid, PROCid, &icfixed, band); break;
			case DSP_FBC: _FBCancel_Biquad_UpdateCoeff(DSPid, 0, &icfixed, band); break;
			default: result = CMD_ERROR_CODE; break;
		}
	}
	else {
#if DBG_SYSTEM_INFO
		TRACE("Not stable >>>", stable); 
#endif
		result = CMD_ERROR_CODE;
	}
	return result;
}


WORD biquad_band_value(WORD cmd, WORD value, WORD start, WORD *band, WORD *iValue)
{
	//if (cmd >= start && cmd < (start + 0x40)) {
	if (cmd >= start && cmd < (start + PEQ_BANDS_MAX_MIKE)) {
		*band = cmd - start;
		*iValue = value;
		return 1;
	}
	else return 0;
}



/*
WORD biquad_set_fbc_freq(WORD band, WORD Freq)
{
	WORD result;
	WORD Q, Gain;

	//Freq = _FBCancel_GetFrequency(DSPid, newFreqIdx);
	Q = 70; //0.7 * 100
	Gain = 0; //500 - 50 * 10 => -50dB

	result = biquad_set_filter(fbcDSP, 0, PEQ_BLOCK_FBC_NOTCH, PEQ_BANDS_MAX_FBC_NOTCH, band, Freq, Q, Gain, BIQ_NOTCH, DSP_FBC);
	return result;
}
*/
