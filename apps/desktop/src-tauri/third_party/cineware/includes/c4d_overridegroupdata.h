/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_OVERRIDEGROUPDATA_H__
#define C4D_OVERRIDEGROUPDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class InExcludeData;
class BaseDocument;
class BaseTake;

//----------------------------------------------------------------------------------------
// Override Group plugin useful to create massive override of the same type
//----------------------------------------------------------------------------------------
class OverrideGroupData : public NodeData
{
	INSTANCEOF(OverrideGroupData, NodeData)

public:
	OverrideGroupData()		{ }
	~OverrideGroupData()	{ }

	virtual Int32 GetChunkLevel() { return 3; }

	virtual Bool Init();

	Bool SetCurrent(BaseDocument* doc);
	Bool SetNotCurrent(BaseDocument* doc);

	void SetEditorMode(Int32 mode);
	void SetRenderMode(Int32 mode);

	Bool Check(BaseDocument* doc, BaseList2D* node, InExcludeData* iData = nullptr);
	BaseTake* GetTake();

	static NodeData* Alloc() { return NewObj(OverrideGroupData); }
};

#pragma pack (pop)
}

#endif	// C4D_OVERRIDEGROUPDATA_H__
