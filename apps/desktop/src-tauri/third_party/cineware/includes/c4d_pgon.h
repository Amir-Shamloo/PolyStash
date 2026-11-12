/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_PGON_H__
#define C4D_PGON_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

struct CPolygon;
class PgonEdge;

//----------------------------------------------------------------------------------------
/// An N-gon represented as a list of edges.
//----------------------------------------------------------------------------------------
class Pgon
{
	friend class NgonBase;

private:
	PgonEdge*	m_Edge;
	Int32			m_Count;

	Bool Alloc(Int32 cnt);
	void Free();
	Bool CopyFrom(const Pgon* src);

	Pgon();
	~Pgon();

public:
	/// @name Edge Access
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of elements in the array (all PgonEdges in all segments).
	/// @note	This is not the same as all N-gon edges since some elements may be from internal polygons with no outer/inner N-gon edge.
	/// @return												The number of elements in the N-gon.
	//----------------------------------------------------------------------------------------
	Int32 GetCount();

	//----------------------------------------------------------------------------------------
	/// Gets direct access to a PgonEdge.
	/// @note The helper methods below should be used.
	/// @param[in] i									An edge index.
	/// @return												The N-gon edge.
	//----------------------------------------------------------------------------------------
	PgonEdge* GetEdge(Int32 i);

	/// @}

	/// @name PgonEdge Convenience Methods
	/// @{

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::ID() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												The polygon ID.
	//----------------------------------------------------------------------------------------
	Int32 ID(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::State() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												The edge state.
	//----------------------------------------------------------------------------------------
	Int32 State(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::Edge() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												The edge of the polygon.
	//----------------------------------------------------------------------------------------
	Int32 Edge(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::EdgeIndex() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												The edge index (4*polygon ID + edge).
	//----------------------------------------------------------------------------------------
	Int32 EdgeIndex(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::IsFirst() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												@trueIfOtherwiseFalse{the polygon ID is the first occurrence in the array}
	//----------------------------------------------------------------------------------------
	Int32 IsFirst(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::IsSegmentEnd() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @return												The PgonEdge::IsSegmentEnd() state.
	//----------------------------------------------------------------------------------------
	Int32 IsSegmentEnd(Int32 i);

	//----------------------------------------------------------------------------------------
	/// Gets the first polygon ID in the array.\n
	/// Useful as a marker (identity) for the N-gon giving a polygon ID that is within the N-gon.
	/// @return												The first polygon ID in the array.
	//----------------------------------------------------------------------------------------
	Int32 GetIdentity();

	//Int32 GetSegmentCount();
	//Int32 GetSegmentLen(Int32 startedge);

	//----------------------------------------------------------------------------------------
	/// Convenience method for PgonEdge::GetEdge() for the edge at index @formatParam{i}.
	/// @param[in] i									An edge index.
	/// @param[in] vadr								The polygon array. @callerOwnsPointed{array}
	/// @param[in] p1									Assigned the first edge point index.
	/// @param[in] p2									Assigned the second edge point index.
	//----------------------------------------------------------------------------------------
	void GetEdge(Int32 i, const CPolygon* vadr, Int32& p1, Int32& p2);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Does a search to see if the polygon with index @formatParam{id} exists within the N-gon.\n
	/// @note	The look up uses tables so it is pretty fast.
	/// @param[in] id									A polygon ID to search for.
	/// @return												The index of the first edge found with the specified polygon @formatParam{id}, or @ref NOTOK if no edge was found.
	//----------------------------------------------------------------------------------------
	Int32 FindPolygon(Int32 id);

	//UChar GetEdgeState(Int32 id);
	//Int32 GetPointCount();
	//Int32 GetPolygonCount();

	/// @}

	/// @name Update
	/// @{

	//----------------------------------------------------------------------------------------
	/// If any changes are done to the edges this must be called when completed to update the @em repeat markers of the N-gon edges.\n
	/// Also note that any changes must correct set/change any of the marker bits (such as the segment end, or no edges markers).
	//----------------------------------------------------------------------------------------
	void UpdateStates();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_PGON_H__
