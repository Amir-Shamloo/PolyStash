/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_PARAMETER_IDS_H__
#define C4D_PARAMETER_IDS_H__

namespace cineware
{
#pragma pack (push, 8)

// scene objects
#include "parameter_ids/object/obaselist.h"						// BaseList2D
#include "parameter_ids/object/obase.h"								// BaseObject
#include "parameter_ids/object/obaseeffector.h"				// BaseEffector
#include "parameter_ids/object/ocamera.h"							// Ocamera (CameraObjectData)
#include "parameter_ids/object/oprimitiveaxis.h"			// (deprecated)
#include "parameter_ids/object/oprimitiveslice.h"			// Otorus, Otube, Ocone, Odisc, Ocylinder, Ocapsule, Ooiltank
#include "parameter_ids/object/oskyshader.h"					// Oskyshader
#include "parameter_ids/object/oconnector.h"					// Oconnector
#include "parameter_ids/object/oalembicgenerator.h"		// Oalembicgenerator
#include "parameter_ids/object/omograph_splinewrap.h"	// Osplinewrap
#include "parameter_ids/object/olight.h"							// Olight
#include "parameter_ids/object/cnurbsobject.h"				// CNURBS_OBJECT_ID (Clothnurbs Object)
#include "parameter_ids/object/oarray.h"							// Oarray
#include "parameter_ids/object/oatomarray.h"					// Oatomarray
#include "parameter_ids/object/oattractor.h"					// Oattractor
#include "parameter_ids/object/obackground.h"					// Obackground
#include "parameter_ids/object/obasedeform.h"					// Obasedeform
#include "parameter_ids/object/obend.h"								// Obend
#include "parameter_ids/object/obezier.h"							// Obezier
#include "parameter_ids/object/obone.h"								// Obone (deprecated)
#include "parameter_ids/object/oboole.h"							// Oboole
#include "parameter_ids/object/obulge.h"							// Obulge
#include "parameter_ids/object/ocapsule.h"						// Ocapsule
#include "parameter_ids/object/ocloud.h"							// Ocloud
#include "parameter_ids/object/ocloudgroup.h"					// Ocloudgroup
#include "parameter_ids/object/ocone.h"								// Ocone
#include "parameter_ids/object/ocube.h"								// Ocube
#include "parameter_ids/object/ocylinder.h"						// Ocylinder
#include "parameter_ids/object/odeflector.h"					// Odeflector
#include "parameter_ids/object/odestructor.h"					// Odestructor
#include "parameter_ids/object/odisc.h"								// Odisc
#include "parameter_ids/object/oenvironment.h"				// Oenvironment
#include "parameter_ids/object/oexplosion.h"					// Oexplosion
#include "parameter_ids/object/oexplosionfx.h"				// Oexplosionfx
#include "parameter_ids/object/oextrude.h"						// Oextrude
#include "parameter_ids/object/ofalloff_object.h"			// Ofalloff
#include "parameter_ids/object/ofalloff_panel.h"			// Ofalloff
#include "parameter_ids/object/ofalloff_standard.h"		// Ofalloff
#include "parameter_ids/object/onoisefalloff.h"				// Ofalloff
#include "parameter_ids/object/offd.h"								// Offd
#include "parameter_ids/object/ofield.h"							// Ofield
#include "parameter_ids/object/ofieldforce.h"					// Ofieldforce
#include "parameter_ids/object/ofigure.h"							// Ofigure
#include "parameter_ids/object/ofloor.h"							// Ofloor
#include "parameter_ids/object/oforeground.h"					// Oforeground
#include "parameter_ids/object/oformula.h"						// Oformula
#include "parameter_ids/object/ofractal.h"						// Ofractal
#include "parameter_ids/object/ofriction.h"						// Ofriction
#include "parameter_ids/object/ogravitation.h"				// Ogravitation
#include "parameter_ids/object/oheadphone.h"					// Oheadphone
#include "parameter_ids/object/oinstance.h"						// Oinstance
#include "parameter_ids/object/olathe.h"							// Olathe
#include "parameter_ids/object/oloft.h"								// Oloft
#include "parameter_ids/object/oloudspeaker.h"				// Oloudspeaker
#include "parameter_ids/object/omelt.h"								// Omelt
#include "parameter_ids/object/ometaball.h"						// Ometaball
#include "parameter_ids/object/omicrophone.h"					// Omicrophone
#include "parameter_ids/object/onull.h"								// Onull
#include "parameter_ids/object/onurbscaps.h"					// Oextrude, ...
#include "parameter_ids/object/onurbscapslegacy.h"		// legacy Oextrude, Olathe, Osweep
#include "parameter_ids/object/ooiltank.h"						// Ooiltank
#include "parameter_ids/object/oparticle.h"						// Oparticle
#include "parameter_ids/object/oparticlebase.h"				// particle modifier
#include "parameter_ids/object/oplane.h"							// Oplane
#include "parameter_ids/object/oplatonic.h"						// Oplatonic
#include "parameter_ids/object/opolyreduction.h"			// Opolyreduction
#include "parameter_ids/object/opyramid.h"						// Opyramid
#include "parameter_ids/object/orelief.h"							// Orelief
#include "parameter_ids/object/orotation.h"						// Orotation
#include "parameter_ids/object/osceneinstance.h"			// Oxref
#include "parameter_ids/object/osds.h"								// Osds
#include "parameter_ids/object/oselection.h"					// Oselection
#include "parameter_ids/object/oshatter.h"						// Oshatter
#include "parameter_ids/object/oshear.h"							// Oshear
#include "parameter_ids/object/osinglepoly.h"					// Osinglepoly
#include "parameter_ids/object/osky.h"								// Osky
#include "parameter_ids/object/osphere.h"							// Osphere
#include "parameter_ids/object/ospherify.h"						// Ospherify
#include "parameter_ids/object/olod.h"								// Olod
#include "parameter_ids/object/ospline.h"							// Ospline
#include "parameter_ids/object/ospline4side.h"				// Ospline4side
#include "parameter_ids/object/osplinearc.h"					// Osplinearc
#include "parameter_ids/object/osplinecircle.h"				// Osplinecircle
#include "parameter_ids/object/osplinecissoid.h"			// Osplinecissoid
#include "parameter_ids/object/osplinecogwheel.h"			// Osplinecissoid
#include "parameter_ids/object/osplinecontour.h"			// Osplinecontour
#include "parameter_ids/object/osplinecycloid.h"			// Osplinecycloid
#include "parameter_ids/object/osplinedeformer.h"			// Osplinedeformer
#include "parameter_ids/object/osplineflower.h"				// Osplineflower
#include "parameter_ids/object/osplineformula.h"			// Osplineformula
#include "parameter_ids/object/osplinehelix.h"				// Osplinehelix
#include "parameter_ids/object/osplinenside.h"				// Osplinenside
#include "parameter_ids/object/osplineprimitive.h"		// Osplineprimitive
#include "parameter_ids/object/osplineprofile.h"			// Osplineprofile
#include "parameter_ids/object/osplinerail.h"					// Osplinerail
#include "parameter_ids/object/osplinerectangle.h"		// Osplinerectangle
#include "parameter_ids/object/osplinestar.h"					// Osplinestar
#include "parameter_ids/object/osplinetext.h"					// Osplinetext
#include "parameter_ids/object/ostage.h"							// Ostage
#include "parameter_ids/object/osweep.h"							// Osweep
#include "parameter_ids/object/osymmetry.h"						// Osymmetry
#include "parameter_ids/object/otaper.h"							// Otaper
#include "parameter_ids/object/otorus.h"							// Otorus
#include "parameter_ids/object/otube.h"								// Otube
#include "parameter_ids/object/oturbulence.h"					// Oturbulence
#include "parameter_ids/object/otwist.h"							// Otwist
#include "parameter_ids/object/ovolumebuilder.h"			// Ovolumebuilder
#include "parameter_ids/object/ovolume.h"							// Ovolume
#include "parameter_ids/object/ovolumeset.h"					// Ovolumeset
#include "parameter_ids/object/ovolumebuilder.h"			// Ovolumebuilder
#include "parameter_ids/object/owind.h"								// Owind
#include "parameter_ids/object/owinddeform.h"					// Owinddeform
#include "parameter_ids/object/owrap.h"								// Owrap

// ca objects
#include "parameter_ids/object/ocajoint.h"				// Ojoint (caJoint)
#include "parameter_ids/object/ocaskin.h"					// Oskin (SkinObjectData)
#include "parameter_ids/object/ocameshdeformer.h"	// CA_MESH_DEFORMER_OBJECT_ID (MeshDeformerObjectData)
#include "parameter_ids/tag/tcaweight.h"					// Tweights

// materials
#include "parameter_ids/material/mmaterial.h"				// Mmaterial
#include "parameter_ids/material/marchigrass.h"			// Marchigrass
#include "parameter_ids/material/millum.h"					// Millum
#include "parameter_ids/material/moutliner.h"				// Msketch (Sketch & Toon)
#include "parameter_ids/material/mfog.h"						// Mfog
#include "parameter_ids/material/mhairmaterial.h"		// Mhair
#include "parameter_ids/material/mshadowcatcher.h"	// Mshadowcatcher
#include "parameter_ids/material/mterrain.h"				// Mterrain

// shader
#include "parameter_ids/shader/xbase.h"										// Xbase
#include "parameter_ids/shader/xbitmap.h"									// Xbitmap
#include "parameter_ids/shader/xcolor.h"									// Xcolor
#include "parameter_ids/shader/xxmbsubsurface.h"					// Xxmbsubsurface
#include "parameter_ids/shader/xbrick.h"									// Xbrick
#include "parameter_ids/shader/xchanlum.h"								// Xchanlum
#include "parameter_ids/shader/xcheckerboard.h"						// Xcheckerboard
#include "parameter_ids/shader/xcloud.h"									// Xcloud
#include "parameter_ids/shader/xcolorstripes.h"						// Xcolorstripes (deprecated, don't use)
#include "parameter_ids/shader/xcyclone.h"								// Xcyclone
#include "parameter_ids/shader/xearth.h"									// Xearth
#include "parameter_ids/shader/xfire.h"										// Xfire
#include "parameter_ids/shader/xflame.h"									// Xflame
#include "parameter_ids/shader/xgalaxy.h"									// Xgalaxy
#include "parameter_ids/shader/xhair.h"										// Xhair
#include "parameter_ids/shader/xmarble.h"									// Xmarble
#include "parameter_ids/shader/xmetal.h"									// Xmetal
#include "parameter_ids/shader/xmosaic.h"									// Xmosaic
#include "parameter_ids/shader/xnoise.h"									// Xsimplenoise
#include "parameter_ids/shader/xnormalizer.h"							// Xnormalizer
#include "parameter_ids/shader/xpavement.h"								// Xpavement
#include "parameter_ids/shader/xplanet.h"									// Xplanet
#include "parameter_ids/shader/xrain.h"										// Xrainsampler
#include "parameter_ids/shader/xrust.h"										// Xrust
#include "parameter_ids/shader/xspectral.h"								// Xspectral
#include "parameter_ids/shader/xstar.h"										// Xstar
#include "parameter_ids/shader/xstarfield.h"							// Xstarfield
#include "parameter_ids/shader/xsunburst.h"								// Xsunburst
#include "parameter_ids/shader/xterrainmask.h"						// Xterrainmask
#include "parameter_ids/shader/xturbulence.h"							// Xsimpleturbulence
#include "parameter_ids/shader/xvenus.h"									// Xvenus
#include "parameter_ids/shader/xwater.h"									// Xwater
#include "parameter_ids/shader/xwood.h"										// Xwood
#include "parameter_ids/shader/xskyshader.h"							// Xskyshader
#include "parameter_ids/shader/xobjectcolor.h"						// Xobjectcolor
#include "parameter_ids/shader/xformula.h"								// Xformula
#include "parameter_ids/shader/xphlensdistortionshader.h"	// Xphlensdistortion / VPPhLensDistortion (VP using the same IDs)
#include "parameter_ids/shader/xvariation.h"							// Xvariation
#include "parameter_ids/shader/xthinfilm.h"								// Xthinfilm
#include "parameter_ids/shader/xsubstance.h"							// Xsubstance
// sla shader & materials
#include "parameter_ids/shader/xslabanj.h"								// Xtranslucency
#include "parameter_ids/shader/xslabanji.h"								// Mbanji
#include "parameter_ids/shader/xslabanzi.h"								// Mbanzi
#include "parameter_ids/shader/xslabuffy.h"								// Xnormaldirection
#include "parameter_ids/shader/xslacheen.h"								// Mcheen
#include "parameter_ids/shader/xslacolorizer.h"						// Xcolorizer (xslacolorizer.h)
#include "parameter_ids/shader/xsladanel.h"								// Mdanel
#include "parameter_ids/shader/xsladirt.h"								// Xdirt
#include "parameter_ids/shader/xsladistorter.h"						// Xdistorter (xsladistorter.h)
#include "parameter_ids/shader/xslafalloff.h"							// Xfalloff
#include "parameter_ids/shader/xslafilter.h"							// Xfilter
#include "parameter_ids/shader/xslafresnel.h"							// Xfresnel
#include "parameter_ids/shader/xslafusion.h"							// Xfusion
#include "parameter_ids/shader/xslagradient.h"						// Xgradient
#include "parameter_ids/shader/xslalayer.h"								// Xlayer
#include "parameter_ids/shader/xslalumas.h"								// Xlumas
#include "parameter_ids/shader/xslamabel.h"								// Mmabel
#include "parameter_ids/shader/xslanoise.h"								// Xnoise
#include "parameter_ids/shader/xslanukei.h"								// Mnukei
#include "parameter_ids/shader/xslaposterizer.h"					// Xposterizer (xslaposterrizer.h)
#include "parameter_ids/shader/xslaprojector.h"						// Xprojector (xslaprojector.h)
#include "parameter_ids/shader/xslaproximal.h"						// Xproximal
#include "parameter_ids/shader/xslaripple.h"							// Xripple
#include "parameter_ids/shader/xslaspline.h"							// Xspline
#include "parameter_ids/shader/xslatiles.h"								// Xtiles
#include "parameter_ids/shader/xslavertexmap.h"						// Xvertexmap
// advanced render
#include "parameter_ids/shader/xambientocclusion.h"				// Xambientocclusion
#include "parameter_ids/shader/xsss.h"										// Xsss (deprecated, don't use)
// sketch & toon
#include "parameter_ids/shader/xartshader.h"							// Xambientocclusion
#include "parameter_ids/shader/xcelshader.h"							// Xcelshader
#include "parameter_ids/shader/xhatchingshader.h"					// Xhatchingshader
#include "parameter_ids/shader/xspotshader.h"							// Xspotshader
// mograph
#include "parameter_ids/shader/xmg_beat.h"								// Xmobeat
#include "parameter_ids/shader/xmg_camera.h"							// Xmocamera
#include "parameter_ids/shader/xmg_color.h"								// Xmocolor
#include "parameter_ids/shader/xmg_multi.h"								// Xmomulti

// tags
#include "parameter_ids/tag/tannotation.h"				// Tannotation
#include "parameter_ids/tag/tarchigrass.h"				// Tarchigrass
#include "parameter_ids/tag/thairlight.h"					// Thairlight
#include "parameter_ids/tag/tvertexcolor.h"				// Tvertexcolor
#include "parameter_ids/tag/ttexture.h"						// Ttexture
#include "parameter_ids/tag/taligntopath.h"				// Taligntopath
#include "parameter_ids/tag/taligntospline.h"			// Taligntospline
#include "parameter_ids/tag/tcompositing.h"				// Tcompositing
#include "parameter_ids/tag/tctpobjectdata.h"			// ID_CTP_OBJECT_TAG
#include "parameter_ids/tag/tdisplay.h"						// Tdisplay
#include "parameter_ids/tag/tedgeselection.h"			// Tedgeselection
#include "parameter_ids/tag/tlookatcamera.h"			// Tlookatcamera
#include "parameter_ids/tag/tmetaball.h"					// Tmetaball
#include "parameter_ids/tag/tmotionblur.h"				// Tmotionblur
#include "parameter_ids/tag/toutliner.h"					// Tsketch
#include "parameter_ids/tag/tphong.h"							// Tphong
#include "parameter_ids/tag/tpointselection.h"		// Tpointselection
#include "parameter_ids/tag/tpolygonselection.h"	// Tpolygonselection
#include "parameter_ids/tag/tprotection.h"				// Tprotection
#include "parameter_ids/tag/trestriction.h"				// Trestriction
#include "parameter_ids/tag/tsds.h"								// Tsds
#include "parameter_ids/tag/tsticktexture.h"			// Tsticktexture
#include "parameter_ids/tag/tstop.h"							// Tstop
#include "parameter_ids/tag/tsunexpression.h"			// Tsunexpression
#include "parameter_ids/tag/ttargetexpression.h"	// Ttargetexpression
#include "parameter_ids/tag/tuvw.h"								// Tuvw
#include "parameter_ids/tag/tvibrate.h"						// Tvibrate
#include "parameter_ids/tag/tvertexmap.h"					// Tvertexmap
#include "parameter_ids/tag/tcaposemorph.h"				// Tposemorph

// rendering
#include "parameter_ids/document/drendersettings.h"			// RenderData
#include "parameter_ids/videopost/vpgisetup.h"					// VPglobalillumination
#include "parameter_ids/videopost/vpcaustics.h"					// VPcaustics
#include "parameter_ids/videopost/vpxmbsampler.h"				// physical renderer
#include "parameter_ids/videopost/distanceblur.h"				// VPopticsuite_depthoffield
#include "parameter_ids/videopost/glow.h"								// VPopticsuite_glow
#include "parameter_ids/videopost/vpgpurenderer.h"			// VPGPURenderer
#include "parameter_ids/videopost/highlights.h"					// VPopticsuite_highlights
#include "parameter_ids/videopost/motionblur.h"					// VPvectormotionblur
#include "parameter_ids/videopost/pvfilter.h"						// VPcolorcorrection
#include "parameter_ids/videopost/vpambientocclusion.h"	// VPambientocclusion
#include "parameter_ids/videopost/vpcolormapping.h"			// VPcolormapping
#include "parameter_ids/videopost/vpcomic.h"						// VPcelrender
#include "parameter_ids/videopost/vpcylinderlens.h"			// VPcylindricallens
#include "parameter_ids/videopost/vpdenoiser.h"					// VPDenoiser
#include "parameter_ids/videopost/vphairtrace.h"				// VPhair
#include "parameter_ids/videopost/vpmedianfilter.h"			// VPmedianfilter
#include "parameter_ids/videopost/vppreviewhardware.h"	// RDATA_RENDERENGINE_PREVIEWHARDWARE
#include "parameter_ids/videopost/vppreviewsoftware.h"	// RDATA_RENDERENGINE_PREVIEWSOFTWARE
#include "parameter_ids/videopost/vpremote.h"						// VPremote
#include "parameter_ids/videopost/vpscenemotionblur.h"	// VPscenemotionblur
#include "parameter_ids/videopost/vpsharpenfilter.h"		// VPsharpenfilter
#include "parameter_ids/videopost/vpsoftfilter.h"				// VPsoftfilter
#include "parameter_ids/videopost/vptonemapping.h"			// VPToneMapping
#include "parameter_ids/videopost/vppositionpass.h"			// VPpositionpass
#include "parameter_ids/videopost/vpnormalpass.h"				// VPnormalpass
#include "parameter_ids/videopost/vpwatermark.h"				// VPwatermark
#include "parameter_ids/rendersettings/zmultipass.h"		// multi pass rendering

// document
#include "parameter_ids/document/ddoc.h"			// BaseDocument
#include "parameter_ids/document/dbasedraw.h"	// BaseDraw
#include "parameter_ids/object/olayer.h"			// Olayer (LayerObject)

// animation
#include "parameter_ids/animation/ckbase.h"
#include "parameter_ids/animation/ckvalue.h"
#include "parameter_ids/animation/ctbase.h"
#include "parameter_ids/animation/ckpla.h"		// PLA Key
#include "parameter_ids/animation/ckmorph.h"	// Morth Key
#include "parameter_ids/animation/cksound.h"	// Sound Key
#include "parameter_ids/animation/ctsound.h"	// Sound Track

// data type
#include "parameter_ids/datatype/dgradient.h"	// Gradient

// if the cinema project was saved with the option "Save Animation for Cineware" with these IDs you get birth and death time of the object (e.g. cloner with animated count)
#define CINEWARE_ANIM_START	465001900
#define CINEWARE_ANIM_END		465001901

// UPDATE FUNCTIONALITY for document - triggered by Merge menu command in Cinema or GeExecuteC4D(..., Bool merge=true) call
enum
{
	// updating/merge options
	UPDATE_SCENE_ID														 = 1025177,		// LONG - marks the scene as update scene with application id
	UPDATE_SCENE_FLAGS												 = 1025562,		// LONG - additional options for updating algorithm
	UPDATE_SCENE_FLAG_CHECKNAMES							 = (1 << 0),	// adds a serial number to equal names at the same hierarchy level
	UPDATE_SCENE_FLAG_LAYERNAMES							 = (1 << 1),	// use layer names for objects if one assigned (the default layer name "None" will not be used)

	UPDATE_SCENE_FLAG_KEEP_OBJECTS_WITHOUT_UID = (1 << 2),	// only objects with UID will be deleted, so new added objects on cinema side will remain (this only works if you have UIDs all along your scene hierarchy)

	// set the one of the following flags ids you want to update this specific kind of data ( the option will enabled and checked in the update dialog on Cinema4D [>=13.053] side)
	UPDATE_SCENE_FLAG_DO_GEOMETRY	 = (1 << 3),	// enable and check dialog option update geometry
	UPDATE_SCENE_FLAG_DO_MATERIALS = (1 << 4),	// enable and check dialog option for update materials
	UPDATE_SCENE_FLAG_DO_LAYERS		 = (1 << 5),	// enable and check dialog option for update layers
	UPDATE_SCENE_FLAG_DO_LIGHTS		 = (1 << 6),	// enable and check dialog option for update light parameter
	UPDATE_SCENE_FLAG_DO_CAMERAS	 = (1 << 7),	// enable and check dialog option for update camera parameter
	UPDATE_SCENE_FLAG_DO_SKY			 = (1 << 8),	// enable and check dialog option for update sky parameter

	// the following options are only used if the equivalent DO flag is also set
	UPDATE_SCENE_FLAG_UNCHECK_GEOMETRY	= (1 << 9),		// uncheck dialog option for update geometry
	UPDATE_SCENE_FLAG_UNCHECK_MATERIALS	= (1 << 10),	// uncheck dialog option for update materials
	UPDATE_SCENE_FLAG_UNCHECK_LAYERS		= (1 << 11),	// uncheck dialog option for update layers
	UPDATE_SCENE_FLAG_UNCHECK_LIGHTS		= (1 << 12),	// uncheck dialog option for update light parameter
	UPDATE_SCENE_FLAG_UNCHECK_CAMERAS		= (1 << 13),	// uncheck dialog option for update camera parameter
	UPDATE_SCENE_FLAG_UNCHECK_SKY				= (1 << 14),	// uncheck dialog option for update sky parameter
};

/// @addtogroup EXECUTIONRESULT
/// @ingroup group_enumeration
/// @{
/// Predefined result codes for objects, tags and scene hooks execution.
/// @see ObjectData::Execute TagData::Execute SceneHookData::Execute
enum EXECUTIONRESULT
{
	EXECUTIONRESULT_OK					= 0,	///< OK.
	EXECUTIONRESULT_USERBREAK		= 1,	///< %User break.
	EXECUTIONRESULT_OUTOFMEMORY	= 2		///< Not enough memory.
} ENUM_END_LIST(EXECUTIONRESULT);
/// @}

/// @addtogroup ROTATIONORDER
/// @ingroup group_enumeration
/// @{
enum ROTATIONORDER
{
	ROTATIONORDER_YXZGLOBAL	= 0,	///< Global YXZ order.
	ROTATIONORDER_YZXGLOBAL	= 1,	///< Global YZX order.
	ROTATIONORDER_ZYXGLOBAL	= 2,	///< Global ZYX order.
	ROTATIONORDER_ZXYGLOBAL	= 3,	///< Global ZXY order.
	ROTATIONORDER_XZYGLOBAL	= 4,	///< Global XZY order.
	ROTATIONORDER_XYZGLOBAL	= 5,	///< Global XYZ order.

	ROTATIONORDER_YXZLOCAL	= 3,	///< Local YXZ order.
	ROTATIONORDER_YZXLOCAL	= 4,	///< Local YZX order.
	ROTATIONORDER_ZYXLOCAL	= 5,	///< Local ZYX order.
	ROTATIONORDER_ZXYLOCAL	= 0,	///< Local ZXY order.
	ROTATIONORDER_XZYLOCAL	= 1,	///< Local XZY order.
	ROTATIONORDER_XYZLOCAL	= 2,	///< Local XYZ order.

	ROTATIONORDER_HPB				= 6,	///< HPB order.
	ROTATIONORDER_DEFAULT		= 6		///< Default order (HPB).
} ENUM_END_LIST(ROTATIONORDER);
/// @}

/// @addtogroup VERSIONTYPE
/// @ingroup group_enumeration
/// @{
enum VERSIONTYPE
{
	VERSIONTYPE_PRIME								 = 0,		///< @C4D Prime.
	VERSIONTYPE_BODYPAINT						 = 1,		///< @BP3D.
	VERSIONTYPE_STUDIO							 = 2,		///< @C4D Studio.
	VERSIONTYPE_VISUALIZE						 = 3,		///< @C4D Visualize.
	VERSIONTYPE_BROADCAST						 = 4,		///< @C4D Broadcast.
	VERSIONTYPE_BENCHMARK						 = 5,		///< Cinebench.
	VERSIONTYPE_UPDATER							 = 6,		///< Online updater.
	VERSIONTYPE_INSTALLER						 = 7,		///< Installer.
	VERSIONTYPE_NET_CLIENT					 = 8,		///< @C4D NET client.
	VERSIONTYPE_NET_SERVER_3				 = 9,		///< @C4D NET server.
	VERSIONTYPE_NET_SERVER_UNLIMITED = 10,	///< @C4D NET server.
	VERSIONTYPE_UNKNOWN							 = 11,	///< Unknown.
	VERSIONTYPE_LICENSESERVER				 = 12		///< @C4D License Server.
} ENUM_END_LIST(VERSIONTYPE);
/// @}

/// @addtogroup DTYPE
/// @ingroup group_enumeration
/// @{
/// Parameter description types.
enum DTYPE
{
	DTYPE_NONE					= 0,					///< None.

	DTYPE_CHILDREN			= 0,					///< Children.
	DTYPE_GROUP					= 1,					///< Group.
	DTYPE_COLOR					= 3,					///< Color.
	DTYPE_SUBCONTAINER  = 5,					///< Sub-container.
	DTYPE_MULTIPLEDATA  = 6,					///< Multiple data entry.
	DTYPE_TEXTURE				= 7,					///< Texture name.
	DTYPE_BUTTON				= 8,					///< Button.
	DTYPE_DYNAMIC				= 10,					///< GV dynamic.
	DTYPE_SEPARATOR			= 11,					///< Separator.
	DTYPE_STATICTEXT		= 12,					///< Static text.
	DTYPE_POPUP					= 13,					///< Popup.

	DTYPE_LONG					= 15,					///< ::Int32
	DTYPE_REAL					= 19,					///< ::Float
	DTYPE_TIME					= 22,					///< BaseTime
	DTYPE_VECTOR				= 23,					///< ::Vector
	DTYPE_MATRIX				= 25,					///< ::Matrix
	DTYPE_STRING				= 130,				///< String
	DTYPE_FILENAME			= 131,				///< Filename
	DTYPE_BASELISTLINK	= 133,				///< BaseLink
	DTYPE_BOOL					= 400006001,	///< GV Bool. ::ID_GV_DATA_TYPE_BOOL
	DTYPE_NORMAL				= 400006005,	///< GV Normal. ::ID_GV_DATA_TYPE_NORMAL
	DTYPE_COLORA				= 400007013,	///< Color with Alpha.
	DTYPE_VECTOR4D			= 1040682,		///< Vector with 4 components.

	DTYPE_
};
/// @}

/// @addtogroup DESC
/// @ingroup group_containerid
/// @{
/// IDs into the information container for a description parameter.
enum
{
	DESC_NAME					 = 1,																		///< String Name for standalone use.
	DESC_SHORT_NAME		 = 2,																		///< String Short name, for attributes dialog.

	DESC_VERSION			 = 3,																		///< ::Int32 Show in versions:
	DESC_VERSION_DEMO	 = (1 << 0),														///< Demo versions.
	DESC_VERSION_XL		 = (1 << 1),														///< XL Versions.
	DESC_VERSION_ALL	 = DESC_VERSION_DEMO | DESC_VERSION_XL,	///< All versions.
	DESC_CHILDREN			 = 4,																		///< BaseContainer @markInternal
	DESC_MIN					 = 5,																		///< ::Int32/::Float/::Vector Minimum value.
	DESC_MAX					 = 6,																		///< ::Int32/::Float/::Vector Maximum value.
	DESC_MINEX				 = 7,																		///< ::Bool @formatConstant{true} if @ref DESC_MIN is exclusive, otherwise it is inclusive.
	DESC_MAXEX				 = 8,																		///< ::Bool @formatConstant{true} if @ref DESC_MAX is exclusive, otherwise it is inclusive.
	DESC_STEP					 = 9,																		///< ::Int32/::Float/::Vector The step for the edit field arrows.
	DESC_ANIMATE			 = 10,																	///< ::Int32 Animation mode:
	DESC_ANIMATE_OFF	 = 0,																		///< Parameter is not animatable.
	DESC_ANIMATE_ON		 = 1,																		///< Parameter is animatable.
	DESC_ANIMATE_MIX	 = 2,																		///< Parameter is animatable, but needs to know the left and right data element.\n
																														///< An example is the Target expression that interpolates the Name parameter. If MIX is specified, the expression can at any time call BaseList2D::GetAnimatedParameter() to get the left, right and mix values.
	DESC_ASKOBJECT		 = 11,																	///< ::Bool @formatConstant{true} means ask object for this parameter, @formatConstant{false} means look in container.
	DESC_UNIT					 = 12,																	///< ::Int32 Unit for @link DTYPE_REAL@endlink/@link DTYPE_VECTOR@endlink:
	DESC_UNIT_FLOAT		 = 'frea',															///< ::FORMAT_FLOAT
	DESC_UNIT_INT			 = 'flng',															///< ::FORMAT_INT
	DESC_UNIT_PERCENT	 = 'fpct',															///< ::FORMAT_PERCENT
	DESC_UNIT_DEGREE	 = 'fdgr',															///< ::FORMAT_DEGREE
	DESC_UNIT_METER		 = 'fmet',															///< ::FORMAT_METER
	DESC_UNIT_TIME		 = 'ffrm',															///< ::FORMAT_FRAMES
	DESC_PARENTGROUP	 = 13,																	///< ::Int32/@link DescID@endlink Parent ID.
	DESC_CYCLE				 = 14,																	///< BaseContainer Contains members of cycle as string. (E.g. @c GetString(10041) == "-X".)
	DESC_HIDE					 = 15,																	///< ::Bool Indicates whether the property is hidden or not.
	DESC_DEFAULT			 = 16,																	///< ::Int32/::Float/::Vector Default numeric value.
	DESC_ACCEPT				 = 17,																	///< BaseContainer Contains the accepted IDs as strings. For C4DAtom::IsInstanceOf() checks. (E.g. @c GetString(5155) == "Obase".)
	DESC_SEPARATORLINE = 18,																	///< ::Bool @formatConstant{true} if separators should have a visible line.
	DESC_REFUSE				 = 19,																	///< BaseContainer Contains the refused IDs as strings. For C4DAtom::IsInstanceOf() checks. (E.g. @c GetString(5155) == "Obase".)
	DESC_PARENTID			 = 20,																	///< ::Int32 The ID of the parent element used for indentation and animation track.
	DESC_CUSTOMGUI		 = 21,																	///< ::Int32 The ID of the GUI for this element. Either a custom ID or one of: @enumerateEnum{CUSTOMGUI}

	/// @addtogroup CUSTOMGUI
	/// @ingroup group_enumeration
	/// @{
#define CUSTOMGUI_REAL					 DTYPE_REAL				///< cineware::Float edit field.
#define CUSTOMGUI_REALSLIDER		 1000489					///< cineware::Float edit field with slider.
#define CUSTOMGUI_REALSLIDERONLY 200000006				///< cineware::Float slider only.
#define CUSTOMGUI_VECTOR				 DTYPE_VECTOR			///< cineware::Vector.
#define CUSTOMGUI_STRING				 DTYPE_STRING			///< String field.
#define CUSTOMGUI_STRINGMULTI		 200000007				///< Multiple line string field.
#define CUSTOMGUI_STATICTEXT		 DTYPE_STATICTEXT	///< Static text field.
#define CUSTOMGUI_CYCLE					 200000180				///< Selection list field.
#define CUSTOMGUI_CYCLEBUTTON		 200000255				///< Selection field list with button functionality, e.g. the function of a button can be changed through a selection field list.
#define CUSTOMGUI_LONG					 DTYPE_LONG				///< cineware::Int32 edit field.
#define CUSTOMGUI_LONGSLIDER		 1000490					///< cineware::Int32 edit field with slider.
#define CUSTOMGUI_BOOL					 DTYPE_BOOL				///< Checkbox.
#define CUSTOMGUI_TIME					 DTYPE_TIME				///< Time edit field.
#define CUSTOMGUI_COLOR					 1000492					///< Color chooser.
#define CUSTOMGUI_MATRIX				 DTYPE_MATRIX			///< Matrix edit fields.
#define CUSTOMGUI_BUTTON				 DTYPE_BUTTON			///< Button.
#define CUSTOMGUI_POPUP					 DTYPE_POPUP			///< Popup field.
#define CUSTOMGUI_SEPARATOR			 DTYPE_SEPARATOR	///< Separator.
#define CUSTOMGUI_SUBDESCRIPTION 0								///< Sub-description.
#define CUSTOMGUI_PROGRESSBAR		 200000265				///< Progress bar.
	/// @}

	DESC_COLUMNS = 22,								///< cineware::Int32 Number of columns for layout groups (DTYPE_GROUP).
	DESC_LAYOUTGROUP = 23,						///< cineware::Bool @formatConstant{true} for layout groups (LAYOUTGROUP).
	DESC_REMOVEABLE	 = 24,						///< cineware::Bool @formatConstant{true} if entry is removable.
	DESC_GUIOPEN	 = 25,							///< cineware::Bool @formatConstant{true} if the maximized GUI is shown by default.
	DESC_EDITABLE	 = 26,							///< cineware::Bool @formatConstant{true} if entry is editable.
	DESC_MINSLIDER = 27,							///< cineware::Int32/::Float/::Vector Minimum value for slider.
	DESC_MAXSLIDER = 28,							///< cineware::Int32/::Float/::Vector Maximum value for slider.
	DESC_GROUPSCALEV = 29,						///< cineware::Bool Allow the group height to be scaled.
	DESC_SCALEH	= 30,									///< cineware::Bool Scale element horizontally.
	DESC_LAYOUTVERSION = 31,					///< cineware::Int32 Layout version.
	DESC_ALIGNLEFT = 32,							///< cineware::Bool Align element left.
	DESC_FITH	= 33,										///< cineware::Bool Fit element.
	DESC_NEWLINE	= 34,								///< cineware::Bool Line break.
	DESC_TITLEBAR	= 35,								///< cineware::Bool Main group title bar.
	DESC_CYCLEICONS	= 36,							///< BaseContainer Icon IDs for cycle.
	DESC_CYCLESYMBOLS	= 37,						///< BaseContainer String identifiers for help symbol export.
	DESC_PARENT_COLLAPSE = 38,				///< cineware::Int32 Parent collapse ID.
	DESC_FORBID_INLINE_FOLDING = 39,	///< cineware::Bool Instructs the Attribute Manager to not allow expanding inline objects for entry.
	DESC_FORBID_SCALING	= 40,					///< cineware::Bool Prevents auto-scaling of the parameter with the scale tool (for ::DESC_UNIT_METER).
	DESC_ANGULAR_XYZ = 41,						///< cineware::Bool @formatConstant{true} for XYZ as angular representation, or @formatConstant{false} for HPB.

	DESC_INPORT	 = 50,								///< cineware::Bool GV in port.
	DESC_OUTPORT = 51,								///< cineware::Bool GV out port.
	DESC_STATICPORT	= 52,							///< cineware::Bool GV static port.
	DESC_NEEDCONNECTION	= 53,					///< cineware::Bool GV needs connection.
	DESC_MULTIPLE				= 54,					///< cineware::Bool GV multiple ports allowed.
	DESC_PORTONLY				= 55,					///< cineware::Bool GV port only.
	DESC_CREATEPORT			= 56,					///< cineware::Bool GV create port.
	DESC_PORTSMIN				= 57,					///< cineware::Int32 GV minimum number of ports.
	DESC_PORTSMAX				= 58,					///< cineware::Int32 GV maximum number of ports.
	DESC_NOTMOVABLE			= 59,					///< cineware::Bool GV not movable.
	DESC_EDITPORT				= 60,					///< cineware::Bool GV editable.
	DESC_ITERATOR				= 61,					///< cineware::Bool GV iterator port.

	DESC_PARENTMSG = 62,							///< cineware::Int32 Used in the %Material Editor on the boolean tabs to specify which section to open.
	DESC_MATEDNOTEXT	= 63,						///< cineware::Bool No text in %Material Editor window.
	DESC_COLUMNSMATED	= 64,						///< cineware::Int32 Number of columns in left %Material Editor window.
	DESC_SHADERLINKFLAG	= 65,					///< cineware::Bool Specifies that this element is a shader link. (Only if datatype==::DTYPE_BASELISTLINK.)
	DESC_NOGUISWITCH		= 66,					///< cineware::Bool Disables GUI switching for this description element.

	DESC_RESOURCEPATH	= 400006007,		///< @markPrivate @since @C4D R18
	DESC_RESOURCELINE	= 400006008,		///< @markPrivate @since @C4D R18

	DESC_TEMPDESCID	= 998,						///< @markInternal Used internally to store the preferred description ID.
	DESC_IDENT = 999,									///< String Resource identifier, e.g. @c ID_BASEOBJECT_REL_POSITION.

	DESC_
};
/// @}

/// @addtogroup DESCIDSTATE
/// @ingroup group_enumeration
/// @{
/// @since 17.008
enum DESCIDSTATE
{
	DESCIDSTATE_0								 = 0,				///< None.
	DESCIDSTATE_LOCKED					 = 1 << 0,	///< Description element is locked.
		DESCIDSTATE_HIDDEN				 = 1 << 1,	///< Description element is hidden.
		DESCIDSTATE_OVERRIDE			 = 1 << 2,	///< Description is overridden. @since @C4D R17.032
		DESCIDSTATE_FORBIDOVERRIDE = 1 << 3		///< Description cannot be overridden. @since @C4D R17.032
} ENUM_END_FLAGS(DESCIDSTATE);
/// @}

/// @addtogroup NOTIFY_EVENT
/// @ingroup group_enumeration
/// @{
/// @markPrivate
enum NOTIFY_EVENT
{
	NOTIFY_EVENT_NONE					= -1,
	NOTIFY_EVENT_ALL					= 10,
	NOTIFY_EVENT_ANY					= 11,

	NOTIFY_EVENT_PRE_DEFORM		= 100,
	NOTIFY_EVENT_POST_DEFORM	= 101,
	NOTIFY_EVENT_UNDO					= 102,
	NOTIFY_EVENT_MESSAGE			= 103,
	NOTIFY_EVENT_FREE					= 104,

	NOTIFY_EVENT_COPY					= 107,
	NOTIFY_EVENT_CACHE				= 108,
	NOTIFY_EVENT_REMOVE				= 109,
	NOTIFY_EVENT_CLONE				= 110,
	NOTIFY_EVENT_INSERT				= 111,
	NOTIFY_EVENT_SELECTIONBIT	= 112,
	NOTIFY_EVENT_HIGHLIGHTBIT	= 113,

	NOTIFY_EVENT_SETNAME			= 200,

	NOTIFY_EVENT_0						= 0
} ENUM_END_LIST(NOTIFY_EVENT);
/// @}

/// @addtogroup NOTIFY_EVENT_FLAG
/// @ingroup group_enumeration
/// @{
/// @markPrivate
enum NOTIFY_EVENT_FLAG
{
	NOTIFY_EVENT_FLAG_REMOVED				 = (1 << 0),

	NOTIFY_EVENT_FLAG_COPY_UNDO			 = (1 << 10),
	NOTIFY_EVENT_FLAG_COPY_CACHE		 = (1 << 11),
	NOTIFY_EVENT_FLAG_COPY_DUPLICATE = (1 << 12),
	NOTIFY_EVENT_FLAG_ONCE					 = (1 << 13),
	NOTIFY_EVENT_FLAG_COPY					 = ((1 << 10) | (1 << 11) | (1 << 12)),

	NOTIFY_EVENT_FLAG_0							 = 0
} ENUM_END_FLAGS(NOTIFY_EVENT_FLAG);
/// @}

/// @addtogroup Tlistelements
/// @ingroup group_enumeration
/// @{
#define Tbaselist2d		110050	///< 2D list.
#define Tbasedocument	110059	///< Document.
#define Tpluginlayer	110064	///< Plugin layer.
#define Tundoablelist	110068	///< Undoable list node (an intermediate class that is not in the SDK).
#define Tgelistnode		110069	///< Node list.
/// @}

/// @addtogroup ObjectTypes Object Types
/// @ingroup group_enumeration
/// @{
#define Opolygon					5100		///< Polygon - PolygonObject.
#define Ospline						5101		///< Spline - SplineObject.
#define Olight						5102		///< Light.
#define Ocamera						5103		///< Camera - CameraObject.
#define Ofloor						5104		///< Floor.
#define Osky							5105		///< Sky.
#define Oenvironment			5106		///< Environment.
#define Oloft							5107		///< Loft NURBS.
#define Offd							5108		///< FFD.
#define Oparticle					5109		///< %Particle emitter - ParticleObject.
#define Odeflector				5110		///< %Particle deflector.
#define Ogravitation			5111		///< %Particle gravitation.
#define Orotation					5112		///< %Particle rotation.
#define Owind							5113		///< %Particle wind.
#define Ofriction					5114		///< %Particle friction.
#define Oturbulence				5115		///< %Particle turbulence.
#define Oextrude					5116		///< Extrude NURBS.
#define Olathe						5117		///< Lathe NURBS.
#define Osweep						5118		///< Sweep NURBS.
#define Oattractor				5119		///< %Particle attractor.
#define Obezier						5120		///< Bezier NURBS.
#define Oforeground				5121		///< Foreground.
#define Obackground				5122		///< Background.
#define Obone							5123		///< Bone. @markDeprecated
#define Odestructor				5124		///< %Particle destructor.
#define Ometaball					5125		///< Metaball.
#define Oinstance					5126		///< Instance.
#define Obend							5128		///< Bend deformer.
#define Obulge						5129		///< Bulge deformer.
#define Oshear						5131		///< Shear deformer.
#define Otaper						5133		///< Taper deformer.
#define Otwist						5134		///< Twist deformer.
#define Owave							5135		///< Wave deformer.
#define Ostage						5136		///< Stage.
#define Oline							5137		///< Line.
#define Omicrophone				5138		///< Microphone.
#define Oloudspeaker			5139		///< Loudspeaker.
#define Onull							5140		///< Null.
#define Osymmetry					5142		///< Symmetry.
#define Owrap							5143		///< Wrap deformer.
#define Oboole						1010865	///< Boolean.
#define Oexplosion				5145		///< Explosion deformer.
#define Oformula					5146		///< Formula deformer.
#define Omelt							5147		///< Melt deformer.
#define Oshatter					5148		///< Shatter deformer.
#define Owinddeform				5149		///< Wind deformer.
#define Oarray						5150		///< Array.
#define Oheadphone				5151		///< Headphone.
#define Oworkplane				5153		///< Workplane.
#define Oplugin						5154		///< Object plugin - pass the plugin ID.
#define Obase							5155		///< Base object - BaseObject. For @c instanceof parameters in the API.
#define Opoint						5156		///< Point - PointObject. For @c instanceof parameters in the API.
#define Obasedeform				5157		///< Base deformer. For @c instanceof parameters in the API.
#define Oparticlemodifier	5158		///< %Particle modifier. For @c instanceof parameters in the API.
#define Opolyreduction		1001253	///< %Polygon reduction.
#define Oshowdisplacement	1001196	///< Show displacement.
#define Ojoint						1019362	///< Joint.
#define Oskin							1019363	///< Skin deformer.
#define Oweighteffector		1019677	///< Weight effector.
#define Ocharacter				1021433	///< Character.
#define Ocmotion					1021824	///< CMotion.
#define Oxref							1025766	///< XRef. (not supported yet)
#define Omotiontracker		1028393	///< MotionTracker. @since @C4D R18
#define Ovolume						1039858	///< Volume Object. @since R20
#define Ovolumebuilder		1039859 ///< Volume Builder. @since R20
#define Ovolumemesher			1039861	///< Volume Mesher. @since R20
#define Ovolumeset				1039867	///< Volume Group. @since R20
#define Ofieldforce				1041451	///< %Particle field force

/// @addtogroup PrimitiveTypes Primitive Types
/// @ingroup group_enumeration
/// @{
#define Ocube				5159				///< Cube.
#define Osphere			5160				///< Sphere.
#define Oplatonic		5161				///< Platonic.
#define Ocone				5162				///< Cone.
#define Otorus			5163				///< Torus.
#define Odisc				5164				///< Disc.
#define Otube				5165				///< Tube.
#define Ofigure			5166				///< Figure.
#define Opyramid		5167				///< Pyramid.
#define Oplane			5168				///< Plane.
#define Ofractal		5169				///< Fractal.
#define Ocylinder		5170				///< Cylinder.
#define Ocapsule		5171				///< Capsule.
#define Ooiltank		5172				///< Oil-tank.
#define Orelief			5173				///< Relief.
#define Osinglepoly	5174				///< Single polygon.
/// @}
#define Opluginpolygon 1001091	///< Polygon plugin.
/// @addtogroup SplinePrimitiveTypes Spline Primitive Types
/// @ingroup group_enumeration
/// @{
#define Osplineprimitive 5152				///< Spline primitive.
#define Osplineprofile	 5175				///< Profile spline.
#define Osplineflower		 5176				///< Flower spline.
#define Osplineformula	 5177				///< Formula spline.
#define Osplinetext			 5178				///< Text spline.
#define Osplinenside		 5179				///< N-sided spline.
#define Ospline4side		 5180				///< 4-sided spline.
#define Osplinecircle		 5181				///< Circle spline.
#define Osplinearc			 5182				///< Arc spline.
#define Osplinecissoid	 5183				///< Cissoid spline.
#define Osplinecycloid	 5184				///< Cycloid spline.
#define Osplinehelix		 5185				///< Helix spline.
#define Osplinerectangle 5186				///< Rectangle spline.
#define Osplinestar			 5187				///< Star spline.
#define Osplinecogwheel	 5188				///< Cogwheel spline.
#define Osplinecontour	 5189				///< Contour spline.
/// @}
#define Oselection				5190			///< Selection object.
#define Osds							1007455		///< SDS (HyperNURBS) - SDSObject.
#define Osplinedeformer		1008982		///< Spline deformer.
#define Osplinerail				1008796		///< Spline rail.
#define Oatomarray				1001002		///< Atom array.
#define Ospherify					1001003		///< Spherify.
#define Oexplosionfx			1002603		///< Explosion FX.
#define Oconnector				1011010		///< Connector.
#define Oalembicgenerator 1028083		///< Alembic generator.
#define Ofalloff					440000229	///< Falloff object. @since R18
#define Olod							431000174	///< LOD object. @since R19
#define Ofield						440000200	///< Field object. @since R20
#define Ovoronoipointgenerator		1036448	///< Voronoi Fracture internal Point generator. @since R20
#define ID_MOTIONFRACTUREVORONOI	1036557	///< MoGraph fracture voronoi object. @since R20

#define SKY_II_SKY_OBJECT	1011146		///< Physical Sky object.
#define Ocloud						1011196		///< Cloud node (sky).
#define	Ocloudgroup				1011194		///< Cloud group (sky).
#define CNURBS_OBJECT_ID	100004007	///< Clothnurbs Object
#define Osplinewrap				1019221		///< mograph wrap modifier
#define Odynamicspline		1029122		///< @markPrivate
#define CA_MESH_DEFORMER_OBJECT_ID	1024543	///< mograph mesh deformer
/// @}

/// @addtogroup CINTERPOLATION
/// @ingroup group_enumeration
/// @{
enum CINTERPOLATION
{
	CINTERPOLATION_SPLINE	= 1,	///< Spline.
	CINTERPOLATION_LINEAR	= 2,	///< Linear.
	CINTERPOLATION_STEP		= 3,	///< Step.

	CINTERPOLATION_DUMMY	= 4
} ENUM_END_LIST(CINTERPOLATION);
/// @}

/// @addtogroup CAUTOMODE
/// @ingroup group_enumeration
/// @{
/// @since 17.008
enum CAUTOMODE
{
	CAUTOMODE_CLASSIC		 = 1,	///< AutoTangent first implementation.
	CAUTOMODE_FIXEDSLOPE = 2,	///< Autotangent with fixed slope for given time.

} ENUM_END_LIST(CAUTOMODE);
/// @}

/// @addtogroup ROTATIONINTERPOLATION
/// @ingroup group_enumeration
/// @{
/// @since 18.002
enum ROTATIONINTERPOLATION
{
	ROTATIONINTERPOLATION_QUATERNION_SLERP = 2,	///< Quaternion Spherical LERP Interpolation (Linear)
	ROTATIONINTERPOLATION_QUATERNION_CUBIC = 3,	///< Quaternion Smooth Cubic Interpolation (former known as Losch)
} ENUM_END_LIST(ROTATIONINTERPOLATION);
/// @}

/// @addtogroup CKEYPRESET
/// @ingroup group_enumeration
/// @{
/// @since 17.008
enum CKEYPRESET
{
	CKEYPRESET_CLASSICAUTO		 = 1,	///< Auto, Classical, Clamp
	CKEYPRESET_NEWAUTO				 = 2,	///< Auto, Classical, remove Overshooting
	CKEYPRESET_NEWAUTOWEIGHTED = 3,	///< Auto, Fixed Slope, remove Overshooting
	CKEYPRESET_CUSTOM					 = 4,	///< User Defined

} ENUM_END_LIST(CKEYPRESET);
/// @}

/// @addtogroup TagTypes Tag Types
/// @ingroup group_enumeration
/// @{
#define Tpoint						5600					///< Point - PointTag. @ingroup VariableTagTypes
#define Tanchor						5608					///< Anchor. @markDeprecated
#define Tphong						5612					///< Phong.
#define Tdisplay					5613					///< Display.
#define Tkinematic				5614					///< IK. @markDeprecated
#define Ttexture					5616					///< Texture - TextureTag.
#define Ttangent					5617					///< Tangent data- TangentTag. @ingroup VariableTagTypes
#define Tprotection				5629					///< Protection.
#define Tparticle					5630					///< %Particle data - ParticleTag. @ingroup VariableTagTypes
#define Tmotionblur				5636					///< Motion blur.
#define Tcompositing			5637					///< Compositing/render.
#define Tannotation				1030659				///< Annotation.
#define Tsavetemp					5650					///< @markInternal
#define Tpolygon					5604					///< Polygon data - PolygonTag. @ingroup VariableTagTypes
#define Tuvw							5671					///< UVW data - UVWTag. @ingroup VariableTagTypes
#define Tsegment					5672					///< Segment data - SegmentTag. @ingroup VariableTagTypes
#define Tpolygonselection	5673					///< Polygon selection - SelectionTag. @ingroup SelectionTagTypes
#define Tpointselection		5674					///< Point selection - SelectionTag. @ingroup SelectionTagTypes
#define Tcoffeeexpression	5675					///< @COFFEE expression.
#define Ttargetexpression	5676					///< Target expression.
#define Tfixexpression		5677					///< Fix expression. @markDeprecated
#define Tsunexpression		5678					///< Sun expression.
#define Tikexpression			5679					///< IK expression. @markDeprecated
#define Tline							5680					///< Line data. @ingroup VariableTagTypes
#define Tvertexmap				5682					///< Vertex map data. @ingroup VariableTagTypes
#define Trestriction			5683					///< Restriction.
#define Tmetaball					5684					///< Metaball.
#define Tbakeparticle			5685					///< Bake particle.
#define Tmorph						5689					///< Morph. @markDeprecated
#define Tsticktexture			5690					///< Stick texture - StickTextureTag. @ingroup VariableTagTypes
#define Tplugin						5691					///< Plugin tag - pass the plugin ID.
#define Tstop							5693					///< Stop.
#define Tbase							5694					///< Base tag - BaseTag. For @c instanceof parameters in the API.
#define Tvariable					5695					///< Variable tag - VariableTag. For @c instanceof parameters in the API.
#define Tvibrate					5698					///< Vibration.
#define Taligntospline		5699					///< Align to spline.
#define Taligntopath			5700					///< Align to path.
#define Tedgeselection		5701					///< Edge selection - SelectionTag. The edges are encodes like this: <tt>(4*poly)+edge</tt>, where @c edge goes from @em 0-@em 3. @ingroup SelectionTagTypes
#define Tclaudebonet_EX		5708					///< Claude bonet. @markDeprecated
#define Tnormal						5711					///< Normal - NormalTag. Contains 12 cineware::Int16 per polygon, enumerated like the following: <tt>ax,ay,az,bx,by,bz,cx,cy,cz,dx,dy,dz</tt>.\n
																				///< The value is the Float value of the normal vector component multiplied by @em 32000.0.
																				///< @ingroup VariableTagTypes
#define Tcorner							 5712				///< Corner. (Phong break.)
#define Tsds								 1007579		///< SDS.
#define Tlookatcamera				 1001001		///< Look at camera.
#define Texpresso						 1001149		///< XPresso.
#define Tsoftselection			 1016641		///< Soft selection.
#define Tbaketexture				 1011198		///< Bake texture.
#define Tsdsdata						 1018016		///< SDS data.
#define Tweights						 1019365		///< Weights.
#define Tposemorph					 1024237		///< Pose morph.
#define Tpython							 1022749		///< Python expression.
#define Tsculpt							 1023800		///< Sculpt.
#define Tmotioncam					 1027742		///< Motion camera.
#define Tmorphcam						 1027743		///< Morph camera.
#define Tcrane							 1028270		///< Camera crane.
#define Tarchigrass					 1028463		///< Architectural grass.
#define Tsculptnormals			 1027660		///< Sculpt normals. @b Private for Sculpting.
#define Tsplinenormal				 440000173	///< Spline normals.
#define Tinteraction				 440000164	///< Interaction.
#define Tgrouppriority			 200000299	///< Group Priority. @since R18
#define Tvertexcolor				 431000045	///< Vertex color. @since R18
#define Thairlight					 1017874		///< Hair light tag type ID.
#define Tsketch							 1011012		///< Sketch & Toon tag
#define Twww								 5647				///< WWW tag  @markDeprecated
#define ID_CTP_OBJECT_TAG		 1019597		///< collision detection tag @markInternal
#define ID_CTP_TAG					 1018862		///< collision cache tag @markInternal
#define AC_TAG_ID						 465000404	///< CAD AutoCAD @markDeprecated
#define VW_TAG_ID						 1009891		///< CAD VectorWorks @markDeprecated
#define Timp								 5642				///< @markDeprecated
#define Tprotectstate				 5692				///< @markDeprecated
#define Tbonecache					 5696				///< @markDeprecated
#define	Texternalcompositing 465000402	///< @markInternal
#define Tdxf								 5688				///< @markInternal
#define Talembicmorphtag		 1037184		///< Alembic Morph. @since R20
#define Tmeshattribute 			 431000188	///< CustomDataTag. @since R20
#define Tvolumebuilder			 1039860		///< Volume Builder Tag. @since R20
/// @}

/// @addtogroup MaterialTypes Material Types
/// @ingroup group_enumeration
/// @{
#define Mbase					 5702				///< Base material.
#define Mmaterial			 5703				///< Standard material.
#define Mplugin				 5705				///< Plugin material - pass the plugin ID.
#define Mfog					 8803				///< Fog.
#define Mterrain			 8808				///< Terrain.
#define Mdanel				 1011117		///< Danel.
#define Mbanji				 1011118		///< Banji.
#define Mbanzi				 1011119		///< Banzi.
#define Mcheen				 1011120		///< Cheen.
#define Mmabel				 1011121		///< Mabel.
#define Mnukei				 1011122		///< Nukei.
#define MCgFX					 450000237	///< @markPrivate
#define Msketch				 1011014		///< Hair material.
#define Mhair					 1017730		///< Sketch and Toon material.
#define Mskysurface		 1011149		///< Sky material (hidden in manager). @since 18.002
#define Marchigrass		 1028461		///< Architectural Grass material.
#define Mshadowcatcher 1036101		///< Shadow Catcher. @since 18.002 and @C4D R18
/// @}

/// @addtogroup ShaderTypes Shader Types
/// @ingroup group_enumeration
/// @{
#define Xbase							5707			///< Shader base (internal only).
#define Xcolor						5832			///< Color.
#define Xbitmap						5833			///< Bitmap.
#define Xbrick						5804			///< Brick.
#define Xcheckerboard			5800			///< Checkerboard.
#define Xcloud						5802			///< Cloud.
#define Xcolorstripes			5822			///< Color stripes (deprecated, don't use).
#define Xcyclone					5821			///< Cyclone.
#define Xearth						5825			///< Earth.
#define Xfire							5803			///< Fire.
#define Xflame						5817			///< Flame.
#define Xgalaxy						5813			///< Galaxy.
#define Xmetal						5827			///< Metal.
#define Xsimplenoise			5807			///< Simple noise.
#define Xrust							5828			///< Rust.
#define Xstar							5816			///< Star.
#define Xstarfield				5808			///< Star field.
#define Xsunburst					5820			///< Sun burst.
#define Xsimpleturbulence	5806			///< Simple turbulence.
#define Xvenus						5826			///< Venus.
#define Xwater						5818			///< Water.
#define Xwood							5823			///< Wood.
#define Xplanet						5829			///< Planet.
#define Xmarble						5830			///< Marble.
#define Xspectral					5831			///< Spectral.
#define Xgradient					1011100		///< Gradient.
#define Xfalloff					1011101		///< Falloff.
#define Xtiles						1011102		///< Tiles.
#define Xfresnel					1011103		///< Fresnel.
#define Xlumas						1011105		///< Lumas.
#define Xproximal					1011106		///< Proximal.
#define Xnormaldirection	1011107		///< Normal direction.
#define Xtranslucency			1011108		///< Translucency.
#define Xfusion						1011109		///< Fusion.
#define Xposterizer				1011111		///< Posterizer.
#define Xcolorizer				1011112		///< Colorizer.
#define Xdistorter				1011114		///< Distorter.
#define Xprojector				1011115		///< Projector.
#define Xnoise						1011116		///< Noise.
#define Xlayer						1011123		///< Layer.
#define Xspline						1011124		///< Spline.
#define Xfilter						1011128		///< Filter.
#define Xripple						1011199		///< Ripple.
#define Xdirt							1011136		///< Dirt (deprecated, don't use).
#define Xvertexmap				1011137		///< Vertex map.
#define Xsss							1001197		///< SSS (deprecated, use Xxmbsubsurface instead).
#define Xambientocclusion	1001191		///< Ambient occlusion.
#define Xchanlum					1007539		///< Chanlum.
#define Xmosaic						1022119		///< Pixelate.
#define Xpavement					1024945		///< Pixelate.
#define Xcelshader				1012158		///< S&T Cel.
#define Xspotshader				1012160		///< S&T Spot.
#define Xartshader				1012161		///< S&T Art.
#define Xhatchingshader		1012166		///< S&T Hatch.
#define Xxmbsubsurface		1025614		///< Subsurface scattering.
#define Xrainsampler			1026576		///< Rain sampler.
#define Xnormalizer				1026588		///< Normalizer.
#define Xterrainmask			1026277		///< Terrain mask.
#define Xobjectcolor			1033961		///< Object color.
#define Xformula					1031433		///< Formula. @since 17.008 and @C4D R17.032
#define Xvariation				1033825		///< Variation. @since 17.008 and @C4D R17.032
#define Xthinfilm					1035731		///< Thinfilm. @since 17.008 and @C4D R18
#define Xmobeat						1018654		///< Mograph beat. @since 18.002
#define Xmocamera					440000050	///< Mograph camera. @since 18.002
#define Xmocolor					1018767		///< Mograph per clone color. @since 18.002
#define Xmomulti					1019397		///< Mograph multi. @since 18.002
#define Xsubstance				1032171		///< Substance. @since 18.002
/// @}

// Xshader special parameter ids
enum
{
	// Xnoise - additional IDs (dynamically internal C4D IDs, not stored in xslanoise.h)
	SLA_NOISE_NOISE_BOX_NOISE			= 2001,
	SLA_NOISE_NOISE_BLIST_TURB		= 2002,
	SLA_NOISE_NOISE_BUYA					= 2003,
	SLA_NOISE_NOISE_CELL_NOISE		= 2004,
	SLA_NOISE_NOISE_CRANAL				= 2005,
	SLA_NOISE_NOISE_DENTS					= 2006,
	SLA_NOISE_NOISE_DISPL_TURB		= 2007,
	SLA_NOISE_NOISE_FBM						= 2008,
	SLA_NOISE_NOISE_HAMA					= 2009,
	SLA_NOISE_NOISE_LUKA					= 2010,
	SLA_NOISE_NOISE_MOD_NOISE			= 2011,
	SLA_NOISE_NOISE_NAKI					= 2012,
	SLA_NOISE_NOISE_NOISE					= 2013,
	SLA_NOISE_NOISE_NUTOUS				= 2014,
	SLA_NOISE_NOISE_OBER					= 2015,
	SLA_NOISE_NOISE_PEZO					= 2016,
	SLA_NOISE_NOISE_POXO					= 2017,
	SLA_NOISE_NOISE_RANDOM				= 2018,
	SLA_NOISE_NOISE_SEMA					= 2019,
	SLA_NOISE_NOISE_STUPL					= 2020,
	SLA_NOISE_NOISE_TURBULENCE		= 2021,
	SLA_NOISE_NOISE_VL_NOISE			= 2022,
	SLA_NOISE_NOISE_WAVY_TURB			= 2023,
	SLA_NOISE_NOISE_SEPARATOR			= 3000,
	SLA_NOISE_NOISE_CELL_VORONOI	= 2024,
	SLA_NOISE_NOISE_DISPL_VORONOI = 2025,
	SLA_NOISE_NOISE_SPARSE_CONV		= 2026,
	SLA_NOISE_NOISE_VORONOI_1			= 2027,
	SLA_NOISE_NOISE_VORONOI_2			= 2028,
	SLA_NOISE_NOISE_VORONOI_3			= 2029,
	SLA_NOISE_NOISE_ZADA					= 2030,
	SLA_NOISE_NOISE_FIRE					= 2031,
	SLA_NOISE_NOISE_ELECTRIC			= 2032,
	SLA_NOISE_NOISE_GASEOUS				= 2033
};

/// @addtogroup VideoPostTypes
/// @ingroup group_enumeration
/// @{
/// Video post types.
#define VPlenseffects							1001049				///< Lens effects.
#define VPobjectglow							1001007				///< Object glow.
#define VPcolorcorrection					1001008				///< Color correction.
#define VPcomic										1001009				///< Comic.
#define VPscenemotionblur					1001010				///< Scene motion blur.
#define VPsoftfilter							1001012				///< Soft filter.
#define VPsharpenfilter						1001013				///< Sharpen filter.
#define VPmedianfilter						1001014				///< Median filter.
#define VPremote									1001015				///< Remote.
#define VPopticsuite_depthoffield 1001400				///< Optic suite dof.
#define VPopticsuite_glow					1001401				///< Optic suite glow.
#define VPopticsuite_highlights		1001402				///< Optic suite highlights.
#define VPambientocclusion				300001045			///< Ambient occlusion.
#define VPxmbsampler							1023342				///< Subsurface scattering sampler.
#define VPGPURenderer							1037639				///< GPU Renderer. @since R19
#define VPToneMapping							1037876				///< Tone-Mapper. @since R19
#define VPpositionpass						1027117 			///< Position Pass. @since R20
#define VPnormalpass							1028398 			///< Normal Pass. @since R20
#define VPPhLensDistortion				1031709				///< Lens Distortion. @since R20
#define VPwatermark								1025462 			///< Watermark. @since R20
#define VPDenoiser								1050980				///< Denoiser. @since R21

#define VPcolormapping			 1001194
#define VPcylindricallens		 1001186
#define VPvectormotionblur	 1002008
#define VPhair							 1017325
#define VPobjectmotionblur	 1001011
#define VPsketch						 1011015
#define VPglobalillumination 1021096
#define VPcaustics					 1000970
/// @}

/// @addtogroup SPLINETYPE
/// @ingroup group_enumeration
/// @{
/// Spline types.
/// @see SplineObject::Alloc SplineObject::GetInterpolationType
enum SPLINETYPE
{
	SPLINETYPE_LINEAR		= 0,	///< Linear.
	SPLINETYPE_CUBIC		= 1,	///< Cubic.
	SPLINETYPE_AKIMA		= 2,	///< Akima.
	SPLINETYPE_BSPLINE	= 3,	///< B-Spline.
	SPLINETYPE_BEZIER		= 4,	///< Bezier.
	RESERVEDSPLINETYPE1	= 5,	///< @markInternal @since @C4D R17.032
	RESERVEDSPLINETYPE2	= 6		///< @markInternal @since @C4D R17.032
} ENUM_END_LIST(SPLINETYPE);
/// @}

/// @addtogroup TrackTypes
/// @ingroup group_enumeration
/// @{
#define CTpla		100004812	///< PLA.
#define CTsound	100004813	///< Sound.
#define CTmorph	100004822	///< Morph.
#define CTtime	-1				///< Time.
/// @}

// CTmorph
#define CK_MORPH_LINK	 10001	// link
#define CK_MORPH_BIAS	 10002	// real
#define CK_MORPH_CUBIC 10003	// bool

#pragma pack (pop)
}

#endif	// C4D_PARAMETER_IDS_H__
