/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATECHUNK_H__
#define PRIVATECHUNK_H__

#include "c4d_system.h"
#include "private_ge_mtools.h"
#include "c4d_gedata.h"
#include "private_symbols.h"

namespace cineware
{
#pragma pack (push, 8)

class DescID;

//----------------------------------------------------------------------------------------
/// Mostly @b private with the exception of GetParameter()/@ref SetParameter():
/// @code
/// if (op->SetParameter(12345, GeData(Vector(1.0,2.0,3.0))))
/// {
///   if (op->GetParameter(12345, data))
///   {
///     Vector v = data.GetVector();
///     printf("Vector: %f %f %f\n", v.x, v.y, v.z);
///
///     data = 5.0;
///     if (op->SetParameter(DescID(DescLevel(12345, DTYPE_VECTOR,0), DescLevel(VECTOR_Y)), data))
///     {
///       if (op->GetParameter(12345, data))
///       {
///         v = data.GetVector();
///         printf("Vector: %f %f %f\n", v.x, v.y, v.z);
///       }
///
///       if (op->GetParameter(DescID(DescLevel(12345, DTYPE_VECTOR, 0), DescLevel(VECTOR_Z)), data))
///         printf("Vector: %f", data.GetReal());
///     }
///   }
/// }
/// @endcode
//----------------------------------------------------------------------------------------
class PrivateChunk : public GeTempPublicNode2D<PrivateChunk>
{
	INSTANCEOFROOT(PrivateChunk)

protected:
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);	// Return true means it knows this chunk, else it return false.

public:
	PrivateChunk();
	Bool ReadChunk(HyperFile* hf, Bool keepin = false, Bool noheader = false);	// reads chunks in a loop until HYPERFILEVALUE_STOP, calls ReadData

	virtual Bool IsListHead() { return false; }
	virtual Int32 GetType() const { return 0; }
	virtual Int32 GetInstanceType() const { return 0; }

	Bool ReadObject(HyperFile* hf, Bool readheader);
	Bool WriteObject(HyperFile* hf, Int32 type);
	virtual Bool Write(HyperFile* hf);

	Bool CopyTo(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	Bool GetParameter(Int32 id, GeData& data);
	Bool SetParameter(Int32 id, const GeData& data);

	Bool GetParameter(const DescID& id, GeData& data)
	{
		return GetDParameter(id, data);
	}
	Bool SetParameter(const DescID& id, const GeData& data)
	{
		return SetDParameter(id, data);
	}

	virtual Int32 GetChunkLevel()
	{
		return 0;
	}
};

#pragma pack (pop)
}

#endif	// PRIVATECHUNK_H__
