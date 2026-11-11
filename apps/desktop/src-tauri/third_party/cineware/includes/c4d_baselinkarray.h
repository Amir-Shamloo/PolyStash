/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASELINKARRAY_H__
#define C4D_BASELINKARRAY_H__

#include "c4d_misc/datastructures/basearray.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDocument;

//----------------------------------------------------------------------------------------
/// A helper class to handle BaseLink instances.
/// @since 17.008
//----------------------------------------------------------------------------------------
class BaseLinkArray : public BaseArray<BaseLink*>
{
public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Destructor.
	//----------------------------------------------------------------------------------------
	~BaseLinkArray()
	{
		FlushAll();
	}

	//----------------------------------------------------------------------------------------
	/// Frees all the base links in the array.
	//----------------------------------------------------------------------------------------
	void FlushAll()
	{
		BaseLink* c; Int32 i;

		for (i = 0; i < GetCount(); ++i)
		{
			c = (*this)[i];
			if (!c)
				continue;
			BaseLink::Free(c);
		}
		Reset();
	}

	/// @}

	/// @name Get link/node/baselist
	/// @{

	//----------------------------------------------------------------------------------------
	/// Searches for a specific @formatParam{bl} in the array and returns its index.
	/// @param[in] bl									The node to find. @callerOwnsPointed{node}
	/// @param[in] doc								The document for @formatParam{bl}. @callerOwnsPointed{document}
	/// @return												The BaseLink's index in the array, or @ref NOTOK if it was not found.
	//----------------------------------------------------------------------------------------
	Int32 Find(GeListNode* bl, BaseDocument* doc) const
	{
		Int32 i;
		for (i = 0; i < GetCount(); i++)
		{
			BaseLink* l = (*this)[i];
			if (!l)
				continue;
			GeListNode* b = (GeListNode*)l->GetLink();
			if (!b)
				continue;
			if (b == bl)
				return i;
		}
		return NOTOK;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the base link at index @formatParam{i}.
	/// @param[in] i									The index of the base link to get. 0 <= @formatParam{i} < GetCount()
	/// @return												The base link. The array owns the pointed base link.
	//----------------------------------------------------------------------------------------
	BaseLink* GetIndexLink(Int32 i) const
	{
		if (i < 0 || i >= GetCount())
			return nullptr;

		return (*this)[i];
	}

	//----------------------------------------------------------------------------------------
	/// Gets the node at index @formatParam{i}.
	/// @param[in] i									The index of the node to get. 0 <= @formatParam{i} < GetCount()
	/// @param[in] doc								The document for the node. @callerOwnsPointed{document}
	/// @return												The node. The array owns the pointed node.
	//----------------------------------------------------------------------------------------
	GeListNode* GetIndex(Int32 i, BaseDocument* doc) const
	{
		if (i < 0 || i >= GetCount())
			return nullptr;

		BaseLink* l = (*this)[i];
		if (!l)
			return nullptr;
		return l->GetLinkAtom(doc);
	}

	//----------------------------------------------------------------------------------------
	/// Gets the base list at index @formatParam{i}.
	/// @param[in] i									The index of the base list to get. 0 <= @formatParam{i} < GetCount()
	/// @param[in] doc								The document for the base list. @callerOwnsPointed{document}
	/// @param[in] instanceof					Checked against GeListNode::IsInstanceOf(). Pass @ref NOTOK to ignore this test. Default to @ref Tbaselist2d.
	/// @return												The base list. The array owns the pointed base list.
	//----------------------------------------------------------------------------------------
	BaseList2D* GetIndexBl(Int32 i, BaseDocument* doc, Int32 instanceof = Tbaselist2d) const
	{
		if (i < 0 || i >= GetCount())
			return nullptr;

		BaseLink* l = (*this)[i];
		if (!l)
			return nullptr;
		return (BaseList2D*)l->GetLinkAtom(doc, instanceof);
	}

	/// @}

	/// @name Append/Remove
	/// @{

	//----------------------------------------------------------------------------------------
	/// Appends an node to the array.
	/// @param[in] bl									The node to append. @callerOwnsPointed{node}
	/// @return												@trueIfOtherwiseFalse{the base link was successfully added to the array}
	//----------------------------------------------------------------------------------------
	Bool Append(GeListNode* bl)
	{
		BaseLink* l = BaseLink::Alloc();
		if (!l)
			return false;
		l->SetLink(bl);
		return BaseArray<BaseLink*>::Append(l) != nullptr;
	}

	//----------------------------------------------------------------------------------------
	/// Removes and frees the base link at position @formatParam{n}.
	/// @param[in] n									The position of the base link to remove. 0 <= @formatParam{n} < GetCount()
	/// @return												@trueIfOtherwiseFalse{the base link was successfully removed}
	//----------------------------------------------------------------------------------------
	Bool Remove(Int32 n)
	{
		if (n < 0 || n >= GetCount())
			return false;

		BaseLink* l = (*this)[n];
		if (l)
			BaseLink::Free(l);

		Erase(n);

		return true;
	}

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of valid base links in the array.
	/// @param[in] doc								The document to check the base links. @callerOwnsPointed{document}
	/// @return												The number of valid base links in the array.
	//----------------------------------------------------------------------------------------
	Int32 GetRealCount(BaseDocument* doc) const
	{
		Int32 realcnt = 0;
		Int32 i, cnt = (Int32)GetCount();
		for (i = 0; i < cnt; i++)
		{
			GeListNode* bl = GetIndex(i, doc);
			if (bl)
				realcnt++;
		}
		return realcnt;
	}

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASELINKARRAY_H__
