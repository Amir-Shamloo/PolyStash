/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_HNWEIGHTTAGDATA_H__
#define C4D_HNWEIGHTTAGDATA_H__

#include "c4d_nodedata.h"
#include "c4d_hndata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's HyperNURBS weight tag.
/// @since R19
//----------------------------------------------------------------------------------------
class HNWeightTagData : public NodeData
{
	INSTANCEOF(HNWeightTagData, NodeData)

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{HyperNURBS weight tag}
	/// @return												@allocReturn{HyperNURBS weight tag}
	//----------------------------------------------------------------------------------------
	static HNWeightTagData* Alloc();

	/// @}

	/// @name Get Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Fills the passed struct with the weight tag data.
	/// @note Remember to set HNData::changed if anything has changed.
	/// @return												@trueIfOtherwiseFalse{the weight tag data was retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetTagData(HNData* data);

	/// @}

private:
	virtual Int32 GetChunkLevel();

	HNWeightTagData();
	~HNWeightTagData();

	virtual Bool Init();
	virtual void Free();

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	Int32				lPoints, lPolys;
	Float32*		prPointWeight;
	PolyWeight*	pPolyWeights;
	Bool				m_bUpdateWeightTag;
};

#pragma pack (pop)
}

#endif	// C4D_HNWEIGHTTAGDATA_H__
