/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DESCLEVEL_H__
#define C4D_DESCLEVEL_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a level within a DescID.
//----------------------------------------------------------------------------------------
struct DescLevel
{
	Int32 id;				///< ID of the level.
	Int32 dtype;		///< Datatype. Either a custom ID or a data type ID.
	Int32 creator;	///< Creator ID.

	//----------------------------------------------------------------------------------------
	/// Constructs a DescLevel with the given ID.
	/// @param[in] t_id								Initial value for id.
	//----------------------------------------------------------------------------------------
	DescLevel(Int32 t_id) : id(t_id), dtype(0), creator(0) { }

	//----------------------------------------------------------------------------------------
	/// Constructs a DescLevel with the given ID, datatype and creator ID.
	/// @param[in] t_id								Initial value for id.
	/// @param[in] t_datatype					Initial value for dtype.
	/// @param[in] t_creator					Initial value for creator.
	//----------------------------------------------------------------------------------------
	DescLevel(Int32 t_id, Int32 t_datatype, Int32 t_creator) : id(t_id), dtype(t_datatype), creator(t_creator) { }

	//----------------------------------------------------------------------------------------
	/// Equality operator. Checks if all the level values are equal to the values in @formatParam{d}.
	/// @param[in] d									Level to compare with.
	/// @return												@trueIfOtherwiseFalse{levels are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (const DescLevel& d) const;

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Checks if any value of the level is different to a value in @formatParam{d}.
	/// @param[in] d									Level to compare with.
	/// @return												@trueIfOtherwiseFalse{levels are not equal}
	//----------------------------------------------------------------------------------------
	Bool operator != (const DescLevel& d) const;
};

#pragma pack (pop)
}

#endif	// C4D_DESCLEVEL_H__
