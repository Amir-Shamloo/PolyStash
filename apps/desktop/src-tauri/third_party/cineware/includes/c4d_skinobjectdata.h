/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SKINOBJECTDATA_H__
#define C4D_SKINOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "private_safebaseptr.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class SkinMeshPtr : public SafeBasePtr
{
public:
	SkinMeshPtr();
	~SkinMeshPtr();

	Vector* m_pPoints;
	Int32		m_Count;
};


/// @addtogroup SKINOBJECT_FLAG
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define SKINOBJECT_FLAG_CLONED	 (1 << 0)
#define SKINOBJECT_FLAG_IMPORTED (1 << 1)
/// @}

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Skin deformer object.
//----------------------------------------------------------------------------------------
class SkinObjectData : public NodeData
{
	INSTANCEOF(SkinObjectData, NodeData)

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	SkinObjectData();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~SkinObjectData();

	/// @}

	/// @name Alloc
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{skin object node data}
	/// @return												@allocReturn{skin object node data}
	//----------------------------------------------------------------------------------------
	static NodeData* Alloc();

	/// @}

	/// @name Init
	/// @{

	//----------------------------------------------------------------------------------------
	/// Called to initialize an instance of a skin object data node.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Init();

	/// @}

	//virtual Bool ModifyObject(BaseObject* mod, BaseDocument* doc, BaseObject* op, const Matrix& op_mg, const Matrix& mod_mg, Float lod, Int32 flags, BaseThread* thread);
	//Bool _ModifyObject(BaseObject* mod, BaseDocument* doc, BaseObject* op, const Matrix& op_mg, const Matrix& mod_mg, Float lod, Int32 flags, BaseThread* thread, SortedArray<BaseTag*,8> &itags, Int32 itype);
	//Bool __ModifyObject(BaseTag *wtag, BaseObject* mod, BaseDocument* doc, BaseObject* op, const Matrix& op_mg, const Matrix& mod_mg, Float lod, Int32 flags, BaseThread* thread);

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetChunkLevel();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	UInt32											 m_Flags;		///< @markPrivate
	UInt32											 m_lDirty;	///< @markPrivate
	DynamicArray<SkinMeshPtr, 8> m_Objects;	///< @markPrivate

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_SKINOBJECTDATA_H__
