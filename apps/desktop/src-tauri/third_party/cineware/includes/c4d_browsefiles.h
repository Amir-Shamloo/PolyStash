/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BROWSEFILES_H__
#define C4D_BROWSEFILES_H__

#include "c4d_file.h"

namespace cineware
{
#pragma pack (push, 8)

#ifndef MAXON_TARGET_ANDROID

class SysBrowseFiles;

/// @addtogroup BROWSEFILES
/// @ingroup group_enumeration
/// @{
/// @since R19
#define BROWSEFILES_CALCSIZE (1 << 0)	///< Specifies if BrowseFiles::GetSize() can be called later on. Only works for files, not for folders.
/// @}

//----------------------------------------------------------------------------------------
/// Helper to browse through files.
/// @since R19
//----------------------------------------------------------------------------------------
class BrowseFiles
{
public:
	BrowseFiles();
	~BrowseFiles();

	//----------------------------------------------------------------------------------------
	/// Initializes the class to browse a certain directory.
	/// @param[in] directory					The directory to browse.
	/// @param[in] flags							The browser files flags: @enumerateEnum{BROWSEFILES}
	//----------------------------------------------------------------------------------------
	void Init(const Filename& directory, Int32 flags);

	//----------------------------------------------------------------------------------------
	/// Gets the next file or subdirectory in the browsed directory.
	/// @note To retrieve the first file call GetNext() once.
	/// @b Example:
	/// BrowseFiles *bf = AllocBrowseFiles(dir, false);
	/// if (bf)
	/// {
	///   while (bf->GetNext())
	///   {
	///     GePrint(bf->GetFilename().GetString());
	///   }
	///   FreeBrowseFiles(bf);
	/// }
	/// @return												@trueIfOtherwiseFalse{another file was found}
	//----------------------------------------------------------------------------------------
	Bool GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the size for the current file.
	/// @return												The byte size of the file.
	/// @warning											Not valid for directories.
	//----------------------------------------------------------------------------------------
	Int64 GetSize();

	//----------------------------------------------------------------------------------------
	/// Checks if the current element is a directory.
	/// @return												@trueIfOtherwiseFalse{the current element is a directory}
	//----------------------------------------------------------------------------------------
	Bool IsDir();

	//----------------------------------------------------------------------------------------
	/// Checks if the current file or directory is hidden.
	/// @return												@trueIfOtherwiseFalse{the current file is hidden}
	//----------------------------------------------------------------------------------------
	Bool IsHidden();

	//----------------------------------------------------------------------------------------
	/// Checks if the current file or directory is a bundle.
	/// @return												@trueIfOtherwiseFalse{the current file is a bundle}
	//----------------------------------------------------------------------------------------
	Bool IsBundle();

	//----------------------------------------------------------------------------------------
	/// Checks if the current file or directory is read-only.
	/// @return												@trueIfOtherwiseFalse{the current file is read-only}
	//----------------------------------------------------------------------------------------
	Bool IsReadOnly();

	//----------------------------------------------------------------------------------------
	/// Gets a time for the current file or directory.
	/// @param[in] mode								The file time mode: @enumerateEnum{GE_FILETIME}
	/// @param[out] out								Filled with the retrieved file time. @callerOwnsPointed{file time}
	//----------------------------------------------------------------------------------------
	void GetFileTime(Int32 mode, LocalFileTime* out);

	//----------------------------------------------------------------------------------------
	/// Gets the name of the current file or directory.
	/// @return												The name of the current file or directory.
	//----------------------------------------------------------------------------------------
	Filename GetFilename();

	//----------------------------------------------------------------------------------------
	/// Gets the name of the current file path or directory.
	/// @return												The path of the current file or directory.
	//----------------------------------------------------------------------------------------
	Filename GetFilePath();

private:
	SysBrowseFiles* uncached;
};

#endif

#pragma pack (pop)
}

#endif	// C4D_BROWSEFILES_H__
