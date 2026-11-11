/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef POINTGENERATOR_H__
#define POINTGENERATOR_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Point Source Distribution of the Fracture Voronoi object.
/// @since R20
//----------------------------------------------------------------------------------------
class PointGenerator : public NodeData
{
	INSTANCEOF(PointGenerator, NodeData)

public:
	PointGenerator();
	~PointGenerator();

	static NodeData*	Alloc();
	virtual Bool			Init();

private:
	virtual Int32			GetChunkLevel();
	virtual Bool			Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool			Write(HyperFile* hf);
	virtual Bool			CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);
	virtual Bool			SetDParameter(const DescID& id, const GeData& t_data);
	virtual Bool			GetDParameter(const DescID& id, GeData& t_data);

	Int32							_generatorType;
};

#pragma pack (pop)
}

#endif // POINTGENERATOR_H__
