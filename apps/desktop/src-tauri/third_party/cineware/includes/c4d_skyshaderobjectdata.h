/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SKYSHADEROBJECTDATA_H__
#define C4D_SKYSHADEROBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

class SplineData;

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Physical Sky object.
//----------------------------------------------------------------------------------------
class SkyShaderObjectData : public NodeData
{
	INSTANCEOF(SkyShaderObjectData, NodeData)

	friend class BaseDocument;

private:
	BaseLink		m_MaterialLink, m_VolumeMatLink;
	SplineData* pSplineData;

	virtual Bool Init();
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
	virtual Int32 GetChunkLevel();

	void GetMaterialParameter(Int32 lID, GeListNode* pNode, GeData& d);
	void SetMaterialParameter(Int32 lID, GeListNode* pNode, const GeData& d);

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	SkyShaderObjectData();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~SkyShaderObjectData();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the Sky object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The ID of the node to read.
	/// @param[in] level							The disklevel of the node to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the Sky object to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_SKYSHADEROBJECTDATA_H__
