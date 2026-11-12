/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_COLORPROFILE_H__
#define CUSTOMDATATYPE_COLORPROFILE_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// Color profile data type ID.
#define CUSTOMDATATYPE_COLORPROFILE	200000266

/// @addtogroup ColorProfilePreset
/// @ingroup group_enumeration
/// @{
/// Presets for color profiles.
#define DefaultSRGBColorProfilePreset				664	///< @C4D's default sRGB color profile.
#define DefaultSGrayColorProfilePreset			372	///< @C4D's default sRGB grayscale color profile.
#define DefaultLinearRGBColorProfilePreset	620	///< @C4D's default linear RGB color profile.
#define DefaultLinearGrayColorProfilePreset	376	///< @C4D's default linear grayscale color profile.
/// @}

//----------------------------------------------------------------------------------------
/// Color profile data type (@ref CUSTOMDATATYPE_COLORPROFILE).
//----------------------------------------------------------------------------------------
class ColorProfile : public CustomDataType
{
public:
	//----------------------------------------------------------------------------------------
	/// Initializes the color profile data with the given @C4D preset.
	/// @param[in] colProfilePreset		The color profile preset: @ref ColorProfilePreset.
	//----------------------------------------------------------------------------------------
	void InitWithColorProfile(Int32 colProfilePreset);

private:
	ColorProfile();
	~ColorProfile();
};

/// @markInternal
class ColorProfileDataTypeClass
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

#endif	// CUSTOMDATATYPE_COLORPROFILE_H__
