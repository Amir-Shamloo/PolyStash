/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_ITEMLIST_H__
#define CUSTOMDATATYPE_ITEMLIST_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseSelect;
class String;
class GeData;
class BaseContainer;
class BaseLink;
class AliasTrans;
class HyperFile;

#define CUSTOMDATA_ITEMLIST 1019655

///////////////////////////////////////////////////////////////////////////////////////////////////

#define ITEMLIST_LEVEL 1

#define MSG_ITEMLIST_CALLBACK 1024312

#define ITEMLIST_COLOR_ID				20000
#define ITEMLIST_BASELINK_ID		20001
#define ITEMLIST_CHECK_STATE_ID	20002
#define ITEMLIST_ICON_ID				20003

#define ITEMLIST_CALLBACK_LISTEVENT 1
#define ITEMLIST_CALLBACK_DRAGDROP	2

#define ITEMLIST_ID_COLOR_FLAG		2000
#define ITEMLIST_ID_MULTIPLE_FLAG	2001
#define ITEMLIST_ID_BASELINK_FLAG	2002
#define ITEMLIST_ID_CHECKBOX_FLAG	2003
#define ITEMLIST_ID_ROWS					2004
#define ITEMLIST_ID_ICON_FLAG			2005

#define ITEMLIST_CALLBACK_FLAG_NOTIFY (1 << 0)

#define ITEMLIST_DRAGDROP_PTR	1000
#define ITEMLIST_DRAGDROP_X		1001
#define ITEMLIST_DRAGDROP_Y		1002
#define ITEMLIST_DRAGDROP_MSG	1003

class ItemListData : public CustomDataType
{
private:
	ItemListData();
	~ItemListData();

public:
	static ItemListData* Alloc();
	static void Free(ItemListData*& pData);

	Bool AddItem(Int32 id, const String& name, const GeData& data);
	Bool UpdateItem(Int32 id, const String& name, const GeData& data);
	void RemoveItem(Int32 id);

	Int32 GetCount();
	Int32 GetSelected();
	void Select(Int32 index);

	GeData GetData(Int32 index);
	String GetName(Int32 index);
	Int32 GetID(Int32 index);

	void SetData(Int32 index, const GeData& data);
	void SetName(Int32 index, const String& name);
	void SetID(Int32 index, Int32 id);

	Int32 FindIndex(Int32 id);

	void Clear();
	void GetSelection(BaseSelect* bs);
	void SetSelection(BaseSelect* bs);
	void GetSelectionIDs(BaseSelect* bs);

	BaseContainer* GetUserData();
	BaseLink* GetOwnerLink();
};

/// @markInternal
class ItemListDataTypeClass
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

#endif	// CUSTOMDATATYPE_ITEMLIST_H__
