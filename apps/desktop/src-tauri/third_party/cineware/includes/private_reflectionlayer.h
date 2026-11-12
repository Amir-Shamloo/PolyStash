/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef REFLECTIONLAYER_H__
#define REFLECTIONLAYER_H__

#include "c4d_file.h"
#include "c4d_reflection.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class XReflectionLayers;
class XRClothWeavePattern;
class BaseLink;

/// @markInternal
struct XRShaderValue
{
	BaseLink*							m_pLink;
	BaseShader*						m_pShader;
	Int32									m_nDirty;
	Float									m_fValue;

	XRShaderValue();
	~XRShaderValue();

	void									Alloc(Bool &bSuccess, Float fValue);
	Bool									Read(HyperFile* pFile);
	Bool									Write(HyperFile* pFile) const;
	Bool									CopyTo(XRShaderValue* pDest, BaseDocument *pDoc, Material *pMaterial, AliasTrans* pTrans, Bool bCopyShaders) const;
	Bool									CopyFrom(const XRShaderValue* pSrc, BaseDocument *pDoc, Material *pMaterial, AliasTrans* pTrans, Bool bCopyShaders);
	Bool									IsChanged(BaseDocument *pDoc, Float fOriginalValue);
	void									CopyLink(const GeData &t_data, BaseDocument *pDoc);
	BaseShader*						GetShader(BaseDocument *pDoc) const;
	int										operator !=(const XRShaderValue& rhs) const;
};

/// @markInternal
class XReflectionLayer : public ReflectionLayer
{
public:
	Int32								_flags;
	XReflectionLayers*	m_pParent;
	String							_oldName;

	// Multipass
	Int32									m_nReflectionIndex;
	Int32									m_nSpecularIndex;

	// Images and Preview
	Int32									m_nDirtyPreview;
	Int32									m_nDirtyModel;
	Int32									m_nDirtyAnisotropy;
	Int32									m_nDirtyPrimary;
	Int32									m_nDirtySecondary;
	Int32									m_nDirtyDistance;
	Int32									m_nDirtyCloth;

	// Main
	Int32									m_nDistribution;
	Int32									m_nAdditiveMode;
	Int32									m_nBlendMode;
	Bool									m_bColoredRefl;
	Bool									m_bColoredSpec;
	Bool									m_bPreviewMode;
	Bool									m_bLinearTextures;

	XRShaderValue					m_sRoughness;
	XRShaderValue					m_sFalloff;
	XRShaderValue					m_sInner;
	XRShaderValue					m_sOrenNayar;
	XRShaderValue					m_sReflection;
	XRShaderValue					m_sSpecular;

	// Bump
	Int32									m_nBumpMode;
	Int32									m_nBumpSpace;
	Float									m_fBumpValueBump;
	Float									m_fBumpValueNorm;
	Bool									m_bBumpMIPMap;
	Bool									m_bBumpFlipX;
	Bool									m_bBumpFlipY;
	Bool									m_bBumpFlipZ;
	Bool									m_bBumpSwap;
	XRShaderValue					m_sBumpMap;
	XRShaderValue					m_sBumpMapCustom;

	// Color
	Vector								m_vColorColor;
	Float									m_fColorBrightness;
	Int32									m_nColorMixMode;
	Float									m_fColorMixStrength;
	XRShaderValue					m_sColorTexture;

	// Transparency
	Vector								m_vTransColor;
	Float									m_fTransAmount;
	Int32									m_nTransMixMode;
	Float									m_fTransMixStrength;
	XRShaderValue					m_sTransTexture;

	// Anisotropy
	Int32									m_nAnisoProjection;
	Int32									m_nAnisoPattern;
	Int32									m_nAnisoMirror;
	Int32									m_nAnisoScratchesMode;
	Int32									m_nAnisoProjectionCount;
	Float									m_fAnisoProjectionScale;
	Float									m_fAnisoProjectionAngle;
	Float									m_fAnisoProjectionOffsetU;
	Float									m_fAnisoProjectionOffsetV;
	XRShaderValue					m_sAnisoPatternCustom;
	XRShaderValue					m_sAnisoAnisotropy;
	XRShaderValue					m_sAnisoOrientation;
	XRShaderValue					m_sAnisoPrimaryAmplitude;
	XRShaderValue					m_sAnisoPrimaryScale;
	XRShaderValue					m_sAnisoPrimaryLength;
	XRShaderValue					m_sAnisoPrimaryAttenuate;
	XRShaderValue					m_sAnisoSecondaryAmplitude;
	XRShaderValue					m_sAnisoSecondaryScale;
	XRShaderValue					m_sAnisoSecondaryLength;
	XRShaderValue					m_sAnisoSecondaryAttenuate;

	// Cloth
#if 0 // no need for Cineware SDK
	maxon::ClothWeavePatternRef	_clothPattern;
#endif

	Int32									m_nClothPreset;
	Int32									m_nClothPattern;
	Int32									m_nClothQuality;
	Float									m_fClothSmoothing;
	Float									m_fClothScatteringA;
	Float									m_fClothScatteringB;
	XRShaderValue					m_sClothOrientation;
	XRShaderValue					m_sClothScaleU;
	XRShaderValue					m_sClothScaleV;
	XRShaderValue					m_sClothHighlights;

	Float									m_fClothNoiseIntensity;
	Float									m_fClothNoiseScale;
	Float									m_fClothNoiseShapeWarp;
	Float									m_fClothNoiseShapeWeft;
	Float									m_fClothNoiseShapeScale;
	Vector								m_vClothDiffuseWarp;
	Vector								m_vClothDiffuseWeft;
	Vector								m_vClothSpecularWarp;
	Vector								m_vClothSpecularWeft;
	XRShaderValue					m_sClothDiffuseWarp;
	XRShaderValue					m_sClothDiffuseWeft;
	XRShaderValue					m_sClothSpecularWarp;
	XRShaderValue					m_sClothSpecularWeft;

	// Fresnel
	Int32									m_nFresnelMode;
	Int32									m_nFresnelPreset;
	Int32									m_nFresnelMetal;
	Bool									m_bFresnelInverted;
	Bool									m_bFresnelOpaque;
	XRShaderValue					m_sFresnelStrength;
	XRShaderValue					m_sFresnelIOR;
	XRShaderValue					m_sFresnelEta;
	XRShaderValue					m_sFresnelAbsorption;

	// Precomputed
	Vector								m_vFresnelIOR;
	Vector								m_vFresnelK;

	// Sampling
	Int32									m_nSamples;
	Bool									m_bDimEnabled;
	Float									m_fDimDistance;
	Float									m_fDimFalloff;
	Float									m_fClamp;
	Float									m_fCutoff;
	Vector								m_vDimColor;
	Vector								m_vExitColor;

	// Debug
	Int32									m_nDebugDUDVMode;
	Int32									m_nDebugScratchMode;
	Int32									m_nDebugClothMode;

	UInt32 m_SpecialDirty;

	Float									_layerStrength;
	Float									_roughness;
	Float									_customBumpStrengthBump;
	Float									_customBumpStrengthNormal;
	Vector								_colorColor;
	Float									_colorBrightness;
	Float									_colorMixStrength;
	Vector								_transColor;
	Float									_transMixStrength;

	XReflectionLayer();
	virtual ~XReflectionLayer();

	//----------------------------------------------------------------------------------------
	/// Allocates a new instance.
	/// @return												The new instance. Caller owns the pointed object and is responsible to free it. @see XReflectionLayer::Free()
	//----------------------------------------------------------------------------------------
	static XReflectionLayer* Alloc();

	//----------------------------------------------------------------------------------------
	/// Frees a reflection layer.
	/// @param[in,out] pLayer					The layer to free.
	//----------------------------------------------------------------------------------------
	static void Free(XReflectionLayer *&pLayer) { DeleteObj(pLayer); }

	Int32 GetLayerDataID() const;
	void SetLayerDataID(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Fills reflection layer @formatParam{pData} settings with default values.
	/// @param[out] pData							Reflection layer container.
	//----------------------------------------------------------------------------------------
	void SetupLayer(BaseContainer* pData);

	//----------------------------------------------------------------------------------------
	// Flags
	//----------------------------------------------------------------------------------------
	Bool CheckFlag(Int32 nFlag) const;
	void SetFlag(Int32 nFlag);
	void ResetFlag(Int32 nFlag);
	void ChangeFlag(Int32 nFlag, Bool bValue);

	//----------------------------------------------------------------------------------------
	/// Copies the reflection layer property with id @formatParam{nID}
	/// @param[in] pObjectSource			Source object to copy data from.
	/// @param[out] pObjectTarget			Target object to copy data to.
	/// @param[in] nSourceID					Source		layer data id. @see ReflectionLayer::GetDataID()
	/// @param[in] nTargetID					Target		layer data id. @see ReflectionLayer::GetDataID()
	/// @param[in] nID								Reflection			layer parameter ID.
	//----------------------------------------------------------------------------------------
	void CopyData(BaseList2D* pObjectSource, BaseList2D* pObjectTarget, Int32 nSourceID, Int32 nTargetID, Int32 nID, Bool anim) const;

	//----------------------------------------------------------------------------------------
	void UpdateName(XReflectionLayers& layers, BaseContainer& data);

	//----------------------------------------------------------------------------------------
	/// Gets the layer blend value. @see REFLECTION_LAYER_TRANS_BRIGHTNESS
	/// @param[in] pParent						The layer owner.
	/// @return												The layer blend value.
	//----------------------------------------------------------------------------------------
	virtual Float GetLayerValue(Material *pParent);

	//----------------------------------------------------------------------------------------
	/// Gets the layer blend mode. @see REFLECTION_LAYER_MAIN_BLEND_MODE
	/// @param[in] pParent						The material that owns the reflection layer.
	/// @return												The layer Blend mode. @see MATERIAL_COLOR_TEXTUREMIXING
	//----------------------------------------------------------------------------------------
	virtual Int32 GetLayerMode(Material *pParent);

	//----------------------------------------------------------------------------------------
	/// Copies this layer (the source) data to @formatParam{pDestLayer} layer data.
	/// @param[in] layers							The layer list that owns the layers.
	/// @param[out] pDestLayer				The layer to copy data to.
	/// @param[in] pSource						The source layer owner.
	/// @param[out] pTarget						The target layer owner.
	/// @param[in] bCopyID						If true the layer IDs will be copied.
	/// @param[in] bCopyName					If true the layer name will be copied.
	/// @param[in] bUpdateName				If this and @formatParam{bCopyName} are true, the destination layer name will be updated (the number suffix)
	/// @param[in] bCopyShaders				If true layer shaders will be copied.
	/// @param[in] copyAnim						If true layer animation tracks will be copied.
	/// @return												True on success, otherwise false.
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTox(XReflectionLayers& layers, XReflectionLayer* pDestLayer, BaseList2D* pSource, BaseList2D* pTarget, Bool bCopyID, Bool bCopyName, Bool bUpdateName, Bool bCopyShaders, Bool copyAnim) const;

	//----------------------------------------------------------------------------------------
	void CopyParameters(BaseList2D* pObjectSource, BaseList2D* pObjectTarget, Int32 nSourceID, Int32 nTargetID, Bool anim) const;

	//----------------------------------------------------------------------------------------
	/// Reads layer data from file.
	/// @param[in] pFile							File to read from.
	/// @param[in] nLevel							Disk level.
	/// @return												True on success, otherwise false.
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* pFile, Int32 nLevel);

	//----------------------------------------------------------------------------------------
	/// Writes layer data to file.
	/// @param[out] pFile							The file to write to.
	/// @return												True on success, otherwise false.
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* pFile) const;

	//----------------------------------------------------------------------------------------
	/// Compares this layer with @formatParam{pLayer} layer.
	/// @param[in] pLayer							The layer to compare with.
	/// @return												0 if equal, 1 if distinct.
	//----------------------------------------------------------------------------------------
 	virtual Int32 Compare(XReflectionLayer* pLayer);

	//----------------------------------------------------------------------------------------
	/// Compares this layer name with @formatParam{pLayer} layer name.
	/// @param[in] pLayer							The layer to compare with.
	/// @return												True if both layer names are equal, otherwise false.
	//----------------------------------------------------------------------------------------
	virtual Bool CompareStructure(XReflectionLayer& pLayer);

	//----------------------------------------------------------------------------------------
	// Remove
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	/// Removes a specific reflection property animation tracks.
	/// @param[out] pData							Reflection layer data container, normally @formatParam{pParent} material @formatParam{data} container.
	/// @param[out] pParent						Material that holds the reflection layer.
	/// @param[in] nID								Id of the reflection property to remove its animation tracks.
	//----------------------------------------------------------------------------------------
	static void RemoveNotifyID(BaseContainer *pData, Material *pParent, Int32 nID);

	//----------------------------------------------------------------------------------------
	/// Removes reflection layer animation tracks.
	/// @param[out] pData							Reflection layer data container, normally @formatParam{pParent} material @formatParam{data} container.
	/// @param[out] pParent						Material that holds the reflection layer.
	/// @param[in] bAddUndo						Set to true to add the action to the undo stack, set to false to ignore undo.
	//----------------------------------------------------------------------------------------
	virtual void RemoveNotify(BaseContainer *pData, Material *pParent, Bool bAddUndo);

	static void RemoveParamsNotify(Int32 nDataID, BaseContainer *pData, Material *pParent, Bool bAddUndo);

	//----------------------------------------------------------------------------------------
	// Rename
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	/// Updates the name the layer @formatParam{nID} animation track.
	/// @param[in] pData							The data that holds the layer values. Normally is @formatParam{pParent} data.
	/// @param[in] pParent						The material that owns the layer.
	/// @param[in] nID								Id of the parameter to update its animation track name.
	//----------------------------------------------------------------------------------------
	virtual void RenameNotifyID(BaseContainer *pData, Material *pParent, Int32 nID);

	//----------------------------------------------------------------------------------------
	/// Updates the names of the layer animation tracks.
	/// @param[in] pData							The data that holds the layer values. Normally is @formatParam{pParent} data.
	/// @param[in] pParent						The material that owns the layer.
	/// @param[in] bAddUndo						Unused.
	//----------------------------------------------------------------------------------------
	virtual void					RenameNotify(BaseContainer *pData, Material *pParent, Bool bAddUndo);

	//----------------------------------------------------------------------------------------
	// Shaders
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	/// Checks if @formatParam{pShader} shader is the same as shader with id @formatParam{nShaderID}.
	/// @param[in] pData							Reflection layer data.
	/// @param[in] pDoc								Document that owns the layers.
	/// @param[in] pShader						The shader to compare with.
	/// @param[in] nShaderID					The id of the layer shader to compare with.
	/// @return												True if both shaders are equal, otherwise false.
	//----------------------------------------------------------------------------------------
	virtual Bool CheckShader(BaseContainer *pData, BaseDocument* pDoc, BaseShader *pShader, Int32 nShaderID);

	//----------------------------------------------------------------------------------------
	// Get All Shaders
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	/// Gets the shader with @formatParam{nShader} id contained in this layer.
	/// @param[in] pData							The data container of the material that owns the reflection channel.
	/// @param[in] pDoc								The document that owns the material.
	/// @param[out] ShaderList				Assigned the reflection/specular shader if found. @cinewareOwnsPointed{shaders}
	/// @param[in] nShaderID					The shader id to get.
	//----------------------------------------------------------------------------------------
	virtual void GetShader(BaseContainer *pData, BaseDocument* pDoc, BaseArray<BaseShader*> &ShaderList, Int32 nShaderID);

	//----------------------------------------------------------------------------------------
	/// Retrieves all the reflection/specular shaders in this layer.
	/// @param[in] pData							The data container of the material that owns the reflection channel.
	/// @param[in] pDoc								The document that owns the material.
	/// @param[out] ShaderList				Assigned all the reflection/specular shaders. @cinewareOwnsPointed{shaders}
	//----------------------------------------------------------------------------------------
	virtual void GetAllShaders(BaseContainer *pData, BaseDocument* pDoc, BaseArray<BaseShader*> &ShaderList);

	//----------------------------------------------------------------------------------------
	// Fresnel Presets
	//----------------------------------------------------------------------------------------
	static Float ComputeFresnelDielectric(Int32 nFresnelPreset);
	static void ComputeFresnelMetal(Int32 nFresnelMetal, Vector& vFresnelIOR, Vector& vFresnelK);

	//----------------------------------------------------------------------------------------
	/// Fills reflection layer @formatParam{pData} settings with a cloth pattern @formatParam{mPreset} values.
	/// @param[out] pData							The reflection layer container.
	/// @param[in] nPreset						Cloth preset. @see REFLECTION_CLOTH_PRESET.
	//----------------------------------------------------------------------------------------
	void SetupClothPreset(BaseContainer *pData, Int32 nPreset);

	//----------------------------------------------------------------------------------------
	// Mixing Sampling
	//----------------------------------------------------------------------------------------
	Vector								CalculateMix(Vector vShaderColor, Vector vValue, Int32 nMixMode, Float fMixValue);

	virtual Bool					GetParameter(Int32 nID, GeData &d) const;
	virtual Bool					SetParameter(Int32 nID, const GeData &d);
	Bool									GetParameter_(Int32 nID, GeData &d) const { return GetParameter(nID, d); }
	Bool									SetParameter_(Int32 nID, const GeData &d) { return SetParameter(nID, d); }

	//----------------------------------------------------------------------------------------
	/// Gets the next layer.
	/// @return												The next layer or nullptr if no layer.
	//----------------------------------------------------------------------------------------
	XReflectionLayer* GetNext();

	void SetMaterial(Material* mat);

	//----------------------------------------------------------------------------------------
	/// Retrieves the layer ID.
	/// @return												The layer ID.
	//----------------------------------------------------------------------------------------
	Int32 GetLayerID(const BaseContainer& data) const;

	//----------------------------------------------------------------------------------------
	/// Sets the layer ID.
	/// @param[in] layerID						The layer id.
	//----------------------------------------------------------------------------------------
	void SetLayerID(BaseContainer& data, Int32 layerID);

	//----------------------------------------------------------------------------------------
	/// Retrieves the layer name.
	/// @param[in] data								The layer's material data.
	/// @return												The layer name.
	//----------------------------------------------------------------------------------------
	String GetName(const BaseContainer& data) const;

	//----------------------------------------------------------------------------------------
	/// Sets the layer name. If not valid (i.e. empty or all chars are whitespaces) a default layer name will be used.
	/// @param[in] name								The layer name.
	/// @param[in] data								The layer's material data.
	//----------------------------------------------------------------------------------------
	void SetName(const String& name, BaseContainer& data);

	//----------------------------------------------------------------------------------------
	/// Retrieves the flags for the layer: @enumerateEnum{REFLECTION_FLAG}
	/// @return												The layer flags.
	//----------------------------------------------------------------------------------------
	Int32 GetFlags() const;

	//----------------------------------------------------------------------------------------
	/// Sets the flags for the layer: @enumerateEnum{REFLECTION_FLAG}
	/// @param[in] flags							The layer flags.
	//----------------------------------------------------------------------------------------
	void SetFlags(Int32 flags);

	static UChar RemapReflectionDistributionForIBL(UChar reflectionDistribution);
	static Bool MaterialHasRealReflections(Int32 reflectionDistribution);
};

#pragma pack (pop)
}

// ---
#endif	// REFLECTIONLAYER_H__
