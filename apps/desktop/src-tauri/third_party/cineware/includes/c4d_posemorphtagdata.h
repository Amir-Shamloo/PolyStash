/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_POSEMORPHTAGDATA_H__
#define C4D_POSEMORPHTAGDATA_H__

#include "c4d_nodedata.h"
#include "c4d_basetime.h"
#include "c4d_uvwstruct.h"
#include "c4d_misc/datastructures/basearray.h"
#include "c4d_baselink.h"
#include "c4d_customdatatype/customdatatype_itemtree.h"
#include "private_selectarrayex.h"

namespace cineware
{
#pragma pack (push, 8)

#define CA_POSEMORPH_LEVEL 6

#define ID_POSEMORPH_CHUNK			 1024240
#define ID_POSEMORPH_CHUNK_LEVEL 5

#define ID_POSEMORPH_NODESEXTRADATA_CHUNK				1024241
#define ID_POSEMORPH_NODESEXTRADATA_CHUNK_LEVEL	0

#define ID_POSEMORPH_BASEEXTRADATA_CHUNK				1024242
#define ID_POSEMORPH_BASEEXTRADATA_CHUNK_LEVEL	0

#define POSEMORPH_POINT_DEFAULTTOLERANCE	 (2e-4)

/// @addtogroup CAMORPH_DATA_FLAGS
/// @ingroup group_enumeration
/// @{
enum CAMORPH_DATA_FLAGS
{
	CAMORPH_DATA_FLAGS_P					= (1 << 0),			///< Position morphing.
	CAMORPH_DATA_FLAGS_S					= (1 << 1),			///< Scale morphing.
	CAMORPH_DATA_FLAGS_R					= (1 << 2),			///< HPB rotation morphing.
	CAMORPH_DATA_FLAGS_POINTS			= (1 << 3),			///< Points morphing.
	CAMORPH_DATA_FLAGS_TANGENTS		= (1 << 4),			///< Tangents morphing.
	CAMORPH_DATA_FLAGS_VERTEXMAPS	= (1 << 5),			///< Vertex map morphing.
	CAMORPH_DATA_FLAGS_WEIGHTMAPS	= (1 << 6),			///< Joint weights morphing.
	CAMORPH_DATA_FLAGS_PARAMS			= (1 << 7),			///< Parameter morphing.
	CAMORPH_DATA_FLAGS_USERDATA		= (1 << 8),			///< %User data morphing.
	CAMORPH_DATA_FLAGS_UV					= (1 << 9),			///< UV coordinate morphing.

	/// This flag has usually to be set to store the current state of an object into the morph.\n
	/// It is storing the morph data as set by the user in the morph tag, otherwise it would mess up the internal data.
	CAMORPH_DATA_FLAGS_ASTAG			= (1 << 15),

	/// All data morphing.
	CAMORPH_DATA_FLAGS_ALL = CAMORPH_DATA_FLAGS_P | CAMORPH_DATA_FLAGS_S | CAMORPH_DATA_FLAGS_R | CAMORPH_DATA_FLAGS_POINTS | CAMORPH_DATA_FLAGS_TANGENTS | CAMORPH_DATA_FLAGS_VERTEXMAPS | CAMORPH_DATA_FLAGS_WEIGHTMAPS | CAMORPH_DATA_FLAGS_PARAMS | CAMORPH_DATA_FLAGS_USERDATA | CAMORPH_DATA_FLAGS_UV,

	CAMORPH_DATA_FLAGS_NONE				= 0							///< None.
} ENUM_END_FLAGS(CAMORPH_DATA_FLAGS);
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPH_COPY_FLAGS
{
	POSEMORPH_COPY_FLAGS_NONE				= 0,
	POSEMORPH_COPY_FLAGS_NONODES		= (1 << 0),
	POSEMORPH_COPY_FLAGS_NODES_ONLY	= (1 << 1),
	POSEMORPH_COPY_FLAGS_NO_EXDATA	=	(1 << 2),
	POSEMORPH_COPY_FLAGS_NO_FLAGS		=	(1 << 3),
	POSEMORPH_COPY_FLAGS_NO_INIT		=	(1 << 4),
	POSEMORPH_COPY_FLAGS_NO_INDEX		= (1 << 5)
} ENUM_END_FLAGS(POSEMORPH_COPY_FLAGS);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPHDATA_FLAG
{
	POSEMORPHDATA_FLAG_NONE = 0,
	POSEMORPHDATA_FLAG_RELATIVE						= (1 << 0),
	POSEMORPHDATA_FLAG_ROTATIONAL					= (1 << 1),
	POSEMORPHDATA_FLAG_CORRECTIONAL				= (1 << 2),
	POSEMORPHDATA_FLAG_UPDATED						= (1 << 3),
	POSEMORPHDATA_FLAG_DIRTY							= (1 << 4),
	POSEMORPHDATA_FLAG_RESTRICT						= (1 << 5),
	POSEMORPHDATA_FLAG_CORRECTIONAL_AREA	= (1 << 6),
	POSEMORPHDATA_FLAG_RESIZE_UPDATE			= (1 << 7),
	POSEMORPHDATA_FLAG_INVALID_EDIT				= (1 << 8),
	POSEMORPHDATA_FLAG_CORRECTIONAL_PSD		= (1 << 9),
	POSEMORPHDATA_FLAG_MODES							= (POSEMORPHDATA_FLAG_RELATIVE | POSEMORPHDATA_FLAG_ROTATIONAL | POSEMORPHDATA_FLAG_CORRECTIONAL)
} ENUM_END_FLAGS(POSEMORPHDATA_FLAG);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPHTAG_FLAGS
{
	POSEMORPHTAG_FLAGS_NONE								= 0,
	POSEMORPHTAG_FLAGS_COPY								= (1 << 0),
	POSEMORPHTAG_FLAGS_DEST_SET						= (1 << 1),
	POSEMORPHTAG_FLAGS_EDIT_DEFORMED			= (1 << 2),
	POSEMORPHTAG_FLAGS_EDITING						= (1 << 3),
	POSEMORPHTAG_FLAGS_EDIT_INPLACE				= (1 << 4),
	POSEMORPHTAG_FLAGS_AS_DEFORMER				= (1 << 5),
	POSEMORPHTAG_FLAGS_EDIT_TARGET				= (1 << 6),
	POSEMORPHTAG_FLAGS_DISABLE						= (1 << 7),
	POSEMORPHTAG_FLAGS_EXITEDIT_PENDING		= (1 << 8),
	POSEMORPHTAG_FLAGS_INITEDIT_PENDING		= (1 << 9),
	POSEMORPHTAG_FLAGS_INIT_DEFORMED			= (1 << 10),
	POSEMORPHTAG_FLAGS_REBUILD						= (1 << 11),
	POSEMORPHTAG_FLAGS_INITIALISED				= (1 << 12),
	POSEMORPHTAG_FLAGS_EMULATE_POSEMIXER	= (1 << 13),
	POSEMORPHTAG_FLAGS_EMULATE_MORPH			= (1 << 14),
	POSEMORPHTAG_FLAGS_DISABLE_BASEPOSE		= (1 << 15),
	POSEMORPHTAG_FLAGS_BASEPOSE_ONLY			= (1 << 16),
	POSEMORPHTAG_FLAGS_EDIT_TMP						= (POSEMORPHTAG_FLAGS_EDITING | POSEMORPHTAG_FLAGS_EDIT_DEFORMED | POSEMORPHTAG_FLAGS_EXITEDIT_PENDING | POSEMORPHTAG_FLAGS_INITEDIT_PENDING | POSEMORPHTAG_FLAGS_INIT_DEFORMED),
	POSEMORPHTAG_FLAGS_EDIT								= (POSEMORPHTAG_FLAGS_EDIT_INPLACE | POSEMORPHTAG_FLAGS_EDIT_TARGET)
} ENUM_END_FLAGS(POSEMORPHTAG_FLAGS);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPH_PSR_FLAG
{
	POSEMORPH_PSR_FLAG_NONE				= 0,
	POSEMORPH_PSR_FLAG_RELATIVE		= (1 << 0),
	POSEMORPH_PSR_FLAG_FLUSH			= (1 << 1),
	POSEMORPH_PSR_FLAG_RELATIVE_P	= (1 << 2),
	POSEMORPH_PSR_FLAG_RELATIVE_S	= (1 << 3),
	POSEMORPH_PSR_FLAG_RELATIVE_R	= (1 << 4),
	POSEMORPH_PSR_FLAG_P					= (1 << 5),
	POSEMORPH_PSR_FLAG_S					= (1 << 6),
	POSEMORPH_PSR_FLAG_R					= (1 << 7),
	POSEMORPH_PSR_FLAG_PSR				= (POSEMORPH_PSR_FLAG_P | POSEMORPH_PSR_FLAG_S | POSEMORPH_PSR_FLAG_R),
} ENUM_END_FLAGS(POSEMORPH_PSR_FLAG);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum CAMORPH_MODE_FLAGS
{
	CAMORPH_MODE_FLAGS_COLLAPSE	= 2048,			///< Collapse data. Needs to be passed to collapse the expanded data, for instance after data access.
	CAMORPH_MODE_FLAGS_EXPAND		= 4096,			///< Expand data. Needs to be passed before accessing any data.
	CAMORPH_MODE_FLAGS_ALL			= 1007,			///< Expand or collapse all data
	CAMORPH_MODE_FLAGS_NONE			= 0					///< None.
} ENUM_END_FLAGS(CAMORPH_MODE_FLAGS);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPH_FLAG
{
	POSEMORPH_FLAG_NONE							= 0,
	POSEMORPH_FLAG_ENABLED					= (1 << 0),
	POSEMORPH_FLAG_LOCKED						= (1 << 1),
	POSEMORPH_FLAG_BASE							= (1 << 2),
	POSEMORPH_FLAG_POSTDEFORM				= (1 << 3),
	POSEMORPH_FLAG_TARGET						= (1 << 4),
	POSEMORPH_FLAG_EXTERNAL					= (1 << 5),
	POSEMORPH_FLAG_HIERARCHY				= (1 << 6),
	POSEMORPH_FLAG_EXTERNAL_CACHED	= (1 << 7),
	POSEMORPH_FLAG_EXTERNAL_DIRTY		= (1 << 8),
	POSEMORPH_FLAG_NOTIFY						= (1 << 9),	// can only be used when POSEMORPH_FLAG_HIERARCHY is set!
} ENUM_END_FLAGS(POSEMORPH_FLAG);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPH_SET_FLAGS
{
	POSEMORPH_SET_FLAGS_NONE			 = 0,
	POSEMORPH_SET_FLAGS_PARAMS		 = (1 << 0),
	POSEMORPH_SET_FLAGS_P					 = (1 << 1),
	POSEMORPH_SET_FLAGS_S					 = (1 << 2),
	POSEMORPH_SET_FLAGS_R					 = (1 << 3),
	POSEMORPH_SET_FLAGS_UPDATE		 = (1 << 4),
	POSEMORPH_SET_FLAGS_USERDATA	 = (1 << 5),
	POSEMORPH_SET_FLAGS_POINTS		 = (1 << 6),
	POSEMORPH_SET_FLAGS_UV				 = (1 << 7),
	POSEMORPH_SET_FLAGS_VMAP			 = (1 << 8),
	POSEMORPH_SET_FLAGS_WMAP			 = (1 << 9),
	POSEMORPH_SET_FLAGS_TOCACHE		 = (1 << 10),
	POSEMORPH_SET_FLAGS_COLLAPSE	 = (1 << 11),
	POSEMORPH_SET_FLAGS_EXPAND		 = (1 << 12),
	POSEMORPH_SET_FLAGS_DIRTY_ONLY = (1 << 13),
	POSEMORPH_SET_FLAGS_HIERARCHY	 = (1 << 14),
	POSEMORPH_SET_FLAGS_UNDEFINED	 = (1 << 15),
	POSEMORPH_SET_FLAGS_FLUSH			 = (1 << 16),
	POSEMORPH_SET_FLAGS_NOEXTDIRTY = (1 << 17),
	POSEMORPH_SET_FLAGS_NOWEIGHT	 = (1 << 18),
	POSEMORPH_SET_FLAGS_RESTRICT	 = (1 << 19),
	POSEMORPH_SET_FLAGS_FROMCACHE	 = (1 << 20),
	POSEMORPH_SET_FLAGS_TMPBASE		 = (1 << 21),
	POSEMORPH_SET_FLAGS_AXIS			 = (1 << 22),
	POSEMORPH_SET_FLAGS_TARGET		 = (1 << 23),
	POSEMORPH_SET_FLAGS_PSR = (POSEMORPH_SET_FLAGS_P | POSEMORPH_SET_FLAGS_S | POSEMORPH_SET_FLAGS_R),
	POSEMORPH_SET_FLAGS_FULL = (POSEMORPH_SET_FLAGS_P | POSEMORPH_SET_FLAGS_S | POSEMORPH_SET_FLAGS_R | POSEMORPH_SET_FLAGS_PARAMS | POSEMORPH_SET_FLAGS_USERDATA | POSEMORPH_SET_FLAGS_POINTS | POSEMORPH_SET_FLAGS_UV | POSEMORPH_SET_FLAGS_VMAP | POSEMORPH_SET_FLAGS_WMAP)
} ENUM_END_FLAGS(POSEMORPH_SET_FLAGS);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPH_FLIP_FLAG
{
	POSEMORPH_FLIP_FLAG_NONE			= 0,
	POSEMORPH_FLIP_FLAG_KEEPDATA	= (1 << 0),
} ENUM_END_FLAGS(POSEMORPH_FLIP_FLAG);

class CAMorph;
class PoseMorphTagData;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphPSR
{
public:
	PoseMorphPSR();
	~PoseMorphPSR();

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);
	Bool CopyTo(PoseMorphPSR* dst);

	void Set(BaseObject* op, POSEMORPH_PSR_FLAG flags);
	void Transform(POSEMORPH_PSR_FLAG flags, const PoseMorphPSR* base);
	Bool IsEmpty() { return (m_Flags & POSEMORPH_PSR_FLAG::POSEMORPH_PSR_FLAG_PSR) == POSEMORPH_PSR_FLAG::POSEMORPH_PSR_FLAG_NONE; }

	// Data Section
public:
	Vector m_Pos;
	Vector m_Scale;
	Vector m_Rot;
	POSEMORPH_PSR_FLAG m_Flags;
	Matrix m_tmpM;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPHVTX_FLAG
{
	POSEMORPHVTX_FLAG_NONE = 0,

	POSEMORPHVTX_FLAG_DELTA						= (1 << 0), // Delta Mode, if not set we're in absolute mode
	POSEMORPHVTX_FLAG_LINEAR					= (1 << 1), // Interpolation Type
	POSEMORPHVTX_FLAG_ROTATIONAL			= (1 << 2), // Interpolation Type
	POSEMORPHVTX_FLAG_TANGENT					= (1 << 3), // Interpolation Type
	POSEMORPHVTX_FLAG_TANGENT_AREA		= (1 << 4), // Interpolation Type
	POSEMORPHVTX_FLAG_PSD							= (1 << 5), // Interpolation Type
	POSEMORPHVTX_FLAG_TYPES = (POSEMORPHVTX_FLAG_LINEAR | POSEMORPHVTX_FLAG_ROTATIONAL | POSEMORPHVTX_FLAG_TANGENT | POSEMORPHVTX_FLAG_TANGENT_AREA | POSEMORPHVTX_FLAG_PSD), // This is used to know rapidly the interpolation type (Delta is not an interpolation Type)
} ENUM_END_FLAGS(POSEMORPHVTX_FLAG);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
enum POSEMORPHVTX_TYPE
{
	POSEMORPHVTX_TYPE_NONE			= 0,
	POSEMORPHVTX_TYPE_POINTS		= 1,
	POSEMORPHVTX_TYPE_TANGENTS	= 2,
} ENUM_END_FLAGS(POSEMORPHVTX_TYPE);

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class CASelectBitArray
{
public:
	CASelectBitArray();
	~CASelectBitArray();

	Bool Select(Int32 i);
 	Bool SelectAll(Int32 max);
 	Bool SelectBlock(UInt32 min, UInt32 max);
	void DeselectAll();
	Bool IsSelected(Int32 i) const;

	Bool Read(HyperFile* hf);
	Bool Write(HyperFile* hf);
	Bool CopyTo(CASelectBitArray* dst) const;

private:
	Bool SetElements(UInt32 e);

	UInt32	m_gCount;
	UInt32	m_sCount;
	UInt32	m_TotalElements;
	UInt32* m_pBits;
	UInt32	m_TopElement;
	Int32		m_Top;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphDeltaIndex
{
protected:
	class IndexData
	{
	public:
		IndexData()
		{
			m_Lower = 0;
			m_Upper = 0;
			m_Index = NOTOK;
		}

		IndexData(Int32 index) : m_Lower(index), m_Upper(index) {}
		IndexData(Int32 lower, Int32 upper) : m_Lower(lower), m_Upper(upper) {}

		Bool operator <(const IndexData& dst) const { return m_Lower < dst.m_Lower; }
		Bool operator >(const IndexData& dst) const { return m_Lower > dst.m_Upper; }
		Bool operator <=(const IndexData& dst) const { return m_Lower <= dst.m_Upper; }
		Bool operator >=(const IndexData& dst) const { return m_Lower >= dst.m_Lower; }
		Bool operator ==(const IndexData& dst) const { return m_Lower >= dst.m_Lower && m_Lower <= dst.m_Upper; }

		IndexData& operator =(const IndexData& src) { m_Lower = src.m_Lower; m_Upper = src.m_Upper; m_Index = src.m_Index; return *this; }

		static Bool Write(HyperFile* hf, IndexData* data, Int32 cnt);
		static Bool Read(HyperFile* hf, IndexData* data, Int32 cnt);

		// Data Section
	public:
		Int32 m_Lower;
		Int32 m_Upper;
		Int32 m_Index;
	};

public:
	PoseMorphDeltaIndex() {}
	virtual ~PoseMorphDeltaIndex() { Free(); }

	virtual void Free();

	virtual Bool Write(HyperFile* hf);
	virtual Bool Read(HyperFile* hf);
	virtual Bool CopyTo(PoseMorphDeltaIndex* dst, AliasTrans* trn);

	void BuildIndexing();

	// Data Section
public:
	CASelectBitArray					m_Select;	// point indexes stored
	SelectArrayExt<IndexData> m_Index;	// indexing table
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphAxisData
{
public:
	Vector m_X, m_Y;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphAxis
{
public:
	PoseMorphAxis();
	~PoseMorphAxis();

	void Free();

	Bool Set(PolygonObject* plyop, CASelectBitArray* select = nullptr, Bool area = true, const Vector* padr = nullptr, Int32 pcnt = NOTOK, const CPolygon* vadr = nullptr, Int32 vcnt = NOTOK);
	Bool Init(Int32 pcnt, Bool area = false);

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);
	Bool CopyTo(PoseMorphAxis* dst);

	// Data Section
public:
	PoseMorphAxisData* m_pData;
	Float32*					 m_pArea;
	Int32							 m_Count;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphVertices : public PoseMorphDeltaIndex
{
public:
	PoseMorphVertices(POSEMORPHVTX_TYPE type) : m_Type(type)
	{
		m_Flags = POSEMORPHVTX_FLAG::POSEMORPHVTX_FLAG_NONE;
		m_pPoints = nullptr;
		m_Count = 0;
		m_Type = POSEMORPHVTX_TYPE::POSEMORPHVTX_TYPE_NONE;
	}
	virtual ~PoseMorphVertices() { Free(); }

	void Free();

	// Get/Set Point individually
	Vector GetPoint(Int32 index);

	// Set Referenced Object and allocate array of points
	Bool Set(BaseObject* op);
	// Copy array of points as point pose
	Bool SetPoints(const Vector* padr, Int32 pcnt);

	// Convert the pose in POSEMORPHVTX_FLAG_DELTA mode (delta according to given base)
	Bool Collapse(PoseMorphVertices* base, Float dlt = POSEMORPH_POINT_DEFAULTTOLERANCE);
	// Convert the pose in absolute mode (add delta to given base pose) (remove POSEMORPHVTX_FLAG_DELTA)
	Bool Expand(PoseMorphVertices* base);

	// Copy From/To Array according to mode (collapsed or expanded)
	Bool ToArray(Vector* pnts, Int32 cnt, Bool clearmem = false);

	virtual Bool Write(HyperFile* hf);
	virtual Bool Read(HyperFile* hf);
	virtual Bool CopyTo(PoseMorphDeltaIndex* dst, AliasTrans* trn);

	// Transform Data into another mode (given by flags). Internal data must be expanded first (see Expand(...))
	void Transform(PoseMorphVertices* base, PoseMorphAxis* axis, POSEMORPHVTX_FLAG flags, const Matrix& tm, Float weight = 1., BaseList2D *dst = nullptr);

	// Data Section
public:
	POSEMORPHVTX_FLAG	m_Flags;
	Vector* m_pPoints;
	Int32		m_Count;
	POSEMORPHVTX_TYPE		m_Type;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphBaseHierarchyData
{
public:
	PoseMorphBaseHierarchyData();
	virtual ~PoseMorphBaseHierarchyData();

	void Free();
	Bool Init();

	Bool Set(BaseList2D* bl);
	Bool CopyTo(PoseMorphBaseHierarchyData* dst, AliasTrans* trn);
	Int	GetMemoryUsed();

	Bool Write(HyperFile* hf);
	Bool Read(HyperFile* hf);

	// Data Section
public:
	BaseLink* m_pLink ;
	GeMarker* m_pMarker;

	Int32			m_Type;
	UInt64		m_ObjID;
	UInt64		m_GUID;
	UInt32		m_Dirty;

	String		m_Name;
	UInt32		m_NameCRC;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphBaseData
{
public:
	PoseMorphBaseData();
	~PoseMorphBaseData();

 	void Free();
 	Bool Init();

 	Bool Set(BaseList2D* bl, POSEMORPH_SET_FLAGS flags);
 	Bool BuildPoints(BaseObject* op);
 	Bool BuildAxis(PolygonObject* plyop);
 	Bool BuildHierarchyData(BaseList2D* bl);
//
 	Bool Write(HyperFile* hf);
 	Bool Read(HyperFile* hf);
 	Bool CopyTo(PoseMorphBaseData* dst, AliasTrans* trn, POSEMORPH_COPY_FLAGS flags) const;

	// Data Section
public:
	PoseMorphBaseHierarchyData* m_pHData;	// POSEMORPH_FLAG_HIERARCHY only

	POSEMORPHDATA_FLAG						m_Flags;

	PoseMorphPSR*									m_pPSR;
	PoseMorphVertices*						m_pVert;
	PoseMorphVertices*						m_pTangents;
	PoseMorphAxis*								m_pAxis;

	// Data not persisted
	UInt64 m_TmpDeformerGUID;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphIndexing
{
	class PoseMorphIndexData
	{
	public:
		PoseMorphIndexData()
		{
			m_sIndex = NOTOK;
			m_dIndex = NOTOK;
		}
		PoseMorphIndexData(Int32 index) : m_sIndex(index) {}
		PoseMorphIndexData(Int32 sindex, Int32 dindex) : m_sIndex(sindex), m_dIndex(dindex) {}

		Bool operator <(const PoseMorphIndexData& dst) const { return m_sIndex < dst.m_sIndex; }
		Bool operator >(const PoseMorphIndexData& dst) const { return m_sIndex > dst.m_sIndex; }
		Bool operator <=(const PoseMorphIndexData& dst) const { return m_sIndex <= dst.m_sIndex; }
		Bool operator >=(const PoseMorphIndexData& dst) const { return m_sIndex >= dst.m_sIndex; }
		Bool operator ==(const PoseMorphIndexData& dst) const { return m_sIndex == dst.m_sIndex; }

		// Data Section
	public:
		Int32 m_sIndex;
		Int32 m_dIndex;
	};

public:
	PoseMorphIndexing();
	virtual ~PoseMorphIndexing();

 	Int32 Alloc();
 	void Remove(Int32& index);
 	void Free();
 	Bool Read(HyperFile* hf);
 	Bool Write(HyperFile* hf);
 	Bool CopyTo(PoseMorphIndexing* dst);

private:

	// Data Section
private:
	Int32								m_TopID;
	SelectBlockArray		m_aID;
	PoseMorphIndexData* m_pLUT;
	Int32								m_Count;
	Int32								m_Dirty;
	Int32								m_lDirty;
};

//----------------------------------------------------------------------------------------
/// CAMorphNode class allows to access the actual morph data. For every morph there exist a morph node tree representing the hierarchy of the morphed objects.\n
/// Before using any of the access function always check which data exist with GetInfo().\n
/// Example of a simple case with one polygon object. In this case just access the first morph node of the morph to get the data from.\n
/// Get the morph to get the data from:
/// @code
/// CAMorph* morph = mtag->GetMorph(1);
/// @endcode
/// Now get the morph node. The first will be the object with the morph tag:
/// @code
/// CAMorphNode* mnode = morph->GetFirst();
/// @endcode
/// The point data can now be accessed, but make sure the data exists:
/// @code
/// mnode->GetInfo() & CAMORPH_DATA_FLAGS::POINTS
/// @endcode
/// To access the data use GetPointCount() and GetPoint().\n
//----------------------------------------------------------------------------------------
class CAMorphNode
{
public:

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the next morph node.
	/// @return												The next morph node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{morph node}
	//----------------------------------------------------------------------------------------
	CAMorphNode *GetNext();

	//----------------------------------------------------------------------------------------
	/// Retrieves the previous morph node.
	/// @return												The previous morph node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{morph node}
	//----------------------------------------------------------------------------------------
	CAMorphNode *GetPrev();

	//----------------------------------------------------------------------------------------
	/// Retrieves the parent morph node.
	/// @return												The parent morph node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{morph node}
	//----------------------------------------------------------------------------------------
	CAMorphNode *GetUp();

	//----------------------------------------------------------------------------------------
	/// Retrieves the first child morph node.
	/// @return												The first child morph node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{morph node}
	//----------------------------------------------------------------------------------------
	CAMorphNode *GetDown();

	/// @}

	/// @name Get Link
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the object linked to the morph node.
	/// @param[in] tag								The morph tag containing the morph data.
	/// @param[in] morph							The morph containing the morph node.
	/// @param[in] doc								The document containing the linked object.
	/// @return												The linked object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseList2D *GetLink(PoseMorphTagData *tag, CAMorph *morph, BaseDocument *doc);

	/// @}

	/// @name Get Info
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets what morph data is stored in the morph node.
	/// @return												The data flags: @enumerateEnum{CAMORPH_DATA_FLAGS}
	//----------------------------------------------------------------------------------------
	CAMORPH_DATA_FLAGS GetInfo();

	/// @}

	/// @name Get/Set Positon/Scale/Rotation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the position of the morph node.
	/// @return												The position.
	//----------------------------------------------------------------------------------------
	Vector GetP();

	//----------------------------------------------------------------------------------------
	/// Retrieves the scale of the morph node.
	/// @return												The scale.
	//----------------------------------------------------------------------------------------
	Vector GetS();

	//----------------------------------------------------------------------------------------
	/// Retrieves the HPB rotation of the morph node.
	/// @return												The HPB rotation.
	//----------------------------------------------------------------------------------------
	Vector GetR();

	/// @name Point
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the point count of the morph node.
	/// @return												The point count.
	//----------------------------------------------------------------------------------------
	Int32 GetPointCount();

	//----------------------------------------------------------------------------------------
	/// Retrieves the point at @formatParam{index}.
	/// @param[in] index							The point index: @em 0 <= @formatParam{index} < GetPointCount()
	/// @return												The point at @formatParam{index}.
	//----------------------------------------------------------------------------------------
	Vector GetPoint(Int32 index);

	/// @}
};

//----------------------------------------------------------------------------------------
/// CAMorph class contains the data for each morph. It has to be retrieved from the CAPoseMorphTag.
/// @see CAMorphNode for an example.
//----------------------------------------------------------------------------------------
class CAMorph
{
public:
	/// @name Name
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the name of the morph.
	/// @return												The name of the morph.
	//----------------------------------------------------------------------------------------
	String GetName();

	//----------------------------------------------------------------------------------------
	/// Sets the name of the morph.
	/// @param[in] name								The new name of the morph.
	//----------------------------------------------------------------------------------------
	void SetName(const String& name);

	/// @}

	/// @name Morph Node
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the first node of the morph.
	/// @return												The first morph node. @theOwnsPointed{tag,morph node}
	//----------------------------------------------------------------------------------------
	CAMorphNode* GetFirst();

	/// @}

	/// @name Target
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the target of the morph.
	/// @param[in] doc								The document containing the morph tag.
	/// @return												The target of the morph. @theOwnsPointed{tag,object}
	//----------------------------------------------------------------------------------------
	BaseList2D* GetTarget(BaseDocument *doc);

	/// @}

	/// @name Strength
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the strength of the morph.
	/// @param[in] strength						The new strength of the morph.
	//----------------------------------------------------------------------------------------
	void SetStrength(Float strength);

	//----------------------------------------------------------------------------------------
	/// Retrieves the strength of the morph.
	/// @return												The strength of the morph.
	//----------------------------------------------------------------------------------------
	Float GetStrength();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Retrieves if the morph is applied at PostDeform.
	/// @return												Morph is post-deform mode.
	//----------------------------------------------------------------------------------------
	Bool IsPostDeform();
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PoseMorphData
{
public:
	PoseMorphData();
	virtual ~PoseMorphData();
 	Bool Init(PoseMorphIndexing* indexing = nullptr);
 	void Free(PoseMorphIndexing* indexing = nullptr);

 	Bool IsEmpty() { return m_Pose.IsEmpty(); }
 	Bool StorePose(GeTreeNode<PoseMorphBaseData>* parent, BaseList2D* bl, POSEMORPH_SET_FLAGS flags, BaseTag* posetag);
 	void RelativePose(GeTreeNode<PoseMorphBaseData>* basenode, GeTreeNode<PoseMorphBaseData>* node, Bool downonly, POSEMORPH_SET_FLAGS flags, Int32 mode, BaseDocument* doc, BaseList2D* dst = nullptr);
 	GeTreeNode<PoseMorphBaseData>* GetHead() const { return m_Pose.GetCRoot()->GetDown(); }
 	Bool Write(HyperFile* hf);
 	Bool Read(HyperFile* hf);
 	Bool CopyTo(PoseMorphData* dst, AliasTrans* trn, POSEMORPH_COPY_FLAGS poseMorphFlags) const;
 	Bool WriteNodes(HyperFile* hf, GeTreeNode<PoseMorphBaseData>* node, Bool first = true);
 	Bool ReadNodes(HyperFile* hf, GeTreeNode<PoseMorphBaseData>* pnode, Bool first = true);
 	Bool CopyNodes(GeTreeNode<PoseMorphBaseData>* src, GeTree<PoseMorphBaseData>& dsttree, GeTreeNode<PoseMorphBaseData>* pnode, AliasTrans* trans, POSEMORPH_COPY_FLAGS flags) const;
 	BaseList2D* GetTarget(BaseDocument* doc, Bool allowcache);
 	BaseLink* GetTargetLink() { return m_pTargetLink; }
	static Int32 FindNodeIndex(GeTreeNode<PoseMorphBaseData>* fnode, Int32& nindex, GeTreeNode<PoseMorphBaseData>* node, Bool downonly);
	static GeTreeNode<PoseMorphBaseData>* FindNode(Int32 findex, Int32& nindex, GeTreeNode<PoseMorphBaseData>* node, Bool downonly);
protected:
 	Bool StorePoseNodes(GeTreeNode<PoseMorphBaseData>* parent, BaseObject* op, BaseObject* parentop, Bool downonly, /*PoseMorphStopData* spdata,*/ POSEMORPH_SET_FLAGS flags, BaseTag* posetag);
 	void RelativeToAbs(GeTreeNode<PoseMorphBaseData>* basenode, GeTreeNode<PoseMorphBaseData>* node, POSEMORPH_SET_FLAGS flags, BaseList2D* dst = nullptr);

	// Data Section
public:
	GeTree<PoseMorphBaseData> m_Pose;
	POSEMORPH_FLAG						m_Flags;
	Int32											m_Count;
	UInt64										m_Dirty, m_HDirty;
	Int32											m_Index;
	UInt64										m_GUID;
	String										m_Name;
	Float											m_Weight;
	Int32											m_Mode;
	Vector										m_RotAxis;
	Matrix										m_RotMatrix;
	Filename									m_ExtTarget;
	Int32											m_cLevel;

private:
	BaseLink* m_pTargetLink;
};

//----------------------------------------------------------------------------------------
/// Pose morph tag storing the morph data.
/// @addAllocFreeAutoAllocNote
/// @see CAMorphNode for an example.
//----------------------------------------------------------------------------------------
class PoseMorphTagData : public NodeData
{
	friend class CAMorphNode;

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{PoseMorphTagData data}
	/// @return												@allocReturn{PoseMorphTagData data}
	//----------------------------------------------------------------------------------------
	static PoseMorphTagData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{PoseMorphTagData data}
	/// @param[in,out] pData					@theToDestruct{PoseMorphTagData data}
	//----------------------------------------------------------------------------------------
	static void Free(PoseMorphTagData*& pData);

	/// @}

public:

	/// @name Morph
	/// @{

	//----------------------------------------------------------------------------------------
	/// Retrieves the number of morphs. This is the number of morphs visible interface.
	/// @return												The number of available morphs.
	//----------------------------------------------------------------------------------------
	Int32 GetMorphCount();

	//----------------------------------------------------------------------------------------
	/// Retrieves the morph at @formatParam{index}.
	/// @param[in] index							The morph index: @em 0 <= @formatParam{index} < GetMorphCount()
	/// @return												The morph. @theOwnsPointed{tag,morph}
	//----------------------------------------------------------------------------------------
	CAMorph* GetMorph(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Retrieves the index of the active morph (usually the one selected in the list).
	/// @return												The index of the active morph.
	//----------------------------------------------------------------------------------------
	Int32 GetActiveMorphIndex();

	//----------------------------------------------------------------------------------------
	/// Retrieves the active morph (usually the one selected).
	/// @return												The active morph. @theOwnsPointed{tag,morph}
	//----------------------------------------------------------------------------------------
	CAMorph *GetActiveMorph() { return GetMorph(GetActiveMorphIndex()); }

	//----------------------------------------------------------------------------------------
	/// Retrieves the base morph. Same as calling GetMorph(0).
	/// @return												The base morph. @theOwnsPointed{tag,morph}
	//----------------------------------------------------------------------------------------
	CAMorph *GetMorphBase() { return GetMorph(0); }

	//----------------------------------------------------------------------------------------
	/// Retrieves the index for the given @formatParam{morph}.
	/// @param[in] morph							The morph.
	/// @return												The index for the @formatParam{morph}.
	//----------------------------------------------------------------------------------------
	Int32 GetMorphIndex(CAMorph *morph);

	/// @}

private:
	PoseMorphTagData();
	~PoseMorphTagData();

	virtual Bool Init();
	void Free();
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

protected:
	BaseTime											 	m_lTime;
	Int32													 	m_iLevel;
	POSEMORPHTAG_FLAGS						 	m_Flags;
	Int32													 	m_ActivePose;
	DynamicArray<PoseMorphData, 8>	m_Poses;
	PoseMorphData				m_LastPose;
	PoseMorphData				m_TmpBasePose;
	PoseMorphIndexing		m_IndexTable;

	UInt64 m_DestGUID;
	BaseLink*	m_pDestLink;
	UInt32	m_LastDirty;
	UInt32	m_Dirty;
	UInt32	m_baseLastDirty;

	UInt64	m_DeformerModCRC;
};

#pragma pack (pop)
}

#endif	// C4D_POSEMORPHTAGDATA_H__
