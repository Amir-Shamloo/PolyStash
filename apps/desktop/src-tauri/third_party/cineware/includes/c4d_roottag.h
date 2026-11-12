/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTTAG_H__
#define C4D_ROOTTAG_H__

#include "c4d_rootlist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class RootTag : public RootList2D
{
	INSTANCEOF(RootTag, RootList2D)

private:
	BaseObject* m_op;

public:
	RootTag(BaseObject* op);
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);
};

#pragma pack (pop)
}

#endif	// C4D_ROOTTAG_H__
