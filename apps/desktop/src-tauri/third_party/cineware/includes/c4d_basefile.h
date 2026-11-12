/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEFILE_H__
#define C4D_BASEFILE_H__

#include "ge_math.h"
#include "private_symbols.h"

namespace cineware
{
#pragma pack (push, 8)

class MemoryFileStruct;
class IpConnection;
class Filename;
class String;
struct Vector32;
struct Vector64;
struct Matrix32;
struct Matrix64;

/// @name Swap/Intel/Motorola Conversions
/// @{

//----------------------------------------------------------------------------------------
/// Swaps the bytes of the ::Int values. (Converting between big endian and little endian.)
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void lSwap(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Swaps the low and high bytes of the ::UInt values. (Converting between big endian and little endian.)
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void wSwap(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::Int value to Intel format. If already in Intel (i.e on a PC) the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void lIntel(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::UInt value to Intel format. If already in Intel (i.e on a PC) the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void wIntel(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::Int value to Motorola format. If already in Motorola (i.e on a Mac) the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void lMotor(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::UInt value to Motorola format, if already in Motorola (i.e on a Mac) then the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void wMotor(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Swaps the bytes of the ::Int64 values. (Converting between big endian and little endian.)
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void llSwap(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::Int64 value to Intel format. If already in Intel (i.e on a PC) the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void llIntel(void* adr, Int cnt = 1);

//----------------------------------------------------------------------------------------
/// Converts the ::Int64 value to Motorola format. If already in Motorola (i.e on a Mac) the values remain unchanged.
/// @param[in,out] adr						Pointer to a memory address. @callerOwnsPointed{memory buffer or variable at @formatParam{adr}}
/// @param[in] cnt								Number of elements in the memory buffer at @formatParam{adr} (@em 1 for a single element or variable).
//----------------------------------------------------------------------------------------
void llMotor(void* adr, Int cnt = 1);

/// @}

//----------------------------------------------------------------------------------------
/// File operations class. Represents a disk file for reading and writing.
//----------------------------------------------------------------------------------------
class BaseFile
{
public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{file}
	/// @since 18.002
	/// @return												@allocReturn{file}
	//----------------------------------------------------------------------------------------
	static BaseFile* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{files}
	/// @since 18.002
	/// @param[in,out] fl							@theToDestruct{file}
	//----------------------------------------------------------------------------------------
	static void Free(BaseFile*& fl);

	/// @}

	/// @name Open/Close
	/// @{

	//----------------------------------------------------------------------------------------
	/// Opens a file.
	/// @note If a plugin has to be cross platform from a PC to a Mac (generally advised) then it must correctly fill in the @formatParam{type} and @formatParam{creator} parameters which are needed on a Mac.
	/// @param[in] name								The name of the file to open.
	/// @param[in] mode								The file access mode: @enumerateEnum{FILEOPEN}
	/// @param[in] error_dialog				Set the type of error reporting that should happen while opening the file: @enumerateEnum{FILEDIALOG}
	/// @param[in] order							The type of byte order in the file: @enumerateEnum{BYTEORDER}
	/// @param[in] type								The type of file, only applies to Mac: @ref MACTYPE_CINEMA.
	/// @param[in] creator						The application that wrote the file, for example 'ttxt' (SimpleText) or 'C4D1' for @C4D, Only relevant to Mac: @ref MACCREATOR_CINEMA.
	/// @return												@trueIfOtherwiseFalse{the file was opened without any problem}
	//----------------------------------------------------------------------------------------
	Bool Open(const Filename& name, FILEOPEN mode = FILEOPEN_READ, FILEDIALOG error_dialog = FILEDIALOG_IGNOREOPEN, BYTEORDER order = BYTEORDER_MOTOROLA, Int32 type = MACTYPE_CINEMA, Int32 creator = MACCREATOR_CINEMA);

	//----------------------------------------------------------------------------------------
	/// Closes the file.\n
	/// Automatically called when a BaseFile object is destroyed.
	/// @return												@trueIfOtherwiseFalse{the file was successfully closed}
	//----------------------------------------------------------------------------------------
	Bool Close();

	/// @}

	/// @name Read/Write Bytes
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads a block of bytes from the file.
	/// @warning Only use this when really needed. Be aware that the byte sequences will not be platform independent.
	/// @param[out] data							The memory buffer to fill with the bytes from the file. @callerOwnsPointed{buffer}
	/// @param[in] len								The number of bytes to read from the file.
	/// @param[in] just_try_it				If @formatConstant{true} and the data retrieved from the file is less than requested, no error will be generated.
	/// @return												The number of bytes read.
	//----------------------------------------------------------------------------------------
	Int ReadBytes(void* data, Int len, Bool just_try_it = false);

	//----------------------------------------------------------------------------------------
	/// Writes a block of bytes to the file.
	/// @warning Only use this when really needed. Be aware that the byte sequences will not be platform independent.
	/// @param[in] data								The memory buffer with the data to write to the file. @callerOwnsPointed{buffer}
	/// @param[in] len								The number of bytes to write to the file.
	/// @return												@trueIfOtherwiseFalse{the bytes were successfully written to the file}
	//----------------------------------------------------------------------------------------
	Bool WriteBytes(const void* data, Int len);

	/// @}

	/// @name File Pointer/Information/Error
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the file pointer within the file.
	/// @param[in] pos								The position within the file to place the file pointer.
	/// @param[in] mode								Sets how the position relates to the file.
	/// @return												@trueIfOtherwiseFalse{the file pointer position was successfully changed}
	//----------------------------------------------------------------------------------------
	Bool Seek(Int64 pos, FILESEEK mode = FILESEEK_RELATIVE);

	//----------------------------------------------------------------------------------------
	/// Gets the file pointer.
	/// @return												The position of the file pointer.
	//----------------------------------------------------------------------------------------
	Int64 GetPosition();

	//----------------------------------------------------------------------------------------
	/// Gets the length of the file in bytes.
	/// @return												The byte length of the file.
	//----------------------------------------------------------------------------------------
	Int64 GetLength();

	//----------------------------------------------------------------------------------------
	/// Gets the last error during read or write.
	/// @return												The last error: @enumerateEnum{FILEERROR}
	//----------------------------------------------------------------------------------------
	FILEERROR GetError() const;

	//----------------------------------------------------------------------------------------
	/// Sets the error manually.
	/// @param[in] error							The error: @enumerateEnum{FILEERROR}
	//----------------------------------------------------------------------------------------
	void SetError(FILEERROR error);

	//----------------------------------------------------------------------------------------
	/// Sets the byte order of the data in the file.
	/// @param[in] order							The byte order: @enumerateEnum{BYTEORDER}
	//----------------------------------------------------------------------------------------
	void SetOrder(BYTEORDER order);

	/// @}

	/// @name Read
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads a ::Char from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Char}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadChar(Char* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UChar from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::UChar}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadUChar(UChar* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Int16 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Int16}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadInt16(Int16* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt16 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::UInt16}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadUInt16(UInt16* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Int32 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Int32}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadInt32(Int32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt32 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::UInt32}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadUInt32(UInt32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Float32 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Float32}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadFloat32(Float32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Float64 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Float64}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadFloat64(Float64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Int64 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Int64}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadInt64(Int64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt64 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::UInt64}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadUInt64(UInt64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Filename from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{Filename}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadFilename(Filename* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Bool from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{::Bool}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadBool(Bool* v);

	//----------------------------------------------------------------------------------------
	/// Reads a String from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{String}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadString(String* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Vector32 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{Vector32}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadVector32(Vector32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Vector64 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{Vector64}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadVector64(Vector64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Matrix32 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{Matrix32}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadMatrix32(Matrix32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Matrix64 from the file.
	/// @param[out] v									Assigned the read value. @callerOwnsPointed{Matrix64}
	/// @return												@trueIfOtherwiseFalse{the value was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadMatrix64(Matrix64* v);

	/// @}

	/// @name Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Writes a ::Char to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteChar(const Char v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UChar to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteUChar(const UChar v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Int16 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteInt16(const Int16 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UInt16 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteUInt16(const UInt16 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Int32 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteInt32(const Int32 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UInt32 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteUInt32(const UInt32 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Float32 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteFloat32(const Float32 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Float64 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteFloat64(const Float64 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Int64 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteInt64(const Int64 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UInt64 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteUInt64(const UInt64 v);

	//----------------------------------------------------------------------------------------
	/// Writes a Filename to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteFilename(const Filename& v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Bool to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteBool(Bool v);

	//----------------------------------------------------------------------------------------
	/// Writes a String to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteString(const String& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Vector32 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteVector32(const Vector32& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Vector64 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteVector64(const Vector64& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Matrix32 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteMatrix32(const Matrix32& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Matrix64 to the file.
	/// @param[in] v									The value to write to the file.
	/// @return												@trueIfOtherwiseFalse{the value was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteMatrix64(const Matrix64& v);

	/// @}

	//----------------------------------------------------------------------------------------
	/// @markPrivate Default constructor.
	//----------------------------------------------------------------------------------------
	BaseFile();

	//----------------------------------------------------------------------------------------
	/// @markPrivate Default destructor.
	//----------------------------------------------------------------------------------------
	~BaseFile();

private:
	void*							_file;

	Bool							_memop;

	FILEERROR					_error;
	BYTEORDER					_order;
	Int32							_memblock;
	Int								_memsize;

	FILEOPEN					_streammode;
	Int64							_length, _position;

	UChar*						_buffer;
	Int								_buffer_size;
	Int64							_buffer_pos;
	Int								_b_start, _b_end;

	MemoryFileStruct*	_mws;

	Int AdjustBuffer(Int rsize);
	void* Gefopen(const Filename& fn, FILEOPEN mode);
};

#pragma pack (pop)
}

#endif	// C4D_BASEFILE_H__
