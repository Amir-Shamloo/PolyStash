/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_SPREADSHEET_H__
#define CUSTOMDATATYPE_SPREADSHEET_H__

#include "c4d_customdatatype.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseSelect;
class AliasTrans;
class HyperFile;

#define CUSTOMDATA_SPREADSHEET 1019660

///////////////////////////////////////////////////////////////////////////////////////////////////

#define SSCOLOR_TEXT				 COLOR_MENU_TEXT
#define SSCOLOR_SELECTEDTEXT COLOR_MENU_TEXT_SELECTED
#define SSCOLOR_BG					 COLOR_MENU_BG
#define SSCOLOR_CURSOR			 COLOR_MENU_BG_BORDER
#define SSCOLOR_OUTLINE			 COLOR_MENU_BORDER_DK
#define SSCOLOR_SELECTED		 COLOR_MENU_BG_SELECTED
#define SSCOLOR_HIGHLIGHT		 COLOR_MENU_TEXT_DISABLED
#define SSCOLOR_HEADERSBG		 COLOR_BG_DARK2
#define SSCOLOR_WARN				 COLOR_SYNTAX_COMMENTWRONG

#define SPREADSHEET_LEVEL				3
#define SPREADSHEET_FLAG_FOCUS	(1 << 0)

#define SPREADSHEET_COLUMN_FLAG_BACK_COLOR (1 << 2)
#define SPREADSHEET_COLUMN_FLAG_DISABLED	 (1 << 3)
#define SPREADSHEET_COLUMN_FLAG_HIDE			 (1 << 4)

#define SPREADSHEET_CELL_FLAG_COLOR					(1 << 0)
#define SPREADSHEET_CELL_FLAG_BACK_COLOR		(1 << 1)
#define SPREADSHEET_CELL_FLAG_DISABLED			(1 << 2)
#define SPREADSHEET_CELL_FLAG_ITERATOR			(1 << 3)
#define SPREADSHEET_CELL_FLAG_ITERATOR_LINK	(1 << 4)
#define SPREADSHEET_CELL_FLAG_NO_SELECT			(1 << 5)
#define SPREADSHEET_CELL_FLAG_NO_EDIT				(1 << 6)
#define SPREADSHEET_CELL_FLAG_COLUMN_SELECT	(1 << 7)
#define SPREADSHEET_CELL_FLAG_ROW_SELECT		(1 << 8)
#define SPREADSHEET_CELL_FLAG_RIGHT_ALIGN		(1 << 9)
#define SPREADSHEET_CELL_FLAG_CENTER_ALIGN	(1 << 10)
#define SPREADSHEET_CELL_FLAG_SELECT_ALL		(1 << 11)
#define SPREADSHEET_CELL_FLAG_HIDE					(1 << 12)
#define SPREADSHEET_CELL_FLAG_CALLBACK			(1 << 13)
#define SPREADSHEET_CELL_FLAG_BOLD					(1 << 14)
#define SPREADSHEET_CELL_FLAG_VERICAL				(1 << 15)

#define SPREADSHEET_FLAGMODE_ADD		1
#define SPREADSHEET_FLAGMODE_REMOVE	2
#define SPREADSHEET_FLAGMODE_SET		3

#define SPREADSHEET_ITERATOR_LINK								1000
#define SPREADSHEET_ITERATOR_INDEX							1001
#define SPREADSHEET_ITERATOR_COUNT							1002
#define SPREADSHEET_ITERATOR_DIRTY							1003
#define SPREADSHEET_ITERATOR_DTYPE							1004
#define SPREADSHEET_ITERATOR_CALLBACK_LINK			1005
#define SPREADSHEET_ITERATOR_CALLBACK_DATA			1006
#define SPREADSHEET_ITERATOR_CALLBACK_CONTAINER	1007
#define SPREADSHEET_ITERATOR_CALLBACK_FLAGS			1008
#define SPREADSHEET_ITERATOR_CALLBACK_ID				1009

#define SPREADSHEET_DATA_ID	2000

#define SSCOLOR_UNSET -2
#define SSCOLOR_COLOR -1

class ssColor
{
public:
	ssColor(_DONTCONSTRUCT v) { }
	ssColor()
	{
		m_ID = SSCOLOR_UNSET;
	}
	ssColor(Int32 id)
	{
		m_ID = id;
	}
	ssColor(const Vector& col)
	{
		m_ID = SSCOLOR_COLOR;
		m_Color = col;
	}

	Int32	 m_ID;
	Vector m_Color;
};

class SpreadSheetItem
{
public:
	SpreadSheetItem ()
	{
		m_ID = 0;
		m_Flags = 0;
	}

	Int32	 m_ID;
	GeData m_Data;
	Int32	 m_Flags;
	Vector m_BackColor, m_TextColor;
	Int32	 m_BackColID, m_TextColID;
};

class RowData
{
public:
	RowData()
	{
		m_Lower = 0;
		m_Upper = 0;
	}
	RowData(Int32 index)
	{
		m_Upper = m_Lower = index;
	}
	RowData(Int32 lower, Int32 upper)
	{
		m_Upper = upper;
		m_Lower = lower;
	}
	RowData(const RowData& src)
	{
		m_Lower = src.m_Lower;
		m_Upper = src.m_Upper;
	}

	Bool operator <(const RowData& dst) const
	{
		return m_Lower < dst.m_Lower;
	}
	Bool operator >(const RowData& dst) const
	{
		return m_Lower > dst.m_Upper;
	}
	Bool operator <=(const RowData& dst) const
	{
		return m_Lower <= dst.m_Upper;
	}
	Bool operator >=(const RowData& dst) const
	{
		return m_Lower >= dst.m_Lower;
	}
	Bool operator ==(const RowData& dst) const
	{
		return m_Lower >= dst.m_Lower && m_Lower <= dst.m_Upper;
	}

	RowData& operator =(const RowData& src)
	{
		m_Lower = src.m_Lower;
		m_Upper = src.m_Upper;
		return *this;
	}

	Int32 m_Lower, m_Upper;
};

class RowTable
{
public:
	RowTable();
	~RowTable();

	Int32 Search(const RowData& v, RowData* pData, Int32 lCount);
	Int32 GetRowIndex(Int32 row);

	RowData* m_pTable;
	Int32		 m_Count;
};

class ssClipItem
{
public:
	GeData m_Data;
	Int32	 m_Index;
};

class SpreadSheetData : public CustomDataType
{
private:
	SpreadSheetData();
	~SpreadSheetData();

public:
	static SpreadSheetData* Alloc();
	static void Free(SpreadSheetData*& pData);

	Int32 AddColumn(Int32 id, const GeData& data, Int32 rows, Int32 flags);
	Int32 InsertColumn(Int32 column, Int32 id, const GeData& data, Int32 rows, Int32 flags);
	Bool RemoveColumn(Int32 column);
	Bool UpdateColumn(Int32 column, Int32 id, const GeData& data);
	Bool GetColumn(Int32 column, Int32* id, GeData* data);
	void SetColumnColor(Int32 column, const ssColor& backcol, const ssColor& txtcol = ssColor());
	void GetColumnColor(Int32 column, ssColor* backcol, ssColor* txtcol = nullptr);
	Int32 GetColumnFlags(Int32 column);
	Int32 SetColumnFlags(Int32 column, Int32 flags, Int32 mode = SPREADSHEET_FLAGMODE_SET);
	Float GetColumnWidth(Int32 column);
	Float SetColumnWidth(Int32 column, Float width);
	Int32 GetColumnCount();
	Int32 FindColumn(Int32 id);

	Int32 AddRow(Int32 column, Int32 id, const GeData& data, Int32 flags);
	Int32 InsertRow(Int32 column, Int32 row, Int32 id, const GeData& data);
	Bool RemoveRow(Int32 column, Int32 row);
	Bool UpdateRow(Int32 column, Int32 row, Int32 id, const GeData& data);
	Bool GetRow(Int32 column, Int32 row, Int32* id, GeData* data);
	void SetRowColor(Int32 column, Int32 row, const ssColor& backcol, const ssColor& txtcol = ssColor());
	void GetRowColor(Int32 column, Int32 row, ssColor* backcol, ssColor* txtcol = nullptr);
	Int32 GetRowCount(Int32 column);
	Int32 GetRowTotalCount(Int32 column);
	Int32 GetRowFlags(Int32 column, Int32 row);
	Int32 SetRowFlags(Int32 column, Int32 row, Int32 flags, Int32 mode = SPREADSHEET_FLAGMODE_SET);
	Int32 FindRow(Int32 column, Int32 id);	// column can be NOTOK

	void GetSelection(BaseSelect* selection);
	void SetSelection(BaseSelect* selection);

	Int32 GetFlags();
	Int32 SetFlags(Int32 flags, Int32 mode = SPREADSHEET_FLAGMODE_SET);

	void Clear();

	Int32 GetRowIndex(Int32 column, Int32 row, Int32* base);
	RowTable* BuildRowTable(Int32 column);

	Int32 GetSelectionCount();
	void GetHidden(BaseSelect* selection);
	void SetHidden(BaseSelect* selection);
	Int32 GetHiddenCount();
	void GetCursor(Int32* cx, Int32* cy);
	void SetCursor(Int32 cx, Int32 cy);

	BaseContainer* GetUserData();
	BaseLink* GetOwnerLink();
};

/// @markInternal
class SpreadSheetDataTypeClass
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

#endif	// CUSTOMDATATYPE_SPREADSHEET_H__
