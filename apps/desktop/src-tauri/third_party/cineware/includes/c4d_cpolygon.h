/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CPOLYGON_H__
#define C4D_CPOLYGON_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a polygon that can be either a triangle or quadrangle.
//----------------------------------------------------------------------------------------
struct CPolygon
{
	//----------------------------------------------------------------------------------------
	/// Constructs a triangle.
	/// @param[in] t_a								Index of the first point in the polygon.
	/// @param[in] t_b								Index of the second point in the polygon.
	/// @param[in] t_c								Index of the third point in the polygon.
	//----------------------------------------------------------------------------------------
	CPolygon(Int32 t_a, Int32 t_b, Int32 t_c);

	//----------------------------------------------------------------------------------------
	/// Constructs a quadrangle.
	/// @param[in] t_a								Index of the first point in the polygon.
	/// @param[in] t_b								Index of the second point in the polygon.
	/// @param[in] t_c								Index of the third point in the polygon.
	/// @param[in] t_d								Index of the third point in the polygon.
	//----------------------------------------------------------------------------------------
	CPolygon(Int32 t_a, Int32 t_b, Int32 t_c, Int32 t_d);

	Int32 a,	///< Index of the first point in the polygon.
				b,	///< Index of the second point in the polygon.
				c,	///< Index of the third point in the polygon.
				d;	///< Index of the fourth point in the polygon.

	//----------------------------------------------------------------------------------------
	/// Vertex index array operator.\n
	/// Accesses the polygon's vertex indices through their polygon vertex numbers (@em 0 - @em 3) instead of @ref a, @ref b, @ref c, @ref d.
	/// @since 18.002
	/// @param[in] index							The vertex index (@em 0 - @em 3) to retrieve in the polygon.
	/// @return												The vertex index.
	//----------------------------------------------------------------------------------------
	Int32& operator[](Int32 index) const { return ((Int32*)this)[index & 3]; }
};

#pragma pack (pop)
}

#endif	// C4D_CPOLYGON_H__
