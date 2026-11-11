/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef GEDATA_H__
#define GEDATA_H__

#include "c4d_tools.h"
#include "c4d_filename.h"
#include "c4d_basecontainer.h"

namespace cineware
{
#pragma pack (push, 8)

class GeMarker;
class BaseList2D;
class BaseLink;
class BaseTime;
class AliasTrans;
class HyperFile;

/// @addtogroup DA
/// @ingroup group_enumeration
/// @{
enum
{
	DA_NIL						= 0,				///< No value.
	DA_VOID						= 14,				///< Void pointer.
	DA_LONG						= 15,				///< ::Int32
	DA_REAL						= 19,				///< ::Float
	DA_TIME						= 22,				///< BaseTime
	DA_VECTOR					= 23,				///< Vector
	DA_MATRIX					= 25,				///< Matrix
	DA_LLONG					= 26,				///< ::Int64
	DA_BYTEARRAY			= 128,			///< @markInternal
	DA_STRING					= 130,			///< String
	DA_FILENAME				= 131,			///< Filename
	DA_CONTAINER			= 132,			///< BaseContainer
	DA_ALIASLINK			= 133,			///< BaseLink
	DA_MARKER					= 256,			///< Not used.
	DA_MISSINGPLUG		= 257,			///< Missing datatype plugin.

	DA_CUSTOMDATATYPE = 1000000,	///< Anything greater than this value are custom data types.

	DA_END
};
/// @}

/// @addtogroup DEFAULTVALUETYPE
/// @ingroup group_enumeration
/// @{
enum DEFAULTVALUETYPE
{
	DEFAULTVALUE	///< Dummy value for the default GeData constructor.
};

enum VOIDVALUETYPE
{
	VOIDVALUE	///< Dummy value for the default GeData constructor.
};

enum LLONGVALUETYPE
{
	LLONGVALUE	///< Dummy value for the default GeData constructor.
};
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//----------------------------------------------------------------------------------------
class ByteArray
{
public:
	ByteArray();
	ByteArray(const ByteArray& src);
	~ByteArray();

	Int		size;
	void*	mem;

	void Init(void* t_mem, Int t_size);
	const ByteArray& operator = (const ByteArray& b);
};

//----------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//----------------------------------------------------------------------------------------
struct DCustom
{
	CustomDataType* data;

	DCustom()
	{
		data = nullptr;
	}
};

//----------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//----------------------------------------------------------------------------------------
struct MissingPluginData
{
	Int32 pluginid;
	Int32 disklevel;
	Int		size;
	void* data;

	MissingPluginData();
	~MissingPluginData();
	Bool CopyTo(MissingPluginData* dst);
};


//----------------------------------------------------------------------------------------
/// Represents any data inside a BaseContainer.
//----------------------------------------------------------------------------------------
class GeData
{
private:
	Int32 Type;
	Int32 dummy;	// Necessary for Linux alignment of structure.

	union
	{
		Int32							 DInteger;
		Float							 DReal;

		void*							 DVoid;
		Vector*						 DVector;
		Matrix*						 DMatrix;
		Int64							 DLLong;

		String*						 DString;
		Filename*					 DFilename;

		GeMarker*					 DMarker;
		BaseTime*					 DBaseTime;

		BaseContainer*		 DContainer;
		ByteArray*				 DArray;

		BaseLink*					 DAliasLink;
		DCustom*					 DCustomData;
		MissingPluginData* DMissingPlugin;
	};

public:
	/// @name Constructors/Destructors
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	GeData();

	// #if !(defined MAXON_TARGET_LINUX) && !( (defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS)) && __LP64__ )
	//		GeData(int n);
	// #endif

	//----------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param[in] n									The data to copy.
	//----------------------------------------------------------------------------------------
	GeData(const GeData& n);

	//----------------------------------------------------------------------------------------
	/// Creates an ::Int32 data.
	/// @param[in] n									Initial ::Int32 value.
	//----------------------------------------------------------------------------------------
	GeData(Int32 n);

	//----------------------------------------------------------------------------------------
	/// Creates a ::Float32 data.
	/// @param[in] n									Initial ::Float32 value.
	//----------------------------------------------------------------------------------------
	GeData(Float32 n);

	//----------------------------------------------------------------------------------------
	/// Creates a double data.
	/// @param[in] n									Initial double value.
	//----------------------------------------------------------------------------------------
	GeData(double n);

	//----------------------------------------------------------------------------------------
	/// Creates a void pointer data.
	/// @param[in] n									Initial void pointer value.
	/// @param[in] v									Dummy parameter. Should always be @ref VOIDVALUE.
	//----------------------------------------------------------------------------------------
	GeData(void* n, VOIDVALUETYPE v);

	//----------------------------------------------------------------------------------------
	/// Creates an ::Int64 data.
	/// @param[in] n									Initial ::Int64 value.
	/// @param[in] v									Dummy parameter. Should always be @ref LLONGVALUE.
	//----------------------------------------------------------------------------------------
	GeData(Int64 n, LLONGVALUETYPE v);

	//----------------------------------------------------------------------------------------
	/// Creates a Vector data.
	/// @param[in] n									Initial Vector value.
	//----------------------------------------------------------------------------------------
	GeData(const Vector& n);

	//----------------------------------------------------------------------------------------
	/// Creates a Matrix data.
	/// @param[in] n									Initial Matrix value.
	//----------------------------------------------------------------------------------------
	GeData(const Matrix& n);

	//----------------------------------------------------------------------------------------
	/// Creates a @c Char* data.
	/// @param[in] n									Initial Char* value.
	//----------------------------------------------------------------------------------------
	GeData(const Char* n);

	//----------------------------------------------------------------------------------------
	/// Creates a String data.
	/// @param[in] n									Initial String value.
	//----------------------------------------------------------------------------------------
	GeData(const String& n);

	//----------------------------------------------------------------------------------------
	/// Creates a Filename data.
	/// @param[in] n									Initial Filename value.
	//----------------------------------------------------------------------------------------
	GeData(const Filename& n);

	//----------------------------------------------------------------------------------------
	/// Creates a GeMarker data.
	/// @param[in] n									Initial GeMarker value.
	//----------------------------------------------------------------------------------------
	GeData(const GeMarker& n);

	//----------------------------------------------------------------------------------------
	/// Creates a BaseTime data.
	/// @param[in] n									Initial BaseTime value.
	//----------------------------------------------------------------------------------------
	GeData(const BaseTime& n);

	//----------------------------------------------------------------------------------------
	/// Creates a BaseContainer data.
	/// @param[in] n									Initial BaseContainer value.
	//----------------------------------------------------------------------------------------
	GeData(const BaseContainer& n);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeData(const ByteArray& n);

	//----------------------------------------------------------------------------------------
	/// Creates a BaseLink data.
	/// @param[in] n									Initial BaseLink value.
	//----------------------------------------------------------------------------------------
	GeData(const BaseLink& n);

	//----------------------------------------------------------------------------------------
	/// Creates a data with a custom data type.
	/// @param[in] type								A custom data type ID.
	/// @param[in] customdata					Initial custom data value.
	//----------------------------------------------------------------------------------------
	GeData(Int32 type, const CustomDataType& customdata);

	//----------------------------------------------------------------------------------------
	/// Creates a custom data type with the default value for the type.\n
	/// The second parameter is simply a typing trick so that the compiler chooses this constructor:
	/// @code
	/// GeData data(DATETIME_DATA, DEFAULTVALUE);
	/// @endcode
	/// @param[in] type								A custom data type ID.
	/// @param[in] v									Dummy parameter. Should always be ::DEFAULTVALUE.
	//----------------------------------------------------------------------------------------
	GeData(Int32 type, DEFAULTVALUETYPE v);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~GeData();

	/// @}

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Assignment operator.
	/// @param[in] d									The right-hand operand data.
	/// @return												The data is assigned to the left-hand operand.
	//----------------------------------------------------------------------------------------
	const GeData& operator = (const GeData& d);

	//----------------------------------------------------------------------------------------
	/// Equality operator.
	/// @param[in] d									The data to compare with.
	/// @return												@trueIfOtherwiseFalse{the data equasl the right-hand operand data}
	//----------------------------------------------------------------------------------------
	Bool operator == (const GeData& d) const;

	//----------------------------------------------------------------------------------------
	/// Not equality operator.
	/// @param[in] d									The data to compare with.
	/// @return												@trueIfOtherwiseFalse{the data is different than the right-hand operand data}
	//----------------------------------------------------------------------------------------
	Bool operator != (const GeData& d) const;

	/// @}

	/// @name Init/Free
	///@{

	//----------------------------------------------------------------------------------------
	/// Initializes the data type.
	/// @param[in] type								The type of the data: @ref DA.
	/// @return												@trueIfOtherwiseFalse{the data was successfully initialized}
	//----------------------------------------------------------------------------------------
	Bool InitType(Int32 type);

	//----------------------------------------------------------------------------------------
	/// Frees the internal data. Sets the type of the data to @ref DA_NIL afterward.
	//----------------------------------------------------------------------------------------
	void Free();

	/// @}

	/// @name Get data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the data.
	/// @return												The type of the data: @ref DA.
	//----------------------------------------------------------------------------------------
	Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of ::Bool data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Bool GetBool() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of ::Int32 data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Int32 GetInt32() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of ::Int64 data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Int64 GetInt64() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of ::Float data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Float GetFloat() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of @c void* data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	void* GetVoid() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of Vector data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	const Vector& GetVector() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of Matrix data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	const Matrix& GetMatrix() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of String data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	const String& GetString() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of Filename data. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	const Filename& GetFilename() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of custom data. Forbidden for other data.
	/// @param[in] datatype						The type ID of the data to retrieve.
	/// @return												The value, or @formatConstant{nullptr} if the wrong datatype was specified. @theOwnsPointed{data, value}
	//----------------------------------------------------------------------------------------
	CustomDataType* GetCustomDataType(Int32 datatype) const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of BaseContainer data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	BaseContainer* GetContainer() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	const ByteArray& GetByteArray() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of BaseLink data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	BaseLink* GetBaseLink() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of BaseList2D data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	BaseList2D* GetLink() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of BaseTime data objects. Forbidden for other data.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	const BaseTime& GetTime() const;

	//----------------------------------------------------------------------------------------
	/// Copies the data object to another with an alias translator for the links.
	/// @param[in] dest								The destination data. @callerOwnsPointed{data}
	/// @param[in] aliastrans					An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	//----------------------------------------------------------------------------------------
	void CopyData(GeData* dest, AliasTrans* aliastrans) const;

	/// @}

	/// @name Set data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the ::Float value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetFloat(Float v);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Int32 value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetInt32(Int32 v);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Int64 value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetInt64(const Int64& v);

	//----------------------------------------------------------------------------------------
	/// Sets the void* value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetVoid(void* v);

	//----------------------------------------------------------------------------------------
	/// Sets the Vector value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetVector(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Sets the Matrix value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetMatrix(const Matrix& v);

	//----------------------------------------------------------------------------------------
	/// Sets the String value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetString(const String& v);

	//----------------------------------------------------------------------------------------
	/// Sets the Filename value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetFilename(const Filename& v);

	//----------------------------------------------------------------------------------------
	/// Sets the GeMarker value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetMarker(const GeMarker& v);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseTime value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetBaseTime(const BaseTime& v);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseContainer value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetContainer(const BaseContainer& v);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetByteArray(const ByteArray& v);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseLink value and change the data type accordingly.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetBaseLink(const BaseLink& v);

	//----------------------------------------------------------------------------------------
	/// Sets the custom data value and change the data type accordingly.
	/// @param[in] datatype						The type ID of the data to set.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetCustomDataType(Int32 datatype, const CustomDataType& v);

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the data was read}
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// Writes the data to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the data was written}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf) const;

	/// @}
};

#pragma pack (pop)
}

#endif	// GEDATA_H__
