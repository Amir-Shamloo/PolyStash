/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LAYERDATA_H__
#define C4D_LAYERDATA_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Structure to store layer's data.
//----------------------------------------------------------------------------------------
struct LayerData
{
public:
	Bool	 solo;				///< Solo mode.
	Bool	 view;				///< Visible in editor view.
	Bool	 render;			///< Visible in render view.
	Bool	 manager;			///< Visible in Object Manager and Timeline.
	Bool	 locked;			///< Not selectable and no modification possible - grayed-out in Object Manager.
	Bool	 generators;	///< Generators on /off.
	Bool	 deformers;		///< Deformers on /off.
	Bool	 expressions;	///< Expressions on /off.
	Bool	 animation;		///< Animation on/off.

	Vector color;				///< Layer color.

	Bool	 xref;				///< Import with XRef.

	LayerData();

	Bool operator == (const LayerData& d) const
	{
		return d.solo == solo && d.view == view && d.render == render && d.manager == manager && d.locked == locked &&
					 d.generators == generators && d.deformers == deformers && d.expressions == expressions &&
					 d.animation == animation && d.color == color && d.xref == xref;
	}

	Bool operator != (const LayerData& d) const
	{
		return !(d == *this);
	}
};

#pragma pack (pop)
}

#endif	// C4D_LAYERDATA_H__
