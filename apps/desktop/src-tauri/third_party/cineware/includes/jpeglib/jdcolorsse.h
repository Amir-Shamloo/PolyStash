#ifndef JDCOLORSSE_H__
#define JDCOLORSSE_H__

#if defined(__i386__) || defined(__x86_64__) || defined(MAXON_TARGET_WINDOWS)

#include "jmorecfg.h"

#ifdef __cplusplus
extern "C"
#endif
void ycc_rgb_convert_SSE216w(const Fw8u* inptr0, const Fw8u* inptr1, const Fw8u* inptr2, Fw8u* outrow);


#endif

#endif // JDCOLORSSE_H__
