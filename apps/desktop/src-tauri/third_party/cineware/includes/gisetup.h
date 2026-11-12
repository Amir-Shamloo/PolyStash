/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_GISETUP_H__
#define C4D_GISETUP_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's GI video post effect.
//----------------------------------------------------------------------------------------
class GIData : public NodeData
{
	INSTANCEOF(GIData, NodeData)

public:
	void MapPresetsParameters(GeListNode* pNode, BaseContainer* pData);
	void MapSamplingParameters(BaseContainer* pData);
	void MapModeParameters(BaseContainer* pData);
	void MapDensityParameters(BaseContainer* pData);
	void MapCorrectionParameters(BaseContainer* pData);
	void MapICDensityParameters(BaseContainer* pData);

	// ---
	virtual Bool Init();
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	static NodeData*			Alloc()	{ return NewObj(GIData); }
};

#pragma pack (pop)
}

#endif	// C4D_GISETUP_H__
