/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASESCENEHOOK_H__
#define C4D_BASESCENEHOOK_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//-------------------------------------------------------------------------------------------------
/// The class for plugin scene hooks. Cannot be instantiated.
/// @since 17.008
//-------------------------------------------------------------------------------------------------
class BaseSceneHook : public BaseList2D
{
	INSTANCEOF(BaseSceneHook, BaseList2D)

	friend class BaseDocument;
	friend class RootSceneHook;

public:
	BaseSceneHook(Int32 id);

protected:
	BaseSceneHook();
	~BaseSceneHook();

private:
	BaseSceneHook* GetNext();
	BaseSceneHook* GetPred();

	NodeData* AllocData(Int32 id, Bool& known);

	Int32 GetType() const;
	Int32 GetDiskType() const;
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
};

#pragma pack (pop)
}

#endif	// C4D_BASESCENEHOOK_H__
