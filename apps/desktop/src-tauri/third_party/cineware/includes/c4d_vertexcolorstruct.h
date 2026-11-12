/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef VERTEXCOLORSTRUCT_H__
#define VERTEXCOLORSTRUCT_H__

#include "vector4.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a polygon and its vertex color values.
/// @since 18.002
//----------------------------------------------------------------------------------------
struct VertexColorStruct
{
	Vector4d32 a, b, c, d;

	VertexColorStruct() { }
	VertexColorStruct(_DONTCONSTRUCT dc) : a(DC), b(DC), c(DC), d(DC) { }
	VertexColorStruct(const Vector64& t_a, const Vector64& t_b, const Vector64& t_c, const Vector64& t_d) { a = Vector4d32(t_a); b = Vector4d32(t_b); c = Vector4d32(t_c); d = Vector4d32(t_d); }
	VertexColorStruct(const Vector32& t_a, const Vector32& t_b, const Vector32& t_c, const Vector32& t_d) { a = Vector4d32(t_a); b = Vector4d32(t_b); c = Vector4d32(t_c); d = Vector4d32(t_d); }
	VertexColorStruct(const Vector4d32& t_a, const Vector4d32& t_b, const Vector4d32& t_c, const Vector4d32& t_d) { a = t_a; b = t_b; c = t_c; d = t_d; }

	Vector4d32& operator[](Int32 index) const { return ((Vector4d32*)this)[index & 3]; }
};

#pragma pack (pop)
}

#endif	// VERTEXCOLORSTRUCT_H__
