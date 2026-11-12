/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_OPERATORDATA_H__
#define C4D_OPERATORDATA_H__

#include "c4d_nodedata.h"
#include "c4d_descid.h"
#include "gv_port.h"

namespace cineware
{
#pragma pack (push, 8)

#define GVpython 1022471

class GvRealNode;

//----------------------------------------------------------------------------------------
/// Placeholder class to read/write/copy XPresso nodes.
/// @since R20
//----------------------------------------------------------------------------------------
class GvOperatorData : public NodeData
{
	public:
		Int32 _operatorPluginID;

		GvOperatorData();
		GvOperatorData(Int32 id);

		virtual Bool Init();
		virtual Bool Read(HyperFile *hf, Int32 id, Int32 level);
};

#pragma pack (pop)
}

#endif // C4D_OPERATORDATA_H__
