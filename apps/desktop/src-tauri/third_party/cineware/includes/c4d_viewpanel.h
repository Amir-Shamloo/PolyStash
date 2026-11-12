/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VIEWPANEL_H__
#define C4D_VIEWPANEL_H__

#include "c4d_baselist2d.h"
#include "c4d_rootbasedraw.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDraw;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class ViewPanel : public BaseList2D
{
	INSTANCEOF(ViewPanel, BaseList2D)

	friend class BaseDocument;

	BaseContainer	layout;
	RootBaseDraw	root_basedraw;

	BaseDraw* GetBaseDraw(Int32 num);

public:
	ViewPanel();

	ViewPanel* GetNext();
	ViewPanel* GetPred();

	Int32 GetType() const;
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);
};

#pragma pack (pop)
}

#endif	// C4D_VIEWPANEL_H__
