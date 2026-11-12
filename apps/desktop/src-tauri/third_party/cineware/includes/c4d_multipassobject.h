/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_MULTIPASS_H__
#define C4D_MULTIPASS_H__

#include "c4d_baselist4d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents @C4D's multipass settings.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class MultipassObject : public BaseList4D
{
	INSTANCEOF(MultipassObject, BaseList4D)

	BaseContainer inheritance_container;

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	MultipassObject();

	/// @}

	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{multipass object}
	/// @return												@allocReturn{multipass object}
	//----------------------------------------------------------------------------------------
	static MultipassObject* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{multipass objects}
	/// @param[in,out] mp							@theToDestruct{multipass object}
	//----------------------------------------------------------------------------------------
	static void Free(MultipassObject*& mp);

	/// @}

	/// @name navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next multipass object in the list.
	/// @return												The next multipass object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{multipass object}
	//----------------------------------------------------------------------------------------
	MultipassObject* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous multipass object in the list.
	/// @return												The previous multipass object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{multipass object}
	//----------------------------------------------------------------------------------------
	MultipassObject* GetPred();

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the multipass object.
	/// @return												The multipass type.
	//----------------------------------------------------------------------------------------
	Int32 GetMultipassType();

	//----------------------------------------------------------------------------------------
	/// Gets the object buffer group ID for the multipass object.
	/// @return												The buffer group ID.
	//----------------------------------------------------------------------------------------
	Int32 GetMultipassObjectBufferGroupID();

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the multipass object.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned multipass object. @callerOwnsPointed{multipass object}
	//----------------------------------------------------------------------------------------
	BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

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

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_MULTIPASS_H__
