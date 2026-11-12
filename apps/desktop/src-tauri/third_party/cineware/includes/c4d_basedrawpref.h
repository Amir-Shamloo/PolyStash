/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEDRAWPREF_H__
#define C4D_BASEDRAWPREF_H__

#include "c4d_system.h"
#include "private_symbols.h"
#include "c4d_filename.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class BaseDrawPref
{
public:
	BaseDrawPref();
	virtual ~BaseDrawPref();

	Bool						 disable_textures, disable_backcull, show_safe, m_bHQOGL;
	DISPLAYMODE			 display_a;	// how to draw active objects
	DISPLAYMODE			 display_i;	// how to draw inactive objects
	Bool						 usetags_a;
	Bool						 usetags_i;
	Bool						 show_normals;
	Bool						 show_path;
	Bool						 show_picture;
	Bool						 xray;
	Filename				 fname;	// backpicture
	Float						 offx, offy, sizex, sizey;

	Bool						 dirty;
	Vector					 quicklightvector;
	Bool						 stacked_materials;

	DISPLAYFILTER		 displayfilter;
	Bool						 tint_selection;
	Bool						 tint_normals;
	Int32						 point_size;
	Int32						 point_selsize;
	Int32						 edge_selsize;
	DISPLAYEDITSTATE editstate_flags;

	UInt32					 lastdirty;

	Bool						 edgepoints;
	Bool						 selected_normals;
	Bool						 wfsel;
	Bool						 wfchild;
	Bool						 bbsel;
	Bool						 bbchild;
	Bool						 use_layercolor;
	Bool						 outline;
	Float						 outline_width;

	// Cache only
	Int32 m_lHQTransparency;
	Bool	m_bHQPostEffects, m_bHQNoises;
	Bool	m_bHQTessellation;
	Bool	m_bHQSSAO;
	Bool	m_bHQReflections;

	Bool	legacy_grid;

	Float highlightFillOpacity;
	Float selectionFillOpacity;
	Float highlightOutlineOpacity;
	Float selectionOutlineOpacity;

	Bool	show_vertex_normals;
	Bool	selected_vertex_normals;

	virtual Bool Write(HyperFile* hf);
	virtual Bool Read (HyperFile* hf, Int32 id, Int32 level);
};

#pragma pack (pop)
}

#endif	// C4D_BASEDRAWPREF_H__
