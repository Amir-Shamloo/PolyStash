/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASESCENEHOOKDATA_H__
#define C4D_BASESCENEHOOKDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseSceneHookData : public NodeData
{
	INSTANCEOF(BaseSceneHookData, NodeData)

public:
	BaseSceneHookData();
	~BaseSceneHookData();

	RootPluginNode* tl_marker;	// TimeLine Marker

	RootPluginNode* tl_region0;	// TimeLine Regions 0
	RootPluginNode* tl_region1;	// TimeLine Regions 1
	RootPluginNode* tl_region2;	// TimeLine Regions 2
	RootPluginNode* tl_region3;	// TimeLine Regions 3

	RootPluginNode* tl_copy;
	RootPluginNode* tl_copymotion;

	RootPluginNode* sb_bookmarks;	// Bookmarks (BaseSettingsObjects)
	RootPluginNode* tl_bookmarks;	// Bookmarks (BaseSettingsObjects)
	RootPluginNode* sb_root;			// plain 4 BaseSettingsObjects for SBManager Settings
	RootPluginNode* tl_root;			// plain 4 BaseSettingsObjects for Timeline Settings

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);
	virtual Bool Init();
	virtual Int32 GetChunkLevel();

	static NodeData* Alloc();
};

#pragma pack (pop)
}

#endif	// C4D_BASESCENEHOOKDATA_H__
