/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEOVERRIDEGROUP_H__
#define C4D_BASEOVERRIDEGROUP_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseTag;
class BaseTake;
class TakeData;

//----------------------------------------------------------------------------------------
/// An Override Group manages the values of multiple objects in a @link BaseTake Take@endlink.
/// @see The Take System Overview articles in the C++ SDK documentation.
/// @since 17.008
//----------------------------------------------------------------------------------------
class BaseOverrideGroup : public BaseList2D
{
	INSTANCEOF(BaseOverrideGroup, BaseList2D)

public:
	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next Override Group in the list. Convenience version of GeListNode::GetNext() returning a @ref BaseOverrideGroup*.
	/// @return												The next Override group, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseOverrideGroup}
	//----------------------------------------------------------------------------------------
	BaseOverrideGroup* GetNext() { return (BaseOverrideGroup*)SUPER::GetNext(); }

	//----------------------------------------------------------------------------------------
	/// Gets the previous Override Group in the list. Convenience version of GeListNode::GetPred() returning a @ref BaseOverrideGroup*.
	/// @return												The previous Override group, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseOverrideGroup}
	//----------------------------------------------------------------------------------------
	BaseOverrideGroup* GetPred() { return (BaseOverrideGroup*)SUPER::GetPred(); }

	/// @}

	/// @name Visibility Modes
	/// @{

	//----------------------------------------------------------------------------------------
	/// Returns the editor visibility mode for the Override Group.
	/// @return												The editor mode.
	//----------------------------------------------------------------------------------------
	Int32 GetEditorMode();

	//----------------------------------------------------------------------------------------
	/// Returns the render visibility mode for the Override Group.
	/// @return												The render mode.
	//----------------------------------------------------------------------------------------
	Int32 GetRenderMode();

	//----------------------------------------------------------------------------------------
	/// Sets the editor visibility mode for the Override Group.
	/// @param[in] mode								The editor mode to set.
	//----------------------------------------------------------------------------------------
	void SetEditorMode(Int32 mode);

	//----------------------------------------------------------------------------------------
	/// Sets the render visibility mode for the Override Group.
	/// @param[in] mode								The render mode to set.
	//----------------------------------------------------------------------------------------
	void SetRenderMode(Int32 mode);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Searches for a tag of the given @formatParam{type} attached to the Override Group.
	/// @param[in] type								The tag type to search for.
	/// @return												The tag if found, otherwise @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseTag* GetTag(Int32 type);

	//----------------------------------------------------------------------------------------
	/// Returns the Take that owns the Override Group.
	/// @return												The owner Take, or @formatConstant{nullptr} if any error.
	//----------------------------------------------------------------------------------------
	BaseTake* GetTake();

	//----------------------------------------------------------------------------------------
	/// Checks if an object is included in the Override Group.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] op									The object to check. @callerOwnsPointed{BaseObject}
	/// @return												@trueIfOtherwiseFalse{the object is included in the Override Group}
	//----------------------------------------------------------------------------------------
	Bool Find(TakeData* takeData, BaseObject* op);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASEOVERRIDEGROUP_H__
