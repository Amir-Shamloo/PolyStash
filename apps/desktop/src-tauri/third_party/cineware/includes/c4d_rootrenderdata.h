/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTRENDERDATA_H__
#define C4D_ROOTRENDERDATA_H__

#include "c4d_rootlist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class RenderData;

//----------------------------------------------------------------------------------------
/// Almost private except AllocObject().
//----------------------------------------------------------------------------------------
class RootRenderData : public RootList2D
{
	INSTANCEOF(RootRenderData, RootList2D)

public:
	//----------------------------------------------------------------------------------------
	/// Overload this method to allocate a render data for the root/list.\n
	/// <b>Example</b>:
	/// @code
	/// RenderData *AlienRootRenderData::AllocObject()
	/// {
	///   RenderData* rd = nullptr;
	///   rd = NewObj(AlienRenderData);
	///   return rd;
	/// }
	/// @endcode
	/// @return												The allocated render data.
	//----------------------------------------------------------------------------------------
	virtual RenderData* AllocObject();

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

#endif	// C4D_ROOTRENDERDATA_H__
