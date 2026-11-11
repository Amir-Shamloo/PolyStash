/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_PRIORITYDATA_H__
#define C4D_PRIORITYDATA_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;
class GeData;

/// Priority datatype ID.
#define CUSTOMGUI_PRIORITY_DATA 1009369

//----------------------------------------------------------------------------------------
/// Priority data for the Priority data type (::CUSTOMGUI_PRIORITY_DATA).
//----------------------------------------------------------------------------------------
class PriorityData : public CustomDataType
{
public:
	Int32	m_lMode,											///< Priority mode.
				m_lLong;											///< Priority.
	Bool	m_bCamera;										///< Camera dependent.

	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	PriorityData();

	/// @}

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool WriteData(HyperFile* hf) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool ReadData (HyperFile* hf, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Copies the priority data to @formatParam{dest}.
	/// @param[in] dest								The destination data. @callerOwnsPointed{priority data}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool CopyTo(PriorityData* dest);

	//----------------------------------------------------------------------------------------
	/// Checks if the priority data is equal to another.
	/// @param[in] dest								The priority data to compare to. @callerOwnsPointed{priority data}
	/// @return												@trueIfOtherwiseFalse{the priority data is equal}
	//----------------------------------------------------------------------------------------
	Bool IsEqual(const PriorityData* dest) const;

	//----------------------------------------------------------------------------------------
	/// Sets a priority value.
	/// @param[in] lValueID						Priority Value ID.
	/// @param[in] data								New value data.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetPriorityValue(Int32 lValueID, const GeData& data);
};

#pragma pack (pop)
}

#endif	// C4D_PRIORITYDATA_H__
