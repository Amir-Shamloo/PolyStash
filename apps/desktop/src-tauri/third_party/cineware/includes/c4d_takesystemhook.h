/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TAKESYSTEMHOOK_H__
#define C4D_TAKESYSTEMHOOK_H__

#include "c4d_nodedata.h"
#include "c4d_baseoverride.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseLinkArray;
class TakeSystemHook;

/// Take system main mode, define the work flow to override parameters.
/// @since 17.008
enum TAKE_MODE
{
	TAKE_MODE_MANUAL = 0,	///< Manual override, the user have to explicitly every any single parameter.
	TAKE_MODE_AUTO				///< Auto Override, the system override any edited parameter (similar to Autokey).
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
TakeSystemHook* GetTakeSystemHook(BaseDocument* doc);

//----------------------------------------------------------------------------------------
/// The SceneHookData that handle the Takes structure and system in a document
//----------------------------------------------------------------------------------------
class TakeSystemHook : public NodeData
{
	INSTANCEOF(TakeSystemHook, NodeData)

public:
	TakeSystemHook()
	{
		_takeMode = TAKE_MODE_MANUAL;
		_showMainOverides = false;
		_treeDisplayMode	= 3;	// TAKETREEDISPLAYMODE::DOUBLETREE;
		_autoSelect = false;
		_overrideEnabling = OVERRIDEENABLING_GLOBAL | OVERRIDEENABLING_OBJECT | OVERRIDEENABLING_TAG | OVERRIDEENABLING_MATERIAL | OVERRIDEENABLING_SHADER | OVERRIDEENABLING_LAYER | OVERRIDEENABLING_OTHER | OVERRIDEENABLING_ENABLING | OVERRIDEENABLING_TRANSFOM | OVERRIDEENABLING_CAMERA | OVERRIDEENABLING_RS | OVERRIDEENABLING_PARAM;
		_mergeCurrentTake = nullptr;
		_takeBranchHead = nullptr;
	}

	virtual Bool Init();
	virtual void Free();
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	virtual Int32 GetChunkLevel() { return 3; }

	//----------------------------------------------------------------------------------------
	/// Retrieve the take system global mode, can be TAKE_MODE_MANUAL or TAKE_MODE_AUTO.
	/// This mode Affect how the user have to interact with GUI to override parameters.
	/// @return												The take system mode.
	//----------------------------------------------------------------------------------------
	TAKE_MODE GetMode() { return _takeMode; }

	//----------------------------------------------------------------------------------------
	/// Define the take system global mode, can be TAKE_MODE_MANUAL or TAKE_MODE_AUTO.
	/// Just for internal use to change mode outside of the take module please use the corresponding Command.
	/// This mode affect how the user have to interact with GUI to override parameters.
	/// @param[in] mode								The new mode to set.
	//----------------------------------------------------------------------------------------
	void SetMode(TAKE_MODE mode) { _takeMode = mode; }

	//----------------------------------------------------------------------------------------
	/// Set auto selection state for the document.
	/// @param[in] autoSelect					If true selected elements in Take manager will be selected also in other related managers.
	//----------------------------------------------------------------------------------------
	void SetAutoSelect(Bool autoSelect) { _autoSelect = autoSelect; }

	//----------------------------------------------------------------------------------------
	/// Gets the state of auto selection for the document.
	/// @return												True if autoselect is on, otherwise false.
	//----------------------------------------------------------------------------------------
	Bool GetAutoSelect() { return _autoSelect; }

	//----------------------------------------------------------------------------------------
	/// Retrieve a pointer to the Main take in doc.
	/// Main take is always the first under the header.
	/// @param[in] doc								A valid BaseDocument.
	/// @return												The Main take pointer, must be !nullptr.
	//----------------------------------------------------------------------------------------
	BaseTake*	GetMainTake(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Retrieve a pointer to the current take in doc.
	/// @param[in] doc								A valid BaseDocument.
	/// @return												The current take pointer, must be !nullptr.
	//----------------------------------------------------------------------------------------
	BaseTake* GetcurrentTake(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Set the current take for this Document.
	/// @param[in] doc								A valid BaseDocument.
	/// @param[in] cTake							The new current take for this document.
	//----------------------------------------------------------------------------------------
	void SetCurrentTake(BaseDocument* doc, BaseTake* cTake);

	//----------------------------------------------------------------------------------------
	/// Create and insert a new take and return a pointer to it.
	///	Note: selections have to be handled manually.
	/// @param[in] doc								A valid BaseDocument.
	/// @param[in] name								The Name or the new take if pass an empty string default name will be used.
	/// @param[in] parent							Optionally can be passed a parent take otherwise mew take will be added at the end of the list under Main.
	/// @param[in] cloneFrom					NOT SUPPORTED YET (If a another take is passed the new take will be cloned form it).
	/// @return												The new take pointer, must be !nullptr.
	//----------------------------------------------------------------------------------------
	BaseTake* AddTake(BaseDocument* doc, const String& name, BaseTake* parent, BaseTake* cloneFrom);

	//----------------------------------------------------------------------------------------
	/// Delete passed take and all connected overrides, if take is the current take The main Take will be set as current.
	/// @param[in] doc								A Valid BaseDocument.
	/// @param[in] take								The Take To be deleted.
	//----------------------------------------------------------------------------------------
	void DeleteTake(BaseDocument* doc, BaseTake* take);

	//----------------------------------------------------------------------------------------
	/// Retrieve a pointer to the Global Camera list for this document.
	/// This list is useful in various part of the GUI EG context menu or combo box.
	/// @return												A pointer to the camera list.
	//----------------------------------------------------------------------------------------
	BaseLinkArray* GetCameraList() { return &_cameraList; }

	//----------------------------------------------------------------------------------------
	/// Retrieve a pointer to the Global RenderData list for this document.
	/// This list is useful in various part of the GUI EG context menu or combo box.
	/// @return												A pointer to the RenderData list.
	//----------------------------------------------------------------------------------------
	BaseLinkArray* GetRenderdataList() { return &_renderDataList; }

	//----------------------------------------------------------------------------------------
	/// Gets the Override enabling state.
	/// @return												The override enabling defined in Take manager.
	//----------------------------------------------------------------------------------------
	OVERRIDEENABLING GetOverrideEnabling() { return _overrideEnabling; }

	//----------------------------------------------------------------------------------------
	/// Define the override enabling.
	/// @param[in] enabling						The overrides type to be enabled.
	//----------------------------------------------------------------------------------------
	void SetOverrideEnabling(OVERRIDEENABLING enabling) { _overrideEnabling = enabling; }

	static NodeData* Alloc() { return NewObj(TakeSystemHook); }
	void CheckTakeName(BaseTake* op);

private:
	RootPluginNode*	 _takeBranchHead;		// takes root in the document.
	String					 _takeBranchName;		// branch name.

	BaseLink				 _currentTake;			// a link to the current take in to the document.

	TAKE_MODE				 _takeMode;					// the take mode Manual/Auto/...
	Bool						 _showMainOverides;	// show overrides in main takes
	Int32						 _treeDisplayMode;	// tree mode

	Bool						 _autoSelect;				// auto select mode is on

	BaseLinkArray		 _cameraList;				// the list of all camera in the scene
	BaseLinkArray		 _renderDataList;		// the list of all render settings in the scene

	OVERRIDEENABLING _overrideEnabling;	// global enabling status based on GUI switch

	BaseTake*				 _mergeCurrentTake;	// used to recover the current take after merge operation (during merge both document will be set on main take)
};

#pragma pack (pop)
}

#endif	// C4D_TAKESYSTEMHOOK_H__
