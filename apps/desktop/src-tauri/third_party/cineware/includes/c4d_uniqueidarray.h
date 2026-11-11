/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_UNIQUEIDARRAY_H__
#define C4D_UNIQUEIDARRAY_H__

namespace cineware
{
#pragma pack (push, 8)

struct UniqueIDEntry
{
	Int32 appid;
	Int		idsize;
	Char* idmem;

	UniqueIDEntry()
	{
		appid	 = 0;
		idsize = 0;
		idmem	 = nullptr;
	}

	~UniqueIDEntry()
	{
		DeleteMem(idmem);
	}
};

class UniqueIDArray : public GeTempDynSortSearchArray<UniqueIDEntry, Int32, true>
{
	virtual Int32 CompareFunc(UniqueIDEntry* aa, UniqueIDEntry* bb)
	{
		if (aa->appid < bb->appid)
			return -1;
		else if (aa->appid > bb->appid)
			return 1;
		return 0;
	}

	virtual Int32 SearchFunc(Int32* data, UniqueIDEntry* bb)
	{
		if (*data < bb->appid)
			return -1;
		else if (*data > bb->appid)
			return 1;
		return 0;
	}
};

#pragma pack (pop)
}

#endif	// C4D_UNIQUEIDARRAY_H__
