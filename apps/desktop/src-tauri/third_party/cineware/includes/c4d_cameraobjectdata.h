/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CAMERAOBJECTDATA_H__
#define C4D_CAMERAOBJECTDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Camera object.
//----------------------------------------------------------------------------------------
class CameraObjectData : public NodeData
{
	INSTANCEOF(CameraObjectData, NodeData)

private:
	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

public:
	//----------------------------------------------------------------------------------------
	/// Called to initialize an instance of a camera object data node.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Init();
};

#pragma pack (pop)
}

#endif	// C4D_CAMERAOBJECTDATA_H__
