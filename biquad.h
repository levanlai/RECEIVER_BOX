#ifndef _BIQUAD_H  
#define _BIQUAD_H


WORD biquad_band_value(WORD cmd, WORD value, WORD start, WORD *band, WORD *iValue);
WORD biquad_set_filter(WORD DSPid, WORD PROCid, WORD iBlock, WORD bandsMax, WORD band, WORD freq, WORD q, WORD gain, WORD biqType, WORD dspType);
void biquad_init_filter(WORD iBlock, WORD maxBands);
WORD biquad_cmd_EQ(WORD cmd, WORD value);
void biquad_init(void);


#endif
