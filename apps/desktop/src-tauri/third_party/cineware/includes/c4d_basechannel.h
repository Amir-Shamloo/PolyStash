/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASECHANNEL_H__
#define C4D_BASECHANNEL_H__

#include "c4d_privatechunk.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a channel in a material. Call Material::GetChannel() to retrieve an instance.
//----------------------------------------------------------------------------------------
class BaseChannel : public PrivateChunk
{
	INSTANCEOF(BaseChannel, PrivateChunk)

private:
	BaseContainer m_bc;

public:
	/// @name GetType
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the channel.
	/// @warning	Must be called to make sure that the derived channel really is of the right type before casting it and trying to access its members.
	/// @return												The channel type.
	//----------------------------------------------------------------------------------------
	Int32 GetType() const;

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASECHANNEL_H__
