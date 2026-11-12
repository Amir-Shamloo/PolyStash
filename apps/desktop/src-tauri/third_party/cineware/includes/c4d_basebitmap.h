/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEBITMAP_H__
#define C4D_BASEBITMAP_H__

#include "c4d_drawport.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseBitmap;
class DrawPort;
class Filename;
class BaseContainer;

class ColorProfileConvert
{
};

/// @addtogroup INITBITMAPFLAGS
/// @ingroup group_enumeration
/// @{
/// Flags used for the initialization of @link BaseBitmap bitmaps@endlink.
/// @see BaseBitmap::Init(Int32 x, Int32 y, Int32 depth = 24, INITBITMAPFLAGS flags = INITBITMAPFLAGS_0)
enum INITBITMAPFLAGS
{
	INITBITMAPFLAGS_0					= 0,				///< None.
	INITBITMAPFLAGS_GRAYSCALE = (1 << 0),	///< Initializes as grayscale bitmap.
	INITBITMAPFLAGS_SYSTEM		= (1 << 1)	///< @markPrivate
} ENUM_END_FLAGS(INITBITMAPFLAGS);
/// @}

/// @markPrivate @markInternal
#define MAX_CHANNELS 4

/// @markPrivate @markInternal
struct CHANNEL
{
	BaseBitmap* bm;
	Bool				IsIntern;
};

//------------------------------------------------------------------------------------------------
/// The bitmap class can be used to load, read, draw and save bitmap pictures of various formats.\n
/// Bitmaps are organized so that the 2D point <i>[0,0]</i> is the top left corner. Be sure to call Init(Int32 x, Int32 y, Int32 depth = 24, INITBITMAPFLAGS flags = INITBITMAPFLAGS_0) before attempting to use a newly allocated bitmap.
//------------------------------------------------------------------------------------------------
class BaseBitmap
{
	friend class HyperFile;
	friend class BaseMaterial;
	friend class SkyObjects;
	friend class iMatPreviewData;
	friend class JpgLoader;
	friend class BmpLoader;

private:
	DrawPort* dp;					///< @markPrivate @markInternal
	COLORMODE colormode;	///< @markPrivate @markInternal

protected:
	CHANNEL Channels[MAX_CHANNELS];																																							///< @markPrivate @markInternal

	IMAGERESULT Init(void* mem, Int size);																																			///< @markPrivate @markInternal
	void _Free();																																														///< @markPrivate @markInternal
	virtual IMAGERESULT _Init(Int32 x, Int32 y, Int32 t, DrawPortTypes type = DpRegular, void* data = nullptr);	///< @markPrivate @markInternal

	inline Bool Check(const Vector& v) const;																																		///< @markPrivate @markInternal
	inline Bool Check(Int32 x1, Int32 y1) const;																																///< @markPrivate @markInternal
	inline Bool Check(Int32 x1, Int32 y1, Int32 x2, Int32 y2) const;																						///< @markPrivate @markInternal

public:
	void* unknownBitmapData;	///< @markPrivate @markInternal
	Int		unknownBitmapSize;	///< @markPrivate @markInternal

	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseBitmap();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseBitmap();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{bitmap}
	/// @since R20
	/// @return												@allocReturn{bitmap}
	//----------------------------------------------------------------------------------------
	static BaseBitmap* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{bitmaps}
	/// @since R20
	/// @param[in,out] bm							@theToDestruct{bitmap}
	//----------------------------------------------------------------------------------------
	static void Free(BaseBitmap*& bm);

	/// @}

	/// @name Init/Load/Save
	/// @{

	//----------------------------------------------------------------------------------------
	/// Initializes the bitmap to the given [@formatParam{x},@formatParam{y}] dimensions and @formatParam{depth}.\n
	/// @warning	Any previous data in the bitmap object is lost.
	/// @param[in] x									The requested width in pixels (Maximum 16 000 pixels).
	/// @param[in] y									The requested height in pixels (Max. 16000 pixels).
	/// @param[in] depth							The requested bit depth. The possible values are (@em 1,  @em 4,  @em 8,  @em 16,  @em 24,  @em 32,  @em 64,  @em 96).\n
	///																On some platforms <i>32</i>-bit will be used even if @em 24 is requested, to allow for padding. In this case GetBt() will return  @em 32.
	/// @param[in] flags							The initialization flags.
	/// @return												The image result.
	//----------------------------------------------------------------------------------------
	virtual IMAGERESULT Init(Int32 x, Int32 y, Int32 depth = 24, INITBITMAPFLAGS flags = INITBITMAPFLAGS_0);

	//----------------------------------------------------------------------------------------
	/// Loads the bitmap from a file.
	/// @param[in] name								A valid filename that can either refer a movie or a picture.
	/// @param[in] format							The image format.
	/// @param[in] data								Some formats provide additional settings. Can be @formatConstant{nullptr}, which means the same as an empty container. @callerOwnsPointed{container}
	///																@markTODO{Add container IDs for additional settings in @formatParam{data}}
	/// @param[in] frame							The frame number to load in a movie.
	/// @return												The image result.
	//----------------------------------------------------------------------------------------
	IMAGERESULT Load(const Filename& name, Int32 format, BaseContainer* data, Int32 frame);

	//----------------------------------------------------------------------------------------
	/// Saves the bitmap to a file.
	/// @param[in] name								A valid filename that can either refer a movie or a picture.
	/// @param[in] format							The image format.
	/// @param[in] data								Some formats provide additional settings. Can be @formatConstant{nullptr}, which means the same as an empty container. @callerOwnsPointed{container}
	///																@markTODO{Add container IDs for additional settings in @formatParam{data}}
	/// @param[in] savebits						A combination of the following flags.
	/// @return												The image result.
	//----------------------------------------------------------------------------------------
	IMAGERESULT Save(const Filename& name, Int32 format, BaseContainer* data, SAVEBIT savebits);

	/// @}

	/// @name Bitmap Dimension/Information
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the width of the bitmap in pixels. If the bitmap has not been initialized the return value is @em 0.
	/// @note	Calling this method is the only way to see if a bitmap passed by @C4D or some other source has been initialized.
	/// @return												The bitmap width in pixels, or @em 0 if the bitmap is not initialized.
	//----------------------------------------------------------------------------------------
	Int32 GetBw() const;

	//----------------------------------------------------------------------------------------
	/// Gets the height of the bitmap in pixels.
	/// @return												The bitmap height in pixels.
	//----------------------------------------------------------------------------------------
	Int32 GetBh() const;

	//----------------------------------------------------------------------------------------
	/// Gets the number of bits per pixel.
	/// @return												The number of bits per pixel.
	//----------------------------------------------------------------------------------------
	Int32 GetBt() const;

	//----------------------------------------------------------------------------------------
	/// Gets the color mode of the bitmap.
	/// @return												The color mode: @enumerateEnum{COLORMODE}
	//----------------------------------------------------------------------------------------
	COLORMODE GetColorMode();

	/// @}

	/// @name Clone/Copy/Scale
	/// @{

	//----------------------------------------------------------------------------------------
	/// Copies the bitmap.
	/// @param[out] dest							The cloned bitmap, or @formatConstant{nullptr} if an error occurred. @callerOwnsPointed{bitmap}
	/// @return												@trueIfOtherwiseFalse{the bitmap was successfully cloned and assigned to @formatParam{dest}}
	//----------------------------------------------------------------------------------------
	Bool CloneIt(BaseBitmap* dest) const;

	//----------------------------------------------------------------------------------------
	/// Copies the bitmap.
	/// @return												The cloned bitmap, or @formatConstant{nullptr} if an error occurred. @callerOwnsPointed{bitmap}
	//----------------------------------------------------------------------------------------
	BaseBitmap* GetClone();

	//----------------------------------------------------------------------------------------
	/// Copies part of the bitmap to @formatParam{dest}.
	/// @param[out] dest							The bitmap to copy the bitmap to. @callerOwnsPointed{bitmap}
	/// @param[in] x									The @em X position of the bitmap part to be copied.
	/// @param[in] y									The @em Y position of the bitmap part to be copied.
	/// @param[in] w									The width of the bitmap part to be copied.
	/// @param[in] h									The height of the bitmap part to be copied.
	/// @param[in] no_reallocation		Pass @formatConstant{true} if @formatParam{dest} is an already initialized bitmap with @formatParam{width} and @formatParam{height} equal to @formatParam{w} and @formatParam{h}. Faster without initialization/allocation.
	/// @return												@trueIfOtherwiseFalse{the bitmap part was successfully copied}
	//----------------------------------------------------------------------------------------
	Bool CopyPart(BaseBitmap* dest, Int32 x, Int32 y, Int32 w, Int32 h, Bool no_reallocation) const;

	//----------------------------------------------------------------------------------------
	/// Scales the bitmap to fit in the destination bitmap and copies it there.\n
	/// The destination bitmap @formatParam{dest} needs to be initialized with the destination size before calling this function.
	/// @param[out] dest							The destination bitmap. @callerOwnsPointed{bitmap}
	/// @param[in] hell								Lets you change brightness of the image (@em 128 = <i>50</i>% brightness, @em 256 = unchanged).
	/// @param[in] sample							If @formatConstant{true} a better scaling algorithm is used, which results in a better quality but a bit slower.
	/// @param[in] nprop							Must be @formatConstant{true} if non-proportional scaling is wanted.
	/// @param[in] disable_gamma			Pass @formatConstant{true} to disable gamma correction.
	//----------------------------------------------------------------------------------------
	void ScaleIt(BaseBitmap* dest, Int32 hell, Bool sample, Bool nprop = false, Bool disable_gamma = false) const;

	/// @}

	/// @name Set/Get Pixels
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the pixel at [@formatParam{x},@formatParam{y}] to the color specified by (@formatParam{red},@formatParam{green},@formatParam{blue}) (0 <= red/green/blue <= 255). The @formatParam{alpha} value for the pixel can also be set.\n
	/// The range of the @formatParam{red}, @formatParam{green}, @formatParam{blue} and @formatParam{alpha} parameters is @em 0 to @em 255, where (@em 255,@em 255,@em 255) is white or opaque, regardless of the bit depth of the image.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	/// @param[in] red								The red component.
	/// @param[in] green							The green component.
	/// @param[in] blue								The blue component.
	/// @param[in] alpha							The alpha.
	/// @return												@trueIfOtherwiseFalse{the pixel was successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetPixel(Int32 x, Int32 y, Int32 red, Int32 green, Int32 blue, Int32 alpha = 0);

	//----------------------------------------------------------------------------------------
	/// Gets the color at [@formatParam{x},@formatParam{y}] and assigns it to the passed (@formatParam{red},@formatParam{green},@formatParam{blue}) parameters.\n
	/// The range of the assigned @formatParam{red}, @formatParam{green} and @formatParam{blue} parameters is @em 0 to @em 255, where (@em 255,@em 255,@em 255) is white, regardless of the bit depth of the image.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	/// @param[out] red								Assigned the red component.
	/// @param[out] green							Assigned the green component.
	/// @param[out] blue							Assigned the blue component.
	//----------------------------------------------------------------------------------------
	void GetPixel(Int32 x, Int32 y, UInt16* red, UInt16* green, UInt16* blue) const;

	//----------------------------------------------------------------------------------------
	/// Gets the color at [@formatParam{x},@formatParam{y}] as RGB Vector.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	//----------------------------------------------------------------------------------------
	Vector GetRGB(Int32 x, Int32 y);

	//----------------------------------------------------------------------------------------
	/// Sets the alpha value at [@formatParam{x},@formatParam{y}] to @formatParam{val}.
	/// @param[in] channel						The alpha channel to use.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	/// @param[in] val								The alpha value. @em 0 <= @formatParam{val} <= @em 255
	//----------------------------------------------------------------------------------------
	virtual Bool SetAlphaPixel(BaseBitmap* channel, Int32 x, Int32 y, Int32 val);

	//----------------------------------------------------------------------------------------
	/// Gets the alpha value at [@formatParam{x},@formatParam{y}] and assigns it to the passed parameter @formatParam{val} (@em 0 <= @formatParam{val} <= @em 255).
	/// @param[in] channel						The alpha channel to use.
	/// @param[in] x									The X coordinate.
	/// @param[in] y									The Y coordinate.
	/// @param[out] val								Assigned the alpha value.
	//----------------------------------------------------------------------------------------
	virtual void GetAlphaPixel(BaseBitmap* channel, Int32 x, Int32 y, UInt16* val) const;

	//----------------------------------------------------------------------------------------
	/// Sets @formatParam{cnt} pixels at [@formatParam{x},@formatParam{y}] in the bitmap from @formatParam{buffer} with color mode @formatParam{srcmode}, incrementing @formatParam{inc} bytes for each pixel.
	/// @param[in] x									The X coordinate of the first pixel to set.
	/// @param[in] y									The Y coordinate of the first pixel to set.
	/// @param[in] cnt								The number of pixels to set.
	/// @param[in] buffer							A pointer to a large enough memory buffer containing the pixels to set. @callerOwnsPointed{buffer}
	/// @param[in] inc								The byte increment per pixel in the buffer.
	/// @param[in] srcmode						The source color mode. @b Warning: None of the alpha modes are supported.
	/// @param[in] flags							The flags.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetPixelCnt(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, COLORMODE srcmode, PIXELCNT flags);

	//----------------------------------------------------------------------------------------
	/// Reads @formatParam{cnt} pixels from [@formatParam{x},@formatParam{y}] in the bitmap to the @formatParam{buffer} with the color mode @formatParam{dstmode}, incrementing @formatParam{inc} bytes for each pixel.
	/// @param[in] x									The X coordinate of the first pixel to get.
	/// @param[in] y									The Y coordinate of the first pixel to get.
	/// @param[in] cnt								The number of pixels to get.
	/// @param[out] buffer						A pointer to a large enough memory buffer where the pixel data will be written. @callerOwnsPointed{buffer}
	/// @param[in] inc								The byte increment per pixel in the buffer.
	/// @param[in] dstmode						The destination color mode.
	/// @param[in] flags							The flags.
	/// @param[in] conversion					This should be normally set to @formatConstant{nullptr}. Pass a color profile only if a conversion is wanted before retrieving the pixel data.\n
	///																This only works if either the bitmap is <i>32</i>-bit per component (so no <i>8</i>/<i>16</i>-bit images) or the @formatParam{dstmode} is <i>32</i>-bit per component.\n
	///																The conversion is done before color reduction (e.g. if @formatParam{dstmode} is <i>16</i>-bit the profile is first applied and then the data resampled to <i>16</i>-bit).
	//----------------------------------------------------------------------------------------
	virtual void GetPixelCnt(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, COLORMODE dstmode, PIXELCNT flags, ColorProfileConvert* conversion = nullptr) const;

	//----------------------------------------------------------------------------------------
	/// Sets @formatParam{cnt} alpha pixels at [@formatParam{x},@formatParam{y}] in the bitmap @formatParam{channel} from @formatParam{buffer} incrementing @formatParam{inc} bytes for each pixel.
	/// @param[in] x									The X coordinate of the first pixel to set.
	/// @param[in] y									The Y coordinate of the first pixel to set.
	/// @param[in] cnt								The number of pixels to set.
	/// @param[in] buffer							A pointer to a large enough memory buffer containing the alpha values to set. @callerOwnsPointed{buffer}
	/// @param[in] inc								The byte increment per pixel in the buffer.
	/// @param[in] channel						The channel to set the alpha values. @callerOwnsPointed{bitmap}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool SetAlphaPixelCnt(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, BaseBitmap* channel);

	//----------------------------------------------------------------------------------------
	/// Reads @formatParam{cnt} alpha pixels from [@formatParam{x},@formatParam{y}] in the bitmap @formatParam{channel} to the @formatParam{buffer} incrementing @formatParam{inc} bytes for each pixel.
	/// @param[in] x									The X coordinate of the first pixel to get.
	/// @param[in] y									The Y coordinate of the first pixel to get.
	/// @param[in] cnt								The number of pixels to get.
	/// @param[out] buffer						A pointer to a large enough memory buffer where the pixel data will be written. @callerOwnsPointed{buffer}
	/// @param[in] inc								The byte increment per pixel in the buffer.
	/// @param[in] channel						The channel to get the alpha values. @callerOwnsPointed{bitmap}
	//----------------------------------------------------------------------------------------
	virtual void GetAlphaPixelCnt(Int32 x, Int32 y, Int32 cnt, UChar* buffer, Int32 inc, BaseBitmap* channel) const;

	//----------------------------------------------------------------------------------------
	/// Sets all line pixels at @formatParam{y} in the bitmap from @formatParam{data}.
	/// @param[in] y									The Y coordinate of the line to set.
	/// @param[in] data								A pointer to a large enough memory buffer containing the pixels to set. @callerOwnsPointed{buffer}
	/// @param[in] depth							The bit depth. The possible values are (@em 1,  @em 4,  @em 8,  @em 16,  @em 24,  @em 32,  @em 64,  @em 96). On some platforms <i>32</i>-bit will be used even if @em 24 is requested, to allow for padding. In this case GetBt() will return  @em 32.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetLineDirect(Int32 y, void* data, Int32 depth);

	/// @}

	/// @name Channel
	/// @{

	//----------------------------------------------------------------------------------------
	/// Adds a new alpha channel to the bitmap.
	/// @note	The bitmap class only supports up to @em 4 channels.
	/// @param[in] internal						Should only be @formatConstant{true} for the first alpha. The internal alpha will be stored within a bitmaps if its format supports alphas.
	/// @param[in] straight						Should be @formatConstant{true} if the bitmap has to be interpreted as straight.\n
	///																For information about straight alphas please take a look at the corresponding option in the render settings and the manual of @C4D.
	/// @return												The newly added channel if successful, otherwise @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	virtual BaseBitmap* AddChannel(Bool internal, Bool straight = false);

	//----------------------------------------------------------------------------------------
	/// Removes the specified @formatParam{channel} from the bitmap.
	/// @param[in] channel						The alpha channel to remove.
	//----------------------------------------------------------------------------------------
	virtual void RemoveChannel(BaseBitmap* channel);

	//----------------------------------------------------------------------------------------
	/// Gets the internal read-only alpha channel.\n
	/// The internal alpha channel is the one that is saved together with the picture, with those formats that support this. If no internal alpha is available, nullptr is returned.
	/// @return												The read-only internal alpha channel, or @formatConstant{nullptr} if no internal alpha is available. @theOwnsPointed{bitmap, channel}
	//----------------------------------------------------------------------------------------
	virtual BaseBitmap* GetInternalChannel() const;

	//----------------------------------------------------------------------------------------
	/// Gets the number of alpha channels in the bitmap, including the internal channel.
	/// @return												The number of alpha channels.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetChannelCount() const;

	//----------------------------------------------------------------------------------------
	/// Gets the alpha channel specified by @formatParam{num}.
	/// @param[in] num								An alpha channel index between @em 0 and GetChannelCount().
	/// @return												The requested channel.
	//----------------------------------------------------------------------------------------
	virtual BaseBitmap* GetChannelNum(Int32 num) const;

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASEBITMAP_H__
