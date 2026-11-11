/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DRAWPORT_H__
#define C4D_DRAWPORT_H__

#include "private_symbols.h"
#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

class ColorProfileConvert;
class BaseBitmap;

/// Returns the integer representation of a color.
#define Rgb32(r, g, b) UInt32((UInt32(r) << 16) | (UInt32(g) << 8) | UInt32(b))

/// Maximum number of color modes.
#define COLORMODE_MAXCOLOR ((1 << 6) - 1)

#define B3D_BITDEPTH(mode) ((mode >> BITDEPTH_SHIFT) & (BITDEPTH_UCHAR | BITDEPTH_UWORD | BITDEPTH_FLOAT))

/// @addtogroup IMAGERESULT
/// @ingroup group_enumeration
/// @{
/// Results for image initialization, open, save etc.
enum IMAGERESULT
{
	IMAGERESULT_OK						= 1,	///< Image loaded/created.
	IMAGERESULT_NOTEXISTING		= -1,	///< Image does not exist.
	IMAGERESULT_WRONGTYPE			= -2,	///< Image has the wrong type.
	IMAGERESULT_OUTOFMEMORY		= -3,	///< Not enough memory.
	IMAGERESULT_FILEERROR			= -4,	///< File error.
	IMAGERESULT_FILESTRUCTURE = -5,	///< Invalid file structure.
	IMAGERESULT_MISC_ERROR		= -6,	///< Unknown error.
	IMAGERESULT_PARAM_ERROR		= -7	///< Parameter error.
} ENUM_END_LIST(IMAGERESULT);
/// @}

/// @addtogroup COLORBYTES
/// @ingroup group_enumeration
/// @{
#define COLORBYTES_GRAY	 1																		///< 8-bit grayscale.
#define COLORBYTES_AGRAY 2																		///< 8-bit grayscale with 8-bit alpha.
#define COLORBYTES_RGB	 3																		///< 8-bit RGB.
#define COLORBYTES_ARGB	 4																		///< 8-bit RGB with 8-bit alpha.
#define COLORBYTES_CMYK	 4																		///< 8-bit CMYK.
#define COLORBYTES_ACMYK 5																		///< 8-bit CMYK with 8-bit alpha.

#define COLORBYTES_GRAYw	(COLORBYTES_GRAY * sizeof(PIX_W))		///< 16-bit grayscale.
#define COLORBYTES_AGRAYw (COLORBYTES_AGRAY * sizeof(PIX_W))	///< 16-bit grayscale with 16-bit alpha.
#define COLORBYTES_RGBw		(COLORBYTES_RGB * sizeof(PIX_W))		///< 16-bit RGB.
#define COLORBYTES_ARGBw	(COLORBYTES_ARGB * sizeof(PIX_W))		///< 16-bit RGB with 16-bit alpha.
#define COLORBYTES_GRAYf	(COLORBYTES_GRAY * sizeof(PIX_F))		///< Floating point grayscale.
#define COLORBYTES_AGRAYf (COLORBYTES_AGRAY * sizeof(PIX_F))	///< Floating point grayscale with floating point alpha.
#define COLORBYTES_RGBf		(COLORBYTES_RGB * sizeof(PIX_F))		///< Floating point RGB.
#define COLORBYTES_ARGBf	(COLORBYTES_ARGB * sizeof(PIX_F))		///< Floating point RGB with floating point alpha.

#define COLORBYTES_MAX (COLORBYTES_ARGBf)											///< Maximum number of color bytes.
/// @}


/// @addtogroup COLORMODE
/// @ingroup group_enumeration
/// @{
/// The color mode for @link BaseBitmap bitmaps@endlink.
/// The most common values are @ref COLORMODE_RGB for <i>24</i>-bit RGB values and @ref COLORMODE_GRAY for <i>8</i>-bit grayscale values.
enum COLORMODE
{
	// 8-bit modes.

	COLORMODE_ILLEGAL = 0,	///< Illegal 8-bit mode.

	COLORMODE_ALPHA		= 1,	///< Only 8-bit alpha channel.
	COLORMODE_GRAY		= 2,	///< 8-bit grayscale channel.
	COLORMODE_AGRAY		= 3,	///< 8-bit grayscale channel with 8-bit alpha.
	COLORMODE_RGB			= 4,	///< 8-bit RGB channels.
	COLORMODE_ARGB		= 5,	///< 8-bit RGB channels with 8-bit alpha.
	COLORMODE_CMYK		= 6,	///< 8-bit CMYK channel.
	COLORMODE_ACMYK		= 7,	///< 8-bit CMYK channel with 8-bit alpha.
	COLORMODE_MASK		= 8,	///< 8-bit grayscale map as mask.
	COLORMODE_AMASK		= 9,	///< 8-bit grayscale map as mask with 8-bit alpha.

	// 16-bit modes.

	COLORMODE_ILLEGALw = (BITDEPTH_UWORD << BITDEPTH_SHIFT),											///< Illegal 16-bit mode.

	COLORMODE_GRAYw		 = (COLORMODE_GRAY | (BITDEPTH_UWORD << BITDEPTH_SHIFT)),		///< 16-bit grayscale channel.
	COLORMODE_AGRAYw	 = (COLORMODE_AGRAY | (BITDEPTH_UWORD << BITDEPTH_SHIFT)),	///< 16-bit grayscale channel with 16-bit alpha.
	COLORMODE_RGBw		 = (COLORMODE_RGB | (BITDEPTH_UWORD << BITDEPTH_SHIFT)),		///< 16-bit RGB channels.
	COLORMODE_ARGBw		 = (COLORMODE_ARGB | (BITDEPTH_UWORD << BITDEPTH_SHIFT)),		///< 16-bit RGB channels with 16-bit alpha.
	COLORMODE_MASKw		 = (COLORMODE_MASK | (BITDEPTH_UWORD << BITDEPTH_SHIFT)),		///< 16-bit grayscale map as mask.

	// 32-bit modes.

	COLORMODE_ILLEGALf = (BITDEPTH_FLOAT << BITDEPTH_SHIFT),											///< Illegal 32-bit mode.

	COLORMODE_GRAYf		 = (COLORMODE_GRAY | (BITDEPTH_FLOAT << BITDEPTH_SHIFT)),		///< Floating point grayscale channel.
	COLORMODE_AGRAYf	 = (COLORMODE_AGRAY | (BITDEPTH_FLOAT << BITDEPTH_SHIFT)),	///< Floating point grayscale channel with floating point alpha.
	COLORMODE_RGBf		 = (COLORMODE_RGB | (BITDEPTH_FLOAT << BITDEPTH_SHIFT)),		///< Floating point RGB channels.
	COLORMODE_ARGBf		 = (COLORMODE_ARGB | (BITDEPTH_FLOAT << BITDEPTH_SHIFT)),		///< Floating point RGB channels with floating point alpha.
	COLORMODE_MASKf		 = (COLORMODE_MASK | (BITDEPTH_FLOAT << BITDEPTH_SHIFT))		///< Floating point grayscale map as mask.
} ENUM_END_FLAGS(COLORMODE);
/// @}

/// @addtogroup COLORSPACETRANSFORMATION
/// @ingroup group_enumeration
/// @{
/// Color space transformations.
enum COLORSPACETRANSFORMATION
{
	COLORSPACETRANSFORMATION_NONE						= 0,	///< None.
	COLORSPACETRANSFORMATION_LINEAR_TO_SRGB = 1,	///< Linear to sRGB color space transformation.
	COLORSPACETRANSFORMATION_SRGB_TO_LINEAR = 2,	///< sRGB to linear color space transformation.

	COLORSPACETRANSFORMATION_LINEAR_TO_VIEW = 10,	///< Linear to display color space transformation.
	COLORSPACETRANSFORMATION_SRGB_TO_VIEW		= 11	///< sRGB to display color space transformation.
} ENUM_END_LIST(COLORSPACETRANSFORMATION);
/// @}

/// @addtogroup PIXELCNT
/// @ingroup group_enumeration
/// @{
/// Flags for BaseBitmap::GetPixelCnt()/@ref BaseBitmap::SetPixelCnt()
enum PIXELCNT
{
	PIXELCNT_0									 = 0,					///< None.
	PIXELCNT_DITHERING					 = (1 << 0),	///< Allow dithering.
	PIXELCNT_B3DLAYERS					 = (1 << 1),	///< Merge BodyPaint 3D layers (MultipassBitmap).
	PIXELCNT_APPLYALPHA					 = (1 << 2),	///< Apply alpha layers to the result (PaintLayer).
	PIXELCNT_INTERNAL_SETLINE		 = (1 << 29),	///< Internal SetLine indicator. @markPrivate
	PIXELCNT_INTERNAL_ALPHAVALUE = (1 << 30)	///< Get also the alpha value (rgba <i>32</i>-bit). @markPrivate
} ENUM_END_FLAGS(PIXELCNT);
/// @}


/// @addtogroup DrawPortTypes
/// @ingroup group_enumeration
/// @{
enum DrawPortTypes
{
	DpNone,					///< None.
	DpGray,					///< Gray.
	DpRegular,			///< Regular.
	DpSystem,				///< System.
	DpNoBlit,				///< No blit.
	DpB3d,					///< BodyPaint 3D.
	DpSystem3DCgGl	///< OpenGL Cg.
};
/// @}

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort
{
public:
	static DrawPort* Alloc(DrawPortTypes type, Int32 BitsPerPixel, void* data);

protected:
	Int32	 bw, bh, bt;
	Int		 Bpz;	// Must be Int so that all the multiplications are correct!
	UChar* Bits;

protected:
	DrawPortTypes	type;
	BaseBitmap*		parent;

public:
	DrawPort();
	virtual ~DrawPort();

	void SetBaseBitmap(BaseBitmap* t_parent)
	{
		parent = t_parent;
	}

	BaseBitmap* GetBaseBitmap()
	{
		return parent;
	}
	const BaseBitmap* GetBaseBitmap() const
	{
		return parent;
	}

	DrawPortTypes GetType() const
	{
		return type;
	}
	Int32 GetBw()
	{
		return bw;
	}
	Int32 GetBh()
	{
		return bh;
	}
	Int32 GetBt()
	{
		return bt;
	}
	Int32 GetBpz()
	{
		return Int32(Bpz);
	}

	virtual Bool CloneIt(DrawPort* dest);
	virtual void ScaleIt(DrawPort* dest, Int32 hell, Bool sample, Bool highq, Bool colorcorrection);

	virtual void Free();
	virtual IMAGERESULT Init(Int32 x, Int32 y) = 0;

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha) = 0;
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue) = 0;
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetLineDirect(Int32 y, void* data, Int32 depth);

	virtual Bool SetPixelCntX(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, COLORMODE srcmode, PIXELCNT flags);
	virtual void GetPixelCntX(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, COLORMODE dstmode, PIXELCNT flags, ColorProfileConvert* conversion);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags) = 0;
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags) = 0;
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class IndependentDrawPort : public DrawPort
{
public:
	virtual ~IndependentDrawPort();

	virtual IMAGERESULT Init(Int32 x, Int32 y);
	virtual void Free();

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha)
	{
		return false;
	}
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue) { }
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_RGBb24 : public IndependentDrawPort
{
public:
	typedef UChar DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)(Bits + ((Int((bh - y) - 1) * Int(Bpz)) + Int(x * 3)));
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_RGBb32 : public IndependentDrawPort
{
public:
	typedef UInt32 DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)((UInt32*)&(Bits[((Int((bh - y) - 1) * Int(Bpz)) + Int(x << 2))]));
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_RGBw : public IndependentDrawPort
{
public:
	typedef UInt16 DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)&Bits[Int(y) * Int(Bpz) + Int(x) * 3 * sizeof(DP_PIX)];
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_RGBf : public IndependentDrawPort
{
public:
	typedef Float32 DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)&Bits[Int(y) * Int(Bpz) + Int(x) * 3 * sizeof(DP_PIX)];
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_GREYb8 : public IndependentDrawPort
{
public:
	typedef UChar DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)&Bits[y * Bpz + x * 1 * sizeof(DP_PIX)];
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_GREYw : public IndependentDrawPort
{
public:
	typedef UInt16 DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)&Bits[y * Bpz + x * 1 * sizeof(DP_PIX)];
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

//------------------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//------------------------------------------------------------------------------------------------
class DrawPort_GREYf : public IndependentDrawPort
{
public:
	typedef Float32 DP_PIX;
	DP_PIX* GetPtr(Int32 x, Int32 y)
	{
		return (DP_PIX*)&Bits[y * Bpz + x * 1 * sizeof(DP_PIX)];
	}

	virtual Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha);
	virtual void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue);
	virtual Vector32 GetPixelDirect(Int32 x, Int32 y);
	virtual Bool SetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
	virtual void GetPixelCntP(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, PIXELCNT flags);
};

#pragma pack (pop)
}

#endif	// C4D_DRAWPORT_H__
