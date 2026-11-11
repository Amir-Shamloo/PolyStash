/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTLIST2D_H__
#define C4D_ROOTLIST2D_H__

#include "c4d_gelistnode.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class RootList2D : public GeListNode
{
	INSTANCEOF(RootList2D, GeListNode)

	friend class GeListNode;
	friend class BaseList2D;
	friend class BaseObject;
	friend class LayerObject;
	friend class RenderData;
	friend class BaseList4D;
	friend class RootObject;
	friend class PluginNode;
	friend class BaseShader;
	friend class TakeSystemHook;
	friend class FieldlayerRootNode;

private:
	GeTempPublicList2D<GeListNode> head;

protected:
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	GeListNode* t_real_parent;

public:
	RootList2D();
	~RootList2D();

	void SetParent(GeListNode* main) { t_real_parent = main; }
	GeListNode* GetParent() const { return t_real_parent; }

	Bool Insert(GeListNode* bl);
	Bool InsertBefore(GeListNode* bl, GeListNode* next);
	Bool InsertAfter(GeListNode* bl, GeListNode* pred);
	Bool InsertLast(GeListNode* bl);
	void Remove(GeListNode* bl);
	virtual Bool IsListHead();
	void FlushAll();

	GeListNode* GetFirst();
	GeListNode* GetLast();

	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
};

#pragma pack (pop)
}

#endif	// C4D_ROOTLIST2D_H__
