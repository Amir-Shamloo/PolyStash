/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CAJOINT_H__
#define C4D_CAJOINT_H__

#include "ge_math.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class SkinJoint;
class JointObjectData;
class PolygonObject;
class BaseObject;
class BaseContainer;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class caJoint
{
public:
	caJoint();
	~caJoint();

	void CreateBone_Standard(PolygonObject* pPoly, const Matrix& m, Float len);
	void CreateBone_Box(PolygonObject* pPoly, const Matrix& m, Float len);
	void CreatePolygon(PolygonObject* pPoly, Int32 type);
	void CreateBone(PolygonObject* pPoly, const Matrix& m, Float len, Int32 type);

	void GetBone(Int32 align, Int32 axis, Matrix& m, Float& len, DynamicArray<SkinJoint, 8>* bindstate = nullptr);
	static void GetChildBone(BaseObject* pChild, Matrix& m, Float& len, Int32 axis, const Matrix& mg, DynamicArray<SkinJoint, 8>* bindstate = nullptr);
	static void AlignZMatrix(Matrix& m, Int32 axis, const Vector& z);

	Float GetLength();
	void SetLength(Float len);
	void Align();

	Float GetBoneSize(Float len);
	Float GetJointSize(Float len);

	static Matrix GetMg(BaseObject* op, DynamicArray<SkinJoint, 8>* bindstate);

	JointObjectData* GetPrev();
	JointObjectData* GetNext();
	JointObjectData* GetDown();
	JointObjectData* GetUp();

	BaseObject* GetPrevOp();
	BaseObject* GetNextOp();
	BaseObject* GetDownOp();
	BaseObject* GetUpOp();

	BaseObject*		 m_pJointObject;
	BaseContainer* m_pData;
};

#pragma pack (pop)
}

#endif	// C4D_CAJOINT_H__
