/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_GELISTNODE_H__
#define C4D_GELISTNODE_H__

#include "c4d_privatechunk.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

class RootList2D;
class BaseDocument;

//----------------------------------------------------------------------------------------
/// Represents a node in a 4D list.
//----------------------------------------------------------------------------------------
class GeListNode : public PrivateChunk
{
	INSTANCEOF(GeListNode, PrivateChunk)

	friend void BaseLink::SetLink(GeListNode* bl);
	friend class BaseList2D;
	friend class RootList2D;
	friend class FieldlayerRootNode;
	friend class TakeSystemHook;

protected:
	GeAliasGoal		m_xgoal;
	UInt32				m_bits[4];
	RootList2D*		t_rootlist_parent;
	RootList2D*		m_children;
	BaseDocument*	parent_doc;

	// plugin layer data
	void*	unknownData;
	Int		unknownDataSize;
	Int32	unknownDataLevel;
	Int32	pluginID;

	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);
	virtual GeListNode* GetClone(COPYFLAGS flags, AliasTrans* trans);
	virtual Bool TranslateDescID(const DescID& id, DescID& res_id, GeListNode*& res_at);

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	GeListNode();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	/// @since 17.008
	//----------------------------------------------------------------------------------------
	~GeListNode();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// Checks if the node is a BaseList2D or BaseList4D.
	/// @return												@trueIfOtherwiseFalse{the node is a base list}
	//----------------------------------------------------------------------------------------
	virtual Bool IsBaseList();

	//----------------------------------------------------------------------------------------
	/// Gets the bit state for the node state @formatParam{bit}.
	/// @param[in] bit								Bit: ::NBIT.
	/// @return												The bit state.
	//----------------------------------------------------------------------------------------
	virtual Bool GetNBit(Int32 bit) const;

	//----------------------------------------------------------------------------------------
	/// Changes the bit state depending on the passed node state  @formatParam{bit} and the  @formatParam{bitmode}.
	/// @param[in] bit								Bit.
	/// @param[in] bitmode						Bit mode.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool ChangeNBit(NBIT bit, NBITCONTROL bitmode);

	//----------------------------------------------------------------------------------------
	/// Gets the document for the base list.
	/// @since 17.008
	/// @return												The document, or @formatConstant{nullptr} if the base list is not in a document. The pointed document owns the list head that possesses the base list.
	//----------------------------------------------------------------------------------------
	BaseDocument* GetDocument();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetPred();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetNext();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetUp();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetDown();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetDownLast();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void InsertBefore(GeListNode* op);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void InsertAfter(GeListNode* op);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void Remove();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void InsertUnder(GeListNode* op);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void InsertUnderLast(GeListNode* op);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	RootList2D* GetListHead();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32 GetNodeID(Int32 index = 0) const { return this->pluginID; }
};

//----------------------------------------------------------------------------------------
/// Gets next element in hierarchy.
/// @param[in] op									The element to get next.
/// @return												The next element to op (can be also the down) or nullptr if op is the last.
//----------------------------------------------------------------------------------------
inline GeListNode* GetNextElement(GeListNode* op)
{
	if (!op)
		return nullptr;
	if (op->GetDown())
		return op->GetDown();
	while (!op->GetNext() && op->GetUp())
		op = op->GetUp();
	return op->GetNext();
}

class GeListNodeArray : protected GeTempDynArray<GeListNode>
{
public:
	static GeListNodeArray* Alloc()						{ return NewObj(GeListNodeArray); }
	static void Free(GeListNodeArray*& obj)		{ DeleteObj(obj); }

	void Flush()															{ FlushThis(); }
	Int32 GetCount() const { return GeTempDynArray<GeListNode>::GetCount(); }
	GeListNode* GetIndex(Int32 idx) const	{ return GeTempDynArray<GeListNode>::GetIndex(idx); }
	void SetIndex(Int32 idx, GeListNode* obj)	{ GeTempDynArray<GeListNode>::SetIndex(idx, obj); }
	Bool Append(GeListNode* obj)							{ return GeTempDynArray<GeListNode>::Append(obj); }
	Bool Remove(GeListNode* obj)							{ return GeTempDynArray<GeListNode>::Remove(obj); }
	Bool Remove(Int32 idx)										{ return GeTempDynArray<GeListNode>::Remove(idx); }

	Int32 Find(GeListNode* e) const	{ return (Int32)GeTempDynArray<GeListNode>::Find(e); }
};

#pragma pack (pop)
}

#endif	// C4D_GELISTNODE_H__
