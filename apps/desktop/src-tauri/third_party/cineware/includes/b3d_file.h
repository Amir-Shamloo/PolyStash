/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef B3D_FILE_H__
#define B3D_FILE_H__

#include "ge_math.h"
#include "c4d_privatechunk.h"
#include "c4d_basetime.h"
#include "c4d_parameter_ids.h"
#include "private_symbols.h"
#include "private_ge_mtools.h"
#include "c4d_privatechunk.h"
#include "c4d_drawport.h"
#include "c4d_hyperfile.h"
#include "c4d_filename.h"
#include "c4d_basebitmap.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseContainer;
class BaseObject;
class IpConnection;
class AliasTrans;
class BaseLink;
class BaseList2D;
class DescID;
struct CustomDataType;
class BaseDocument;

//----------------------------------------------------------------------------------------
/// Identifies the type of the given file @formatParam{name}.
/// @param[in] name								File to check.
/// @param[in] recognition				Identification flags.
/// @return												The file identification type.
//----------------------------------------------------------------------------------------
Int32 GeIdentifyFile(const Filename& name, Int32 recognition = IDENTIFYFILE_SCENE);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
Bool B3DIdentify(const Char* filename, UChar* probe, Int32 size);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
Bool B3DLoad(const Char* filename, const Char* searchstring);

//----------------------------------------------------------------------------------------
/// Checks if a file or directory exists.
/// @param[in] name								The name of the file or directory to check for.
/// @param[in] isdir							@formatConstant{true} if checking a directory.
/// @return												@trueIfOtherwiseFalse{the file or directory exists}
//----------------------------------------------------------------------------------------
Bool GeFExist(const Filename& name, Bool isdir = false);

class B3DImageFilter;
class B3DImage;
class B3DAlpha;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class B3DPaintLayer : public PrivateChunk
{
	friend class B3DAlpha;
	COLORMODE	colormode;
	UChar			nativecolor[B3D_BYTE_MAX];

protected:
	B3DAlpha*	parent;

public:
	B3DPaintLayer();

	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	virtual Bool Init(HyperFile* hf, Int32 w, Int32 h, Int32 colormode);
	virtual Bool SetPixelCount(HyperFile* hf, Int32 x, Int32 y, Int32 numx, UChar* buf, Int32 channels);
	virtual COLORMODE GetColorMode(HyperFile* hf);
	virtual void GetPixelCount(HyperFile* hf, Int32 x, Int32 y, Int32 numx, UChar* buf);
	virtual Bool Write(HyperFile* hf);
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class B3DAlpha : public PrivateChunk
{
	B3DPaintLayer	paintlayer;
	Int32					channel, colormode;

public:
	B3DAlpha();
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	virtual Bool Init(HyperFile* hf, Int32 w, Int32 h, Int32 colormode);
	virtual Bool SetPixelCount(HyperFile* hf, Int32 x, Int32 y, Int32 numx, UChar* buf, Int32 channels);
	virtual COLORMODE GetColorMode(HyperFile* hf);
	virtual void GetPixelCount(HyperFile* hf, Int32 x, Int32 y, Int32 numx, UChar* buf);

	virtual Bool Write(HyperFile* hf);
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class B3DTexture : public PrivateChunk
{
	B3DAlpha b3dalpha;

public:
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class B3DImageFilter : public PrivateChunk
{
	friend class B3DTexture;
	friend class B3DPaintLayer;
	friend class B3DAlpha;
	HyperFile file;

protected:
	B3DImage*	 bmp;
	B3DTexture b3dtex;

	Int32			 width, height, colormode;

public:
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	Int32 PackBitsEncode(UChar* unpacked, Int32 usize, UChar* packed, Int32 psize);
	Bool PackBitsDecode(UChar* packed, Int psize, UChar* unpacked, Int32 usize);

	Bool LoadImage(const Char* name, B3DImage* t_bmp);
	Bool SaveImage(const Char* name, B3DImage* t_bmp);

	B3DImageFilter();
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class B3DImage
{
public:
	// Overload in child classes.

	// Needed to import BodyPaint 3D files.

	virtual Bool Init(Int32 w, Int32 h, Int32 colormode) = 0;
	virtual Bool InitAlphaChannel(Int32 w, Int32 h, Int32 colormode);
	virtual Bool SetPixelCount(Int32 x, Int32 y, Int32 numx, UChar* buf, Int32 channels);
	virtual Bool SetAlphaPixel(Int32 channel, Int32 x, Int32 y, Int32 numx, UChar* c_buf);

	// Needed to export BodyPaint 3D files.

	virtual Int32 GetBw() = 0;
	virtual Int32 GetBh() = 0;
	virtual COLORMODE GetColorMode() = 0;
	virtual void GetPixelCount(Int32 x, Int32 y, Int32 cnt, UChar* data) = 0;
	virtual Int32 GetAlphaCount() = 0;
	virtual void GetAlphaPixel(Int32 channel, Int32 x, Int32 y, Int32 cnt, UChar* data) = 0;
};

#pragma pack (pop)
}

#endif	// B3D_FILE_H__
