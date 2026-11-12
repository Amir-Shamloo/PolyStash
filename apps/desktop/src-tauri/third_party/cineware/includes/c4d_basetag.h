/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASETAG_H__
#define C4D_BASETAG_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup TAG
/// @ingroup group_enumeration
/// @{
#define TAG_VISIBLE				 (1 << 0)	///< The tag can be seen in the Object Manager.
#define TAG_MULTIPLE			 (1 << 1)	///< Multiple copies of the tag allowed on a single object.
#define TAG_HIERARCHICAL	 (1 << 2)	///< The tag works hierarchical, so that sub-objects inherit its properties (e.g. the material tag).
#define TAG_EXPRESSION		 (1 << 3)	///< The tag is an expression.
#define TAG_TEMPORARY			 (1 << 4)	///< @markPrivate
#define TAG_MODIFYOBJECT	 (1 << 6)	///< @markPrivate
#define TAG_ADDTOTAKEGROUP (1 << 7)	///< The tag is added to the Take override groups system.
/// @}

//----------------------------------------------------------------------------------------
/// A variety of properties can be added to objects using tags. For example, adding smoothing (Phong tag) or preventing an object from being edited accidentally (Protection tag).
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseTag : public BaseList2D
{
	INSTANCEOF(BaseTag, BaseList2D)

	friend class RootTag;
	friend class BaseObject;
	friend class PointObjectData;
	friend class PolygonObjectData;
	friend class PolygonObject;

protected:
	BaseObject* parent_op;

	virtual Int32 GetInstanceType() const { return Tbase; }

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor. @markPrivate
	//----------------------------------------------------------------------------------------
	BaseTag(Int32 id = NOTOK, Int32 pD = NOTOK);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseTag();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base tag}
	/// @param[in] type								The tag type.
	/// @return												@allocReturn{base tag}
	//----------------------------------------------------------------------------------------
	static BaseTag* Alloc(Int32 type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base tags}
	/// @param[in,out] bl							@theToDestruct{base tag}
	//----------------------------------------------------------------------------------------
	static void Free(BaseTag*& bl);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next tag in the list.
	/// @return												The next tag, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{tag}
	//----------------------------------------------------------------------------------------
	BaseTag* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous tag in the list.
	/// @return												The previous tag, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{tag}
	//----------------------------------------------------------------------------------------
	BaseTag* GetPred();

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the tag.
	/// @warning	This must be used to make sure that the derived tag really is of the right type before casting it and accessing its members.
	/// @note			Plugin tags return their unique plugin IDs.
	/// @return												The tag type, for example @em Tprotection.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the tag's unique disk type.
	/// @note	Normally this is the same as GetType(), but there are exceptions.
	/// @return												The tag's unique disk type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	/// @name Clone/Copy
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the tag.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned tag. @callerOwnsPointed{tag}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the tag to @formatParam{dest}.
	/// @warning	The tags must be of the same type!
	/// @param[in] dest								The destination tag. @callerOwnsPointed{tag}
	/// @param[in] flags							Flags for the copy.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the tag values were copied to @formatParam{dest}}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyDataV(BaseTag* dest, COPYFLAGS flags, AliasTrans* trans);

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

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object that the tag is attached to.
	/// @return												The host object, or @formatConstant{nullptr} if the tag is not associated with an object. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetObject();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASETAG_H__
