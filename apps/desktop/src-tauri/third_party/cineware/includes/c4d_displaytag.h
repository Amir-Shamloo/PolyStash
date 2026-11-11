/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DISPLAYTAG_H__
#define C4D_DISPLAYTAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum C4DDISPLAYFLAGS
{
	C4DDISPLAYFLAGS_0									= 0,
	C4DDISPLAYFLAGS_AFFECT_LOD				= (1 << 0),
	C4DDISPLAYFLAGS_AFFECT_DISPLAY		= (1 << 1),
	C4DDISPLAYFLAGS_AFFECT_BACKCULL		= (1 << 2),
	C4DDISPLAYFLAGS_AFFECT_TEXTURES		= (1 << 3),
	C4DDISPLAYFLAGS_AFFECT_VISIBILITY	= (1 << 4),
	C4DDISPLAYFLAGS_AFFECT_HQOGL			= (1 << 5),
	C4DDISPLAYFLAGS_V_BACKCULL				= (1 << 6),
	C4DDISPLAYFLAGS_V_TEXTURES				= (1 << 7),
	C4DDISPLAYFLAGS_V_HQOGL						= (1 << 8),
	C4DDISPLAYFLAGS_RECEIVE_SHADOWS		= (1 << 9)
} ENUM_END_FLAGS(C4DDISPLAYFLAGS);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class C4DDisplay
{
public:
	C4DDisplay();

	C4DDISPLAYFLAGS	m_displaybits;
	DISPLAYMODE			v_display;
	Float32					v_lod, v_visibility;	// keep as Float32 so that structure is as small as possible
};


//----------------------------------------------------------------------------------------
/// Display tag.
//----------------------------------------------------------------------------------------
class DisplayTag : public BaseTag
{
	INSTANCEOF(DisplayTag, BaseTag)

private:
	C4DDisplay display;

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	DisplayTag();

	/// @}

	/// @name Get/Set Parameter
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] data								Assigned the parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter was retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetDParameter(const DescID& id, GeData& data);

	//----------------------------------------------------------------------------------------
	/// Sets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] data								The parameter data to set.
	/// @return												@trueIfOtherwiseFalse{the parameter was set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	/// @}

	/// @name Private
	/// @{

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

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_DISPLAYTAG_H__
