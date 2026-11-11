/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEOVERRIDEDATA_H__
#define C4D_BASEOVERRIDEDATA_H__

#include "c4d_nodedata.h"
#include "c4d_descid.h"
#include "c4d_misc/datastructures/sort.h"
#include "c4d_baselinkarray.h"

namespace cineware
{
#pragma pack (push, 8)

class SubOverride;
class BaseOverrideGroup;
class BaseTake;
class BaseOverride;

static const Int32 ID_TAKE_PROGRESSIVEINDEX = 5000;	// a container id for take progressive indexing in take hook
static const Int32 ID_HOOK_ALLOW_MESSAGE = 5001;		// Bool if true the system will update all new tags added to an overridegroup

template <typename FN> void ForEachTake(BaseDocument* doc, FN&& src);


// helper struct to use BaseArrays instead of HashMaps
struct OverrideMapEntry
{
	OverrideMapEntry(DescID inKey, SubOverride* inEntry) : _key(inKey), _entry(inEntry)
	{
	}

	OverrideMapEntry(const OverrideMapEntry& src) : _key(src._key), _entry(src._entry)
	{
	}

	OverrideMapEntry(OverrideMapEntry&& src) : _key(std::move(src._key)), _entry(src._entry)
	{
		src._entry = nullptr;
	}

	DescID			 _key;
	SubOverride* _entry;
};

//----------------------------------------------------------------------------------------
// A node in the document and overridden parameters
// work in take hierarchy so can be used by main take
// to know what is already reverted to the correct status
//----------------------------------------------------------------------------------------
struct NodeChange
{
	DISALLOW_COPY_AND_ASSIGN(NodeChange)

public:
	NodeChange(BaseList2D* node = nullptr)
	{
		_node = node;
		_changed.Reset();
	}

	~NodeChange()
	{
		_node = nullptr;
		_changed.Reset();
	}

	//----------------------------------------------------------------------------------------
	// Move semantics
	//----------------------------------------------------------------------------------------
	NodeChange(NodeChange&& src)
	{
		_node = src._node;
		_changed = std::move(src._changed);
	}

	MOVE_ASSIGNMENT_OPERATOR(NodeChange)

	//  maxon::MemoryBool CopyFrom(const NodeChange& src)
	//  {
	//    _node = src._node;
	//    iferr(_changed.CopyFrom(src._changed))
	//      return maxon::MemoryBool(false);
	//    return maxon::MemoryBool(true);
	//  }

	BaseList2D * _node;										// the node in the document
	BaseArray<OverrideMapEntry>	_changed;	// a map of parameters already changed
};

//----------------------------------------------------------------------------------------
/// A compact structure to override a parameter for a specific node in a Take.
//----------------------------------------------------------------------------------------
struct OverrideDataSet
{
public:
	OverrideDataSet(BaseDocument* doc = nullptr, BaseList2D* node = nullptr, BaseTake* take = nullptr, const DescID& did = DescID(), const GeData& data = GeData(), const GeData& oldData = GeData(), Bool deleteAnim = false, BaseList2D* parent = nullptr) : _doc(doc), _node(node), _take(take), _id(did), _data(data), _oldData(oldData), _deleteAnim(deleteAnim), _parent(parent), _overrideNode(nullptr), _desc(nullptr) { }

	BaseDocument* _doc;						// The document for _take.
	BaseList2D*		_node;					// The node to override.
	BaseTake*			_take;					// The take to where to override the param.
	DescID				_id;						// The DescID for the param to override
	GeData				_data;					// The override value.
	GeData				_oldData;				// The override value to set in the backup take.
	Bool					_deleteAnim;		// if true and the original parameter is animated animation will not be copied in the destination override.
	BaseList2D*		_parent;				// the parent override if exist or nullptr
	BaseOverride* _overrideNode;	// if the override is already created this is !nullptr, to speedup the creation process
	void*					_desc;					// if the override is already created this is !nullptr, to speedup the creation process
};

//----------------------------------------------------------------------------------------
/// A class to store override Description id and additional data need to be save to the disk with override
//----------------------------------------------------------------------------------------
class OverrideEntry
{
public:
	OverrideEntry(const DescID& did = DescID()) : _descID(did), _bits(0), _parent(DescID()) { }

	OverrideEntry(const DescID& did, const DescID& parent)
	{
		_descID = did;
		_bits = 0;
		_parent = parent;
	}

	Bool Read(HyperFile* hf, Int32 level)
	{
		if (!_descID.Read(hf))
			return false;
		if (!hf->ReadInt32(&_bits))
			return false;
		if (level >= 1)
		{
			if (level < 3)
			{
				Int32 oldParent;
				if (!hf->ReadInt32(&oldParent))
					return false;
				_parent = DescID(oldParent);
			}
			else
			{
				if (!_parent.Read(hf))
					return false;
			}
		}
		return true;
	}

	Bool Write(HyperFile* hf)
	{
		if (!_descID.Write(hf))
			return false;
		if (!hf->WriteInt32(_bits))
			return false;
		if (!_parent.Write(hf))
			return false;
		return true;
	}

	Bool CopyTo(OverrideEntry& dest)
	{
		dest._descID = _descID;
		dest._bits = _bits;
		dest._parent = _parent;
		return true;
	}

	DescID _descID;	// The descID to be overridden.
	Int32	 _bits;		// additional bits (see BaseList2D Bits) EG GUI folding and selection.
	DescID _parent;	// optional parent group.
};

//----------------------------------------------------------------------------------------
// A list of overridden description data
//----------------------------------------------------------------------------------------
class SubOverride
{
	DISALLOW_COPY_AND_ASSIGN(SubOverride)

public:
	SubOverride() { }

	~SubOverride() { }

	//----------------------------------------------------------------------------------------
	// Move semantics
	//----------------------------------------------------------------------------------------
	SubOverride(SubOverride&& src)
	{
		_sub = std::move(src._sub);
	}

	MOVE_ASSIGNMENT_OPERATOR(SubOverride)

	Bool ContainsSubId(const DescID& idx, Int& position)
	{
		position = 0;
		for (Int32 e = 0; e < _sub.GetCount(); e++)
		{
			const OverrideEntry& entry = _sub[e];
			if (entry._descID == idx)
				return true;
			position++;
		}
		position = NOTOK;
		return false;
	}

	BaseArray<OverrideEntry> _sub;
};

class NodeChangeSort : public BaseSort<NodeChangeSort>
{
public:
	static inline Bool LessThan(const NodeChange& a, const NodeChange& b)
	{
		return a._node < b._node;
	}

	static inline Bool LessThan(BaseList2D* node, const NodeChange& b)
	{
		return node < b._node;
	}

	static inline Bool IsEqual(BaseList2D* node, const NodeChange& b)
	{
		return node == b._node;
	}
};

//-------------------------------------------------------------------------------------------------
/// Node data class for BaseOverride objects.
//-------------------------------------------------------------------------------------------------
class BaseOverrideData : public NodeData
{
	INSTANCEOF(BaseOverrideData, NodeData)

public:
	BaseOverrideData() : _preview(nullptr) { _subNodes.FlushAll(); }
	~BaseOverrideData() { _subNodes.FlushAll(); }

	virtual Bool Init();
	virtual void Free();

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	virtual Int32 GetChunkLevel() { return 3; }

	virtual Bool TranslateDescID(GeListNode* node, const DescID& id, DescID& res_id, GeListNode*& res_at);

	//  // basic take functions
	BaseList2D* GetMainNode();
	BaseTake* GetTakeNode(BaseDocument* doc);
	Bool SetTakeNode(BaseTake* newTake);
	BaseOverrideGroup* GetGroupLink(BaseDocument* doc);
	Bool SetGroupLink(BaseOverrideGroup* group);

	// parameters function
	Int GetOverrideParameterCount();
	Bool IsOverriddenParam(const DescID& pIdx);

	Bool IsAnimatedOverrideParam(const DescID& pIdx);
	Bool DeleteTrackIfAnimated(const DescID& pIdx);
	Bool OverrideParam(const OverrideDataSet& data);

	GeData GetOverrideParam(const DescID& pIdx);
	Bool RemoveOverrideParam(BaseDocument* doc, const DescID& pIdx, Bool removeEntry = true);
	Bool GetValuesFromDocument(BaseDocument* doc, NodeChange* exclude);
	Bool GetAllValuesFromDocument(BaseDocument* doc, BaseArray<OverrideMapEntry>& changed);
	Bool SendValuesToDocument(BaseDocument* doc, NodeChange* exclude, BaseList2D* toClone = nullptr);
	Bool SendAllValuesToDocument(BaseDocument* doc, BaseArray<OverrideMapEntry>& changed, BaseList2D* toClone = nullptr);
	Bool IsTakeAffected(BaseDocument* doc, BaseTake* take, const DescID& pIdx);
	Bool GetAllOverrideDescID(cineware::BaseArray<DescID>& result);

	Bool RebuildUserData(GeListNode* node);

	void AppendSubNode(BaseDocument* doc, BaseList2D* node);
	Bool HasValidSubNodes(BaseDocument* doc);

	BaseList2D* GetParentNode(BaseDocument* doc);
	void SetParentNode(BaseList2D* parent);

	static NodeData* Alloc() { return NewObj(BaseOverrideData); }

private:
	BaseArray<OverrideMapEntry>	_overrideMap;	// a map with all overridden DescIDs

	BaseLink										_takeLink;		// safe link to the corresponding take
	BaseLink										_groupLink;		// safe link to optional group if GetLink() != nullptr the override is inside a group

	BaseLink										_parentNode;	// in case of this override is a child of another this is a link to the parent node
	BaseLinkArray								_subNodes;		// a list of linked nodes, used for branched override so each override stay in the correct place but is displayed in a logic way in the tree

	BaseBitmap*									_preview;			// optional preview for materials texture tags
};

#pragma pack (pop)
}

#endif	// C4D_BASEOVERRIDEDATA_H__
