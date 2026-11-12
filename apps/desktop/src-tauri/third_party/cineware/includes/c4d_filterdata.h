/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_FILTERDATA_H__
#define C4D_FILTERDATA_H__

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// A data class for bitmap loaders plugins.
//----------------------------------------------------------------------------------------
class BitmapLoaderData
{
public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BitmapLoaderData() { }

	//----------------------------------------------------------------------------------------
	/// Identifies the file type as one that can be loaded using the bitmap loader plugin. If possible, the file should not be identified through the suffix, but through the probe data.
	/// @param[in] name								The filename of the file.
	/// @param[in] probe							The start of a small chunk of data from the start of the file for testing the file type. Usually the probe size is @em 1024 bytes. @cinewareOwnsPointed{array}
	/// @param[in] size								The size of the chunk for testing the file type.
	/// @return												@formatConstant{true} if the plugin recognizes the file.
	//----------------------------------------------------------------------------------------
	virtual Bool Identify(const Filename& name, UChar* probe, Int32 size) = 0;

	//----------------------------------------------------------------------------------------
	/// Loads the image file into a bitmap.
	/// @param[in] name								The name of the file to load.
	/// @param[in] bm									The bitmap to load the image into. @callerOwnsPointed{bitmap}
	/// @param[in] frame							The frame number for formats containing multiple images in a file such as @em Quicktime or @em AVI.
	/// @return												The result of loading the file: @enumerateEnum{IMAGERESULT}
	//----------------------------------------------------------------------------------------
	virtual IMAGERESULT Load(const Filename& name, BaseBitmap* bm, Int32 frame) = 0;

	//----------------------------------------------------------------------------------------
	/// Returns the ID of the corresponding bitmap saver for the bitmap loader, if there is one.
	/// @return												The plugin ID of the corresponding BitmapSaverData, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetSaver() = 0;
};

//----------------------------------------------------------------------------------------
/// A data class for bitmap savers plugins.
//----------------------------------------------------------------------------------------
class BitmapSaverData
{
public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BitmapSaverData() { }

	//----------------------------------------------------------------------------------------
	/// Saves a bitmap to a file.
	/// @param[in] name								The name of the file to save.
	/// @param[in] bm									The bitmap to save the image from. @callerOwnsPointed{bitmap}
	/// @param[in] data								The settings for the bitmap saver plugin. These settings are stored with the general preferences. @callerOwnsPointed{container}
	/// @param[in] savebits						The flags for saving the bitmap: @enumerateEnum{SAVEBIT}
	/// @return												The result of saving the file: @enumerateEnum{IMAGERESULT}
	//----------------------------------------------------------------------------------------
	virtual IMAGERESULT Save(const Filename& name, BaseBitmap* bm, BaseContainer* data, SAVEBIT savebits) = 0;
};

#pragma pack (pop)
}

#endif	// C4D_FILTERDATA_H__
