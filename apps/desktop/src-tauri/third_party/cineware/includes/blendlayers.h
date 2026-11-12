/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BLENDLAYERS_H__
#define C4D_BLENDLAYERS_H__

#include "c4d_baselink.h"
#include "c4d_system.h"
#include "c4d_tools.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup LayerType
/// @ingroup group_enumeration
/// @{
enum LayerType
{
	TypeUnknown						 = 0,	///< Unknown.
	TypeFolder						 = 1,	///< Folder.
	TypeShader						 = 2,	///< Shader.
	TypeBrightnessContrast = 3,	///< Brightness/contrast.
	TypeHSL								 = 4,	///< HSL.
	TypePosterize					 = 5,	///< Posterize.
	TypeColorize					 = 6,	///< Colorize.
	TypeClamp							 = 7,	///< Clamp.
	TypeClip							 = 8,	///< Clip.
	TypeDistorter					 = 9,	///< Distorter.
	TypeTransform					 = 10	///< Transform.
};
/// @}

/// @addtogroup LayerFlags
/// @ingroup group_enumeration
/// @{
enum LayerFlags
{
	LAYER_INVERT			 = 1,	///< Invert.
	LAYER_ALPHA				 = 2,	///< Alpha.
	LAYER_INVERT_ALPHA = 6,	///< Invert alpha.
	LAYER_IMAGE_ALPHA	 = 8	///< Image alpha.
};
/// @}

/// @addtogroup BlendMode
/// @ingroup group_enumeration
/// @{
enum BlendMode
{
	BlendNormal = 1,	///< Normal.
	BlendMultiply,		///< Multiply.
	BlendScreen,			///< Screen.
	BlendOverlay,			///< Overlay.
	BlendHardLight,		///< Hard light.
	BlendSoftLight,		///< Soft light.
	BlendDodge,				///< Dodge.
	BlendBurn,				///< Burn.
	BlendDarken,			///< Darken.
	BlendLighten,			///< Lighten.
	BlendAdd,					///< Add.
	BlendSubtract,		///< Subtract.
	BlendDifference,	///< Difference.
	BlendExclusion,		///< Exclusion.
	BlendHue,					///< Hue.
	BlendSat,					///< Saturation.
	BlendColor,				///< Color.
	BlendLuminance,		///< Luminance.
	BlendLevr,				///< Levr.
	BlendAlphaMask		///< Alpha mask.
};
/// @}

/// @addtogroup LAYER_S_PARAM
/// @ingroup group_enumeration
/// @{

/// @addtogroup LAYER_S_PARAM_ALL
/// @{
/// Common parameters IDs for all layers.
#define LAYER_S_PARAM_ALL_ACTIVE	 1000	///< Bool Active.
#define LAYER_S_PARAM_ALL_SELECTED 1001	///< Bool Selected.
#define LAYER_S_PARAM_ALL_FLAGS		 1002	///< LayerFlags Layer flags: LayerFlags
/// @}

/// @addtogroup LAYER_S_PARAM_FOLDER
/// @{
/// Parameters IDs for folder layers.
#define LAYER_S_PARAM_FOLDER_MODE				2000	///< BlendMode Blend mode: BlendMode
#define LAYER_S_PARAM_FOLDER_BLEND			2001	///< Float Blend parameter.
#define LAYER_S_PARAM_FOLDER_OPEN				2002	///< Int32 Folder is open.
#define LAYER_S_PARAM_FOLDER_FIRSTCHILD	2003	///< LayerShaderLayer* Pointer to the first LayerShaderLayer. Read-only.
/// @}

/// @addtogroup LAYER_S_PARAM_SHADER
/// @{
#define LAYER_S_PARAM_SHADER_MODE	 2000	///< BlendMode Blend mode: BlendMode
#define LAYER_S_PARAM_SHADER_BLEND 2001	///< Float Blend parameter.
#define LAYER_S_PARAM_SHADER_LINK	 2002	///< BaseLink Pointer to the shader. Read-only.
/// @}

/// @addtogroup LAYER_S_PARAM_BC
/// @{
#define LAYER_S_PARAM_BC_BRIGHTNESS	2000	///< Float Brightness.
#define LAYER_S_PARAM_BC_CONTRAST		2001	///< Float Contrast.
#define LAYER_S_PARAM_BC_GAMMA			2002	///< Float Gamma.
/// @}

/// @addtogroup LAYER_S_PARAM_HSL
/// @{
#define LAYER_S_PARAM_HSL_HUE				 2000	///< Float Hue.
#define LAYER_S_PARAM_HSL_SATURATION 2001	///< Float Saturation.
#define LAYER_S_PARAM_HSL_LIGHTNESS	 2002	///< Float Lightness.
#define LAYER_S_PARAM_HSL_COLORIZE	 2003	///< Int32 Colorize.
/// @}

/// @addtogroup LAYER_S_PARAM_POSTER
/// @{
#define LAYER_S_PARAM_POSTER_LEVELS 2000	///< Int32 Poster levels.
#define LAYER_S_PARAM_POSTER_WIDTH	2001	///< Float Poster width.
/// @}

/// @addtogroup LAYER_S_PARAM_COLORIZE
/// @{
#define LAYER_S_PARAM_COLORIZE_INPUT		2000	///< Int32 [0..6] Same order as the combo box in the dialog.
#define LAYER_S_PARAM_COLORIZE_GRADIENT	2001	///< Gradient Colorization gradient.
#define LAYER_S_PARAM_COLORIZE_OPEN			2002	///< Bool Full settings open.
#define LAYER_S_PARAM_COLORIZE_CYCLE		2003	///< Bool Cycle.
/// @}

/// @addtogroup LAYER_S_PARAM_CLAMP
/// @{
#define LAYER_S_PARAM_CLAMP_LOW_CLIP	2000	///< Float Clamp low clipping.
#define LAYER_S_PARAM_CLAMP_HIGH_CLIP	2001	///< Float Clamp high clipping.
/// @}

/// @addtogroup LAYER_S_PARAM_CLIP
/// @{
#define LAYER_S_PARAM_CLIP_LOW_CLIP	 2000	///< Float Clip low clipping.
#define LAYER_S_PARAM_CLIP_HIGH_CLIP 2001	///< Float Clip high clipping.
/// @}

/// @addtogroup LAYER_S_PARAM_TRANS
/// @{
#define LAYER_S_PARAM_TRANS_ANGLE	 2000	///< Float Angle.
#define LAYER_S_PARAM_TRANS_MIRROR 2001	///< Int32 Mirror.
#define LAYER_S_PARAM_TRANS_FLIP	 2002	///< Int32 Flip.
#define LAYER_S_PARAM_TRANS_SCALE	 2003	///< Vector Scale.
#define LAYER_S_PARAM_TRANS_MOVE	 2004	///< Vector Move.
/// @}

/// @addtogroup LAYER_S_PARAM_DISTORT
/// @{
#define LAYER_S_PARAM_DISTORT_NOISE				2000	///< NoiseType Noise type. (See dialog.)
#define LAYER_S_PARAM_DISTORT_STRENGTH		2001	///< Float Strength.
#define LAYER_S_PARAM_DISTORT_OCTACES			2002	///< Float Octaves.
#define LAYER_S_PARAM_DISTORT_TIME_SCALE	2003	///< Float Time scale.
#define LAYER_S_PARAM_DISTORT_NOISE_SCALE	2004	///< Float Noise scale.
#define LAYER_S_PARAM_DISTORT_3D_NOISE		2005	///< Int32 3D noise.
#define LAYER_S_PARAM_DISTORT_WRAP				2006	///< Int32 [0..3]. Wrapping. (See dialog.)
/// @}

/// @} // group LAYER_S_PARAM

/// @addtogroup NoiseType
/// @ingroup group_enumeration
/// @{
enum NoiseType
{
	NOISE_UNKNOWN	= 0,	///< Unknown.

	// 2D & 3D
	NOISE_BOX_NOISE			=  1,	///< Box noise.
	NOISE_BLIST_TURB		=  2,	///< Blistered turbulence.
	NOISE_BUYA					=  3,	///< Buya.
	NOISE_CELL_NOISE		=  4,	///< Cell noise.
	NOISE_CRANAL				=  5,	///< Cranal.
	NOISE_DENTS					=  6,	///< Dents.
	NOISE_DISPL_TURB		=  7,	///< Displaced turbulence.
	NOISE_FBM						=  8,	///< FBM.
	NOISE_HAMA					=  9,	///< Hama.
	NOISE_LUKA					= 10,	///< Luka.
	NOISE_MOD_NOISE			= 11,	///< Mod noise.
	NOISE_NAKI					= 12,	///< Naki.
	NOISE_NOISE					= 13,	///< Noise.
	NOISE_NUTOUS				= 14,	///< Nutous.
	NOISE_OBER					= 15,	///< Ober.
	NOISE_PEZO					= 16,	///< Pezo.
	NOISE_POXO					= 17,	///< Poxo.
	NOISE_RANDOM				= 18,	///< Random.
	NOISE_SEMA					= 19,	///< Sema.
	NOISE_STUPL					= 20,	///< Stupl.
	NOISE_TURBULENCE		= 21,	///< Turbulence.
	NOISE_VL_NOISE			= 22,	///< VL noise.
	NOISE_WAVY_TURB			= 23,	///< Wavy turbulence.
	NOISE_CELL_VORONOI	= 24,	///< Cell voronoi.
	NOISE_DISPL_VORONOI = 25,	///< Displaced voronoi.
	NOISE_SPARSE_CONV		= 26,	///< Sparse convolution.
	NOISE_VORONOI_1			= 27,	///< Voronoi 1.
	NOISE_VORONOI_2			= 28,	///< Voronoi 2.
	NOISE_VORONOI_3			= 29,	///< Voronoi 3.
	NOISE_ZADA					= 30,	///< Zada.
	NOISE_FIRE					= 31,	///< Fire.
	NOISE_ELECTRIC			= 32,	///< Electric.
	NOISE_GASEOUS				= 33,	///< Gaseous.

	// 3D only
	NOISE_3D_VORTEX_NOISE = 31,	///< 3D vertex noise.

	NOISE_NONE						= 99	///< None.
};
/// @}


/// Layer disk level.
#define LAYER_DISK_LEVEL 3

#define CUSTOMGUI_BLEND_LIST	1011130	///< ID for blend layer shader layer list custom GUI.
#define CUSTOMDATA_BLEND_LIST	1011131	///< ID for blend layer shader layer list data type.

#define BLEND_DATA_OFFSET		 10000		///< Blend data offset.
#define BLEND_DATA_STEP_SIZE 20				///< Blend data step size.

/// Layer flag for change points.
#define LAYER_FLAG_HAS_CHANGE_POINTS 1


class BaseShader;
class Gradient;
class BlendLayer;


/// @addtogroup DragPosition
/// @ingroup group_enumeration
/// @{
enum DragPosition
{
	DragPositionBefore,		///< Drag position before.
	DragPositionUnder,		///< Drag position under.
	DragPositionAfter,		///< Drag position after.
	DragPositionForbidden	///< Drag position forbidden
};
/// @}


class CBlendLayers : public CDynObjectArray<BlendLayer>
{
public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	CBlendLayers() : CDynObjectArray<BlendLayer>()
	{
		m_pLayer = nullptr;
	}

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual BlendLayer* AllocObject() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void FreeObject(BlendLayer*& pObj) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual BlendLayer* AllocObjectType(Int32 lID) const;

	//----------------------------------------------------------------------------------------
	/// Adds a new layer to the layer array.
	/// @param[in] t									The layer type.
	/// @param[in] lID								A unique ID for the layer.
	/// @param[in] bAppend						If @formatConstant{true} the added layer is inserted at the end of the layer list, otherwise at the start of the layer list.
	/// @return												The added layer, or @formatConstant{nullptr} if it failed.
	//----------------------------------------------------------------------------------------
	BlendLayer* Add(LayerType t, Int32 lID, Bool bAppend = true);

	//----------------------------------------------------------------------------------------
	/// Removes a layer.
	/// @param[in] pLayer							The layer to remove. @callerOwnsPointed{layer}
	/// @param[in] pParent						Not used. Pass @formatConstant{nullptr}.
	/// @param[in] pDoc								Not used. Pass @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	void Remove(BlendLayer* pLayer, BaseList2D* pParent, BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// Frees the layer array.
	//----------------------------------------------------------------------------------------
	virtual void Free();

	//----------------------------------------------------------------------------------------
	/// Gets the layer at the index @formatParam{l} in the array.
	/// @param[in] l									The index of the layer.
	/// @return												The layer, or @formatConstant{nullptr} if it cannot be found.
	//----------------------------------------------------------------------------------------
	BlendLayer* GetLayer(Int32 l);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool LinkLayer(BlendLayer* pMovedLayer, BlendLayer* pDestLayer, DragPosition pos);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void Unlink(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool ReadElementCreate(HyperFile* pFile, BlendLayer*& pLayer, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool WriteElementHF(HyperFile* pFile, const BlendLayer* pElement) const;

	//----------------------------------------------------------------------------------------
	/// Compares the layer array with @formatParam{pLayers}.
	/// @param[in] pLayers						The layer array to compare with. @callerOwnsPointed{layers array}
	/// @return												@em 0 if the arrays are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	Int32 Compare(CBlendLayers* pLayers);

	//----------------------------------------------------------------------------------------
	/// Finds a layer.
	/// @param[in] pLayer							The layer to search for. @callerOwnsPointed{layer}
	/// @return												The ID of the found layer.
	//----------------------------------------------------------------------------------------
	Int32 FindLayer(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	Bool CopyTo(CBlendLayers* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// Deselects all layers.
	//----------------------------------------------------------------------------------------
	void DeselectAll();

	//----------------------------------------------------------------------------------------
	/// Gets the selected layer.
	/// @return												The selected layer, or @formatConstant{nullptr} if no layer is selected.
	//----------------------------------------------------------------------------------------
	BlendLayer* FindSelected();

	//----------------------------------------------------------------------------------------
	/// Searches a layer by ID.
	/// @param[in] lLayerID						A layer ID.
	/// @return												The found layer, or @formatConstant{nullptr} if no layer exists for the specified ID.
	//----------------------------------------------------------------------------------------
	BlendLayer* SearchLayer(Int32 lLayerID);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool CompareStructure(CBlendLayers* p, Bool bCompareNames);

	//----------------------------------------------------------------------------------------
	/// Clears all linked shaders of the layers in the array.
	/// @param[in] pParent						Not used. Pass @formatConstant{nullptr}.
	/// @param[in] pDoc								Not used. Pass @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	void ClearShaders(BaseList2D* pParent, BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void UpdateShaderBitmaps(Int32 lMinHeight, BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void UpdateFolderBitmaps(Int32 lMinHeight, BaseDocument* pDoc);

	BlendLayer*	m_pLayer;	///< @markPrivate

private:
	CDynArray<BlendLayer*> m_arTransformLayers;	///< @markPrivate Contains all active layers that have the LAYER_FLAG_HAS_CHANGE_POINTS bit set for faster rendering.
};


class LayerShaderLayer;

//----------------------------------------------------------------------------------------
/// Base class of the different Layer shader layers. See LayerShaderLayer.
//----------------------------------------------------------------------------------------
class BlendLayer
{
private:
public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendLayer();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{layers}
	/// @param[in] pLayer							@theToDestruct{layer}
	//----------------------------------------------------------------------------------------
	static void Free(BlendLayer*& pLayer);

	CBlendLayers*	m_pParent;				///< @markPrivate
	String				m_strName;				///< @markPrivate
	LayerType			m_LayerType;			///< @markPrivate
	Bool					m_bDirty;					///< @markPrivate
	Bool					m_bIsSelected,		///< @markPrivate
								m_bIsActive;			///< @markPrivate
	Int32					m_lLayerID,				///< @markPrivate
								m_lFlags;					///< @markPrivate
	Int32					m_lRenderFlags;		///< @markPrivate
	BaseBitmap*		m_pAlphaChannel;	///< @markPrivate

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false) = 0;

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans) = 0;

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel) = 0;

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const = 0;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CompareStructure(BlendLayer* pLayer, Bool bCompareNames);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags) { }

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags) { }

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void ClearShaders(BaseList2D* pParent, BaseDocument* pDoc) { }

	//----------------------------------------------------------------------------------------
	/// Checks if the layer is a folder layer.
	/// @return												@trueIfOtherwiseFalse{it is a folder layer}
	//----------------------------------------------------------------------------------------
	inline Bool IsFolder()
	{
		return m_LayerType == TypeFolder;
	}

	//----------------------------------------------------------------------------------------
	/// Checks if the layer is a shader layer.
	/// @return												@trueIfOtherwiseFalse{it is a shader layer}
	//----------------------------------------------------------------------------------------
	inline Bool IsShader()
	{
		return m_LayerType == TypeShader;
	}

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID. For all layers: @enumerateEnum{LAYER_S_PARAM_ALL}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID. For all layers: @enumerateEnum{LAYER_S_PARAM_ALL}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	//----------------------------------------------------------------------------------------
	/// Gets the next Layer shader layer.
	/// @return												The next Layer shader layer. @theOwnsPointed{Layer shader,layer}
	//----------------------------------------------------------------------------------------
	LayerShaderLayer* GetNext();

	//----------------------------------------------------------------------------------------
	/// @see GetParameter()
	//----------------------------------------------------------------------------------------
	Bool GetParameter_(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// @see SetParameter()
	//----------------------------------------------------------------------------------------
	Bool SetParameter_(Int32 lID, const GeData& d);

	//----------------------------------------------------------------------------------------
	/// Gets the name of the layer.
	/// @param[in] pDoc								The document where the layer resides. @callerOwnsPointed{document}
	/// @return												The name of the layer.
	//----------------------------------------------------------------------------------------
	String GetName(BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// Gets the layer type.
	/// @return												The layer type.
	//----------------------------------------------------------------------------------------
	LayerType GetType();

protected:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendLayer();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool CopyBasics(BlendLayer* pDest);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	static Bool AllocDefaults(BlendLayer* pLayer);
};


class BlendFolder : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendFolder();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendFolder();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{folder layer}
	/// @return												@allocReturn{folder layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CompareStructure(BlendLayer* pLayer, Bool bCompareNames);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void ClearShaders(BaseList2D* pParent, BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_FOLDER}
	/// @param[out] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_FOLDER}
	/// @param[out] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	BlendMode		 m_BlendMode;	///< @markPrivate
	Float				 m_rBlend;		///< @markPrivate
	Bool				 m_bOpen;			///< @markPrivate
	CBlendLayers m_Children;	///< @markPrivate
};


class BlendShader : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendShader();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendShader();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{shader layer}
	/// @return												@allocReturn{shader layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	/// @markPrivate
	struct ShaderRenderData
	{
		Vector	 vUV, vBackP;
		Vector64 vWorld;
	};

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CompareStructure(BlendLayer* p, Bool bCompareNames);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void ClearShaders(BaseList2D* pParent, BaseDocument* pDoc);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_SHADER}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_SHADER}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	BlendMode	m_BlendMode;				///< @markPrivate
	Bool			m_bAlpha;						///< @markPrivate
	Float			m_rBlend;						///< @markPrivate
	BaseLink*	m_pLink;						///< @markPrivate
	Int32			m_lLastShaderDirty;	///< @markPrivate

protected:
	ShaderRenderData*	m_pRenderData;	///< @markPrivate
	BaseShader*				m_pShader;			///< @markPrivate
	Int32							m_lCPUCount;		///< @markPrivate
};


class BlendEffectBC : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectBC();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectBC();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{Brightness/Contrast layer}
	/// @return												@allocReturn{Brightness/Contrast layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_BC}
	/// @param[out] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_BC}
	/// @param[out] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Float	m_rBrightness,	///< @markPrivate
				m_rContrast,		///< @markPrivate
				m_rGamma;				///< @markPrivate

protected:
	Float	m_rRenderGamma,			///< @markPrivate
				m_rRenderContrast;	///< @markPrivate
};


class BlendEffectHSL : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectHSL();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectHSL();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{HSL layer}
	/// @return												@allocReturn{HSL layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_HSL}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_HSL}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Float	m_rHue,					///< @markPrivate
				m_rSaturation,	///< @markPrivate
				m_rLightness;		///< @markPrivate
	Bool m_bColorize;			///< @markPrivate

protected:
};


class BlendEffectPosterize : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectPosterize();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectPosterize();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{posterize layer}
	/// @return												@allocReturn{posterize layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_POSTER}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_POSTER}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Int32	m_lLevels;			///< @markPrivate
	Float	m_rFilterWidth;	///< @markPrivate

protected:
};


class BlendEffectColorize : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectColorize();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectColorize();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{colorize layer}
	/// @return												@allocReturn{colorize layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	/// @markPrivate
	enum ColInput { ColInputLum = 0, ColInputHue, ColInputSat, ColInputLight, ColInputRed, ColInputGreen, ColInputBlue };

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CompareStructure(BlendLayer* pLayer, Bool bCompareNames);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_COLORIZE}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_COLORIZE}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	ColInput m_Input;					///< @markPrivate
	GeData	 m_GradientData;	///< @markPrivate
	Bool		 m_bIsOpen,				///< @markPrivate
					 m_bCycle;				///< @markPrivate

protected:
	Gradient*	m_pGradient;				///< @markPrivate
	Gradient*	m_pPreviewGradeint;	///< @markPrivate
};


class BlendEffectClamp : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectClamp();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectClamp();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{clamp layer}
	/// @return												@allocReturn{colorize layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_CLAMP}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_CLAMP}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Float	m_rLowClip,		///< @markPrivate
				m_rHighClip;	///< @markPrivate

protected:
};


class BlendEffectClip : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectClip();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectClip();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{clip layer}
	/// @return												@allocReturn{clip layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_CLIP}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_CLIP}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Float	m_rLowClip,		///< @markPrivate
				m_rHighClip;	///< @markPrivate

protected:
};


class BlendEffectTransform : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectTransform();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectTransform();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{transform layer}
	/// @return												@allocReturn{transform layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write with. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_TRANS}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_TRANS}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	Float	 m_rAngle;	///< @markPrivate
	Bool	 m_bMirror,	///< @markPrivate
				 m_bFlip;		///< @markPrivate
	Vector m_vScale,	///< @markPrivate
				 m_vMove;		///< @markPrivate

protected:
	Float	 m_rCos,					///< @markPrivate
				 m_rSin;					///< @markPrivate
	Vector m_vRenderScale;	///< @markPrivate
};


class BlendEffectDistorter : public BlendLayer
{
private:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BlendEffectDistorter();

public:
	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BlendEffectDistorter();

	//----------------------------------------------------------------------------------------
	/// @allocatesA{transform layer}
	/// @return												@allocReturn{distorter layer}
	//----------------------------------------------------------------------------------------
	static BlendLayer* Alloc();

	/// @markPrivate
	enum WrapType { WrapNone, WrapClamp, WrapCycle, WrapSeamless };

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual String GetLayerName(BaseDocument* pDoc, Bool bReset = false);

	//----------------------------------------------------------------------------------------
	/// Reads the layer from a hyper file.
	/// @param[in] pFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] lLevel							The disklevel of the layer to read.
	/// @return												@trueIfOtherwiseFalse{the layer was read}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 lLevel);

	//----------------------------------------------------------------------------------------
	/// Writes the layer to a hyper file.
	/// @param[in] pFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the layer was written}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares two layers.
	/// @param[in] pLayer							The layer to compare with. @callerOwnsPointed{layer}
	/// @return												@em 0 if the layers are identical, otherwise @em 1.
	//----------------------------------------------------------------------------------------
	virtual Int32 Compare(BlendLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Copies all values from the layer to @formatParam{pDest}.
	/// @param[in] pDest							The destination layer. @callerOwnsPointed{layer}
	/// @param[in] pTrans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{the layer was copied}
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(BlendLayer* pDest, AliasTrans* pTrans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_DISTORT}
	/// @param[in] d									Filled with the layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 lID, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] lID								The layer parameter ID: @enumerateEnum{LAYER_S_PARAM_DISTORT}
	/// @param[in] d									The layer parameter value.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 lID, const GeData& d);

	NoiseType	m_Noise;					///< @markPrivate
	Float			m_rNoiseStrength,	///< @markPrivate
						m_rOctaves,				///< @markPrivate
						m_rTimeScale,			///< @markPrivate
						m_rNoiseScale;		///< @markPrivate
	Bool		 m_bUse3DNoise;			///< @markPrivate
	WrapType m_Wrap;						///< @markPrivate

protected:
	Float	m_rInvNoiseScale;	///< @markPrivate
};


#pragma pack (pop)
}				// end namespace cineware

#endif	// C4D_BLENDLAYERS_H__
