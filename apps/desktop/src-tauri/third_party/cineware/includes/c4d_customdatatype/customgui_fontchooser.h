/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_FONTCHOOSER_H__
#define CUSTOMGUI_FONTCHOOSER_H__

/// Font chooser custom data ID.
#define FONTCHOOSER_DATA 1009372

#include "c4d_customdatatype.h"
#include "c4d_basecontainer.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

//----------------------------------------------------------------------------------------
/// Custom data (@ref FONTCHOOSER_DATA) for the font chooser GUI.
/// @since 17.008
//----------------------------------------------------------------------------------------
class FontData : public CustomDataType
{
private:
	FontData();
	~FontData();

public:
	//----------------------------------------------------------------------------------------
	/// Retrieves the font container.
	/// @return												The font container.
	//----------------------------------------------------------------------------------------
	BaseContainer GetFont() const;

	//----------------------------------------------------------------------------------------
	/// Sets the font container.
	/// @param[in] bc									The font container.
	//----------------------------------------------------------------------------------------
	void SetFont(BaseContainer* bc);
};

/// @markInternal
class FontDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_FONTCHOOSER_H__
