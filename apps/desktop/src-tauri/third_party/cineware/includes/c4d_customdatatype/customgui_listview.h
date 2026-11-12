/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_LISTVIEW_H__
#define CUSTOMGUI_LISTVIEW_H__

#include "c4d_customdatatype.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

#define CUSTOMDATA_LISTVIEW 1018397

class ListViewData : public CustomDataType
{
private:
	ListViewData();
	~ListViewData();

public:
	static ListViewData* Alloc();
	static void Free(ListViewData*& pData);

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
};

/// @markInternal
class ListViewDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData (CustomDataType* d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_LISTVIEW_H__
