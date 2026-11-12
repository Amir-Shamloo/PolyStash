/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTLAYER_H__
#define C4D_ROOTLAYER_H__

#include "c4d_rootlist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class LayerObject;

//----------------------------------------------------------------------------------------
/// Almost private except AllocObject().
//----------------------------------------------------------------------------------------
class RootLayer : public RootList2D
{
	INSTANCEOF(RootLayer, RootList2D)

	friend class LayerObject;

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	RootLayer();

	//----------------------------------------------------------------------------------------
	/// Overload this method to allocate a layer for the root/list.\n
	/// <b>Example</b>:
	/// @code
	/// LayerObject *AlienRootLayer::AllocObject()
	/// {
	///   LayerObject* layer = nullptr;
	///   layer = NewObj(AlienLayer);
	///   return layer;
	/// }
	/// @endcode
	/// @return												The allocated layer.
	//----------------------------------------------------------------------------------------
	virtual LayerObject* AllocObject();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);
};

#pragma pack (pop)
}

#endif	// C4D_ROOTLAYER_H__
