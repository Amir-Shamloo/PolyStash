/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_SYMBOLS_H__
#define PRIVATE_SYMBOLS_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

#define HandleDescGetVector(tid, vector, t_data) \
	switch (tid[1].id) \
	{ \
		case 0:			t_data = GeData(vector); break; \
		case 1000:	t_data = GeData(vector.x); break; \
		case 1001:	t_data = GeData(vector.y); break; \
		case 1002:	t_data = GeData(vector.z); break; \
	} \

#define HandleDescSetVector(v, tid, vector, t_data) \
	switch (tid[1].id) \
	{ \
		case 0:			v = t_data.GetVector(); break; \
		case 1000:	v = Vector(t_data.GetFloat(), vector.y, vector.z); break; \
		case 1001:	v = Vector(vector.x, t_data.GetFloat(), vector.z); break; \
		case 1002:	v = Vector(vector.x, vector.y, t_data.GetFloat()); break; \
	} \

#ifndef NOTINDEX
	#define NOTINDEX (LIMIT<Int32>::MAX)	// an "invalid" index to mark "NOTOK"
#endif

#define DESCID_DYNAMICSUB	DescLevel(ID_USERDATA, DTYPE_SUBCONTAINER, 0)

#define ID_XCHANGE_OBJECTCHECKSUM	1020800

#define COPY_IDENTMARKER		(1 << 0)	// private flags
#define COPY_NO_INTERNALS		(1 << 8)
#define COPY_NO_PLUGINLAYER	(1 << 9)
#define COPY_CACHE_BUILD		(1 << 13)	// marks the copy as coming from the cache build
#define COPY_RECURSIONCHECK	(1 << 14)

#define BIT_OVERDISABLE		(1 << 10)	///< override is disabled in the take tree (not used at the moment)
#define BIT_OVERRIDECHILD (1 << 9)	///< override is child of another override (the bit exist for the complete override lifetime), used by the manager to know place the node in the correct position in the tree

// flags for GetDParameter/SetDParameter
enum DESCFLAGS_GET
{
	DESCFLAGS_GET_0											= 0,
	DESCFLAGS_GET_PARAM_GET							= (1 << 1),
	DESCFLAGS_GET_NO_GLOBALDATA					= (1 << 4),
	DESCFLAGS_GET_NO_GEDATADEFAULTVALUE	= (1 << 5)
} ENUM_END_FLAGS(DESCFLAGS_GET);

enum DESCFLAGS_SET
{
	DESCFLAGS_SET_0											= 0,
	DESCFLAGS_SET_PARAM_SET							= (1 << 1),
	DESCFLAGS_SET_USERINTERACTION				= (1 << 2),
	DESCFLAGS_SET_DONTCHECKMINMAX				= (1 << 3),
	DESCFLAGS_SET_DONTAFFECTINHERITANCE	= (1 << 6),	// for render settings and post effects only (SetParameter)
	DESCFLAGS_SET_FORCESET							= (1 << 7)	// SetParameter: force the set value without GetParameter/Compare, use only for calls where you definatly changed the value!
} ENUM_END_FLAGS(DESCFLAGS_SET);

/// @addtogroup COPYFLAGS
/// @ingroup group_enumeration
/// @{
/// Flags for the copy of @C4D objects.
enum COPYFLAGS
{
	COPYFLAGS_0																 = 0,					///< None.
	COPYFLAGS_NO_HIERARCHY										 = (1 << 2),	///< Copy without children.
	COPYFLAGS_NO_ANIMATION										 = (1 << 3),	///< Copy without tracks, sequences or keys.
	COPYFLAGS_NO_BITS													 = (1 << 4),	///< Do not copy BaseList2D bits.
	COPYFLAGS_NO_MATERIALPREVIEW							 = (1 << 5),	///< Do not create a new material preview.
	COPYFLAGS_NO_BRANCHES											 = (1 << 7),	///< Do not copy branches, for example tags on an object. Automatically implies @ref COPYFLAGS_NO_ANIMATION, as animation is a branch.
	COPYFLAGS_DOCUMENT												 = (1 << 10),	///< This is a read-only flag that is set when a complete document is copied.
	COPYFLAGS_NO_NGONS												 = (1 << 11),	///< Do not copy N-gons.
	COPYFLAGS_CACHE_BUILD											 = (1 << 13),	///< This is a read-only flag that is set when a cache is built.
	COPYFLAGS_RECURSIONCHECK									 = (1 << 14),	///< Checks and avoids instances to cause recursions.

	COPYFLAGS_PRIVATE_IDENTMARKER							 = (1 << 0),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_INTERNALS						 = (1 << 8),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_PLUGINLAYER					 = (1 << 9),	///< @markPrivate
	COPYFLAGS_PRIVATE_UNDO										 = (1 << 12),	///< @markPrivate
	COPYFLAGS_PRIVATE_CONTAINER_COPY_DIRTY		 = (1 << 15),	///< @markPrivate
	COPYFLAGS_PRIVATE_CONTAINER_COPY_IDENTICAL = (1 << 16),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_TAGS									 = (1 << 17),	///< @markPrivate
	COPYFLAGS_PRIVATE_DELETE									 = (1 << 18),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_ASSETS								 = (1 << 20),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_RESULTASSETS					 = (1 << 21),	///< @markPrivate
	COPYFLAGS_PRIVATE_NO_LOGS									 = (1 << 22),	///< @markPrivate
	COPYFLAGS_PRIVATE_BODYPAINT_NODATA				 = (1 << 29),	///< @markPrivate
	COPYFLAGS_PRIVATE_BODYPAINT_CONVERTLAYER	 = (1 << 30)	///< @markPrivate

} ENUM_END_FLAGS(COPYFLAGS);
/// @}

enum DIRTYFLAGS
{
	DIRTYFLAGS_0					 = 0,
	DIRTYFLAGS_MATRIX			 = (1 << 1),	// object matrix changed
	DIRTYFLAGS_DATA				 = (1 << 2),	// object internal data changed
	DIRTYFLAGS_SELECT			 = (1 << 3),	// object selections changed
	DIRTYFLAGS_CACHE			 = (1 << 4),	// object caches changed
	DIRTYFLAGS_CHILDREN		 = (1 << 5),
	DIRTYFLAGS_DESCRIPTION = (1 << 6),	// description changed

	// basedocument
	DIRTYFLAGS_SELECTION_OBJECTS	 = (1 << 20),
	DIRTYFLAGS_SELECTION_TAGS			 = (1 << 21),
	DIRTYFLAGS_SELECTION_MATERIALS = (1 << 22),

	DIRTYFLAGS_ALL								 = -1
} ENUM_END_FLAGS(DIRTYFLAGS);

/// @addtogroup FINDANIM
/// @ingroup group_enumeration
/// @{
enum FINDANIM
{
	FINDANIM_EXACT = 0,	///< Exact.
	FINDANIM_LEFT	 = 1,	///< Search left.
	FINDANIM_RIGHT = 2	///< Search right.
} ENUM_END_LIST(FINDANIM);
/// @}

/// @addtogroup NBIT
/// @ingroup group_enumeration
/// @{
/// GeListNode bits.
/// @see GeListNode::GetNBit GeListNode::ChangeNBit
enum NBIT
{
	NBIT_0										= 0,	///< None.

	NBIT_TL1_FOLD							= 1,	///< Folding bit for Timeline 1.
	NBIT_TL2_FOLD							= 2,	///< Folding bit for Timeline 2.
	NBIT_TL3_FOLD							= 3,	///< Folding bit for Timeline 3.
	NBIT_TL4_FOLD							= 4,	///< Folding bit for Timeline 4.

	NBIT_TL1_SELECT						= 5,	///< Selection bit for Timeline 1.
	NBIT_TL2_SELECT						= 6,	///< Selection bit for Timeline 2.
	NBIT_TL3_SELECT						= 7,	///< Selection bit for Timeline 3.
	NBIT_TL4_SELECT						= 8,	///< Selection bit for Timeline 4.

	NBIT_TL1_TDRAW						= 9,	///< @markPrivate
	NBIT_TL2_TDRAW						= 10,	///< @markPrivate
	NBIT_TL3_TDRAW						= 11,	///< @markPrivate
	NBIT_TL4_TDRAW						= 12,	///< @markPrivate

	NBIT_CKEY_ACTIVE					= 13,	///< Active point of animation path in editor.

	NBIT_OM1_FOLD							= 14,	///< Folding bit for Object Manager 1.
	NBIT_OM2_FOLD							= 15,	///< Folding bit for Object Manager 2.
	NBIT_OM3_FOLD							= 16,	///< Folding bit for Object Manager 3.
	NBIT_OM4_FOLD							= 17,	///< Folding bit for Object Manager 4.

	NBIT_TL1_FOLDTR						= 18,	///< Track folding for Timeline 1.
	NBIT_TL2_FOLDTR						= 19,	///< Track folding for Timeline 2.
	NBIT_TL3_FOLDTR						= 20,	///< Track folding for Timeline 3.
	NBIT_TL4_FOLDTR						= 21,	///< Track folding for Timeline 4.

	NBIT_TL1_FOLDFC						= 22,	///< F-Curve folding for Timeline 1.
	NBIT_TL2_FOLDFC						= 23,	///< F-Curve folding for Timeline 2.
	NBIT_TL3_FOLDFC						= 24,	///< F-Curve folding for Timeline 3.
	NBIT_TL4_FOLDFC						= 25,	///< F-Curve folding for Timeline 4.

	NBIT_SOURCEOPEN						= 26,

	NBIT_TL1_HIDE							= 27,	///< Hide in Timeline 1.
	NBIT_TL2_HIDE							= 28,	///< Hide in Timeline 2.
	NBIT_TL3_HIDE							= 29,	///< Hide in Timeline 3.
	NBIT_TL4_HIDE							= 30,	///< Hide in Timeline 4.

	NBIT_SOLO_ANIM						= 31,	///< Solo animation.
	NBIT_SOLO_LAYER						= 32,	///< Solo layer.

	NBIT_TL1_SELECT2					= 33,	///< @markPrivate
	NBIT_TL2_SELECT2					= 34,	///< @markPrivate
	NBIT_TL3_SELECT2					= 35,	///< @markPrivate
	NBIT_TL4_SELECT2					= 36,	///< @markPrivate

	NBIT_SOLO_MOTION					= 37,	///< Solo motion.

	NBIT_CKEY_LOCK_T					= 38,	///< Lock key time.
	NBIT_CKEY_LOCK_V					= 39,	///< Lock key value.
	NBIT_CKEY_MUTE						= 40,	///< Mute key.
	NBIT_CKEY_CLAMP						= 41,	///< Clamp key tangents.

	NBIT_CKEY_BREAK						= 42,	///< Break key tangents.
	NBIT_CKEY_KEEPVISUALANGLE	= 43,	///< Keep visual angle.

	NBIT_CKEY_LOCK_O					= 44,	///< Lock key tangents angles.
	NBIT_CKEY_LOCK_L					= 45,	///< Lock key tangents length.
	NBIT_CKEY_AUTO						= 46,	///< Key auto tangents.
	NBIT_CKEY_ZERO_O_OLD			= 48,	///< @markDeprecated
	NBIT_CKEY_ZERO_L_OLD			= 49,	///< @markDeprecated

	NBIT_TL1_FCSELECT					= 50,	///< F-Curve selection bit for Timeline 1.
	NBIT_TL2_FCSELECT					= 51,	///< F-Curve selection bit for Timeline 2.
	NBIT_TL3_FCSELECT					= 52,	///< F-Curve selection bit for Timeline 3.
	NBIT_TL4_FCSELECT					= 53,	///< F-Curve selection bit for Timeline 4.

	NBIT_CKEY_BREAKDOWN				= 54,	///< @markPrivate

	NBIT_TL1_FOLDMOTION				= 55,	///< Motion clip folding for Timeline 1.
	NBIT_TL2_FOLDMOTION				= 56,	///< Motion clip folding for Timeline 2.
	NBIT_TL3_FOLDMOTION				= 57,	///< Motion clip folding for Timeline 3.
	NBIT_TL4_FOLDMOTION				= 58,	///< Motion clip folding for Timeline 4.

	NBIT_TL1_SELECTMOTION			= 59,	///< Motion clip selection for Timeline 1.
	NBIT_TL2_SELECTMOTION			= 60,	///< Motion clip selection for Timeline 2.
	NBIT_TL3_SELECTMOTION			= 61,	///< Motion clip selection for Timeline 3.
	NBIT_TL4_SELECTMOTION			= 62,	///< Motion clip selection for Timeline 4.

	NBIT_OHIDE								= 63,	///< Hide object in Object Manager.
	NBIT_TL_TBAKE							= 64,	///< @markPrivate

	NBIT_TL1_FOLDSM						= 66,	///< @markPrivate
	NBIT_TL2_FOLDSM						= 67,	///< @markPrivate
	NBIT_TL3_FOLDSM						= 68,	///< @markPrivate
	NBIT_TL4_FOLDSM						= 69,	///< @markPrivate

	NBIT_SUBOBJECT						= 70,	///< @markPrivate
	NBIT_LINK_ACTIVE					= 71,	///< @markPrivate
	NBIT_THIDE								= 72,	///< Hide object in Timeline.
	NBIT_SUBOBJECT_AM					= 74,	///< @markPrivate
	NBIT_PROTECTION						= 75,	///< PSR protection.
	NBIT_NOANIM								= 76,	///< No animation.
	NBIT_NOSELECT							= 77,	///< No selection.
	NBIT_EHIDE								= 78,	///< Hide in viewport.
	NBIT_REF									= 79,	///< XRef.
	NBIT_REF_NO_DD						= 80,	///< XRef object no drag and drop. @markPrivate
	NBIT_REF_OHIDE						= 81,	///< XRef object hide. @markPrivate
	NBIT_NO_DD								= 82,	///< No drag and drop duplication.
	NBIT_HIDEEXCEPTVIEWSELECT	= 83,	///< Hide in viewport except to viewport select.
	NBIT_CKEY_WEIGHTEDTANGENT = 84,	///<
	NBIT_CKEY_REMOVEOVERSHOOT = 85,	///< Gradual Clamp of Tangent to avoid OverShooting
	NBIT_CKEY_AUTOWEIGHT			= 86,	///< Weight still adjusted even if Angle is User Defined
	NBIT_TAKE_LOCK						= 87,	///< A node in a override group can't be changed
	NBIT_TAKE_OBJINGROUP			= 88,	///< The object is overridden by an override group
	NBIT_TAKE_EGROUPOVERIDDEN	= 89,	///< The object editor visibility is overridden by an override group
	NBIT_TAKE_RGROUPOVERIDDEN	= 90,	///< The object render visibility is overridden by an override group
	NBIT_MAX									= 91,	///< Maximum @ref NBIT.
	NBIT_PRIVATE_MASK1				= -1,	///< @markPrivate
	NBIT_PRIVATE_MASK2				= -2,	///< @markPrivate
	NBIT_PRIVATE_MASK3				= -3,	///< @markPrivate
	NBIT_PRIVATE_MASK4				= -4	///< @markPrivate
} ENUM_END_LIST(NBIT);
/// @}

/// @addtogroup NBITCONTROL
/// @ingroup group_enumeration
/// @{
/// @see GeListNode::ChangeNBit
enum NBITCONTROL
{
	NBITCONTROL_SET							= 1,		///< Set bit.
	NBITCONTROL_CLEAR						= 2,		///< Clear bit.
	NBITCONTROL_TOGGLE					= 3,		///< Toggle bit.
	NBITCONTROL_PRIVATE_NODIRTY	= 0xf0	///< @markPrivate
} ENUM_END_FLAGS(NBITCONTROL);
/// @}

/// @addtogroup BIT
/// @ingroup group_enumeration
/// @{
/// BaseList2D bits.
/// @see	BaseList2D::SetBit BaseList2D::GetBit BaseList2D::DelBit BaseList2D::ToggleBit\n
///				BaseList2D::GetAllBits BaseList2D::SetAllBits
/// @name Active bits
/// @{
#define BIT_ACTIVE	(1 << 1)	///< Active.
#define BIT_ACTIVE2	(1 << 29)	///< @markPrivate
#define BIT_ACTIVE3	(1 << 30)	///< @markPrivate
/// @}
/// @name Material Bits
/// @{
#define BIT_MATMARK				(1 << 2)	///< Marked material.
#define BIT_ENABLEPAINT		(1 << 3)	///< Enable painting.
#define BIT_RECALCPREVIEW	(1 << 5)	///< Recalculate preview.
#define BIT_MFOLD					(1 << 6)	///< Folded in material manager.
#define BIT_BP_FOLDLAYERS	(1 << 9)	///< Fold layers in material manager.
/// @}
/// @name Object Bits
/// @{
#define BIT_IGNOREDRAW										 (1 << 2)		///< Ignore object during draw.
#define BIT_OFOLD													 (1 << 6)		///< Folded in object manager.
#define BIT_CONTROLOBJECT									 (1 << 9)		///< Internal bit set by generators.
#define BIT_RECMARK												 (1 << 11)	///< Help bit for recursive operations. @markPrivate
#define BIT_IGNOREDRAWBOX									 (1 << 12)	///< Ignore object drawing box.
#define BIT_EDITOBJECT										 (1 << 13)	///< Edit object from SDS. @markPrivate
#define BIT_ACTIVESELECTIONDRAW						 (1 << 15)	///< Draw active selection.
#define BIT_TEMPDRAW_VISIBLE_CACHECHILD		 (1 << 16)	///< @markPrivate. Temp bit for faster redraw.
#define BIT_TEMPDRAW_VISIBLE_DEFCACHECHILD (1 << 17)	///< @markPrivate. Temp bit for faster redraw.
#define BIT_TEMPDRAW_VISIBLE_CHILD				 (1 << 18)	///< @markPrivate. Temp bit for faster redraw.
#define BIT_HIGHLIGHT											 (1 << 20)	///< Object highlighted in viewport. @markPrivate
#define BIT_FORCE_UNOPTIMIZED							 (1 << 21)	///< Do not optimize the points of a polygon object during OpenGL redraw. @markPrivate
/// @}
/// @name Track Bits
/// @{
#define BIT_TRACKPROCESSED				(1 << 16)	///< Track has been processed, avoid recursions.
#define BIT_ANIM_OFF							(1 << 17)	///< Inactive sequence.
#define BIT_ANIM_SOLO							(1 << 18)	///< Solo track.
#define BIT_ANIM_CONSTANTVELOCITY	(1 << 19)	///< Constant velocity.
/// @}
/// @name Videopost Bits
/// @{
#define BIT_VPDISABLED (1 << 2)	///< Videopost is disabled.
/// @}
/// @name Document Bits
/// @{
#define BIT_DOCUMENT_CHECKREWIND (1 << 2)	///< Document needs to check for a rewind.
/// @}
/// @name RenderData Bits
/// @{
#define BIT_ACTIVERENDERDATA (1 << 28)	///< Active render data.
/// @}
/// @}

// Object Manager
#define HID_SORT_NAME 30000
#define HID_SHOW_LAY	30001
#define HID_SHOW_FLAT 30002
#define HID_SHOW_VERT 30003

/// @addtogroup FLbase
/// @ingroup group_enumeration
/// @{

// UPDATE SYMBOLS_OLD.H for new ids!

static const Int32 FLbase = 440000249;						///< FieldLayer base.
static const Int32 FLplugin = 440000259;					///< FieldLayer plugin.

// Part of xtensions module
static const Int32 FLfolder	= 440000250;					///< FieldLayer Folder layer.
static const Int32 FLfield	= 440000251;					///< FieldLayer Field object based layer.
static const Int32 FLsolid	= 440000252;					///< FieldLayer Solid value/color layer.
static const Int32 FLdescid = 440000253;					///< FieldLayer DescID based layer.
static const Int32 FLclamp	= 440000295;					///< FieldLayer Clamping remapping layer
static const Int32 FLremap	= 440000296;					///< FieldLayer between values remapping layer
static const Int32 FLcurve	= 440000297;					///< FieldLayer curve remapping layer
static const Int32 FLgradient = 440000298;				///< FieldLayer gradient remapping layer
static const Int32 FLquantize = 440000299;				///< FieldLayer quantize remapping layer
static const Int32 FLinvert = 440000300;					///< FieldLayer invert remapping layer
static const Int32 FLcolorize = 440000301;				///< FieldLayer colorize remapping layer
static const Int32 FLrangemap = 440000302;				///< FieldLayer colorize remapping layer

// Part of mograph module
static const Int32 FLnoise	= 440000290;					///< Fieldlayer Noise remapping layer.
static const Int32 FLspline = 440000276;					///< Spline FieldLayer
static const Int32 FLdelay	= 440000291;					///< Delay FieldLayer
static const Int32 FLdecay = 440000292;						///< Decay FieldLayer
static const Int32 FLstep = 440000293;						///< Step FieldLayer
static const Int32 FLweight = 440000294;					///< Weight FieldLayer
static const Int32 FLproximity = 1040379;					///< Proximity FieldLayer
static const Int32 FLformula = 1040450;						///< Formula FieldLayer
static const Int32 FLtime = 1040451;							///< Time FieldLayer
static const Int32 FLpython = 1040420;						///< Python FieldLayer
static const Int32 FLmograph = 440000325;					///< MoGraph FieldLayer
static const Int32 FLpolygonobject = 440000327;		///< Polygon Object FieldLayer
static const Int32 FLvolumeobject = 440000328;		///< Volume Object FieldLayer
static const Int32 FLparticleobject = 440000330;	///< Particle Object FieldLayer
/// @}

// opointgenerator_panel.h
enum
{
	ID_POINTCREATOR_CREATORTYPE = 5999,
	ID_POINTCREATOR_CREATORTYPE_DISTRIBUTION = 1,
	ID_POINTCREATOR_CREATORTYPE_SHADER = 2,

	ID_POINTCREATOR_CREATEPOINTSTAB = 6000,
	ID_POINTCREATOR_CREATEPOINTSNAME = 6001,
	ID_POINTCREATOR_CREATEPOINTSDISTRIBUTION = 6002,
	ID_POINTCREATOR_CREATEPOINTSUNIFORM = 1,
	ID_POINTCREATOR_CREATEPOINTSNORMAL = 2,
	ID_POINTCREATOR_CREATEPOINTSINVNORMAL = 3,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL = 4,

	ID_POINTCREATOR_CREATEPOINTSNORMAL_STANDARDDEVIATION = 6003,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_X = 6004,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_Y = 6005,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_Z = 6006,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_ON = 1,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_OFF = 2,
	ID_POINTCREATOR_CREATEPOINTSEXPONENTIAL_INV = 3,
	ID_POINTCREATOR_CREATEDPOINTAMOUNT = 6007,
	ID_POINTCREATOR_CREATEPOINTSSEED = 6008,
	ID_POINTCREATOR_CREATEPOINTS_ONLY_INSIDE = 6009,
	ID_POINTCREATOR_CREATEPOINTS_ONLY_INSIDE_HIGH_QUALITY = 6038,
	ID_POINTCREATOR_CREATEPOINTS_PEROBJECT = 6010,

	ID_POINTCREATOR_CREATEPOINTS_TRANSFORMATIONTAB = 6011,
	ID_POINTCREATOR_CREATEPOINTS_TRANSLATE = 6012,
	ID_POINTCREATOR_CREATEPOINTS_SCALE = 6013,
	ID_POINTCREATOR_CREATEPOINTS_ROTATE = 6014,


	ID_POINTCREATOR_SHADERTAB = 6016,
	ID_POINTCREATOR_SHADERNAME = 6017,
	ID_POINTCREATOR_SHADERINPUT = 6018,

	ID_POINTCREATOR_SHADER_SAMPLEMODE = 6035,
	ID_POINTCREATOR_SHADER_SAMPLEMODE_VOLUME = 1,
	ID_POINTCREATOR_SHADER_SAMPLEMODE_SURFACE = 2,
	ID_POINTCREATOR_SHADERSAMPLEAMOUNT = 6036,
	ID_POINTCREATOR_SHADERSAMPLEDENSITY = 6019,
	ID_POINTCREATOR_SHADERSAMPLESEED = 6020,
	ID_POINTCREATOR_SHADERSAMPLESAMPLEDEPTH = 6021,

	ID_POINTCREATOR_SHADER_CHANNELSELECT = 6022,

	ID_POINTCREATOR_SHADER_CHANNELSELECT_COLOR = 807,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_DIFFUSION = 808,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_LUMINANCE = 809,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_TRANSPARENCY = 810,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_REFLECTION = 811,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_ENVIRONMENT = 812,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_BUMP = 814,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_ALPHA = 815,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_SPECULAR = 816,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_DISPLACEMENT = 819,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_NORMAL = 3301,
	ID_POINTCREATOR_SHADER_CHANNELSELECT_DIRECT = 830,

	ID_POINTCREATOR_SHADER_EDIT_CHANNEL = 6023,
	ID_POINTCREATOR_SHADER_MATERIALTAG = 6024,
	ID_POINTCREATOR_SHADER_SHADER = 6025,
	ID_POINTCREATOR_SHADER_GROUPPROJECTION = 6026,
	ID_POINTCREATOR_SHADER_TEXTAG_OFFSETX = 6027,
	ID_POINTCREATOR_SHADER_TEXTAG_OFFSETY = 6028,
	ID_POINTCREATOR_SHADER_TEXTAG_LENGTHX = 6029,
	ID_POINTCREATOR_SHADER_TEXTAG_LENGTHY = 6030,
	ID_POINTCREATOR_SHADER_TEXTAG_TILE = 6031,
	ID_POINTCREATOR_SHADER_GROUPALPHA = 6032,
	ID_POINTCREATOR_SHADER_USE = 6033,
	ID_POINTCREATOR_SHADER_USE_ALPHA = 1,
	ID_POINTCREATOR_SHADER_USE_GRAY = 2,
	ID_POINTCREATOR_SHADER_USE_RED = 3,
	ID_POINTCREATOR_SHADER_USE_GREEN = 4,
	ID_POINTCREATOR_SHADER_USE_BLUE = 5,
	ID_POINTCREATOR_SHADER_INVERTALPHA = 6034,
	ID_POINTCREATOR_SHADER_PEROBJECT = 6037,
};

// BaseSettings / TimeLine
#define ID_BS_HOOK	 100004808	///< BaseSceneHook ID.
#define ID_TAKE_HOOK 431000055	///< TakeSceneHook ID.

enum
{
	TLMARKER_TIME					= 1000,
	TLMARKER_TEXT					= 1001,
	TLMARKER_TIMEBACKUP		= 1002,
	TLMARKER_TASK					= 1003,
	TLMARKER_DONE					= 1004,
	TLMARKER_SELECT				= 1005,
	TLMARKER_CHECKED			= 1006,
	TLMARKER_COLOR				= 1007,
	TLMARKER_LENGTH				= 1008,
	TLMARKER_LENGTHBACKUP = 1009
};


// snap flags
#define TL_SNAP_GRL_PRJIN		 (1 << 2)
#define TL_SNAP_GRL_PRJOUT	 (1 << 3)
#define TL_SNAP_GRL_KEY			 (1 << 4)
#define TL_SNAP_GRL_CURTIME	 (1 << 5)
#define TL_SNAP_GRL_PREVTIME (1 << 6)
#define TL_SNAP_GRL_FRAME		 (1 << 7)
#define TL_SNAP_GRL_VISIBLE	 (1 << 8)
#define TL_SNAP_GRL_MARKER	 (1 << 9)
#define TL_SNAP_GRL_FKEY		 (1 << 10)
#define TL_SNAP_GRL_FGRID		 (1 << 11)
#define TL_SNAP_GRL_CLIP		 (1 << 12)

// motion view settings
#define TL_MV_FROM			 (1 << 2)
#define TL_MV_TO				 (1 << 3)
#define TL_MV_LOOP			 (1 << 4)
#define TL_MV_TRIM			 (1 << 5)
#define TL_MV_SOURCENAME (1 << 6)
#define TL_MV_CLIPNAME	 (1 << 7)
#define TL_MV_NONAME		 (1 << 8)
#define TL_MV_SCALE			 (1 << 9)
// #define TL_MV_LOOPCOUNT (1<<10)
#define TL_MV_FCINLAYER (1 << 11)
#define TL_MV_PICTURE		(1 << 12)
#define TL_MV_UNTRIMMED	(1 << 13)

// track/master flags
#define TL_FLAG_TR_PARENTSELECTED (1 << 2)
#define TL_FLAG_TR_RECALCMOVE			(1 << 3)
#define TL_FLAG_TR_ISLOCKED				(1 << 4)
#define TL_FLAG_TR_FCOPEN					(1 << 5)
#define TL_FLAG_TR_CHANGEDMASTER	(1 << 6)
#define TL_FLAG_TR_SUBMUTE				(1 << 7)
#define TL_FLAG_TR_CALCJUMP				(1 << 8)
#define TL_FLAG_TR_NOMUTESOLO			(1 << 10)
#define TL_FLAG_TR_ISSOLO					(1 << 11)
#define TL_FLAG_TR_SUBSOLO				(1 << 12)
#define TL_FLAG_TR_HIDEMASTER			(1 << 13)
#define TL_FLAG_TR_TRACKSEQSEL		(1 << 14)
#define TL_FLAG_TR_VALUETRACK			(1 << 15)
#define TL_FLAG_TR_ZEROHEIGHT			(1 << 16)
#define TL_FLAG_TR_PSTRSEQSEL			(1 << 17)
#define TL_FLAG_TR_DEPSTRSEQSEL		(1 << 18)
#define TL_FLAG_TR_ANIMATIONLAYER (1 << 19)
#define TL_FLAG_TR_HASLAYER				(1 << 20)
#define TL_FLAG_TR_COSTANT				(1 << 21)
#define TL_FLAG_TR_RELATIV				(1 << 22)
#define TL_FLAG_TR_UNDO						(1 << 23)
#define TL_FLAG_TR_SYNC						(1 << 24)

// track/master and key/master key flags
#define TL_FLAG_SUBSELECTED		 (1 << 0)
#define TL_FLAG_ISSELECTED		 (1 << 2)
#define TL_FLAG_MOVED					 (1 << 3)
#define TL_FLAG_MAINMUTE			 (1 << 4)
#define TL_FLAG_VISIBLEELEMENT (1 << 5)
#define TL_FLAG_MOVECRTL			 (1 << 6)
#define TL_FLAG_PSEUDOSELECT	 (1 << 7)
#define TL_FLAG_PSEUDODESELECT (1 << 8)
#define TL_FLAG_MOVEELEMENT		 (1 << 10)
#define TL_FLAG_MOVELTANG			 (1 << 11)
#define TL_FLAG_MOVERTANG			 (1 << 12)
#define TL_FLAG_REGIONONE			 (1 << 13)
#define TL_FLAG_CHANGED				 (1 << 14)
#define TL_FLAG_REGVERT				 (1 << 15)
#define TL_FLAG_REGHOR				 (1 << 16)
#define TL_FLAG_DOROLLRIPPLE	 (1 << 17)
#define TL_FLAG_TEMPKEY				 (1 << 18)
#define TL_FLAG_DIRTY					 (1 << 19)
#define TL_FLAG_CLIPLOCK			 (1 << 20)
#define TL_FLAG_CLIPTRANSITION (1 << 21)
#define TL_FLAG_MOVESHIFT			 (1 << 22)
#define TL_FLAG_CLIPPRE				 (1 << 23)
#define TL_FLAG_CLIPNEXT			 (1 << 24)
#define TL_FLAG_CLIPAUTO			 (1 << 25)
#define TL_FLAG_CLIPAUTOGAP		 (1 << 26)
#define TL_FLAG_MOVEREMOVE		 (1 << 27)
#define TL_FLAG_CLIPRELATIVE	 (1 << 28)


// key flags
#define TL_FLAG_ISCLAMP					(1 << 0)
#define TL_FLAG_BREAK						(1 << 2)
#define TL_FLAG_KEEPVISUALANGLE (1 << 3)
#define TL_FLAG_LOCKO						(1 << 4)
#define TL_FLAG_LOCKL						(1 << 5)
#define TL_FLAG_AUTO						(1 << 6)
#define TL_FLAG_LOCKTIME				(1 << 10)
#define TL_FLAG_LOCKVALUE				(1 << 11)
#define TL_FLAG_LEFTTANGENT			(1 << 12)
#define TL_FLAG_RIGHTTANGENT		(1 << 13)
#define TL_FLAG_MUTE						(1 << 14)
#define TL_FLAG_BREAKDOWN				(1 << 15)

// area flags
#define TL_FLAG_MOVEDRAW			(1 << 0)
#define TL_FLAG_DRAGTIME			(1 << 8)
#define TL_FLAG_SHOWINDICATOR (1 << 10)
#define TL_FLAG_NEWHIGHLIGHT	(1 << 11)
#define TL_FLAG_INMAINAREA		(1 << 12)
#define TL_FLAG_MOTIONLINE		(1 << 13)

// TL flags
#define TL_FLAG_LINKTOPREVIEW				(1 << 0)
#define TL_FLAG_PREPOSTON						(1 << 1)
#define TL_FLAG_ANIMATEDONLY				(1 << 3)
#define TL_FLAG_HLEMODE							(1 << 4)
#define TL_FLAG_SNAP								(1 << 5)
#define TL_FLAG_AUTOMATICMODE				(1 << 6)
#define TL_FLAG_GRID_DELETEME				(1 << 7)
#define TL_FLAG_TANGENT							(1 << 8)
#define TL_FLAG_REGIONTOOL					(1 << 9)
#define TL_FLAG_HLE_MODNOCURVE			(1 << 10)
#define TL_FLAG_VELOCITY						(1 << 11)
#define TL_FLAG_FCSHOWALL						(1 << 12)
#define TL_FLAG_QUANTIZEINTERACTION (1 << 13)
#define TL_FLAG_LINKSM							(1 << 14)
#define TL_FLAG_ROLLRIPPLE					(1 << 16)
#define TL_FLAG_TOLSELECTION				(1 << 17)
#define TL_FLAG_SHOWTRACKVALUE			(1 << 18)
#define TL_FLAG_PREVIEWENABLE				(1 << 19)
#define TL_FLAG_SHOWMINMAX					(1 << 20)
#define TL_FLAG_ENABLESNAPSHOT1			(1 << 21)
#define TL_FLAG_SHOWSELTANGENTS			(1 << 22)
#define TL_FLAG_SHOWSELKEYS					(1 << 23)
#define TL_FLAG_NOGRID_DELETEME			(1 << 24)
#define TL_FLAG_SHOWALLKEYS					(1 << 25)
#define TL_FLAG_MOVESEQANDCURVE			(1 << 26)
#define TL_FLAG_LINKSELECTION				(1 << 27)
#define TL_FLAG_ENABLESNAPSHOT2			(1 << 28)
#define TL_FLAG_ENABLESNAPSHOT3			(1 << 29)
#define TL_FLAG_ENABLESNAPSHOT4			(1 << 30)
#define TL_FLAG_ENABLESNAPSHOT5			(1 << 31)

// TLFlag2
#define TL2_FLAG_MOTIONCUTCON			(1 << 0)
#define TL2_FLAG_MOTIONMOVEFC			(1 << 1)
#define TL2_FLAG_MOTIONBLEND			(1 << 2)
#define TL2_FLAG_MTREMOVEMC				(1 << 3)
#define TL2_FLAG_SHOWACTIVELAYER	(1 << 4)
#define TL2_FLAG_MOTIONFCINTER		(1 << 5)
#define TL2_FLAG_HIDEANIMLAYER		(1 << 6)
#define TL2_FLAG_SHOWSOUND				(1 << 7)
#define TL2_FLAG_SHOWTANGENTCURVE (1 << 8)
#define TL2_FLAG_MAYALINKING			(1 << 9)
#define TL2_FLAG_LOCKRATIO				(1 << 10)
#define TL2_FLAG_ACTIVETAKE				(1 << 11)

// Hle flags
#define ID_TL_HLE_CURVE	0
#define ID_TL_HLE_MOVE	1
#define ID_TL_HLE_SCALE	2

// pref flags
#define TL_FLAG_SHOWTRACKSSTARTEND			(1 << 2)
#define TL_FLAG_KEYSYMBOLS							(1 << 3)
#define TL_FLAG_HIGHLIGHTCURVE					(1 << 4)
#define TL_FLAG_HIGHLIGHT								(1 << 5)
#define TL_FLAG_SHOWBRANCHES						(1 << 6)
#define TL_FLAG_SHOWVECTRACK						(1 << 7)
#define TL_FLAG_CURSORINDICATOR					(1 << 8)
#define TL_FLAG_MINIMUM									(1 << 10)
#define TL_FLAG_SHOWLAYERCOL						(1 << 11)
#define TL_FLAG_SUMMARY									(1 << 13)
#define TL_FLAG_INTERACTIVEUPDATE				(1 << 14)
#define TL_FLAG_KEYSINBETWEEN						(1 << 15)
#define TL_FLAG_LINKKEYFCSELECTION			(1 << 16)
#define TL_FLAG_LINKOMSELECTONLY				(1 << 17)
#define TL_FLAG_GLOBALSELECTION					(1 << 18)
#define TL_FLAG_LINKOMSELECTONLYBUTTAGS (1 << 19)
#define TL_FLAG_FCLISTVIEWHIERARCHICAL	(1 << 20)
#define TL_FLAG_FCLISTVIEW							(1 << 21)

// double click
#define TL_FLAG_CURRTIME			(1 << 0)
#define TL_FLAG_ADDKEY				(1 << 13)
#define TL_FLAG_KEY						(1 << 14)
#define TL_FLAG_CLIPSTART			(1 << 15)
#define TL_FLAG_CLIPEND				(1 << 16)
#define TL_FLAG_CLIPTRIMSTART (1 << 17)
#define TL_FLAG_CLIPTRIMEND		(1 << 18)
#define TL_FLAG_CLIPSCALE			(1 << 19)
#define TL_FLAG_LAYERVALUE		(1 << 20)
#define TL_FLAG_CLIPMIXSTART	(1 << 21)
#define TL_FLAG_CLIPMIXEND		(1 << 22)
#define TL_FLAG_CLIPMOVE2			(1 << 23)
#define TL_FLAG_CLIPSCALEBOTH	(1 << 24)
#define TL_FLAG_CLIPLOOP			(1 << 25)
#define TL_FLAG_CLIPSOURCE		(1 << 26)
#define TL_FLAG_CLIPNAME			(1 << 27)
#define TL_FLAG_CLIPAUTOOFF		(1 << 28)
#define TL_FLAG_MARKERNAME		(1 << 29)

// masterstruct flags
#define TL_FLAG_TI_SELECTEDKEY		(1 << 2)
#define TL_FLAG_TI_SELECTEDSUBKEY (1 << 3)
#define TL_FLAG_TI_MUTE						(1 << 4)
#define TL_FLAG_TI_BREAKDOWN			(1 << 5)
#define TL_FLAG_TI_CLIPLOCK				(1 << 6)

// region flags
#define TL_FLAG_REGMOVE_NO			(1 << 0)
#define TL_FLAG_REGMOVE_AREA		(1 << 2)
#define TL_FLAG_REGMOVE_AREA_LT	(1 << 3)
#define TL_FLAG_REGMOVE_AREA_MT	(1 << 4)
#define TL_FLAG_REGMOVE_AREA_RT	(1 << 5)
#define TL_FLAG_REGMOVE_AREA_LB	(1 << 6)
#define TL_FLAG_REGMOVE_AREA_MB	(1 << 7)
#define TL_FLAG_REGMOVE_AREA_RB	(1 << 8)
#define TL_FLAG_REGMOVE_AREA_LY	(1 << 9)
#define TL_FLAG_REGMOVE_AREA_RY	(1 << 10)

// Create source Flag
#define TL_FLAG_SOURCE_LAYER	 (1 << 2)
#define TL_FLAG_SOURCE_POS		 (1 << 3)
#define TL_FLAG_SOURCE_ROT		 (1 << 4)
#define TL_FLAG_SOURCE_SCALE	 (1 << 5)
#define TL_FLAG_SOURCE_PARAM	 (1 << 6)
#define TL_FLAG_SOURCE_PLA		 (1 << 7)
#define TL_FLAG_SOURCE_BAKE		 (1 << 8)
#define TL_FLAG_SOURCE_REMOVE	 (1 << 9)
#define TL_FLAG_SOURCE_ALL		 (1 << 10)
#define TL_FLAG_SOURCE_ALLTIME (1 << 11)
#define TL_FLAG_SOURCE_BIGPIC	 (1 << 12)

// bake Flag
#define TL_FLAG_BAKE_POS				(1 << 3)
#define TL_FLAG_BAKE_ROT				(1 << 4)
#define TL_FLAG_BAKE_SCALE			(1 << 5)
#define TL_FLAG_BAKE_PARAM			(1 << 6)
#define TL_FLAG_BAKE_PLA				(1 << 7)
#define TL_FLAG_BAKE_EXPRESSION	(1 << 8)
#define TL_FLAG_BAKE_COPY				(1 << 9)
#define TL_FLAG_BAKE_CLEAN			(1 << 10)
#define TL_FLAG_BAKE_ALL				(1 << 11)

#define TL_TYPE_OBJECT 0
#define TL_TYPE_BRANCH 1
#define TL_TYPE_VECTOR 2
#define TL_TYPE_TRACK	 3
#define TL_TYPE_MOTION 4
#define TL_TYPE_LAYER	 5

// Euler Filter Flag
#define TL_FLAG_EULERFILTER_SYNCHROT	 (1 << 1)
#define TL_FLAG_EULERFILTER_MINGIMBAL	 (1 << 2)
#define TL_FLAG_EULERFILTER_ADJTANGENT (1 << 3)

enum GRID_TYPE
{
	GRID_TYPE_NO		 = 0,
	GRID_TYPE_SUB		 = 1,
	GRID_TYPE_FINE	 = 2,
	GRID_TYPE_MIDDLE = 3,
	GRID_TYPE_SEC		 = 4
} ENUM_END_LIST(GRID_TYPE);

enum VisualisationMode
{
	VM_Key = 0,
	VM_FCurve,
	VM_Motion,
	VM_DUM,
};

// id's for save TL settings
#define HID_GRID_OLD		20000	// has to be removed because of a bug
#define HID_SOUNDNAME		20001
#define HID_STARTTIME		20002
#define HID_ENDTIME			20003
#define HID_BACKUPSTART	20004
#define HID_BACKUPEND		20005
#define HID_SNAP				20006
#define HID_FLAGS				20007
#define HID_DISTSNAP		20008
#define HID_FCSCALE			20009
#define HID_FCMOVE			20010
#define HID_LINKCOUNT		20012
#define HID_INDIVIDUAL	20014
#define HID_MODE				20015
#define HID_DISTRSNAP		20016
#define HID_HLE					20017
#define HID_FLAGS2			20018
#define HID_MVSET				20019
#define HID_SOUNDTRACK	20020
#define HID_GRIDTYPE		20021
#define HID_GRIDVALUE		20022
#define HID_MARKERCOUNT	20023
#define HID_MARKER			21000
#define HID_LINK				22000

/// @addtogroup TakeTypes
/// @ingroup group_enumeration
/// @{
/// @since 17.008 and @C4D R17.032
#define OverrideBase	431000051	///< Override Node.
#define OverrideGroup	431000075	///< Override Group Node.
#define TakeBase			431000054	///< Take Nodes.
#define TakeFolder		431000073	///< Take Folder Nodes.
/// @}

enum
{
	ID_OVERRIDEGROUP_NODELIST						= 1000,		// the group linked elements. The link list (InExclude) must contain real document object
	ID_OVERRIDEGROUP_TAG_DOCUMENT_START = 20000,	// start index for the subcontainer document tags map
	ID_OVERRIDEGROUP_TAG_BACKUP_START		= 21000,	// start index for the subcontainer backup tags map
	ID_OVERRIDEGROUP_TAG_COUNT					= 10000		// the number of elements in the tag map
};

static const Int32 ID_OVERRIDEPLACEHOLDER_LINK = 10000;	// this is the container id for the folder type

enum
{
	FOLDERTYPE_NONE = 0,
	FOLDERTYPE_OBJ,
	FOLDERTYPE_TAG,
	FOLDERTYPE_SHD,
	FOLDERTYPE_MAT,
	FOLDERTYPE_LAYER,
	FOLDERTYPE_OTHER,
	_STANDARD_FOLDER_COUNT,	// just utility
	FOLDERTYPE_SINGLE
};

#define		TL_MARKEROBJ						465001514
#define		TIMELINE_ID							465001516
#define		TIMELINE_DATA						465001515
#define		ID_REGIONOBJ						465001517
#define   ID_TLCOPY								465001518
#define   ID_MT_CLIPCUSTONGUI			465001521
#define   ID_MT_ENABLE						465001522
#define   ID_MT_SOURCEDLGONOFF		465001525
#define   ID_TLRECALCMESSAGE			465001526
#define   ID_MT_ARRAYBITMAPID			465001527
#define   ID_MT_CUSTOMIDAB				465001528
#define   ID_TLCOPYMOTION					465001529
#define		MT_LIBID								465001530
#define		MT_SOURCEPIC1						465001531
#define		MT_SOURCEPIC2						465001532
#define		TL_BAKE_MESSAGE					465001533
#define		ID_TL_M_ASCII						1015840
#define		M_TIMELINE							10068
#define		C4D_TLMARKER_LIBRARY_ID	465001534
#define		WORLD_TL_HOOK_ID				465001535
#define		WORLD_TODO_HOOK_ID			465001536
#define		TAG_TODO								465001537
#define		ID_TODO_CUSTONGUI				465001538
#define		ID_TODO_SELECT_TASK			465001539
#define		ID_TODO_SELECT_DONE			465001540

#define		TIMELINE_DOPESHEET 465001541
#define		TIMELINE_FCURVE		 465001542

// channels
#define MAX_MATERIALCHANNELS 14	// channels in C4D material

#define CHANNEL_ANY	-1					// unlabeled plugin channel

#define CHANNEL_COLOR					0
#define CHANNEL_LUMINANCE			1
#define CHANNEL_TRANSPARENCY	2
#define CHANNEL_REFLECTION		3
#define CHANNEL_ENVIRONMENT		4
#define CHANNEL_FOG						5
#define CHANNEL_BUMP					6
#define CHANNEL_ALPHA					7
#define CHANNEL_SPECULAR			8
#define CHANNEL_SPECULARCOLOR 9
#define CHANNEL_GLOW					10
#define CHANNEL_DISPLACEMENT	11
#define CHANNEL_DIFFUSION			12
#define CHANNEL_NORMAL				13


#define ID_TIMETRACK -1
#define CTplugin		 5353

// dynamic spline
#define CTdynamicspline	1029124

// CTdynamicsplinedata
#define CK_DYNAMICSPLINE_DATA 10001	// dynamic spline custom data type

enum
{
	VECTOR_X = 1000,
	VECTOR_Y = 1001,
	VECTOR_Z = 1002
};

enum CCURVE
{
	CCURVE_CURVE		 = 1,
	CCURVE_HLE_BASE	 = 2,
	CCURVE_HLE_CURVE = 3,
	CCURVE_SS_CURVE	 = 4,

	// multiple Snapshots
	CCURVE_SS_CURVE2 = 5,
	CCURVE_SS_CURVE3 = 6,
	CCURVE_SS_CURVE4 = 7,
	CCURVE_SS_CURVE5 = 8,

	// Scale and Move HLE Curve
	CCURVE_HLE_SCALE = 9,
	CCURVE_HLE_MOVE	 = 10
} ENUM_END_LIST(CCURVE);

enum CLOOP
{
	CLOOP_OFF					 = 0,
	CLOOP_CONSTANT		 = 1,
	CLOOP_CONTINUE		 = 2,
	CLOOP_REPEAT			 = 3,
	CLOOP_OFFSETREPEAT = 4,
	CLOOP_OSCILLATE		 = 5
} ENUM_END_LIST(CLOOP);

// display filter	(NULL to OTHER match SELECTIONFILTERBIT_)
enum DISPLAYFILTER
{
	DISPLAYFILTER_0									 = 0,
	DISPLAYFILTER_NULL							 = (1 << 0),
	DISPLAYFILTER_POLYGON						 = (1 << 1),
	DISPLAYFILTER_SPLINE						 = (1 << 2),
	DISPLAYFILTER_GENERATOR					 = (1 << 3),
	DISPLAYFILTER_HYPERNURBS				 = (1 << 4),
	DISPLAYFILTER_UNUSED1						 = (1 << 5),
	DISPLAYFILTER_DEFORMER					 = (1 << 6),
	DISPLAYFILTER_CAMERA						 = (1 << 7),
	DISPLAYFILTER_LIGHT							 = (1 << 8),
	DISPLAYFILTER_SCENE							 = (1 << 9),
	DISPLAYFILTER_PARTICLE					 = (1 << 10),
	DISPLAYFILTER_OTHER							 = (1 << 11),
	DISPLAYFILTER_GRID							 = (1 << 13),
	DISPLAYFILTER_HORIZON						 = (1 << 14),
	DISPLAYFILTER_WORLDAXIS					 = (1 << 15),
	DISPLAYFILTER_BOUNDS						 = (1 << 16),
	DISPLAYFILTER_HUD								 = (1 << 17),
	DISPLAYFILTER_SDS								 = (1 << 18),
	DISPLAYFILTER_HIGHLIGHTING			 = (1 << 19),
	DISPLAYFILTER_MULTIAXIS					 = (1 << 20),
	DISPLAYFILTER_OBJECTHANDLES			 = (1 << 21),
	DISPLAYFILTER_HANDLEBANDS				 = (1 << 22),
	DISPLAYFILTER_SDSCAGE						 = (1 << 23),
	DISPLAYFILTER_NGONLINES					 = (1 << 24),
	DISPLAYFILTER_JOINT							 = (1 << 25),
	DISPLAYFILTER_OBJECTHIGHLIGHTING = (1 << 26),
	DISPLAYFILTER_GUIDELINES				 = (1 << 27),
	DISPLAYFILTER_POI								 = (1 << 28)
} ENUM_END_FLAGS(DISPLAYFILTER);

enum DISPLAYEDITSTATE
{
	DISPLAYEDITSTATE_0				= 0,
	DISPLAYEDITSTATE_SDS			= (1 << 0),
	DISPLAYEDITSTATE_DEFORM		= (1 << 1),

	DISPLAYEDITSTATE_DOCUMENT	= -1
} ENUM_END_FLAGS(DISPLAYEDITSTATE);

enum DISPLAYMODE
{
	DISPLAYMODE_UNKNOWN					= -1,
	DISPLAYMODE_GOURAUD					= 0,
	DISPLAYMODE_QUICK						= 1,
	DISPLAYMODE_WIRE						= 2,
	DISPLAYMODE_ISOPARM					= 3,
	DISPLAYMODE_SHADEDBOX				= 4,
	DISPLAYMODE_BOX							= 5,
	DISPLAYMODE_SKELETON				= 6,
	DISPLAYMODE_GOURAUDWIRE			= 7,
	DISPLAYMODE_GOURAUDISOPARM	= 8,
	DISPLAYMODE_QUICKWIRE				= 9,
	DISPLAYMODE_QUICKISOPARM		= 10,
	DISPLAYMODE_FLATWIRE				= 11,
	DISPLAYMODE_FLATISOPARM			= 12,
	DISPLAYMODE_FLATBOX					= 13,
	DISPLAYMODE_HIDDENWIRE			= 14,
	DISPLAYMODE_HIDDENISOPARM		= 15,
	DISPLAYMODE_HIDDENBOX				= 16,
	DISPLAYMODE_SHADEDBOXWIRE		= 17,
	DISPLAYMODE_QUICKBOXWIRE		= 18,
	DISPLAYMODE_QUICKBOX				= 19,

	DISPLAYMODE_PRIVATE_ISOLINE	= 100,
	DISPLAYMODE_PRIVATE_FLAT		= 1100,
	DISPLAYMODE_PRIVATE_HIDDEN	= 1400
} ENUM_END_LIST(DISPLAYMODE);

// timeline doc settings
#define TIMELINE_POS			10014
#define TIMELINE_SCALE		10015
#define TIMELINE_ROT			10016
#define TIMELINE_PARAM		10018
#define TIMELINE_PLA			10019
#define TIMELINE_AUTOKEYS	10020

#define TIMELINE_PLAY_MODE 10034					// LONG
#define TIMELINE_USE_LOOP	 10035					// BOOL
#define TIMELINE_DROPLESS	 10036					// BOOL
#define TIMELINE_FPS_MODE	 10037					// LONG
#define TIMELINE_DIR			 10039					// REAL

#define TIMELINE_RUNNING						10041	// LONG, 1 forward, 2 backward - (unused in cineware)
#define TIMELINE_SOUND							10046	// BOOL
#define TIMELINE_KEYFRAMEGENERATION	10051	// BOOL not needed anymore
#define TIMELINE_LINK_XYZGENERATION	10052	// BOOL

// material bits
#define BIT_MATMARK				(1 << 2)	// marked material
#define BIT_ENABLEPAINT		(1 << 3)	// enable painting
#define BIT_RECALCPREVIEW	(1 << 5)	// recalculate preview
#define BIT_MFOLD					(1 << 6)	// folded in material manager
#define BIT_BP_FOLDLAYERS	(1 << 9)	// fold layers in material manger

#define RDATA_DISKID				 300001047
#define CHUNK_ROOT_MULTIPASS 300001049
#define Zmultipass					 300001048
#define POSTEFFECT_DISKID		 300001060
#define ID_MT_SOURCECOUNTER	 465001520	// LONG
#define DOCUMENT_RVIEW			 10001

#define HERMITEFAK 4.0	// internal scaling of tangent vectors for hermite interpolation for better curve control

#define PGONEDGE_REPEAT			(0x80000000)
#define PGONEDGE_RESET			(0x7FFFFFFF)
#define PGONEDGE_NOEDGES		(0x40000000)
#define PGONEDGE_EDGEINDEX	(0x1FFFFFFF)
#define PGONEDGE_ENDSEGMENT	(0x20000000)

#define CHUNK_V6_ROOTLIST2D		110100
#define OBJECT_PAINTLAYER			(501)
#define OBJECT_PAINTTEXTURE		(502)
#define OBJECT_PAINTVOXEL			(505)
#define OBJECT_PAINTSELECTION (506)

#define OBJECT_SELECTIONHEAD (596)
#define OBJECT_PAINTTOOLHEAD (597)	// helper object
#define OBJECT_ALPHAHEAD		 (599)	// helper object

#define B3D_BYTE_MAX 5

#define LAYER_NORMAL		 0
#define LAYER_DISSOLVE	 1
#define LAYER_DIFFERENCE 2
#define LAYER_LIGHTEN		 3
#define LAYER_DARKEN		 4
#define LAYER_MULTIPLY	 5
#define LAYER_SCREEN		 6
#define LAYER_ADD				 7
#define LAYER_EXCLUSION	 8

#define PAINT_IDENT								'Body'
#define PAINT_TEXTURE							'BdTx'
#define PAINT_LAYER								'BdLy'
#define PAINT_BITMAP							'BdBm'
#define PAINT_VOXELARRAY					'BdVx'
#define PAINT_ALPHAHEAD						'BdAh'
#define PAINT_TOOL								'BdTl'
#define PAINT_SETTINGS						'BdSt'
#define PAINT_SETTINGSPREVIEW			'BdPr'
#define PAINT_BRUSHSETTINGS				'BdBr'
#define PAINT_SBRUSHSETTINGS			'BdBs'
#define PAINT_BBRUSHSETTINGS			'BdBb'
#define PAINT_COLORSETTINGS				'BdCs'
#define PAINT_BRUSH_PROFILE				'BdPf'
#define PAINT_SINGLECOLORSETTINGS 'BdSc'

#define LAYER_NORMAL		 0
#define LAYER_DISSOLVE	 1
#define LAYER_DIFFERENCE 2
#define LAYER_LIGHTEN		 3
#define LAYER_DARKEN		 4
#define LAYER_MULTIPLY	 5
#define LAYER_SCREEN		 6
#define LAYER_ADD				 7
#define LAYER_EXCLUSION	 8

// ----- CINEMA file identification -----
#define R12_IDENT			'QC4D'
#define R12BETA_IDENT	'YC4D'
#define OXYGEN_IDENT	'XC4D'
#define SEVEN_IDENT		'6C4D'
#define AMBER_IDENT		'AC4D'
#define TMP_IDENT			'8C4D'

#define DOC_IDENT	 'C4D6'
#define PRF_IDENT	 'PRFY'
#define LAY_IDENT	 'LAY6'
#define PAL_IDENT	 'PAL6'
#define LPRF_IDENT 'LPR6'

enum HYPERFILEARRAY
{
	HYPERFILEARRAY_CHAR	 = 1,
	HYPERFILEARRAY_WORD	 = 2,
	HYPERFILEARRAY_LONG	 = 3,
	HYPERFILEARRAY_LLONG = 4,
	HYPERFILEARRAY_SREAL = 5,
	HYPERFILEARRAY_LREAL = 6,
	HYPERFILEARRAY_REAL	 = 7
} ENUM_END_LIST(HYPERFILEARRAY);

enum FILEERROR
{
	FILEERROR_NONE				=  0,	// no error
	FILEERROR_OPEN				= -1,	// problems opening the file
	FILEERROR_CLOSE				= -2,	// problems closing the file
	FILEERROR_READ				= -3,	// problems reading the file
	FILEERROR_WRITE				= -4,	// problems writing the file
	FILEERROR_SEEK				= -5,	// problems seeking the file
	FILEERROR_INVALID			= -6,	// invalid parameter or operation (e.g. writing in read-mode)
	FILEERROR_OUTOFMEMORY	= -7,	// not enough memory
	FILEERROR_USERBREAK		= -8,	// user break

	// the following values can only occur in HyperFiles
	FILEERROR_WRONG_VALUE		 = -100,	// other value detected than expected
	FILEERROR_CHUNK_NUMBER	 = -102,	// wrong number of chunks or sub chunks detected
	FILEERROR_VALUE_NO_CHUNK = -103,	// there was a value without any enclosing START/STOP chunks
	FILEERROR_FILE_END			 = -104,	// the file end was reached without finishing reading
	FILEERROR_UNKNOWN_VALUE	 = -105		// unknown value detected
} ENUM_END_LIST(FILEERROR);

enum FILEOPEN
{
	FILEOPEN_APPEND				= 0,
	FILEOPEN_READ					= 1,
	FILEOPEN_WRITE				= 2,
	FILEOPEN_READWRITE		= 3,
	FILEOPEN_READ_NOCACHE	= 4,
	FILEOPEN_SHAREDREAD		= 5
} ENUM_END_LIST(FILEOPEN);

enum FILESEEK
{
	FILESEEK_START		= 0,
	FILESEEK_RELATIVE	= 2
} ENUM_END_LIST(FILESEEK);

enum FILEDIALOG
{
	FILEDIALOG_NONE				= 0,
	FILEDIALOG_ANY				= 1,
	FILEDIALOG_IGNOREOPEN	= 2
} ENUM_END_LIST(FILEDIALOG);

enum BYTEORDER
{
	BYTEORDER_MOTOROLA = 1,
	BYTEORDER_INTEL		 = 2
} ENUM_END_LIST(BYTEORDER);

enum HYPERFILEVALUE
{
	HYPERFILEVALUE_NONE							 =  0,

	HYPERFILEVALUE_START						 =  1,
	HYPERFILEVALUE_STOP							 =  2,
	HYPERFILEVALUE_CSTOP						 =  3,
	HYPERFILEVALUE_CHAR							 = 11,
	HYPERFILEVALUE_UCHAR						 = 12,
	HYPERFILEVALUE_INT16						 = 13,
	HYPERFILEVALUE_UINT16						 = 14,
	HYPERFILEVALUE_INT32						 = 15,
	HYPERFILEVALUE_UINT32						 = 16,
	HYPERFILEVALUE_INT64						 = 17,
	HYPERFILEVALUE_UINT64						 = 18,
	HYPERFILEVALUE_FLOAT						 = 19,
	HYPERFILEVALUE_FLOAT64					 = 20,
	HYPERFILEVALUE_BOOL							 = 21,
	HYPERFILEVALUE_TIME							 = 22,
	HYPERFILEVALUE_VECTOR						 = 23,
	HYPERFILEVALUE_VECTOR64					 = 24,
	HYPERFILEVALUE_MATRIX						 = 25,
	HYPERFILEVALUE_MATRIX64					 = 26,
	HYPERFILEVALUE_VECTOR32					 = 27,
	HYPERFILEVALUE_MATRIX32					 = 28,
	HYPERFILEVALUE_FLOAT32					 = 29,

	HYPERFILEVALUE_MEMORY						 = 128,
	HYPERFILEVALUE_IMAGE						 = 129,
	HYPERFILEVALUE_STRING						 = 130,
	HYPERFILEVALUE_FILENAME					 = 131,
	HYPERFILEVALUE_CONTAINER				 = 132,
	HYPERFILEVALUE_ALIASLINK				 = 138,
	HYPERFILEVALUE_LMEMORY					 = 139,
	HYPERFILEVALUE_VECTOR_ARRAY_EX	 = 133,
	HYPERFILEVALUE_POLYGON_ARRAY_EX	 = 134,
	HYPERFILEVALUE_UWORD_ARRAY_EX		 = 135,
	HYPERFILEVALUE_PARTICLE_ARRAY_EX = 136,
	HYPERFILEVALUE_SREAL_ARRAY_EX		 = 137,
	HYPERFILEVALUE_ARRAY						 = 140
} ENUM_END_LIST(HYPERFILEVALUE);

enum RLAFLAGS
{
	RLAFLAGS_0							 = 0,
	RLAFLAGS_Z							 = (1 << 0),
	RLAFLAGS_OBJECTBUFFER		 = (1 << 2),
	RLAFLAGS_UV							 = (1 << 3),
	RLAFLAGS_NORMAL					 = (1 << 4),
	RLAFLAGS_ORIGCOLOR			 = (1 << 5),
	RLAFLAGS_COVERAGE				 = (1 << 6),
	RLAFLAGS_OBJECTID				 = (1 << 8),
	RLAFLAGS_COLOR					 = (1 << 9),
	RLAFLAGS_TRANSPARENCY		 = (1 << 10),
	RLAFLAGS_SUBPIXEL_WEIGHT = (1 << 12),
	RLAFLAGS_SUBPIXEL_MASK	 = (1 << 13)
} ENUM_END_FLAGS(RLAFLAGS);

// savebits
enum SAVEBIT
{
	SAVEBIT_0									= 0,
	SAVEBIT_ALPHA							= (1 << 0),
	SAVEBIT_MULTILAYER				= (1 << 1),
	SAVEBIT_USESELECTEDLAYERS	= (1 << 2),
	SAVEBIT_16BITCHANNELS			= (1 << 3),
	SAVEBIT_GREYSCALE					= (1 << 4),
	SAVEBIT_INTERNALNET				= (1 << 5),	// private
	SAVEBIT_DONTMERGE					= (1 << 7),	// flag to avoid merging of layers in b3d files
	SAVEBIT_32BITCHANNELS			= (1 << 8),
	SAVEBIT_SAVERENDERRESULT	= (1 << 9),
	SAVEBIT_FIRSTALPHA_ONLY		= (1 << 10)	// private
} ENUM_END_FLAGS(SAVEBIT);

enum SCENEFILTER
{
	SCENEFILTER_0									= 0,
	SCENEFILTER_OBJECTS						= (1 << 0),
	SCENEFILTER_MATERIALS					= (1 << 1),
	SCENEFILTER_DIALOGSALLOWED		= (1 << 3),
	SCENEFILTER_PROGRESSALLOWED		= (1 << 4),
	SCENEFILTER_MERGESCENE				= (1 << 5),
	SCENEFILTER_NONEWMARKERS			= (1 << 6),
	SCENEFILTER_SAVECACHES				= (1 << 7),		// for cineware export only
	SCENEFILTER_NOUNDO						= (1 << 8),
	SCENEFILTER_SKIPOLDERTHANV13	=	(1 << 9),		// skip files older than v13
	SCENEFILTER_PREVIEWONLY				= (1 << 10),	// read until preview chunk, mutually exclusive
	SCENEFILTER_DOCUMENTPREFSONLY	= (1 << 11),	// read until doc prefs chunk, mutually exclusive
	SCENEFILTER_SKIPPREVIEWIMAGES	= (1 << 12)		// skip loading of (jpeg) preview images
} ENUM_END_FLAGS(SCENEFILTER);

enum SAVEDOCUMENTFLAGS
{
	SAVEDOCUMENTFLAGS_0										= 0,
	SAVEDOCUMENTFLAGS_DIALOGSALLOWED			= (1 << 0),
	SAVEDOCUMENTFLAGS_SAVEAS							= (1 << 1),
	SAVEDOCUMENTFLAGS_DONTADDTORECENTLIST	= (1 << 2),
	SAVEDOCUMENTFLAGS_AUTOSAVE						= (1 << 3),
	SAVEDOCUMENTFLAGS_SAVECACHES					= (1 << 4),
	SAVEDOCUMENTFLAGS_EXPORTDIALOG				= (1 << 5),
	SAVEDOCUMENTFLAGS_CRASHSITUATION			= (1 << 6),
	SAVEDOCUMENTFLAGS_NO_SHADERCACHE			= (1 << 7)
} ENUM_END_FLAGS(SAVEDOCUMENTFLAGS);

#define FILE_NODIALOG		0
#define FILE_DIALOG			1
#define FILE_IGNOREOPEN	2

// file format V6 chunk ids
#define CHUNK_UNITPREFS				110006
#define CHUNK_BASEDRAWPREFS		110008
#define CHUNK_DISPLAY					110009
#define CHUNK_TEXTURE					110010
#define CHUNK_BASESELECT			110011
#define CHUNK_BASEMATERIAL		110012
#define CHUNK_OUTWINDOWPREFS	110013
#define CHUNK_BROWSERPREFS		110014
#define CHUNK_DOCUMENTPREFS		110015
#define CHUNK_LAYOUT					110017
#define CHUNK_PAINTPREFS			110019
#define CHUNK_WORLDPREFS			110022
#define CHUNK_AUTHOR					110023
#define CHUNK_VERSION					110024
#define CHUNK_WORLDPREFS_EXT1 110025

#define CHUNK_BASELIST2D	 110050
#define CHUNK_BASELIST4D	 110051
#define CHUNK_BASEOBJECT	 110052
#define CHUNK_BASETRACK		 110054
#define CHUNK_BASESEQUENCE 110055
#define CHUNK_BASEKEY			 110056
#define CHUNK_BASEDIA			 110058
#define CHUNK_BASEDOCUMENT 110059
#define CHUNK_BASETAG			 110060

#define Yplugin						110061	// small listnode plugin
#define Zplugin						110062	// big listnode plugin
#define ID_LISTHEAD				110063
#define CHUNK_PLUGINLAYER 110064
#define SHplugin					110065
#define VPplugin					110066

#define CHUNK_NEWANIMATION 110067
#define CHUNK_UNDOABLELIST 110068
#define CHUNK_GELISTNODE	 110069
#define CHUNK_LAYER				 110313

#define CHUNK_ROOTLIST2D							 110100
#define CHUNK_ROOTLIST4D							 110101
#define CHUNK_ROOT_TLMARKER						 110103
#define CHUNK_ROOT_RDATA							 110106
#define CHUNK_ROOT_OBJECT							 110107
#define CHUNK_ROOT_TAG								 110108
#define CHUNK_ROOT_TRACK							 110109
#define CHUNK_ROOT_SEQUENCE						 110110
#define CHUNK_ROOT_KEY								 110111
#define CHUNK_ROOT_TKEY								 110112
#define CHUNK_ROOT_MATERIAL_EX				 110113
#define CHUNK_ROOT_LAST								 110114
#define CHUNK_ROOT_SMARKER						 110115
#define CHUNK_ROOT_DIA								 110116
#define CHUNK_BSP											 110117
#define CHUNK_HIDEP										 110118
#define CHUNK_BSV											 110119
#define CHUNK_HIDEV										 110120
#define CHUNK_ROOT_DOCUMENT						 110122
#define CHUNK_ROOT_VIEWPANEL					 110123
#define CHUNK_ROOT_BASEDRAW						 110124
#define CHUNK_ROOT_SCENEHOOK					 110125
#define CHUNK_ROOT_TIMELINE						 110126
#define CHUNK_ROOT_FCURVE							 110127
#define CHUNK_POLYGONOBJECT_EDGE			 110128
#define CHUNK_POLYGONOBJECT_EDGEHIDE	 110129
#define CHUNK_ROOT_POSTEFFECT					 110130
#define CHUNK_ROOT_SHADER							 110131
#define CHUNK_POLYGONOBJECT_BREAKPHONG 110132
#define CHUNK_POLYGONOBJECT_NGONEDGES	 110133
#define CHUNK_ROOT_LAYER_STRUCT				 110134
#define CHUNK_ROOT_LAYER_INFO					 110135

#define CHUNK_CTRACK								110414
#define CHUNK_CSEQ									110415
#define CHUNK_CKEY									110416
#define CHUNK_ROOT_CTRACK						110417
#define CHUNK_ROOT_CSEQ							110418
#define CHUNK_ROOT_CKEY							110419
#define CHUNK_ROOT_TAKEOVERRIDE			431000052
#define CHUNK_ROOT_SHADERREPOSITORY 431000139
#define CHUNK_ROOT_SHADERHIDDEN     310006009

#define CHUNK_FIELDLAYER								440000254
#define CHUNK_ROOT_FIELDMASK						440000345
#define CHUNK_CINEWARE_MATERIAL_CACHE		1054543

#define CHUNK_LASTLIST								 110300
#define CHUNK_PRIMITIVEOBJECT_EX			 110301
#define CHUNK_SPLINEPRIMITIVEOBJECT_EX 110302
#define CHUNK_RDATA										 110304
#define CHUNK_BASEDRAW								 110305
#define CHUNK_VIEWPANEL								 110306
#define CHUNK_MORPHTARGET							 110307
#define CHUNK_ROOT_MATERIAL						 110308
#define CHUNK_LISTHEAD								 110310
#define Rbase													 CHUNK_RDATA
//#define CHUNK_BASESOUND                 110311
#define CHUNK_WORLD 110312

// list elements
#define Tbaselist4d	CHUNK_BASELIST4D

// material/shader components
#define Millum	 1001063
#define Mpreview 1001065
#define Massign	 1001074

#define CHUNK_MATDATA		5704	// Materialdaten
#define CHUNK_MATTIMING 5705
#define CHUNK_MATPLUG		5706

/*
// timeline object
#define IDtimeline       1001004

// fcurve object
#define IDfcurve                1001005
*/
// DLayerStruct object
#define Olayer 100004801	//1001006
/*
// (virtual) filter base
#define Fbase          1001024

// Tracks
#define Sfade_EX        5201
#define Spos_EX         5202
#define Sscale_EX        5203
#define Srot_EX         5204
#define Saligntospline_EX    5205
#define Skinematic_EX      5206
#define Saligntopath_EX     5207
#define Starget_EX       5208
#define Stexture_EX       5209
#define Smorph         5210
#define Spulsate_EX       5211
#define Svibrate_EX       5212
#define Scoffeeplugin_EX    5213
#define Spla          5214
#define Smarker         5215
#define Sfolder_EX       5216
#define Ssound         5217
#define Stime_EX        5219
#define Sparameter_EX      5220
#define Splugin         5221
#define Sbase          5222 // for instanceof
#define Sxref          5223 // for instanceof
#define Svalue         5224
#define Sdata          5225
#define Sp2p          5226
#define Smomix         5227

#define TEfade_EX        5301
#define TEpos_EX        5302
#define TEscale_EX       5303
#define TErot_EX        5304
#define TEaligntospline_EX   5305
#define TEkinematic_EX     5306
#define TEaligntopath_EX    5307
#define TEtarget_EX       5308
#define TEtexture_EX      5309
#define TEmorph         5310
#define TEpulsate_EX      5311
#define TEvibrate_EX      5312
#define TEcoffeeplugin_EX    5313
#define TEpla          5314
#define TEmarker        5315
#define TEfolder_EX       5316
#define TEsound         5317
#define TEtime_EX        5319
#define TEparameter_EX     5320
#define TEplugin         5321
#define TEbase         5322 // for instanceof

#define TEvalue         5324
#define TEdata         5325
#define TEp2p          5326
#define TEmomix         5327
*/

/// @addtogroup CAnim
/// @ingroup group_enumeration
/// @{
#define CTbase 5350	///< Base track ID.
#define CSbase 5351	///< Base curve ID.
#define CKbase 5352	///< Base key ID.
/// @}

/*
#define SEfade_EX        5401
#define SEpos_EX        5402
#define SEscale_EX       5403
#define SErot_EX        5404
#define SEaligntospline_EX   5405
#define SEkinematic_EX     5406
#define SEaligntopath_EX    5407
#define SEtarget_EX       5408
#define SEtexture_EX      5409
#define SEmorph         5410
#define SEpulsate_EX      5411
#define SEvibrate_EX      5412
#define SEcoffeeplugin_EX    5413
#define SEpla          5414
#define SEmarker        5415
#define SEfolder_EX       5416
#define SEsound         5417
#define SEtime_EX        5419
#define SEparameter_EX     5420
#define SEplugin         5421
#define SEbase         5422 // for instanceof
#define SEvalue         5424
#define SEdata         5425
#define SEp2p          5426
#define SEmomix         5427

#define KEfade_EX        5501
#define KEpos_EX        5502
#define KEscale_EX       5503
#define KErot_EX        5504
#define KEaligntospline_EX   5505
#define KEkinematic_EX     5506
#define KEaligntopath_EX    5507
#define KEtarget_EX       5508
#define KEtexture_EX      5509
#define KEmorph         5510
#define KEpulsate_EX      5511
#define KEvibrate_EX      5512
#define KEcoffeeplugin_EX    5513
#define KEpla          5514
#define KEmarker        5515
#define KEfolder_EX       5516
#define KEsound         5517
#define KEtime_EX        5519
#define KEparameter_EX     5520
#define KEplugin         5521
#define KEbase         5522 // for instanceof
#define KEvalue         5524
#define KEdata         5525
#define KEp2p          5526
*/
/*
#define SHD_COLOR3D     8800
#define SHD_WOOD3D        8801
#define SHD_MARBLE3D      8804
#define SHD_EARTH3D       8805
#define SHD_VENUS3D       8806
#define SHD_METALL3D      8809
#define SHD_ROST3D        8810
*/
// Materialchannel
#define BASECHANNEL			5899
#define FARB_CHUNK			5900
#define LEUCHT_CHUNK		5901
#define TRANS_CHUNK			5902
#define SPIEG_CHUNK			5903
#define UMGEB_CHUNK			5904
#define NEBEL_CHUNK			5905
#define RELIEF_CHUNK		5906
#define GEN_CHUNK				5907
#define GLANZ_CHUNK			5908
#define GFARBE_CHUNK		5909
#define GLOW_CHUNK			5910
#define DIS_CHUNK				5911
#define DIFFUSION_CHUNK 5912
/*
// Identificationen der Objectformate
#define FORMAT_PREF    1000
#define FORMAT_CAT    1016
#define FORMAT_WAV    1018
#define FORMAT_FCV    1019
#define FORMAT_L4D    1020
#define FORMAT_P4D    1022

#define FORMAT_C4DIMPORT  1001025
#define FORMAT_C4DEXPORT  1001026
#define FORMAT_XMLIMPORT  1001027
#define FORMAT_XMLEXPORT  1001028
#define FORMAT_C4D4IMPORT  1001029
#define FORMAT_C4D5IMPORT  1001030
#define FORMAT_VRML1IMPORT 1001031
#define FORMAT_VRML1EXPORT 1001032
#define FORMAT_VRML2IMPORT 1001033
#define FORMAT_VRML2EXPORT 1001034
#define FORMAT_DXFIMPORT  1001035
#define FORMAT_DXFEXPORT  1001036
#define FORMAT_3DSIMPORT  1001037
#define FORMAT_3DSEXPORT  1001038
#define FORMAT_OBJIMPORT  1001039
#define FORMAT_OBJEXPORT  1001040
#define FORMAT_Q3DIMPORT  1001041
#define FORMAT_Q3DEXPORT  1001042
#define FORMAT_LWSIMPORT  1001043
#define FORMAT_LWOIMPORT  1001044
#define FORMAT_AIIMPORT   1001045
#define FORMAT_DEMIMPORT  1001046
#define FORMAT_D3DEXPORT  1001047
*/

// deprecated stuff

// all RDATA_xxx here are discontinued
#define RDATA_NAME_EX								5000
#define RDATA_RENDERMODE_EX					5001
#define RDATA_COMIC_EDGECOLOR_EX		5029
#define RDATA_COMIC_ILLUMINATION_EX	5030
#define RDATA_COMIC_OUTLINE_EX			5032
#define RDATA_EXTERNAL_EX						5042
#define RDATA_LENSEFFECTS_EX				5050
#define RDATA_GLOWEFFECTS_EX				5051
#define RDATA_SCENEMB_EX						5053
#define RDATA_SCENESTRENGTH_EX			5056
#define RDATA_COMIC_BACKCOLOR_EX		5059
#define RDATA_COMIC_EDGES_EX				5060
#define RDATA_BLURJITTER_EX					5062

#define RDATA_GI_ENABLEDIFFUSE_EX						5100	// BOOL
#define RDATA_GI_MAXDIFFUSEDEPTH_EX					5102	// LONG
#define RDATA_GI_MONTECARLOSAMPLES_EX				5104	// LONG
#define RDATA_GI_ACCURACY_EX								5105	// REAL
#define RDATA_GI_MAXRESOLUTION_EX						5106	// LONG
#define RDATA_GI_STRENGTH_EX								5108	// REAL
#define RDATA_GI_MINRESOLUTION_EX						5117	// LONG
#define RDATA_GI_RECOMPUTE_EX								5119	// LONG
#define RDATA_GI_SAVESOLUTION_EX						5120	// BOOL
#define RDATA_GI_PREPASS_EX									5123	// LONG
#define RDATA_GI_CAMERASOLUTION_EX					5124	// BOOL
#define RDATA_GI_STOCHASTICMODE_EX					5126	// BOOL
#define RDATA_GI_STOCHASTICDEPTH_EX					5127	// LONG
#define RDATA_GI_STOCHASTICSAMPLES_EX				5128	// LONG
#define RDATA_GI_STOCHASTICACCURACY_EX			5129	// REAL
#define RDATA_GI_LOCKTOSCREEN_EX						5130	// BOOL
#define RDATA_GI_MODE_EX										5131	// LONG
#define RDATA_GI_MODE_EX_STANDARD						0
#define RDATA_GI_MODE_EX_STOCHASTIC					1
#define RDATA_GI_MODE_EX_CAMERAANIMATION		2
#define RDATA_GI_MODE_EX_OBJECTANIMATION		3
#define RDATA_GI_SHOWDIFFUSEILLUMINATION_EX	5602	// BOOL

#define RDATA_CS_ENABLECAUSTICS_EX			 5111			// BOOL
#define RDATA_CS_ENABLEVOLUMECAUSTICS_EX 5112			// BOOL
#define RDATA_CS_STEPSIZE_EX						 5113			// REAL
#define RDATA_CS_SAMPLERADIUS_EX				 5114			// REAL
#define RDATA_CS_ACCURACY_EX						 5115			// LONG
#define RDATA_CS_STRENGTH_EX						 5116			// REAL
#define RDATA_CS_RECOMPUTE_EX						 5121			// LONG
#define RDATA_CS_SAVESOLUTION_EX				 5122			// BOOL
#define RDATA_CS_CAMERASOLUTION_EX			 5125			// BOOL

#define RDATA_MULTIPASS_CHANNELS_EX			5209			// BaseContainer
#define RDATA_MULTIPASS_ACTIVE_EX				1					// BOOL
#define RDATA_MULTIPASS_BLENDDATA_EX		4					// BaseContainer
#define RDATA_MULTIPASS_SPECIALGROUP_EX 5					// LONG

#define RDATA_POSTEFFECTS_EX				5402					// BaseContainer
#define RDATA_POSTEFFECTS_ACTIVE_EX 1							// BOOL
#define RDATA_AMBIENTOCCLUSION_EX		5500					// BaseContainer

#define RDATA_AO_APPLYTOSCENE_EX 5605							// BOOL

#define RDATA_TRANSPARENCY_EX	5004
#define RDATA_REFLECTION_EX		5005
#define RDATA_SHADOW_EX				5006

#define Oprimitive_EX	5141
#define Oboole_EX			5144

// VPobjectmotionblur (vpobjectmotionblur.h) (deprecated)
enum
{
	VP_OMBSTRENGTH = 1000
};

// Oconplane.h (parameter deprecated, rename with Oworkplane)
enum
{
	CONSTRUCTIONPLANE_TYPE		 = 1000,	// LONG
	CONSTRUCTIONPLANE_TYPE_XY	 = 0,
	CONSTRUCTIONPLANE_TYPE_ZY	 = 1,
	CONSTRUCTIONPLANE_TYPE_XZ	 = 2,
	CONSTRUCTIONPLANE_SPACING	 = 1001,	// REAL - Grid Spacing
	CONSTRUCTIONPLANE_SUB			 = 1002,	// LONG - Subdivision
	CONSTRUCTIONPLANE_ROUGHSUB = 1003		// LONG - Subdivision
};

// Tkinematic.h (deprecated)
enum
{
	KINEMATICTAG_WX			 = 1000,	// BOOL
	KINEMATICTAG_WY			 = 1001,	// BOOL
	KINEMATICTAG_WZ			 = 1002,	// BOOL
	KINEMATICTAG_MIN		 = 1003,	// VECTOR
	KINEMATICTAG_MAX		 = 1004,	// VECTOR
	KINEMATICTAG_DAMPING = 1005		// REAL
};

// Old WWW tag properties. Needed for loading old files. (Twww)
enum
{
	WWWTAG_URL	= 1000,	// String
	WWWTAG_INFO = 1001	// String
};

// Tfixexpression.h (deprecated)
enum
{
	FIXEXPRESSIONTAG_POINT = 1000	// VECTOR
};

// Tikexpression.h (deprecated)
enum
{
	IKEXPRESSIONTAG_LINK = 1001	// ObjectLink
};

//////////////////////////////////////////////////////////////////////////

enum
{
	LIGHT_DETAILS_FALLOFF_LINEAR_EX				 = 1,
	LIGHT_DETAILS_FALLOFF_INVERSE_EX			 = 2,
	LIGHT_DETAILS_FALLOFF_INVERSESQUARE_EX = 3,
	LIGHT_DETAILS_FALLOFF_INVERSECUBIC_EX	 = 4,
	LIGHT_DETAILS_NODIFFUSE_EX						 = 90016,	// BOOL
	LIGHT_DETAILS_NOSPECULAR_EX						 = 90017,	// BOOL
	LIGHT_DETAILS_BRIGHTNESS_EX						 = 90023,	// REAL
	LIGHT_DETAILS_INNERCOLOR_EX						 = 90026,	// VECTOR
	LIGHT_SHADOW_AREASAMPLES_EX						 = 50014,	// REAL
};

enum
{
	ACCEPT_DRAG_OBJECT_FORCE_COPY											 = 1073741824,
	ACTIVEOBJECTMANAGER_SETOBJECTS_NOMODESWITCH				 = 2,
	ACTIVEOBJECTMANAGER_SETOBJECTS_OPEN								 = 1,
	ACTIVEOBJECTMODE_																	 = 11,
	ACTIVEOBJECTMODE_BITMAPINFO												 = 8,
	ACTIVEOBJECTMODE_FCURVE														 = 7,
	ACTIVEOBJECTMODE_MATERIAL													 = 3,
	ACTIVEOBJECTMODE_NODE															 = 5,
	ACTIVEOBJECTMODE_NONE															 = 0,
	ACTIVEOBJECTMODE_OBJECT														 = 1,
	ACTIVEOBJECTMODE_SHADER														 = 4,
	ACTIVEOBJECTMODE_TAG															 = 2,
	ACTIVEOBJECTMODE_TIMELINE													 = 6,
	ACTIVEOBJECTMODE_TOOL															 = 9,
	ACTIVEOBJECTMODE_VIEW															 = 10,
	ACTIVE_OBJECT_CHANGED															 = 602,
	ALIGNTOPATH_ANGLE																	 = 1,
	ALIGNTOPATH_LOOKAHEAD															 = 1,
	ALIGNTOSPLINEKEY_NAME															 = 1,
	ALIGNTOSPLINEKEY_TANGENTIAL												 = 1,
	ALLOCTRACK_CREATESEQUENCE													 = 1,
	ALLOCTRACK_DOUNDO																	 = 2,
	ALLOCTRACK_EVENT																	 = 8,
	ALLOCTRACK_INSERT																	 = 4,
	ALLOCTRACK_STD																		 = 15,
	ALPHA_DELTA																				 = 1011,
	ALPHA_IMAGEALPHA																	 = 1013,
	ALPHA_INVERT																			 = 1014,
	ALPHA_SOFT																				 = 1012,
	ANGLE_DEG																					 = 0,
	ANGLE_RAD																					 = 1,
	ANIMATE_NOT_KEYFRAME															 = 16,
	ANIMATE_NOT_UPWARDS																 = 32,
	ANIMATE_NO_CHILDS																	 = 64,
	ANIMATE_NO_PARTICLES															 = 4,
	ANIMATE_QUICK																			 = 8,
	ANIMINFO_QUICK																		 = 1,
	ANTI_BEST																					 = 2,
	ANTI_GEOMETRY																			 = 1,
	ANTI_NONE																					 = 0,
	ANY_CHAR																					 = 2,
	ANY_DIGIT																					 = 3,
	AOM_MSG_GETATOMLIST																 = 1734702177,
	AOM_MSG_ISENABLED																	 = 1634690414,
	API_VERSION																				 = 9509,
	ARROW_DOWN																				 = 4,
	ARROW_LEFT																				 = 1,
	ARROW_RIGHT																				 = 2,
	ARROW_SMALL_DOWN																	 = 8,
	ARROW_SMALL_LEFT																	 = 5,
	ARROW_SMALL_RIGHT																	 = 6,
	ARROW_SMALL_UP																		 = 7,
	ARROW_UP																					 = 3,
	AUTOSCROLL_DOWN																		 = 2,
	AUTOSCROLL_LEFT																		 = 3,
	AUTOSCROLL_OWNX																		 = 5,
	AUTOSCROLL_OWNY																		 = 6,
	AUTOSCROLL_RIGHT																	 = 4,
	AUTOSCROLL_UP																			 = 1,
	AVISAVER_FCCHANDLER																 = 10001,
	AVISAVER_FCCTYPE																	 = 10000,
	AVISAVER_LDATARATE																 = 10003,
	AVISAVER_LKEY																			 = 10002,
	AVISAVER_LQ																				 = 10004,
	BACKGROUNDHANDLER_FLAGS_EDITORRENDDER							 = 2,
	BACKGROUNDHANDLER_FLAGS_MATERIALPREVIEW						 = 4,
	BACKGROUNDHANDLER_FLAGS_RENDEREXTERNAL						 = 8,
	BACKGROUNDHANDLER_FLAGS_VIEWREDRAW								 = 1,
	BACKGROUNDHANDLER_ISRUNNING												 = 100,
	BACKGROUNDHANDLER_PRIORITY_REDRAWANTS							 = -2000,
	BACKGROUNDHANDLER_PRIORITY_REDRAWVIEW							 = 4000,
	BACKGROUNDHANDLER_PRIORITY_RENDERACTIVEMATERIAL		 = 5000,
	BACKGROUNDHANDLER_PRIORITY_RENDEREXTERNAL					 = -1000,
	BACKGROUNDHANDLER_PRIORITY_RENDERINACTIVEMATERIALS = 3000,
	BACKGROUNDHANDLER_REMOVE													 = 103,
	BACKGROUNDHANDLER_START														 = 102,
	BACKGROUNDHANDLER_STOP														 = 101,
	BACKGROUNDHANDLER_TYPECLASS_C4D										 = 1000,
	BAKEPARTICLETAG_FROM															 = 1001,
	BAKEPARTICLETAG_IGNORE														 = 1004,
	BAKEPARTICLETAG_STEP															 = 1003,
	BAKEPARTICLETAG_TO																 = 1002,
	BAKE_RESIZE_NOTIFY																 = 19999,
	BAKE_STATE_COMPLETE																 = 10003,
	BAKE_STATE_FILL_IMAGE															 = 10002,
	BAKE_STATE_GI_PREPASS															 = 10001,
	BAKE_STATE_INITIALIZE															 = 10004,
	BAKE_STATE_PREPARE																 = 10000,
	BAKE_TEX_ALPHA																		 = 1015,
	BAKE_TEX_AMBIENT_OCCLUSION												 = 1009,
	BAKE_TEX_AO_SELFINTERSECTION											 = 2151,
	BAKE_TEX_AO_VERTEXMAPS														 = 2152,
	BAKE_TEX_AO_VERTEX_MAP														 = 2150,
	BAKE_TEX_AUTO_PIXEL_SIZE													 = 5004,
	BAKE_TEX_AUTO_SIZE																 = 5001,
	BAKE_TEX_AUTO_SIZE_MAX														 = 5003,
	BAKE_TEX_AUTO_SIZE_MIN														 = 5002,
	BAKE_TEX_BUMP																			 = 1018,
	BAKE_TEX_COLOR																		 = 1012,
	BAKE_TEX_COLOR_DIFFUSION													 = 2103,
	BAKE_TEX_COLOR_ILLUM															 = 2100,
	BAKE_TEX_COLOR_LUMINANCE													 = 2102,
	BAKE_TEX_COLOR_SHADOWS														 = 2101,
	BAKE_TEX_CONTINUE_UV															 = 1033,
	BAKE_TEX_DIFFUSION																 = 1013,
	BAKE_TEX_DUMMY																		 = 20000,
	BAKE_TEX_ERR_NO_DOC																 = 3000,
	BAKE_TEX_ERR_NO_MEM																 = 3001,
	BAKE_TEX_ERR_NO_OBJECT														 = 3004,
	BAKE_TEX_ERR_NO_OPTIMAL_MAPPING										 = 3009,
	BAKE_TEX_ERR_NO_RENDER_DOC												 = 3002,
	BAKE_TEX_ERR_NO_TEXTURE_TAG												 = 3003,
	BAKE_TEX_ERR_NO_UVW_TAG														 = 3005,
	BAKE_TEX_ERR_TEXTURE_MISSING											 = 3006,
	BAKE_TEX_ERR_USERBREAK														 = 3008,
	BAKE_TEX_ERR_WRONG_BITMAP													 = 3007,
	BAKE_TEX_FILL_COLOR																 = 1023,
	BAKE_TEX_HEIGHT																		 = 1021,
	BAKE_TEX_ILLUMINATION															 = 1016,
	BAKE_TEX_ILLUMINATION_SHADOWS											 = 2120,
	BAKE_TEX_LUMINANCE																 = 1014,
	BAKE_TEX_LUMINANCE_DIFFUSION											 = 2110,
	BAKE_TEX_NORMAL																		 = 1010,
	BAKE_TEX_NORMAL_FLIP_X														 = 2131,
	BAKE_TEX_NORMAL_FLIP_Y														 = 2132,
	BAKE_TEX_NORMAL_FLIP_Z														 = 2133,
	BAKE_TEX_NORMAL_METHOD														 = 2130,
	BAKE_TEX_NORMAL_METHOD_OBJECT											 = 1,
	BAKE_TEX_NORMAL_METHOD_TANGENT										 = 2,
	BAKE_TEX_NORMAL_METHOD_WORLD											 = 3,
	BAKE_TEX_NORMAL_SWAP_YZ														 = 2134,
	BAKE_TEX_NO_GI																		 = 1032,
	BAKE_TEX_NO_INIT_BITMAP														 = 5000,
	BAKE_TEX_OPTIMAL_MAPPING													 = 2200,
	BAKE_TEX_OPTIMAL_MAPPING_ANGLE										 = 2,
	BAKE_TEX_OPTIMAL_MAPPING_CUBIC										 = 1,
	BAKE_TEX_OPTIMAL_MAPPING_OFF											 = 0,
	BAKE_TEX_OPTIMAL_MAPPING_PREVIEW									 = 2201,
	BAKE_TEX_OPTIMAL_MAPPING_RELAXCOUNT								 = 2207,
	BAKE_TEX_PIXELBORDER															 = 1022,
	BAKE_TEX_PROGRESS_BITMAP													 = 2001,
	BAKE_TEX_REFLECTION																 = 1041,
	BAKE_TEX_SHADOWS																	 = 1017,
	BAKE_TEX_SHOW_STATUS															 = 2000,
	BAKE_TEX_STATUSBAR																 = 5005,
	BAKE_TEX_SUPERSAMPLING														 = 1030,
	BAKE_TEX_SURFACECOLOR															 = 1011,
	BAKE_TEX_SURFACE_ILLUMINATION											 = 2140,
	BAKE_TEX_TRANSPARENCY															 = 1019,
	BAKE_TEX_USE_BUMP																	 = 1031,
	BAKE_TEX_USE_CAMERA_VECTOR												 = 1006,
	BAKE_TEX_USE_POLYSELECTION												 = 1007,
	BAKE_TEX_UVMAP																		 = 1040,
	BAKE_TEX_UV_BOTTOM																 = 1027,
	BAKE_TEX_UV_LEFT																	 = 1024,
	BAKE_TEX_UV_RIGHT																	 = 1025,
	BAKE_TEX_UV_TOP																		 = 1026,
	BAKE_TEX_WIDTH																		 = 1020,
	BASEBITMAP_DATA_EXPOSURE													 = 1001,
	BASEBITMAP_DATA_GAMMA															 = 1000,
	BASEBITMAP_DATA_TARGETGAMMA												 = 1002,
	BASECHANNEL_BLUR_OFFSET														 = 1003,
	BASECHANNEL_BLUR_STRENGTH													 = 1004,
	BASECHANNEL_BRIGHTNESS_EX													 = 1001,
	BASECHANNEL_COLOR_EX															 = 1000,
	BASECHANNEL_INTERPOLATION													 = 1005,
	BASECHANNEL_MIXMODE_EX														 = 1006,
	BASECHANNEL_MIXSTRENGTH_EX												 = 1007,
	BASECHANNEL_SHADERID															 = 1016,
	BASECHANNEL_SUGGESTEDFOLDER												 = 1017,
	BASECHANNEL_TEXTURE																 = 1002,
	BASECHANNEL_TIME_END															 = 1014,
	BASECHANNEL_TIME_FPS															 = 1010,
	BASECHANNEL_TIME_FROM															 = 1008,
	BASECHANNEL_TIME_LOOPS														 = 1015,
	BASECHANNEL_TIME_MODE															 = 1011,
	BASECHANNEL_TIME_START														 = 1013,
	BASECHANNEL_TIME_TIMING														 = 1012,
	BASECHANNEL_TIME_TO																 = 1009,
	BDRAW_SETLIGHTLIST_NOLIGHTS												 = -1,
	BDRAW_SETLIGHTLIST_QUICKSHADING										 = 1,
	BDRAW_SETLIGHTLIST_SCENELIGHTS										 = 0,
	BFH_CENTER																				 = 0,
	BFH_FIT																						 = 24,
	BFH_LEFT																					 = 8,
	BFH_MASK																					 = 24,
	BFH_RIGHT																					 = 16,
	BFH_SCALE																					 = 32,
	BFH_SCALEFIT																			 = 56,
	BFM_ACTION																				 = 1648444244,
	BFM_ACTION_DP_ADDSELECT														 = 1633969267,
	BFM_ACTION_DP_ANIMCLICK														 = 1634625901,
	BFM_ACTION_DP_FOCUS																 = 1685087843,
	BFM_ACTION_DP_MENUCLICK														 = 1835363957,
	BFM_ACTION_DP_SUBSELECT														 = 1937072755,
	BFM_ACTION_ID																			 = 1835362660,
	BFM_ACTION_INDRAG																	 = 1835361394,
	BFM_ACTION_QUAL																		 = 1903518060,
	BFM_ACTION_STRCHG																	 = 1835365236,
	BFM_ACTION_VALCHG																	 = 1986226279,
	BFM_ACTION_VALUE																	 = 1835365985,
	BFM_ACTIVATE_WINDOW																 = 2002871156,
	BFM_ACTIVE																				 = 1633907830,
	BFM_ACTIVE_CHG																		 = 1648444231,
	BFM_ADJUSTSIZE																		 = 1715553354,
	BFM_ADJUSTSIZE_HEIGHT															 = 4,
	BFM_ADJUSTSIZE_LEFT																 = 1,
	BFM_ADJUSTSIZE_TOP																 = 2,
	BFM_ADJUSTSIZE_WIDTH															 = 3,
	BFM_ASKCLOSE																			 = 1634954083,
	BFM_ASK_TABSWITCH																	 = 1937006946,
	BFM_ASK_VISIBLE																		 = 1635150195,
	BFM_CALCSIZE																			 = 1648574803,
	BFM_CHECKCLOSE																		 = 1667787619,
	BFM_CHILD_ID																			 = 1,
	BFM_CHILD_REMOVED																	 = 1649567085,
	BFM_CLEARFOCUS																		 = 1648576067,
	BFM_CLOSEWINDOW																		 = 1668050803,
	BFM_CMD_DEST																			 = 1684370292,
	BFM_CMD_ENABLED																		 = 1073741824,
	BFM_CMD_FIRSTFOCUS																 = 1718575989,
	BFM_CMD_ID																				 = 1668113513,
	BFM_CMD_USED																			 = 536870912,
	BFM_CMD_VALUEMASK																	 = 536870911,
	BFM_COLORCHOOSER																	 = 1668246595,
	BFM_COLORCHOOSER_H_RANGE													 = 3,
	BFM_COLORCHOOSER_MIXINGPANEL											 = 7,
	BFM_COLORCHOOSER_PARENTMESSAGE										 = 1668246608,
	BFM_COLORCHOOSER_QUICKSTORE												 = 6,
	BFM_COLORCHOOSER_RGB_RANGE												 = 2,
	BFM_COLORCHOOSER_SV_RANGE													 = 4,
	BFM_COLORCHOOSER_SYSTEM														 = 1,
	BFM_COLORCHOOSER_SYSTEMMESSAGE										 = 5,
	BFM_COMMAND																				 = 1668115812,
	BFM_COMMANDNOTIFY																	 = 1668113518,
	BFM_COMMANDSCHANGED																 = 1668113256,
	BFM_CORE_ID																				 = 1298360649,
	BFM_CORE_MESSAGE																	 = 1298360653,
	BFM_CORE_PAR1																			 = 1298360625,
	BFM_CORE_PAR2																			 = 1298360626,
	BFM_CORE_SPECIALCOREID														 = 1935894884,
	BFM_CORE_UNIQUEID																	 = 1299540324,
	BFM_CORE_UPDATEACTIVECOMMANDS											 = 1970300001,
	BFM_CORE_UPDATECOMMANDS														 = 1970300003,
	BFM_CURSORINFO_REMOVE															 = 1667854957,
	BFM_DESTINATION_GROUP															 = 1734636404,
	BFM_DESTROY																				 = 1685288057,
	BFM_DISABLE																				 = 1648650611,
	BFM_DRAGAUTOSCROLL																 = 10005,
	BFM_DRAGEND																				 = 10004,
	BFM_DRAGRECEIVE																		 = 10003,
	BFM_DRAGSTART																			 = 10001,
	BFM_DRAG_DATA_																		 = 2,
	BFM_DRAG_DATA_NEW																	 = 9,
	BFM_DRAG_FINISHED																	 = 5,
	BFM_DRAG_LOST																			 = 7,
	BFM_DRAG_PRIVATE																	 = 6,
	BFM_DRAG_SCREENX																	 = 3,
	BFM_DRAG_SCREENY																	 = 4,
	BFM_DRAG_TYPE_NEW																	 = 8,
	BFM_DRAW																					 = 1648644673
};
enum
{
	BFM_DRAWUSERITEM									= 1970500196,
	BFM_DRAWUSERITEM_ID								= 1,
	BFM_DRAW_BOTTOM										= 4,
	BFM_DRAW_HASRECT									= 5,
	BFM_DRAW_LEFT											= 1,
	BFM_DRAW_OGL											= 6,
	BFM_DRAW_RIGHT										= 3,
	BFM_DRAW_TOP											= 2,
	BFM_DUMMY													= 0,
	BFM_ENABLE												= 1648717409,
	BFM_GETACTIVETABTITLE							= 1952539508,
	BFM_GETCURSORINFO									= 1667853926,
	BFM_GETCUSTOMGUILAYOUTDATA				= 1818327367,
	BFM_GETINFO												= 1648979558,
	BFM_GETVALUE											= 1733706060,
	BFM_GETVIEWPANELDATA							= 1987077236,
	BFM_GETVIEWPANEL_PRIVATEDATA			= 1987080292,
	BFM_GETVISIBLE_HEIGHT							= 10014,
	BFM_GETVISIBLE_WIDTH							= 10013,
	BFM_GETVISIBLE_XOFF								= 10011,
	BFM_GETVISIBLE_YOFF								= 10012,
	BFM_GOTFOCUS											= 1648838211,
	BFM_GUIPREFSCHANGED								= 1735418728,
	BFM_IDCHNG												= 1648968771,
	BFM_INIT													= 1648971337,
	BFM_INITVALUES										= 1649822030,
	BFM_INPUT													= 1648971854,
	BFM_INPUT_ASC											= 1801548643,
	BFM_INPUT_CHANNEL									= 1768973153,
	BFM_INPUT_DEVICE									= 1768973430,
	BFM_INPUT_DOUBLECLICK							= 1768973410,
	BFM_INPUT_FINGERWHEEL							= 1768320615,
	BFM_INPUT_KEYBOARD								= 1801812322,
	BFM_INPUT_MOUSE										= 1836021107,
	BFM_INPUT_MOUSELEFT								= 1,
	BFM_INPUT_MOUSEMIDDLE							= 3,
	BFM_INPUT_MOUSEMOVE								= 101,
	BFM_INPUT_MOUSERIGHT							= 2,
	BFM_INPUT_MOUSEWHEEL							= 100,
	BFM_INPUT_MOUSEX1									= 5,
	BFM_INPUT_MOUSEX2									= 6,
	BFM_INPUT_QUALIFIER								= 1768976737,
	BFM_INPUT_ROTATION								= 1769107316,
	BFM_INPUT_TILT										= 1769237620,
	BFM_INPUT_VALUE										= 1768978017,
	BFM_INPUT_VALUE_REAL							= 1768977985,
	BFM_INPUT_X												= 1768978040,
	BFM_INPUT_Y												= 1768978041,
	BFM_INPUT_Z												= 1768978042,
	BFM_INTERACTEND										= 1768846437,
	BFM_INTERACTSTART									= 1768846433,
	BFM_ISACTIVE											= 1767990132,
	BFM_LAYOUT_CHANGED								= 1716273497,
	BFM_LAYOUT_GETDATA								= 1818327399,
	BFM_LAYOUT_SETDATA								= 1818327411,
	BFM_LOSTFOCUS											= 1649165891,
	BFM_MANAGER_ID										= 2,
	BFM_MARKFOCUS											= 1649231427,
	BFM_MARKFORCELAYOUT								= 1835755116,
	BFM_MAXCHNG												= 1649230147,
	BFM_MENUFINDER										= 1718185572,
	BFM_MENU_CHECK										= 4,
	BFM_MENU_OFF											= 2,
	BFM_MENU_ON												= 1,
	BFM_MENU_SET											= 10000,
	BFM_MENU_UNCHECK									= 8,
	BFM_MINCHNG												= 1649232195,
	BFM_MOVEFOCUSNEXT									= 1648774744,
	BFM_MOVEFOCUSPREV									= 1648775250,
	BFM_OPTIMIZE											= 1869640809,
	BFM_PARENT_TITLECHNG							= 1886545262,
	BFM_POPUPNOTIFY										= 1886351470,
	BFM_REDRAW_EDITMODE								= 1685546340,
	BFM_RELOAD_MENUS									= 1818520942,
	BFM_REMOVE_DIALOG									= 1649558861,
	BFM_RENAMEWINDOW									= 2003985774,
	BFM_SCROLLAREA										= 10010,
	BFM_SCROLLGROUP_SCROLLED					= 1935897203,
	BFM_SCROLLX												= 1,
	BFM_SCROLLY												= 2,
	BFM_SETACTIVE											= 1649623363,
	BFM_SETACTIVE_DATA								= 1649623363,
	BFM_SETFIRSTFOCUS									= 1649624646,
	BFM_SETFOCUS											= 1649624643,
	BFM_SETLASTFOCUS									= 1649626182,
	BFM_SETMAINTITLE									= 1836345716,
	BFM_SETSTATUSBAR									= 1937006964,
	BFM_SETVIEWPANELLAYOUT						= 1987081068,
	BFM_SETVIEWPORTORIGIN							= 1666142791,
	BFM_SETVIEWPORTORIGIN_X						= 1,
	BFM_SETVIEWPORTORIGIN_Y						= 2,
	BFM_SETVIEWPORTSIZE								= 1666402650,
	BFM_SET_MSG_BITMASK								= 1651078253,
	BFM_SHOW_AREA											= 1935766117,
	BFM_SHOW_FRAME										= 1933996653,
	BFM_SHOW_ID												= 1,
	BFM_SIZED													= 1648972617,
	BFM_SPECIALACTIVATE								= 1935762292,
	BFM_SPECIALENABLE									= 1936745838,
	BFM_SPECIALGETLONG								= 1735683175,
	BFM_SPECIALGETREAL								= 1735550316,
	BFM_SPECIALGETSTRING							= 1735684980,
	BFM_SPECIALGETVECTOR							= 1735812451,
	BFM_SPECIALLONG										= 1936747623,
	BFM_SPECIALSETCONTAINER						= 1935896180,
	BFM_SPECIALSETRANGE								= 1936879207,
	BFM_SPECIALSETREAL								= 1936876908,
	BFM_SPECIALSTRING									= 1936749428,
	BFM_SPECIALVECTOR									= 1937139043,
	BFM_STATUSBAR_HELP								= 5,
	BFM_STATUSBAR_PROGRESS						= 3,
	BFM_STATUSBAR_PROGRESSON					= 1,
	BFM_STATUSBAR_PROGRESSSPIN				= 4,
	BFM_STATUSBAR_TXT									= 2,
	BFM_STORE_WEIGHTS									= 1937012583,
	BFM_SYNC_MESSAGE									= 1937337955,
	BFM_TESTONLY											= 1952805748,
	BFM_TIMER_MESSAGE									= 10020,
	BFM_TITLECHNG											= 1649690947,
	BFM_UPDATE_REGION									= 1970303591,
	BFM_VALUEADD											= 1649819972,
	BFM_VALUECHNG											= 1649819971,
	BFM_VALUESUB											= 1649824578,
	BFM_VISIBLE_OFF										= 1986622278,
	BFM_VISIBLE_ON										= 1986622292,
	BFM_VPD_LAYOUTTYPE								= 1987079284,
	BFM_VPD_MAXIMIZED									= 1987079544,
	BFM_VPD_PANELID										= 1987078500,
	BFM_VPD_PRIVATEDATA								= 1987077236,
	BFM_WEIGHTS_CHANGED								= 2000906343,
	BFV_															= 65,
	BFV_BORDERGROUP_CHECKBOX					= 4096,
	BFV_BORDERGROUP_FOLD2							= 32768,
	BFV_BORDERGROUP_FOLD							= 8192,
	BFV_BORDERGROUP_FOLD_OPEN					= 16384,
	BFV_BOTTOM												= 2,
	BFV_CENTER												= 0,
	BFV_CMD_EQUALCOLUMNS							= 2048,
	BFV_DIALOG_BAR_VERT								= 8,
	BFV_DIALOG_NOBUTTONS							= 16,
	BFV_DIALOG_REMOVEABLE							= 4,
	BFV_FIT														= 3,
	BFV_GRIDGROUP_ALLOW_WEIGHTS				= 65536,
	BFV_GRIDGROUP_EQUALCOLS						= 1,
	BFV_GRIDGROUP_EQUALROWS						= 2,
	BFV_IGNORE_FOCUS									= 1024,
	BFV_LAYOUTGROUP_NODROP2						= 8192,
	BFV_LAYOUTGROUP_NODROP						= 4096,
	BFV_LAYOUTGROUP_PALETTEOUTLINES		= 512,
	BFV_MASK													= 3,
	BFV_SCALE													= 4,
	BFV_SCALEFIT											= 7,
	BFV_TABGROUP_RELOADDIALOG					= 2048,
	BFV_TOP														= 1,
	BF_INSERT_LAST										= 2147483647,
	BF_PREFS_DOCUMENT_PREVIEW					= 7,
	BF_PREFS_DOCUMENT_PREVIEW_DFLT		= 1,
	BF_PREFS_DST_PREVIEW							= 1,
	BF_PREFS_DST_PREVIEW_DFLT					= 1,
	BF_PREFS_GLOBAL										= 1000001,
	BF_PREFS_NO_PS_APP_PATHS					= 1010,
	BF_PREFS_NO_PS_APP_PATHS_DFLT			= 0,
	BF_PREFS_NO_PS_PATHS							= 9,
	BF_PREFS_NO_PS_PATHS_DFLT					= 0,
	BF_PREFS_PREVIEW_HEIGHT						= 3,
	BF_PREFS_PREVIEW_HEIGHT_DFLT			= 192,
	BF_PREFS_PREVIEW_PERCENT					= 4,
	BF_PREFS_PREVIEW_PERCENT_DFLT			= 4,
	BF_PREFS_PREVIEW_REALTIME					= 5,
	BF_PREFS_PREVIEW_REALTIME_DFLT		= 0,
	BF_PREFS_PREVIEW_WIDTH						= 2,
	BF_PREFS_PREVIEW_WIDTH_DFLT				= 256,
	BF_PREFS_PSF_ENABLED							= 8,
	BF_PREFS_PSF_ENABLED_DFLT					= 0,
	BF_PREFS_PS_APP_PATH0							= 1011,
	BF_PREFS_PS_APP_PATH_XXX					= 1110,
	BF_PREFS_PS_APP_VERSION0					= 1111,
	BF_PREFS_PS_APP_VERSION_XXX				= 1110,
	BF_PREFS_PS_PATH0									= 10,
	BF_PREFS_PS_PATH_XXX							= 1009,
	BF_PREFS_SRC_PREVIEW							= 0,
	BF_PREFS_SRC_PREVIEW_DFLT					= 0,
	BF_PREFS_TEST											= 1000000,
	BF_PREFS_TILE_FLAGS								= 6,
	BF_PREFS_TILE_FLAGS_DFLT					= 0,
	BFx_NOEQUAL												= 64,
	BITDEPTH_FLOAT										= 2,
	BITDEPTH_SHIFT										= 4,
	BITDEPTH_UCHAR										= 0,
	BITDEPTH_UWORD										= 1,
	BITMAPBUTTON_BORDER								= 1651663986,
	BITMAPBUTTON_BUTTON								= 1651864692,
	BITMAPBUTTON_DRAWPOPUPBUTTON			= 1886351477,
	BITMAPBUTTON_IGNORE_BITMAP_HEIGTH = 1768386152,
	BITMAPBUTTON_IGNORE_BITMAP_WIDTH	= 1768386167,
	BITMAPBUTTON_TOGGLE								= 1953458023,
	BITMAPBUTTON_TOOLTIP							= 1953786224,
	BITMAPFILTER_MAGIC_V2							= 1651339366,
	BITMAPFILTER_MAGIC_V3							= 1651336755,
	BITMAPLOADER_FREE									= 3,
	BITMAPLOADER_INIT									= 1,
	BITMAPLOADER_LOAD									= 2,
	BITMASK_CORE_MESSAGE							= 1,
	BITMASK_SYNC_MESSAGE							= 2,
	BIT_AAMAN													= 16,
	BIT_AAMAN_LX											= 1024,
	BIT_AAMAN_LY											= 1024,
	BIT_AAMAN_LZ											= 1024,
	BIT_AAMAN_PX											= 512,
	BIT_AAMAN_PY											= 512,
	BIT_AAMAN_PZ											= 512,
	BIT_AAMAN_RX											= 2048,
	BIT_AAMAN_RY											= 2048,
	BIT_AAMAN_RZ											= 2048,
	BIT_TFOLD													= 32768,
	BLT_MEM_ERR												= -1,
	BLT_OK														= 0,
	BLT_PX_FORMAT_ERR									= -2,
	BMFI_SUPPORT_16BIT								= 2,
	BMFI_SUPPORT_32BIT								= 4,
	BMFI_SUPPORT_8BIT									= 1,
	BMFI_SUPPORT_NO_GREY							= 8,
	BMFI_SUPPORT_NO_RGB								= 16,
	BMP_ALLOWALPHA										= 256,
	BMP_DARKEN												= 2,
	BMP_EMBOSSED											= 3,
	BMP_NORMAL												= 0,
	BMP_NORMALSCALED									= 1,
	BOOL_PAGEMODE											= 1651535204,
	BORDER_ACTIVE_1										= 10,
	BORDER_ACTIVE_2										= 11,
	BORDER_ACTIVE_3										= 13,
	BORDER_ACTIVE_4										= 14,
	BORDER_BLACK											= 9,
	BORDER_GROUP_IN										= 5,
	BORDER_GROUP_OUT									= 6,
	BORDER_GROUP_TOP									= 12,
	BORDER_IN													= 3,
	BORDER_MASK												= 2147483647,
	BORDER_NONE												= 0,
	BORDER_OUT2												= 7,
	BORDER_OUT3												= 8,
	BORDER_OUT												= 4,
	BORDER_THIN_IN										= 1
};
enum
{
	BORDER_THIN_OUT														= 2,
	BORDER_WITH_TITLE													= -2147483647,
	BRANCHINFO_ANIMATE												= 1,
	BRANCHINFO_HEADISATOM											= 2,
	BROWSER_LIB_ID														= 1017645,
	BUMP_MIPFALLOFF														= 1010,
	BUMP_STRENGTH															= 1009,
	BasicItemType															= 1,
	Between																		= 4,
	BetweenOrEqual														= 5,
	BetweenOrEqualX														= 6,
	BetweenOrEqualY														= 7,
	BfBLACK																		= 0,
	BfBLUE																		= 10,
	BfBROWN																		= 19,
	BfDBLUE																		= 15,
	BfDGRAY																		= 12,
	BfDGREEN																	= 14,
	BfDRED																		= 13,
	BfDWHITE																	= 17,
	BfDYELLOW																	= 16,
	BfGRAY																		= 7,
	BfGREEN																		= 9,
	BfLTBLUE																	= 5,
	BfLTGRAY																	= 2,
	BfLTGREEN																	= 4,
	BfLTRED																		= 3,
	BfLTYELLOW																= 6,
	BfORANGE																	= 18,
	BfPURPLE																	= 20,
	BfRED																			= 8,
	BfWHITE																		= 1,
	BfYELLOW																	= 11,
	C4DGUIPTR																	= -1,
	C4DMSG_PRIORITY														= 0,
	C4DPL_BITMAPFILTER												= 12,
	C4DPL_BITMAPLOADER												= 22,
	C4DPL_BITMAPSAVER													= 23,
	C4DPL_BUILDMENU														= 1001188,
	C4DPL_COFFEEMESSAGE												= 4,
	C4DPL_COMMAND															= 7,
	C4DPL_COMMANDLINEARGS											= 1002,
	C4DPL_COREMESSAGE													= 26,
	C4DPL_CUSTOMDATATYPE											= 28,
	C4DPL_CUSTOMGUI														= 27,
	C4DPL_DEVICECHANGE												= 1001190,
	C4DPL_EDITIMAGE														= 1003,
	C4DPL_END																	= 3,
	C4DPL_ENDACTIVITY													= 1001,
	C4DPL_ENDPROGRAM													= 1001084,
	C4DPL_ERROR																= -1,
	C4DPL_ERROR_MEM														= -5,
	C4DPL_ERROR_OS														= -4,
	C4DPL_ERROR_TYPE													= -2,
	C4DPL_ERROR_VERSION												= -3,
	C4DPL_INIT																= 2,
	C4DPL_INIT_PRIORITY_ADVANCEDRENDER				= 17000,
	C4DPL_INIT_PRIORITY_MODELING							= 18500,
	C4DPL_INIT_PRIORITY_MODULES								= 10000,
	C4DPL_INIT_PRIORITY_OBJECTS								= 19000,
	C4DPL_INIT_PRIORITY_PLUGINS								= 0,
	C4DPL_INIT_PRIORITY_SHADER								= 18000,
	C4DPL_INIT_PRIORITY_SLA										= 850,
	C4DPL_INIT_PRIORITY_XTENSIONS							= 20000,
	C4DPL_INIT_SYS														= 1,
	C4DPL_INIT_VERSION												= 4,
	C4DPL_KEY																	= 19,
	C4DPL_LIBRARY															= 18,
	C4DPL_MATERIAL														= 3,
	C4DPL_MESSAGE															= 6,
	C4DPL_NODE																= 16,
	C4DPL_OBJECT															= 8,
	C4DPL_RESOURCEDATATYPE										= 29,
	C4DPL_SCENEHOOK														= 15,
	C4DPL_SCENELOADER													= 24,
	C4DPL_SCENESAVER													= 25,
	C4DPL_SEQUENCE														= 20,
	C4DPL_SHADER															= 2,
	C4DPL_STARTACTIVITY												= 1000,
	C4DPL_TAG																	= 10,
	C4DPL_TOOL																= 14,
	C4DPL_TRACK																= 21,
	C4DPL_VERSION															= 3,
	C4DPL_VIDEOPOST														= 13,
	C4D_MEMORY_STAT_MEMORY_INUSE							= 1,
	C4D_MEMORY_STAT_MEMORY_PEAK								= 2,
	C4D_MEMORY_STAT_NO_OF_ALLOCATIONS_CURRENT = 4,
	C4D_MEMORY_STAT_NO_OF_ALLOCATIONS_TOTAL		= 3,
	C4D_PATH_LIBRARY													= 3,
	C4D_PATH_LIBRARY_USER											= 4,
	C4D_PATH_PREFS														= 1,
	C4D_PATH_RESOURCE													= 2,
	C4D_RADIOSITY_ID													= 1000969,
	CAMERAOBJECT_DEPTHBACK										= 1005,
	CAMERAOBJECT_DEPTHFRONT										= 1003,
	CAMERAOBJECT_DEPTHMIDDLE									= 1010,
	CAMERAOBJECT_DEPTHTYPE										= 1002,
	CAMERA_AXONOMETRIC												= 2,
	CAMERA_CUSTOMLENS													= 4,
	CAMERA_PARALLEL														= 1,
	CAMERA_PERSPECTIVE												= 0,
	CAMERA_VRPANORAMA													= 3,
	CBPluginTypeCatalog												= 1018025,
	CBPluginTypeCatalogFile										= 1018761,
	CBPluginTypeCategory											= 1018024,
	CBPluginTypeDefaults											= 1018030,
	CBPluginTypeDummy													= 1018029,
	CBPluginTypeFilePreset										= 1018031,
	CBPluginTypeFolder												= 1018027,
	CBPluginTypeImage													= 1018021,
	CBPluginTypeMaterial											= 1018028,
	CBPluginTypeMaterialPreset								= 1018108,
	CBPluginTypeMovie													= 1018022,
	CBPluginTypeObjectPreset									= 1018107,
	CBPluginTypePresetLibrary									= 1018760,
	CBPluginTypeRenderDataPreset							= 1018110,
	CBPluginTypeScene													= 1018023,
	CBPluginTypeScriptFile										= 1018762,
	CBPluginTypeShaderPreset									= 1018111,
	CBPluginTypeTagPreset											= 1018109,
	CBPluginTypeUnknown												= 1018026,
	CBPluginTypeVideoPostPreset								= 1018112,
	CBPluginTypeXPressoPreset									= 1018113,

	CHECKISRUNNING_ANIMATIONRUNNING						= 0,
	CHECKISRUNNING_EDITORRENDERING						= 2,
	CHECKISRUNNING_EXTERNALRENDERING					= 3,
	CHECKISRUNNING_MATERIALPREVIEWRUNNING			= 5,
	CHECKISRUNNING_PAINTERUPDATING						= 4,
	CHECKISRUNNING_VIEWDRAWING								= 1,
	CH_BLUR_OFFSET														= 1003,
	CH_BLUR_STRENGTH													= 1004,
	CH_BRIGHTNESS															= 1001,
	CH_COLOR																	= 1000,
	CH_INTERPOLATION													= 1005,
	CH_MIXMODE																= 1006,
	CH_MIXSTRENGTH														= 1007,
	CH_TEXTURE																= 1002,
	CH_TIME_END																= 1014,
	CH_TIME_FPS																= 1010,
	CH_TIME_FROM															= 1008,
	CH_TIME_LOOPS															= 1015,
	CH_TIME_MODE															= 1011,
	CH_TIME_START															= 1013,
	CH_TIME_TIMING														= 1012,
	CH_TIME_TO																= 1009,
	CL_NO_BITS																= 16,
	CL_NO_HIERARCHY														= 4,
	CL_NO_TRACKS															= 8,
	CL_ONLY_VISIBLETAGS												= 0,
	CMD_																			= 1073741825,
	CMD_ARROW																	= 64,
	CMD_BUTTONLIKE														= 256,
	CMD_CYCLE																	= 1024,
	CMD_EDITPALETTE														= 4096,
	CMD_ENABLED																= 1073741824,
	CMD_MENU																	= 512,
	CMD_PIC																		= 4,
	CMD_POPUP_BELOW														= 2,
	CMD_POPUP_RIGHT														= 1,
	CMD_SHORTCUT															= 32,
	CMD_SMALLICONS														= 8192,
	CMD_TEXT																	= 16,
	CMD_TOGGLE																= 8,
	CMD_VALUE																	= 1073741823,
	CMD_VERT																	= 128,
	COFFEEEDITOR_COMPILE											= 1668246896,
	COFFEEEDITOR_EXECUTE											= 1702389091,
	COFFEEEDITOR_GETERROR_LINE								= 1919251308,
	COFFEEEDITOR_GETERROR_POS									= 1919251312,
	COFFEEEDITOR_GETERROR_RES									= 1919251314,
	COFFEEEDITOR_GETERROR_STRING							= 1919251315,
	COFFEEEDITOR_GETSTRING										= 1734702195,
	COFFEEEDITOR_SETSTRING										= 1936028787,
	COFFEEERROR_ARGV													= 3,
	COFFEEERROR_ARGW													= 4,
	COFFEEERROR_BBLOCK												= 1,
	COFFEEERROR_BMETHOD												= 8,
	COFFEEERROR_BOLOOP												= 106,
	COFFEEERROR_BOPCODE												= 6,
	COFFEEERROR_BPARENT												= 113,
	COFFEEERROR_BSUPER												= 115,
	COFFEEERROR_BTYP													= 5,
	COFFEEERROR_CLASS													= 201,
	COFFEEERROR_CLASSB												= 200,
	COFFEEERROR_COLOOP												= 107,
	COFFEEERROR_DIV0													= 15,
	COFFEEERROR_ECLOSE												= 121,
	COFFEEERROR_ECNAME												= 104,
	COFFEEERROR_EDEC													= 100,
	COFFEEERROR_EFUNC													= 102,
	COFFEEERROR_ELVAL													= 111,
	COFFEEERROR_EMDEC													= 101,
	COFFEEERROR_EPRIM													= 119,
	COFFEEERROR_EVAR													= 118,
	COFFEEERROR_EXEPT													= 16,
	COFFEEERROR_EXPECT												= 204,
	COFFEEERROR_FUNC													= 202,
	COFFEEERROR_INCLUDE												= 125,
	COFFEEERROR_INVALIDID											= 17,
	COFFEEERROR_IRDEF													= 103,
	COFFEEERROR_ISPACE												= 120,
	COFFEEERROR_MEM														= 2,
	COFFEEERROR_MEMBERF												= 203,
	COFFEEERROR_MPRIVATE											= 12,
	COFFEEERROR_MPROTECTED										= 11,
	COFFEEERROR_NCASE													= 109,
	COFFEEERROR_NCLASSM												= 112,
	COFFEEERROR_NMETHOD												= 9,
	COFFEEERROR_NPARENT												= 114,
	COFFEEERROR_NPROC													= 7,
	COFFEEERROR_NPUBLIC												= 10,
	COFFEEERROR_NSUPER												= 116,
	COFFEEERROR_NVAR													= 110,
	COFFEEERROR_PARAMACCESS										= 18,
	COFFEEERROR_PARAMTYPE											= 20,
	COFFEEERROR_PARAMVALUE										= 19,
	COFFEEERROR_PARGV													= 122,
	COFFEEERROR_PARGW													= 123,
	COFFEEERROR_PROTECT												= 117
};
enum
{
	COFFEEERROR_SOLOOP									= 108,
	COFFEEERROR_STACKOV									= 14,
	COFFEEERROR_SUBSCR									= 13,
	COFFEEERROR_SYNTAX									= 124,
	COFFEEERROR_TMLOOP									= 105,
	COFFEEERROR_VARONLY									= 126,
	COFFEEEXPRESSIONTAG_PASS						= 1500,
	COFFEEEXPRESSIONTAG_TEXT						= 1000,
	COFFEESCRIPT_ADDEVENT								= 1003,
	COFFEESCRIPT_CONTAINER							= 65536,
	COFFEESCRIPT_SCRIPTENABLE						= 1006,
	COFFEESCRIPT_SCRIPTHELP							= 2,
	COFFEESCRIPT_SCRIPTNAME							= 1,
	COFFEESCRIPT_SHOWINMENU							= 1002,
	COFFEESCRIPT_TEXT										= 1000,
	COLORSYSTEM_HSV											= 20,
	COLORSYSTEM_HSVTAB									= 22,
	COLORSYSTEM_HSV_COLOR								= 21,
	COLORSYSTEM_RANGE_255								= 1,
	COLORSYSTEM_RANGE_65535							= 2,
	COLORSYSTEM_RANGE_DEGREE						= 3,
	COLORSYSTEM_RANGE_PERCENT						= 0,
	COLORSYSTEM_RGB											= 10,
	COLORSYSTEM_RGB_COLOR								= 11,
	COLORSYSTEM_TABLE										= 30,
	COLOR_3DTEXT												= 997,
	COLOR_ACTIVEBOX											= 19,
	COLOR_ACTIVEPOINT										= 12,
	COLOR_ACTIVEPOLYGON									= 15,
	COLOR_ANIMPATH											= 20,
	COLOR_B															= 1002,
	COLOR_BACKFACING										= 51,
	COLOR_BG														= 1000,
	COLOR_BGEDIT												= 1001,
	COLOR_BGFOCUS												= 1002,
	COLOR_BGGADGET											= 1013,
	COLOR_BGSELECTEDTEXT								= 1014,
	COLOR_BGTRISTATE										= 1017,
	COLOR_BG_DARK1											= 1100,
	COLOR_BG_DARK2											= 1101,
	COLOR_BG_MM_E_ACTIVE_LAYER					= 1191,
	COLOR_BG_MM_E_ACTIVE_TEXTURE				= 1185,
	COLOR_BG_MM_E_INACTIVE_LAYER				= 1193,
	COLOR_BG_MM_E_INACTIVE_TEXTURE			= 1187,
	COLOR_BG_MM_E_SELECTED_LAYER				= 1189,
	COLOR_BG_TEXTUREVIEW								= 1197,
	COLOR_BONE													= 10,
	COLOR_BROWSER_LINES_SELECTED				= 1120,
	COLOR_C4DBACKGROUND									= 0,
	COLOR_CAMERA												= 7,
	COLOR_CONSOLE_BG										= 1148,
	COLOR_CONSOLE_TEXT									= 1149,
	COLOR_CONTENTBROWSER_BG_TEXT_ACT		= 1216,
	COLOR_CONTENTBROWSER_BG_TEXT_INACT	= 1217,
	COLOR_CONTENTBROWSER_FG_TEXT_ACT		= 1218,
	COLOR_CONTENTBROWSER_FG_TEXT_INACT	= 1219,
	COLOR_CONTENTBROWSER_NOTAVAILABLE		= 1220,
	COLOR_CONTENTBROWSER_TEXT						= 1215,
	COLOR_CTIMELINE_COLOR								= 1208,
	COLOR_CTIMELINE_CURRENTFRAME				= 1212,
	COLOR_CTIMELINE_GENERAL							= 1209,
	COLOR_CTIMELINE_GRID								= 1203,
	COLOR_CTIMELINE_MASTER							= 1210,
	COLOR_CTIMELINE_MASTERIDENTICAL			= 1211,
	COLOR_CTIMELINE_PLA									= 1207,
	COLOR_CTIMELINE_POSITION						= 1204,
	COLOR_CTIMELINE_PREVIEW							= 1213,
	COLOR_CTIMELINE_ROTATION						= 1205,
	COLOR_CTIMELINE_SCALE								= 1206,
	COLOR_DBARBG1												= 1010,
	COLOR_DBARBG2												= 1012,
	COLOR_DBARBG3												= 1214,
	COLOR_DBARFG1												= 1009,
	COLOR_DBARFG2												= 1011,
	COLOR_DUMMY_												= 1221,
	COLOR_EDGEBL												= 1008,
	COLOR_EDGEDK												= 1007,
	COLOR_EDGELT												= 1005,
	COLOR_EDGEWH												= 1006,
	COLOR_EMBOSS_DK											= 1103,
	COLOR_EMBOSS_WH											= 1102,
	COLOR_FCURVE_BG											= 1121,
	COLOR_FCURVE_GRAY1									= 1127,
	COLOR_FCURVE_GRAY2									= 1128,
	COLOR_FCURVE_GRAY3									= 1129,
	COLOR_FCURVE_LINES									= 1123,
	COLOR_FCURVE_LLTGRAY								= 1126,
	COLOR_FCURVE_MAINGRID								= 1124,
	COLOR_FCURVE_POINTSELECTED					= 1130,
	COLOR_FCURVE_SUBGRID								= 1125,
	COLOR_FCURVE_TEXT										= 1122,
	COLOR_FGSELECTEDTEXT								= 1015,
	COLOR_FG_MM_E_ACTIVE_LAYER					= 1192,
	COLOR_FG_MM_E_ACTIVE_TEXTURE				= 1186,
	COLOR_FG_MM_E_INACTIVE_LAYER				= 1194,
	COLOR_FG_MM_E_INACTIVE_TEXTURE			= 1188,
	COLOR_FG_MM_E_SELECTED_LAYER				= 1190,
	COLOR_FILMFORMAT										= 1,
	COLOR_FRONTFACING										= 50,
	COLOR_G															= 1001,
	COLOR_GRID_MAJOR										= 3,
	COLOR_GRID_MINOR										= 4,
	COLOR_HORIZON												= 2,
	COLOR_INACTIVEPOINT									= 13,
	COLOR_INACTIVEPOLYGON								= 16,
	COLOR_LAYER0												= 28,
	COLOR_LAYER1												= 29,
	COLOR_LAYER2												= 30,
	COLOR_LAYER3												= 31,
	COLOR_LAYER4												= 32,
	COLOR_LAYER5												= 33,
	COLOR_LAYER6												= 34,
	COLOR_LAYER7												= 35,
	COLOR_MATERIALMANAGER_BG						= 1131,
	COLOR_MATERIALMANAGER_SELECTED			= 1134,
	COLOR_MATERIALMANAGER_TEXT					= 1132,
	COLOR_MATERIALMANAGER_TEXT_SELECTED = 1133,
	COLOR_MAXHNWEIGHT										= 56,
	COLOR_MAXSOFTSELECT									= 53,
	COLOR_MEASURETOOL										= 48,
	COLOR_MENU_BG												= 1170,
	COLOR_MENU_BG_SELECTED							= 1171,
	COLOR_MENU_BORDER_DK								= 1176,
	COLOR_MENU_BORDER_WH								= 1175,
	COLOR_MENU_TEXT											= 1172,
	COLOR_MENU_TEXT_DISABLED						= 1173,
	COLOR_MENU_TEXT_SELECTED						= 1174,
	COLOR_MINHNWEIGHT										= 54,
	COLOR_MINSOFTSELECT									= 52,
	COLOR_MODIFIER											= 11,
	COLOR_NGONLINE											= 49,
	COLOR_OBJECTMANAGER_ACTIVECAMERA		= 1136,
	COLOR_OBJECTMANAGER_BG							= 1135,
	COLOR_OBJECTMANAGER_INACTIVECAMERA	= 1137,
	COLOR_OBJECTMANAGER_SELECTEDTAG			= 1141,
	COLOR_OBJECTMANAGER_TEXT						= 1138,
	COLOR_OBJECTMANAGER_TEXT_ACTIVE2		= 1140,
	COLOR_OBJECTMANAGER_TEXT_ACTIVE3		= 1198,
	COLOR_OBJECTMANAGER_TEXT_ACTIVE			= 1139,
	COLOR_OBJECTMANAGER_TREE						= 1142,
	COLOR_PAINTER_SELECTED							= 1150,
	COLOR_PAINTER_TREE									= 1151,
	COLOR_PARTICLE											= 8,
	COLOR_PMODIFIER											= 9,
	COLOR_R															= 1000,
	COLOR_SELECTION_PREVIEW							= 47,
	COLOR_SELECT_AXIS										= 27,
	COLOR_SPLINEEND											= 6,
	COLOR_SPLINESTART										= 5,
	COLOR_SPREADSHEET_BG								= 1143,
	COLOR_SPREADSHEET_EDITBG						= 1145,
	COLOR_SPREADSHEET_EDITTEXT					= 1146,
	COLOR_SPREADSHEET_SELECTED					= 1147,
	COLOR_SPREADSHEET_TEXT							= 1144,
	COLOR_SYNTAX_CLASS									= 1201,
	COLOR_SYNTAX_COFFEE									= 1152,
	COLOR_SYNTAX_COMMENT								= 1154,
	COLOR_SYNTAX_COMMENTWRONG						= 1155,
	COLOR_SYNTAX_CONSTANT								= 1199,
	COLOR_SYNTAX_FUNCTION								= 1200,
	COLOR_SYNTAX_INCLUDE								= 1153,
	COLOR_SYNTAX_MEMBER									= 1202,
	COLOR_SYNTAX_STRING									= 1156,
	COLOR_TANGENT												= 14,
	COLOR_TEXT													= 1003,
	COLOR_TEXTFOCUS											= 1004,
	COLOR_TEXTURE												= 17,
	COLOR_TEXTUREAXIS										= 18,
	COLOR_TEXT_BUTTON										= 1166,
	COLOR_TEXT_BUTTON_DISABLED					= 1167,
	COLOR_TEXT_COMBO										= 1168,
	COLOR_TEXT_COMBO_DISABLED						= 1169,
	COLOR_TEXT_DISABLED									= 1159,
	COLOR_TEXT_EDIT											= 1160,
	COLOR_TEXT_EDIT_DISABLED						= 1161,
	COLOR_TEXT_MENU											= 1162,
	COLOR_TEXT_MENU_DISABLED						= 1163,
	COLOR_TEXT_SELECTED									= 1157,
	COLOR_TEXT_SELECTED_DARK						= 1158,
	COLOR_TEXT_TAB											= 1164,
	COLOR_TEXT_TAB_DISABLED							= 1165,
	COLOR_TIMELINE											= 1016,
	COLOR_TIMELINESCROLLER_CURRENT			= 1119,
	COLOR_TIMELINESCROLLER_MINMAX				= 1118,
	COLOR_TIMELINE_BG										= 1104,
	COLOR_TIMELINE_BG_DARK							= 1105,
	COLOR_TIMELINE_LAYER0								= 1177,
	COLOR_TIMELINE_LAYER1								= 1178,
	COLOR_TIMELINE_LAYER2								= 1179,
	COLOR_TIMELINE_LAYER3								= 1180,
	COLOR_TIMELINE_LAYER4								= 1181,
	COLOR_TIMELINE_LAYER5								= 1182,
	COLOR_TIMELINE_LAYER6								= 1183,
	COLOR_TIMELINE_LAYER7								= 1184,
	COLOR_TIMELINE_LINES								= 1109,
	COLOR_TIMELINE_LINES_SEL						= 1110,
	COLOR_TIMELINE_LINES_SEPARATOR1			= 1111,
	COLOR_TIMELINE_LINES_SEPARATOR2			= 1112,
	COLOR_TIMELINE_LOOP									= 1114,
	COLOR_TIMELINE_SELECTION						= 1115,
	COLOR_TIMELINE_TEXT									= 1106,
	COLOR_TIMELINE_TEXT_SELECTED2				= 1108,
	COLOR_TIMELINE_TEXT_SELECTED				= 1107,
	COLOR_TIMELINE_TICKS1								= 1116,
	COLOR_TIMELINE_TICKS2								= 1117,
	COLOR_TIMELINE_TREE									= 1113,
	COLOR_TRANS													= 999,
	COLOR_VERTEXEND											= 37,
	COLOR_VERTEXSTART										= 36,
	COLOR_WXAXIS												= 24,
	COLOR_WYAXIS												= 25,
	COLOR_WZAXIS												= 26,
	COLOR_XAXIS													= 21,
	COLOR_XOR														= 998,
	COLOR_YAXIS													= 22,
	COLOR_ZAXIS													= 23,
	COLOR_ZEROHNWEIGHT									= 55,
	COL_ALL_CONTACTS										= 1,
	COL_ERR_EMPTY_OBJECT								= -4,
	COL_ERR_OUT_OF_MEMORY								= -1,
	COL_ERR_UNPROCESSED_MODEL						= -2,
	COL_ERR_WRONG_BUILD_ORDER						= -3,
	COL_FIRST_CONTACT										= 2,
	COL_OK															= 0,
	COREMSG_CINEMA											= 1129145193,
	COREMSG_CINEMA_EXECUTEEDITORCOMMAND = 200000023,
	COREMSG_CINEMA_FORCE_AM_UPDATE			= 1001077,
	COREMSG_CINEMA_GETCOMMANDCHECKED		= 300000115,
	COREMSG_CINEMA_GETCOMMANDENABLED		= 200000035,
	COREMSG_CINEMA_GETCOMMANDNAME				= 200000033,
	COREMSG_CINEMA_GETOPENGLFEATURES		= 1733248844,
	COREMSG_UPDATECOMMANDSMESSAGE				= 200000100,
	CREATE_GL_HAS_ROOT									= 1,
	CREATE_GL_IS_ROOT										= 2,
	CSEbase															= 1001080,
	CSPACE_1COMPONENT										= 1,
	CSPACE_2COMPONENTS									= 2,
	CSPACE_3COMPONENTS									= 3,
	CSPACE_4COMPONENTS									= 4,
	CSPACE_5COMPONENTS									= 5,
	CSPACE_6COMPONENTS									= 6,
	CSPACE_7COMPONENTS									= 7,
	CSPACE_AGRAY												= 263,
	CSPACE_ALPHA_FLAG										= 256,
	CSPACE_APhotoYCC										= 264,
	CSPACE_ARGB													= 257,
	CSPACE_AYCbCr												= 261,
	CSPACE_CMYK													= 4,
	CSPACE_COMP_MASK										= 15,
	CSPACE_FLAGS												= 65280,
	CSPACE_GRAY													= 7,
	CSPACE_HSL													= 3,
	CSPACE_HSV													= 2,
	CSPACE_Lab													= 18
};
enum
{
	CSPACE_Luv																	 = 19,
	CSPACE_MASK																	 = 255,
	CSPACE_PhotoYCC															 = 8,
	CSPACE_RGB																	 = 1,
	CSPACE_UNKNOWN															 = 0,
	CSPACE_XYZ																	 = 16,
	CSPACE_YCCK																	 = 6,
	CSPACE_YCbCr																 = 5,
	CSPACE_Yxy																	 = 17,
	CUSTOMDATATYPE_BITMAPBUTTON									 = 1001053,
	CUSTOMDATATYPE_DESCID												 = 1000486,
	CUSTOMDATATYPE_GRADIENT											 = 1000481,
	CUSTOMDATATYPE_HYPER_LINK_STATIC						 = 1009617,
	//	CUSTOMDATATYPE_INEXCLUDE_LIST								 = 1009290,
	CUSTOMDATATYPE_INFO_DONTREGISTERGVTYPE			 = 8,
	CUSTOMDATATYPE_INFO_HASSUBDESCRIPTION				 = 1,
	CUSTOMDATATYPE_INFO_HASSUBDESCRIPTION_NOANIM = 65,
	CUSTOMDATATYPE_INFO_NEEDDATAFORSUBDESC			 = 2,
	CUSTOMDATATYPE_INFO_SUBDESCRIPTIONDISABLEGUI = 16,
	CUSTOMDATATYPE_INFO_TOGGLEDISPLAY						 = 4,
	CUSTOMDATATYPE_INFO_UNDOSAMECUSTOMGUI				 = 32,
	CUSTOMDATATYPE_LAYERSET											 = 200000044,
	CUSTOMDATATYPE_LENSGLOW											 = 1000997,
	CUSTOMDATATYPE_MATASSIGN										 = 200000027,
	//	CUSTOMDATATYPE_MATPREVIEW										 = 1011153,
	CUSTOMDATATYPE_SPLINE												 = 1009060,
	CUSTOMDATATYPE_TEXGROUP											 = 1001051,
	CUSTOMGUI_BITMAPBUTTON											 = 1000479,
	CUSTOMGUI_DEFAULTEDITHEIGHT									 = 1684366952,
	CUSTOMGUI_DESCRIPTION												 = 1000469,
	CUSTOMGUI_DESCRIPTIONPROPERTY								 = 1000485,
	CUSTOMGUI_FILENAME													 = 1000478,
	CUSTOMGUI_GRADIENT													 = 1000480,
	CUSTOMGUI_HYPER_LINK_STATIC									 = 1009180,
	CUSTOMGUI_INEXCLUDE_LIST										 = 1009290,
	CUSTOMGUI_LAYERSET													 = 200000043,
	CUSTOMGUI_LAYOUTMODE												 = 1818327405,
	CUSTOMGUI_LENSGLOW													 = 1000996,
	CUSTOMGUI_LINKBOX														 = 1009415,
	CUSTOMGUI_LONG_LAT													 = 1011148,
	CUSTOMGUI_MATASSIGN													 = 200000026,
	//	CUSTOMGUI_MATPREVIEW												 = 1011152,
	CUSTOMGUI_NORMAL														 = 400006005,
	CUSTOMGUI_PRIORITY													 = 1009364,
	CUSTOMGUI_QUICKTAB													 = 1000495,
	CUSTOMGUI_SAVEDLAYOUTDATA										 = 1936483425,
	CUSTOMGUI_SPLINE														 = 1009059,
	CUSTOMGUI_SUPPORT_LAYOUTDATA								 = 2,
	CUSTOMGUI_SUPPORT_LAYOUTSWITCH							 = 1,
	CUSTOMGUI_TEXBOX														 = 1001066,
	CUSTOMGUI_TEXGROUP													 = 1001050,
	CUSTOMGUI_TEXTURENAME												 = 1000484,
	CUSTOMGUI_TREEVIEW													 = 1000475,
	CUSTOMGUI_VECTOR2D													 = 200000101,
	CYCLE_ANIMATION															 = 1,
	CYCLE_DYNAMICS															 = 3,
	CYCLE_EXPRESSION														 = 2,
	CYCLE_GENERATORS														 = 4,
	CYCLE_INITIAL																 = 0,
	CategoryCatalogNodes												 = 1,
	CategoryDriveNodes													 = 2,
	CategoryPresetNodes													 = 0,
	CategoryRecentNodes													 = 3,
	CategorySearchNodes													 = 4,
	CheckOperation															 = 0,
	Computer																		 = 0,
	CopyError																		 = -2147482624,
	CopyNotPermitted														 = -2147483644,
	DATACHANNEL_INFO_CHAN												 = 100,
	DATACHANNEL_INFO_NAME												 = 101,
	DATACHANNEL_INFO_TYPE												 = 102,
	DATETIME_CALENDAR_OPEN											 = 1685349217,
	DATETIME_CLOCK_OPEN													 = 1685349228,
	DATETIME_COLUMN															 = 1685349231,
	DATETIME_COMPACT_MODE												 = 1685349229,
	DATETIME_COMPACT_OPEN												 = 1685349231,
	DATETIME_DATA																 = 1009398,
	DATETIME_DATE_CONTROL												 = 1685349475,
	DATETIME_DATE_TODAY													 = 1685349492,
	DATETIME_GUI																 = 1009383,
	DATETIME_NO_DATE_COLLAPSE										 = 1685349473,
	DATETIME_NO_SECONDS													 = 1685352051,
	DATETIME_NO_TIME_COLLAPSE										 = 1685353569,
	DATETIME_TIME_CONTROL												 = 1685353571,
	DATETIME_TIME_NOW														 = 1685353582,
	DA_CAMERAEXPRESSION													 = 65536,
	DA_FORCEFULLREDRAW													 = 32768,
	DA_INDRAG																		 = 8192,
	DA_NODUALPLANE															 = 1048576,
	DA_NOT_KEY																	 = 128,
	DA_NO_ANIMATION															 = 256,
	DA_NO_BLIT																	 = 512,
	DA_NO_EXPRESSIONS														 = 4096,
	DA_NO_REDUCTION															 = 4,
	DA_NO_THREAD																 = 2,
	DA_ONEPLANE																	 = 64,
	DA_ONLY_ACTIVE															 = 8,
	DA_ONLY_ACTIVE_VIEW													 = 1024,
	DA_ONLY_BACK																 = 16,
	DA_ONLY_HIGHLIGHT														 = 262144,
	DA_STATICBREAK															 = 524290,
	DEFAULTFILENAME_SHADER_EFFECTS							 = 1002,
	DEFAULTFILENAME_SHADER_SURFACES							 = 1001,
	DEFAULTFILENAME_SHADER_VOLUME								 = 1003,
	DELETE_GL_HAS_ROOT													 = 1,
	DELETE_GL_IS_ROOT														 = 2,
	DESCFLAGS_DESC_DONTLOADDEFAULT							 = 8,
	DESCFLAGS_DESC_LOADED												 = 2,
	DESCFLAGS_DESC_RECURSIONLOCK								 = 4,
	DESCFLAGS_DESC_RESOLVEMULTIPLEDATA					 = 1,
	//DESCFLAGS_DONTCHECKMINMAX=8,
	//DESCFLAGS_GET_PARAM_GET=2,
	//DESCFLAGS_SET_PARAM_SET=2,
	//DESCFLAGS_USERINTERACTION=4,
	DESCPROP_ANIM_NOT_ALLOWED				 = 64,
	DESCPROP_AUTOKEY_ACTIVE					 = 4,
	DESCPROP_AUTOKEY_INACTIVE				 = 8,
	DESCPROP_ENABLED								 = 32,
	DESCPROP_GLOBAL_PARAMETER				 = 128,
	DESCPROP_KEY										 = 2,
	DESCPROP_NOANIM									 = 16,
	DESCPROP_SEQUENCE								 = 1,
	DESCRIPTIONPROPERTY_COLOR_ANIM	 = 10003,
	DESCRIPTIONPROPERTY_COLOR_KEY		 = 10001,
	DESCRIPTIONPROPERTY_COLOR_NOANIM = 10002,
	DESCRIPTIONPROPERTY_COLOR_SEQ		 = 10000,
	DESCRIPTIONPROPERTY_PARENTMSG		 = 20000,
	DESCRIPTION_ALLOWFOLDING				 = 1718379623,
	DESCRIPTION_LEFTMATEDITREGION		 = 1819108722,
	DESCRIPTION_MODALMODE						 = 1836016748,
	DESCRIPTION_NOUNDO							 = 1852798318,
	DESCRIPTION_OBJECTSNOTINDOC			 = 1869506916,
	DESCRIPTION_SHOWTITLE						 = 1953068140,
	DESCRIPTION_STORAGE							 = 1937010546,
	DIALOGRESOURCE_									 = 1935960889,
	DIALOG_													 = 58,
	DIALOG_ACTIVATE									 = 26,
	DIALOG_ADDMENUCMD								 = 29,
	DIALOG_ADDMENUSEP								 = 40,
	DIALOG_ADDMENUSTR								 = 38,
	DIALOG_ADDSUBMENU								 = 27,
	DIALOG_ARROWBUTTON							 = 5,
	DIALOG_BUTTON										 = 4,
	DIALOG_CHECKBOX									 = 2,
	DIALOG_CHECKCLOSE								 = 48,
	DIALOG_CHECKNUMBERS							 = 32,
	DIALOG_CHILD										 = 17,
	DIALOG_COLORCHOOSER							 = 12,
	DIALOG_COLORFIELD								 = 11,
	DIALOG_COMBOBOX									 = 15,
	DIALOG_DLGGROUP									 = 19,
	DIALOG_EDITNUMBER								 = 7,
	DIALOG_EDITNUMBERUD							 = 8,
	DIALOG_EDITSLIDER								 = 9,
	DIALOG_EDITTEXT									 = 6,
	DIALOG_ENDSUBMENU								 = 28,
	DIALOG_FLUSHGROUP								 = 34,
	DIALOG_FLUSHMENU								 = 30,
	DIALOG_FREECHILDS								 = 18,
	DIALOG_GETTRISTATE							 = 49,
	DIALOG_GROUPBORDER							 = 22,
	DIALOG_GROUPBORDERSIZE					 = 23,
	DIALOG_GROUPSPACE								 = 21,
	DIALOG_INIT											 = 31,
	DIALOG_INITMENUSTR							 = 43,
	DIALOG_ISOPEN										 = 52,
	DIALOG_LAYOUTCHANGED						 = 25,
	DIALOG_LISTVIEW									 = 46,
	DIALOG_LOADWEIGHTS							 = 57,
	DIALOG_MENUGROUPBEGIN						 = 54,
	DIALOG_MENURESOURCE							 = 45,
	DIALOG_MULTILINEEDITTEXT				 = 42,
	DIALOG_NOMENUBAR								 = 55,
	DIALOG_PIN											 = 1,
	DIALOG_POPUPBUTTON							 = 16,
	DIALOG_RADIOBUTTON							 = 39,
	DIALOG_RADIOGROUP								 = 14,
	DIALOG_RADIOTEXT								 = 44
};
enum
{
	DIALOG_REMOVEGADGET					= 53,
	DIALOG_SAVEWEIGHTS					= 56,
	DIALOG_SCREEN2LOCALX				= 36,
	DIALOG_SCREEN2LOCALY				= 37,
	DIALOG_SCROLLGROUP					= 51,
	DIALOG_SDK									= 50,
	DIALOG_SEPARATOR						= 41,
	DIALOG_SETGROUP							= 33,
	DIALOG_SETIDS								= 24,
	DIALOG_SETMENU							= 35,
	DIALOG_SETTITLE							= 20,
	DIALOG_SLIDER								= 10,
	DIALOG_STATICTEXT						= 3,
	DIALOG_SUBDIALOG						= 47,
	DIALOG_USERAREA							= 13,
	DIFFUSION_AFFECT_LUMINANCE	= 1002,
	DIFFUSION_AFFECT_REFLECTION = 1001,
	DIFFUSION_AFFECT_SPECULAR		= 1000,
	//DIRTY_CACHE=16,
	//DIRTYFLAGS_DATA=4,
	//DIRTYFLAGS_MATRIX=2,
	//DIRTY_SELECT=8,
	DISPLACEMENT_HEIGHT							 = 1026,
	DISPLACEMENT_STRENGTH						 = 1025,
	DISPLAYCONTROLMODE_BOX					 = 5,
	DISPLAYCONTROLMODE_FLATBOX			 = 13,
	DISPLAYCONTROLMODE_FLATISOPARM	 = 12,
	DISPLAYCONTROLMODE_FLATWIRE			 = 11,
	DISPLAYCONTROLMODE_GOURAUD			 = 0,
	DISPLAYCONTROLMODE_GOURAUDISO		 = 8,
	DISPLAYCONTROLMODE_GOURAUDWIRE	 = 7,
	DISPLAYCONTROLMODE_HIDDENBOX		 = 16,
	DISPLAYCONTROLMODE_HIDDENISOPARM = 15,
	DISPLAYCONTROLMODE_HIDDENWIRE		 = 14,
	DISPLAYCONTROLMODE_ISOBATH			 = 3,
	DISPLAYCONTROLMODE_QUICK				 = 1,
	DISPLAYCONTROLMODE_QUICKISO			 = 10,
	DISPLAYCONTROLMODE_QUICKWIRE		 = 9,
	DISPLAYCONTROLMODE_SHADEDBOX		 = 4,
	DISPLAYCONTROLMODE_SKELETON			 = 6,
	DISPLAYCONTROLMODE_WIRE					 = 2,
	DISPLAYTAG_BACKCULL							 = 1001,
	DISPLAYTAG_DISPLAY							 = 1000,
	DISPLAYTAG_LOD									 = 1010,
	DISPLAYTAG_LODVAL								 = 1004,
	DISPLAYTAG_SDISPLAY							 = 1000,
	DISPLAYTAG_WDISPLAY							 = 1013,
	DISPLAY_EDITSTATE_DEFORM				 = 2,
	DISPLAY_EDITSTATE_SDS						 = 1,
	DLG_														 = 5,
	DLG_ALIGN_CENTER								 = 1,
	DLG_ALIGN_LEFT									 = 0,
	DLG_ALIGN_RIGHT									 = 2,
	DLG_CANCEL											 = 2,
	DLG_HELP												 = 4,
	DLG_MAC													 = 1,
	DLG_OK													 = 1,
	DOF_BACK												 = 3,
	DOF_BOTH												 = 1,
	DOF_FRONT												 = 2,
	DOF_NONE												 = 0,
	DRAGANDDROPTAG_FLAG_ACCEPT			 = 4,
	DRAGANDDROPTAG_FLAG_DROP				 = 2,
	DRAGANDDROPTAG_FLAG_RECEIVE			 = 1,
	DRAGTYPE_ANAME									 = 104,
	DRAGTYPE_ATOMARRAY							 = 201,
	DRAGTYPE_ATRENN									 = 103,
	DRAGTYPE_A_KEY									 = 44006,
	DRAGTYPE_A_LINE									 = 45000,
	DRAGTYPE_A_LOOP									 = 44018,
	DRAGTYPE_A_LOOP_MAX							 = 44014,
	DRAGTYPE_A_LOOP_MIN							 = 44013,
	DRAGTYPE_A_MARKER								 = 44007,
	DRAGTYPE_A_OBJECT								 = 44000,
	DRAGTYPE_A_RKEYS								 = 305,
	DRAGTYPE_A_SELECT								 = 44017,
	DRAGTYPE_A_SELECT_MAX						 = 44011,
	DRAGTYPE_A_SELECT_MIN						 = 44010,
	DRAGTYPE_A_SEQ									 = 44002,
	DRAGTYPE_A_SEQ_NEW							 = 44005,
	DRAGTYPE_A_TIME									 = 44008,
	DRAGTYPE_A_TIME_PS							 = 44020,
	DRAGTYPE_A_TRACK								 = 44001,
	DRAGTYPE_A_TRENN1								 = 44015,
	DRAGTYPE_A_TRENN2								 = 44016,
	DRAGTYPE_A_VDRAG								 = 45001,
	DRAGTYPE_BROWSER								 = 1017705,
	DRAGTYPE_BROWSER_COFFEE					 = 405,
	DRAGTYPE_BROWSER_FCV						 = 404,
	DRAGTYPE_BROWSER_MATERIAL				 = 402,
	DRAGTYPE_BROWSER_SCENE					 = 401,
	DRAGTYPE_BROWSER_SOUND					 = 403,
	DRAGTYPE_CMDPALETTE							 = 7,
	DRAGTYPE_COMMAND								 = 6,
	DRAGTYPE_CTRENN									 = 105,
	DRAGTYPE_DESCID									 = 113,
	DRAGTYPE_FILENAME_IMAGE					 = 202,
	DRAGTYPE_FILENAME_OTHER					 = 205,
	DRAGTYPE_FILENAME_SCENE					 = 204,
	DRAGTYPE_FILES									 = 1,
	DRAGTYPE_GVNODE_DUMMY						 = 400007003,
	DRAGTYPE_GVNODE_EDITOR					 = 400007000,
	DRAGTYPE_GVNODE_MANAGER					 = 400007001,
	DRAGTYPE_GVNODE_STORE						 = 400007002,
	DRAGTYPE_HV											 = 106,
	DRAGTYPE_ICON										 = 4,
	DRAGTYPE_MANAGER								 = 5,
	DRAGTYPE_MCURVE									 = 110,
	DRAGTYPE_OTRENN									 = 102,
	DRAGTYPE_RGB										 = 203,
	DRAGTYPE_TIMEA									 = 107,
	DRAGTYPE_TIMEC									 = 108,
	DRAGTYPE_TIMEU									 = 109,
	DRAGTYPE_U_MARKERX							 = 111,
	DRAGTYPE_U_MARKERY							 = 112,
	DRAWFLAGS_HIGHLIGHT							 = 2,
	DRAWFLAGS_INVERSE_Z							 = 1,
	DRAWPASS_BOX										 = 1,
	DRAWPASS_HANDLES								 = 2,
	DRAWPASS_HIGHLIGHTS							 = 3,
	DRAWPASS_OBJECT									 = 0,
	DRAWPOLYOBJECT_EDITMODE					 = 8,
	DRAWPOLYOBJECT_FORCELINES				 = 1,
	DRAWPOLYOBJECT_LOCALMATRIX			 = 4,
	DRAWPOLYOBJECT_NOBACKCULL				 = 2,
	DRAW_ALPHA_INVERTED							 = 11,
	DRAW_ALPHA_NONE									 = 10,
	DRAW_ALPHA_NORMAL								 = 12,
	DRAW_AXIS												 = 2,
	DRAW_HANDLES										 = 1,
	DRAW_HIGHLIGHTS									 = 4,
	DRAW_PARAMETER_ALPHA_THRESHOLD	 = 1002,
	DRAW_PARAMETER_LINEWIDTH				 = 1000,
	DRAW_PARAMETER_SETZ							 = 2,
	DRAW_Z_GREATER									 = 1,
	DRAW_Z_LOWEREQUAL								 = 0,
	DROLDWIN_SDK										 = 1935960888,
	DR_COLORFIELD_BODYPAINT					 = 4,
	DR_COLORFIELD_NO_BRIGHTNESS			 = 1,
	DR_COLORFIELD_NO_COLOR					 = 2,
	DR_MULTILINE_HIGHLIGHTLINE			 = 8,
	DR_MULTILINE_MONOSPACED					 = 1,
	DR_MULTILINE_STATUSBAR					 = 4,
	DR_MULTILINE_SYNTAXCOLOR				 = 2,
	DT_ARRAY												 = 9,
	DT_BYTECODE											 = 10,
	DT_BYTES												 = 5,
	DT_CLASS												 = 7,
	DT_CODE													 = 11,
	DT_DICTIONARY										 = 15,
	DT_ENTRY												 = 16,
	DT_EXCEPTION										 = 13,
	DT_EXTCODE											 = 12,
	DT_FLOAT												 = 2,
	DT_INSTANCE											 = 17,
	DT_LONG													 = 1,
	DT_NIL													 = 0,
	DT_NUMBER												 = 99,
	DT_OBJECT												 = 8,
	DT_SEXCEPTION										 = 14,
	DT_STRING												 = 6,
	DT_VECTOR												 = 3,
	DT_VOID													 = 4,
	DefaultQuality									 = 0,
	DeleteError											 = -2147481600,
	DeleteNotPermitted							 = -2147483640,
	Different												 = 8,
	Directory												 = 1,
	Document												 = 8,
	Drive														 = 1,
	DriveCD													 = 2,
	EDGES_HIDDEN										 = 1,
	EDGES_PHONG											 = 2,
	EDGES_SELECTION									 = 0,
	EDITTEXT_PASSWORD								 = 1,
	ENVIRONMENT_TILESX							 = 1006,
	ENVIRONMENT_TILESY							 = 1007,
	EVENT_ANIMATE										 = 2,
	EVENT_CAMERAEXPRESSION					 = 16,
	EVENT_FORCEREDRAW								 = 1,
	EVENT_GLHACK										 = 8,
	EVENT_NOEXPRESSION							 = 4,
	EVMSG_ACTIVEVIEWCHANGED					 = 1633908343,
	EVMSG_ASYNCEDITORMOVE						 = 1701080438,
	EVMSG_AUTKEYMODECHANGED					 = 200000009,
	EVMSG_BROWSERCHANGE							 = -1002
};
enum
{
	EVMSG_CHANGE										 = 604,
	EVMSG_DOCUMENTRECALCULATED			 = 1685218156,
	EVMSG_FCURVECHANGE							 = -1010,
	EVMSG_FRAME_SCENE								 = -1005,
	EVMSG_GRAPHVIEWCHANGED					 = 400008000,
	EVMSG_MATERIALPREVIEW						 = -1008,
	EVMSG_MATERIALSELECTION					 = -1009,
	EVMSG_RAYTRACER_FINISHED				 = -1003,
	EVMSG_TIMECHANGED								 = 1952671847,
	EVMSG_TIMELINESELECTION					 = -1001,
	EVMSG_TOOLCHANGED								 = -826267,
	EVMSG_UPDATEHIGHLIGHT						 = 200000073,
	EXECUTION_ANIMATION							 = 2000,
	EXECUTION_DYNAMICS							 = 4000,
	EXECUTION_EXPRESSION						 = 3000,
	EXECUTION_GENERATOR							 = 5000,
	EXECUTION_INITIAL								 = 1000,
	EXPRESSIONTAG_PASS							 = 999103,
	EXTENDED_MODE_AUTOSWITCH				 = 3,
	EXTENDED_MODE_NORMAL						 = 0,
	EXTENDED_MODE_SOFT							 = 2,
	EXTENDED_MODE_TWEAK							 = 1,
	FIELD_EVEN											 = 1,
	FIELD_NONE											 = 0,
	FIELD_ODD												 = 2,
	FILENAME_DIRECTORY							 = 10000,
	FILENAME_SAVE										 = 10002,
	FILENAME_TEXTURE								 = 10001,
	FILETIME_ACCESS									 = 2,
	FILETIME_CREATED								 = 0,
	FILETIME_MODIFIED								 = 1,
	FILTER_ABORT										 = 2,
	FILTER_AVI_BIG									 = 1121,
	FILTER_AVI_SMALL								 = 1120,
	FILTER_AVI_USER									 = 1122,
	FILTER_B3D											 = 1109,
	FILTER_BMP											 = 1102,
	FILTER_CANCEL										 = 1,
	FILTER_HDR											 = 1001379,
	FILTER_IFF											 = 1103,
	FILTER_JPG											 = 1104,
	FILTER_MEM_ERR									 = -2,
	FILTER_MISC_ERR									 = -3,
	FILTER_MOVIE_BIG								 = 1124,
	FILTER_MOVIE_SMALL							 = 1123,
	FILTER_MOVIE_USER								 = 1125,
	FILTER_OK												 = 0,
	FILTER_PARAM_ERR								 = -1,
	FILTER_PICT											 = 1105,
	FILTER_PSD											 = 1106,
	FILTER_QTVRSAVER_OBJECT					 = 1151,
	FILTER_QTVRSAVER_PANORAMA				 = 1150,
	FILTER_RLA											 = 1107,
	FILTER_RPF											 = 1108,
	FILTER_TGA											 = 1101,
	FILTER_TIF											 = 1100,
	FIRST_POPUP_ID									 = 900000,
	FOCUS_													 = 5,
	FOCUS_HIDDEN										 = 3,
	FOCUS_INACT											 = 2,
	FOCUS_INPROGRESS								 = 4,
	FOCUS_OFF												 = 0,
	FOCUS_ON												 = 1,
	FOG_DISTANCE										 = 1008,
	FOLDERKEY_VALUE									 = 1,
	FONT_BOLD												 = 2,
	FONT_DEFAULT										 = 0,
	FONT_MONOSPACED									 = 3,
	FONT_POSTSCRIPT_NAME						 = 505,
	FONT_STANDARD										 = 1,
	FONT_USE_POSTSCRIPT							 = 499,
	FORMAT_3DSEXPORT								 = 1001038,
	FORMAT_3DSIMPORT								 = 1001037,
	FORMAT_AIIMPORT									 = 1001045,
	FORMAT_C4D4IMPORT								 = 1001029,
	FORMAT_C4D5IMPORT								 = 1001030,
	FORMAT_C4DEXPORT								 = 1001026,
	FORMAT_C4DIMPORT								 = 1001025,
	FORMAT_C4D_6_XML								 = 1021,
	FORMAT_D3DEXPORT								 = 1001047,
	FORMAT_DEGREE										 = 1717856114,
	FORMAT_DEMIMPORT								 = 1001046,
	FORMAT_DXFEXPORT								 = 1001036,
	FORMAT_DXFIMPORT								 = 1001035,
	FORMAT_FCV											 = 1019,
	FORMAT_FRAMES										 = 1717989997,
	FORMAT_L4D											 = 1020,
	FORMAT_INT											 = 1718382183,
	FORMAT_LWOIMPORT								 = 1001044,
	FORMAT_LWSIMPORT								 = 1001043,
	FORMAT_METER										 = 1718445428,
	FORMAT_OBJEXPORT								 = 1001040,
	FORMAT_OBJIMPORT								 = 1001039,
	FORMAT_P4D											 = 1022,
	FORMAT_PERCENT									 = 1718641524,
	FORMAT_PREF											 = 1000,
	FORMAT_Q3DEXPORT								 = 1001042,
	FORMAT_Q3DIMPORT								 = 1001041,
	FORMAT_FLOAT										 = 1718773089,
	FORMAT_SECONDS									 = 1718838627,
	FORMAT_SMPTE										 = 1718840688,
	FORMAT_VRML1EXPORT							 = 1001032,
	FORMAT_VRML1IMPORT							 = 1001031,
	FORMAT_VRML2EXPORT							 = 1001034,
	FORMAT_VRML2IMPORT							 = 1001033,
	FORMAT_WAV											 = 1018,
	FORMAT_XMLEXPORT								 = 1001028,
	FORMAT_XMLIMPORT								 = 1001027,
	FSTYPE_ANYTHING									 = 0,
	FSTYPE_COFFEE										 = 3,
	FSTYPE_IMAGES										 = 1,
	FSTYPE_SCENES										 = 2,
	FindBestMatch										 = 2,
	FindExactMatch									 = 1,
	FixedSize												 = -2147483647,
	Flush														 = 64,
	Folder													 = 3,
	FolderCatalogs									 = 6,
	FolderOpen											 = 4,
	FolderPresets										 = 5,
	FolderSearch										 = 7,
	ForceDirectoryUpdate						 = 4,
	GAGDET_BMFILTER_DST							 = 1048577,
	GAGDET_BMFILTER_GROW						 = 1048578,
	GAGDET_BMFILTER_PERCENT					 = 1048580,
	GAGDET_BMFILTER_SHRINK					 = 1048579,
	GAGDET_BMFILTER_SRC							 = 1048576,
	GEMB_ABORTRETRYIGNORE						 = 2,
	GEMB_ICONASTERISK								 = 64,
	GEMB_ICONEXCLAMATION						 = 48,
	GEMB_ICONQUESTION								 = 32,
	GEMB_ICONSTOP										 = 16,
	GEMB_MULTILINE									 = 128,
	GEMB_OK													 = 0,
	GEMB_OKCANCEL										 = 1,
	GEMB_RETRYCANCEL								 = 5,
	GEMB_R_ABORT										 = 3,
	GEMB_R_CANCEL										 = 2,
	GEMB_R_IGNORE										 = 5,
	GEMB_R_NO												 = 7,
	GEMB_R_OK												 = 1,
	GEMB_R_RETRY										 = 4,
	GEMB_R_YES											 = 6,
	GEMB_YESNO											 = 4,
	GEMB_YESNOCANCEL								 = 3,
	GETACTIVEUVSET_ALL							 = 65534,
	GETACTIVEUVSET_ALL_CHECKENABLING = 65535,
	GETACTIVEUVSET_CHECKENABLING		 = 1,
	GETACTIVEUVSET_MODE							 = 128,
	GETACTIVEUVSET_OBJECT						 = 64,
	GETACTIVEUVSET_POINTS						 = 16,
	GETACTIVEUVSET_POINTSELECTION		 = 32,
	GETACTIVEUVSET_POLYS						 = 4,
	GETACTIVEUVSET_POLYSELECTION		 = 8,
	GETACTIVEUVSET_UVWS							 = 2,
	GETALLSTRINGS_CHANNEL_ID				 = 3,
	GETALLSTRINGS_FILESTRING				 = 1,
	GETALLSTRINGS_SUGGESTED_PATH		 = 2,
	GETPIXEL_ALL										 = 3,
	GETPIXEL_ALPHAMASKS							 = 1,
	GETPIXEL_CACHE									 = 8,
	GETPIXEL_DITHERING							 = 512,
	GETPIXEL_DONTAPPLYMASK					 = 128,
	GETPIXEL_MERGE									 = 8192,
	GETPIXEL_NOMASK									 = 4,
	GETPIXEL_NOWORK									 = 16,
	GETPIXEL_ONLYWORK								 = 32,
	GETPIXEL_PROJECTIONPIXEL				 = 1024,
	GETPIXEL_RECALCBRUSHSTROKE			 = 2048,
	GETPIXEL_TILED									 = 64,
	GETPIXEL_USEBLENDING						 = 2,
	GETPIXEL_USESELECTED						 = 256,
	GETPIXEL_VOXELCACHING						 = 4096,
	GLOW_FREQUENCY									 = 1023,
	GLOW_INNER											 = 1019,
	GLOW_OUTER											 = 1020,
	GLOW_RADIUS											 = 1021,
	GLOW_RANDOM											 = 1022,
	GLOW_USEMATERIAL								 = 1024,
	GRAPHVIEW_CORE_PRIORITY					 = 30000,
	GRAPHVIEW_EDITOR_PRIORITY				 = 2000,
	GRAPHVIEW_HOOK_PRIORITY					 = 1000,
	GROUPWEIGHTS_PERCENT_H_CNT			 = 3000,
	GROUPWEIGHTS_PERCENT_H_VAL			 = 3001,
	GROUPWEIGHTS_PERCENT_W_CNT			 = 2000,
	GROUPWEIGHTS_PERCENT_W_VAL			 = 2001,
	GUI_DIALOG_CLOSE								 = 1002
};
enum
{
	GUI_DIALOG_MINIMIZE														 = 1000,
	GUI_DIALOG_RELEASE														 = 1001,
	GUI_H_																				 = 0,
	GV_ANIM_HOOK_FREE															 = 1,
	GV_ANIM_HOOK_INIT															 = 0,
	GV_ANIM_HOOK_MAIN															 = 2,
	GV_ATOM																				 = 400000008,
	GV_BOOL																				 = 400000000,
	GV_CONNECTION_MODE_DIRECT											 = 0,
	GV_CONNECTION_MODE_NORMAL											 = 2,
	GV_CONNECTION_MODE_STRAIGHT										 = 1,
	GV_CONSTANT_FIRST															 = 1,
	GV_CONSTANT_NONE															 = 0,
	GV_COPY_OFFSET_X															 = 10,
	GV_COPY_OFFSET_Y															 = 10,
	GV_DEFAULT_BODY_HEIGHT												 = 12,
	GV_DEFAULT_BODY_WIDTH													 = 32,
	GV_DESCRIPTIONFLAG_NONE												 = 0,
	GV_DRAW_EXTENDED															 = 2,
	GV_DRAW_FULLSCREEN														 = 3,
	GV_DRAW_MINIMIZED															 = 0,
	GV_DRAW_NONE																	 = -1,
	GV_DRAW_NORMAL																 = 1,
	GV_ELEMENT_SEPERATOR_CHR											 = 124,
	GV_GROUP_INPUT_FIRST_ID												 = 1000000,
	GV_GROUP_INPUT_FIRST_PRIVATE_ID								 = 10000,
	GV_GROUP_INPUT_LAST_ID												 = 536870911,
	GV_GROUP_INPUT_LAST_PRIVATE_ID								 = 49999,
	GV_GROUP_OUTPUT_FIRST_ID											 = 536870912,
	GV_GROUP_OUTPUT_FIRST_PRIVATE_ID							 = 50000,
	GV_GROUP_OUTPUT_LAST_ID												 = 1073741824,
	GV_GROUP_OUTPUT_LAST_PRIVATE_ID								 = 89999,
	GV_GUI_CONFIG_ADDITIONAL_FRAME_SELECT_DISTANCE = 1022,
	GV_GUI_CONFIG_ADDITIONAL_PORT_LABEL_SPACE			 = 1023,
	GV_GUI_CONFIG_BODY_COLOR											 = 1029,
	GV_GUI_CONFIG_BORDER_NORMAL_COLOR							 = 1040,
	GV_GUI_CONFIG_BORDER_NORMAL_DISABLED_COLOR		 = 1054,
	GV_GUI_CONFIG_BORDER_SELECTED_COLOR						 = 1041,
	GV_GUI_CONFIG_BORDER_SELECTED_DISABLED_COLOR	 = 1055,
	GV_GUI_CONFIG_BORDER_SIZE											 = 1009,
	GV_GUI_CONFIG_CONNECTION_DISABLED_COLOR				 = 1049,
	GV_GUI_CONFIG_CONNECTION_FORBIDDEN_COLOR			 = 1050,
	GV_GUI_CONFIG_CONNECTION_IN_FOCUS_COLOR				 = 1052,
	GV_GUI_CONFIG_CONNECTION_MAX_OFFSET						 = 1006,
	GV_GUI_CONFIG_CONNECTION_MIN_OFFSET						 = 1005,
	GV_GUI_CONFIG_CONNECTION_MODE									 = 1001,
	GV_GUI_CONFIG_CONNECTION_NORMAL_COLOR					 = 1046,
	GV_GUI_CONFIG_CONNECTION_SELECTED_COLOR				 = 1051,
	GV_GUI_CONFIG_CONNECTION_SELECT_DISTANCE			 = 1021,
	GV_GUI_CONFIG_CONNECTION_TEMP_COLOR						 = 1047,
	GV_GUI_CONFIG_CONNECTION_VALID_COLOR					 = 1048,
	GV_GUI_CONFIG_DOCK_HEIGHT											 = 1011,
	GV_GUI_CONFIG_DOCK_WIDTH											 = 1010,
	GV_GUI_CONFIG_FOOT_COLOR											 = 1028,
	GV_GUI_CONFIG_FOOT_HEIGHT											 = 1020,
	GV_GUI_CONFIG_GRID_COLOR											 = 1030,
	GV_GUI_CONFIG_GRID_X													 = 1002,
	GV_GUI_CONFIG_GRID_Y													 = 1003,
	GV_GUI_CONFIG_HEAD_ERROR_COLOR								 = 1027,
	GV_GUI_CONFIG_HEAD_HEIGHT											 = 1019,
	GV_GUI_CONFIG_HEAD_NORMAL_COLOR								 = 1026,
	GV_GUI_CONFIG_INDOCK_COLOR										 = 1035,
	GV_GUI_CONFIG_INPORTLABEL_BORDER_COLOR				 = 1044,
	GV_GUI_CONFIG_INPORTLABEL_COLOR								 = 1042,
	GV_GUI_CONFIG_INPORT_AREA_COLOR								 = 1031,
	GV_GUI_CONFIG_INPORT_COLOR										 = 1033,
	GV_GUI_CONFIG_LAYOUT_TYPE											 = 1000,
	GV_GUI_CONFIG_LINE_SPACE											 = 1024,
	GV_GUI_CONFIG_NR_OF_ENTRIES										 = 1058,
	GV_GUI_CONFIG_OUTDOCK_COLOR										 = 1036,
	GV_GUI_CONFIG_OUTPORTLABEL_BORDER_COLOR				 = 1045,
	GV_GUI_CONFIG_OUTPORTLABEL_COLOR							 = 1043,
	GV_GUI_CONFIG_OUTPORT_AREA_COLOR							 = 1032,
	GV_GUI_CONFIG_OUTPORT_COLOR										 = 1034,
	GV_GUI_CONFIG_PAN_COLOR												 = 1037,
	GV_GUI_CONFIG_PAN_HEIGHT											 = 1015,
	GV_GUI_CONFIG_PAN_WIDTH												 = 1014,
	GV_GUI_CONFIG_PORTS_SQUARE										 = 1053,
	GV_GUI_CONFIG_PORT_HEIGHT											 = 1013,
	GV_GUI_CONFIG_PORT_SPACE											 = 1018,
	GV_GUI_CONFIG_PORT_WIDTH											 = 1012,
	GV_GUI_CONFIG_SELECTBOX_COLOR									 = 1039,
	GV_GUI_CONFIG_SORT_MENUS											 = 1056,
	GV_GUI_CONFIG_SORT_PORTS											 = 1057,
	GV_GUI_CONFIG_STATUS_BAR											 = 1004,
	GV_GUI_CONFIG_TEXT_BORDER_HORZ								 = 1007,
	GV_GUI_CONFIG_TEXT_BORDER_VERT								 = 1008,
	GV_GUI_CONFIG_TEXT_COLOR											 = 1025,
	GV_GUI_CONFIG_ZOOM_COLOR											 = 1038,
	GV_GUI_CONFIG_ZOOM_HEIGHT											 = 1017,
	GV_GUI_CONFIG_ZOOM_WIDTH											 = 1016,
	GV_GUI_PREFERENCES_ID													 = 40000000,
	GV_INITIAL_DATA_SUB_ID												 = -1,
	GV_INITIAL_OPCLASS_SUB_ID											 = -1,
	GV_INITIAL_OPGROUP_SUB_ID											 = -1,
	GV_INITIAL_RUN_ID															 = 1,
	GV_INITIAL_VALGROUP_SUB_ID										 = -1,
	GV_INITIAL_VALUE_SUB_ID												 = -1,
	GV_INPUT_FIRST_ID															 = 10000000,
	GV_INPUT_FIRST_PRIVATE_ID											 = 30000000,
	GV_INPUT_LAST_ID															 = 19999990,
	GV_INPUT_LAST_PRIVATE_ID											 = 39999990,
	GV_INSERT_AFTER																 = 3,
	GV_INSERT_BEFORE															 = 2,
	GV_INSERT_NONE																 = 4,
	GV_INSERT_UNDER																 = 0,
	GV_INSERT_UNDER_LAST													 = 1,
	GV_INT																				 = 400000001,
	GV_INVALID																		 = 0,
	GV_INVALID_POS_VALUE													 = 2147483647,
	GV_IO_ERR_FILE_NOT_FOUND											 = 17,
	GV_IO_ERR_NONE																 = 0,
	GV_IO_ERR_NOT_A_GROUPNODE											 = 20,
	GV_IO_ERR_NO_MEMORY														 = 16,
	GV_IO_ERR_READ																 = 18,
	GV_IO_ERR_WRITE																 = 19,
	GV_LAYOUT_IN_BOTTOM														 = 3,
	GV_LAYOUT_IN_LEFT															 = 0,
	GV_LAYOUT_IN_RIGHT														 = 1,
	GV_LAYOUT_IN_TOP															 = 2,
	GV_MACRO_IDENT																 = 1000,
	GV_MASTER_CONFIG_ANIM_REFRESH									 = 101,
	GV_MASTER_CONFIG_LIVE_REFRESH									 = 100,
	GV_MASTER_PREFERENCES_ID											 = 40000002,
	GV_MATRIX																			 = 400000005,
	GV_MESSAGE_ADD_PORT_EX												 = 400000002,
	GV_MESSAGE_FIRST_DATA_ID											 = 100,
	GV_MESSAGE_MASTER_INIT												 = 400000016,
	GV_MESSAGE_MASTER_REDRAW											 = 400000017,
	GV_MESSAGE_NODE_CREATED												 = 400000010,
	GV_MESSAGE_NODE_DESELECTED										 = 400000013,
	GV_MESSAGE_NODE_DISABLED											 = 400000009,
	GV_MESSAGE_NODE_ENABLED												 = 400000008,
	GV_MESSAGE_NODE_REMOVED												 = 400000011,
	GV_MESSAGE_NODE_RENAMED												 = 400000007,
	GV_MESSAGE_NODE_SELECTED											 = 400000012,
	GV_MESSAGE_NONE																 = 0,
	GV_MESSAGE_OPERATOR_EDIT_SETTINGS							 = 400000015,
	GV_MESSAGE_OPERATOR_MENU											 = 400000014,
	GV_MESSAGE_PORT_ADDED													 = 400000000,
	GV_MESSAGE_PORT_REMOVED												 = 400000001,
	GV_MESSAGE_PORT_RENAMED												 = 400000004,
	GV_MESSAGE_REMOVE_PORT_EX											 = 400000003,
	GV_MESSAGE_RENAME_PORT_EX											 = 400000006,
	GV_MESSAGE_TRANSLATE_ID												 = 400000018,
	GV_MESSAGE_VIEW_CHANGED												 = 400000005,
	GV_NODEMASTER_RELEASE													 = 1102,
	GV_NODE_INVALID_INDEX													 = -1,
	GV_NORMAL																			 = 400000004,
	GV_NR_OF_LAYOUT_TYPES													 = 4,
	GV_NR_OF_STATES																 = 4,
	GV_NR_OF_SYSTEM_ERRORS												 = 21,
	GV_OPERATORFLAGS_MASK													 = 65535,
	GV_OPERATORFLAG_ITERATOR											 = 16,
	GV_OPERATORFLAG_MP_SAVE												 = 1,
	GV_OPERATORFLAG_MULTIPLE_DRAW									 = 4,
	GV_OPERATORFLAG_NONE													 = 0,
	GV_OPERATORFLAG_READ_ONLY_SAVE								 = 2,
	GV_OPERATORFLAG_REFRESH_ON_DELETE							 = 8,
	GV_OPERATORFLAG_SAVE													 = 3,
	GV_OPERATOR_DATA_ID														 = 102,
	GV_OPERATOR_INVALID_ID												 = -1,
	GV_OPERATOR_PORTS_ID													 = 101,
	GV_OPTIMIZE_NODES_EXTRA_BORDER								 = 6,
	GV_OP_DROP																		 = 2,
	GV_OP_DROP_IN_BODY														 = 3,
	GV_OP_NOP																			 = 0,
	GV_OP_SET																			 = 1,
	GV_OUTPUT_FIRST_ID														 = 20000000,
	GV_OUTPUT_FIRST_PRIVATE_ID										 = 40000000,
	GV_OUTPUT_LAST_ID															 = 29999990,
	GV_OUTPUT_LAST_PRIVATE_ID											 = 49999990,
	GV_PACK_NODES_EXTRA_BORDER										 = 12,
	GV_PORT_INVALID_ID														 = -1,
	GV_PORT_INVALID_INDEX													 = -1,
	GV_PORT_INVALID_SUB_ID												 = -1,
	GV_PRESET_IDENT																 = 1001,
	GV_REAL																				 = 400000002
};
enum
{
	GV_RESOURCE_SYMBOL_MAX_LENGTH												= 32,
	GV_STATE_DISABLED																		= 2,
	GV_STATE_NORMAL																			= 0,
	GV_STATE_SELECTED																		= 1,
	GV_STATE_SELECTED_AND_DISABLED											= 3,
	GV_STRING																						= 400000007,
	GV_TIME																							= 400000006,
	GV_USERDATA_INVALID_ID															= -1,
	GV_USERDEF																					= 400000009,
	GV_VALUE_SEPERATOR_CHR															= 59,
	GV_VECTOR																						= 400000003,
	GV_WORLD_CONFIG_UNDO_DEPTH													= 100,
	GV_WORLD_PREFERENCES_ID															= 40000001,
	GVbase																							= 1001101,
	Greater																							= 0,
	GreaterOrEqual																			= 1,
	HAIR_CALC_FLAG_APPLYDEFORMERS												= 2,
	HAIR_CALC_FLAG_APPLYMATERIALS												= 1,
	HAIR_CALC_FLAG_DISPLACE															= 4,
	HAIR_CALC_FLAG_SKIP																	= 8,
	HAIR_GENERATE_FLAGS_NONE														= 0,
	HAIR_GENERATE_FLAGS_NO_DEFORMERS										= 8,
	HAIR_GENERATE_FLAGS_NO_DYNAMICS											= 2,
	HAIR_GENERATE_FLAGS_NO_MATERIAL											= 1,
	HAIR_GENERATE_FLAGS_NO_TRANSFORM										= 4,
	HAIR_GENERATE_HOOK_BUILD														= 1,
	HAIR_GENERATE_HOOK_DYNAMICS													= 2,
	HAIR_GENERATE_PASS_BUILD														= 1,
	HAIR_GENERATE_PASS_FREE															= 2,
	HAIR_GENERATE_PASS_INIT															= 0,
	HAIR_GENERATE_PASS_THICKNESS												= 3,
	HAIR_GUIDE_FLAGS_AS_GUIDES													= 2,
	HAIR_GUIDE_FLAGS_HAIRS															= 16,
	HAIR_GUIDE_FLAGS_INITIAL														= 8,
	HAIR_GUIDE_FLAGS_INITIALAXIS												= 64,
	HAIR_GUIDE_FLAGS_LOCAL															= 1,
	HAIR_HOOK_TYPE_GENERATE_HAIR												= 4,
	HAIR_HOOK_TYPE_ILLUMINATE														= 5,
	HAIR_HOOK_TYPE_SAMPLE_COLOR													= 1,
	HAIR_HOOK_TYPE_SAMPLE_SHADOWS												= 3,
	HAIR_HOOK_TYPE_SAMPLE_TRANS													= 2,
	HAIR_LOCK_FLAGS_ANIMATING														= 4,
	HAIR_LOCK_FLAGS_FULL_UPDATE													= 16,
	HAIR_LOCK_FLAGS_NO_TRANSFORM												= 32,
	HAIR_MODE_GUIDES																		= 2,
	HAIR_MODE_HIDDEN																		= -1,
	HAIR_MODE_LOCKED																		= -2,
	HAIR_MODE_POINTS																		= 1,
	HAIR_MODE_ROOTS																			= 3,
	HAIR_MODE_TIPS																			= 0,
	HAIR_MODE_VERTEX																		= 4,
	HAIR_MPARAMETER_BACKCOLOR														= 3,
	HAIR_MPARAMETER_COLOR																= 2,
	HAIR_MPARAMETER_THICKNESS														= 1,
	HAIR_MPARAMETER_TRANSPARENCY												= 4,
	HAIR_MSG_DATA_VERSION																= 1000,
	HAIR_PLUGIN_DATA_VERSION														= 1000,
	HAIR_PLUGIN_TYPE_COLLIDER														= 3,
	HAIR_PLUGIN_TYPE_CONSTRAINT													= 2,
	HAIR_PLUGIN_TYPE_DEFORMER														= 4,
	HAIR_PLUGIN_TYPE_FORCE															= 1,
	HAIR_PLUGIN_TYPE_GENERATOR													= 5,
	HAIR_PLUGIN_TYPE_SHADER															= 6,
	HAIR_PLUGIN_TYPE_STYLING														= 7,
	HAIR_RENDER_INIT_PASS_BUILD													= 2,
	HAIR_RENDER_INIT_PASS_POST													= 3,
	HAIR_RENDER_INIT_PASS_PRE														= 1,
	HAIR_ROOT_TYPE_CLONE																= 8,
	HAIR_ROOT_TYPE_GUIDES																= 6,
	HAIR_ROOT_TYPE_INVALID															= -1,
	HAIR_ROOT_TYPE_NONE																	= 0,
	HAIR_ROOT_TYPE_NULL																	= 7,
	HAIR_ROOT_TYPE_POLY																	= 2,
	HAIR_ROOT_TYPE_SEGMENT															= 3,
	HAIR_ROOT_TYPE_SPLINE2D															= 4,
	HAIR_ROOT_TYPE_SPLINE3D															= 5,
	HAIR_ROOT_TYPE_VERTEX																= 1,
	HAIR_VP_FLAG_NOHOOKS																= 1,
	HANDLE_BIG																					= 3,
	HANDLE_MIDDLE																				= 2,
	HANDLE_MINI																					= 0,
	HANDLE_SMALL																				= 1,
	HCLONE_ASIS																					= 1,
	HCLONE_ASLINE																				= 4,
	HCLONE_ASPOLY																				= 2,
	HCLONE_ASSPLINE																			= 8,
	HIGHLIGHT_FLAG_ACTIVEONLY														= 256,
	HIGHLIGHT_FLAG_ADDFLAGS															= -2147483647,
	HIGHLIGHT_FLAG_CALLCURSOR														= 16,
	HIGHLIGHT_FLAG_FREESELECT														= 2,
	HIGHLIGHT_FLAG_FULLMESH															= 8,
	HIGHLIGHT_FLAG_HIDE																	= 32,
	HIGHLIGHT_FLAG_NOEDITSTATE													= 4,
	HIGHLIGHT_FLAG_REALTIME															= 1,
	HIGHLIGHT_FLAG_SELECTIONS														= 64,
	HIGHLIGHT_FLAG_SNAPPING															= 128,
	HIGHLIGHT_TRANSPARENCY															= -140,
	HOTKEY_CAMERA_MOVE																	= 13563,
	HOTKEY_CAMERA_ROTATE																= 13565,
	HOTKEY_CAMERA_SCALE																	= 13564,
	HOTKEY_OBJECT_MOVE																	= 13566,
	HOTKEY_OBJECT_ROTATE																= 13568,
	HOTKEY_OBJECT_SCALE																	= 13567,
	HYPERLINK_ALIGN_LEFT																= 1751933292,
	HYPERLINK_ALIGN_RIGHT																= 1751933298,
	HYPERLINK_IS_LINK																		= 1751935340,
	HYPERLINK_LINK_DEST																	= 1751936100,
	HYPERLINK_LINK_TEXT																	= 1751936116,
	HYPERLINK_NO_UNDERLINE															= 1751936620,
	HierarchicalItemType																= 3,
	HighQuality																					= 4,
	ICONFLAG_COPY																				= 1,
	ICONFLAG_OBJECTBIT																	= 1073741824,
	ICONPOOL_FILENAME																		= 1851878757,
	ICONPOOL_ICON																				= 1768124270,
	ICON_H																							= 1751476583,
	ICON_ID																							= 1768188260,
	ICON_W																							= 2003072104,
	ICON_X																							= 2020634483,
	ICON_Y																							= 2037411699,
	IDC_CANCEL																					= 2,
	IDC_OK																							= 1,
	IDENTIFYFILE_COFFEE																	= 32,
	IDENTIFYFILE_IMAGE																	= 2,
	IDENTIFYFILE_LAYOUT																	= 128,
	IDENTIFYFILE_MOVIE																	= 4,
	IDENTIFYFILE_SCENE																	= 1,
	IDENTIFYFILE_SCRIPT																	= 16,
	IDENTIFYFILE_SKIPQUICKTIME													= 8,
	IDENTIFYFILE_SOUND																	= 64,
	IDM_CM_CLOSEWINDOW																	= 12097,
	IDM_COPY																						= 12107,
	IDM_CUT																							= 12106,
	IDM_DELETE																					= 12109,
	IDM_INVERSION																				= 12374,
	IDM_PASTE																						= 12108,
	IDM_REDO																						= 12297,
	IDM_SELECTALL																				= 12112,
	IDM_SELECTNONE																			= 12113,
	IDM_UNDO																						= 12105,
	ID_AUTOSWITCH_MODE																	= 1016118,
	ID_CAMERA_MAGNIFY																		= 1016008,
	ID_CAMERA_MOVE																			= 1016007,
	ID_CAMERA_ZOOM																			= 1016009,
	ID_CAMERA_ZOOM_IN																		= 1016010,
	ID_CAMERA_ZOOM_OUT																	= 1016011,
	ID_COFFEESCRIPT																			= 1001085,
	ID_CONVERT_SELECTION_TOOL														= 1011181,
	ID_EXPRESSION_PLUGIN																= 1001148,
	ID_EXPRESSION_TAG																		= 1001149,
	ID_EXTENDED_MODE																		= 1016116,
	ID_GRAPHVIEW																				= 1001100,
	ID_GRAPHVIEW_COLORS_PREFS														= 400005002,
	ID_GRAPHVIEW_GUI_PREFS															= 400005001,
	ID_GRAPHVIEW_MASTER_PREFS														= 400005004,
	ID_GRAPHVIEW_PREFS																	= 400005000,
	ID_GRAPHVIEW_PRIVATE																= 1001104,
	ID_GRAPHVIEW_WORLD_PREFS														= 400005003,
	ID_GV_DATA_DESCRIPTOR																= 1001109,
	ID_GV_GENERAL_OWNER																	= 1001141,
	ID_GV_GROUPDATA																			= 1001102,
	ID_GV_IGNORE_OWNER																	= 1001142,
	ID_GV_NODEDATA																			= 1001101,
	ID_GV_NODEMASTERDATA																= 1001103,
	ID_GV_OPCLASS_DESCRIPTOR														= 1001106,
	ID_GV_OPCLASS_TYPE_GENERAL													= 1001111,
	ID_GV_OPERATOR_DUMMY																= 1001143,
	ID_GV_OPERATOR_GROUP																= 1001144,
	ID_GV_OPGROUP_DESCRIPTOR														= 1001105,
	ID_GV_OPGROUP_TYPE_ADAPTER													= 1001113,
	ID_GV_OPGROUP_TYPE_BOOL															= 1001114,
	ID_GV_OPGROUP_TYPE_CALCULATE												= 1001115,
	ID_GV_OPGROUP_TYPE_GENERAL													= 1001112,
	ID_GV_OPGROUP_TYPE_ITERATOR													= 1001117,
	ID_GV_OPGROUP_TYPE_LOGICAL													= 1001116,
	ID_GV_SYSTEM_OWNER																	= 1001140,
	ID_GV_VALGROUP_DESCRIPTOR														= 1001107,
	ID_GV_VALGROUP_TYPE_GENERAL													= 1001110,
	ID_GV_VALUE_DESCRIPTOR															= 1001108,
	ID_GV_VALUE_TYPE_BOOL																= 400007001,
	ID_GV_VALUE_TYPE_GENERAL_INSTANCE										= 400007012,
	ID_GV_VALUE_TYPE_GENERAL_MATERIAL										= 400007011,
	ID_GV_VALUE_TYPE_GENERAL_OBJECT											= 400007009,
	ID_GV_VALUE_TYPE_GENERAL_TAG												= 400007010,
	ID_GV_VALUE_TYPE_INTEGER														= 400007002,
	ID_GV_VALUE_TYPE_MATRIX															= 400007006,
	ID_GV_VALUE_TYPE_NONE																= 0,
	ID_GV_VALUE_TYPE_NORMAL															= 400007005,
	ID_GV_VALUE_TYPE_REAL																= 400007003,
	ID_GV_VALUE_TYPE_STRING															= 400007008,
	ID_GV_VALUE_TYPE_TIME																= 400007007,
	ID_GV_VALUE_TYPE_UNDEFINED													= 400007000,
	ID_GV_VALUE_TYPE_VECTOR															= 400007004,
	ID_MEASURE_OBJECT																		= 200000082,
	ID_MEASURE_SCENEHOOK																= 200000079,
	ID_MEASURE_TOOL																			= 1011182,
	ID_MODELING_ARRANGE_TOOL														= 200000068,
	ID_MODELING_ARRAY_TOOL															= 450000045,
	ID_MODELING_AXIS																		= 200000087,
	ID_MODELING_BEVEL_TOOL															= 450000005,
	ID_MODELING_BRIDGE_TOOL															= 450000008,
	ID_MODELING_BRUSH_TOOL															= 1016202,
	ID_MODELING_CENTER_TOOL															= 200000071,
	ID_MODELING_CLONE_TOOL															= 450000042,
	ID_MODELING_CLOSEHOLE_TOOL													= 450000010,
	ID_MODELING_COLLAPSE_COMMAND												= 1015924,
	ID_MODELING_DUPLICATE_TOOL													= 200000067,
	ID_MODELING_EDGECUT_TOOL														= 1009661,
	ID_MODELING_EDGE_SPLINE_COMMAND											= 1009671,
	ID_MODELING_EXTRUDE_INNER_TOOL											= 450000004,
	ID_MODELING_EXTRUDE_TOOL														= 1011183,
	ID_MODELING_FILL_SELECTION_TOOL											= 1011178,
	ID_MODELING_FREESELECTION														= 200000085,
	ID_MODELING_INTERACTIVEMODELINGUNDO_COMMAND					= 200000061,
	ID_MODELING_INTERACTIVEMODELINGUNDO_RESTART_COMMAND = 200000060,
	ID_MODELING_IRON_TOOL																= 450000014,
	ID_MODELING_KNIFE_TOOL															= 1016030,
	ID_MODELING_LIVESELECTION														= 200000083,
	ID_MODELING_LOOP_TOOL																= 1011192,
	ID_MODELING_MAGNET_TOOL															= 1016185,
	ID_MODELING_MATRIX_EXTRUDE_TOOL											= 1011126,
	ID_MODELING_MELT_COMMAND														= 1010136,
	ID_MODELING_MODIFY_TOOL															= 200000074,
	ID_MODELING_MOVE																		= 200000088,
	ID_MODELING_NORMALMOVE_TOOL													= 450000000,
	ID_MODELING_NORMALROTATE_TOOL												= 450000002,
	ID_MODELING_NORMALSCALE_TOOL												= 450000001,
	ID_MODELING_OM_SELECT_HIDE													= 1016208,
	ID_MODELING_OM_SELECT_INVERT												= 1016207
};
enum
{
	ID_MODELING_OM_SELECT_SHOW				 = 1016209,
	ID_MODELING_OUTLINE_SELECTION_TOOL = 1011179,
	ID_MODELING_POINT_ADD_TOOL				 = 450000007,
	ID_MODELING_POLYGON_CREATE_TOOL		 = 450000006,
	ID_MODELING_POLYSELECTION					 = 200000086,
	ID_MODELING_RANDOMIZE_TOOL				 = 200000070,
	ID_MODELING_RECTSELECTION					 = 200000084,
	ID_MODELING_RING_TOOL							 = 1011180,
	ID_MODELING_ROTATE								 = 200000090,
	ID_MODELING_SCALE									 = 200000089,
	ID_MODELING_SETVALUE_TOOL					 = 200000065,
	ID_MODELING_SHORTCUT_SELECTVISIBLE = 1016757,
	ID_MODELING_SLIDE_TOOL						 = 450000012,
	ID_MODELING_SMOOTH_SHIFT_TOOL			 = 450000003,
	ID_MODELING_SOFTSELECTION					 = 1016699,
	ID_MODELING_SPLINE_CHAMFER_TOOL		 = 450000043,
	ID_MODELING_SPLINE_PROJECT_TOOL		 = 450000046,
	ID_MODELING_SPLINE_ROUND_TOOL			 = 450000039,
	ID_MODELING_STITCHANDSEW_TOOL			 = 450000011,
	ID_MODELING_TRANSFER_TOOL					 = 200000069,
	ID_MODELING_WELD_TOOL							 = 450000009,
	ID_NGON_FORCERETRI_MENU						 = 1016173,
	ID_NGON_REMOVE_MENU								 = 1016174,
	ID_NGON_RETRI_MENU								 = 1016160,
	ID_NORMAL_MODE										 = 1016117,
	ID_OBJECTHANDLES_ONOFF_MENU				 = 1016176,
	ID_OBJECT_PLUGIN									 = 1001146,
	ID_OPERATORCONTAINER							 = 1000,
	ID_OPERATORPOOL_PLUGIN						 = 1001138,
	ID_OPERATOR_ABS										 = 400001134,
	ID_OPERATOR_ADAPTER								 = 400001103,
	ID_OPERATOR_BITMAP								 = 450000037,
	ID_OPERATOR_BOOL									 = 400001122,
	ID_OPERATOR_BOX										 = 400001146,
	ID_OPERATOR_CLAMP									 = 400001113,
	ID_OPERATOR_CMP										 = 400001125,
	ID_OPERATOR_COFFEE								 = 400001154,
	ID_OPERATOR_COLLISION							 = 400001147,
	ID_OPERATOR_COLORSPACE						 = 400001142,
	ID_OPERATOR_COLORTEMP							 = 450000060,
	ID_OPERATOR_COMMENT								 = 400001129,
	ID_OPERATOR_CONDITION							 = 400001117,
	ID_OPERATOR_CONST									 = 400001120,
	ID_OPERATOR_CROSS									 = 400001104,
	ID_OPERATOR_DEGREE								 = 400001135,
	ID_OPERATOR_DETAILS								 = 400001152,
	ID_OPERATOR_DISTANCE							 = 400001116,
	ID_OPERATOR_DOT										 = 400001141,
	ID_OPERATOR_EQU										 = 400001105,
	ID_OPERATOR_FLOATFUNC							 = 400001139,
	ID_OPERATOR_FLOATMATH							 = 400001138,
	ID_OPERATOR_FORMULA								 = 400001133,
	ID_OPERATOR_FREEZE								 = 400001132,
	ID_OPERATOR_HIERARCHY							 = 400001151,
	ID_OPERATOR_INCLUDE								 = 400001159,
	ID_OPERATOR_INV										 = 400001123,
	ID_OPERATOR_ITERATE								 = 400001131,
	ID_OPERATOR_LINK									 = 400001160,
	ID_OPERATOR_MATERIAL							 = 400001158,
	ID_OPERATOR_MATH									 = 400001121,
	ID_OPERATOR_MATRIX2VECT						 = 400001109,
	ID_OPERATOR_MATRIXCALCHPB					 = 400001115,
	ID_OPERATOR_MATRIXMULVECTOR				 = 400001149,
	ID_OPERATOR_MEMORY								 = 400001130,
	ID_OPERATOR_MIX										 = 400001128,
	ID_OPERATOR_MONOFLOP							 = 400001155,
	ID_OPERATOR_NEG										 = 400001124,
	ID_OPERATOR_NIL										 = 400001106,
	ID_OPERATOR_NOISE									 = 400001144,
	ID_OPERATOR_NOT										 = 400001111,
	ID_OPERATOR_OBJECT								 = 400001000,
	ID_OPERATOR_ORDER									 = 400001126,
	ID_OPERATOR_POINT									 = 400001112,
	ID_OPERATOR_POLYGON								 = 400001140,
	ID_OPERATOR_RANDOM								 = 400001100,
	ID_OPERATOR_RANGEMAPPER						 = 400001136,
	ID_OPERATOR_RAY										 = 400001161,
	ID_OPERATOR_REAL2VECT							 = 400001101,
	ID_OPERATOR_REFERENCE							 = 400001156,
	ID_OPERATOR_RESULT								 = 400001118,
	ID_OPERATOR_SELECTION							 = 400001150,
	ID_OPERATOR_SOUND									 = 400001143,
	ID_OPERATOR_SPLINE								 = 400001148,
	ID_OPERATOR_SPY										 = 400001119,
	ID_OPERATOR_SWITCH								 = 400001127,
	ID_OPERATOR_TAG										 = 400001153,
	ID_OPERATOR_TIME									 = 400001107,
	ID_OPERATOR_TRIGGER								 = 400001145,
	ID_OPERATOR_TRIGO									 = 400001108,
	ID_OPERATOR_VECT2MATRIX						 = 400001110,
	ID_OPERATOR_VECT2REAL							 = 400001102,
	ID_OPERATOR_VECTCALCMATRIX				 = 400001114,
	ID_OPERATOR_WEIGHTMAP							 = 400001157,
	ID_RENDERDATA											 = 110304,
	ID_SCENEHOOK_PLUGIN								 = 1001147,
	ID_SDS_DEC_MENU										 = 1016178,
	ID_SDS_INC_MENU										 = 1016177,
	ID_SDS_ONFFF_MENU									 = 1016179,
	ID_SHAPECONTAINER									 = 1001,
	ID_SNAP_BASE											 = 200000093,
	ID_THINKINGCOMMAND								 = 1001439,
	ID_THINKINGPARTICLES							 = 1001428,
	ID_TOOL_BASE											 = 200000064,
	ID_TP_DATA_TYPE_GROUP							 = 1001399,
	ID_TP_DATA_TYPE_PARTICLE					 = 1001395,
	ID_TP_DATA_TYPE_SHAPE							 = 1001397,
	ID_TP_DATA_TYPE_SPIN							 = 1001434,
	ID_TP_HP_CHRONOMETER							 = 1001430,
	ID_TP_HP_PARTICLEDATA							 = 1001403,
	ID_TP_HP_PGROUP										 = 1001409,
	ID_TP_HP_SPINCONVERT							 = 1001408,
	ID_TP_HP_SURFACEPOSITION					 = 1001440,
	ID_TP_HP_TIMER										 = 1001429,
	ID_TP_HP_VELOCITYCONVERT					 = 1001410,
	ID_TP_HP_VOLUMEPOSITION						 = 1001441,
	ID_TP_IF_LIGHT										 = 1001438,
	ID_TP_IF_PARTICLEAGE							 = 1001406,
	ID_TP_IN_INTERACT									 = 1001405,
	ID_TP_IN_SELFDYNAMIC							 = 1001435,
	ID_TP_OPCLASS											 = 1001385,
	ID_TP_OPGROUP_CONDITION						 = 1001386,
	ID_TP_OPGROUP_DYNAMIC							 = 1001391,
	ID_TP_OPGROUP_GENERATOR						 = 1001389,
	ID_TP_OPGROUP_HELPER							 = 1001387,
	ID_TP_OPGROUP_INITIATOR						 = 1001388,
	ID_TP_OPGROUP_STANDARD						 = 1001390,
	ID_TP_OP_ALIGNMENT								 = 1001416,
	ID_TP_OP_BLURP										 = 1001444,
	ID_TP_OP_BORN											 = 1001442,
	ID_TP_OP_BUBBLE										 = 1001424,
	ID_TP_OP_DEFLECTOR								 = 1001411,
	ID_TP_OP_DIE											 = 1001425,
	ID_TP_OP_FRAGMENT									 = 1001431,
	ID_TP_OP_FREEZE										 = 1001432,
	ID_TP_OP_FRICTION									 = 1001421,
	ID_TP_OP_GRAVITY									 = 1001426,
	ID_TP_OP_MASS											 = 1001418,
	ID_TP_OP_MATTERWAVES							 = 1001447,
	ID_TP_OP_MOTIONINHERI							 = 1001423,
	ID_TP_OP_OBJECTSHAPE							 = 1001413,
	ID_TP_OP_PARTICLEDATA							 = 1001404,
	ID_TP_OP_PARTICLEDRAW							 = 1001445,
	ID_TP_OP_PARTICLEDRAWTOOL					 = 1001446,
	ID_TP_OP_PGROUP										 = 1001417,
	ID_TP_OP_POSFOLLOW								 = 1001412,
	ID_TP_OP_REPULSEBOUNCE						 = 1001436,
	ID_TP_OP_ROLLING									 = 1001437,
	ID_TP_OP_SCALE										 = 1001420,
	ID_TP_OP_SIZE											 = 1001419,
	ID_TP_OP_SPIN											 = 1001415,
	ID_TP_OP_STORM										 = 1001382,
	ID_TP_OP_VELOCITY									 = 1001422,
	ID_TP_OP_WIND											 = 1001427,
	ID_TP_PARTICLEGEOMETRY						 = 1001414,
	ID_TP_PGROUP											 = 1001381,
	ID_TP_REGISTER										 = 1001443,
	ID_TP_VALUE_TYPE_GROUP						 = 1001398,
	ID_TP_VALUE_TYPE_PARTICLE					 = 1001394,
	ID_TP_VALUE_TYPE_SHAPE						 = 1001396,
	ID_TP_VALUE_TYPE_SPIN							 = 1001433,
	ID_TREEVIEW_CONTEXT_REMOVE				 = 900000,
	ID_TREEVIEW_CONTEXT_RESET					 = 900001,
	ID_TREEVIEW_FIRST_NEW_ID					 = 900010,
	ID_TREEVIEW_PLUGIN								 = 1001145,
	ID_TWEAK_MODE											 = 1016114,
	ID_USERDATA												 = 700,
	ID_VIEW_SCENEHOOKHIGHLIGHT				 = 200000078,
	ID_VIEW_SCENEHOOKHUD							 = 1011173,
	IKEXPRESSIONTAG_NAME							 = 1000,
	ILLUMINATE_SHADOW_NOENVIRONMENT		 = 2,
	ILLUMINATE_SHADOW_OFF							 = 0,
	ILLUMINATE_SHADOW_ON							 = 1,
	IMAGESAVER_DPI										 = 10022,
	INDEX_GV_CLASS_TYPE_INVALID				 = -1,
	INDEX_GV_CLASS_TYPE_UNDEFINED			 = 0,
	INDEX_GV_DATA_TYPE_INVALID				 = -1,
	INDEX_GV_DATA_TYPE_UNDEFINED			 = 0,
	INDEX_GV_GROUP_TYPE_INVALID				 = -1,
	INDEX_GV_GROUP_TYPE_UNDEFINED			 = 0,
	INDEX_GV_VALUE_TYPE_INVALID				 = -1,
	INITRENDERFLAG_NOMIP							 = 4,
	INITRENDERFLAG_PAINTERNOMIP				 = 2,
	INITRENDERFLAG_PREVIEWRENDER			 = 8,
	INITRENDERFLAG_TEXTURES						 = 1,
	INIT_DISPLACEMENT									 = 3,
	INIT_SURFACE											 = 0,
	INIT_TRANSPARENCY									 = 1,
	INPUT_DBLCLK											 = 32768,
	INSERT_AFTER											 = 2,
	INSERT_BEFORE											 = 1,
	INSERT_UNDER											 = 4,
	INSTANCEOBJECT_SOURCE							 = 999,
	//	IN_EXCLUDE_DATA_SELECTION					 = 10,
	//  IN_EXCLUDE_FLAG_IMAGE_01_OFF			 = 2002,
	//  IN_EXCLUDE_FLAG_IMAGE_01_ON				 = 2001,
	//  IN_EXCLUDE_FLAG_IMAGE_02_OFF			 = 2004,
	//  IN_EXCLUDE_FLAG_IMAGE_02_ON				 = 2003,
	//  IN_EXCLUDE_FLAG_IMAGE_03_OFF			 = 2006,
	//  IN_EXCLUDE_FLAG_IMAGE_03_ON				 = 2005,
	//  IN_EXCLUDE_FLAG_IMAGE_04_OFF			 = 2008,
	//  IN_EXCLUDE_FLAG_IMAGE_04_ON				 = 2007,
	//  IN_EXCLUDE_FLAG_IMAGE_05_OFF			 = 2010,
	//  IN_EXCLUDE_FLAG_IMAGE_05_ON				 = 2009,
	//  IN_EXCLUDE_FLAG_IMAGE_06_OFF			 = 2012,
	//  IN_EXCLUDE_FLAG_IMAGE_06_ON				 = 2011,
	//  IN_EXCLUDE_FLAG_IMAGE_07_OFF			 = 2014,
	//  IN_EXCLUDE_FLAG_IMAGE_07_ON				 = 2013,
	//  IN_EXCLUDE_FLAG_IMAGE_08_OFF			 = 2016,
	//  IN_EXCLUDE_FLAG_IMAGE_08_ON				 = 2015,
	//  IN_EXCLUDE_FLAG_IMAGE_09_OFF			 = 2018,
	//  IN_EXCLUDE_FLAG_IMAGE_09_ON				 = 2017,
	//  IN_EXCLUDE_FLAG_IMAGE_10_OFF			 = 2020,
	//  IN_EXCLUDE_FLAG_IMAGE_10_ON				 = 2019,
	//  IN_EXCLUDE_FLAG_IMAGE_11_OFF			 = 2022,
	//  IN_EXCLUDE_FLAG_IMAGE_11_ON				 = 2021,
	//  IN_EXCLUDE_FLAG_IMAGE_12_OFF			 = 2024,
	//  IN_EXCLUDE_FLAG_IMAGE_12_ON				 = 2023,
	//  IN_EXCLUDE_FLAG_IMAGE_13_OFF			 = 2026,
	//  IN_EXCLUDE_FLAG_IMAGE_13_ON				 = 2025,
	//  IN_EXCLUDE_FLAG_IMAGE_14_OFF			 = 2028,
	//  IN_EXCLUDE_FLAG_IMAGE_14_ON				 = 2027,
	//  IN_EXCLUDE_FLAG_IMAGE_15_OFF			 = 2030,
	//  IN_EXCLUDE_FLAG_IMAGE_15_ON				 = 2029,
	//  IN_EXCLUDE_FLAG_IMAGE_16_OFF			 = 2032,
	//  IN_EXCLUDE_FLAG_IMAGE_16_ON				 = 2031,
	//  IN_EXCLUDE_FLAG_IMAGE_17_OFF			 = 2034,
	//  IN_EXCLUDE_FLAG_IMAGE_17_ON				 = 2033,
	//  IN_EXCLUDE_FLAG_IMAGE_18_OFF			 = 2036,
	//  IN_EXCLUDE_FLAG_IMAGE_18_ON				 = 2035,
	//  IN_EXCLUDE_FLAG_IMAGE_19_OFF			 = 2038,
	//  IN_EXCLUDE_FLAG_IMAGE_19_ON				 = 2037,
	//  IN_EXCLUDE_FLAG_IMAGE_20_OFF			 = 2040,
	//  IN_EXCLUDE_FLAG_IMAGE_20_ON				 = 2039,
	//  IN_EXCLUDE_FLAG_IMAGE_21_OFF			 = 2042,
	//  IN_EXCLUDE_FLAG_IMAGE_21_ON				 = 2041,
	//  IN_EXCLUDE_FLAG_IMAGE_22_OFF			 = 2044,
	//  IN_EXCLUDE_FLAG_IMAGE_22_ON				 = 2043,
	//  IN_EXCLUDE_FLAG_IMAGE_23_OFF			 = 2046,
	//  IN_EXCLUDE_FLAG_IMAGE_23_ON				 = 2045,
	//  IN_EXCLUDE_FLAG_IMAGE_24_OFF			 = 2048,
	//  IN_EXCLUDE_FLAG_IMAGE_24_ON				 = 2047,
	//  IN_EXCLUDE_FLAG_IMAGE_25_OFF			 = 2050,
	//  IN_EXCLUDE_FLAG_IMAGE_25_ON				 = 2049,
	//  IN_EXCLUDE_FLAG_IMAGE_26_OFF			 = 2052,
	//  IN_EXCLUDE_FLAG_IMAGE_26_ON				 = 2051,
	//  IN_EXCLUDE_FLAG_IMAGE_27_OFF			 = 2054,
	//  IN_EXCLUDE_FLAG_IMAGE_27_ON				 = 2053,
	//  IN_EXCLUDE_FLAG_IMAGE_28_OFF			 = 2056,
	//  IN_EXCLUDE_FLAG_IMAGE_28_ON				 = 2055,
	//  IN_EXCLUDE_FLAG_IMAGE_29_OFF			 = 2058,
	//  IN_EXCLUDE_FLAG_IMAGE_29_ON				 = 2057,
	//  IN_EXCLUDE_FLAG_IMAGE_30_OFF			 = 2060,
	//  IN_EXCLUDE_FLAG_IMAGE_30_ON				 = 2059,
	//  IN_EXCLUDE_FLAG_IMAGE_31_OFF			 = 2062,
	//  IN_EXCLUDE_FLAG_IMAGE_31_ON				 = 2061,
	//  IN_EXCLUDE_FLAG_IMAGE_32_OFF			 = 2064,
	//  IN_EXCLUDE_FLAG_IMAGE_32_ON				 = 2063,
	//	IN_EXCLUDE_FLAG_INIT_STATE				 = 2065
};
enum
{
	//	IN_EXCLUDE_FLAG_NUM_FLAGS					 = 2000,
	//	IN_EXCLUDE_FLAG_SEND_SELCHANGE_MSG = 2066,
	IS_DEGREE													= 1,
	IS_LONG														= 5,
	IS_METER													= 3,
	IS_NOTHING												= 0,
	IS_PERCENT												= 2,
	Iadaptive													= 3,
	IconFolderOpen										= 1,
	IconNone													= 0,
	IconPreviewBitmap									= 32768,
	ImageFile													= 10,
	Inatural													= 1,
	InfoTab														= 1,
	Inone															= 0,
	InvalidPresetDb										= -1,
	Isubdiv														= 4,
	Iuniform													= 2,
	JPGSAVER_QUALITY									= 10021,
	KEYFLAGS_CLAMP										= 2,
	KEYFLAGS_LINKTANGENTS							= 1,
	KEYINTERPOLATION_CUSTOM						= 0,
	KEYINTERPOLATION_FAST							= 5,
	KEYINTERPOLATION_LINEAR						= 2,
	KEYINTERPOLATION_SLOW							= 4,
	KEYINTERPOLATION_SLOWSLOW					= 6,
	KEYINTERPOLATION_SOFT							= 1,
	KEYINTERPOLATION_STEP							= 3,
	KEY_ALT														= 61458,
	KEY_BACKSPACE											= 32784,
	KEY_CONTROL												= 61457,
	KEY_DELETE												= 32777,
	KEY_DOWN													= 32769,
	KEY_END														= 32775,
	KEY_ENTER													= 32785,
	KEY_ESC														= 32805,
	KEY_F10														= 32802,
	KEY_F11														= 32803,
	KEY_F12														= 32804,
	KEY_F1														= 32787,
	KEY_F2														= 32788,
	KEY_F3														= 32789,
	KEY_F4														= 32790,
	KEY_F5														= 32791,
	KEY_F6														= 32792,
	KEY_F7														= 32793,
	KEY_F8														= 32800,
	KEY_F9														= 32801,
	KEY_HOME													= 32774,
	KEY_INSERT												= 32776,
	KEY_LEFT													= 32771,
	KEY_MLEFT													= 61440,
	KEY_MMIDDLE												= 61442,
	KEY_MRIGHT												= 61441,
	KEY_MX1														= 61443,
	KEY_MX2														= 61444,
	KEY_PGDOWN												= 32773,
	KEY_PGUP													= 32772,
	KEY_RIGHT													= 32770,
	KEY_SHIFT													= 61456,
	KEY_SPACE													= 32806,
	KEY_TAB														= 32786,
	KEY_UP														= 32768,
	KEbase														= 5522,
	KINEMATICKEY_NAME									= 1,
	KINEMATICTAG_DAMP_W								= 1005,
	KINEMATICTAG_MAX_W								= 1004,
	KINEMATICTAG_MIN_W								= 1003,
	KeepResident											= 1073741824,
	LAYERSETMODE_ALPHAS								= 2,
	LAYERSETMODE_LAYERALPHA						= 3,
	LAYERSETMODE_LAYERMASKS						= 1,
	LAYERSETMODE_LAYERS								= 0,
	LAYOUTMODE_MAXIMIZED							= 2,
	LAYOUTMODE_MINIMIZED							= 1,
	LAYOUTMODE_NONE										= 0,
	LIBRARY_ACTIVEOBJECTMANAGER				= 1000473,
	LIBRARY_DESCRIPTIONLIB						= 1000467,
	LIBRARY_EDITORTOOLS								= 1000465,
	LIBRARY_HAIR											= 1018173,
	LIBRARY_HAIR_COLLIDER							= 1018174,
	LIBRARY_HIGHLIGHT									= 1016132,
	LIBRARY_HUD												= 1016073,
	LIBRARY_ICON_COLLECTION						= 1009310,
	LIBRARY_MODELLING									= 1015814,
	LIBRARY_NGON											= 1015903,
	LIBRARY_OBJECTLIST								= 1000464,
	LIBRARY_PREFS											= 1000466,
	LIBRARY_SDS												= 1016122,
	LIBRARY_SN												= 200000008,
	LIBRARY_SNAPPING									= 1015841,
	LIBRARY_ZIPFILE										= 1011188,
	LIB_CLIPMAP												= 1000494,
	LIB_CLIPMAPRGN										= 1000493,
	LIB_COLLIDERCACHE									= 1000487,
	LIB_COLLIDERENGINE								= 1000488,
	LIB_RAYCOLLIDER										= 1000499,
	LIB_SND														= 1000496,
	LIGHTRESTRICTION_NODIFFUSE				= 1,
	LIGHTRESTRICTION_NOSHADOW					= 4,
	LIGHTRESTRICTION_NOSPECULAR				= 2,
	LIGHT_GLOW_COL										= 1,
	LIGHT_GLOW_SIZE										= 1,
	LIGHT_GLOW_SV											= 1,
	LIGHT_GLOW_TYP										= 1,
	LIGHT_HALO_COL										= 1,
	LIGHT_HALO_SIZE										= 1,
	LIGHT_HALO_SV											= 1,
	LIGHT_HALO_TYP										= 1,
	LIGHT_LENS_TYP										= 10003,
	LIGHT_LFSTRUCT_FADEBEHIND					= 80008,
	LIGHT_LFSTRUCT_FADENEAR						= 80009,
	LIGHT_LFSTRUCT_FADEOFFSCREEN			= 80010,
	LIGHT_LFSTRUCT_GLOWHELL						= 80002,
	LIGHT_LFSTRUCT_GLOWSV							= 80000,
	LIGHT_LFSTRUCT_LENSANZ						= 80013,
	LIGHT_LFSTRUCT_LENSGLOW						= 80014,
	LIGHT_LFSTRUCT_LENSREFL						= 80015,
	LIGHT_LFSTRUCT_LIGHTPARAM					= 80007,
	LIGHT_LFSTRUCT_REFERENCE					= 80006,
	LIGHT_LFSTRUCT_REFLHELL						= 80003,
	LIGHT_LFSTRUCT_REFLSV							= 80001,
	LIGHT_LFSTRUCT_ROTATION						= 80005,
	LIGHT_LFSTRUCT_SCALE							= 80004,
	LIGHT_LFSTRUCT_SCALEGLOW					= 80011,
	LIGHT_LFSTRUCT_SCALEREFLEXES			= 80012,
	LIGHT_MAIN_AMBIENT								= 90015,
	LIGHT_MAIN_ASPECTRATIO						= 90022,
	LIGHT_MAIN_BRIGHTNESS							= 90023,
	LIGHT_MAIN_COLFALLOFF							= 90020,
	LIGHT_MAIN_CONTRAST								= 90021,
	LIGHT_MAIN_DARK										= 90009,
	LIGHT_MAIN_F											= 90000,
	LIGHT_MAIN_FALLOFF								= 90014,
	LIGHT_MAIN_FARCLIP								= 90028,
	LIGHT_MAIN_FARFROM								= 90031,
	LIGHT_MAIN_FARTO									= 90032,
	LIGHT_MAIN_HELL										= 90001,
	LIGHT_MAIN_ICOL										= 90026,
	LIGHT_MAIN_INNERANGLE							= 90010,
	LIGHT_MAIN_INNERCOLOR							= 90019,
	LIGHT_MAIN_INNERCONE							= 90018,
	LIGHT_MAIN_INNERDIST							= 90024,
	LIGHT_MAIN_INNERRADIUS						= 90012,
	LIGHT_MAIN_NEARCLIP								= 90027,
	LIGHT_MAIN_NEARFROM								= 90029,
	LIGHT_MAIN_NEARTO									= 90030,
	LIGHT_MAIN_NODIFFUSE							= 90016,
	LIGHT_MAIN_NOSPECULAR							= 90017,
	LIGHT_MAIN_NTYPE									= 90005,
	LIGHT_MAIN_OUTERANGLE							= 90011,
	LIGHT_MAIN_OUTERDIST							= 90025,
	LIGHT_MAIN_OUTERRADIUS						= 90013,
	LIGHT_MAIN_SHOWCLIP								= 90008,
	LIGHT_MAIN_SHOWLI									= 90006,
	LIGHT_MAIN_SHOWVL									= 90007,
	LIGHT_MAIN_STYPE									= 90003,
	LIGHT_MAIN_TYPE										= 90002,
	LIGHT_MAIN_VTYPE									= 90004,
	LIGHT_SHADOW_AREARADIUS						= 50004,
	LIGHT_SHADOW_BIAS									= 50016,
	LIGHT_SHADOW_CONE									= 50011,
	LIGHT_SHADOW_PARRADIUS						= 50002,
	LIGHT_SHADOW_SMPRADIUS						= 50007,
	LIGHT_STREAK_ANZ									= 1,
	LIGHT_STREAK_COL									= 1,
	LIGHT_STREAK_DICKE								= 1,
	LIGHT_STREAK_HOLEANZ							= 1,
	LIGHT_STREAK_HOLEWIDTH						= 1,
	LIGHT_STREAK_RANDOM								= 1,
	LIGHT_STREAK_RANDOMLEN						= 1,
	LIGHT_STREAK_ROT									= 1,
	LIGHT_STREAK_SIZE									= 1,
	LIGHT_STREAK_STERN								= 1,
	LIGHT_STREAK_SV										= 1,
	LIGHT_STREAK_TYP									= 1,
	LIGHT_VLSTRUCT_ADDITIVE						= 70022,
	LIGHT_VLSTRUCT_ANGLESCALE					= 70023,
	LIGHT_VLSTRUCT_BRIGHTNESS					= 70019,
	LIGHT_VLSTRUCT_COLFALLOFF					= 70003,
	LIGHT_VLSTRUCT_CUSTOMCOL					= 70000,
	LIGHT_VLSTRUCT_DUST								= 70021,
	LIGHT_VLSTRUCT_EDGEFALLOFF				= 70002,
	LIGHT_VLSTRUCT_EDGESTRENGTH				= 70005,
	LIGHT_VLSTRUCT_FALLOFF						= 70001,
	LIGHT_VLSTRUCT_ICOL								= 70007,
	LIGHT_VLSTRUCT_INNERDIST					= 70009,
	LIGHT_VLSTRUCT_JITTER							= 70006,
	LIGHT_VLSTRUCT_OCOL								= 70008,
	LIGHT_VLSTRUCT_OUTERDIST					= 1,
	LIGHT_VLSTRUCT_OUTERDISTY					= 1,
	LIGHT_VLSTRUCT_OUTERDISTZ					= 1,
	LIGHT_VLSTRUCT_SAMPLEDIST					= 70020,
	LIGHT_VLSTRUCT_STRENGTH						= 70004,
	LIGHT_VNSTRUCT_AMPLITUDE					= 60004,
	LIGHT_VNSTRUCT_LOCAL							= 60008,
	LIGHT_VNSTRUCT_NOISE							= 60000,
	LIGHT_VNSTRUCT_OCTAVES						= 60001,
	LIGHT_VNSTRUCT_OFFSET							= 60006,
	LIGHT_VNSTRUCT_RSCALE							= 60003,
	LIGHT_VNSTRUCT_VELOCITY						= 60002,
	LIGHT_VNSTRUCT_VSCALE							= 60009,
	LIGHT_VNSTRUCT_WIND								= 60007,
	LIGHT_VNSTRUCT_WINDVELOCITY				= 60005,
	LINKBOX_ACCEPT_MESSAGE_ACCEPT			= 3,
	LINKBOX_ACCEPT_MESSAGE_CONTROL_ID = 4,
	LINKBOX_ACCEPT_MESSAGE_ELEMENT		= 2,
	LINKBOX_ACCEPT_MESSAGE_TYPE				= 1,
	LINKBOX_HIDE_ICON									= 10000,
	LMOBJECT_DFACTOR									= 1009,
	LMOBJECT_DOPPLER									= 1008,
	LOADTEXTURE_FILENAME							= 1,
	LOAD_DOUBLENAME										= -105,
	LOAD_NOMEM												= -100,
	LOAD_NOTFOUND											= -101,
	LOAD_OK														= 0,
	LOAD_UNKNOWN											= -102,
	LONG_LAT_LONGITUDE								= 1000,
	LV_ACTION													= 51,
	LV_DATACHANGED										= 54,
	LV_DESTROYLISTVIEW								= 11
};
enum
{
	LV_DRAGRECEIVE						 = 56,
	LV_GETCOLSPACE						 = 8,
	LV_GETCOLUMDATA						 = 6,
	LV_GETCOLUMNCOUNT					 = 2,
	LV_GETCOLUMNWIDTH					 = 4,
	LV_GETCOLUMTYPE						 = 5,
	LV_GETFIXEDLAYOUT					 = 10,
	LV_GETLINECOUNT						 = 1,
	LV_GETLINEHEIGHT					 = 3,
	LV_GETLINESELECTED				 = 7,
	LV_GETLINESPACE						 = 9,
	LV_INITCACHE							 = 12,
	LV_LMOUSEDOWN							 = 50,
	LV_REDRAW									 = 53,
	LV_RES_INT								 = 1819242087,
	LV_RES_STRING							 = 1937011303,
	LV_SHOWLINE								 = 55,
	LV_SIMPLE_BUTTONCLICK			 = 103,
	LV_SIMPLE_CHECKBOXCHANGED	 = 101,
	LV_SIMPLE_COL_ID					 = 2,
	LV_SIMPLE_DATA						 = 3,
	LV_SIMPLE_DOUBLECLICK			 = 104,
	LV_SIMPLE_FOCUSITEM				 = 102,
	LV_SIMPLE_FOCUSITEM_NC		 = 105,
	LV_SIMPLE_ITEM_ID					 = 1,
	LV_SIMPLE_SELECTIONCHANGED = 100,
	LV_TREE										 = 1953654117,
	LV_USER										 = 1970496882,
	LV_USERDRAW								 = 52,
	LV_USERTREE								 = 1970500722,
	LV__											 = 0,
	LanguageCZ								 = 1668939776,
	LanguageDE								 = 1684340736,
	LanguageDefault						 = 0,
	LanguageES								 = 1702035456,
	LanguageFR								 = 1718747136,
	LanguageIT								 = 1769209856,
	LanguageJP								 = 1785724928,
	LanguagePL								 = 1886126080,
	LanguageUS								 = 1970470912,
	LazyIdentification				 = 2,
	Less											 = 2,
	LessOrEqual								 = 3,
	Link											 = 8,
	LinkThrough								 = -2147483647,
	LowQuality								 = 1,
	MACCREATOR_CINEMA					 = 1127498801,
	MACTYPE_CINEMA						 = 1127498819,
	MANAGER_CLOSEBUTTON				 = 3,
	MANAGER_DUMMY							 = 10,
	MANAGER_ID								 = 6,
	MANAGER_MENUID						 = 7,
	MANAGER_NOBLITBIT					 = 8,
	MANAGER_NONE							 = 0,
	MANAGER_SCROLLER_HORIZ		 = 1,
	MANAGER_SCROLLER_VERT			 = 2,
	MANAGER_SIZEABLE					 = 4,
	MANAGER_TITLE							 = 5,
	MANAGER_WANTOWNSTATUSBAR	 = 9,
	MARKERKEY_NAME						 = 1,
	MASK_TILE_MODE						 = 15,
	MASK_TILE_TYPE						 = 65280,
	MATASSIGN_CONTAINER_ID		 = 1001,
	MATASSIGN_MATERIAL_ID			 = 1002,
	MATASSIGN_TAG_ID					 = 1000,
	MATERIALGROUP_ID					 = 100,
	MATERIALGROUP_NAME				 = 101,
	MATERIALGROUP_SELECTED		 = 102,
	MATERIAL_CHANGED					 = -1004,
	//	MATPREVIEW_COPY_USERDATA						= 450000030,
	//	MATPREVIEW_DRAGNDROP_END						= 450000064,
	//	MATPREVIEW_DRAGNDROP_RECV						= 450000062,
	//	MATPREVIEW_DRAGNDROP_START					= 450000063,
	//	MATPREVIEW_FLAG_ALLOW_DRAGNDROP			= 64,
	//  MATPREVIEW_FLAG_HIDE_ANIMATE				= 4,
	//  MATPREVIEW_FLAG_HIDE_OPEN						= 16,
	//  MATPREVIEW_FLAG_HIDE_ROTATION				= 1,
	//  MATPREVIEW_FLAG_HIDE_SCENES					= 2,
	//  MATPREVIEW_FLAG_HIDE_SCENE_SETTINGS = 32,
	//  MATPREVIEW_FLAG_HIDE_SIZE						= 8,
	//	MATPREVIEW_FREE_USERDATA						= 450000029,
	//	MATPREVIEW_GENERATE_IMAGE						= 1011148,
	//	MATPREVIEW_GET_DIRTY_COUNT					= 450000065,
	//  MATPREVIEW_GET_OBJECT_INFO					= 1011147,
	//  MATPREVIEW_GET_POPUP_OPTIONS				= 1011141,
	//  MATPREVIEW_GET_PREVIEW_ID						= 1011143,
	//  MATPREVIEW_HANDLE_POPUP_MSG					= 1011144,
	//  MATPREVIEW_MIN_HEIGHT								= 1001,
	//  MATPREVIEW_MIN_WIDTH								= 1000,
	//  MATPREVIEW_MODIFY_CACHE_SCENE				= 1011149,
	//  MATPREVIEW_NO_ANIMATION							= 1004,
	//  MATPREVIEW_NO_BORDER								= 1003,
	//  MATPREVIEW_NO_FLOATWINDOW						= 1005,
	//  MATPREVIEW_NO_SCENESETTINGS					= 1006,
	//  MATPREVIEW_NO_SIZE_POPUP						= 1002,
	//  MATPREVIEW_POPUP_NAME								= 99999,
	//  MATPREVIEW_PREPARE_SCENE						= 1011142,
	//  MATPREVIEW_RECTSELECT								= 1007,
	//MAX_MATERIALCHANNELS=14,
	//MAXELEMENTS=100000000,
	//MAXGLOW_CNT=5,
	//MAXSTREAK_CNT=5,
	MAXTIME																	 = 10000,
	MAXTRIX_MAX_COLUMNS											 = 15,
	MAXTRIX_MAX_ROWS												 = 15,
	MAX_GLOBAL_TEXTURE_PATHS								 = 10,
	MAX_GLOW_COUNT													 = 5,
	MAX_LENS_COUNT													 = 40,
	MAX_STREAK_COUNT												 = 5,
	MAX_THREADS															 = 16,
	MAX_Z																		 = 1000000,
	MCB_NAVIGATIONCHANGED										 = 1851881059,
	MCOMMAND_ALIGNNORMALS										 = 218,
	MCOMMAND_BREAKPHONG											 = 255,
	MCOMMAND_CONVERTSELECTION								 = 247,
	MCOMMAND_CURRENTSTATETOOBJECT						 = 245,
	MCOMMAND_DELETE													 = 224,
	MCOMMAND_DESELECTALL										 = 201,
	MCOMMAND_DISCONNECT											 = 228,
	MCOMMAND_EDGE_TO_SPLINE									 = 251,
	MCOMMAND_EXPLODESEGMENTS								 = 243,
	MCOMMAND_GENERATESELECTION							 = 209,
	MCOMMAND_HIDEINVERT											 = 213,
	MCOMMAND_HIDESELECTED										 = 211,
	MCOMMAND_HIDEUNSELECTED									 = 212,
	MCOMMAND_JOIN														 = 246,
	MCOMMAND_KNIFE													 = 244,
	MCOMMAND_MAKEEDITABLE										 = 230,
	MCOMMAND_MATRIXEXTRUDE									 = 238,
	MCOMMAND_MELT														 = 264,
	MCOMMAND_MIRROR													 = 237,
	MCOMMAND_OPTIMIZE												 = 227,
	MCOMMAND_PHONGTOSELECTION								 = 257,
	MCOMMAND_REVERSENORMALS									 = 217,
	MCOMMAND_SELECTADJACENT									 = 208,
	MCOMMAND_SELECTALL											 = 200,
	MCOMMAND_SELECTCONNECTED								 = 203,
	MCOMMAND_SELECTGROW											 = 204,
	MCOMMAND_SELECTINVERSE									 = 202,
	MCOMMAND_SELECTPOINTTOPOLY							 = 206,
	MCOMMAND_SELECTPOLYTOPOINT							 = 207,
	MCOMMAND_SELECTSHRINK										 = 205,
	MCOMMAND_SMOOTHSHIFT										 = 450000003,
	MCOMMAND_SPLINE_ADDPOINT								 = 116,
	MCOMMAND_SPLINE_BREAKSEGMENT						 = 110,
	MCOMMAND_SPLINE_CREATEOUTLINE						 = 114,
	MCOMMAND_SPLINE_EQUALDIRECTION					 = 112,
	MCOMMAND_SPLINE_EQUALLENGTH							 = 111,
	MCOMMAND_SPLINE_HARDINTERPOLATION				 = 100,
	MCOMMAND_SPLINE_JOINSEGMENT							 = 109,
	MCOMMAND_SPLINE_LINEUP									 = 113,
	MCOMMAND_SPLINE_MOVEDOWN								 = 104,
	MCOMMAND_SPLINE_MOVEUP									 = 105,
	MCOMMAND_SPLINE_PROJECT									 = 115,
	MCOMMAND_SPLINE_REORDER									 = 102,
	MCOMMAND_SPLINE_REVERSE									 = 103,
	MCOMMAND_SPLINE_SOFTINTERPOLATION				 = 101,
	MCOMMAND_SPLIT													 = 220,
	MCOMMAND_SUBDIVIDE											 = 242,
	MCOMMAND_TRIANGULATE										 = 221,
	MCOMMAND_UNBREAKPHONG										 = 256,
	MCOMMAND_UNHIDE													 = 214,
	MCOMMAND_UNTRIANGULATE									 = 222,
	MDATA_BRIDGE_TRIANGULATE								 = 2066,
	MDATA_CONVERTSELECTION_LEFT							 = 2126,
	MDATA_CONVERTSELECTION_RIGHT						 = 2127,
	MDATA_CONVERTSELECTION_TOLERANT					 = 2128,
	MDATA_CREATEPOLYGON_KEEPQUADRANGLE			 = 2081,
	MDATA_CROSSSECTION_ANGLE								 = 2082,
	MDATA_CRUMPLE_INANDOUT									 = 2026,
	MDATA_CRUMPLE_MODE											 = 2025,
	MDATA_CRUMPLE_VAL												 = 2027,
	MDATA_CURRENTSTATETOOBJECT_INHERITANCE	 = 2140,
	MDATA_CURRENTSTATETOOBJECT_KEEPANIMATION = 2141,
	MDATA_CURRENTSTATETOOBJECT_NOGENERATE		 = 2162,
	MDATA_DISCONNECT_PRESERVEGROUPS					 = 2028,
	MDATA_EXTRUDE_CREATEQUADS								 = 2168,
	MDATA_FILLSEL_SELECTION									 = 2158,
	MDATA_FILLSEL_START_POLY								 = 2157,
	MDATA_KNIFE_ANGLE												 = 2115,
	MDATA_KNIFE_P1													 = 2110,
	MDATA_KNIFE_P2													 = 2112,
	MDATA_KNIFE_V1													 = 2111,
	MDATA_KNIFE_V2													 = 2113,
	MDATA_MAGNET_NEARESTPOINT								 = 0,
	MDATA_MAGNET_RADIUS											 = 0,
	MDATA_MAGNET_TYPE												 = 0,
	MDATA_MAGNET_WIDTH											 = 0,
	MDATA_MIRROR_DUPLICATE									 = 2070,
	MDATA_MIRROR_ONPLANE										 = 2122,
	MDATA_MIRROR_PLANE											 = 2068,
	MDATA_MIRROR_POINT											 = 2074,
	MDATA_MIRROR_SELECTIONS									 = 2120,
	MDATA_MIRROR_SNAPPOINTS									 = 2069,
	MDATA_MIRROR_SYSTEM											 = 2067,
	MDATA_MIRROR_TOLERANCE									 = 2072,
	MDATA_MIRROR_VALUE											 = 2073,
	MDATA_MIRROR_VECTOR											 = 2075,
	MDATA_MIRROR_WELD												 = 2071,
	MDATA_OPTIMIZE_POINTS										 = 2077,
	MDATA_OPTIMIZE_POLYGONS									 = 2078,
	MDATA_OPTIMIZE_TOLERANCE								 = 2076,
	MDATA_OPTIMIZE_UNUSEDPOINTS							 = 2079,
	MDATA_OUTLINESEL_SELECTION							 = 2160,
	MDATA_OUTLINESEL_START_POLY							 = 2159,
	MDATA_QUANTIZE_ENABLEX									 = 2003,
	MDATA_QUANTIZE_ENABLEY									 = 2004,
	MDATA_QUANTIZE_ENABLEZ									 = 2005,
	MDATA_QUANTIZE_VAL											 = 2006,
	MDATA_RING_EDGE													 = 2154,
	MDATA_RING_SELECTION										 = 2155,
	MDATA_RING_SKIP													 = 2156,
	MDATA_ROTATECAMERA											 = 2142,
	MDATA_SELECTFREE_TOLERANCE							 = 2106,
	MDATA_SELECTFREE_VISIBLE								 = 2107,
	MDATA_SELECTPOLY_TOLERANCE							 = 2106,
	MDATA_SELECTPOLY_VISIBLE								 = 2107,
	MDATA_SELECTRECT_TOLERANCE							 = 2106,
	MDATA_SELECTRECT_VISIBLE								 = 2107,
	MDATA_SETVERTEX_MODE										 = 4001,
	MDATA_SETVERTEX_VALUE										 = 4000,
	MDATA_SPLINE_ADDPOINTPOSITION						 = 2024,
	MDATA_SPLINE_ADDPOINTSEGMENT						 = 2023,
	MDATA_SPLINE_FREEHANDTOLERANCE					 = 2020,
	MDATA_SPLINE_OUTLINE										 = 2021,
	MDATA_SPLINE_OUTLINESEPARATE						 = 2080,
	MDATA_SPLINE_PROJECTMODE								 = 2022,
	MDATA_SUBDIVIDE_ANGLE										 = 2099,
	MDATA_SUBDIVIDE_HYPER										 = 2098,
	MDATA_SUBDIVIDE_SPLINESUB								 = 2100,
	MDATA_SUBDIVIDE_SUB											 = 2101,
	MDATA_UNTRIANGULATE_ANGLE								 = 2121,
	MDATA_UNTRIANGULATE_ANGLE_RAD						 = 2161,
	MDATA_UNTRIANGULATE_NGONS								 = 2143,
	MDATA_VERTEXPAINT_ENABLE								 = 0,
	MEMORYPOOL_DEFAULT_BLOCKSIZE						 = 524288,
	MENURESOURCE_COMMAND										 = 3,
	MENURESOURCE_MENURESOURCE								 = 6,
	MENURESOURCE_SEPERATOR									 = 2,
	MENURESOURCE_STRING											 = 5,
	MENURESOURCE_SUBMENU										 = 1,
	MENURESOURCE_SUBTITLE										 = 4,
	MESSAGE_SOFTPREVIEW											 = 1016765,
	MM_HASOBJECTS														 = 1751217007,
	MM_LOCKFREE															 = 1818454886,
	MM_LOCKSET															 = 1818454899,
	MODELINGCOMMANDFLAG_CREATEUNDO					 = 1,
	MODELING_COMMIT_ADDUNDO									 = 32,
	MODELING_COMMIT_CREATEMAP								 = 2,
	MODELING_COMMIT_NONE										 = 0,
	MODELING_COMMIT_NOVALIDATION						 = 128,
	MODELING_COMMIT_NO_NGONS								 = 72,
	MODELING_COMMIT_QUADLIMIT								 = 256,
	MODELING_COMMIT_QUADS										 = 64,
	MODELING_COMMIT_REFRESH									 = 4,
	MODELING_COMMIT_RESTORE									 = 16,
	MODELING_COMMIT_TRINGONS								 = 8,
	MODELING_COMMIT_UPDATE									 = 1,
	MODELING_ERROR_BADARGS									 = 11,
	MODELING_ERROR_CORRUPTOBJECT						 = 12
};

enum
{
	MODELING_ERROR_FAILED							= 1,
	MODELING_ERROR_FATAL							= 5,
	MODELING_ERROR_ILLEGAL						= 7,
	MODELING_ERROR_INTERNAL						= 10,
	MODELING_ERROR_INVALIDKERNEL			= 9,
	MODELING_ERROR_INVALIDOBJECT			= 6,
	MODELING_ERROR_INVALIDOP					= 4,
	MODELING_ERROR_MSGFAIL						= 13,
	MODELING_ERROR_NOMEMORY						= 2,
	MODELING_ERROR_NONE								= 0,
	MODELING_ERROR_NOTFOUND						= 3,
	MODELING_ERROR_TRIANGULATION			= 8,
	MODELING_ERROR_UNKNOWN						= -1,
	MODELING_GETNGON_FLAG_EMPTY				= 0,
	MODELING_GETNGON_FLAG_READONLY		= 1,
	MODELING_SETNGON_FLAG_EMPTY				= 0,
	MODELING_SETNGON_FLAG_FIXEDQUADS	= 2,
	MODELING_SETNGON_FLAG_NGONQUADS		= 4,
	MODELING_SETNGON_FLAG_NOROTATE		= 32,
	MODELING_SETNGON_FLAG_QUADS				= 8,
	MODELING_SETNGON_FLAG_TRIANGULATE = 1,
	MODELING_SETNGON_FLAG_UNION				= 16,
	MODELING_SETPOINT_FLAG_EMPTY			= 0,
	MODELING_SETPOINT_FLAG_NODIRTY		= 1,
	MODELING_SETPOINT_FLAG_UNION			= 2,
	//MODE_ALPHA=1,
	MODE_CMYK													= 6,
	MODE_CMYKA												= 7,
	MODE_DEFORM												= 2,
	MODE_EDITOR												= 0,
	//MODE_GREY8=2,
	MODE_GREYA8												= 3,
	MODE_GREYAf												= 35,
	MODE_GREYAw												= 19,
	MODE_GREYf												= 34,
	MODE_GREYw												= 18,
	MODE_MASK													= 8,
	MODE_MASKA												= 9,
	MODE_MAXCOLOR											= 63,
	MODE_OFF													= 1,
	MODE_ON														= 0,
	MODE_RENDER												= 1,
	//MODE_RGB=4,
	//MODE_RGBA=5,
	MODE_RGBAf																= 4,
	MODE_RGBAw																= 4,
	MODE_RGBf																	= 36,
	MODE_RGBw																	= 20,
	MODE_UNDEF																= 2,
	MODIFY_ALL																= 0,
	MODIFY_EDGESELECTION											= 3,
	MODIFY_POINTSELECTION											= 1,
	MODIFY_POLYGONSELECTION										= 2,
	MORPHKEY_NAME															= 1,
	MOUSEDRAG_CONTINUE												= 3,
	MOUSEDRAG_DONTHIDEMOUSE										= 1,
	MOUSEDRAG_ESCAPE													= 1,
	MOUSEDRAG_EVERYPACKET											= 4,
	MOUSEDRAG_FINISHED												= 2,
	MOUSEDRAG_NOMOVE													= 2,
	MOUSEPOS																	= 2147483647,
	MOUSESELECT_FREE													= 3,
	MOUSESELECT_NOCROSSCURSOR									= 1073741824,
	MOUSESELECT_POLYGON												= 2,
	MOUSESELECT_RECTANGLE											= 1,
	MOUSE_ADDPOINTS														= 29,
	MOUSE_ADDPOLYGONS													= 30,
	MOUSE_ARROW_H															= 16,
	MOUSE_ARROW_HV														= 18,
	MOUSE_ARROW_V															= 17,
	MOUSE_BRIDGE															= 31,
	MOUSE_BUSY																= 3,
	MOUSE_COPY																= 10,
	MOUSE_CROSS																= 4,
	MOUSE_DELETE															= 9,
	MOUSE_EXTRUDE															= 27,
	MOUSE_FORBIDDEN														= 8,
	MOUSE_HIDE																= 0,
	MOUSE_IBEAM																= 21,
	MOUSE_INSERTCOPY													= 11,
	MOUSE_INSERTCOPYDOWN											= 12,
	MOUSE_INSERTMOVE													= 14,
	MOUSE_INSERTMOVEDOWN											= 15,
	MOUSE_MIRROR															= 32,
	MOUSE_MOVE																= 13,
	MOUSE_MOVE_HAND														= 20,
	MOUSE_NORMAL															= 2,
	MOUSE_NORMALMOVE													= 28,
	MOUSE_PAINTBRUSH													= 42,
	MOUSE_PAINTCLONE													= 43,
	MOUSE_PAINTCOLORRANGE											= 39,
	MOUSE_PAINTCROP														= 45,
	MOUSE_PAINTFILL														= 40,
	MOUSE_PAINTLINE														= 46,
	MOUSE_PAINTMAGICWAND											= 38,
	MOUSE_PAINTMOVE														= 33,
	MOUSE_PAINTPICK														= 41,
	MOUSE_PAINTPOLYSHAPE											= 47,
	MOUSE_PAINTSELECTCIRCLE										= 35,
	MOUSE_PAINTSELECTFREE											= 37,
	MOUSE_PAINTSELECTPOLY											= 36,
	MOUSE_PAINTSELECTRECT											= 34,
	MOUSE_PAINTTEXT														= 44,
	MOUSE_POINT_HAND													= 19,
	MOUSE_QUESTION														= 5,
	MOUSE_SELECT_FREE													= 23,
	MOUSE_SELECT_LIVE													= 22,
	MOUSE_SELECT_POLY													= 25,
	MOUSE_SELECT_RECT													= 24,
	MOUSE_SHOW																= 1,
	MOUSE_SPLINETOOLS													= 26,
	MOUSE_ZOOM_IN															= 6,
	MOUSE_ZOOM_OUT														= 7,
	MOVE_CONTINUE															= 1,
	MOVE_END																	= 2,
	MOVE_START																= 0,
	MPB_BITMAPTYPE														= 1005,
	MPB_BLENDMODE															= 1003,
	MPB_COLORMODE															= 1004,
	MPB_DPI																		= 1007,
	MPB_NAME																	= 1006,
	MPB_PERCENT																= 1002,
	MPB_SAVE																	= 1001,
	MPB_SHOW																	= 1000,
	MPB_USERID																= 1008,
	MPB_USERSUBID															= 1009,
	MSG_BASECONTAINER													= 9,
	MSG_CHANGE																= 7,
	MSG_COFFEESCRIPT_EXECUTE									= 1001184,
	MSG_COFFEESCRIPT_RETRIEVEBITMAP						= 1001185,
	MSG_DESCRIPTION_CHECKDRAGANDDROP					= 26,
	MSG_DESCRIPTION_CHECKUPDATE								= 17,
	MSG_DESCRIPTION_CHECKUPDATE_AUTOKEY				= 4,
	MSG_DESCRIPTION_CHECKUPDATE_DOCUMENT			= 2,
	MSG_DESCRIPTION_CHECKUPDATE_NOSCENEUPDATE = 8388608,
	MSG_DESCRIPTION_CHECKUPDATE_SYNC					= 16,
	MSG_DESCRIPTION_COMMAND										= 18,
	MSG_DESCRIPTION_EDIT_ENTRY								= 25,
	MSG_DESCRIPTION_GETBITMAP									= 27,
	MSG_DESCRIPTION_GETOBJECTS								= 30,
	MSG_DESCRIPTION_INITUNDO									= 16,
	MSG_DESCRIPTION_REMOVE_ENTRY							= 24,
	MSG_DESCRIPTION_USERINTERACTION_END				= 31,
	MSG_DESCRIPTION_VALIDATE									= 20,
	MSG_DOCUMENTINFO													= 1001078,
	MSG_DOCUMENTINFO_TYPE_LOAD								= 1001,
	MSG_DOCUMENTINFO_TYPE_MERGE								= 1002,
	MSG_DOCUMENTINFO_TYPE_REMOVE							= 1008,
	MSG_DOCUMENTINFO_TYPE_SAVEPROJECT_AFTER		= 1007,
	MSG_DOCUMENTINFO_TYPE_SAVEPROJECT_BEFORE	= 1006,
	MSG_DOCUMENTINFO_TYPE_SAVE_AFTER					= 1005,
	MSG_DOCUMENTINFO_TYPE_SAVE_BEFORE					= 1004,
	MSG_DOCUMENTINFO_TYPE_SETACTIVE						= 1000,
	MSG_DOCUMENT_MODE_CHANGED									= 200000091,
	MSG_DRAGANDDROP_TAG												= 1018756,
	MSG_EDIT																	= 21,
	MSG_EMULATIONLAYER												= 112007,
	MSG_FILTER																= 14,
	MSG_GETALLSTRINGS													= 112004,
	MSG_GETDATA																= 112001,
	MSG_HAIR_GET_OBJECTS											= 1018959,
	MSG_HAIR_GET_OBJECT_TYPE									= 1018952,
	MSG_INITIALCHANNEL												= 1001073,
	MSG_MARK_MATERIALS												= 1073741828,
	MSG_MATERIALDRAGANDDROP										= 1001069,
	MSG_MATERIALS_CHANGED											= 3,
	MSG_MENUPREPARE														= 22,
	MSG_MOVE_FINISHED													= 32,
	MSG_MULTI_DOCUMENTCLONED									= 11,
	MSG_MULTI_DOCUMENTIMPORTED								= 13,
	MSG_MULTI_MARKMATERIALS										= 1073741828,
	MSG_MULTI_RENDERNOTIFICATION							= 1001071,
	MSG_POINTS_CHANGED												= 1,
	MSG_POLYGONS_CHANGED											= 2,
	MSG_PRETRANSLATE_NGONS										= 1015824,
	MSG_PRETRANSLATE_POINTS										= 1015822,
	MSG_PRETRANSLATE_POLYGONS									= 1015823,
	MSG_PRETRANSLATE_SEGMENTS									= 1015832,
	MSG_RENAMETEXTURES												= 112010,
	MSG_RETRIEVEPRIVATEDATA										= 23,
	MSG_SEGMENTS_CHANGED											= 10,
	MSG_SETDATA																= 112000,
	MSG_SHADERREAD														= 112009,
	MSG_SHADERV5TOV6													= 112005,
	MSG_SHADERV6TOV7													= 112006,
	MSG_SMALLUPDATE														= 6,
	MSG_TIMER																	= 112008,
	MSG_TOOL_RESTART													= 200000096,
	MSG_TRANSFERGOALS													= 15,
	MSG_TRANSLATE_NGONS												= 1015634,
	MSG_TRANSLATE_POINTS											= 1015632,
	MSG_TRANSLATE_POLYGONS										= 1015633,
	MSG_TRANSLATE_SEGMENTS										= 1015831,
	MSG_UPDATE																= 5,
	MSG_UPDATE_NGONS													= 475000000,
	MSG_VIDEOPOST_CREATERAY										= 1001187,
	MSG_VIDEOPOST_TIMESHIFT										= 300000106,
	MULTIMSG_BROADCAST												= 4,
	MULTIMSG_DOWN															= 3,
	MULTIMSG_MASK															= 7,
	MULTIMSG_ROOT															= 2,
	MULTIMSG_UP																= 1,
	Manimation																= 9,
	MaximumQuality														= 8,
	Maxis																			= 2,
	Mcamera																		= 0,
	Mdrag																			= 1000,
	Medges																		= 6,
	MediumQuality															= 2,
	Merge																			= 1,
	Mkinematic																= 10,
	Mmodel																		= 11,
	Mobject																		= 1,
	MoveError																	= -2147483136,
	MoveNotPermitted													= -2147483646,
	MovieFile																	= 11,
	Mpaint																		= 12,
	Mpoints																		= 5,
	Mpolyedgepoint														= 15,
	Mpolygons																	= 7,
	Mtexture																	= 3,
	Mtextureaxis															= 4,
	Muvpoints																	= 13,
	Muvpolygons																= 14,
	M2dtracks																  = 18,
	NEW_ACTIVE_OBJECT													= 603,
	NEW_ACTIVE_TAG														= -1007,
	NEW_DOCUMENT															= 605,
	NGON_FLAG_NOCHANGE												= 2,
	NGON_FLAG_NOTRANSLATE											= 4,
	NGON_FLAG_NOVALIDATION										= 1,
	NGON_TOSELECTION_HOLES										= 5,
	NGON_TOSELECTION_INNEREDGES								= 4,
	NGON_TOSELECTION_OUTEREDGES								= 3,
	NGON_TOSELECTION_OUTLINE									= 2,
	NGON_TOSELECTION_POLYGONS									= 1,
	NOCLIP_D																	= 1
};
enum
{
	NOCLIP_Z = 2,
	//NOISE_BLIST_TURB=2,
	//NOISE_BOX_NOISE=1,
	//NOISE_BUYA=3,
	//NOISE_CELL_NOISE=4,
	//NOISE_CELL_VORONOI=24,
	//NOISE_CRANAL=5,
	//NOISE_DENTS=6,
	//NOISE_DISPL_TURB=7,
	//NOISE_DISPL_VORONOI=25,
	//NOISE_ELECTRIC=32,
	//NOISE_FBM=8,
	//NOISE_FIRE=31,
	//NOISE_GASOUS=33,
	//NOISE_HAMA=9,
	NOISE_LIB = 1011190,
	//NOISE_LUKA=10,
	//NOISE_MOD_NOISE=11,
	//NOISE_NAKI=12,
	//NOISE_NOISE=13,
	//NOISE_NONE=99,
	//NOISE_NUTOUS=14,
	//NOISE_OBER=15,
	NOISE_OFFSET = 2000,
	//NOISE_PEZO=16,
	//NOISE_POXO=17,
	//NOISE_RANDOM=18,
	NOISE_RESOLUTION = 1024,
	//NOISE_SEMA=19,
	//NOISE_SPARSE_CONV=26,
	//NOISE_STUPL=20,
	//NOISE_TURBULENCE=21,
	//NOISE_UNKNOWN=0,
	//NOISE_VL_NOISE=22,
	//NOISE_VORONOI_1=27,
	//NOISE_VORONOI_2=28,
	//NOISE_VORONOI_3=29,
	//NOISE_WAVY_TURB=23,
	//NOISE_ZADA=30,
	NORMALTOOL_MOVE		= 0,
	NORMALTOOL_ROTATE = 2,
	NORMALTOOL_SCALE	= 1,
	//NOTINDEX=-1,
	//NOTOK=-1,
	NewFolderNotPermitted													= -2147483632,
	NoPreview																			= 128,
	OBJECTRESTRICTION_AO													= 16,
	OBJECTRESTRICTION_REFLECTION									= 4,
	OBJECTRESTRICTION_REFRACTION									= 2,
	OBJECTRESTRICTION_TRANSPARENCY								= 1,
	OBJECTRESTRICTION_TRANSPARENCY_INUSE					= 8,
	OBJECT_ARRAY																	= 5150,
	OBJECT_ATTRACTOR															= 5119,
	OBJECT_BACKGROUND															= 5122,
	OBJECT_BEND																		= 5128,
	OBJECT_BEZIER																	= 5120,
	OBJECT_BONE																		= 5123,
	OBJECT_BOOLE																	= 1010865,
	OBJECT_BULGE																	= 5129,
	OBJECT_CAMERA																	= 5103,
	OBJECT_CAMERADEPENDENT												= 512,
	OBJECT_CAPSULE																= 5171,
	OBJECT_CONE																		= 5162,
	OBJECT_CONPLANE																= 5153,
	OBJECT_CUBE																		= 5159,
	OBJECT_CYLINDER																= 5170,
	OBJECT_DEFLECTOR															= 5110,
	OBJECT_DESTRUCTOR															= 5124,
	OBJECT_DISC																		= 5164,
	OBJECT_ENVIRONMENT														= 5106,
	OBJECT_EXPLOSION															= 5145,
	OBJECT_EXTRUDE																= 5116,
	OBJECT_FFD																		= 5108,
	OBJECT_FIGURE																	= 5166,
	OBJECT_FLOOR																	= 5104,
	OBJECT_FOREGROUND															= 5121,
	OBJECT_FORMULA																= 5146,
	OBJECT_FRACTAL																= 5169,
	OBJECT_FRICTION																= 5114,
	OBJECT_GENERATOR															= 4,
	OBJECT_GRAVITATION														= 5111,
	OBJECT_HEADPHONE															= 5151,
	OBJECT_HIERARCHYMODIFIER											= 2,
	OBJECT_HYPERNURBS															= 5127,
	OBJECT_INPUT																	= 8,
	OBJECT_INSTANCE																= 5126,
	OBJECT_ISSPLINE																= 128,
	OBJECT_LATHE																	= 5117,
	OBJECT_LIGHT																	= 5102,
	OBJECT_LOFT																		= 5107,
	OBJECT_LOUDSPEAKER														= 5139,
	OBJECT_MELT																		= 5147,
	OBJECT_METABALL																= 5125,
	OBJECT_MICROPHONE															= 5138,
	OBJECT_MODIFIER																= 1,
	OBJECT_NOCACHESUB															= 64,
	OBJECT_NULL																		= 5140,
	OBJECT_OILTANK																= 5172,
	OBJECT_PAINTBITMAP														= 543,
	//OBJECT_PAINTLAYER=544,
	OBJECT_PAINTLAYERBMP													= 501,
	//OBJECT_PAINTTEXTURE=502,
	OBJECT_PARTICLE																= 5109,
	OBJECT_PARTICLEMODIFIER												= 32,
	OBJECT_PLANE																	= 5168,
	OBJECT_PLATONIC																= 5161,
	OBJECT_POLY																		= 5174,
	OBJECT_POLYGON																= 5100,
	OBJECT_PRIMITIVE															= 5141,
	OBJECT_PYRAMID																= 5167,
	OBJECT_RELIEF																	= 5173,
	OBJECT_ROTATION																= 5112,
	OBJECT_SHATTER																= 5148,
	OBJECT_SHEAR																	= 5131,
	OBJECT_SKY																		= 5105,
	OBJECT_SPHERE																	= 5160,
	OBJECT_SPLINE4SIDE														= 5180,
	OBJECT_SPLINE																	= 5101,
	OBJECT_SPLINEARC															= 5182,
	OBJECT_SPLINECIRCLE														= 5181,
	OBJECT_SPLINECISSOID													= 5183,
	OBJECT_SPLINECOGWHEEL													= 5188,
	OBJECT_SPLINECONTOUR													= 5189,
	OBJECT_SPLINECYCLOID													= 5184,
	OBJECT_SPLINEFLOWER														= 5176,
	OBJECT_SPLINEFORMULA													= 5177,
	OBJECT_SPLINEHELIX														= 5185,
	OBJECT_SPLINENSIDE														= 5179,
	OBJECT_SPLINEPRIMITIVE												= 5152,
	OBJECT_SPLINEPROFILE													= 5175,
	OBJECT_SPLINERECTANGLE												= 5186,
	OBJECT_SPLINESTAR															= 5187,
	OBJECT_SPLINETEXT															= 5178,
	OBJECT_STAGE																	= 5136,
	OBJECT_SWEEP																	= 5118,
	OBJECT_SYMMETRY																= 5142,
	OBJECT_TAPER																	= 5133,
	OBJECT_TORUS																	= 5163,
	OBJECT_TUBE																		= 5165,
	OBJECT_TURBULENCE															= 5115,
	OBJECT_TWIST																	= 5134,
	OBJECT_UNIQUEENUMERATION											= 256,
	OBJECT_USECACHECOLOR													= 1024,
	OBJECT_WAVE																		= 5135,
	OBJECT_WIND																		= 5113,
	OBJECT_WINDDEFORM															= 5149,
	OBJECT_WRAP																		= 5143,
	OBSOLETE_WPREF_UNITS_COLOR										= 10002,
	OGL_SET_BACKIMAGEMODE_OLD											= 0,
	OGL_SET_BACKIMAGEMODE_TEX_INTERPOL						= 2,
	OGL_SET_BACKIMAGEMODE_TEX_NEAREST							= 1,
	ONLINEHELP_MODE_HTTP													= 1,
	ONLINEHELP_MODE_LOCAL													= 0,
	OPENGL_EXTENSION_STRING												= 1003,
	OPENGL_SUPPORT_DUALPLANE_ARB									= 1001,
	OPENGL_SUPPORT_DUALPLANE_KTX									= 1002,
	OPENGL_SUPPORT_GL_EXT_SEPARATE_SPECULAR_COLOR = 1004,
	OPERATOR_CONCAT																= 1,
	OPTIMALMAPPING_AREAFAK												= 1004,
	OPTIMALMAPPING_DISTORTION											= 1002,
	OPTIMALMAPPING_PRESERVEORIENTATION						= 1000,
	OPTIMALMAPPING_PROGRESSBAR										= 1006,
	OPTIMALMAPPING_RELAXCOUNT											= 1007,
	OPTIMALMAPPING_SPACING												= 1003,
	OPTIMALMAPPING_STRETCHTOFIT										= 1001,
	OPTIMALMAPPING_TWOD														= 1005,
	OVERSAMPLING																	= 16,
	O_FLOOR																				= 0,
	O_POLYGON																			= 3,
	O_SKY																					= 1,
	O_SPHERE																			= 2,
};
enum
{
	PAINTER_BRINGTOFRONT												= 1001,
	PAINTER_CLOSETEXTURE												= 1004,
	PAINTER_FORCECLOSETEXTURE										= 1003,
	PAINTER_LOADTEXTURE													= 1002,
	PAINTER_SAVEBIT_RENAMEINSCENE								= 8,
	PAINTER_SAVEBIT_SAVEALLOWWARNING						= 4,
	PAINTER_SAVEBIT_SAVEAS											= 1,
	PAINTER_SAVEBIT_SAVECOPY										= 2,
	PAINTER_SAVETEXTURE													= 1000,
	PAINTER_SAVETEXTURE_DIRECT									= 1005,
	PAINTER_SAVETEXTURE_FLAGS										= 1,
	PAINTMESH_QUAD															= 2,
	PAINTMESH_SEL																= 64,
	PARAMETERKEY_OBJECTID												= 600,
	PARTICLEOBJECT_LASTP												= 999,
	PARTICLEOBJECT_LASTT												= 999,
	PARTICLE_ALIVE															= 2,
	PARTICLE_VISIBLE														= 1,
	PENDRAWDISTANCE															= 8,
	PENDRAWRANDOMNESS														= 5,
	PENDRAWROTATION															= 4,
	PENDRAWWHEELPRESSURE												= 7,
	PENDRAWWHEELROTATION												= 6,
	PENFINGERWHEEL															= 9,
	PENPRESSURE																	= 1,
	PENROTATION																	= 3,
	PENTILT																			= 2,
	PHONGTAG_PHONG															= 1000,
	PLUGINFLAG_BITMAPLOADER_MOVIE								= 64,
	PLUGINFLAG_BITMAPSAVER_ALLOWOPTIONS					= 134217728,
	PLUGINFLAG_BITMAPSAVER_FORCESUFFIX					= 67108864,
	PLUGINFLAG_BITMAPSAVER_MOVIE								= 64,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_16BIT				= 4,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_16BIT_LAYERS = 16,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_32BIT				= 1,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_32BIT_LAYERS = 32,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_8BIT					= 2,
	PLUGINFLAG_BITMAPSAVER_SUPPORT_8BIT_LAYERS	= 8,
	PLUGINFLAG_COFFEE														= -2147483647,
	PLUGINFLAG_COMMAND_DONT_CLONE_ICON					= 16777216,
	PLUGINFLAG_HIDE															= 536870912,
	PLUGINFLAG_HIDEPLUGINMENU										= 268435456,
	PLUGINFLAG_MATERIAL_GLIMAGE									= 1,
	PLUGINFLAG_MATERIAL_HIDE_IN_MATERIALMANAGER = 4,
	PLUGINFLAG_MATERIAL_NO_MATERIALEDITOR				= 2,
	PLUGINFLAG_REFRESHALWAYS										= 33554432,
	PLUGINFLAG_SCENEHOOK_NOTDRAGGABLE						= 1,
	PLUGINFLAG_SCENELOADER_MERGEORIGINAL				= 67108864,
	PLUGINFLAG_SMALLNODE												= 1073741824,
	PLUGINFLAG_TOOL_DRAW_MULTIPLANE							= 4,
	PLUGINFLAG_TOOL_EDITSTATES									= 32,
	PLUGINFLAG_TOOL_EVALUATEHANDLES							= 2,
	PLUGINFLAG_TOOL_HIGHLIGHT										= 16,
	PLUGINFLAG_TOOL_SINGLECLICK									= 128,
	PLUGINFLAG_TOOL_SNAPSETTINGS								= 64,
	PLUGINFLAG_TOOL_TWEAK												= 8,
	PLUGINFLAG_TOOL_TWEAK_NO_HIGHLIGHT					= 264,
	PLUGINFLAG_VIDEOPOST_INHERENT								= 2,
	PLUGINFLAG_VIDEOPOST_MULTIPLE								= 1,
	POLY_TRIANG_CONSTRAINED_OUTLINE							= 1,
	POLY_TRIANG_FORCE_NGON											= 160,
	POLY_TRIANG_FORCE_VORONOI										= 64,
	POLY_TRIANG_NGON														= 32,
	POLY_TRIANG_QUADS														= 8,
	POPUPEDITTEXTCALLBACK_CANCELED							= 3,
	POPUPEDITTEXTCALLBACK_CLOSED								= 2,
	POPUPEDITTEXTCALLBACK_CURSOR_DOWN						= 5,
	POPUPEDITTEXTCALLBACK_CURSOR_UP							= 4,
	POPUPEDITTEXTCALLBACK_TEXTCHANGED						= 1,
	POPUP_																			= 257,
	POPUP_ABOVE																	= 1,
	POPUP_ADJUSTHEIGHT													= 32,
	POPUP_ADJUSTWIDTH														= 16,
	POPUP_ALLOWUNDOCK														= 128,
	POPUP_ALLOWUNDOCK_REC												= 256,
	POPUP_BELOW																	= 2,
	POPUP_CENTERHORIZ														= 12,
	POPUP_CENTERVERT														= 3,
	POPUP_EXECUTECOMMANDS												= 64,
	POPUP_LEFT																	= 4,
	POPUP_RIGHT																	= 8,
	PRIMITIVE_4SIDE															= 1019,
	PRIMITIVE_ARC																= 2001,
	PRIMITIVE_CAPSULE														= 1012,
	PRIMITIVE_CIRCLE														= 2000,
	PRIMITIVE_CISSOID														= 2002,
	PRIMITIVE_COGWHEEL													= 2009,
	PRIMITIVE_CONE															= 1003,
	PRIMITIVE_CONTOUR														= 2010,
	PRIMITIVE_CUBE															= 1000,
	PRIMITIVE_CYCLOID														= 2003,
	PRIMITIVE_CYLINDER													= 1011,
	PRIMITIVE_DISC															= 1005,
	PRIMITIVE_FIGURE														= 1007,
	PRIMITIVE_FLOWER														= 2007,
	PRIMITIVE_FORMULA														= 1016,
	PRIMITIVE_FRACTAL														= 1010,
	PRIMITIVE_HELIX															= 2005,
	PRIMITIVE_NSIDE															= 1018,
	PRIMITIVE_OILTANK														= 1013,
	PRIMITIVE_PLANE															= 1009,
	PRIMITIVE_PLATONIC													= 1002,
	PRIMITIVE_POLY															= 1015,
	PRIMITIVE_PROFILE														= 2004,
	PRIMITIVE_PYRAMID														= 1008,
	PRIMITIVE_RECTANGLE													= 2006,
	PRIMITIVE_RELIEF														= 1014,
	PRIMITIVE_SPHERE														= 1001,
	PRIMITIVE_STAR															= 2008,
	PRIMITIVE_TEXT															= 1017,
	PRIMITIVE_TORUS															= 1004,
	PRIMITIVE_TUBE															= 1006,
	PRIM_CONE_BFILANGLE													= 1143,
	PRIM_CONE_BFILLET														= 1138,
	PRIM_CONE_BFILLETY													= 1140,
	PRIM_CONE_TFILANGLE													= 1142,
	PRIM_CONE_TFILLET														= 1137,
	PRIM_CONE_TFILLETY													= 1139,
	PRIM_CONE_TYPE															= 1136,
	PRIM_CUBE_FILLET														= 1105,
	PRIM_TEXT_USE_PS														= 2110,
	PRIORITYVALUE_CAMERADEPENDENT								= 2,
	PRIORITYVALUE_MODE													= 0,
	PRIORITYVALUE_PRIORITY											= 1,
	PRIORITY_FLAG_NOCAMERA											= 1000,
	PSD_PS7SUPPORT															= 10025,
	PSRKEY_P_X_YY																= 1003,
	PSRKEY_P_Y_YY																= 1004,
	PSRKEY_P_Z_YY																= 1005,
	PSRKEY_VL_X_XX															= 1006,
	PSRKEY_VL_X_YY															= 1009,
	PSRKEY_VL_Y_XX															= 1007,
	PSRKEY_VL_Y_YY															= 1010,
	PSRKEY_VL_Z_XX															= 1008,
	PSRKEY_VL_Z_YY															= 1011,
	PSRKEY_VR_X_XX															= 1012,
	PSRKEY_VR_X_YY															= 1015,
	PSRKEY_VR_Y_XX															= 1013,
	PSRKEY_VR_Y_YY															= 1016,
	PSRKEY_VR_Z_XX															= 1014,
	PSRKEY_VR_Z_YY															= 1017,
	PULSATEKEY_POS_AMP													= 1,
	PULSATEKEY_POS_FREQ													= 1,
	PULSATEKEY_ROT_AMP													= 1,
	PULSATEKEY_ROT_FREQ													= 1,
	PULSATEKEY_SCALE_AMP												= 1,
	PULSATEKEY_SCALE_FREQ												= 1,
	PX_0LROTATE																	= 4194304,
	PX_0RROTATE																	= 0,
	PX_128BIT																		= 128,
	PX_16BIT																		= 16,
	PX_1BIT																			= 1,
	PX_1COMP																		= 16777216,
	PX_1LROTATE																	= 5242880,
	PX_1RROTATE																	= 1048576,
	PX_24BIT																		= 24,
	PX_2BIT																			= 2,
	PX_2COMP																		= 33554432,
	PX_32BIT																		= 32,
	PX_3BIT																			= 3,
	PX_3COMP																		= 50331648,
	PX_48BIT																		= 48,
	PX_4BIT																			= 4,
	PX_4COMP																		= 67108864,
	PX_5COMP																		= 83886080,
	PX_64BIT																		= 64,
	PX_6COMP																		= 100663296,
	PX_7COMP																		= 117440512,
	PX_8BIT																			= 8,
	PX_96BIT																		= 96,
	PX_AGRAY16																	= 838994454,
	PX_AGRAY32																	= 1107435042,
	PX_AGRAY64																	= 1375944768,
	PX_ARGB128																	= 1409508508,
	PX_ARGB32																		= 872554018,
	PX_ARGB64																		= 1140998208,
	PX_BITS																			= 255,
	PX_CMPNTS																		= 251658240,
	PX_CMPTREVERSED															= 524288,
	PX_DEPTH																		= 1879048192,
	PX_DEPTH_16																	= 1073741824,
	PX_DEPTH_32																	= 1342177280,
	PX_DEPTH_8																	= 805306368,
	PX_FLAGS																		= 16252928,
	PX_FLOAT																		= 196608,
	PX_GRAY16																		= 1090652694,
	PX_GRAY32																		= 1359158818,
	PX_GRAY8																		= 822216712,
	PX_GRAYA16																	= 844237334,
	PX_GRAYA32																	= 1112677922,
	PX_GRAYA64																	= 1381187648,
	PX_MAC15																		= 859965973,
	PX_MAC1																			= 822214913,
	PX_MAC32																		= 859968548,
	PX_MAC4																			= 822215684,
	PX_MAC8																			= 822216712,
	PX_PACKED																		= 131072,
	PX_PACKING																	= 196608,
	PX_PLANES																		= 65536,
	PX_REVERSED																	= 8388608,
	PX_RGB24																		= 855774236,
	PX_RGB48																		= 1124214840,
	PX_RGB96																		= 1392730208,
	PX_RGBA128																	= 1414751388,
	PX_RGBA32																		= 877796898,
	PX_RGBA64																		= 1146241088,
	PX_RGBx24																		= 855774244,
	PX_RTCMPNTS																	= 3145728,
	PX_RTDIR																		= 4194304,
	PX_USED																			= 65280,
	PX_USES128																	= 32768,
	PX_USES15																		= 3840,
	PX_USES16																		= 4096,
	PX_USES1																		= 256,
	PX_USES24																		= 6144,
	PX_USES2																		= 512,
	PX_USES30																		= 7680,
	PX_USES32																		= 8192,
	PX_USES3																		= 768,
	PX_USES48																		= 12288,
	PX_USES4																		= 1024,
	PX_USES64																		= 16384,
	PX_USES8																		= 2048,
	PX_USES96																		= 24576,
	PX_VGA15																		= 868354581,
	PX_VGA16																		= 864160278,
	PX_VGA24																		= 864162844,
	PX_VGA32																		= 868357156,
	PX_VGA8																			= 822216712,
	PX_xFIRST																		= 4194304,
	PX_xLAST																		= 0,
	PX_xRGB15																		= 859965973,
	PX_xRGB24																		= 859968548,
	PX_xRGB48																		= 1128409160,
	PX_xRGB96																		= 1396924544,
	P_CAMERAMAP																	= 8,
	P_CUBIC																			= 3,
	P_CYLINDRICAL																= 1,
	P_FLAT																			= 2,
	P_FRONTAL																		= 4,
	P_SHRINKWRAP																= 7,
	P_SPATIAL																		= 5,
	P_SPHERICAL																	= 0,
	P_UVW																				= 6,
	P_VOLUMESHADER															= 10,
	PerformOperation														= 2,
	PrepareOperation														= 1,
	Preset																			= 12,
	PreviewTab																	= 0,
	PrivateFlag0																= 65536,
	PrivateFlag1																= 131072,
	PrivateFlag2																= 262144,
	PrivateFlag3																= 524288,
	PrivateTab0																	= 65536,
	PrivateTab1																	= 65537
};
enum
{
	PrivateTab2											= 65538,
	PrivateTab3											= 65539,
	PrivateTab4											= 65540,
	PrivateTab5											= 65541,
	PrivateTab6											= 65542,
	PrivateTab7											= 65543,
	QALT2														= 8,
	QALT3														= 16,
	QALT														= 4,
	QCTRL														= 2,
	QQUAL_													= 17,
	QSHIFT													= 1,
	QTSAVER_COMPRESSOR							= 10010,
	QTSAVER_DATARATE								= 10016,
	QTSAVER_FIXEDFRAMERATE					= 10020,
	QTSAVER_FRAMEDURATION						= 10017,
	QTSAVER_FRAMERATE								= 10013,
	QTSAVER_KEYFRAMES								= 10014,
	QTSAVER_MINQUALITY							= 10018,
	QTSAVER_MINTEMPQUAL							= 10019,
	QTSAVER_PLANES									= 10015,
	QTSAVER_QUALITY									= 10011,
	QTSAVER_TEMPQUAL								= 10012,
	QUALIFIER_CTRL									= 2,
	QUALIFIER_SHIFT									= 1,
	QUICKTAB_BARSUBGROUP						= 1937072743,
	QUICKTAB_BARTITLE								= 1651796340,
	QUICKTAB_SEPARATOR							= 1936027745,
	QUICKTAB_SHOWSINGLE							= 1936942956,
	QUICKTAB_SPRINGINGFOLDERS				= 1936749166,
	QualityMask											= 15,
	RAYPOLYSTATE_QUADRANGLE					= 2,
	RAYPOLYSTATE_SPLIT							= 4,
	RAYPOLYSTATE_TRIANGLE						= 1,
	RAY_ALLOWVLMIX									= 512,
	RAY_BACKGROUND									= 5,
	RAY_BLURRY											= 2048,
	RAY_CAMERA											= 2,
	RAY_CURR_AO											= 4096,
	RAY_CURR_CUSTOM									= 128,
	RAY_CURR_REFLECTION							= 16,
	RAY_CURR_REFRACTION							= 64,
	RAY_CURR_TRANSPARENCY						= 32,
	RAY_CUSTOM											= 8,
	RAY_DIFFUSE_GI									= 1024,
	RAY_ENVIRONMENT									= 1,
	RAY_FOREGROUND									= 4,
	RAY_IMAGE												= 5,
	RAY_MP													= 4,
	RAY_NOMEM												= 1,
	RAY_OK													= 0,
	RAY_PARAMETER										= 0,
	RAY_REFLECTION									= 1,
	RAY_REFRACTION									= 4,
	RAY_SKY													= 3,
	RAY_SSS													= 4096,
	RAY_TEXMISSING									= 2,
	RAY_TRANSPARENCY								= 2,
	RAY_USERBREAK										= 6,
	RAY_VOLUMETRICLIGHT							= 256,
	RECALC_INITBMP									= 2,
	RECALC_NOGRID										= 1,
	REDRAW_ALL											= 601,
	REG_EXP_PARSER									= 1011197,
	RENDERFLAG_EXTERNAL							= 1,
	RENDERFLAG_NODOCUMENTCLONE			= 2,
	RENDERFLAG_PREVIEWRENDER				= 8,
	RENDERFLAG_SHOWERRORS						= 4,
	RENDERPROPERTY_CLUSTERFRAGMENTS = 1,
	RENDER_PREVIEW_USE_BMP_SIZE			= 1,
	RESCMD_DESTINATION							= 1684370292,
	RESCMD_FLAGS										= 1718378855,
	RESCMD_HELP											= 1751477360,
	RESCMD_ICONNUM									= 1768846701,
	RESCMD_ICONPOOL									= 1886351212,
	RESCMD_LASTUPDATE								= 1970300020,
	RESCMD_MAIN1										= 1835100465,
	RESCMD_MAIN2										= 1835100466,
	RESCMD_SHORTKEY2								= 1936423218,
	RESCMD_SHORTKEY									= 1936418169,
	RESCMD_SHORTQUAL2								= 1936813106,
	RESCMD_SHORTQUAL								= 1936810348,
	RESCMD_SHORTSTRING							= 1936946290,
	RESCMD_SUB											= 1937072739,
	RESCMD_TITLE										= 1953068140,
	RESCMD_VALUE										= 1986096245,
	RESOURCE_COMMAND								= 1919118692,
	RESOURCE_CONTAINER							= 1919118964,
	RESOURCE_DIALOG									= 1919183975,
	RESOURCE_ICON										= 1919509358,
	RESOURCE_MENU										= 1919774325,
	RESULT_BUBBLEHELP								= 2,
	RESULT_CURSOR										= 1,
	RESULT_SUPPRESSBUBBLE						= 3,
	RE_BITMAP_FILTER								= 22,
	RE_COFFEE_EXT										= 11,
	RE_CUSTOMDATATYPE								= 29,
	RE_DESCRIPTION									= 27,
	RE_IO_CATALOG										= 17,
	RE_IO_FCV												= 20,
	RE_IO_IMAGE											= 12,
	RE_IO_IMAGESAVER_EX							= 19,
	RE_IO_LAYOUT										= 21,
	RE_IO_MOVIE											= 13,
	RE_IO_PREF											= 15,
	RE_IO_SCENE											= 14,
	RE_IO_SOUND											= 18,
	RE_KEY													= 6,
	RE_LIBRARY											= 28,
	RE_MATERIAL											= 8,
	RE_NODE													= 25,
	RE_OBJECT												= 3,
	RE_PAINTER											= 50,
	RE_RESOURCEDATATYPE							= 30,
	RE_SCENEHOOK										= 24,
	RE_SEQUENCE											= 5,
	RE_SHADER2D											= 9,
	RE_SHADER3D											= 10,
	RE_TAG													= 7,
	RE_TRACK												= 4,
	RE_VIDEOPOST										= 23,
	RE_WINDOW												= 1,
	RLA_COLOR												= 512,
	RLA_COVERAGE										= 64,
	RLA_NORMAL											= 16,
	RLA_OBJECTBUFFER								= 4,
	RLA_OBJECTID										= 256,
	RLA_OPTIONS											= 10024
};
enum
{
	RLA_ORIGCOLOR															 = 32,
	RLA_SUBPIXEL_MASK													 = 8192,
	RLA_SUBPIXEL_WEIGHT												 = 4096,
	RLA_TRANSPARENCY													 = 1024,
	RLA_UV																		 = 8,
	RLA_Z																			 = 1,
	ROW_FETCH_UPDATE_VIEW											 = 1,
	RT_FALLOFF_INVERSE												 = 9,
	RT_FALLOFF_INVERSE_CLAMPED								 = 6,
	RT_FALLOFF_LINEAR													 = 8,
	RT_FALLOFF_NONE														 = 0,
	RT_FALLOFF_SQUARE													 = 10,
	RT_FALLOFF_SQUARE_CLAMPED									 = 7,
	RT_FALLOFF_STEP														 = 5,
	RT_LT_AREA																 = 8,
	RT_LT_DISTANT															 = 3,
	RT_LT_OMNI																 = 0,
	RT_LT_PARALLEL														 = 4,
	RT_LT_PARSPOT															 = 5,
	RT_LT_PARSPOTRECT													 = 6,
	RT_LT_SPOT																 = 1,
	RT_LT_SPOTRECT														 = 2,
	RT_SHADOW_AREA														 = 3,
	RT_SHADOW_HARD														 = 2,
	RT_SHADOW_NONE														 = 0,
	RT_SHADOW_SOFT														 = 1,
	RT_VL_INVVOLUMETRIC												 = 3,
	RT_VL_NONE																 = 0,
	RT_VL_VISIBLE															 = 1,
	RT_VL_VOLUMETRIC													 = 2,
	RT_VN_BOTH																 = 3,
	RT_VN_HARDTURBULENCE											 = 2,
	RT_VN_ILLUM																 = 1,
	RT_VN_NOISE																 = 0,
	RT_VN_NONE																 = 0,
	RT_VN_SOFTTURBULENCE											 = 1,
	RT_VN_VISIB																 = 2,
	RT_VN_WAVYTURBULENCE											 = 3,
	ReadOnly																	 = 16,
	RegParseMatch															 = 1,
	RenameError																 = -2147483392,
	RenameNotPermitted												 = -2147483647,
	SAFEFRAME_HORIZONTAL											 = 2,
	SAFEFRAME_NONE														 = 0,
	SAFEFRAME_VERTICAL												 = 1,
	SAMPLEBUMP_MIPFALLOFF											 = 1,
	SCENEHOOKDRAW_ACTIVE_PASS									 = 1,
	SCENEHOOKDRAW_HIGHLIGHT_PASS							 = 4,
	SCENEHOOKDRAW_HIGHLIGHT_PASS_INV					 = 8,
	SCENEHOOKDRAW_INACTIVE_PASS								 = 2,
	SCENEHOOKEXECUTION_ANIMATION							 = 2,
	SCENEHOOKEXECUTION_CACHEBUILDING					 = 8,
	SCENEHOOKEXECUTION_CAMERAONLY							 = 16,
	SCENEHOOKEXECUTION_EXPRESSION							 = 4,
	SCENEHOOKEXECUTION_INDRAG									 = 32,
	SCENEHOOKEXECUTION_INMOVE									 = 64,
	SCROLLGROUP_AUTOHORIZ											 = 256,
	SCROLLGROUP_AUTOVERT											 = 512,
	SCROLLGROUP_BORDERIN											 = 64,
	SCROLLGROUP_HEADER												 = -559038738,
	SCROLLGROUP_HORIZ													 = 8,
	SCROLLGROUP_LEFT													 = 32,
	SCROLLGROUP_NOBLIT												 = 16,
	SCROLLGROUP_NOSCROLLER										 = 1024,
	SCROLLGROUP_NOVGAP												 = 2048,
	SCROLLGROUP_STATUSBAR											 = 128,
	SCROLLGROUP_VERT													 = 4,
	SCR_VISIBLEOBJS														 = 1935034707,
	SDKBROWSER_MIN_GADGET_ID									 = 1000000,
	SDKBrowserPreviewDialogAddTab							 = 3,
	SDKBrowserPreviewDialogAlloc							 = 0,
	SDKBrowserPreviewDialogAskClose						 = 15,
	SDKBrowserPreviewDialogChangePreviewBitmap = 9,
	SDKBrowserPreviewDialogCommand						 = 14,
	SDKBrowserPreviewDialogCommandNotify			 = 13,
	SDKBrowserPreviewDialogCoreMessage				 = 12,
	SDKBrowserPreviewDialogCreateLayout				 = 10,
	SDKBrowserPreviewDialogDestroyWindow			 = 17,
	SDKBrowserPreviewDialogFree								 = 1,
	SDKBrowserPreviewDialogGetActiveTab				 = 8,
	SDKBrowserPreviewDialogInit								 = 2,
	SDKBrowserPreviewDialogInitValues					 = 11,
	SDKBrowserPreviewDialogLayoutAddToTab			 = 6,
	SDKBrowserPreviewDialogLayoutChangedTab		 = 7,
	SDKBrowserPreviewDialogLayoutFlushTab			 = 5,
	SDKBrowserPreviewDialogMessage						 = 18,
	SDKBrowserPreviewDialogRemoveDefaultTab		 = 4,
	SDKBrowserPreviewDialogTimer							 = 16,
	SELECTIONFILTERBIT_BONE										 = 32,
	SELECTIONFILTERBIT_CAMERA									 = 128,
	SELECTIONFILTERBIT_DEFORMER								 = 64,
	SELECTIONFILTERBIT_GENERATOR							 = 8,
	SELECTIONFILTERBIT_HYPERNURBS							 = 16,
	SELECTIONFILTERBIT_LIGHT									 = 256,
	SELECTIONFILTERBIT_NULL										 = 1,
	SELECTIONFILTERBIT_OTHER									 = 2048,
	SELECTIONFILTERBIT_PARTICLE								 = 1024,
	SELECTIONFILTERBIT_POLYGON								 = 2,
	SELECTIONFILTERBIT_SCENE									 = 512,
	SELECTIONFILTERBIT_SPLINE									 = 4,
	SENDMAIL_SENDDIRECTLY											 = 1,
	SEQDESCRIPTION_OFFSET											 = 500000000,
	SERIAL_BODYPAINT													 = 1,
	SERIAL_CINEMA4D														 = 0,
	SETTINGS_ANIMATIONSYSTEM									 = 7,
	SETTINGS_COORDINATEMANAGER								 = 5,
	SETTINGS_DOCUMENT													 = 2,
	SETTINGS_GENERAL													 = 0,
	SETTINGS_MATERIALMANAGER									 = 4,
	SETTINGS_MODELER													 = 1,
	SETTINGS_OBJECTMANAGER										 = 3,
	SETTINGS_STRUCTUREMANAGER									 = 6,
	SETTING_AUTOIDENT													 = 1,
	SETTING_HEIGHT														 = 2,
	SEbase																		 = 5422,
	SHADERPOPUP_CLEARSHADER										 = 99992,
	SHADERPOPUP_COPYCHANNEL										 = 99995,
	SHADERPOPUP_CREATENEWTEXTURE							 = 99993,
	SHADERPOPUP_EDITIMAGE											 = 99997,
	SHADERPOPUP_EDITPARAMS										 = 99999,
	SHADERPOPUP_LOADIMAGE											 = 99991,
	SHADERPOPUP_PASTECHANNEL									 = 99994,
	SHADERPOPUP_RELOADIMAGE										 = 99998,
	SHADERPOPUP_SETFILENAME										 = 99990,
	SHADERPOPUP_SETSHADER											 = 99989,
	SHADER_ALPHA															 = 8,
	SHADER_ASYNC_EX														 = 16777216,
	SHADER_CHANGENORMAL												 = 8192,
	SHADER_DISPLACE														 = 16384,
	SHADER_DUDVREQUIRED												 = 2097152,
	SHADER_ENVREQUIRED												 = 1048576,
	SHADER_EVALUATEPROJECTION									 = 67108864,
	SHADER_INITCALCULATION										 = -2147483647,
	SHADER_MIPSAT															 = 33554432,
	SHADER_RAYTRACING_EX											 = 1,
	SHADER_REFLECTANCE												 = 2,
	SHADER_SOFTSHADOW													 = 0,
	SHADER_TRANSFORM													 = 16,
	SHADER_TRANSFORM_N												 = 32,
	SHADER_TRANSPARENCY												 = 4,
	SHADER_VOLUMETRIC													 = 536870912,
	SIDE_BACK																	 = 2,
	SIDE_FRONT																 = 1,
	SIDE_FRONT_AND_BACK												 = 0,
	SLV_MULTIPLESELECTION											 = 1,
	SNFLAG_OWN																 = 1,
	SNFLAG_PRIV																 = 2,
	SN_EXPIRED																 = -2,
	SN_EXPIRE_14															 = -100,
	SN_NET																		 = -4,
	SN_OKAY																		 = 0,
	SN_WRONGNUMBER														 = -1,
	SOUNDCOMMAND_DRAGCONTINUE									 = 2,
	SOUNDCOMMAND_DRAGSTART										 = 1,
	SOUNDCOMMAND_START												 = 0,
	SOUNDKEY_BALANCE													 = 1002,
	SOUNDKEY_VOLUME														 = 1001,
	SPECULAR_HEIGHT														 = 1017,
	SPECULAR_MODE															 = 1018,
	SPECULAR_WIDTH														 = 1016,
	SPLINECONTROL_GRIDLINES_H									 = 10014,
	SPLINECONTROL_GRIDLINES_V									 = 10015,
	SPLINECONTROL_GRID_H											 = 10000,
	SPLINECONTROL_GRID_V											 = 10001,
	SPLINECONTROL_HIDE_GRID_H									 = 10026,
	SPLINECONTROL_HIDE_GRID_V									 = 10027,
	SPLINECONTROL_HIDE_PRESET_BTN							 = 10028,
	SPLINECONTROL_HIDE_ROUND_SLIDER						 = 10029,
	SPLINECONTROL_HIDE_VALUE_EDIT_H						 = 10030,
	SPLINECONTROL_HIDE_VALUE_EDIT_V						 = 10031,
	SPLINECONTROL_HIDE_VALUE_LABELS_H					 = 10032,
	SPLINECONTROL_HIDE_VALUE_LABELS_V					 = 10033,
	SPLINECONTROL_MINSIZE_H										 = 10016,
	SPLINECONTROL_MINSIZE_V										 = 10017,
	SPLINECONTROL_NEW_NO_HORIZ								 = 10024,
	SPLINECONTROL_NEW_NO_VERT									 = 10025,
	SPLINECONTROL_PRESET_BTN									 = 10012,
	SPLINECONTROL_ROUND_SLIDER								 = 10013,
	SPLINECONTROL_VALUE_EDIT_H								 = 10002,
	SPLINECONTROL_VALUE_EDIT_V								 = 10003,
	SPLINECONTROL_VALUE_LABELS_H							 = 10004,
	SPLINECONTROL_VALUE_LABELS_V							 = 10005,
	SPLINECONTROL_X_MAX												 = 10007,
	SPLINECONTROL_X_MAX_TEXT									 = 10019,
	SPLINECONTROL_X_MIN												 = 10006,
	SPLINECONTROL_X_MIN_TEXT									 = 10018,
	SPLINECONTROL_X_STEPS											 = 10008,
	SPLINECONTROL_X_TEXT											 = 10022,
	SPLINECONTROL_Y_MAX												 = 10010,
	SPLINECONTROL_Y_MAX_TEXT									 = 10021,
	SPLINECONTROL_Y_MIN												 = 10009,
	SPLINECONTROL_Y_MIN_TEXT									 = 10020,
	SPLINECONTROL_Y_STEPS											 = 10011,
	SPLINECONTROL_Y_TEXT											 = 10023,
	STAGEOBJECT_BNAME													 = 999,
	STAGEOBJECT_CNAME													 = 999,
	STAGEOBJECT_ENAME													 = 999,
	STAGEOBJECT_FNAME													 = 999,
	STAGEOBJECT_SNAME													 = 999,
	ST_CDATA																	 = 3,
	ST_CLASS																	 = 1,
	ST_CMETHOD																 = 7,
	ST_DATA																		 = 2,
	ST_FUNCTION																 = 5,
	ST_METHOD																	 = 6,
	ST_PDATA																	 = 4,
	ST_PMETHOD																 = 8,
	SUNEXPRESSIONTAG_DATE											 = 1001,
	SUNEXPRESSIONTAG_TIME											 = 1002,
	SYSTEMINFO_NOGUI													 = 2,
	SYSTEMINFO_OSX														 = 1,
	Sbase																			 = 5222,
	Sdata																			 = 5225,
	SlowIdentification												 = 1,
	Smarker																		 = 5215,
	Smorph																		 = 5210,
	Sp2p																			 = 5226,
	SpecificItemType													 = 2,
	Spla																			 = 5214,
	Splugin																		 = 5221,
	Ssound																		 = 5217,
	Svalue																		 = 5224,
	TAB_CYCLE																	 = 2,
	TAB_NOSELECT															 = 1,
	TAB_RADIO																	 = 3,
	TAB_TABS																	 = 0,
	TAG_ANCHOR																 = 5608,
	TAG_BAKEPARTICLE													 = 5685,
	TAG_COFFEEEXPRESSION											 = 5675,
	TAG_COMPOSITING														 = 5637,
	TAG_DISPLAY																 = 5613,
	TAG_EXPRESSION_PLUGIN											 = 5687,
	TAG_FIXEXPRESSION													 = 5677,
	TAG_HERMITE																 = 5617,
	TAG_IKEXPRESSION													 = 5679,
	TAG_KINEMATIC															 = 5614,
	TAG_METABALL															 = 5684,
	TAG_MOTIONBLUR														 = 5636,
	TAG_PARTICLE															 = 5630,
	TAG_PHONG																	 = 5612,
	TAG_PLUGIN																 = 5686
};
enum
{
	TARGETKEY_NAME															= 1,
	TEXBOX_ALPHAMODE														= 1952541808,
	TEXBOX_POPUP_ONLY														= 1952608367,
	TEXTUREKEY_LENX															= 1,
	TEXTUREKEY_LENY															= 1,
	TEXTUREKEY_MIX															= 1,
	TEXTUREKEY_OX																= 1,
	TEXTUREKEY_OY																= 1,
	TEXTUREKEY_PROJECTION												= 1,
	TEXTUREKEY_RESTRICTION											= 1,
	TEXTUREKEY_SEAMLESS													= 1,
	TEXTUREKEY_SIDE															= 1,
	TEXTUREKEY_TILE															= 1,
	TEXTURETAG_LENX															= 1002,
	TEXTURETAG_LENY															= 1003,
	TEXTURETAG_OX																= 1000,
	TEXTURETAG_OY																= 1001,
	TEXTURE_COLOR																= 1004,
	TEXTURE_FILEFORMAT													= 1000,
	TEXTURE_HEIGHT															= 1002,
	TEXTURE_MODE																= 1003,
	TEXTURE_SAVE_IMMEDIATLY											= 1005,
	TEXTURE_WIDTH																= 1001,
	TEX_ALPHA																		= 4,
	TEX_BUMP																		= 8,
	TEX_MIRROR																	= 2,
	TEX_TILE																		= 1,
	TEbase																			= 5322,
	THINKINGPARTICLES_PRIORITY									= 900,
	THINKINGPARTICLES_VERSION										= 0,
	THREADPRIORITY_ABOVE												= 1000,
	THREADPRIORITY_BELOW												= 1001,
	THREADPRIORITY_NORMAL												= 0,
	THREADTYPE_EDITORREDRAW											= 1,
	THREADTYPE_RENDEREDITOR											= 2,
	THREADTYPE_RENDEREXTERNAL										= 4,
	TILE_BM_READ_ONLY														= 513,
	TILE_BM_READ_SCRATCH												= 516,
	TILE_BM_READ_WRITE													= 515,
	TILE_BM_SCRATCH															= 512,
	TILE_BM_WRITE																= 514,
	TILE_LAST_RESULT														= 4,
	TILE_MASK_READ_ONLY													= 257,
	TILE_MASK_READ_WRITE												= 259,
	TILE_MASK_SCRATCH														= 256,
	TILE_MASK_WRITE															= 258,
	TILE_MODE_READ															= 1,
	TILE_MODE_READ_SCRATCH											= 4,
	TILE_MODE_READ_WRITE												= 3,
	TILE_MODE_SCRATCH														= 0,
	TILE_MODE_WRITE															= 2,
	TILE_REPEAT_BORDER													= 1,
	TILE_REPEAT_TILING													= 2,
	TILE_TYPE_BM																= 512,
	TILE_TYPE_MASK															= 256,
	TIMEKEY_P																		= 1000,
	TIMEKEY_VL																	= 1001,
	TIMEKEY_VR																	= 1002,
	TIMELINE_CHANGED														= -1001,
	TP_GETPGROUP_ALL														= 0,
	TP_GETPGROUP_SELECTED												= 2,
	TP_GETPGROUP_WITHPARTICLES									= 1,
	TP_INSERT_AFTER															= 4,
	TP_INSERT_BEFORE														= 3,
	TP_INSERT_NONE															= 0,
	TP_INSERT_UNDERFIRST												= 1,
	TP_INSERT_UNDERLAST													= 2,
	TP_MSG_DCHANNEL_CHANGED											= 1000003,
	TP_MSG_DCHANNEL_PREREMOVE										= 1000002,
	TP_MSG_GET_COLLISIONINTERFACE								= 1000006,
	TP_MSG_GET_PSHAPEINTERFACE									= 1000005,
	TP_MSG_OBJECT_GETPARTICLES									= 1000004,
	TP_MSG_PGROUP_PREREMOVE											= 1000000,
	TP_MSG_PGROUP_TREECHANGED										= 1000001,
	TP_MSG_START_ID															= 1000000,
	TRACK_ALIGNTOPATH														= 5207,
	TRACK_ALIGNTOSPLINE													= 5205,
	TRACK_FADE																	= 5201,
	TRACK_FOLDER																= 5216,
	TRACK_KINEMATIC															= 5206,
	TRACK_MARKER																= 5215,
	TRACK_MORPH																	= 5210,
	TRACK_PARAMETER															= 5220,
	TRACK_PLA																		= 5214,
	TRACK_PLUGIN																= 5213,
	TRACK_POSITION															= 5202,
	TRACK_PULSATE																= 5211,
	TRACK_ROTATION															= 5204,
	TRACK_SCALE																	= 5203,
	TRACK_SOUND																	= 5217,
	TRACK_TARGET																= 5208,
	TRACK_TEXTURE																= 5209,
	TRACK_TIME																	= 5219,
	TRACK_VIBRATE																= 5212,
	TRANSMAP_FLAG_CLONE													= 2,
	TRANSMAP_FLAG_DELETED												= 8,
	TRANSMAP_FLAG_MOVED													= 4,
	TRANSMAP_FLAG_NEW														= 1,
	TRANSMAP_FLAG_UNION													= 16,
	TRANSMAP_PLYFLAG_MELTED											= 1048576,
	TRANSMAP_PLYFLAG_PGON												= 2097152,
	TRANSMAP_PNTFLAG_DELTA											= 2097152,
	TRANSMAP_PNTFLAG_INTER											= 1048576,
	TRANSMAP_PNTFLAG_WELDED											= 4194304,
	TRANSPARENCY_ADDITIVE												= 1004,
	TRANSPARENCY_FRESNEL												= 1003,
	TRANSPARENCY_N															= 1005,
	TREEVIEW_ALT_DRAG														= 1953915236,
	TREEVIEW_BORDER															= 1651663986,
	TREEVIEW_CTRL_DRAG													= 1668572274,
	TREEVIEW_FIXED_LAYOUT												= 1953916524,
	TREEVIEW_HAS_HEADER													= 1953917032,
	TREEVIEW_HIDE_LINES													= 1953917036,
	TREEVIEW_MOVE_COLUMN												= 1953918307,
	TREEVIEW_NO_BACK_DELETE											= 1953918562,
	TREEVIEW_NO_MULTISELECT											= 1953918573,
	TREEVIEW_NO_OPEN_CTRLCLK										= 1953915748,
	TREEVIEW_OUTSIDE_DROP												= 1953918820,
	TREEVIEW_RESIZE_HEADER											= 1953919592,
	TRIANGSTATE_MISSING_CONSTRAINT							= 2,
	TRIANGSTATE_SKIP_EDGE												= 1,
	T_LOGIC_COPY																= 0,
	T_PREPARELINE																= -2147483647,
	Takima																			= 2,
	TargetNotFound															= 536870912,
	Tbspline																		= 3,
	Tcubic																			= 1,
	Thaircollider																= 1017326,
	Thairmaterial																= 1017729,
	Thairselection															= 1017746,
	Thairtangent																= 1017671,
	Thairvertex																	= 1018329,
	Thermite																		= 4,
	ThreeDFile																	= 9,
	UNDO_ACTIVATE																= 46,
	UNDO_CHANGE																	= 40,
	UNDO_CHANGE_NOCHILDS												= 41,
	UNDO_CHANGE_SELECTION												= 43,
	UNDO_CHANGE_SMALL														= 42,
	UNDO_DEACTIVATE															= 47,
	UNDO_DELETE																	= 45,
	UNDO_KEY_DATA																= 40,
	UNDO_KEY_DEL																= 45,
	UNDO_KEY_NEW																= 44,
	UNDO_MATERIAL_DATA													= 40,
	UNDO_MATERIAL_DEL														= 45,
	UNDO_MATERIAL_NEW														= 44,
	UNDO_NEW																		= 44,
	UNDO_OBJECT																	= 41,
	UNDO_OBJECT_ACTIVE													= 46,
	UNDO_OBJECT_BASEDATA												= 40,
	UNDO_OBJECT_BASEDATA_REC										= 42,
	UNDO_OBJECT_DEL															= 45,
	UNDO_OBJECT_NEW															= 44,
	UNDO_OBJECT_REC															= 40,
	UNDO_OBJECT_SELECTION												= 43,
	UNDO_SEQUENCE_DATA													= 40,
	UNDO_SEQUENCE_DEL														= 45,
	UNDO_SEQUENCE_NEW														= 44,
	UNDO_TAG_DATA																= 40,
	UNDO_TAG_DEL																= 45,
	UNDO_TAG_NEW																= 44,
	UNDO_TLACTIVATE															= 48,
	UNDO_TLDEACTIVATE														= 49,
	UNDO_TRACK_DATA															= 40,
	UNDO_TRACK_DEL															= 45,
	UNDO_TRACK_NEW															= 44,
	UNIT_CM																			= 3,
	UNIT_FEET																		= 9,
	UNIT_INCH																		= 10,
	UNIT_KM																			= 1,
	UNIT_M																			= 2,
	UNIT_MILE																		= 7,
	UNIT_MM																			= 4,
	UNIT_NM																			= 6,
	UNIT_NONE																		= 0,
	UNIT_UM																			= 5,
	UNIT_YARD																		= 8,
	UNUSED_COLOR_FG_MM_C_ACTIVE_LAYER_ACT_TEX		= 1195,
	UNUSED_COLOR_FG_MM_C_ACTIVE_LAYER_INACT_TEX = 1196
};
enum
{
	USERAREA_COREMESSAGE						 = 4,
	USERAREA_HANDLEFOCUS						 = 2,
	USERAREA_SYNCMESSAGE						 = 8,
	USERAREA_TABSTOP								 = 1,
	UVCOMMAND_COPY_UV								 = 10004,
	UVCOMMAND_FIT_TO_CANVAS					 = 10011,
	UVCOMMAND_FLIP_SEQUENCE					 = 10007,
	UVCOMMAND_MAX_UV								 = 10006,
	UVCOMMAND_MOVE_DOWN_SEQUENCE		 = 10008,
	UVCOMMAND_MOVE_UP_SEQUENCE			 = 10009,
	UVCOMMAND_OPTIMALCUBICMAPPING		 = 10001,
	UVCOMMAND_OPTIMALMAPPING				 = 10000,
	UVCOMMAND_PASTE_UV							 = 10005,
	UVCOMMAND_REALIGN								 = 10002,
	UVCOMMAND_RELAX									 = 10003,
	UVCOMMAND_TERRACE								 = 10010,
	UVCOMMAND_TRANSFORM							 = 10012,
	UVCOMMAND_TRANSFORM_ANGLE				 = 1024,
	UVCOMMAND_TRANSFORM_MOVE_X			 = 1020,
	UVCOMMAND_TRANSFORM_MOVE_Y			 = 1021,
	UVCOMMAND_TRANSFORM_SCALE_X			 = 1022,
	UVCOMMAND_TRANSFORM_SCALE_Y			 = 1023,
	UV_COMMAND_RELAX_KEEP_BORDER		 = 1010,
	UV_COMMAND_RELAX_KEEP_NEIGHBORS	 = 1011,
	UV_COMMAND_RELAX_PIN_POINTS			 = 1012,
	UnsupportedNodeType							 = -1610612736,
	UserCancelled										 = -1073741824,
	UserPresetDb										 = 1,
	VARIABLE_ARRAY_SIZE							 = 1,
	VECTOR2D_BORDER									 = 1651663986,
	VERSION_BENCHMARK								 = 64,
	VERSION_BODYPAINT2							 = 128,
	VERSION_BODYPAINT								 = 2,
	VERSION_CINEMA4D								 = 1,
	VERSION_DEMO										 = 16,
	VERSION_MODELER									 = 256,
	VERSION_NET											 = 4,
	VERSION_OPERATOR_ABS						 = 2,
	VERSION_OPERATOR_ADAPTER				 = 2,
	VERSION_OPERATOR_BITMAP					 = 2,
	VERSION_OPERATOR_BOOL						 = 2,
	VERSION_OPERATOR_BOX						 = 2,
	VERSION_OPERATOR_CLAMP					 = 2,
	VERSION_OPERATOR_CMP						 = 2,
	VERSION_OPERATOR_COFFEE					 = 2,
	VERSION_OPERATOR_COLLISION			 = 2,
	VERSION_OPERATOR_COLORSPACE			 = 2,
	VERSION_OPERATOR_COLORTEMP			 = 2,
	VERSION_OPERATOR_COMMENT				 = 2,
	VERSION_OPERATOR_CONDITION			 = 2,
	VERSION_OPERATOR_CONST					 = 2,
	VERSION_OPERATOR_CROSS					 = 2,
	VERSION_OPERATOR_DEGREE					 = 2,
	VERSION_OPERATOR_DETAILS				 = 2,
	VERSION_OPERATOR_DISTANCE				 = 2,
	VERSION_OPERATOR_DOT						 = 2,
	VERSION_OPERATOR_DUMMY					 = 0,
	VERSION_OPERATOR_EQU						 = 2,
	VERSION_OPERATOR_FLOATFUNC			 = 2,
	VERSION_OPERATOR_FLOATMATH			 = 2,
	VERSION_OPERATOR_FORMULA				 = 2,
	VERSION_OPERATOR_FREEZE					 = 2,
	VERSION_OPERATOR_GROUP					 = 5,
	VERSION_OPERATOR_HIERARCHY			 = 3,
	VERSION_OPERATOR_INCLUDE				 = 3,
	VERSION_OPERATOR_INV						 = 2,
	VERSION_OPERATOR_ITERATE				 = 4,
	VERSION_OPERATOR_LINK						 = 2,
	VERSION_OPERATOR_MATERIAL				 = 3,
	VERSION_OPERATOR_MATH						 = 2,
	VERSION_OPERATOR_MATRIX2VECT		 = 2,
	VERSION_OPERATOR_MATRIXCALCHPB	 = 2,
	VERSION_OPERATOR_MATRIXMULVECTOR = 2,
	VERSION_OPERATOR_MEMORY					 = 2,
	VERSION_OPERATOR_MIX						 = 2,
	VERSION_OPERATOR_MONOFLOP				 = 2,
	VERSION_OPERATOR_NEG						 = 2,
	VERSION_OPERATOR_NIL						 = 2,
	VERSION_OPERATOR_NOISE					 = 2,
	VERSION_OPERATOR_NOT						 = 2,
	VERSION_OPERATOR_OBJECT					 = 2,
	VERSION_OPERATOR_ORDER					 = 2,
	VERSION_OPERATOR_POINT					 = 2,
	VERSION_OPERATOR_POLYGON				 = 2,
	VERSION_OPERATOR_RANDOM					 = 2,
	VERSION_OPERATOR_RANGEMAPPER		 = 2,
	VERSION_OPERATOR_RAY						 = 2,
	VERSION_OPERATOR_REAL2VECT			 = 2,
	VERSION_OPERATOR_REFERENCE			 = 2,
	VERSION_OPERATOR_RESULT					 = 2,
	VERSION_OPERATOR_SELECTION			 = 3,
	VERSION_OPERATOR_SOUND					 = 2,
	VERSION_OPERATOR_SPLINE					 = 2,
	VERSION_OPERATOR_SPY						 = 2,
	VERSION_OPERATOR_SWITCH					 = 2,
	VERSION_OPERATOR_TAG						 = 3,
	VERSION_OPERATOR_TIME						 = 2,
	VERSION_OPERATOR_TRIGGER				 = 2,
	VERSION_OPERATOR_TRIGO					 = 2,
	VERSION_OPERATOR_VECT2MATRIX		 = 2,
	VERSION_OPERATOR_VECT2REAL			 = 2,
	VERSION_OPERATOR_VECTCALCMATRIX	 = 2,
	VERSION_OPERATOR_WEIGHTMAP			 = 2,
	VERSION_SERVER									 = 8,
	VFLAG_EXTERNALRENDERER					 = 4,
	VFLAG_INTERNALRENDERER					 = 2,
	VFLAG_ISOPARM										 = 8,
	VFLAG_POLYGONAL									 = 16,
	VIBRATEKEY_FREQUENCY						 = 1,
	VIBRATEKEY_POS_V								 = 1,
	VIBRATEKEY_ROT_V								 = 1,
	VIBRATEKEY_SCALE_V							 = 1,
	VIBRATEKEY_SEED									 = 1,
	VIDEOPOST_CALCSHADOW						 = 512,
	VIDEOPOST_CALCVOLUMETRIC				 = 256,
	VIDEOPOST_CUSTOMLENS						 = 1024,
	VIDEOPOST_EXECUTELINE						 = 16,
	VIDEOPOST_EXECUTEPIXEL					 = 32,
	VIDEOPOST_RAYTRACING_EX					 = 2,
	VIDEOPOST_REFRESH								 = 4,
	VIDEOPOST_REQUEST_MOTIONGEOMETRY = 128,
	VIDEOPOST_REQUEST_MOTIONMATRIX	 = 64,
	VIDEOPOST_STOREFRAGMENTS				 = 1,
	VIEWPORT_CLEAR_EDGE							 = 4,
	VIEWPORT_CLEAR_POINT						 = 1,
	VIEWPORT_CLEAR_POLYGON					 = 2,
	VIEWPORT_USE_DEFORMERS					 = 2,
	VIEWPORT_USE_HN									 = 1,
	VISIBILITYKEY_VALUE							 = 1,
	VPBUFFER_ALLPOSTEFFECTS					 = 12,
	VPBUFFER_ALPHA									 = 1002,
	VPBUFFER_AMBIENT								 = 2,
	VPBUFFER_AMBIENTOCCLUSION				 = 113,
	VPBUFFER_ATMOSPHERE							 = 10,
	VPBUFFER_ATMOSPHERE_MUL					 = 11,
	VPBUFFER_BLEND									 = 1000,
	VPBUFFER_CAUSTICS								 = 9,
	VPBUFFER_DEPTH									 = 100,
	VPBUFFER_DIFFUSE								 = 3,
	VPBUFFER_ILLUMINATION						 = 109,
	VPBUFFER_IMAGEBUILDING_PASS			 = 32,
	VPBUFFER_LIGHTBLEND							 = 1001,
	VPBUFFER_MAT_COLOR							 = 101,
	VPBUFFER_MAT_DIFFUSION					 = 102,
	VPBUFFER_MAT_ENVIRONMENT				 = 106,
	VPBUFFER_MAT_LUMINANCE					 = 103,
	VPBUFFER_MAT_NORMAL							 = 114,
	VPBUFFER_MAT_REFLECTION					 = 105,
	VPBUFFER_MAT_SPECULAR						 = 107,
	VPBUFFER_MAT_SPECULARCOLOR			 = 108,
	VPBUFFER_MAT_TRANSPARENCY				 = 104,
	VPBUFFER_MAT_UV									 = 115,
	VPBUFFER_MOTIONVECTOR						 = 116,
	VPBUFFER_OBJECTBUFFER						 = 110,
	VPBUFFER_POSTEFFECT							 = 111,
	VPBUFFER_POSTEFFECT_MUL					 = 112,
	VPBUFFER_RADIOSITY							 = 8,
	VPBUFFER_REFLECTION							 = 6,
	VPBUFFER_RGBA										 = 1,
	VPBUFFER_SHADOW									 = 5,
	VPBUFFER_SPECULAR								 = 4,
	VPBUFFER_TRANSPARENCY						 = 7,
	VPGETFRAGMENTS_N								 = 2,
	VPGETFRAGMENTS_Z_P							 = 1,
	VPGETINFO_BITDEPTH							 = 2,
	VPGETINFO_CPP										 = 3,
	VPGETINFO_LINEOFFSET						 = 5,
	VPGETINFO_VISIBLE								 = 4,
	VPGETINFO_XRESOLUTION						 = 0,
	VPGETINFO_YRESOLUTION						 = 1,
	VPPRIORITY_COLORCORRECTION			 = 500,
	VPPRIORITY_COMIC								 = 600,
	VPPRIORITY_DEPTHOFFIELD					 = 300,
	VPPRIORITY_EXTERNAL							 = 100,
	VPPRIORITY_FILTERMEDIAN					 = 100,
	VPPRIORITY_FILTERSHARPEN				 = 100,
	VPPRIORITY_FILTERSOFT						 = 100,
	VPPRIORITY_LENSEFFECTS					 = 200,
	VPPRIORITY_OBJECTGLOW						 = 500,
	VPPRIORITY_OBJECTMOTIONBLUR			 = 400,
	VPPRIORITY_SCENEMOTIONBLUR			 = 100,
	VP_FRAME												 = 2,
	VP_FRAMESEQUENCE								 = 1,
	VP_INNER												 = 5,
	VP_PREPASS											 = 6,
	VP_RENDER												 = 4,
	VP_SUBFRAME											 = 3,
	VPbase													 = 5709,
	VirtualDirectory								 = 2,
	VirtualFile											 = 4,
	WIN_DLG													 = 2,
	WIN_FULLSCREEN									 = 7,
	WIN_POPUP												 = 3,
	WIN_WINDOW2											 = 4,
	WIN_WINDOW3											 = 5,
	WIN_WINDOW4											 = 6,
	WIN_WINDOW											 = 1,
	WPREF_3DGRID										 = 1032,
	WPREF_ACTION_FAC								 = 1020,
	WPREF_ACTION_SAFE								 = 1016,
	WPREF_ACTIVEBORDER							 = 21007,
	WPREF_ACTIVEBOX_COL							 = 10026,
	WPREF_ACTIVECHILDBOX_COL				 = 10051,
	WPREF_ACTIVEPOINT_COL						 = 10019,
	WPREF_ACTIVEPOLYBOX_COL					 = 10052,
	WPREF_ACTIVEPOLYCHILDBOX_COL		 = 10053,
	WPREF_ACTIVEPOLYGON_COL					 = 10022,
	WPREF_ALLOWBUGREPORTS						 = 1068,
	WPREF_ANIMPATH_COL							 = 10027,
	WPREF_AXIS_BAND_COL							 = 10056,
	WPREF_BACKFACING_COL						 = 10060,
	WPREF_BACKGROUND_COL						 = 10007,
	WPREF_BACKUP										 = 1003,
	WPREF_BONE_COL									 = 10017,
	WPREF_BUBBLEHELP1								 = 21001,
	WPREF_BUBBLEHELP2								 = 21002,
	WPREF_CAMERAROTATION						 = 1020,
	WPREF_CAMERA_COL								 = 10014,
	WPREF_CENTER										 = 1002,
	WPREF_CENTERAXIS								 = 1044,
	WPREF_COLOR_BG									 = 20025,
	WPREF_COLOR_BGEDIT							 = 20026,
	WPREF_COLOR_BGFOCUS							 = 20028,
	WPREF_COLOR_BGGADGET						 = 20027,
	WPREF_COLOR_BGSELTEXT						 = 20035,
	WPREF_COLOR_BGTRISTATE					 = 20037,
	WPREF_COLOR_DBARBG1							 = 20032,
	WPREF_COLOR_DBARBG2							 = 20034,
	WPREF_COLOR_DBARFG1							 = 20031,
	WPREF_COLOR_DBARFG2							 = 20033,
	WPREF_COLOR_EDGEBL							 = 20023,
	WPREF_COLOR_EDGEDK							 = 20022,
	WPREF_COLOR_EDGELT							 = 20021,
	WPREF_COLOR_EDGEWH							 = 20020,
	WPREF_COLOR_FGSELTEXT						 = 20036,
	WPREF_COLOR_HRANGE							 = 1103,
	WPREF_COLOR_MIXING_1						 = 1301,
	WPREF_COLOR_MIXING_2						 = 1302,
	WPREF_COLOR_MIXING_3						 = 1303,
	WPREF_COLOR_MIXING_4						 = 1304,
	WPREF_COLOR_MIX_BP							 = 1108,
	WPREF_COLOR_MIX_C4D							 = 1107,
	WPREF_COLOR_QUICKPRESET					 = 1200,
	WPREF_COLOR_QUICKPRESET_LAST		 = 1299,
	WPREF_COLOR_QUICK_BP						 = 1106,
	WPREF_COLOR_QUICK_C4D						 = 1105,
	WPREF_COLOR_RGBRANGE						 = 1102,
	WPREF_COLOR_SVRANGE							 = 1104,
	WPREF_COLOR_SYSTEM_BP						 = 1101,
	WPREF_COLOR_SYSTEM_C4D					 = 1100,
	WPREF_COLOR_TEXT								 = 20029
};
enum
{
	WPREF_COLOR_TEXTFOCUS									= 20030,
	WPREF_COLOR_TIMESLIDER								= 20024,
	WPREF_CPUCOUNT												= 1046,
	WPREF_DELAY_CMDPOPUP									= 21012,
	WPREF_DELAY_DRAGSCROLL								= 21013,
	WPREF_DELAY_MENU											= 21010,
	WPREF_DELAY_MENUSCROLL								= 21011,
	WPREF_DELAY_SCROLLER1									= 21014,
	WPREF_DELAY_SCROLLER2									= 21015,
	WPREF_DELAY_TABSWITCH									= 21016,
	WPREF_DELETEATQUADS										= 1015,
	WPREF_DESCRIPTIONLIMIT								= 1049,
	WPREF_DIALOGALIGN											= 21009,
	WPREF_DIALOGSTYLE											= 21008,
	WPREF_DRAWCACHE												= 1033,
	WPREF_FILEPATH_ALL										= 90000,
	WPREF_FILEPATH_BP											= 90022,
	WPREF_FILEPATH_CAT										= 90005,
	WPREF_FILEPATH_COF										= 90007,
	WPREF_FILEPATH_DOC										= 90003,
	WPREF_FILEPATH_FCV										= 90008,
	WPREF_FILEPATH_MAT										= 90002,
	WPREF_FILEPATH_PRF										= 90004,
	WPREF_FILEPATH_TEX										= 90001,
	WPREF_FILEPATH_WAV										= 90006,
	WPREF_FILMFORMAT_COL									= 10008,
	WPREF_FONT_MONO												= 21051,
	WPREF_FONT_STANDARD										= 21050,
	WPREF_FRONTFACING_COL									= 10059,
	WPREF_GRID_MAJOR_COL									= 10010,
	WPREF_GRID_MINOR_COL									= 10011,
	WPREF_HORIZON_COL											= 10009,
	WPREF_HPB_SYSTEM											= 1001,
	WPREF_INACTIVEBORDER									= 21006,
	WPREF_INACTIVEPOINT_COL								= 10020,
	WPREF_INACTIVEPOLYGON_COL							= 10023,
	WPREF_INSERTAT												= 1016,
	WPREF_LAYER0_COL											= 10035,
	WPREF_LAYER1_COL											= 10036,
	WPREF_LAYER2_COL											= 10037,
	WPREF_LAYER3_COL											= 10038,
	WPREF_LAYER4_COL											= 10039,
	WPREF_LAYER5_COL											= 10040,
	WPREF_LAYER6_COL											= 10041,
	WPREF_LAYER7_COL											= 10042,
	WPREF_LINK_SELECTION									= 1009,
	WPREF_LINUX_BROWSERPATH								= 21070,
	WPREF_LINUX_IMAGEEDITPATH							= 21077,
	WPREF_LOAD_MANAGER										= 1011,
	WPREF_MAC_ALLOWSTOPSLEEP							= 21064,
	WPREF_MAC_EXCHNGCTRLCMD								= 21062,
	WPREF_MATPREVIEW_AUTO_UPDATE					= 1054,
	WPREF_MATPREVIEW_DEFAULTOBJECT_MAT		= 1050,
	WPREF_MATPREVIEW_DEFAULTOBJECT_SHD		= 1052,
	WPREF_MATPREVIEW_DEFAULTSIZE					= 1048,
	WPREF_MATPREVIEW_DEFAULTUSERSCENE_MAT = 1051,
	WPREF_MATPREVIEW_DEFAULTUSERSCENE_SHD = 1053,
	WPREF_MATPREVIEW_FPS									= 1056,
	WPREF_MATPREVIEW_LENGTH								= 1057,
	WPREF_MATPREVIEW_MAX_MEM							= 1058,
	WPREF_MATPREVIEW_REDRAW_TIME					= 1055,
	WPREF_MAXHNWEIGHT_COL									= 10065,
	WPREF_MAXSOFTSELECT_COL								= 10062,
	WPREF_MAX_BACKUP											= 1043,
	WPREF_MAX_LAST												= 1019,
	WPREF_MAX_UNDOS												= 1018,
	WPREF_MENUICON												= 21004,
	WPREF_MENUSHORTCUT										= 21005,
	WPREF_MINHNWEIGHT_COL									= 10063,
	WPREF_MINSOFTSELECT_COL								= 10061,
	WPREF_MODIFIER_COL										= 10018,
	WPREF_MOUSEOVER_AUTO									= 21071,
	WPREF_MOUSEOVER_SHORTCUT							= 21072,
	WPREF_MOUSEOVER_TIME									= 21073,
	WPREF_MULTISPLINEDRAW									= 1021,
	WPREF_NAVSERVICE											= 20061,
	WPREF_NGONLINE_COL										= 10058,
	WPREF_NORMALS													= 10050,
	WPREF_NORMAL_COL											= 10050,
	WPREF_ONLINEHELP_MODE									= 21074,
	WPREF_ONLINEHELP_PATH									= 21075,
	WPREF_ONLINEHELP_URL									= 21076,
	WPREF_ONLY_ACTIVEVIEW									= 1010,
	WPREF_ONLY_ACTIVEVIEW_ANIMATION				= 1039,
	WPREF_OPENGL													= 1008,
	WPREF_OPENGL_AA_LINES									= 1023,
	WPREF_OPENGL_BACKIMAGEMODE						= 1063,
	WPREF_OPENGL_DUALPLANES_ACTIVE				= 1036,
	WPREF_OPENGL_DUALPLANES_ARB						= 1047,
	WPREF_OPENGL_DUALPLANES_HIGHLIGHT			= 1066,
	WPREF_OPENGL_FASTLIVESELECTION				= 1028,
	WPREF_OPENGL_FASTREDRAW								= 1027,
	WPREF_OPENGL_GLPOINTS									= 1037,
	WPREF_OPENGL_HIGHENDSHADING						= 1038,
	WPREF_OPENGL_LIGHTING									= 1035,
	WPREF_OPENGL_MIPMAP										= 1025,
	WPREF_OPENGL_PERSPECT									= 1024,
	WPREF_OPENGL_POLYLINES								= 1034,
	WPREF_OPENGL_ROTATEQUADS							= 1064,
	WPREF_OPENGL_SHAREDLISTS							= 1065,
	WPREF_PARTICLE_COL										= 10015,
	WPREF_PASTEAT													= 1017,
	WPREF_PATTERN_BG											= 21041,
	WPREF_PATTERN_BGEDIT									= 21042,
	WPREF_PATTERN_BGFOCUS									= 21044,
	WPREF_PATTERN_BGGADGET								= 21043,
	WPREF_PATTERN_DBARBG1									= 21045,
	WPREF_PATTERN_DBARBG2									= 21046,
	WPREF_PATTERN_TIMESLIDER							= 21040,
	WPREF_PATTERN_USE											= 21060,
	WPREF_PMODIFIER_COL										= 10016,
	WPREF_RATIO_X													= 10005,
	WPREF_RATIO_Y													= 10006,
	WPREF_REALTIMEMANAGER									= 1042,
	WPREF_REALTIMESPINNER									= 1040,
	WPREF_REFRESHTIME											= 10004,
	WPREF_RENDERMATS											= 1004,
	WPREF_RENDERMATS_EX										= 1004,
	WPREF_RENDER_SAFE											= 1015,
	WPREF_REWIND													= 1045,
	WPREF_SAVE_LAYOUT											= 1014,
	WPREF_SAVE_OPENGL											= 1012,
	WPREF_SAVE_PARTICLES									= 1013,
	WPREF_SCALE_AXES											= 1006,
	WPREF_SELECTION_PREVIEW_COL						= 10054,
	WPREF_SELECT_AXIS_COL									= 10034,
	WPREF_SEMI_FAC												= 1022,
	WPREF_SEMI_TRANSPARENCY								= 1007,
	WPREF_SHADEDWIRE_COL									= 10057,
	WPREF_SPLINEEND_COL										= 10013,
	WPREF_SPLINESTART_COL									= 10012,
	WPREF_SUBPIXELLIMIT										= 1062,
	WPREF_TABLET													= 1005,
	WPREF_TABLET_HIRES										= 1030,
	WPREF_TANGENT_COL											= 10021,
	WPREF_TEXTUREAXIS_COL									= 10025,
	WPREF_TEXTURE_COL											= 10024,
	WPREF_THREADPRIORITY									= 21003,
	WPREF_TITLE_FAC												= 1021,
	WPREF_TITLE_SAFE											= 1017,
	WPREF_UNITS_BASIC											= 10000,
	WPREF_UNITS_COLOR											= 10002,
	WPREF_UNITS_TIME											= 10001,
	WPREF_UNITS_USEUNITS									= 10003,
	WPREF_USE_QUICKTIME										= 1029,
	WPREF_USE_TEXTURES										= 1026,
	WPREF_UVMESH_ACTIVEPOINT_COL					= 10048,
	WPREF_UVMESH_ACTIVEPOLY_COL						= 10046,
	WPREF_UVMESH_GREYED_COL								= 10045,
	WPREF_UVMESH_INACTIVEAPOINT_COL				= 10049,
	WPREF_UVMESH_INACTIVEAPOLY_COL				= 10047,
	WPREF_UV_OPTIMAL_MAPPING_DATA					= 21066,
	WPREF_UV_PROJECTION_DATA							= 21067,
	WPREF_UV_RELAX_DATA										= 21065,
	WPREF_UV_TAB_SEL											= 21068,
	WPREF_UV_TRANSFORM_DATA								= 21069,
	WPREF_VERTEXEND_COL										= 10044,
	WPREF_VERTEXSTART_COL									= 10043,
	WPREF_VIEWGAMMA												= 1067,
	WPREF_WXAXIS_COL											= 10031,
	WPREF_WYAXIS_COL											= 10032,
	WPREF_WZAXIS_COL											= 10033,
	WPREF_XAXIS_COL												= 10028,
	WPREF_YAXIS_COL												= 10029,
	WPREF_ZAXIS_COL												= 10030,
	WPREF_ZEROHNWEIGHT_COL								= 10064,
	ZIP_FLAG_DIRECTORY										= 16,
	_SPLINECONTROL_DUMMY_									= 10034,
	_WPREF_COLOR_LAST_										= 10065,
	ddumdumdimdum													= 5,

	COFFEE_HEADERFILE_END_
};

/// @addtogroup WPREF_TAKE
/// @ingroup group_enumeration
/// @{
enum
{
	WPREF_TAKE_BASIC_GROUP = 1000,
	WPREF_TAKE_MAIN_AUTOTAKE,
	WPREF_TAKE_MANAGER_SHOWFILTER,
	WPREF_TAKE_MANAGER_SHOWSHOWSEARCH,
	WPREF_TAKE_MANAGER_TREEMODE,
	WPREF_TAKE_DEFAULT_FILETR_OBJ,
	WPREF_TAKE_DEFAULT_FILETR_TAG,
	WPREF_TAKE_DEFAULT_FILETR_MAT,
	WPREF_TAKE_DEFAULT_FILETR_SHD,
	WPREF_TAKE_DEFAULT_FILETR_LAY,
	WPREF_TAKE_DEFAULT_FILETR_OTH,
	WPREF_TAKE_DEFAULT_FILETR_VIS,
	WPREF_TAKE_DEFAULT_FILETR_TRN,
	WPREF_TAKE_DEFAULT_FILETR_PRM,
	WPREF_TAKE_DEFAULT_FILETR_CAM,
	WPREF_TAKE_DEFAULT_FILETR_RS,
};
/// @}

// flbase.h
enum
{
	// GROUPS
	ID_FIELDLAYER_FIELDINFO = 440000335, // SUBCONTAINER GROUP
	ID_FIELDLAYER_BASICGROUP = 805,			 // GROUP

	// PARAMETERS
	ID_FIELDLAYER_STRENGTH = 440000260,			// FLOAT
	ID_FIELDLAYER_BLENDINGMODE = 440000261, // INT CYCLE
	ID_FIELDLAYER_BLENDINGMODE_LIGHTEN = 0,
	ID_FIELDLAYER_BLENDINGMODE_DARKEN = 1,
	ID_FIELDLAYER_BLENDINGMODE_MULTIPLY = 2,
	ID_FIELDLAYER_BLENDINGMODE_AVERAGE = 3,
	ID_FIELDLAYER_BLENDINGMODE_ADD = 4,
	ID_FIELDLAYER_BLENDINGMODE_SUBTRACT = 5,
	ID_FIELDLAYER_BLENDINGMODE_DIFFERENCE = 6,
	ID_FIELDLAYER_BLENDINGMODE_NEGATION = 7,
	ID_FIELDLAYER_BLENDINGMODE_SCREEN = 8,
	ID_FIELDLAYER_BLENDINGMODE_EXCLUSION = 9,
	ID_FIELDLAYER_BLENDINGMODE_LEGACYOVERLAY = 10,
	ID_FIELDLAYER_BLENDINGMODE_SOFTLIGHT = 11,
	ID_FIELDLAYER_BLENDINGMODE_HARDLIGHT = 12,
	ID_FIELDLAYER_BLENDINGMODE_COLORDODGE = 13,
	ID_FIELDLAYER_BLENDINGMODE_COLORBURN = 14,
	ID_FIELDLAYER_BLENDINGMODE_LINEARDODGE = 15,
	ID_FIELDLAYER_BLENDINGMODE_LINEARBURN = 16,
	ID_FIELDLAYER_BLENDINGMODE_LINEARLIGHT = 17,
	ID_FIELDLAYER_BLENDINGMODE_VIVIDLIGHT = 18,
	ID_FIELDLAYER_BLENDINGMODE_PINLIGHT = 19,
	ID_FIELDLAYER_BLENDINGMODE_HARDMIX = 20,
	ID_FIELDLAYER_BLENDINGMODE_REFLECT = 21,
	ID_FIELDLAYER_BLENDINGMODE_GLOW = 22,
	ID_FIELDLAYER_BLENDINGMODE_PHOENIX = 23,
	ID_FIELDLAYER_BLENDINGMODE_LEVR = 24,
	ID_FIELDLAYER_BLENDINGMODE_COLOR = 25,
	ID_FIELDLAYER_BLENDINGMODE_COLOR_INV = 26,
	ID_FIELDLAYER_BLENDINGMODE_NORMAL = 27,
	ID_FIELDLAYER_BLENDINGMODE_LAYERMASK = 28,
	ID_FIELDLAYER_BLENDINGMODE_ABSLIGHTEN = 29,
	ID_FIELDLAYER_BLENDINGMODE_ABSDARKEN = 30,
	ID_FIELDLAYER_BLENDINGMODE_ABSOVERLAY = 31,
	ID_FIELDLAYER_BLENDINGMODE_TRUESUBTRACT = 32,
	ID_FIELDLAYER_BLENDINGMODE_OVERLAY = 33,
	ID_FIELDLAYER_ENABLED = 440000265,					// BOOLEAN
	ID_FIELDLAYER_ENABLE_VALUE = 440000306,			// BOOLEAN / SUB ID_FIELDLAYER_ENABLED
	ID_FIELDLAYER_ENABLE_COLOR = 440000307,			// BOOLEAN / SUB ID_FIELDLAYER_ENABLED
	ID_FIELDLAYER_ENABLE_DIRECTION = 440000308, // BOOLEAN / SUB ID_FIELDLAYER_ENABLED
	ID_FIELDLAYER_ENABLE_ROTATION = 440000309,	// BOOLEAN / SUB ID_FIELDLAYER_ENABLED

	ID_FIELDLAYER_DUMMY__
};

#pragma pack (pop)
}

#endif	// PRIVATE_SYMBOLS_H__
