/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEVIDEOPOST_H__
#define C4D_BASEVIDEOPOST_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a video post node.
/// @addAllocFreeNote
///
/// @note	To check if a video post node is enabled call:
/// @code
/// Bool vpEnabled = !pvp->GetBit(BIT_VPDISABLED);
/// @endcode
//----------------------------------------------------------------------------------------
class BaseVideoPost : public BaseList2D
{
	INSTANCEOF(BaseVideoPost, BaseList2D)

	BaseContainer inheritance_container;

	Int32 vpID;

protected:
	virtual Int32 GetInstanceType() const { return VPbase; }

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseVideoPost();

	//----------------------------------------------------------------------------------------
	/// Construct a video post of the given type @formatParam{id}.
	///
	/// @param[in] id									The video post type ID.
	//----------------------------------------------------------------------------------------
	BaseVideoPost(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseVideoPost();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{video post}
	/// @param[in] id									The video post type ID.
	/// @return												@allocReturn{video post}
	//----------------------------------------------------------------------------------------
	static BaseVideoPost* Alloc(Int32 id);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{video posts}
	/// @param[in,out] vp							@theToDestruct{video post}
	//----------------------------------------------------------------------------------------
	static void Free(BaseVideoPost*& vp);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next video post in the list.
	/// @return												The next video post effect, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{video post}
	//----------------------------------------------------------------------------------------
	BaseVideoPost* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous video post in the list.
	/// @return												The previous video post effect, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{video post}
	//----------------------------------------------------------------------------------------
	BaseVideoPost* GetPred();

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the video post.
	/// @warning	This must be used to make sure that the derived tag really is of the right type before casting it and accessing its members.
	/// @note			Plugin materials return their unique plugin IDs.
	/// @return												The video post type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the video post's unique disk type.
	/// @note	Normally this is the same as GetType(), but there are exceptions.
	/// @return												The video post's unique disk type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	//virtual Int32 GetBasePluginID() const;

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the video post.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned video post. @callerOwnsPointed{video post}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	NodeData* AllocData(Int32 id, Bool& known);

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

#endif	// C4D_BASEVIDEOPOST_H__
