/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_ROOTMATERIAL_H__
#define C4D_ROOTMATERIAL_H__

#include "c4d_rootlist2d.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseMaterial;

//----------------------------------------------------------------------------------------
/// Almost private except AllocObject().
//----------------------------------------------------------------------------------------
class RootMaterial : public RootList2D
{
	INSTANCEOF(RootMaterial, RootList2D)

public:
	//----------------------------------------------------------------------------------------
	/// Overload this method to allocate a material for the root/list.
	/// <b>Example</b>:
	/// @code
	/// BaseMaterial *AlienRootMaterial::AllocObject(Int32 Mtype)
	/// {
	///   BaseMaterial* mat = nullptr;
	///
	///	if (Mtype==Mplugin)
	///     mat = gNew AlienBaseMaterial(Mplugin);
	///   else
	///     mat = NewObj(AlienMaterial);
	///
	///	return mat;
	/// }
	/// @endcode
	/// @param[in] Mtype							The material type ID.
	/// @return												The allocated material.
	//----------------------------------------------------------------------------------------
	virtual BaseMaterial* AllocObject(Int32 Mtype);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);
};

#pragma pack (pop)
}

#endif	// C4D_ROOTMATERIAL_H__
