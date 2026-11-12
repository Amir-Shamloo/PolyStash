/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CLOUDDATA_H__
#define C4D_CLOUDDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class SplineData;

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Cloud object.
//----------------------------------------------------------------------------------------
class CloudData : public NodeData
{
	INSTANCEOF(CloudData, NodeData)

private:
	UChar*			m_pchDensity;
	Bool				m_bIsSelected;
	Int32				m_lSizeX, m_lSizeY, m_lSizeZ;
	Int					m_lDenstitySize;
	Int32				m_lPlaneIndex;
	Int32				m_lPlanePos;
	SplineData*	pSplineData;

	virtual Bool Init();
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	Bool ChangeSize(Int32 lSizeX, Int32 lSizeY, Int32 lSizeZ, Bool bResample);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	CloudData();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~CloudData();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the Cloud object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The ID of the node to read.
	/// @param[in] level							The disklevel of the node to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the Cloud object to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_CLOUDDATA_H__
