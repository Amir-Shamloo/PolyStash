/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_FILTERBMP_H__
#define PRIVATE_FILTERBMP_H__

#include "c4d_filterdata.h"

namespace cineware
{
#pragma pack (push, 8)

//////////////////////////////////////////////////////////////////////////

class BmpLoader : public BitmapLoaderData
{
public:
	virtual Bool Identify(const Filename& name, UChar* probe, Int32 size);
	virtual IMAGERESULT Load (const Filename& name, BaseBitmap* bm, Int32 frame);
	virtual Int32 GetSaver()
	{
		return FILTER_BMP;
	}
};

class BmpSaver : public BitmapSaverData
{
public:
	virtual IMAGERESULT Save(const Filename& name, BaseBitmap* bm, BaseContainer* data, SAVEBIT savebits);
	virtual Int32 GetMaxResolution(Bool layers)
	{
		return 32000;
	}
};

//////////////////////////////////////////////////////////////////////////

#pragma pack (pop)
}

#endif	// PRIVATE_FILTERBMP_H__
