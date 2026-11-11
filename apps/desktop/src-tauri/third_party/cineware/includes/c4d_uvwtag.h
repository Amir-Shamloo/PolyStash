/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_UVWTAG_H__
#define C4D_UVWTAG_H__

#include "c4d_basetag.h"
#include "c4d_uvwstruct.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a UV.
/// @note	No vectors are used to save memory
//----------------------------------------------------------------------------------------
struct SmallUVW
{
	Float32 u,	///< U coordinate.
					v;	///< V coordinate.

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	SmallUVW() { }

	//----------------------------------------------------------------------------------------
	/// Constructor that initializes SmallUVW::u and SmallUVW::v coordinates.
	/// @param[in] t_u								U coordinate.
	/// @param[in] t_v								V coordinate.
	//----------------------------------------------------------------------------------------
	SmallUVW(Float32 t_u, Float32 t_v)
	{
		u = t_u;
		v = t_v;
	};
};

//----------------------------------------------------------------------------------------
/// Represents a polygon UVs.
//----------------------------------------------------------------------------------------
struct SmallUVWStruct
{
	SmallUVW a,	///< First point UV coordinates.
					 b,	///< Second point UV coordinates.
					 c,	///< Third point UV coordinates.
					 d;	///< Fourth point UV coordinates.
};

//----------------------------------------------------------------------------------------
/// UVW tag. Holds UVW data.
//----------------------------------------------------------------------------------------
class UVWTag : public BaseTag
{
	INSTANCEOF(UVWTag, BaseTag)

private:
	Bool						locked;

	SmallUVWStruct*	s_data;
	Int32						s_count;

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	UVWTag();

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	virtual ~UVWTag();

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the size of the UVW data array.
	/// @return												The size of the uvw array.
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// Resizes the UVW data array.
	/// @param[in] newcnt							The new UVW data size.
	/// @return												@trueIfOtherwiseFalse{the UVW array was resized successfully}
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newcnt);

	//----------------------------------------------------------------------------------------
	/// Gets a handle to the read-only UVW data.
	/// @return												The handle to the read-only UVW data. @theOwnsPointed{tag,UVW data}
	//----------------------------------------------------------------------------------------
	ConstUVWHandle GetDataAddressR();

	//----------------------------------------------------------------------------------------
	/// Gets a handle to the writable UVW data.
	/// @return												The handle to the writable UVW data. @theOwnsPointed{tag,UVW data}
	//----------------------------------------------------------------------------------------
	UVWHandle GetDataAddressW();

	//----------------------------------------------------------------------------------------
	/// Gets the UVW coordinates for a polygon.
	/// <b>Example:</b>
	/// @code
	/// ConstUVWHandle dataptr = uvwtag->GetDataAddressR();
	/// for (Int32 i=0; i<cnt; i++)
	/// {
	///   UVWStruct res;
	///   UVWTag::Get(dataptr, i, res);
	///
	///   /// // Do something with UVW res
	/// }
	/// @endcode
	/// @param[in] dataPtr						The handle to the UVW data. @callerOwnsPointed{UVW data}
	/// @param[in] i									The index of the polygon to get the coordinates for.
	/// @param[out] res								Assigned the UVW coordinates for the polygon.
	//----------------------------------------------------------------------------------------
	static void Get(ConstUVWHandle dataPtr, Int32 i, UVWStruct& res);

	//----------------------------------------------------------------------------------------
	/// Sets the UVW coordinates for a polygon.
	/// @param[in] dataPtr						The handle to the UVW data. @callerOwnsPointed{UVW data}
	/// @param[in] i									The index of the polygon to set the coordinates for.
	/// @param[out] s									The UVW coordinates for the polygon.
	//----------------------------------------------------------------------------------------
	static void Set(UVWHandle dataPtr, Int32 i, const UVWStruct& s);

	//----------------------------------------------------------------------------------------
	/// Copies UVW coordinates.
	/// @param[in] destPtr						The handle to the destination UVW data.
	/// @param[in] dstIndex						The destination polygon index in the destination UVW data @formatParam{destptr}.
	/// @param[in] srcPtr							The handle to the source UVW data.
	/// @param[in] srcIndex						The source polygon index in the source UVW data @formatParam{srcptr}.
	//----------------------------------------------------------------------------------------
	static void Copy(UVWHandle destPtr, Int32 dstIndex, ConstUVWHandle srcPtr, Int32 srcIndex);

	/// @}

	/// @name Get/Set Parameter
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] data								Assigned the parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter was retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetDParameter(const DescID& id, GeData& data);

	//----------------------------------------------------------------------------------------
	/// Sets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] data								The parameter data to set.
	/// @return												@trueIfOtherwiseFalse{the parameter was set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_UVWTAG_H__
