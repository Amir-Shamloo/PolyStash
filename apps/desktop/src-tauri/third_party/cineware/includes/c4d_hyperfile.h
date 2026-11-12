/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef HYPERFILE_H__
#define HYPERFILE_H__

#include "c4d_basefile.h"
#include "c4d_basecontainer.h"
#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

struct CPolygon;
class BaseBitmap;
class AliasTrans;
class BaseDocument;

/// @addtogroup CINEMA4D_VERSION
/// @ingroup group_enumeration
/// @{
/// Internal version definitions for SetCinema4DVersion().
enum CINEMA4D_VERSION
{
	CINEMA4D_VERSION_NATIVE = 0,
	CINEMA4D_VERSION_R14		= 1,
	CINEMA4D_VERSION_R15		= 2,
	CINEMA4D_VERSION_R16		= 3,
	CINEMA4D_VERSION_R17		= 4,
	CINEMA4D_VERSION_R18		= 5,
	CINEMA4D_VERSION_R19		= 6,
	CINEMA4D_VERSION_R20		= 7,
	CINEMA4D_VERSION_R21		= 8
} ENUM_END_LIST(CINEMA4D_VERSION);
/// @}

//----------------------------------------------------------------------------------------
/// Status callback to set the progress when loading hyper files.
/// @see HyperFile::SetLoadStatusCallback()
/// @param[in] status							Load status progress, going from @em 0 to @em 100.
/// @param[in] userdata						User data passed from HyperFile::SetLoadStatusCallback().
//----------------------------------------------------------------------------------------
typedef void LoadStatusCallback (Int32 status, void* userdata);

//----------------------------------------------------------------------------------------
/// Status callback to set the progress when saving hyper files.
/// @see HyperFile::SetSaveStatusCallback()
/// @param[in] status							Save status progress, going from @em 0 to @em 100.
/// @param[in] userdata						User data passed from HyperFile::SetSaveStatusCallback().
//----------------------------------------------------------------------------------------
typedef void SaveStatusCallback (Int64 status, void* userdata);

//----------------------------------------------------------------------------------------
/// Hyper files are used to save plugin data that cannot be stored in a BaseContainer.\n
/// Access to the hyper file's data is like a @em FIFO (first in, first out) buffer; the values must written and read in the same order.
//----------------------------------------------------------------------------------------
class HyperFile
{
	friend class BaseLink;
	friend class GeAliasGoal;
	friend class BaseDocument;
	friend class IpCommunicationThread;
	friend class Material;

	BaseFile			file;

	FILEERROR			error;
	Bool					ascii;
	Bool					singleprecision;
	FILEOPEN			mode;
	Int64					flen;
	Int32					last_b;
	void*					data;
	BaseDocument* doc;
	Int32					fileversion;

	Int32					chunkdepth;

	inline void ReadStatus()
	{
		if (!callbackloadstatus)
			return;

		if (last_b >= flen / 500)
		{
			callbackloadstatus(Int32(100.0f * Float(file.GetPosition()) / Float(flen)), callbackUserDataAtLoad);
			last_b = 0;
		}
		last_b++;
	}

	inline void WriteStatus()
	{
		if (!callbackloadstatus)
			return;

		if (last_b == 50000)
		{
			callbacksavestatus(file.GetPosition(), callbackUserDataAtSave);
			last_b = 0;
		}
		last_b++;
	}


	Bool ReadDummy(Int64 n);

	void SetFileVersion(Int32 version);

	Bool ReadContainerV7(BaseContainer* v, Bool flush);
	Bool ReadContainerV8(BaseContainer* v, Bool flush);

	LoadStatusCallback* callbackloadstatus;
	SaveStatusCallback* callbacksavestatus;
	void*								callbackUserDataAtLoad;
	void*								callbackUserDataAtSave;
	CINEMA4D_VERSION		cinema4dSaveVersion;

protected:
	AliasTrans* aliastrans;

	Bool WriteVReal(Float v);
	Bool ReadVReal(Float* v);

	CINEMA4D_VERSION GetCinema4DVersion();
	void SetCinema4DVersion(CINEMA4D_VERSION version);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Constructs a hyper file for a @C4D document.
	/// @param[in] t_doc							The document for the hyper file.
	//----------------------------------------------------------------------------------------
	HyperFile(BaseDocument* t_doc = nullptr);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~HyperFile();

	/// @}

	/// @name Document
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the active document for the hyper file operation. Can be @formatConstant{nullptr}, for example when saving layouts.
	/// @return												The document for the hyper file operation.
	//----------------------------------------------------------------------------------------
	BaseDocument* GetDocument();

	/// @}

	/// @name Length/Position
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the length of the file in bytes.
	/// @return												The byte length of the file.
	//----------------------------------------------------------------------------------------
	Int64 GetLength();

	//----------------------------------------------------------------------------------------
	/// Gets the position of the current file pointer.
	/// @return												The current file pointer.
	//----------------------------------------------------------------------------------------
	Int64 GetPosition();

	/// @}

	/// @name Open/Close
	/// @{

	//----------------------------------------------------------------------------------------
	/// Opens the hyper file.
	/// @param[in] ident							File identification.
	/// @param[in] name								File to open.
	/// @param[in] mod								File mode.
	/// @param[in] error_dialog				File error dialog.
	/// @return												@trueIfOtherwiseFalse{the hyper file could be opened}
	//----------------------------------------------------------------------------------------
	Bool Open(Int32 ident, const Filename& name, FILEOPEN mod, FILEDIALOG error_dialog = FILEDIALOG_NONE);

	//----------------------------------------------------------------------------------------
	/// Closes the hyper file.
	/// @return												@trueIfOtherwiseFalse{the hyper file could be closed}
	//----------------------------------------------------------------------------------------
	Bool Close();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Gets the version of @C4D that wrote the file. (Only valid during reading a @C4D scene, object, material etc.)
	/// @return												The file version.
	//----------------------------------------------------------------------------------------
	Int32 GetFileVersion();

	//----------------------------------------------------------------------------------------
	/// Checks the data type in the hyper file at the current file position.
	/// @param[in] type								The type to check.
	/// @return												@trueIfOtherwiseFalse{the current type is equal to type}
	//----------------------------------------------------------------------------------------
	Bool CheckIt(HYPERFILEVALUE type);

	/// @name Value
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the value header from the file. This is only necessary in combination with loops.\n
	/// For example:
	/// @code
	/// static Bool ReadSettings(BaseObject *op, HyperFile *hf, Int32 level)
	/// {
	///   Int32 id, level, v;
	///   String test;
	///   HYPERFILEVALUE h;
	///   while (hf->ReadValueHeader(&h) && h==HYPERFILEVALUE_START)
	///   {
	///     if (!hf->ReadChunkStart(&id, &level))
	///       return false;
	///
	///     if (id==2)
	///     {
	///       hf->ReadInt32(&v);
	///       hf->ReadInt32(&v);
	///     }
	///     else if (id==3)
	///     {
	///       hf->ReadInt32(&v);
	///       hf->ReadInt32(&v);
	///       hf->ReadInt32(&v);
	///     }
	///     hf->SkipToEndChunk(); // Never forget this at the end
	///     if (id==0)
	///       break; // Chunk ID 0 is end chunk
	///   }
	///
	///   hf->ReadString(test);
	///
	///   return true;
	/// }
	/// @endcode
	/// @param[in] h									A pointer to a HYPERFILEVALUE.
	/// @return												@trueIfOtherwiseFalse{the header was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadValueHeader(HYPERFILEVALUE* h);

	//----------------------------------------------------------------------------------------
	/// Skips a given type of value.
	/// @param[in] h									The hyper file value to skip.
	/// @return												@trueIfOtherwiseFalse{the value was of the given header type and it was skipped}
	//----------------------------------------------------------------------------------------
	Bool SkipValue(HYPERFILEVALUE h);

	/// @}

	/// @name Start/End Chunk
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads a chunk identification from the hyper file.
	/// @param[out] id								Assigned the chunk ID.
	/// @param[out] level							Assigned the chunk level.
	/// @return												@trueIfOtherwiseFalse{the chunk start was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadChunkStart(Int32* id, Int32* level);

	//----------------------------------------------------------------------------------------
	/// Reads a chunk end marker from the hyper file.
	/// @return												@trueIfOtherwiseFalse{the chunk end was read successfully}
	//----------------------------------------------------------------------------------------
	Bool ReadChunkEnd();

	//----------------------------------------------------------------------------------------
	/// Writes a chunk marker into the file indicating the beginning of a new chunk of data.
	/// @param[in] id									The ID for the chunk.
	/// @param[in] level							Increase this level to save/read new values.
	/// @return												@trueIfOtherwiseFalse{the chunk identification was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteChunkStart(Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes a chunk ending marker into the hyper file.
	/// @return												@trueIfOtherwiseFalse{the chunk end was written successfully}
	//----------------------------------------------------------------------------------------
	Bool WriteChunkEnd();

	//----------------------------------------------------------------------------------------
	/// Moves the file pointer to the end of the chunk.
	/// @warning	Should always be called after having finished reading values from the current chunk.
	/// @return												@trueIfOtherwiseFalse{the end of the chunk was found and sought}
	//----------------------------------------------------------------------------------------
	Bool SkipToEndChunk();

	/// @}

	/// @name Read
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads a ::Char value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadChar(Char* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UChar value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadUChar(UChar* v);

	//----------------------------------------------------------------------------------------
	/// Reads an ::Int16 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadInt16(Int16* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt16 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadUInt16(UInt16* v);

	//----------------------------------------------------------------------------------------
	/// Reads an ::Int32 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadInt32(Int32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt32 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadUInt32(UInt32* v);

	//----------------------------------------------------------------------------------------
	/// Reads an ::Int64 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadInt64(Int64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::UInt64 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadUInt64(UInt64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Float value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadFloat(Float* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Float32 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadFloat32(Float32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Float64 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadFloat64(Float64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Bool value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadBool(Bool* v);

	//----------------------------------------------------------------------------------------
	/// Reads a BaseTime value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadTime(BaseTime* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Vector value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadVector(Vector* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Vector32 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadVector32(Vector32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Vector64 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadVector64(Vector64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a ::Matrix value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadMatrix(Matrix* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Matrix32 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadMatrix32(Matrix32* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Matrix64 value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadMatrix64(Matrix64* v);

	//----------------------------------------------------------------------------------------
	/// Reads a String value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadString(String* v);

	//----------------------------------------------------------------------------------------
	/// Reads a Filename value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadFilename(Filename* v);

	//----------------------------------------------------------------------------------------
	/// Reads a BaseBitmap value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadImage(BaseBitmap* v);

	//----------------------------------------------------------------------------------------
	/// Reads a GeData value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadGeData(GeData& v);

	//----------------------------------------------------------------------------------------
	/// Reads a BaseContainer value from the hyper file.
	/// @param[out] v									Assigned the value read from the hyper file.
	/// @param[in] flush							Specify if the container should be cleared before reading the new values.\n
	///																Using @formatConstant{false} will merge the container with the one read from the file.
	/// @return												Success of reading the value.
	//----------------------------------------------------------------------------------------
	Bool ReadContainer(BaseContainer* v, Bool flush);


	//----------------------------------------------------------------------------------------
	/// Reads an array of the specified @formatParam{datatype} from the hyper file.
	/// @param[out] data							Array to be filled. @callerOwnsPointed{array}
	/// @param[in] datatype						Data type.
	/// @param[in] structure_increment	Size of the data type (<i>sizeof()</i>).
	/// @param[in] count							Number of elements in @formatParam{data}.
	/// @return												Success of reading the array.
	//----------------------------------------------------------------------------------------
	Bool ReadArray(void* data, HYPERFILEARRAY datatype, Int32 structure_increment, Int32 count);


	//----------------------------------------------------------------------------------------
	/// Reads a block of memory from the hyper file.\n
	/// @note	Only use this when really needed.Be aware that the byte sequences will not be platform independent.
	/// Example:
	/// @code
	/// Int32 size;
	///
	/// void* data = nullptr;
	/// Bool ok = ReadMemory(&data, &size);
	/// DeleteMem(data);
	/// @endcode
	/// @param[out] data							Pointer to the start of the memory block to read.
	/// @param[out] size							Assigned the byte size of the memory data read.
	/// @return												Success of reading the memory block.
	//----------------------------------------------------------------------------------------
	Bool ReadMemory(void** data, Int* size);

	//----------------------------------------------------------------------------------------
	/// @markPrivate @markInternal
	//----------------------------------------------------------------------------------------
	Bool ReadUnknownChunk(void** mem, Int* size, Bool dontreadendbyte);

	//----------------------------------------------------------------------------------------
	/// @markPrivate @markInternal
	//----------------------------------------------------------------------------------------
	Bool ReadFloat32ArrayEx(Float32** data, Int32* count);

	//----------------------------------------------------------------------------------------
	/// @markPrivate @markInternal
	//----------------------------------------------------------------------------------------
	Bool ReadVectorArrayEx(Vector** data, Int32* count);

	//----------------------------------------------------------------------------------------
	/// @markPrivate @markInternal
	//----------------------------------------------------------------------------------------
	Bool ReadPolygonArrayEx(CPolygon** data, Int32* count);

	//----------------------------------------------------------------------------------------
	/// @markPrivate @markInternal
	//----------------------------------------------------------------------------------------
	Bool ReadUInt16ArrayEx(UInt16** data, Int32* count);

	/// @}

	/// @name Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Writes a ::Char value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteChar(Char v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UChar value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteUChar(UChar v);

	//----------------------------------------------------------------------------------------
	/// Writes an ::Int16 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteInt16(Int16 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::UInt16 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteUInt16(UInt16 v);

	//----------------------------------------------------------------------------------------
	/// Writes an ::Int32 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteInt32(Int32 v);

	//----------------------------------------------------------------------------------------
	/// Writes an ::UInt32 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteUInt32(UInt32 v);

	//----------------------------------------------------------------------------------------
	/// Writes an ::Int64 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteInt64(Int64 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Float32 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteFloat32(Float32 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Float value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteFloat(Float v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Float64 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteFloat64(Float64 v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Bool value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteBool(Bool v);

	//----------------------------------------------------------------------------------------
	/// Writes a BaseTime value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteTime(const BaseTime& v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Vector value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteVector(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Vector32 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteVector32(const Vector32& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Vector64 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteVector64(const Vector64& v);

	//----------------------------------------------------------------------------------------
	/// Writes a ::Matrix value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteMatrix(const Matrix& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Matrix32 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteMatrix32(const Matrix32& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Matrix64 value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteMatrix64(const Matrix64& v);

	//----------------------------------------------------------------------------------------
	/// Writes a String value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteString(const String& v);

	//----------------------------------------------------------------------------------------
	/// Writes a Filename value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteFilename(const Filename& v);

	//----------------------------------------------------------------------------------------
	/// Writes a GeData value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteGeData(const GeData& v);

	//----------------------------------------------------------------------------------------
	/// Writes a BaseContainer value to the hyper file.
	/// @param[in] v									The value to write to the hyper file.
	/// @return												Success of writing the value.
	//----------------------------------------------------------------------------------------
	Bool WriteContainer(const BaseContainer& v);

	//----------------------------------------------------------------------------------------
	/// Writes a bitmap image to the hyper file.
	/// @param[in] bmp								Bitmap to write to the hyperfile. @callerOwnsPointed{bitmap}
	/// @param[in] type								Image type.
	/// @param[in] data								Container with additional format settings, or @formatConstant{nullptr}
	/// @param[in] savebits						Save bits.
	/// @return												Success of writing the bitmap.
	//----------------------------------------------------------------------------------------
	Bool WriteImage(BaseBitmap* bmp, Int32 type, BaseContainer* data, SAVEBIT savebits = SAVEBIT_ALPHA);

	//----------------------------------------------------------------------------------------
	/// Writes an array of the specified type @formatParam{datatype} to the hyper file.
	/// @param[in] data								Pointer to the array. @callerOwnsPointed{array}
	/// @param[in] datatype						Array data type.
	/// @param[in] structure_increment	Size of the data type (<i>sizeof()</i>).
	/// @param[in] count							Number of elements in @formatParam{data}.
	/// @return												Success of writing the array.
	//----------------------------------------------------------------------------------------
	Bool WriteArray(const void* data, HYPERFILEARRAY datatype, Int32 structure_increment, Int32 count);

	//----------------------------------------------------------------------------------------
	/// Writes a block of memory to the hyper file.
	/// @note	Only use this when really needed.Be aware that the byte sequences will not be platform independent.
	/// @param[in] data								Start of the memory block to write to the hyper file.
	/// @param[in] count							Size in bytes of the memory data to write.
	/// @return												Success of writing the memory block.
	//----------------------------------------------------------------------------------------
	Bool WriteMemory(const void* data, Int count);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool WriteUnknownChunk(const void* mem, Int size, Bool endchunkNOTincluded);

	/// @}

	/// @name Error
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the error from the last hyper file operation.
	/// @return												The error.
	//----------------------------------------------------------------------------------------
	FILEERROR GetError();

	//----------------------------------------------------------------------------------------
	/// Sets the error value for the hyper file.
	/// @param[in] err								The error.
	//----------------------------------------------------------------------------------------
	void SetError(FILEERROR err);

	/// @}

	//virtual Bool ReadParticleArrayEx (Particle **data, Int32 *count);

	/// @name LoadStatus Callback
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the status for the loading of hyper files.
	/// @see SetSaveStatusCallback()
	/// Here is an example of usage to get the load progress:
	/// @code
	/// void loadProgressCb(Int32 status, void *udata)
	/// {
	///   Int32 test = status;							//For the read percentage of the file (0 - 100)
	///   printf("%d",(int)status);
	///   printf("%%");
	///   printf(".");
	/// }
	///
	/// // Set the callback for the load progress
	/// newC4Dfile.SetLoadStatusCallback(loadProgressCb, nullptr);
	/// @endcode
	/// @param[in] cb									Load status callback.
	/// @param[in] ud									User data.
	//----------------------------------------------------------------------------------------
	void SetLoadStatusCallback(LoadStatusCallback cb, void* ud);

	//----------------------------------------------------------------------------------------
	/// Sets the status for the saving of hyper files.
	/// @see SetLoadStatusCallback()
	/// @param[in] cb									Save status callback.
	/// @param[in] ud									User data.
	//----------------------------------------------------------------------------------------
	void SetSaveStatusCallback(SaveStatusCallback cb, void* ud);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void* GetPrivateData();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetPrivateData(void* t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Before8();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Before12();

	/// @}
};

#pragma pack (pop)
}

#endif	// HYPERFILE_H__
