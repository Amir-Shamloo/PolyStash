/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_EFFECTORPTR_H__
#define C4D_EFFECTORPTR_H__

#include "private_safebaseptr.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup EFFECTORPTR_FLAG
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define EFFECTORPTR_FLAG_SELECTED	(1 << 0)
#define EFFECTORPTR_FLAG_FOUND		(1 << 1)	// Temp flag.
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class EffectorPtr : public SafeBasePtr
{
public:
	EffectorPtr();
	EffectorPtr(const EffectorPtr& src);
	EffectorPtr& operator =(const EffectorPtr& src);

	Bool CopyFrom(const EffectorPtr& src, AliasTrans* trn);

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);

	Int32 m_Flags;
};

#pragma pack (pop)
}

#endif	// C4D_EFFECTORPTR_H__
