/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_UNITSCALE_H__
#define CUSTOMGUI_UNITSCALE_H__

#include "c4d_customdatatype.h"
#include "c4d_parameter_ids.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// Unit scale data type ID.
#define CUSTOMDATATYPE_UNITSCALE 200000275

/// @addtogroup UNITSCALE_SETTINGS
/// @ingroup group_customguisettings
/// @{
#define UNITSCALE_SCALE							1			///< cineware::Float The unit scale.
#define UNITSCALE_UNIT							2			///< cineware::Int32 The unit: @enumerateEnum{DOCUMENT_UNIT}
/// @}

/// @addtogroup DOCUMENT_UNIT
/// @ingroup group_enumeration
/// @{
/// Document units.
enum DOCUMENT_UNIT
{
	DOCUMENT_UNIT_UNDEF								= 0,			///< Undefined.
	DOCUMENT_UNIT_KM									= 1,			///< Kilometer.
	DOCUMENT_UNIT_M										= 2,			///< Meter.
	DOCUMENT_UNIT_CM									= 3,			///< Centimeter.
	DOCUMENT_UNIT_MM									= 4,			///< Millimeter.
	DOCUMENT_UNIT_MICRO								= 5,			///< Micrometer.
	DOCUMENT_UNIT_NM									= 6,			///< Nanometer.
	DOCUMENT_UNIT_MILE								= 7,			///< Mile.
	DOCUMENT_UNIT_YARD								= 8,			///< Yard.
	DOCUMENT_UNIT_FOOT								= 9,			///< Foot.
	DOCUMENT_UNIT_INCH								= 10			///< Inch.
} ENUM_END_LIST(DOCUMENT_UNIT);
/// @}

//----------------------------------------------------------------------------------------
/// Unit scale data type (@ref CUSTOMDATATYPE_UNITSCALE).
///
/// <b>Example</b>: <i>Set the unit and scale of a document:</i>
/// @code
/// BaseDocument *doc = BaseDocument::Alloc();
///
/// UnitScaleData uscale = UnitScaleData::Alloc();
/// if (uscale==nullptr)
///   return;
///
/// uscale->SetUnitScale(10.0, DOCUMENT_UNIT_MICRO);
///
/// GeData data;
/// data.SetCustomDataType(CUSTOMDATATYPE_UNITSCALE, *uscale);
/// doc->SetParameter(DOCUMENT_DOCUNIT, data);
///
/// UnitScaleData::Free(uscale)
/// @endcode
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class UnitScaleData : public iCustomDataType<UnitScaleData>
{
private:
	UnitScaleData();
	~UnitScaleData();

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{unit scale data}
	/// @return												@allocReturn{unit scale data}
	//----------------------------------------------------------------------------------------
	static UnitScaleData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{unit scale data}
	/// @param[in,out] data						@theToDestruct{unit scale data}
	//----------------------------------------------------------------------------------------
	static void Free(UnitScaleData*& data);

	/// @}

	/// @name Get/Set Unit/Scale
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the values of the unit scale data.
	/// @param[out] scale							Assigned the unit scale.
	/// @param[out] unit							Assigned the unit.
	/// @return												@trueIfOtherwiseFalse{unit scale data values were successfully retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetUnitScale(Float& scale, DOCUMENT_UNIT& unit) const;

	//----------------------------------------------------------------------------------------
	/// Sets the values for the unit scale data.
	/// @param[in] scale							The unit scale.
	/// @param[in] unit								The unit.
	/// @return												@trueIfOtherwiseFalse{unit scale data values were successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetUnitScale(Float scale, DOCUMENT_UNIT unit);

	/// @}

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Equality operator. Checks if two unit scale data are equal.
	/// @param[in] b									A unit scale data.
	/// @return												@trueIfOtherwiseFalse{the unit scale datas are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (const UnitScaleData& b) const;

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Checks if two unit scale data are different.
	/// @param[in] b									A unit scale data.
	/// @return												@trueIfOtherwiseFalse{the unit scale datas are different}
	//----------------------------------------------------------------------------------------
	Bool operator != (const UnitScaleData& b) const;

	/// @}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// @markInternal
class UnitScaleDataTypeClass
{
public:
	UnitScaleDataTypeClass();

	virtual Int32 GetId();

	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData(CustomDataType* d, HyperFile* hf, Int32 level);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack (pop)
}

#endif	// CUSTOMGUI_UNITSCALE_H__
