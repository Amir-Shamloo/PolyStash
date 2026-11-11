/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_FRACTUREVORONOIDATA_H__
#define C4D_FRACTUREVORONOIDATA_H__

#include "c4d_nodedata.h"
#include "c4d_rootobject.h"
#include "c4d_misc/datastructures/basearray.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDocument;

struct LoadSelectionTagData
{
	Bool loadTags;
	Int32 tagCount;
	BaseArray<Int32> tagIndices;
};

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Mograph Fracture Voronoi object.
/// @since R20
//----------------------------------------------------------------------------------------
class MoGraphFractureVoronoiObject : public NodeData
{
	INSTANCEOF(MoGraphFractureVoronoiObject, NodeData)

	friend class BaseObject;

public:

protected:
	static NodeData* Alloc();

private:
	virtual Bool Init();
	virtual void Free();

	virtual Int32 GetChunkLevel();

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	RootObject*						_head;
	Bool									_createDefaultPointGen;
	LoadSelectionTagData	_loadSelectionTags;
	BaseArray<BaseLink*>	_selectionTags;
};

#pragma pack (pop)
}

#endif	// C4D_FRACTUREVORONOIDATA_H__
