/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SKINJOINT_H__
#define C4D_SKINJOINT_H__

#include "c4d_jointweight.h"
#include "c4d_effectorptr.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup SKINJOINT_FLAG
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define SKINJOINT_FLAG_SELECTED	(1 << 0)
#define SKINJOINT_FLAG_LOCAL		(1 << 1)
#define SKINJOINT_FLAG_CONVERT	(1 << 2)
/// @}

#define CA_JOINT_OBJECT_ID Ojoint	///< @markPrivate

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class SkinJoint
{
public:
	SkinJoint();
	~SkinJoint();

	BaseObject* Get(BaseDocument* pDoc, Int32 base = 0) const;
	Bool Set(BaseObject* op);

	SkinJoint(const SkinJoint& src);
	SkinJoint& operator =(const SkinJoint& src);

	Bool CopyFrom(const SkinJoint& src, AliasTrans* trans);

	Bool Write(HyperFile* hf);
	Bool WriteEx(HyperFile* hf);
	Bool Read(HyperFile* hf, Int32 level);
	Bool ReadEx(HyperFile* hf, Int32 level);

	BaseObject* GetBone(BaseDocument* doc, Matrix& m, Float& len, Bool skipnull = false, DynamicArray<SkinJoint, 8>* bindstate = nullptr);
	//BaseObject *GetMg(BaseDocument *doc, Matrix &m);
	Matrix GetMg(BaseObject* op, DynamicArray<SkinJoint, 8>* bindstate);
	static Int32 FindJoint(BaseObject* op, DynamicArray<SkinJoint, 8>* bindstate);

	//void GetWeights(WeightTagData *wtag, SReal *weights, Int32 pcnt);
	//void GetEffectorWeights(WeightTagData *wtag, SReal *weights, const Vector *padr, Int32 pcnt, const Matrix &mg, BaseDocument *doc);

	//void InitEffectors(WeightTagData *wtag, BaseDocument *doc, const Matrix &mg);
	//Float GetEffectorWeight(const Vector &p);
	//void FreeEffectors();

	BaseLink*										 m_pLink;
	BaseLink*										 m_pMapLink;

	Matrix											 m_oM, m_iM;		// Bone matrices.
	Matrix											 m_oMg, m_iMg;	// Object matrices.
	Float												 m_oLen;

	JointWeight									 m_Weight;
	UInt32											 m_lDirty;

	Int32												 m_Flags;

	SelectBitArray							 m_Lock;

	DynamicArray<EffectorPtr, 8> m_Effectors;
};

#pragma pack (pop)
}

#endif	// C4D_SKINJOINT_H__
