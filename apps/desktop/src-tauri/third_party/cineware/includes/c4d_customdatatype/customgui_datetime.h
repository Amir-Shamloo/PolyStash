/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_DATETIME_H__
#define CUSTOMGUI_DATETIME_H__

#include "c4d_customdatatype.h"
#include "c4d_basetime.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// DateTime data type ID.
#define DATETIME_DATA	1009398

//----------------------------------------------------------------------------------------
/// DateTime data type (@ref DATETIME_DATA).
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class DateTimeData : public CustomDataType
{
public:
	//----------------------------------------------------------------------------------------
	/// @allocatesA{DateTime data}
	/// @return												@allocReturn{DateTime data}
	//----------------------------------------------------------------------------------------
	static DateTimeData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{DateTime data}
	/// @param[in,out] pData					@theToDestruct{DateTime data}
	//----------------------------------------------------------------------------------------
	static void Free(DateTimeData*& pData);

	//----------------------------------------------------------------------------------------
	/// Gets the date time.
	/// @return												The date time.
	//----------------------------------------------------------------------------------------
	DateTime GetDateTime() const;

	//----------------------------------------------------------------------------------------
	/// Sets the date time.
	/// @param[in] d									The date time to set.
	/// @param[in] bSetDate						If @formatConstant{false} the date part is discarded. Default to @formatConstant{true}.
	/// @param[in] bSetTime						If @formatConstant{false} the time part is discarded. Default to @formatConstant{true}.
	//----------------------------------------------------------------------------------------
	void SetDateTime(const DateTime* d, Bool bSetDate = true, Bool bSetTime = true);

private:
	DateTimeData();
	~DateTimeData();
};

/// @markInternal
class DateTimeDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
	static Bool GetParameter(const CustomDataType* data, const DescID& id, GeData& t_data, Int32& flags);
	static Bool SetDParameter(CustomDataType* data, const DescID& id, const GeData& t_data, Int32& flags);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_DATETIME_H__
