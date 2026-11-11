/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASESETTINGS_H__
#define C4D_BASESETTINGS_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDocument;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class BaseSettings : public NodeData
{
public:
	// ------------ internal stuff do not touch --------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool Init();
	virtual void Free();
	Bool Copy(BaseSettings* dest);

	// ----------------  basic data --------------------------
	Int32					b_type;
	Bool					initialised;

	String				search_original;
	String				search_lowercase;

	Int32					iconsize;	//0=small, 1=medium, 2=large
	Bool					show_filter;
	Bool					show_source;
	Bool					show_path;
	Bool					show_find;
	BaseContainer weights;

	Bool					auto_h;

	// ----------------  for overriding -----------------------
	virtual Bool BS_Init() = 0;
	virtual void BS_Free() = 0;
	virtual Bool BS_CopyTo(BaseSettings* dest) = 0;
	virtual Bool BS_ReadfromContainer(BaseContainer* bc, BaseDocument* doc) = 0;
	virtual Bool BS_WriteToContainer(BaseContainer* bc, BaseDocument* doc)	= 0;
};

#pragma pack (pop)
}

#endif	// C4D_BASESETTINGS_H__
