/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef UVWSTRUCT_H__
#define UVWSTRUCT_H__

#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

/// Handle/pointer to UVW data for UVWTag.
typedef void* UVWHandle;

/// Const handle/pointer to UVW data for UVWTag.
typedef const void* ConstUVWHandle;

//----------------------------------------------------------------------------------------
/// Struct holding UVW data.
//----------------------------------------------------------------------------------------
struct UVWStruct
{
public:
	//----------------------------------------------------------------------------------------
	/// Creates an empty (all zero) UVW variable.
	//----------------------------------------------------------------------------------------
	UVWStruct();

	//----------------------------------------------------------------------------------------
	/// Constructs the UVW data with the coordinates for a quadrangle.
	/// @param[in] t_a								The coordinates of the first point.
	/// @param[in] t_b								The coordinates of the second point.
	/// @param[in] t_c								The coordinates of the third point.
	/// @param[in] t_d								The coordinates of the fourth point.
	//----------------------------------------------------------------------------------------
	UVWStruct(const Vector& t_a, const Vector& t_b, const Vector& t_c, const Vector& t_d);

	//----------------------------------------------------------------------------------------
	/// Constructs the UVW data with the coordinates for a triangle.
	/// @param[in] t_a								The coordinates of the first point.
	/// @param[in] t_b								The coordinates of the second point.
	/// @param[in] t_c								The coordinates of the third point.
	//----------------------------------------------------------------------------------------
	UVWStruct(const Vector& t_a, const Vector& t_b, const Vector& t_c);

	Vector a;	///< The UVW coordinate for the first point in the polygon.
	Vector b;	///< The UVW coordinate for the second point in the polygon.
	Vector c;	///< The UVW coordinate for the third point in the polygon.
	Vector d;	///< The UVW coordinate for the fourth point in the polygon.
};

#pragma pack (pop)
}

#endif	// UVWSTRUCT_H__
