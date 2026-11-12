/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CLOUDGROUPDATA_H__
#define C4D_CLOUDGROUPDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Cloud Group object.
//----------------------------------------------------------------------------------------
class CloudGroupData : public NodeData
{
	INSTANCEOF(CloudGroupData, NodeData)

private:
	Bool m_bIsSelected;

	Bool Init();
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	CloudGroupData();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~CloudGroupData();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the Cloud Group object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The ID of the node to read.
	/// @param[in] level							The disklevel of the node to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the Cloud Group object to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_CLOUDGROUPDATA_H__
