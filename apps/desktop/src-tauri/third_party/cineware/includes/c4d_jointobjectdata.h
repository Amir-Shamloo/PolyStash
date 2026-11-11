/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_JOINTOBJECTDATA_H__
#define C4D_JOINTOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_cajoint.h"

namespace cineware
{
#pragma pack (push, 8)

struct CPolygon;

/// @addtogroup JOINTEXECUTE
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define JOINTEXECUTE_CLEAN			 0
#define JOINTEXECUTE_SELFDIRTY	 1
#define JOINTEXECUTE_MIRRORDIRTY 2
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class JointObjectData : public NodeData
{
	INSTANCEOF(JointObjectData, NodeData)

public:
	JointObjectData();
	virtual ~JointObjectData();

	virtual Bool Init();
	virtual void Free();

	virtual Int32 GetChunkLevel();
	virtual Int32 GetDiskType() const;

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	//virtual void GetDimension(BaseObject* op, Vector* mp, Vector* rad);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	Bool UpdatePolygons(BaseObject* op);

	static NodeData* Alloc()
	{
		return NewObj(JointObjectData);
	}

	Int32			m_iLevel;

	BaseLink*	m_LastMirror;

	UInt32		m_ParamDirty;
	UInt32		m_lPolyCRC;
	Int32			m_PolyMode;

	Vector*		m_pPointBk;
	CPolygon*	m_pPolyBk;

	Vector*		m_pPointOrigin;
	Int32			m_lPointOriginCount;
	UInt32		m_lPolyOriginCRC;

	Matrix		m_PolyMg;
	Float			m_PolyLen;

	Int32			m_lPointCount;
	Int32			m_lPolyCount;

	Int32			m_ExecuteState;

	caJoint		m_Joint;

	BaseLink*	m_pCharBase;	// Authoring base.
};

#pragma pack (pop)
}

#endif	// C4D_JOINTOBJECTDATA_H__
