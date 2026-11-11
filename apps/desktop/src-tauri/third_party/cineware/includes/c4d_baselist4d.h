/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASELIST4D_H__
#define C4D_BASELIST4D_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a node in a 4D list.
/// @see BaseList2D
//----------------------------------------------------------------------------------------
class BaseList4D : public BaseList2D
{
	INSTANCEOF(BaseList4D, BaseList2D)

protected:
	BaseList4D();
	~BaseList4D();

	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

public:
	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the parent node.
	/// @return												The parent node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	BaseList4D* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child node.
	/// @return												The first child node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	BaseList4D* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child node.
	/// @return												The last child node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	BaseList4D* GetDownLast();

	/// @}

	/// @name Insertion
	/// @{

	//----------------------------------------------------------------------------------------
	/// Inserts the node before @formatParam{bl}.
	/// @warning	Make sure that the insertion is sane. For example it is not allowed to insert a key into the object list.\n
	///						It is also necessary to call Remove() first if the object is already inserted into another list.
	/// @param[in] bl									The node to insert before. @callerOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	void InsertBefore(BaseList4D* bl);

	//----------------------------------------------------------------------------------------
	/// Inserts the node after @formatParam{bl}.
	/// @warning	Make sure that the insertion is sane. For example it is not allowed to insert a key into the object list.\n
	///						It is also necessary to call Remove() first if the object is already inserted into another list.
	/// @param[in] bl									The node to insert after. @callerOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	void InsertAfter(BaseList4D* bl);

	//----------------------------------------------------------------------------------------
	/// Inserts the node under @formatParam{bl} as the first child.
	/// @warning	Make sure that the insertion is sane. For example it is not allowed to insert a key into the object list.\n
	///						It is also necessary to call Remove() first if the object is already inserted into another list.
	/// @param[in] bl									The node to insert under. @callerOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	void InsertUnder(BaseList4D* bl);

	//----------------------------------------------------------------------------------------
	/// Insert the node as the last child of @formatParam{bl}.
	/// @warning	Make sure that the insertion is sane. For example it is not allowed to insert a key into the object list.\n
	///						It is also necessary to call Remove() first if the object is already inserted into another list.
	/// @param[in] bl									The node to insert under as last child. @callerOwnsPointed{node}
	//----------------------------------------------------------------------------------------
	void InsertUnderLast(BaseList4D* bl);

	/// @}

	/// @name Removal
	/// @{

	//----------------------------------------------------------------------------------------
	/// Removes the node from its list.
	/// @warning	When a node is removed the owner becomes responsible for freeing it or passing its ownership to another list.
	//----------------------------------------------------------------------------------------
	virtual void Remove();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASELIST4D_H__
