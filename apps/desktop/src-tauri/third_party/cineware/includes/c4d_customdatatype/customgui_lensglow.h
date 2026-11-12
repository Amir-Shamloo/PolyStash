/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_LENSGLOW_H__
#define CUSTOMGUI_LENSGLOW_H__

#include "c4d_customdatatype.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

#define CUSTOMDATATYPE_LENSGLOW 1000997

#define MAXGLOW_CNT		5
#define MAXSTREAK_CNT	5

struct Glow
{
	Float	 size, sv;
	Vector col;
	Int32	 type;
};

struct Streak
{
	Bool	 random, randomlen, star;
	Int32	 cnt, holecnt;
	Float	 size, sv, holewidth, thickness, rot;
	Vector col;
	Int32	 type;
};

struct Halo
{
	Float	 size, sv;
	Vector col;
	Int32	 type;
};

class LensGlowStruct : public CustomDataType
{
public:
	Glow	 glow[MAXGLOW_CNT];
	Streak streak[MAXSTREAK_CNT];
	Halo	 halo;
};

void LGSGlowSet(LensGlowStruct& l, Int32 glownum);

/// @markInternal
class LensGlowDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_LENSGLOW_H__
