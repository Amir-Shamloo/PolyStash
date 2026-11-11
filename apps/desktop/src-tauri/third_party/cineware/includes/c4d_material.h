/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_MATERIAL_H__
#define C4D_MATERIAL_H__

#include "c4d_basematerial.h"
#include "c4d_basechannel.h"
#include "private_reflectionmain.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// The standard @C4D material.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class Material : public BaseMaterial
{
	friend class ReflectionLayer;

	INSTANCEOF(Material, BaseMaterial)

private:
	BaseChannel					m_channel[14];
	Bool								m_on[MAX_MATERIALCHANNELS];

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

protected:
	MaterialReflections reflections;

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	Material();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{standard material}
	/// @return												@allocReturn{standard material}
	//----------------------------------------------------------------------------------------
	static Material* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{standard materials}
	/// @param[in,out] mat						@theToDestruct{standard material}
	//----------------------------------------------------------------------------------------
	static void Free(Material*& mat);

	/// @}

	/// @name Channel State
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the enable state of a channel.
	/// @param[in] id									The channel to check: @ref CHANNEL.
	/// @return												The enable state of the channel.
	//----------------------------------------------------------------------------------------
	Bool GetChannelState(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Sets the enable state of a channel.
	/// @param[in] id									The channel to change: @ref CHANNEL.
	/// @param[in] state							The enable state.
	//----------------------------------------------------------------------------------------
	void SetChannelState(Int32 id, Bool state);

	/// @}

	/// @name Reflection/Specular Layer Access
	/// @{

	//----------------------------------------------------------------------------------------
	/// Adds a reflection/specular layer.
	/// @return												The added reflection/specular layer. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	ReflectionLayer* AddReflectionLayer();

	//----------------------------------------------------------------------------------------
	/// Retrieves a reflection/specular layer by ID.
	/// @param[in] id									The layer ID.
	/// @return												The reflection/specular layer. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	ReflectionLayer* GetReflectionLayerID(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Retrieves a reflection/specular layer by index.
	/// @param[in] index							The layer index.
	/// @return												The reflection/specular layer. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	ReflectionLayer* GetReflectionLayerIndex(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Retrieves the transparency layer.
	/// @return												The transparency layer, or @formatConstant{nullptr} if not available. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	ReflectionLayer* GetReflectionLayerTrans();

	//----------------------------------------------------------------------------------------
	/// Retrieves the number of reflection/specular layers.
	/// @return												The reflection/specular layers' count.
	//----------------------------------------------------------------------------------------
	Int32 GetReflectionLayerCount();

	//----------------------------------------------------------------------------------------
	/// Removes a reflection/specular layer by ID.
	/// @param[in] id									The layer ID.
	//----------------------------------------------------------------------------------------
	void RemoveReflectionLayerID(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Removes a reflection/specular layer by index.
	/// @param[in] index							The layer index.
	//----------------------------------------------------------------------------------------
	void RemoveReflectionLayerIndex(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Deletes all reflection/specular layers.
	//----------------------------------------------------------------------------------------
	void RemoveReflectionAllLayers();

	//----------------------------------------------------------------------------------------
	/// Retrieves all the reflection/specular shaders.
	/// @param[out] shaders						Assigned all the reflection/specular shaders. @cinewareOwnsPointed{shaders}
	//----------------------------------------------------------------------------------------
	void GetAllReflectionShaders(BaseArray<BaseShader*>& shaders);

	//----------------------------------------------------------------------------------------
	/// Retrieves the indices to the primary reflection and specular layers (can be -1 for empty).
	/// @param[out] reflection_layer	The most significant reflection layer.
	/// @param[out] specular_layer		The most significant specular layer.
	//----------------------------------------------------------------------------------------
	void GetReflectionPrimaryLayers(Int32& reflection_layer, Int32& specular_layer);

	/// @}

	/// @name Shader
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the shader of a channel.
	/// @param[in] id									The shader ID.
	/// @return												The shader, or @formatConstant{nullptr} if the channel does not contain a shader. @theOwnsPointed{channel,shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetShader(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Creates a bitmap shader (::Xbitmap) in a channel and load a texture into it.
	/// @param[in] id									The channel: @ref CHANNEL.
	/// @param[in] filename						The filename of the texture to load.
	/// @return												The bitmap shader, or @formatConstant{nullptr} if no bitmap was loaded.
	//----------------------------------------------------------------------------------------
	BaseShader* MakeBitmapShader(Int32 id, Filename filename);

	//----------------------------------------------------------------------------------------
	/// Creates a shader for a channel.
	/// @param[in] channelID					The channel: @ref CHANNEL.
	/// @param[in] shaderType					The shader type, for instance @em Xnoise.
	/// @return												The shader, or @formatConstant{nullptr} if the creation failed.
	/// @markTODO{Add shader type IDs to @formatParam{shaderType}}
	//----------------------------------------------------------------------------------------
	BaseShader* MakeShader(Int32 channelID, Int32 shaderType);

	/// @}

	/// @name Channel
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a channel associated with the material, such as the color channel or bump channel.
	/// @param[in] id									The channel: @ref CHANNEL.
	/// @return												The channel.
	//----------------------------------------------------------------------------------------
	BaseChannel& GetChannel(Int32 id);

	/// @}

	/// @name Alien Overload
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this for self defined materials.\n
	/// <b>Example:</b>
	/// @code
	/// Bool AlienMaterial::Execute()
	/// {
	/// Char *pName = GetName().GetCStringCopy();
	/// if (pName)
	///   {
	///     printf("\n - AlienMaterial (%d): %s\n", (int)GetType(), pName);
	///     DeleteMem(pChar);
	///   }
	///   else
	///     printf("\n - AlienMaterial (%d): <noname>\n", (int)GetType());
	///
	///   PrintUniqueIDs(this);
	///
	///   // Assign a id to the material
	///   id = tempmatid;
	///
	///   tempmatid++;
	///
	///   // Material preview custom datatype
	///   GeData data;
	///   if (GetParameter(MATERIAL_PREVIEW, data))
	///   {
	///     MaterialPreviewData* pPreview = (MaterialPreviewData*)data.GetCustomDataType(CUSTOMDATATYPE_MATPREVIEW);
	///     if (pPreview)
	///     {
	///       MatPreviewType previewType = pPreview->GetPreviewType();
	///       MatPreviewSize previewSize = pPreview->GetPreviewSize();
	///       printf("MaterialPreview: Type:%d Size:%d\n", previewType, previewSize);
	///     }
	///   }
	///
	///   PrintShaderInfo(GetShader(MATERIAL_COLOR_SHADER), 4);
	///   PrintAnimInfo(this);
	///
	///   return true;
	/// }
	/// @endcode
	/// @return												@trueIfOtherwiseFalse{the material type is supported by the external application}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool IsParameterReflection(const DescID& id) const;

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_MATERIAL_H__
