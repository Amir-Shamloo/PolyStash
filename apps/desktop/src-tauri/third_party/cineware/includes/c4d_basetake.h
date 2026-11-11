/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASETAKE_H__
#define C4D_BASETAKE_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class TakeData;
class BaseOverrideGroup;

/// BaseTake container IDs
enum
{
	TAKEBASE_INDEX				= 1000,		// Int32
	TAKEBASE_CAMERA				= 10002,	// link
	TAKEBASE_RDATA				= 10003,	// link
	TAKEBASE_CAMERA_COMBO = 10004,	// Int32 cycle
	TAKEBASE_RDATA_COMBO	= 10005,	// Int32 cycle
	TAKEBASE_NOTES				= 10006,	// String
	TAKEBASE_OVERRIDES		= 20000,	// link list
	TAKEBASE_CHECK				=	20001,	// Bool take is checked

	TAKEBASE
};

static const Int32 FAKE_DESCID = -2;	// used to add an empty override to use it just as a parent

//----------------------------------------------------------------------------------------
/// Represents a Take of the @link TakeData Take System@endlink. It stores BaseOverride objects that define how a specific entity is changed in a Take.
/// @note To retrieve the Take System for a document simply call BaseDocument::GetTakeData().
/// @see The Take System Overview articles in the C++ SDK documentation.
/// @since 17.008
//----------------------------------------------------------------------------------------
class BaseTake : public BaseList2D
{
	INSTANCEOF(BaseTake, BaseList2D)

public:
	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next Take in the list. Convenience version of GeListNode::GetNext() returning a @ref BaseTake*.
	/// @return												The next Take, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous Take in the list. Convenience version of GeListNode::GetPred() returning a @ref BaseTake*.
	/// @return												The previous Take, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetPred();

	//----------------------------------------------------------------------------------------
	/// Gets the parent Take of the list node. Convenience version of GeListNode::GetUp() returning a @ref BaseTake*.
	/// @return												The parent Take, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child Take of the list node. Convenience version of GeListNode::GetDown() returning a @ref BaseTake*.
	/// @return												The first child Take, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child Take of the list node. Convenience version of GeListNode::GetDownLast() returning a @ref BaseTake*.
	/// @return												The last child Take, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetDownLast();

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the Take is the Main Take.
	/// @return												@trueIfOtherwiseFalse{the Take is the Main Take}
	//----------------------------------------------------------------------------------------
	Bool IsMain();

	//----------------------------------------------------------------------------------------
	/// Checks if the Take is a child of @formatParam{op}.
	/// @param[in] op									The Take to search within his hierarchy. @callerOwnsPointed{BaseTake}
	/// @return												@trueIfOtherwiseFalse{the Take is child of @formatParam{op}}
	//----------------------------------------------------------------------------------------
	Bool SearchHierarchy(BaseTake* op);

	/// @}

	/// @name Override
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves all Override nodes owned by the Take.
	/// @param[out] selection					The Array to be filled.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool GetOverrides(GeTempDynArray<GeListNode>& selection);

	//----------------------------------------------------------------------------------------
	/// Retrieves all Override Groups nodes owned by the Take.
	/// @param[out] selection					The Array to be filled.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool GetOverrideGroups(GeTempDynArray<GeListNode>& selection);

	//----------------------------------------------------------------------------------------
	/// Searches if parameter with @formatParam{descID} is Overridden. If not adds a new Override with passed value for the Take.\n
	/// If the BaseOverride node does not exist the function automatically allocates and inserts it, plus takes care to backup data properly in parent or Main Take.
	/// @note An undo step is added automatically if the call is added from the main (GUI) thread and global undo is allowed (see %TakeData::GetUndoState/%TakeData::SetUndoState).
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] node								The scene node to override. @callerOwnsPointed{BaseList2D}
	/// @param[in] descID							The parameter DescID to override.
	/// @param[in] overrideValue			The initial value to set in the overrides for the Take.
	/// @param[in] backupValue				Optionally provide the backup value for the Main/parent Take. Mandatory to set this for parameters not from the GUI.
	/// @param[in] deleteAnim					If @formatParam{true} and original parameter in the scene is animated the animation will be removed in the resulting Override.
	/// @return												The found or newly created BaseOverride node, or @formatParam{nullptr} if any error.
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOrAddOverrideParam(TakeData* takeData, BaseList2D* node, const DescID& descID, const GeData& overrideValue, const GeData& backupValue = GeData(), Bool deleteAnim = false);

	//----------------------------------------------------------------------------------------
	/// Deletes a single parameter override for @formatParam{node} with @formatParam{descID}. If the Override results empty (no more overridden parameters) then it will be deleted too.
	/// @note An undo step is added automatically if the call is added from the main (GUI) thread and global undo is allowed (see %TakeData::GetUndoState/%TakeData::SetUndoState).
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] node								The scene node to delete the parameter for. @callerOwnsPointed{BaseList2D}
	/// @param[in] descID							The parameter DescID to be deleted.
	//----------------------------------------------------------------------------------------
	void DeleteOverride(TakeData* takeData, BaseList2D* node, const DescID& descID);

	//----------------------------------------------------------------------------------------
	/// Searches if @formatParam{node} is overridden in the Take.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] node								The node to search the BaseOverride for. @callerOwnsPointed{BaseList2D}
	/// @return												The BaseOverride if found, otherwise @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOverride(TakeData* takeData, BaseList2D* node);

	//----------------------------------------------------------------------------------------
	/// Searches if @formatParam{node} parameter with @formatParam{descID} is overridden in the Take or in a parent.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] node								The node to search the BaseOverride for. @callerOwnsPointed{BaseList2D}
	/// @param[in] descID							The parameter DescID to search for.
	/// @param[out] resultTake				Filled with The Take that owns the Override. Can be equal to @c *this or @formatConstant{nullptr} if the Override is not found. @callerOwnsPointed{BaseTake}
	/// @return												The BaseOverride if found, otherwise @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOverrideInHierarchy(TakeData* takeData, BaseList2D* node, const DescID& descID, BaseTake*& resultTake);

	//----------------------------------------------------------------------------------------
	/// Adds a new Override Group to the Take.
	/// @note An undo step is added automatically if the call is added from the main (GUI) thread and global undo is allowed (see %TakeData::GetUndoState/%TakeData::SetUndoState).
	/// @return												The added Override Group, or @formatConstant{nullptr} if any error.
	//----------------------------------------------------------------------------------------
	BaseOverrideGroup* AddOverrideGroup();

	//----------------------------------------------------------------------------------------
	/// Gets the first Override Group in the Take.
	/// @return												The first Override Group, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	BaseOverrideGroup* GetFirstOverrideGroup();

	/// @}

	/// @name Camera
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the camera for the Take.
	/// @note Can return the default camera.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @return												The camera assigned to the Take, or @formatConstant{nullptr} if the Take uses the camera from a parent Take.
	//----------------------------------------------------------------------------------------
	BaseObject* GetCamera(TakeData* takeData);

	//----------------------------------------------------------------------------------------
	/// Gets the camera used by the Take even if it comes from a parent Take.
	/// @note Can return the default camera.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[out] resultTake				The Take the camera comes from. @cinewareOwnsPointed{BaseTake}
	/// @return												The camera used by the Take.
	//----------------------------------------------------------------------------------------
	BaseObject* GetEffectiveCamera(TakeData* takeData, BaseTake*& resultTake);

	//----------------------------------------------------------------------------------------
	/// Sets the camera for the Take.
	/// @note Can be the default camera.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] camera							The camera to set, or @formatConstant{nullptr} to reset and use one from a parent Take.
	//----------------------------------------------------------------------------------------
	void SetCamera(TakeData* takeData, BaseObject* camera);

	/// @}

	/// @name RenderData
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the RenderData for the Take.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @return												The RenderData assigned to the Take, or @formatConstant{nullptr} if the Take uses the RenderData from a parent Take.
	//----------------------------------------------------------------------------------------
	RenderData* GetRenderData(TakeData* takeData);

	//----------------------------------------------------------------------------------------
	/// Gets the RenderData used by the Take even if it comes from a parent Take.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[out] resultTake				The Take the RenderData come from. @cinewareOwnsPointed{BaseTake}
	/// @return												The RenderData used by the Take.
	//----------------------------------------------------------------------------------------
	RenderData* GetEffectiveRenderData(TakeData* takeData, BaseTake*& resultTake);

	//----------------------------------------------------------------------------------------
	/// Sets the RenderData for the Take.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] rData							The RenderData to set, or @formatConstant{nullptr} to reset and use one from a parent Take. @callerOwnsPointed{RenderData}
	//----------------------------------------------------------------------------------------
	void SetRenderData(TakeData* takeData, RenderData* rData);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Resets all sub-structures and Overrides for the Take.
	/// @warning All data not in the current state of the document are deleted.
	//----------------------------------------------------------------------------------------
	void Reset();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool CompactTake(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool RestoreTake(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	static BaseTake* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	static void Free(BaseTake*& p);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASETAKE_H__
