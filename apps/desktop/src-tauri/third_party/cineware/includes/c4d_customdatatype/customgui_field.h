/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_FIELD_H__
#define CUSTOMGUI_FIELD_H__

#include "c4d_basecontainer.h"
#include "c4d_customdatatype.h"
#include "c4d_pluginnode.h"
#include "c4d_fieldlayerrootnode.h"

namespace cineware
{
#pragma pack(push, 8)

/// Field custom GUI ID.
#define CUSTOMGUI_FIELDLIST				440000224
#define CUSTOMDATATYPE_FIELDLIST	440000246

/// FieldList custom GUI Description Flags
#define DESC_FIELDLIST_MULTISELECT	1019921			///< @markPrivate
#define DESC_FIELDLIST_HIERARCHICAL 1019922			///< @markPrivate
#define DESC_FIELDLIST_DRAGTYPE			1019923			///< Accepted drag types for the list (Int32) default value is DRAGTYPE_ATOMARRAY
#define DESC_FIELDLIST_NOCOLOR			440000316		///< Disable the Color channel for the list (Bool)
#define DESC_FIELDLIST_NODIRECTION	440000317		///< Disable the Direction channel for the list (Bool)
#define DESC_FIELDLIST_NOROTATION		440000336		///< Disable the Rotation channel for the list (Bool)
#define DESC_FIELDLIST_NOVALUE			440000318		///< Disable the Value channel for the list (Bool)
#define DESC_FIELDLIST_NOHEADER			440000329		///< Disable the list header (Bool)

enum FIELDLIST_FLAGS
{
	FIELDLIST_FLAGS_NONE											= 0,					///< No flags
	FIELDLIST_FLAGS_CLAMPOUTPUT								= (1 << 0),		///< Clamp the output values from the FieldList
	FIELDLIST_FLAGS_ENABLEDIRECTIONBYDEFAULT	= (1 << 4),		///< Have direction enabled by default on new layers
	FIELDLIST_FLAGS_ENABLECOLORBYDEFAULT			= (1 << 5),		///< Have color enabled by default on new layers
	FIELDLIST_FLAGS_DISABLEVALUEBYDEFAULT			= (1 << 6),		///< Disable the value channel by default on new layers
	FIELDLIST_FLAGS_SIMPLEGUI									= (1 << 7),		///< Use a simplified GUI, this is only possible if there is only a single standard fieldlayer in the datatype
	FIELDLIST_FLAGS_ENABLEROTATIONBYDEFAULT		= (1 << 8)		///< Have direction enabled by default on new layers
} ENUM_END_FLAGS(FIELDLIST_FLAGS);

/// @addtogroup FIELDLAYER_FLAG
/// @ingroup group_enumeration
/// @{
/// %FieldLayer status values for execution and display.
/// @see FieldLayer::GetLayerFlags
enum FIELDLAYER_FLAG
{
	FIELDLAYER_FLAG_NONE										= 0,						///< Default state
	FIELDLAYER_FLAG_SKIP										= (1 << 0),			///< Layer calculation is disabled, e.g. the linked object on a standard layer's deform mode is false.
	FIELDLAYER_FLAG_ERRORSKIP								= (1 << 1),			///< Layer calculation is disabled due to a non critical error in InitializeSampling or as user specified in GetFieldLayerFlags.
	FIELDLAYER_FLAG_HIDE										= (1 << 2),			///< Layer should be completely ignored from both calculation and GUI display, e.g. the linked object has been deleted.
	FIELDLAYER_FLAG_MODIFYDATA							= (1 << 3),			///< The layer modifies existing data and needs the values from layers beneath it in the layerlist in the "results" array inside of Sample.
	FIELDLAYER_FLAG_PRESERVEALPHA						= (1 << 4),			///< The layer doesn't adjust the existing alpha and it's color changes should be applied ignoring any alpha values.
	FIELDLAYER_FLAG_NOCHILDVALUE						= (1 << 5),			///< Children of this layer wont output their value.
	FIELDLAYER_FLAG_NOCHILDCOLOR						= (1 << 6),			///< Children of this layer wont output their color.
	FIELDLAYER_FLAG_NOCHILDDIRECTION				= (1 << 7),			///< Children of this layer wont output their direction.
	FIELDLAYER_FLAG_TEMPORARY								= (1 << 8),			///< Don't save this layer with the list or duplicate it (outside of undo)
	FIELDLAYER_FLAG_NOCHILDROTATION					= (1 << 9),			///< Children of this layer wont output their direction.
	FIELDLAYER_FLAG_SKIPWITHDEFAULTOUTPUTS	= (1 << 10),		///< The Field is sampling is skipped with no error, untouched FieldOutput will be blended with previous layer.
} ENUM_END_FLAGS(FIELDLAYER_FLAG);
/// @}

/// @addtogroup FIELDLAYER_CHANNELFLAG
/// @ingroup group_enumeration
/// @{
/// Channel flags for FieldLayer objects
enum FIELDLAYER_CHANNELFLAG
{
	FIELDLAYER_CHANNELFLAG_NONE					= 0,						///< None.
	FIELDLAYER_CHANNELFLAG_ENABLE				= (1 << 0),			///< The layer is enabled and being sampled.
	FIELDLAYER_CHANNELFLAG_VALUE				= (1 << 1),			///< Value sampling is enabled for this layer.
	FIELDLAYER_CHANNELFLAG_COLOR				= (1 << 2),			///< Color sampling is enabled for this layer.
	FIELDLAYER_CHANNELFLAG_DIRECTION		= (1 << 3),			///< Direction sampling is enabled for this layer.
	FIELDLAYER_CHANNELFLAG_ROTATION			= (1 << 4)			///< Rotation sampling is enabled for this layer.
} ENUM_END_FLAGS(FIELDLAYER_CHANNELFLAG);
/// @}

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class FieldLayer : public PluginNode
{
	INSTANCEOF(FieldLayer, PluginNode)

public:
	FieldLayer(Int32 id);

	//----------------------------------------------------------------------------------------
	/// @allocatesA{Field layer}
	/// @param[in] type								The layer type.
	/// @return												@allocReturn{Field layer}
	//----------------------------------------------------------------------------------------
	static FieldLayer* Alloc(Int32 type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{Field layer}
	/// @param[in,out] layer					@theToDestruct{Field layer}
	//----------------------------------------------------------------------------------------
	static void Free(FieldLayer*& layer);

	FIELDLAYER_FLAG GetLayerFlags() const;
	void SetLayerFlags(FIELDLAYER_FLAG flags, Bool state = true);
	virtual Int32 GetDiskType() const;

	Int32 GetUniqueID() const;
	void SetUniqueID(Int32 ID);

	//----------------------------------------------------------------------------------------
	/// Query if this layer is equal to another. Used by the undo system and to update dirtiness as required.
	/// @param[in] comp								The FieldLayer to compare to.
	/// @return												True if equal, false if not.
	//----------------------------------------------------------------------------------------
	Bool IsEqual(FieldLayer* comp) const;

	virtual Bool Write(HyperFile* hf);
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);
	FieldLayer* GetCloneFieldLayer(Bool rekurs, COPYFLAGS flags, AliasTrans* trans);

protected:
	// Persistent data
	FIELDLAYER_FLAG _layerFlags; // The layers general execution and display flags

private:
	FIELDLAYER_CHANNELFLAG _layerEnableFlags; // Value, color and direction

	Int32		 _layerBlendingMode;							// The standard layer blending mode
	Int32		 _uniqueID;												// The initial unique ID, unique to each FieldList
	Float		 _layerStrength;									// The strength of this layer in the field list
	BaseLink _attemptedDocumentLink;					// An attempt to find an acceptable document link
};

//----------------------------------------------------------------------------------------
/// Field custom GUI (@ref CUSTOMDATATYPE_FIELDLIST) for the Field list data.
/// @since R20
//----------------------------------------------------------------------------------------
class FieldList : public CustomDataType
{
	/// Default constructor.
	FieldList();

	/// Destructor.
	~FieldList();
};

class iFieldCustomDatatype : public iCustomDataType<iFieldCustomDatatype>
{
	friend class CustomDataTypeFieldClass;

public:
	//----------------------------------------------------------------------------------------
	/// Allocate iFieldCustomDatatype.
	/// If this function is used, no need to call Init.
	/// Use Free(iFieldCustomDatatype*&) for de-allocation.
	/// @return												allocated iFieldCustomDatatype
	//----------------------------------------------------------------------------------------
	static iFieldCustomDatatype* Alloc();
	static void									 Free(iFieldCustomDatatype*& pData);

	/// Destructor.
	~iFieldCustomDatatype();

protected:
	/// Default constructor.
	iFieldCustomDatatype();

	//----------------------------------------------------------------------------------------
	/// Allocate mandatory data.
	/// This is required before use if allocation is not done with  iFieldCustomDatatype::Alloc()
	/// @return												A maxon::Result&lt;void&gt;
	//----------------------------------------------------------------------------------------
	Bool Init();

	//----------------------------------------------------------------------------------------
	/// Copies this from a passed source instance of the iFieldCustomDatatype.
	/// @param[in] source							Another instance to copy from.
	/// @param[in] flags							The flags.
	/// @param[in] trans							If non-null, the AliasTrans to translate any document linkages.
	/// @return												A maxon::Result&lt;void&gt;
	//----------------------------------------------------------------------------------------
	Bool CopyFrom(const iFieldCustomDatatype* source, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// Reads the field customdatatype from the passed hyperfile.
	/// @param[in] hf									The hyperfile to read from.
	/// @param[in] level							The file level.
	/// @return												A maxon::Result&lt;void&gt;
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the field customdatatype from the passed hyperfile.
	/// @param[in] hf									The hyperfile to write to.
	/// @return												A maxon::Result&lt;void&gt;
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf) const;
public:
	// Stored data
	FieldlayerRootNode*	_fieldHead ;		/// The root holding the fields
	Int32								_idCounter;			/// The ID counter used to add new entries
	FIELDLIST_FLAGS			_fieldFlags;		/// Flags for setting up the field, stuff like clipping, disabling of channels in the UI etc
};

/// @markInternal
class CustomDataTypeFieldClass
{
public:
	static Int32						GetId();
	static CustomDataType*	AllocData();
	static void							FreeData(CustomDataType* data);
	static Bool							CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32						Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool							WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool							ReadData(CustomDataType* d, HyperFile* hf, Int32 level);
	static const Char*			GetResourceSym();
	static void							GetDefaultProperties(BaseContainer& data);
};

#pragma pack(pop)
}

#endif // CUSTOMGUI_FIELD_H__
