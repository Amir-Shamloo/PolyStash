/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TAKEDATA_H__
#define C4D_TAKEDATA_H__

#include "c4d_system.h"
#include "c4d_parameter_ids.h"
#include "c4d_string.h"
#include "c4d_descid.h"
#include "c4d_baseoverride.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseList2D;
class BaseDocument;
class BaseTake;
class BaseOverride;
struct OverrideDataSet;

//----------------------------------------------------------------------------------------
/// Stores the Take System related data of a BaseDocument.
/// @see The Take System Overview articles in the C++ SDK documentation.
/// @since 17.008
//----------------------------------------------------------------------------------------
class TakeData
{
	friend class BaseDocument;

public:
	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the internal document link for the TakeData.
	/// @markPrivate
	/// @param[in] doc								The document to attach to the TakeData. @callerOwnsPointed{BaseDocument}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Set(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Retrieves the document for the TakeData.
	/// @return												The document, or @formatConstant{nullptr} if TakeData is not initialized. @cinewareOwnsPointed{BaseDocument}
	//----------------------------------------------------------------------------------------
	BaseDocument* GetDocument();

	//----------------------------------------------------------------------------------------
	/// Retrieves the Main Take.
	/// @note Main Take is always the first under the header.
	/// @return												The Main Take. Must be !@formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseTake* GetMainTake();

	/// @}

	/// @name Override Enabling
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the ability for the Take System to override a specific kind of node based on global switch.
	/// @return												The Take System override enabling: @enumerateEnum{OVERRIDEENABLING}
	//----------------------------------------------------------------------------------------
	OVERRIDEENABLING GetOverrideEnabling();

	//----------------------------------------------------------------------------------------
	/// Checks for a specific @ref OVERRIDEENABLING.
	/// @param[in] mask								The Take System override enabling mask: @enumerateEnum{OVERRIDEENABLING}
	/// @return												@formatConstant{true} if the @ref OVERRIDEENABLING is set otherwise @formatConstant{false}.
	//----------------------------------------------------------------------------------------
	Bool CheckOverrideEnabling(OVERRIDEENABLING mask);

	/// @}

	/// @name Take
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves a pointer to the current Take.
	/// @return												The current Take, or @formatConstant{nullptr} if any error. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetCurrentTake();

	//----------------------------------------------------------------------------------------
	/// Sets the current Take.
	/// @param[in] take								The Take to set. If @formatConstant{nullptr} is passed the Main Take will be set. @callerOwnsPointed{BaseTake}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetCurrentTake(BaseTake* take);

	//----------------------------------------------------------------------------------------
	/// Creates and inserts a new Take.
	/// @note An undo step is added automatically if the call is added from the main (GUI) thread and global undo is allowed (see %TakeData::GetUndoState/%TakeData::SetUndoState).
	/// @warning Selections have to be handled manually.
	/// @param[in] name								The name of the Take to add. If an empty string is passed default Take name will be used.
	/// @param[in] parent							Optionally pass a parent Take otherwise new Take will be added at the end of the list under the Main Take. @callerOwnsPointed{BaseTake}
	/// @param[in] cloneFrom					UNUSED, NOT SUPPORTED YET. (Optionally pass a Take the new Take will be cloned from. @callerOwnsPointed{BaseTake})
	/// @return												The new Take. Must be !@formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseTake* AddTake(const String& name, BaseTake* parent, BaseTake* cloneFrom = nullptr);

	//----------------------------------------------------------------------------------------
	/// Deletes a Take and all connected overrides. If Take is the current the Main Take will be set as current.
	/// @param[in] take								The Take to delete. @callerOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	void DeleteTake(BaseTake* take);

	//----------------------------------------------------------------------------------------
	/// Moves a Take in the hierarchy in a safe way.\n
	/// The Take system has several hierarchy dependencies.\n
	/// If a Take is moved while it is current or while it is a child of the Current Take then this would need to manually take care of all data sorting and handling.\n
	/// This function do all this work for you.\n
	/// @param[in] takeToMove					The Take to move. @callerOwnsPointed{BaseTake}
	/// @param[in] destTake						The parent destination Take. If @formatConstant{nullptr} the Main Take will be used. @callerOwnsPointed{BaseTake}
	/// @param[in] insertMode					The insertion mode. Can be @ref INSERT_BEFORE or @ref INSERT_AFTER or @ref INSERT_UNDER.
	//----------------------------------------------------------------------------------------
	void InsertTake(BaseTake* takeToMove, BaseTake* destTake, Int32 insertMode);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOverride(BaseTake* take, BaseList2D* op);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	BaseOverride* AddOverride(const OverrideDataSet& param);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void DeleteOverride(const OverrideDataSet& param);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOverrideCounterPart(BaseOverride* overrideNode, const DescID& descId, BaseTake*& resultTake);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	BaseOverride* FindOverrideInHierarchy(BaseTake* take, BaseList2D* op, const DescID& descId, BaseTake*& resultTake);

	/// @}

protected:
	TakeData() : _doc(nullptr) { }
	~TakeData()  { }

	BaseDocument* _doc;	//a pointer to the document own the TakeData
};

#pragma pack (pop)
}

#endif	// C4D_TAKEDATA_H__
