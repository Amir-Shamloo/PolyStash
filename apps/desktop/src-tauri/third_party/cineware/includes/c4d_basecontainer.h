/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASECONTAINER_H__
#define C4D_BASECONTAINER_H__

#include "c4d_system.h"
#include "private_symbols.h"
#include "ge_math.h"
#include "c4d_filename.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseContainer;
class GeData;
struct GeDataList;
class BaseTime;
class BaseList2D;
class BaseObject;
class AliasTrans;
class DescID;
struct CustomDataType;

//----------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//----------------------------------------------------------------------------------------
class GeContainer
{
	//friend void* XBrowseContainer(const BaseContainer* bc, Int32* id, GeData** data, void* handle);
	friend class BrowseContainer;

protected:
	GeDataList* First;
	GeDataList* Last;
	Int32				Id;
	UInt16			dirty_sum;
	UInt16			dirty_last;
	void*				future_enhancements;

	GeContainer();
	explicit GeContainer(Int32 id);
	GeContainer(const BaseContainer& n);

public:
	~GeContainer();

	void SDKInit(Int32 id);

	void ClearDirty();
	void SetDirty();
	void RestoreDirty(UInt32 dirty);
	Bool IsDirty();
	UInt32 GetDirty();

	BaseContainer* GetClone(COPYFLAGS flags, AliasTrans* aliastrans) const;
	void CopyTo(BaseContainer* dest, COPYFLAGS flags, AliasTrans* aliastrans) const;

	void FlushAll();

	Int32 GetId() const;
	void SetId(Int32 c_id);

	GeData* InsData(Int32 id, const GeData& n);
	GeData* InsDataAfter(Int32 id, const GeData& n, GeData* last);
	BaseContainer* InsContainer(Int32 id, const BaseContainer& gc);
	GeData* SetData(Int32 id, const GeData& n);
	Bool RemoveData(Int32 id);
	Bool RemoveIndex(Int32 i);

	Int32	FindIndex(Int32 id) const;

	Int32	GetIndexId(Int32 index) const;
	GeData*	GetIndexData(Int32 index) const;

	const GeData& GetData(Int32 id) const;

protected:
	void _SDKInit(Int32 id);
	void _ClearDirty();
	void _SetDirty();
	Bool _IsDirty();
	UInt32 _GetDirty();
	BaseContainer* _GetClone(COPYFLAGS flags, AliasTrans* aliastrans) const;
	void _CopyTo(BaseContainer* dest, COPYFLAGS flags, AliasTrans* aliastrans) const;
	const GeContainer& operator = (const GeContainer& n);
	void _FlushAll();
	Int32 _GetId() const;
	void _SetId(Int32 c_id);
	GeData* _InsData(Int32 id, const GeData& n);
	GeData* _InsDataAfter(Int32 id, const GeData& n, GeData* last);
	BaseContainer* _InsContainer(Int32 id, const BaseContainer& gc);
	GeData* _SetData(Int32 id, const GeData& n);
	Bool _RemoveData(Int32 id);
	Bool _RemoveIndex(Int32 i);
	Int32 _FindIndex(Int32 id) const;
	Int32 _GetIndexId(Int32 index) const;
	GeData* _GetIndexData(Int32 index) const;
	const GeData& _GetData(Int32 id) const;
};

//----------------------------------------------------------------------------------------
/// A container is a collection of individual values. Each value has its own ID and type. Container can also carry any number of child containers.\n
/// @em 90% of @C4D's internal values are stored in containers and all messages are working with container, so this class is an essential part of the Cineware SDK.\n
/// Containers can store any GeData type, including custom data types. Use GetCustomDataType() to access these values in a safe manner.
///
/// @warning	Keep in mind that there is no guarantee for a value to be in the container. Use default values whenever possible when accessing container's ID data.\n
///						Please use the typed access methods (for example GetBool()) whenever possible, instead of the low-level GetData().\n
///						Once a container value has been set using one type you must neither try to access it using another type, nor overwrite it with a value of another type! Using the wrong access will not crash, but it is illegal.
///
/// <b>Example</b>: Set and get data.
/// @code
/// if (op->SetParameter(12345, GeData(Vector(1.0,2.0,3.0))))
/// {
///   if (op->GetParameter(12345, data))
///   {
///     Vector vec = data.GetVector();
///     printf("   - VTEST: %f %f %f\n", vec.x, vec.y, vec.z);
///
///     data = 5.0;
///     if (op->SetParameter(DescID(DescLevel(12345, DTYPE_VECTOR,0), DescLevel(VECTOR_Y)), data))
///     {
///       if (op->GetParameter(12345, data))
///       {
///         vec = data.GetVector();
///         printf("   - VTEST: %f %f %f\n", vec.x, vec.y, vec.z);
///       }
///
///       if (op->GetParameter(DescID(DescLevel(12345, DTYPE_VECTOR,0), DescLevel(VECTOR_Z)), data))
///       printf("   - VTEST: %f", data.GetReal());
///     }
///   }
/// }
/// @endcode
//----------------------------------------------------------------------------------------
class BaseContainer : public GeContainer
{
public:
	/// @name Constructors
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor. Creates an empty container with ID @em 0.
	//----------------------------------------------------------------------------------------
	BaseContainer() { }

	//----------------------------------------------------------------------------------------
	/// Creates a container with a specific @formatParam{id}.
	/// @param[in] id									The container ID.
	//----------------------------------------------------------------------------------------
	BaseContainer(Int32 id) : GeContainer(id) { }

	//----------------------------------------------------------------------------------------
	/// Copy constructor. Creates a container with all the IDs and values from the source one.
	/// @param[in] n									The source container.
	//----------------------------------------------------------------------------------------
	BaseContainer(const BaseContainer& n) : GeContainer(n) { }

	/// @}

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Assignment operator. Copies all values from the source container.
	/// @param[in] n									The source container.
	/// @return												The assigned source container to the left-operand container.
	//----------------------------------------------------------------------------------------
	const BaseContainer& operator = (const BaseContainer& n);

	//----------------------------------------------------------------------------------------
	/// Equal operator.
	/// @param[in] d									The container to compare against.
	/// @return												@trueIfOtherwiseFalse{the compared containers have the same IDs, have the same values and all values are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (const BaseContainer& d) const;

	//----------------------------------------------------------------------------------------
	/// Not equal operator.
	/// @param[in] d									The container to compare against.
	/// @return												@trueIfOtherwiseFalse{the compared containers have different IDs, have different values or values are different}
	//----------------------------------------------------------------------------------------
	Bool operator != (const BaseContainer& d) const;

	/// @}

	/// @name Get Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the ::Bool value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Bool GetBool(Int32 id, Bool preset = false) const;

	//----------------------------------------------------------------------------------------
	/// Gets the ::Int32 value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Int32 GetInt32(Int32 id, Int32 preset = 0) const;

	//----------------------------------------------------------------------------------------
	/// Gets the ::Int64 value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Int64 GetInt64(Int32 id, Int64 preset = 0) const;

	//----------------------------------------------------------------------------------------
	/// Gets the @c void* value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value, or @formatParam{preset} if it does not exist.
	//----------------------------------------------------------------------------------------
	void* GetVoid(Int32 id, void* preset = nullptr) const;

	//----------------------------------------------------------------------------------------
	/// Gets the ::Float value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Float GetFloat(Int32 id, Float preset = 0.0) const;

	//----------------------------------------------------------------------------------------
	/// Gets the ::Vector value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Vector GetVector(Int32 id, const Vector& preset = Vector()) const;

	//----------------------------------------------------------------------------------------
	/// Gets the ::Matrix value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Matrix GetMatrix(Int32 id, const Matrix& preset = Matrix()) const;

	//----------------------------------------------------------------------------------------
	/// Gets the String value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	String GetString(Int32 id, const String& preset = String()) const;

	//----------------------------------------------------------------------------------------
	/// Gets the BaseTime value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	BaseTime GetTime(const Int32 id);

	//----------------------------------------------------------------------------------------
	/// Gets the BaseList2D value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	BaseList2D* GetLink(const Int32 id);

	//----------------------------------------------------------------------------------------
	/// Gets the Filename value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] preset							Returned if the value is not available.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Filename GetFilename(Int32 id, const Filename& preset = Filename()) const;

	//----------------------------------------------------------------------------------------
	/// Gets the custom data type value with the specified @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested custom data type value.
	/// @param[in] datatype						The custom data type's ID.
	/// @return												The custom data type value, or @formatConstant{nullptr}. @theOwnsPointed{container, custom data type value}
	//----------------------------------------------------------------------------------------
	const CustomDataType* GetCustomDataType(Int32 id, Int32 datatype) const;

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the sub-container with the specified @formatParam{id}, or an empty container if it does not exist.
	/// @param[in] id									The ID of the requested sub-container.
	/// @return												The sub-container.
	//----------------------------------------------------------------------------------------
	BaseContainer GetContainer(Int32 id) const;

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the sub-container with the specified @formatParam{id}. Changes to the pointed container are reflected in the stored sub-container.
	/// @param[in] id									The ID of the requested sub-container ID.
	/// @return												A pointer to the sub-container, or @formatConstant{nullptr} if it doesn't exist.
	//----------------------------------------------------------------------------------------
	BaseContainer* GetContainerInstance(Int32 id) const;

	//----------------------------------------------------------------------------------------
	/// Gets a linked object for the link stored at the specified @formatParam{id}.
	/// @param[in] id									The ID of the requested link.
	/// @return												The linked object, or @formatConstant{nullptr} if the link is broken.
	//----------------------------------------------------------------------------------------
	BaseObject* GetObjectLink(Int32 id) const;

	//----------------------------------------------------------------------------------------
	/// Gets the GeData with the specified parameter @formatParam{id} in the container.
	/// @param[in] id									The ID of the requested value.
	/// @param[in] data								Assigned the retrieved data.
	/// @return												@trueIfOtherwiseFalse{the data was retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetParameter(const DescID& id, GeData& data);

	/// @}

	/// @name Set Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Stores the values from @formatParam{src} in the container, overwriting any elements with the same IDs and keeping the rest.
	/// @param[in] src								The source container to merge.
	//----------------------------------------------------------------------------------------
	void MergeContainer(const BaseContainer& src);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Bool value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] b									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetBool(Int32 id, Bool b);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Int32 value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] l									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetInt32(Int32 id, Int32 l);

	//----------------------------------------------------------------------------------------
	/// Sets the ::UInt32 value at the specified @formatParam{id}, or inserts it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] l									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetUInt32(Int32 id, UInt32 l);

	//----------------------------------------------------------------------------------------
	/// Set the ::Int64 value with the specified @formatParam{id}, or insert it if it does not exist.
	///
	/// @param[in] id									The ID of the value to set.
	/// @param[in] l									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetInt64(Int32 id, Int64 l);

	//----------------------------------------------------------------------------------------
	/// Sets the ::UInt64 value at the specified @formatParam{id}, or inserts it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] l									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetUInt64(Int32 id, UInt64 l);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Float value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] r									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetFloat(Int32 id, Float r);

	//----------------------------------------------------------------------------------------
	/// Sets the @c void * value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] v									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetVoid(Int32 id, void* v);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Vector value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] v									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetVector(Int32 id, const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Sets the ::Matrix value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] m									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetMatrix(Int32 id, const Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Sets the String value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] str								The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetString(Int32 id, const String& str);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseTime value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] s									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetTime(const Int32 id, const BaseTime& s);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseList2D link value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] bl									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetLink(const Int32 id, BaseList2D* bl);

	//----------------------------------------------------------------------------------------
	/// Sets the Filename value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] s									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetFilename(const Int32 id, const Filename& s);

	//----------------------------------------------------------------------------------------
	/// Sets the BaseContainer value with the specified @formatParam{id}, or insert it if it does not exist.
	/// @param[in] id									The ID of the value to set.
	/// @param[in] bc									The new or inserted value.
	//----------------------------------------------------------------------------------------
	void SetContainer(const Int32 id, const BaseContainer& bc);

	//----------------------------------------------------------------------------------------
	/// Sets the GeData for the specified parameter @formatParam{id} in the container.
	/// @param[in] id									The ID of the parameter to set.
	/// @param[in] t_data							The data value to set or insert.
	//----------------------------------------------------------------------------------------
	Bool SetParameter(const DescID& id, const GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool RemoveParameter(const DescID &id);

	/// @}
};

//----------------------------------------------------------------------------------------
/// Compares two container.
/// @param[in] a									First container.
/// @param[in] b									Second container.
/// @return												@trueIfOtherwiseFalse{the containers are identical}
//----------------------------------------------------------------------------------------
Bool IdenticalContainer(const BaseContainer& a, const BaseContainer& b);

#pragma pack (pop)
}

#endif	// C4D_BASECONTAINER_H__
