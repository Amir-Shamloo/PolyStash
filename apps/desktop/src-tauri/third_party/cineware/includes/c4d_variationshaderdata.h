/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VARIATIONSHADEROBJECTDATA_H__
#define C4D_VARIATIONSHADEROBJECTDATA_H__

#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseShader;

/// @addtogroup DISTRIBUTION
/// @ingroup group_enumeration
/// @{
/// Modes to distribute probability of variation shader textures.
enum DISTRIBUTION
{
	DISTRIBUTION_NORMALIZE = 0,	///< Normalize all values to have a sum of 100% of all.
	DISTRIBUTION_EVENLY		 = 1,	///< Same value for each texture to reach 100% in sum.
} ENUM_END_LIST(DISTRIBUTION);
/// @}

//----------------------------------------------------------------------------------------
/// Data structure for texture layers of variation shader.
//----------------------------------------------------------------------------------------
struct TextureLayer
{
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	TextureLayer()
	{
		_shader = nullptr;
		_active = true;
		_probability = 1.0;
	}

	//----------------------------------------------------------------------------------------
	/// Constructor.
	/// @param[in] shader								Pointer to the shader or @formatConstant{nullptr}.
	/// @param[in] active								Enabled or disabled state.
	/// @param[in] propability					Probability in relation to all active layers.
	//----------------------------------------------------------------------------------------
	TextureLayer(BaseShader* shader, Bool active, Float propability)
	{
		_shader = shader;
		_active = active;
		_probability = propability;
	}

	BaseShader* _shader;			///< Pointer to the shader, or @formatConstant{nullptr}.
	Bool				_active;			///< Enabled or disabled state.
	Float				_probability;	///< Probability in relation to all active layers.
};

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Variation shader.
//----------------------------------------------------------------------------------------
class VariationShaderData : public NodeData
{
	INSTANCEOF(VariationShaderData, NodeData)

private:
	virtual Bool Init();
	virtual void Free();

public:
	/// @name Allocation/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{VariationShaderData}
	/// @return												@allocReturn{VariationShaderData}
	//----------------------------------------------------------------------------------------
	static VariationShaderData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{VariationShaderData}
	/// @param[in,out] v							@theToDestruct{VariationShaderData}
	//----------------------------------------------------------------------------------------
	static void Free(VariationShaderData*& v);

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the variation shader data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The ID of the node to read.
	/// @param[in] level							The disklevel of the node to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the variation shader to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}

	/// @name Add / Remove textures
	/// @{

	//----------------------------------------------------------------------------------------
	/// Adds a new texture layer.
	/// @note	A passed shader will be added automatically, a @formatConstant{nullptr} will just keep the link empty.
	/// @param[in] layerData					The data to be used for the new texture layer.
	/// @return												@trueIfOtherwiseFalse{the texture layer was successfully added}
	//----------------------------------------------------------------------------------------
	Bool AddTextureLayer(TextureLayer layerData);

	//----------------------------------------------------------------------------------------
	/// Gets the data of the texture layer.
	/// @param[in] textureId					The ID to get the data for.
	/// @return												The texture layer data.
	//----------------------------------------------------------------------------------------
	TextureLayer GetTextureLayer(Int32 textureId);

	//----------------------------------------------------------------------------------------
	/// Removes a texture layer.
	/// @note	If there is only one texture layer nothing will be removed.
	/// @param[in] textureId					The ID of the texture layer to remove.
	/// @return												@trueIfOtherwiseFalse{the texture layer was successfully removed}
	//----------------------------------------------------------------------------------------
	Bool RemoveTextureLayer(Int32 textureId);

	//----------------------------------------------------------------------------------------
	/// Removes all texture layers from the shader list except the first one.
	/// @return												@trueIfOtherwiseFalse{all texture layers were successfully removed}
	//----------------------------------------------------------------------------------------
	Bool RemoveAllTextures();

	//----------------------------------------------------------------------------------------
	/// Returns the amount of textures the shader list contains.
	/// @note	The minimum amount of textures is 1.
	/// @return												The number of textures the shader list contains.
	//----------------------------------------------------------------------------------------
	Int32 GetTextureCount();

	//----------------------------------------------------------------------------------------
	/// Returns the activation state of the texture layer.
	/// @param[in] textureId					The ID of the layer to get the state for.
	/// @return												@trueIfOtherwiseFalse{the texture layer is active}
	//----------------------------------------------------------------------------------------
	Bool GetActive(Int32 textureId);

	//----------------------------------------------------------------------------------------
	/// Enables or disables a texture layer.
	/// @param[in] textureId					The ID of the texture to set the state for.
	/// @param[in] active							@formatConstant{true} to enable or or @formatConstant{false} to disable the layer.
	/// @return												@trueIfOtherwiseFalse{the state could be set}
	//----------------------------------------------------------------------------------------
	Bool SetActive(Int32 textureId, Bool active);

	//----------------------------------------------------------------------------------------
	/// Returns the probability of the texture layer.
	/// @param[in] textureId					The ID of the layer to get the probability for.
	/// @return												The probability.
	//----------------------------------------------------------------------------------------
	Float GetPropability(Int32 textureId);

	//----------------------------------------------------------------------------------------
	/// Sets the probability for the texture layer.
	/// @param[in] textureId					The ID of the texture to set the probability for.
	/// @param[in] probability				The new probability value of the layer.
	/// @return												@trueIfOtherwiseFalse{the probability could be set}
	//----------------------------------------------------------------------------------------
	Bool SetPropability(Int32 textureId, Float probability);

	//----------------------------------------------------------------------------------------
	/// Distributes the probability values for all textures in the shader list.
	/// @note	For normalized distribution at least 2 textures are needed. Otherwise the single one value will not be changed.
	/// @param[in] dis								The type of distribution: @enumerateEnum{DISTRIBUTION}
	/// @return												@trueIfOtherwiseFalse{the distribution was successful}
	//----------------------------------------------------------------------------------------
	Bool Distribute(DISTRIBUTION dis);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_VARIATIONSHADEROBJECTDATA_H__
