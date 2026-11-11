/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_FILENAME_H__
#define C4D_FILENAME_H__

#include "c4d_string.h"
#include "c4d_memoryfilestruct.h"

namespace cineware
{
#pragma pack (push, 8)

/// URL separator.
#define URL_SEPARATOR '/'

/// System separator.
#if defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS)
	#define SYSTEM_SEPARATOR '/'
#else
	#define SYSTEM_SEPARATOR '\\'
#endif

//----------------------------------------------------------------------------------------
/// Manages file and path names.
//----------------------------------------------------------------------------------------
class Filename
{
	friend class BaseFile;

private:
	String						t_name;								///< @markPrivate @markInternal
	MemoryFileStruct*	t_memblock,						///< @markPrivate @markInternal
										t_readmfs;						///< @markPrivate @markInternal

	Bool GetMemoryMode() const;							///< @markPrivate @markInternal
	inline UInt16 GetSeparator() const;			///< @markPrivate @markInternal

public:
	/// @name Constructors/Destructors
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	Filename();

	//----------------------------------------------------------------------------------------
	/// Creates the filename from a character array.
	/// @param[in] string							The filename character array.
	//----------------------------------------------------------------------------------------
	Filename(const Char* string);

	//----------------------------------------------------------------------------------------
	/// Creates the filename from a string.
	/// @param[in] string							The filename string.
	//----------------------------------------------------------------------------------------
	Filename(const String& string);

	//----------------------------------------------------------------------------------------
	/// Copy constructor. Creates a filename from another filename.
	/// @param[in] fn									The source filename.
	//----------------------------------------------------------------------------------------
	Filename(const Filename& fn);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~Filename();

	/// @}

	/// @name String Content
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the filename has been set.
	/// @since R20 Was Content() before.
	/// @return												@trueIfOtherwiseFalse{filename was set}
	//----------------------------------------------------------------------------------------
	Bool IsPopulated() const;

	//----------------------------------------------------------------------------------------
	/// Gets the String of the filename.
	/// @return												The string with the full filename.
	//----------------------------------------------------------------------------------------
	const String GetString() const;

	//----------------------------------------------------------------------------------------
	/// Sets the filename to the passed string.
	/// @param[in] str								The string to use to set the filename.
	//----------------------------------------------------------------------------------------
	void SetString(const String& str);

	//----------------------------------------------------------------------------------------
	/// Checks if the filename is a <i>Content Browser</i> URL.
	/// @return												@trueIfOtherwiseFalse{the file name is a <i>Content Browser</i> URL}
	//----------------------------------------------------------------------------------------
	Bool IsBrowserUrl() const;

	/// @}

	/// @name Read/Write Memory
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the filename to read from a memory block instead of from a file.
	/// @param[in] adr								The memory to read from. @callerOwnsPointed{memory block}
	/// @param[in] size								The size of memory block @formatParam{adr}, or @em -1 if the buffer is "big enough".
	//----------------------------------------------------------------------------------------
	void SetMemoryReadMode(void* adr, Int size);

	//----------------------------------------------------------------------------------------
	/// Sets the filename to write to a memory block instead of to a file.
	/// @param[in] mfs								The memory file to write to. @callerOwnsPointed{memory file}
	//----------------------------------------------------------------------------------------
	void SetMemoryWriteMode(MemoryFileStruct& mfs);

	/// @}

	/// @name Directory/File Parts
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the directory that the file is located.
	/// @return												The directory part for the filename.
	//----------------------------------------------------------------------------------------
	const Filename GetDirectory() const;

	//----------------------------------------------------------------------------------------
	/// Gets the file part of the filename only, without the path.
	/// @return												The file part of the filename.
	//----------------------------------------------------------------------------------------
	const Filename GetFile() const;

	//----------------------------------------------------------------------------------------
	/// Sets the directory for the filename. The file part remains unchanged.
	/// @param[in] str								The directory to set for the filename.
	//----------------------------------------------------------------------------------------
	void SetDirectory(const Filename& str);

	//----------------------------------------------------------------------------------------
	/// Sets the file part for the filename. The directory part remains unchanged.
	/// @param[in] str								The file part to set for the filename.
	//----------------------------------------------------------------------------------------
	void SetFile(const Filename& str);

	//----------------------------------------------------------------------------------------
	/// Convenience function to directly get the file part string. Identical to <code>GetFile().GetString()</code>.
	/// @return												The file part of the filename as a string.
	//----------------------------------------------------------------------------------------
	const String GetFileString() const;

	/// @}

	/// @name Suffix Part
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the suffix of the filename.
	/// @return												The suffix. Only contain the suffix part and no dot (e.g. @em "TIF").
	//----------------------------------------------------------------------------------------
	String GetSuffix() const;

	//----------------------------------------------------------------------------------------
	/// Sets the suffix part of the filename.
	/// @note	Suffix strings do not contain the dot character @em '.'.
	/// @param[in] str								A string with the suffix for the file. Only contain the suffix part and no dot (e.g. @em "TIF").
	//----------------------------------------------------------------------------------------
	void SetSuffix(const String& str);

	//----------------------------------------------------------------------------------------
	/// Clears the suffix part of the filename.
	//----------------------------------------------------------------------------------------
	void ClearSuffix();

	//----------------------------------------------------------------------------------------
	/// The same as ClearSuffix() only that it also clears suffixes longer than seven characters.
	//----------------------------------------------------------------------------------------
	void ClearSuffixComplete();

	//----------------------------------------------------------------------------------------
	/// Checks if the suffix of the filename is the same as its string.
	/// @param[in] str								The suffix to check against.
	/// @return												@trueIfOtherwiseFalse{the suffix is the same}
	//----------------------------------------------------------------------------------------
	Bool CheckSuffix(const String& str) const;

	/// @}

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Assignment operator. Assigns the filename right-operand to the left-operand filename.
	/// @param[in] fname							The right-operand filename.
	//----------------------------------------------------------------------------------------
	const Filename& operator = (const Filename& fname);

	//----------------------------------------------------------------------------------------
	/// Concatenates two filenames. It will join directories and files parts.
	/// @param[in] fname1							The left-operand filename.
	/// @param[in] fname2							The right-operand filename.
	//----------------------------------------------------------------------------------------
	friend const Filename operator + (const Filename& fname1, const Filename& fname2);

	//----------------------------------------------------------------------------------------
	/// Concatenates filenames and set the left-hand operand to the result.
	/// @param[in] fname							The right-operand filename.
	//----------------------------------------------------------------------------------------
	const Filename& operator += (const Filename& fname);

	//----------------------------------------------------------------------------------------
	/// Checks if filenames are equal.
	/// @param[in] fname							The right-operand filename.
	//----------------------------------------------------------------------------------------
	Bool operator == (const Filename& fname) const;

	//----------------------------------------------------------------------------------------
	/// Checks if filenames are different.
	/// @param[in] fname							The right-operand filename.
	//----------------------------------------------------------------------------------------
	Bool operator != (const Filename& fname) const;

	/// @}

	/// @name Mac OS X
	/// @{

#ifdef MAXON_TARGET_OSX
	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool GetFSRef(FSRef* fs) const;

	//	Bool SetFSRef(FSRef *fs);
	//	Bool SetCFURL(CFURLRef url);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	CFURLRef CreateCFURL() const;

	//----------------------------------------------------------------------------------------
	/// Converts a file name to a UTF8 @em C string for the standard library functions.
	/// @note: <i>OS X</i> only.
	/// @param[out] out_path					Assigned the UTF8 @em C string.
	/// @param[in] max								The length of the @formatParam{out_path} string.
	/// @return												@trueIfOtherwiseFalse{file name was successfully converted}
	//----------------------------------------------------------------------------------------
	Bool ConvertToUTF8(Char* out_path, Int32 max) const;
#endif

	/// @}

	//  /// @name Private
	//  /// @{
	//
	//  //----------------------------------------------------------------------------------------
	//  /// @markPrivate
	//  //----------------------------------------------------------------------------------------
	//  void SetIStream(void* is);
	//
	//  //----------------------------------------------------------------------------------------
	//  /// @markPrivate
	//  //----------------------------------------------------------------------------------------
	//  void* GetIStream() const;
	//
	//  //----------------------------------------------------------------------------------------
	//  /// @markPrivate
	//  //----------------------------------------------------------------------------------------
	//  void SetIpConnection(IpConnection* ipc);
	//
	//  //----------------------------------------------------------------------------------------
	//  /// @markPrivate
	//  //----------------------------------------------------------------------------------------
	//  IpConnection* GetIpConnection() const;
	//
	//  /// @}
};

#pragma pack (pop)
}

#endif	// C4D_FILENAME_H__
