/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef XMBVIDEOPOST_H__
#define XMBVIDEOPOST_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's XMB video post effect (physical renderer).
//----------------------------------------------------------------------------------------
class VPXMBData : public NodeData
{
	INSTANCEOF(VPXMBData, NodeData)

public:
	virtual Bool Init();
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	static NodeData*			Alloc()	{ return NewObj(VPXMBData); }
};

#pragma pack (pop)
}

#endif	// XMBVIDEOPOST_H__
