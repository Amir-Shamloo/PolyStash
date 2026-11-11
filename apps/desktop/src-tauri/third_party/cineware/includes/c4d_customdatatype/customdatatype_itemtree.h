/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_ITEMTREE_H__
#define CUSTOMDATATYPE_ITEMTREE_H__

#include "c4d_customdatatype.h"
#include "c4d_string.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDocument;
class AliasTrans;
class HyperFile;

#define CUSTOMDATA_ITEMTREE 1024869

///////////////////////////////////////////////////////////////////////////////////////////////////

#define ITEMTREE_LEVEL						0
#define ITEMTREE_DATA_TYPE				1000

#define ITEMTREE_DATA_TYPE_NONE			0
#define ITEMTREE_DATA_TYPE_JOINT		1
#define ITEMTREE_DATA_TYPE_EFFECTOR	2

#define ITEMTREE_ID_LIST_FLAG			2000
#define ITEMTREE_ID_DRAG_FLAG			2001
#define ITEMTREE_ID_HEADERS_FLAG	2002
#define ITEMTREE_ID_ROWS					2003
#define ITEMTREE_ID_CHECKBOX_FLAG	2004
#define ITEMTREE_ID_FOLDERS_FLAG	2005
#define ITEMTREE_ID_PADLOCK_FLAG	2006
#define ITEMTREE_ID_COLOR_FLAG		2007
#define ITEMTREE_ID_ICON_FLAG			2008
#define ITEMTREE_ID_MULTIPLE_FLAG	2009
#define ITEMTREE_ID_NO_RENAME			2010

#define ITEMTREE_ID_HEADERS_DATA 5000

#define ITEMTREE_OBJECT_COL	10000
#define ITEMTREE_ENABLE_COL	10001
#define ITEMTREE_LOCK_COL		10002
#define ITEMTREE_COLOR_COL	10003

#define ITEMTREE_USER_COL	50000

#define MSG_ITEMTREE_CALLBACK 1024875

#define ITEMTREE_FLAG_INSERT_BEFORE (1 << 0)
#define ITEMTREE_FLAG_INSERT_AFTER	(1 << 1)
#define ITEMTREE_FLAG_INSERT_UNDER	(1 << 2)
#define ITEMTREE_FLAG_OPEN					(1 << 3)
#define ITEMTREE_FLAG_SELECT				(1 << 4)
#define ITEMTREE_FLAG_ENABLE				(1 << 5)
#define ITEMTREE_FLAG_NOLINK				(1 << 6)
#define ITEMTREE_FLAG_NODRAG				(1 << 7)
#define ITEMTREE_FLAG_BL_NAME				(1 << 8)
#define ITEMTREE_FLAG_NO_RENAME			(1 << 9)
#define ITEMTREE_FLAG_LOCK					(1 << 10)
#define ITEMTREE_FLAG_DIRTY					(1 << 11)
#define ITEMTREE_FLAG_FOLDER				(1 << 12)
#define ITEMTREE_FLAG_NO_LOCK				(1 << 13)
#define ITEMTREE_FLAG_NO_ENABLE			(1 << 14)
#define ITEMTREE_FLAG_NO_COLOR			(1 << 15)

#define ITEMTREE_CALLBACK_DRAGACCEPT		 1
#define ITEMTREE_CALLBACK_INSERT				 2
#define ITEMTREE_CALLBACK_OPEN					 3
#define ITEMTREE_CALLBACK_CREATEMENU		 4
#define ITEMTREE_CALLBACK_CALLMENU			 5
#define ITEMTREE_CALLBACK_DELETE				 6
#define ITEMTREE_CALLBACK_SELECTIONCHG	 7
#define ITEMTREE_CALLBACK_DBLCLICK			 8
#define ITEMTREE_CALLBACK_INSERTNODE		 9
#define ITEMTREE_CALLBACK_GETCOLWIDTH		 10
#define ITEMTREE_CALLBACK_GETLINEHEIGHT	 11
#define ITEMTREE_CALLBACK_DRAWCELL			 12
#define ITEMTREE_CALLBACK_CHECKBOX			 13
#define ITEMTREE_CALLBACK_GETDRAGTYPE		 14
#define ITEMTREE_CALLBACK_CALLMENU_PASTE 15
#define ITEMTREE_CALLBACK_MOVENODE			 16
#define ITEMTREE_CALLBACK_RENAME				 17
#define ITEMTREE_CALLBACK_LOCK					 18
#define ITEMTREE_CALLBACK_DELETE_NODE		 19

#define ITEMTREE_DRAG_TYPE	 1000
#define ITEMTREE_DRAG_OBJECT 1001
#define ITEMTREE_DRAG_NODE	 1002
#define ITEMTREE_DRAG_MASK	 1003

#define ITEMTREE_MENU_COPY			 ID_TREEVIEW_FIRST_NEW_ID
#define ITEMTREE_MENU_PASTE			 (ID_TREEVIEW_FIRST_NEW_ID + 1)
#define ITEMTREE_MENU_SELECT		 (ID_TREEVIEW_FIRST_NEW_ID + 2)
#define ITEMTREE_MENU_LOCK			 (ID_TREEVIEW_FIRST_NEW_ID + 3)
#define ITEMTREE_MENU_UNLOCK		 (ID_TREEVIEW_FIRST_NEW_ID + 4)
#define ITEMTREE_MENU_UNLOCK_ALL (ID_TREEVIEW_FIRST_NEW_ID + 5)
#define ITEMTREE_MENU_NEW_FOLDER (ID_TREEVIEW_FIRST_NEW_ID + 6)
#define ITEMTREE_MENU_REPLACE		 (ID_TREEVIEW_FIRST_NEW_ID + 7)

#define ITEMTREE_MENU_CUSTOM (ID_TREEVIEW_FIRST_NEW_ID + 1000)

class ItemTreeNodeData
{
public:
	ItemTreeNodeData();
	~ItemTreeNodeData();

	const ItemTreeNodeData& operator = (const ItemTreeNodeData& s);
	ItemTreeNodeData(const ItemTreeNodeData& s);

	Bool operator !=(const ItemTreeNodeData& rhs) const;
	Bool CopyFrom(const ItemTreeNodeData& s, AliasTrans* trn);

	Bool Init();

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);

	UInt64		m_GUID;
	Int32			m_ID;
	UInt32		m_Flags;
	String		m_Text;
	GeData		m_Data;
	BaseLink* m_pLink;
};

template <class TYPE> class GeTree;

enum GETREENODE_FLAGS
{
	GETREENODE_FLAGS_ROOT = (1 << 0),

	GETREENODE_FLAGS_0		= 0
} ENUM_END_FLAGS(GETREENODE_FLAGS);

template <class TYPE> class GeTreeNode
{
	friend class GeTree<TYPE>;

public:
	GeTreeNode()
	{
		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pParent = nullptr;
		m_pChild	= nullptr;
		m_Data	= TYPE();
		m_Flags = GETREENODE_FLAGS_0;
	}
	GeTreeNode(const TYPE& data)
	{
		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pParent = nullptr;
		m_pChild	= nullptr;
		m_Data	= data;
		m_Flags = GETREENODE_FLAGS_0;
	}
	~GeTreeNode()
	{
		DebugAssert(!m_pNext && !m_pPrev && !m_pChild && !m_pParent);
	}

	GeTreeNode(const GeTreeNode<TYPE>& src)
	{
		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pParent = nullptr;
		m_pChild	= nullptr;
		m_Data	= src.m_Data;
		m_Flags = GETREENODE_FLAGS_0;
	}

	const GeTreeNode<TYPE>& operator = (const GeTreeNode<TYPE>& src)
	{
		m_Data = src.m_Data;
		return *this;
	}

	void Release()
	{
		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pParent = nullptr;
		m_pChild	= nullptr;
		m_Data = TYPE();
	}

	GeTreeNode* GetNext() const
	{
		return m_pNext;
	}
	GeTreeNode* GetPrev() const
	{
		return m_pPrev;
	}
	GeTreeNode* GetUp() const
	{
		if (m_pParent && m_pParent->m_Flags & GETREENODE_FLAGS_ROOT)
			return nullptr;
		return m_pParent;
	}
	GeTreeNode* GetDown() const
	{
		return m_pChild;
	}

	void Remove()
	{
		DebugAssert(!(m_Flags & GETREENODE_FLAGS_ROOT));
		if (m_Flags & GETREENODE_FLAGS_ROOT)
			return;

		if (m_pPrev)
		{
			m_pPrev->m_pNext = m_pNext;
			if (m_pNext)
			{
				m_pNext->m_pPrev = m_pPrev;
				m_pNext = nullptr;
			}
			m_pPrev = nullptr;
			m_pParent = nullptr;
		}
		else if (m_pNext)
		{
			m_pNext->m_pPrev = nullptr;
			if (m_pParent && m_pParent->m_pChild == this)
				m_pParent->m_pChild = m_pNext;
			m_pNext = nullptr;
			m_pParent = nullptr;
		}
		else if (m_pChild)
		{
			m_pChild->m_pParent = m_pParent;
			if (m_pParent && m_pParent->m_pChild == this)
				m_pParent->m_pChild = m_pChild;
			m_pChild	= nullptr;
			m_pParent = nullptr;
		}
		else if (m_pParent)
		{
			m_pParent->m_pChild = nullptr;
			m_pParent = nullptr;
		}
	}

	void Insert(GeTreeNode* pParent, GeTreeNode* pPrev)
	{
		if (pPrev)
		{
			if (pPrev->m_pNext)
				pPrev->m_pNext->m_pPrev = this;
			m_pNext = pPrev->m_pNext;
			pPrev->m_pNext = this;
			m_pPrev = pPrev;
			m_pParent = pPrev->m_pParent;
		}
		else if (pParent)
		{
			if (pParent->m_pChild)
			{
				GeTreeNode* pNext = pParent->m_pChild;
				while (pNext->m_pNext)
				{
					pNext = pNext->m_pNext;
				}

				pNext->m_pNext = this;
				m_pPrev = pNext;
			}
			else
			{
				pParent->m_pChild = this;
			}

			m_pParent = pParent;
		}
	}

	GeTreeNode* GetFirst()
	{
		GeTreeNode* top = this;
		while (top->m_pParent)
		{
			top = top->m_pParent;
		}
		DebugAssert(top->m_Flags & GETREENODE_FLAGS_ROOT);
		return top->m_pChild;
	}

	GeTreeNode* GetRoot()
	{
		GeTreeNode* top = this;
		while (top->m_pParent)
		{
			top = top->m_pParent;
		}
		DebugAssert(top->m_Flags & GETREENODE_FLAGS_ROOT);
		return top;
	}

	TYPE m_Data;

private:
	GETREENODE_FLAGS m_Flags;	// NOTE: Node flags are NOT copied!
	GeTreeNode*			 m_pNext, *m_pPrev, *m_pParent, *m_pChild;
};

template <class TYPE> class GeTree
{
private:
	GeTreeNode<TYPE>* _Find(const TYPE& data, GeTreeNode<TYPE>* node, Bool (* fn)(const TYPE& src, const TYPE& dst))
	{
		while (node)
		{
			if (fn(node->m_Data, data))
				return node;

			GeTreeNode<TYPE>* rnode = _Find(data, node->GetDown(), fn);
			if (rnode)
				return rnode;

			node = node->GetNext();
		}

		return nullptr;
	}

	GeTreeNode<TYPE>* _Find(const TYPE& data, GeTreeNode<TYPE>* node)
	{
		while (node)
		{
			if (node->m_Data == data)
				return node;

			GeTreeNode<TYPE>* rnode = _Find(data, node->GetDown());
			if (rnode)
				return rnode;

			node = node->GetNext();
		}

		return nullptr;
	}

	GeTree(const GeTree& src)
	{
		m_pAvailable = nullptr;
	}

	GeTree& operator =(const GeTree& src)
	{
		return *this;
	}

	void _GetChildCount(GeTreeNode<TYPE>* node, Int32& cnt)
	{
		while (node)
		{
			cnt++;
			_GetChildCount(node->GetDown(), cnt);
			node = node->GetNext();
		}
	}

	void _FillArray(TYPE* data, GeTreeNode<TYPE>* node, Int32& index)
	{
		while (node)
		{
			data[index++] = node->m_Data;
			_FillArray(data, node->GetDown(), index);
			node = node->GetNext();
		}
	}

	void _FillDataArray(TYPE** data, GeTreeNode<TYPE>* node, Int32& index)
	{
		while (node)
		{
			data[index++] = &node->m_Data;
			_FillDataArray(data, node->GetDown(), index);
			node = node->GetNext();
		}
	}

#ifdef MAXON_TARGET_DEBUG
	Bool _FindNode(GeTreeNode<TYPE>* fnode, GeTreeNode<TYPE>* node)
	{
		while (node)
		{
			if (node == fnode)
				return true;
			if (_FindNode(fnode, node->GetDown()))
				return true;
			node = node->GetNext();
		}
		return false;
	}
#endif

public:
	GeTree()
	{
		m_pAvailable = nullptr;
		m_Root.m_Flags = GETREENODE_FLAGS_ROOT;
	}

	~GeTree()
	{
		Free();
	}

	void Destruct(GeTreeNode<TYPE>* node)
	{
		while (node)
		{
			node->m_Data.~TYPE();
			Destruct(node->GetDown());
			node = node->GetNext();
		}
	}

	void Free()
	{
		Destruct(&m_Root);

		m_Root.Release();
		m_pAvailable = nullptr;
	}

	Bool Init()
	{
		Free();
		m_Root.m_Flags = GETREENODE_FLAGS_ROOT;
		return true;
	}

	GeTreeNode<TYPE>* Alloc(Bool insert = false)
	{
		GeTreeNode<TYPE>* node;

		if (m_pAvailable)
		{
			node = m_pAvailable;
			m_pAvailable	= node->m_pNext;
			node->m_pNext = nullptr;
		}
		else
		{
			node = (GeTreeNode<TYPE>*)NewObj(GeTreeNode<TYPE>);
		}

		if (node)
		{
			node->m_Data = TYPE();
			if (insert)
				node->Insert(&m_Root, nullptr);
		}

		return node;
	}

	GeTreeNode<TYPE>* Add(const TYPE& data, Bool insert = false)
	{
		GeTreeNode<TYPE>* node = Alloc(insert);
		if (node)
			node->m_Data = data;
		return node;
	}

	GeTreeNode<TYPE>* Insert(const TYPE& data, GeTreeNode<TYPE>* parent, GeTreeNode<TYPE>* prev)
	{
		GeTreeNode<TYPE>* node = Add(data);
		if (node)
			Insert(node, parent, prev);
		return node;
	}

	void Insert(GeTreeNode<TYPE>* node, GeTreeNode<TYPE>* parent, GeTreeNode<TYPE>* prev)
	{
		if (!parent)
			parent = &m_Root;
		node->Insert(parent, prev);
	}

	void InsertLast(GeTreeNode<TYPE>* node, GeTreeNode<TYPE>* parent)
	{
		if (!parent)
			parent = &m_Root;

		if (parent->GetDown())
		{
			parent = parent->GetDown();

			while (parent->GetNext())
			{
				parent = parent->GetNext();
			}
			node->Insert(nullptr, parent);
		}
		else
		{
			node->Insert(parent, nullptr);
		}
	}

	void Remove(GeTreeNode<TYPE>* node)
	{
		node->Remove();
	}

	GeTreeNode<TYPE>* GetRoot()
	{
		return &m_Root;
	}
	const GeTreeNode<TYPE>* GetCRoot() const
	{
		return &m_Root;
	}

	void Free(GeTreeNode<TYPE>*& node)
	{
		if (!node)
			return;

		DebugAssert(!(node->m_Flags & GETREENODE_FLAGS_ROOT));
		if (node->m_Flags & GETREENODE_FLAGS_ROOT)
			return;

		DebugAssert(_FindNode(node, m_Root.GetDown()));

		while (node->GetDown())
		{
			GeTreeNode<TYPE>* cnode = node->GetDown();
			Free(cnode);
		}

		node->Remove();
		node->m_pNext = m_pAvailable;
		node->m_Data.~TYPE();
		m_pAvailable = node;
		node = nullptr;
	}

	Bool IsEmpty()
	{
		return m_Root.GetDown() == nullptr;
	}

	GeTreeNode<TYPE>* Find(const TYPE& data, Bool (* fn)(const TYPE& src, const TYPE& dst))
	{
		return _Find(data, m_Root.GetDown(), fn);
	}

	GeTreeNode<TYPE>* Find(const TYPE& data)
	{
		return _Find(data, m_Root.GetDown());
	}

	void Move(GeTreeNode<TYPE>* node, GeTreeNode<TYPE>* parent, GeTreeNode<TYPE>* prev)
	{
		node->Remove();
		Insert(node, parent, prev);
	}

	Int32 GetChildCount(GeTreeNode<TYPE>* root = nullptr)
	{
		Int32 cnt = 0;
		if (!root)
			root = m_Root.GetDown();
		else
			root = root->GetDown();
		_GetChildCount(root, cnt);
		return cnt;
	}

	void FillArray(TYPE* data, GeTreeNode<TYPE>* root = nullptr)
	{
		Int32 index = 0;
		if (!root)
			root = m_Root.GetDown();
		else
			root = root->GetDown();
		_FillArray(data, root, index);
	}

	void FillDataArray(TYPE** data, GeTreeNode<TYPE>* root = nullptr)
	{
		Int32 index = 0;
		if (!root)
			root = m_Root.GetDown();
		else
			root = root->GetDown();
		_FillDataArray(data, root, index);
	}

	static Bool IsEqualTree(const GeTreeNode<TYPE>* src, const GeTreeNode<TYPE>* dst)
	{
		while (src && dst)
		{
			if (!IsEqualTree(src->GetDown(), dst->GetDown()))
				return false;

			src = src->GetNext();
			dst = dst->GetNext();
		}

		return src == nullptr && dst == nullptr;
	}

	static Bool IsEqual(const GeTreeNode<TYPE>* src, const GeTreeNode<TYPE>* dst)
	{
		while (src && dst)
		{
			if (src->m_Data != dst->m_Data)
				return false;

			if (!IsEqual(src->GetDown(), dst->GetDown()))
				return false;

			src = src->GetNext();
			dst = dst->GetNext();
		}

		return src == nullptr && dst == nullptr;
	}

	Bool CopyTo(GeTree<TYPE>* dsttree, const GeTreeNode<TYPE>* src = nullptr, GeTreeNode<TYPE>* dst = nullptr, Bool downonly = true) const
	{
		if (!dst)
		{
			if (!dsttree->Init())
				return false;
			return CopyTo(dsttree, &m_Root, &dsttree->m_Root, true);
		}

		while (src)
		{
			dst->m_Data = src->m_Data;

			if (src->GetDown())
			{
				GeTreeNode<TYPE>* dnode = dsttree->Alloc();
				if (!dnode)
					return false;

				dsttree->Insert(dnode, dst, nullptr);

				if (!CopyTo(dsttree, src->GetDown(), dnode, false))
					return false;
			}

			if (downonly)
				break;

			src = src->GetNext();
			if (src)
			{
				GeTreeNode<TYPE>* dnode = dsttree->Alloc();
				if (!dnode)
					return false;

				dsttree->Insert(dnode, nullptr, dst);

				dst = dnode;
			}
		}

		return true;
	}

private:
	GeTreeNode<TYPE>	m_Root;
	GeTreeNode<TYPE>* m_pAvailable;
};

class ItemTreeNode;

class ItemTreeNodeIndex
{
public:
	ItemTreeNodeIndex()
	{
		node = nullptr;
		data = nullptr;
		m_GUID	= 0;
		m_Index = NOTOK;
	}
	ItemTreeNodeIndex(UInt64 guid)
	{
		node = nullptr;
		data = nullptr;
		m_GUID	= guid;
		m_Index = NOTOK;
	}

	Bool operator <(const ItemTreeNodeIndex& dst) const
	{
		return m_GUID < dst.m_GUID;
	}
	Bool operator >(const ItemTreeNodeIndex& dst) const
	{
		return m_GUID > dst.m_GUID;
	}
	Bool operator <=(const ItemTreeNodeIndex& dst) const
	{
		return m_GUID <= dst.m_GUID;
	}
	Bool operator >=(const ItemTreeNodeIndex& dst) const
	{
		return m_GUID >= dst.m_GUID;
	}
	Bool operator ==(const ItemTreeNodeIndex& dst) const
	{
		return m_GUID == dst.m_GUID;
	}

	Int32							m_Index;
	UInt64						m_GUID;
	ItemTreeNode*			node;
	ItemTreeNodeData* data;
};

class ItemTreeData : public CustomDataType
{
private:
	ItemTreeData();
	~ItemTreeData();

public:
	static ItemTreeData* Alloc();
	static void Free(ItemTreeData*& data);

	ItemTreeNode* AddItem(Int32 id, const String& name, BaseList2D* bl, const GeData& data, UInt32 flags);
	ItemTreeNode* AddNode(Int32 id, const String& name, BaseList2D* bl, const GeData& data, UInt32 flags, ItemTreeNode* parent, ItemTreeNode* prev);
	ItemTreeNodeData* GetNodeData(ItemTreeNode* node);

	ItemTreeNode* GetNext(ItemTreeNode* node);
	ItemTreeNode* GetPrev(ItemTreeNode* node);
	ItemTreeNode* GetUp(ItemTreeNode* node);
	ItemTreeNode* GetDown(ItemTreeNode* node);
	ItemTreeNode* GetRoot();
	ItemTreeNode* GetFirst();

	void Remove(ItemTreeNode* node);
	void FreeNode(ItemTreeNode*& node);
	void Insert(ItemTreeNode* node, ItemTreeNode* parent, ItemTreeNode* prev);

	void SetOwner(BaseList2D* bl);
	BaseLink* GetOwnerLink();
	BaseContainer* GetUserData();
	Int32 GetSelected(ItemTreeNode*** table);

	ItemTreeNode* Find(UInt64 guid, ItemTreeNode* node = nullptr);

	void ClearSelection();
	void SetSelectedID(UInt64 guid);
	void SetSelected(ItemTreeNode* node);
	ItemTreeNode* GetFirstSelected();

	void Reset();
	ItemTreeNodeIndex* BuildIndexTable(Int32& tcnt, Bool sorted);
};

/// @markInternal
class ItemTreeDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType*	AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData (CustomDataType* d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMDATATYPE_ITEMTREE_H__
