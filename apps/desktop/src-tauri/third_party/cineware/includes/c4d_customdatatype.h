/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CUSTOMDATATYPE_H__
#define C4D_CUSTOMDATATYPE_H__

#include "c4d_memory.h"
#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup c4d_system_customdatatype Custom Data Type
/// @ingroup group_topic
/// @{
struct CustomDataType
{
};

template <class X> struct iCustomDataType : public CustomDataType
{
	static X* Alloc()
	{
		return NewObj(X);
	}
	static void Free(X*& data)
	{
		DeleteObj(data);
	}
};
/// @}


#pragma pack (pop)
}

#endif	// C4D_CUSTOMDATATYPE_H__

