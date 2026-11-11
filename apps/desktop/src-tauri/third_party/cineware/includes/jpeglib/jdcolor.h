#ifndef JDCOLOR_H__
#define JDCOLOR_H__
#include "jmorecfg.h"


#ifdef __cplusplus
extern "C" 
#endif
void ycc_rgb_convert_SSE216w(const Fw8u* inptr0, const Fw8u* inptr1, const Fw8u* inptr2, Fw8u* outrow);

#endif	// JDCOLOR_H__
