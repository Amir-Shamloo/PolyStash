/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEOVERRIDE_H__
#define C4D_BASEOVERRIDE_H__

#include "c4d_baselist2d.h"
#include "c4d_misc/general.h"
#include "c4d_misc/datastructures/basearray.h"
#include "c4d_baseoverridedata.h"

namespace cineware
{
#pragma pack (push, 8)

class TakeData;

//----------------------------------------------------------------------------------------
/// Define which kind of nodes can be overridden based on global gui switch.
/// @since 17.008
//----------------------------------------------------------------------------------------
enum OVERRIDEENABLING
{
	OVERRIDEENABLING_0				= 0,					///< None.
	OVERRIDEENABLING_OBJECT		= (1 << 0),		///< Object override enabled.
	OVERRIDEENABLING_TAG			= (1 << 1),		///< Tag override enabled.
	OVERRIDEENABLING_MATERIAL	= (1 << 2),		///< Material override enabled.
	OVERRIDEENABLING_SHADER		= (1 << 3),		///< Shader override enabled.
	OVERRIDEENABLING_LAYER		= (1 << 4),		///< Layer override enabled.
	OVERRIDEENABLING_OTHER		= (1 << 5),		///< Other types override enabled.
	OVERRIDEENABLING_GLOBAL		= (1 << 6),		///< Main switch types override enabled.

	OVERRIDEENABLING_ENABLING = (1 << 7),		///< Object enabling and visibility override enabled.
	OVERRIDEENABLING_TRANSFOM = (1 << 8),		///< Transformation override enabled.
	OVERRIDEENABLING_CAMERA		= (1 << 9),		///< Active camera override enabling.
	OVERRIDEENABLING_RS				= (1 << 10),	///< Active Render Settings override enabling.
	OVERRIDEENABLING_PARAM		= (1 << 11)		///< Generic parameter override enabling.

} ENUM_END_FLAGS(OVERRIDEENABLING);

//----------------------------------------------------------------------------------------
/// An Override holds the settings for a specific object in a @link BaseTake Take@endlink.
/// @see The Take System Overview articles in the C++ SDK documentation.
/// @since 17.008
//----------------------------------------------------------------------------------------
class BaseOverride : public BaseList2D
{
	INSTANCEOF(BaseOverride, BaseList2D)

public:
	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the parameter at @formatParam{descID} is overridden.
	/// @param[in] descID							The parameter to be checked.
	/// @return												@trueIfOtherwiseFalse{parameter is overridden}
	//----------------------------------------------------------------------------------------
	Bool IsOverriddenParam(const DescID& descID);

	//----------------------------------------------------------------------------------------
	/// Returns a pointer to the original scene node connected to the Override node.
	/// @return												A pointer to the original scene node, or @formatConstant{nullptr} if any error.
	//----------------------------------------------------------------------------------------
	BaseList2D* GetSceneNode();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the Take that owns the Override.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @return												A pointer to the Take for this Override, or @formatConstant{nullptr} if any error. @cinewareOwnsPointed{BaseTake}
	//----------------------------------------------------------------------------------------
	BaseTake* GetOwnerTake(TakeData* takeData);

	//----------------------------------------------------------------------------------------
	/// Checks if the Override is also part of an Override Group, and if yes fills @formatParam{resGroup}.
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[out] resGroup					The assigned BaseOverrideGroup. @formatConstant{nullptr} if the function returns @formatConstant{false}. @cinewareOwnsPointed{BaseOverrideGroup}
	/// @return												@trueIfOtherwiseFalse{the Override is part of an Override Group}
	//----------------------------------------------------------------------------------------
	Bool IsInGroup(TakeData* takeData, BaseOverrideGroup*& resGroup);

	//----------------------------------------------------------------------------------------
	/// Retrieves the DescID of all parameters in the Override.
	/// @note Sub-descriptions are included if overridden.
	/// @param[out] result						The array to be filled.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool GetAllOverrideDescID(BaseArray<DescID>& result);

	//----------------------------------------------------------------------------------------
	/// Updates the scene node whenever data is directly changed on the base Override (for example with @ref SetParameter).\n
	/// This ensures the scene node is properly updated if the Override affects the current document state.
	/// @note An undo step is added automatically if the call is added from the main (GUI) thread and global undo is allowed (see %TakeData::GetUndoState/%TakeData::SetUndoState).
	/// @param[in] takeData						The Take System context. @callerOwnsPointed{TakeData}
	/// @param[in] descID							The DescID of the edited parameter.
	//----------------------------------------------------------------------------------------
	void UpdateSceneNode(TakeData* takeData, const DescID& descID);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool SetTakeNode(BaseTake* newTake);

	//----------------------------------------------------------------------------------------
	/// Private
	//----------------------------------------------------------------------------------------

	BaseOverrideGroup* GetGroupLink(BaseDocument* doc);
	BaseTake* GetTakeNode(BaseDocument* doc);
	BaseList2D* GetMainNode();
	Bool GetValuesFromDocument(BaseDocument* doc, NodeChange* exclude);
	Bool GetAllValuesFromDocument(BaseDocument* doc, BaseArray<OverrideMapEntry>& changed);
	Bool SendValuesToDocument(BaseDocument* doc, NodeChange* exclude, BaseList2D* toClone = nullptr);
	Bool SendAllValuesToDocument(BaseDocument* doc, BaseArray<OverrideMapEntry>& changed, BaseList2D* toClone = nullptr);
	Bool IsAnimatedOverrideParam(const DescID& pIdx);
	Bool DeleteTrackIfAnimated(const DescID& pIdx);
	Bool OverrideParam(const OverrideDataSet& data);
	GeData GetOverrideParam(const DescID& pIdx);
	Bool IsTakeAffected(BaseDocument* doc, BaseTake* take, const DescID& pIdx);
	Int GetOverrideParameterCount();
	Bool RemoveOverrideParam(BaseDocument* doc, const DescID& pIdx);
	Bool HasValidSubNodes(BaseDocument* doc);
	BaseList2D* GetParentNode(BaseDocument* doc);
	void SetParentNode(BaseList2D* parent);
	void AppendSubNode(BaseDocument* doc, BaseList2D* node);
	Bool RebuildUserData();
	Bool SetGroupLink(BaseOverrideGroup* group);

	static BaseOverride* Alloc() { return NewObj(BaseOverride); }
	static void Free(BaseOverride*& p)	{ DeleteObj((BaseList2D*&)p); }

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASEOVERRIDE_H__
