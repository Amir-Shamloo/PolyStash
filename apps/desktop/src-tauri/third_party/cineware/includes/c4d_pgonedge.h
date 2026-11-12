/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_PGONEDGE_H__
#define C4D_PGONEDGE_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

struct CPolygon;

//----------------------------------------------------------------------------------------
/// Represents an N-gon edge of a Pgon.
//----------------------------------------------------------------------------------------
class PgonEdge
{
public:
	//----------------------------------------------------------------------------------------
	/// Check if the polygon ID is the first occurrence in the array.\n
	/// @note Useful to know which polygons form an N-gon.
	/// @return												@trueIfOtherwiseFalse{the polygon ID is the first occurrence in the array}
	//----------------------------------------------------------------------------------------
	Bool IsFirst();

	//----------------------------------------------------------------------------------------
	/// Checks if it is the last edge in the segment.
	/// @return												@trueIfOtherwiseFalse{it is the last edge in the segment}
	//----------------------------------------------------------------------------------------
	Bool IsSegmentEnd();

	//----------------------------------------------------------------------------------------
	/// Gets the polygon ID.
	/// @return												The polygon ID.
	//----------------------------------------------------------------------------------------
	Int32 ID();

	//----------------------------------------------------------------------------------------
	/// Gets the edge state.
	/// @return												The edge as a bit, or @em 0 if the polygon has no edge on the outer/inner of the N-gon.
	//----------------------------------------------------------------------------------------
	Int32 State();

	//----------------------------------------------------------------------------------------
	/// Gets the edge of the polygon.
	/// @return												The edge of the polygon, or @ref NOTOK for no edge.
	//----------------------------------------------------------------------------------------
	Int32 Edge();

	//----------------------------------------------------------------------------------------
	/// Gets the edge index.
	/// @return												The edge index (4*polygon ID + edge).
	//----------------------------------------------------------------------------------------
	Int32 EdgeIndex();

	//----------------------------------------------------------------------------------------
	/// Assigns to @formatParam{p1} and @formatParam{p2} the point IDs for the edge or @ref NOTINDEX if it is not an N-gon edge.
	/// @param[in] vadr								The polygon array. @callerOwnsPointed{array}
	/// @param[out] p1								Assigned the first point ID.
	/// @param[out] p2								Assigned the second point ID.
	//----------------------------------------------------------------------------------------
	void GetEdge(const CPolygon* vadr, Int32& p1, Int32& p2);

	UInt32 edge_index;	///< Edge index.
};

#pragma pack (pop)
}

#endif	// C4D_PGONEDGE_H__
