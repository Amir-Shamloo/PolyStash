/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_HNDATA_H__
#define C4D_HNDATA_H__

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Holds the weights of the edges for a polygon.
/// @note For triangles @ref a, @ref b and @ref d contain valid data.
/// @since R19
//----------------------------------------------------------------------------------------
struct PolyWeight
{
	Float32 a,	///< The weight of the first edge.
					b,	///< The weight of the second edge.
					c,	///< The weight of the third edge.
					d;	///< The weight of the fourth edge.
};

//----------------------------------------------------------------------------------------
/// Holds HyperNURBS weight tag data.
/// @since R19
//----------------------------------------------------------------------------------------
struct HNData
{
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	HNData() { points = polys = nullptr; pointweight = nullptr; polyweight = nullptr; changed = nullptr; }

	Int32*				points,				///< Points to the point count.
	*							polys;				///< Points to the polygon count.
	Float32**			pointweight;	///< Points to an array of *@ref points weights, one for each point.
	PolyWeight**	polyweight;		///< Points to an array of *@ref polys polygon weights, one for each polygon.
	Bool*					changed;			///< Points to a flag to set if any of the values are changed.
};

#pragma pack (pop)
}

#endif	// C4D_HNDATA_H__
