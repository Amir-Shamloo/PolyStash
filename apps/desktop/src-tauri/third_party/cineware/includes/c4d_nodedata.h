/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_NODEDATA_H__
#define C4D_NODEDATA_H__

#include "c4d_privatechunk.h"

namespace cineware
{
#pragma pack (push, 8)

class GeListNode;
class BaseTag;

//----------------------------------------------------------------------------------------
/// Parent class for all object data. All data types (e.g. Polygon, Light, Camera) are derived from this class.
/// @see The articles @ref page_implementation_cineware and @ref page_implementation_on_the_external_side.
//----------------------------------------------------------------------------------------
class NodeData : public PrivateChunk
{
	INSTANCEOF(NodeData, PrivateChunk)

	friend class BaseList2D;
	friend class BaseObject;
	friend class BaseTag;
	friend class BaseShader;
	friend class BaseVideoPost;
	friend class BaseSceneHook;
	friend class BaseSceneHookData;
	friend class PluginNode;
	friend class BaseOverrideData;
	friend class BaseMaterial;

private:
	BaseList2D*	m_ptr;

protected:
	//----------------------------------------------------------------------------------------
	/// Called when an instance of a derived node data class needs to be initialized.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Init();
	virtual void Free();

	//----------------------------------------------------------------------------------------
	/// Reads the node from a hyper file.
	/// @param[in] hf									The hyper file to read from. @cinewareOwnsPointed{hyper file}
	/// @param[in] id									The ID of the node to read.
	/// @param[in] level							The disklevel of the node to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Overload this for self defined Alien data classes. Called when Cineware SDK loads a document.
	/// @return												@trueIfOtherwiseFalse{the node type is supported by the external application}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	//----------------------------------------------------------------------------------------
	/// Gets the node's unique disk type ID.
	/// @return												The node's disk type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool TranslateDescID(GeListNode* node, const DescID& id, DescID& res_id, GeListNode*& res_at)
	{
		return false;
	}

private:
	Bool GetParameter(Int32 id, GeData& data)
	{
		return false;
	}
	Bool SetParameter(Int32 id, const GeData& data)
	{
		return false;
	}

	Bool GetParameter(const DescID& id, GeData& data)
	{
		return false;
	}
	Bool SetParameter(const DescID& id, const GeData& data)
	{
		return false;
	}

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	NodeData();

	/// @}

	/// @name Node
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object connected with the data instance.
	/// @return												The object. @callerOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseList2D* GetNode();

	//----------------------------------------------------------------------------------------
	/// Sets the object connected with the data instance.
	/// @param[in] ptr								The object to set. @callerOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	void SetNode(BaseList2D* ptr);

	/// @}

	/// @name Tag
	/// @{

	//----------------------------------------------------------------------------------------
	/// @see BaseObject::GetTag(Int32 id)
	//----------------------------------------------------------------------------------------
	BaseTag* GetTag(Int32 id);

	//----------------------------------------------------------------------------------------
	/// @see BaseObject::MakeTag(Int32 id)
	//----------------------------------------------------------------------------------------
	BaseTag* MakeTag(Int32 id);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_NODEDATA_H__
