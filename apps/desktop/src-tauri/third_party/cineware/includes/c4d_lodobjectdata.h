/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LODOBJECTDATA_H__
#define C4D_LODOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_customdatatype/customgui_range.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's LOD object.
/// @since R19
//----------------------------------------------------------------------------------------
class LodObjectData : public NodeData
{
	INSTANCEOF(LodObjectData, NodeData)

	friend class BaseObject;

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	LodObjectData();

	//----------------------------------------------------------------------------------------
	/// Destructor.
	//----------------------------------------------------------------------------------------
	~LodObjectData();

	/// @}

	/// @name Level
	/// @{

	//----------------------------------------------------------------------------------------
	/// Queries the current LOD level.
	/// @return												The current LOD level index, or ::NOTOK in error case.
	//----------------------------------------------------------------------------------------
	Int32 GetCurrentLevel();

	//----------------------------------------------------------------------------------------
	/// Queries the current number of LOD levels.
	/// @return												The current LOD level count, or ::NOTOK in error case.
	//----------------------------------------------------------------------------------------
	Int32 GetLevelCount();

	/// @}

protected:
	static NodeData* Alloc();

private:
	virtual Bool Init();
	virtual void Free();

	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);
};

#pragma pack (pop)
}

#endif	// C4D_LODOBJECTDATA_H__
