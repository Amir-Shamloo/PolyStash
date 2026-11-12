/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_EXPRESSIONTAGDATA_H__
#define C4D_EXPRESSIONTAGDATA_H__

#include "c4d_nodedata.h"
#include "c4d_expresso/gv_nodemaster.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Xpresso tag.
/// @since R20
//----------------------------------------------------------------------------------------
class GvExpressionData : public NodeData
{
	INSTANCEOF(GvExpressionData, NodeData)

private:

	GvRealNodeMaster* master;

	virtual Int32 GetChunkLevel();

	GvExpressionData();
	~GvExpressionData();

	virtual Bool Init();
	virtual void Free();

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

public:
	/// @name Alloc
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{weight tag node data}
	/// @return												@allocReturn{weight tag node data}
	//----------------------------------------------------------------------------------------
	static NodeData* Alloc();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_EXPRESSIONTAGDATA_H__
