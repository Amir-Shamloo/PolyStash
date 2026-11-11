/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_MESHDEFORMEROBJECTDATA_H__
#define C4D_MESHDEFORMEROBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_misc/datastructures/basearray.h"
#include "private_safebaseptr.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------

class CageWeight
{
public:
	Int32		m_Index;
	Float32 m_W;
};

class CageWeights
{
public:
	CageWeights() { m_pData = nullptr; m_Count = 0; }
	~CageWeights() { Free(); }

	void Free();
	Bool SetWeights(Float* weights, Int32 count);
	Vector CalculateP(const Vector* padr, Int32 count);
	Int64 GetMemoryUsed()
	{
		return sizeof(CageWeight) * m_Count;
	}

	CageWeight* m_pData;
	Int32				m_Count;
};

class CageSurfaceWeights
{
public:
	Int32		m_pIndex, m_Index;
	Float32 m_S, m_T, m_R;
	Float		m_D;
	Vector	m_P;
};

class MeshDeformerDirtyData
{
public:
	MeshDeformerDirtyData() { m_LastDirty = 0; m_DirtyCRC = CRC64_NEGL; }
	~MeshDeformerDirtyData() { }

	SafeBasePtr m_Object;
	Int32				m_LastDirty;
	UInt64			m_DirtyCRC;
};

class MeshDeformerData
{
public:
	MeshDeformerData() { m_Count = 0; m_pSurfData = nullptr; m_sCount = 0; m_FitScale = 1.0; }
	~MeshDeformerData() { Free(); }

	void Free()
	{
		m_pWeightData.Flush();
		DeleteMem(m_pSurfData);
		m_sCount = 0;
		m_Count	 = 0;
		m_FitScale = 1.0;
	}

	Float								m_FitScale;

	CageSurfaceWeights* m_pSurfData;
	Int32								m_sCount;

	BaseArray<CageWeights>	m_pWeightData;
	Int32								m_Count;
};

class MeshDeformerCageData
{
public:
	MeshDeformerCageData()
	{
		m_pPoints = nullptr;
		m_Count = 0;
	}
	~MeshDeformerCageData()
	{
		DeleteMem(m_pPoints);
	}

	void Scale(Float scale)
	{
		Int32 i;
		if (!m_pPoints)
			return;
		for (i = 0; i < m_Count; i++)
		{
			m_pPoints[i] *= scale;
		}
	}

	Int32															m_LastDirty;
	SafeBasePtr												m_Cage;
	DynamicArray<MeshDeformerData, 8> m_Data;

	Vector*														m_pPoints;
	Int32															m_Count;
};

class MeshDeformerObjectData : public NodeData
{
	INSTANCEOF(MeshDeformerObjectData, NodeData)

public:
	MeshDeformerObjectData();
	virtual ~MeshDeformerObjectData();

	virtual Bool Init();
	virtual void Free();

	virtual Int32 GetChunkLevel();

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	static NodeData* Alloc()
	{
		return NewObj(MeshDeformerObjectData);
	}

	Bool InitCages(BaseObject* op, BaseDocument* doc, BaseContainer* bc, Bool addundo, Bool autoinit);

public:
	DynamicArray<MeshDeformerDirtyData, 8> m_Objects;
	DynamicArray<MeshDeformerCageData, 8>	 m_Cages;

	UInt32			 m_Flags;

	Int32				 FalloffDirty;
	Int32				 m_LastDirty;
};

#pragma pack (pop)
}

#endif	// C4D_MESHDEFORMEROBJECTDATA_H__
