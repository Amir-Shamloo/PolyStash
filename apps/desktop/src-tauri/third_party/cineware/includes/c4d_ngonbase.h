/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_NGONBASE_H__
#define C4D_NGONBASE_H__

#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

class Pgon;
struct CPolygon;
class NgonBase;
class HyperFile;

//----------------------------------------------------------------------------------------
/// Contains functions for handling N-gons.
//----------------------------------------------------------------------------------------
class NgonBase
{
private:
	Pgon* m_pNgons;
	Int32 m_nCount;

	NgonBase();
	~NgonBase();

	void FreeData();

public:
	//----------------------------------------------------------------------------------------
	/// @allocatesAN{NgonBase instance}
	/// @note Normally this function is not needed, since the PolygonObject manages its own NgonBase.
	/// @return												@allocReturn{NgonBase instance}
	//----------------------------------------------------------------------------------------
	static NgonBase* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{NgonBase instances}
	/// @param[in,out] p							@theToDestruct{NgonBase instance}
	//----------------------------------------------------------------------------------------
	static void Free(NgonBase*& p);

	//----------------------------------------------------------------------------------------
	/// Copies the N-gon data to another NgonBase.
	/// @param[in,out] dst						Destination N-gon helper.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool CopyTo(NgonBase* dst);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Gets the number of N-gons.
	/// @return												The N-gon count.
	//----------------------------------------------------------------------------------------
	Int32 GetCount();

	//----------------------------------------------------------------------------------------
	/// Gets the array of Pgon data.
	/// @return												The N-gon array. @theOwnsPointed{N-gon base,array}
	//----------------------------------------------------------------------------------------
	Pgon* GetNgons();

	//----------------------------------------------------------------------------------------
	/// Resizes the Pgon data array.
	/// @param[in] cnt								New N-gon count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 cnt);

	//----------------------------------------------------------------------------------------
	/// Searches for polygon index @formatParam{id} in all N-gons.
	/// @param[in] id									A polygon ID.
	/// @return												The found N-gon index, or @ref NOTOK if the polygon was not found in any N-gon.
	//----------------------------------------------------------------------------------------
	Int32 FindPolygon(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Creates a new N-gon (appended to the N-gon array) from the edge array passed. The marker bits for the segment end and no edges must be correctly set.
	/// @param[in] nedges							New edge array. @callerOwnsPointed{array}
	/// @param[in] ncnt								Edge array @formatParam{nedges} size.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Create(UInt32* nedges, Int32 ncnt);

	//----------------------------------------------------------------------------------------
	/// Builds an N-gon from a list of polygons and optionally the outer edges.
	/// @markTODO{Add (fixed) example}
	/// @param[in] polys							An array of polygons that form the N-gon. @callerOwnsPointed{array}
	/// @param[in] outer							An array of the outline of the N-gon. Use the internal mark (@ref PGONEDGE_ENDSEGMENT) to mark segments in the N-gon outline. @callerOwnsPointed{array}
	/// @param[in] pcnt								Size of @formatParam{polys} array.
	/// @param[in] ocnt								Size of @formatParam{outer} array.
	/// @param[in] vadr								Polygon array. @callerOwnsPointed{array}
	/// @param[in] padr								Point array. @callerOwnsPointed{array}
	/// @param[in] hpolys							An array of polygons that surround holes in an N-gon. @callerOwnsPointed{array}
	/// @param[in] inner							An array of point indices that form the outline of holes in an N-gon. @callerOwnsPointed{array}
	/// @param[in] hpcnt							Number of polygons in @formatParam{hpolys} array.
	/// @param[in] icnt								Number of point indices in @formatParam{inner} array.
	/// @return												Index of the added N-gon, or @ref NOTOK if the operation failed.
	//----------------------------------------------------------------------------------------
	Int32 BuildNgonFromPolys(Int32* polys, Int32* outer, Int32 pcnt, Int32 ocnt, const CPolygon* vadr, const Vector* padr, Int32* hpolys = nullptr, Int32* inner = nullptr, Int32 hpcnt = 0, Int32 icnt = 0);
};

#pragma pack (pop)
}

#endif	// C4D_NGONBASE_H__
