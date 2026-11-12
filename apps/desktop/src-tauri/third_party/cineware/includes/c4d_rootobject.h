/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTOBJECT_H__
#define C4D_ROOTOBJECT_H__

#include "c4d_rootlist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Almost private except AllocObject().
//----------------------------------------------------------------------------------------
class RootObject : public RootList2D
{
	INSTANCEOF(RootObject, RootList2D)

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	RootObject();

	//----------------------------------------------------------------------------------------
	/// Overload this method to allocate an object for the root/list.\n
	/// <b>Example</b>:
	/// @code
	/// BaseObject *AlienRootObject::AllocObject(Int32 id)
	/// {
	///   BaseObject *obj = nullptr;
	///
	///   switch (id)
	///   {
	///     case Oline:
	///     case Ospline:
	///       obj = (BaseObject*)NewObj(AlienSplineObject);
	///       if (!obj)
	///         return nullptr;
	///       ((AlienSplineObject*)obj)->SetNodeData(NewObj(PointObjectData));
	///       ((AlienSplineObject*)obj)->GetNodeData()->SetNode((AlienSplineObject*)obj);
	///       break;
	///
	///     case Oplugin:
	///       obj = BaseObject::Alloc(Oplugin);
	///       if (!obj)
	///         return nullptr;
	///       break;
	///
	///     case Opoint:
	///       obj = BaseObject::Alloc(Opoint);
	///       if (!obj)
	///         return nullptr;
	///       break;
	///
	///     default:
	///       obj = BaseObject::Alloc(id);
	///   }
	///
	///   return obj;
	/// }
	/// @endcode
	///
	/// @param[in] id									The object type ID.
	/// @return												The allocated object.
	//----------------------------------------------------------------------------------------
	virtual BaseObject* AllocObject(Int32 id);

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
	virtual Bool Write(HyperFile* hf);
};

#pragma pack (pop)
}

#endif	// C4D_ROOTOBJECT_H__
