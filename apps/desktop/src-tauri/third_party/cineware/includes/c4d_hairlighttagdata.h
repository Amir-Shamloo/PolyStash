/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_HAIRLIGHTTAGDATA_H__
#define C4D_HAIRLIGHTTAGDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class HairLightTagData : public NodeData
{
	INSTANCEOF(HairLightTagData, NodeData)

public:
	virtual Bool Init();
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	static NodeData* Alloc() { return NewObj(HairLightTagData); }

private:
	Int32 m_iLevel;
};

#pragma pack (pop)
}

#endif	// C4D_HAIRLIGHTTAGDATA_H__
