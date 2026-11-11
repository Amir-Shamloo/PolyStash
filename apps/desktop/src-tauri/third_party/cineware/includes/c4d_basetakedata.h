/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASETAKEDATA_H__
#define C4D_BASETAKEDATA_H__

#include "c4d_nodedata.h"
#include "c4d_overridegroupdata.h"
#include "c4d_baseobject.h"
#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseOverrideGroup;
class TakeData;
class GroupPlaceholder;

static const Int32 ID_FOLDERTYPE = 10000;			// this is the container id for the folder type
static const Int32 ID_FOLDERNODETYPE = 10001;	// this is the container id for the node type of the folder

Bool RemoveOverrideObjectByGroup(BaseDocument* doc, GroupPlaceholder* placeHolderData, BaseObject* placeHolder, BaseObject* documentNode, Int32 tagType = NOTOK);

//----------------------------------------------------------------------------------------
// simple compact class to handle take sub branches
//----------------------------------------------------------------------------------------
class TakeSubBranchManager
{
public:
	TakeSubBranchManager()
	{
		_overrideFolder = NewObj(RootPluginNode);
		_overrideGroup	= NewObj(RootPluginNode);
	}

	~TakeSubBranchManager()
	{
		DeleteObj(_overrideFolder);
		DeleteObj(_overrideGroup);
	}

	Bool Init(GeListNode* node)
	{
		if (!_overrideFolder || !_overrideGroup)
			return false;

		_overrideFolder->SetParent(node);
		_overrideFolderName = String("Override Folders");

		_overrideGroup->SetParent(node);
		_overrideGroupName = String("Override Groups");
		return true;
	}

	Bool Read(HyperFile* hf, Int32 level)
	{
		if (level >= 1)
		{
			if (!_overrideFolder->ReadObject(hf, true))
				return false;
			if (!_overrideGroup->ReadObject(hf, true))
				return false;
		}
		return true;
	}

	Bool Write(HyperFile* hf)
	{
		return _overrideFolder->WriteObject(hf, 0) && _overrideGroup->WriteObject(hf, OverrideGroup);
	}

	Bool CopyTo(TakeSubBranchManager* dest, COPYFLAGS flags, AliasTrans* trn)
	{
		if (!dest)
			return false;
		return _overrideFolder->CopyTo(dest->_overrideFolder, flags, trn) && _overrideGroup->CopyTo(dest->_overrideGroup, flags, trn);
	}

	RootPluginNode*	_overrideFolder;			// header for folder nodes used in the tree for override containers
	String					_overrideFolderName;	// folder branch name.
	RootPluginNode*	_overrideGroup;				// header for group nodes used for massive tag override
	String					_overrideGroupName;		// group branch name.
};

//----------------------------------------------------------------------------------------
// folder object to make the GUI management simple
//----------------------------------------------------------------------------------------
class TakeFolderData : public NodeData
{
	INSTANCEOF(TakeFolderData, NodeData)

public:
	TakeFolderData()	{ }
	~TakeFolderData() { }

	virtual Bool Init() { return true; }

	static NodeData* Alloc() { return NewObj(TakeFolderData); }
};

//----------------------------------------------------------------------------------------
// Group Placeholder node is created as child of group and hold original tags and overridden parameters while the take that own the group is active.
// In the Placeholder container are stored a link to the original node in the document (part of the group) and a map in between document tag and backup tag.
// Each side of the map in the container could be nullptr but not both at the same time.
//----------------------------------------------------------------------------------------
class GroupPlaceholder : public NodeData
{
	INSTANCEOF(GroupPlaceholder, NodeData)

public:
	GroupPlaceholder()	{ }
	~GroupPlaceholder() { }

	virtual Bool Init()
	{
		BaseObject* op = static_cast<BaseObject*>(((NodeData*)this)->GetNode());
		if (!op)
			return false;

		BaseContainer* bc = op->GetDataInstance();
		if (!bc)
			return false;

		return true;
	}

	Bool SetNode(BaseList2D* node)
	{
		if (!node)
			return false;

		BaseObject* op = static_cast<BaseObject*>(((NodeData*)this)->GetNode());
		if (!op)
			return false;

		BaseContainer* bc = op->GetDataInstance();
		if (!bc)
			return false;

		bc->SetLink(ID_OVERRIDEPLACEHOLDER_LINK, node);
		return true;
	}

	BaseList2D* GetNode(BaseDocument* doc)
	{
		if (!doc)
			return nullptr;

		BaseObject* op = static_cast<BaseObject*>(((NodeData*)this)->GetNode());
		if (!op)
			return nullptr;

		BaseContainer* bc = op->GetDataInstance();

		return bc->GetLink(ID_OVERRIDEPLACEHOLDER_LINK);
	}

	Bool SetTagsMap(BaseContainer* bc, BaseTag* documentTag, BaseTag* backupTag, Int32 docIndex)
	{
		if (!bc)
			return false;

		Int32 tTagCurrenCount = bc->GetInt32(ID_OVERRIDEGROUP_TAG_COUNT, 0);
		Int32 backIndex = ID_OVERRIDEGROUP_TAG_BACKUP_START + (docIndex - ID_OVERRIDEGROUP_TAG_DOCUMENT_START);
		tTagCurrenCount++;
		bc->SetInt32(ID_OVERRIDEGROUP_TAG_COUNT, tTagCurrenCount);	// increase the counter

		bc->SetLink(docIndex, documentTag);
		bc->SetLink(backIndex, backupTag);

		return true;
	}

	Bool GetTagsMap(BaseDocument* doc, BaseContainer* bc, BaseTag*& documentTag, BaseTag*& backupTag, Int32 docIndex)
	{
		documentTag = nullptr;
		backupTag = nullptr;

		if (!bc || !doc)
			return false;

		Int32	backIndex = ID_OVERRIDEGROUP_TAG_BACKUP_START + (docIndex - ID_OVERRIDEGROUP_TAG_DOCUMENT_START);

		documentTag = static_cast<BaseTag*>(bc->GetLink(docIndex));
		backupTag = static_cast<BaseTag*>(bc->GetLink(backIndex));
		return true;
	}

	static NodeData* Alloc() { return NewObj(OverrideGroupData); }
};

//----------------------------------------------------------------------------------------
// Take Data Plugin
//----------------------------------------------------------------------------------------
class BaseTakeData : public NodeData
{
	INSTANCEOF(BaseTakeData, NodeData)

public:
	BaseTakeData()
	{
		_isMain = false;
	}

	~BaseTakeData()
	{
		_isMain = false;
	}

	virtual Bool Init();
	virtual void Free();

	virtual Int32 GetChunkLevel();

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// Gets if this take is the main one in the document.
	/// @return												True if it is the main take otherwise false.
	//----------------------------------------------------------------------------------------
	Bool IsMain() { return _isMain; }

	//----------------------------------------------------------------------------------------
	/// Set this take as the main.
	//----------------------------------------------------------------------------------------
	void SetMain(Bool isMain = true) { _isMain = isMain; }

	Bool GetOverrides(GeTempDynArray<GeListNode>& selection);
	Bool GetOverrideGroups(GeTempDynArray<GeListNode>& selection);

	BaseOverrideGroup* AddOverrideGroup();
	BaseOverrideGroup* GetFirstOverrideGroup();

	BaseObject* GetCamera(TakeData* takeData);
	BaseObject* GetEffectiveCamera(TakeData* takeData, BaseTake*& resultTake);
	void SetCamera(TakeData* takeData, BaseObject* camera);

	RenderData* GetRenderData(TakeData* takeData);
	RenderData* GetEffectiveRenderData(TakeData* takeData, BaseTake*& resultTake);
	void SetRenderData(TakeData* takeData, RenderData* rData);

	Bool CompactTake(BaseDocument* doc);
	Bool RestoreTake(BaseDocument* doc);
	BaseOverride* FindOverride(BaseDocument* doc, BaseList2D* node);

	BaseList2D* AddFolder(Int32 folderType);
	BaseList2D* GetFolder(Int32 folderType);
	BaseList2D* GetOrAddFolder(Int32 folderType);
	void DeleteFolder(BaseList2D* folder);

	void Reset();

	static NodeData* Alloc() { return NewObj(BaseTakeData); }

private:
	Bool								 _isMain;			// define if a take is the main take or not
	TakeSubBranchManager _subBranch;	// sub Branch system contains folders and group
};

#pragma pack (pop)
}

#endif	// C4D_BASETAKEDATA_H__
