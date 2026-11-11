/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LAYERSHADERLAYER_H__
#define C4D_LAYERSHADERLAYER_H__

#include "blendlayers.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a layer in the layer shader.\n
/// The layer shader can contain several layers of different types (shaders, folders, effects etc.).
//----------------------------------------------------------------------------------------
class LayerShaderLayer : public BlendLayer
{
public:
	//----------------------------------------------------------------------------------------
	/// Convenience method to get the next layer in the layer shader list of layers.
	/// @return												The next layer. @theOwnsPointed{layer shader,layer}
	//----------------------------------------------------------------------------------------
	LayerShaderLayer* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the layer type.
	/// @return												The layer type.
	//----------------------------------------------------------------------------------------
	LayerType GetType();

	//----------------------------------------------------------------------------------------
	/// Gets the name of the layer.
	/// @param[in] doc								The document for the operation. @callerOwnsPointed{document}
	/// @return												The name of the layer.
	//----------------------------------------------------------------------------------------
	String GetName(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Gets layer parameters.
	/// @param[in] id									The ID of the parameter:\n
	///																For all layers: @enumerateEnum{LAYER_S_PARAM_ALL} \n
	///																For folders (@ref TypeFolder): @enumerateEnum{LAYER_S_PARAM_FOLDER} \n
	///																For shaders (@ref TypeShader): @enumerateEnum{LAYER_S_PARAM_SHADER} \n
	///																For brightness/contrast layers (@ref TypeBrightnessContrast): @enumerateEnum{LAYER_S_PARAM_BC} \n
	///																For HSL layers (@ref TypeHSL): @enumerateEnum{LAYER_S_PARAM_HSL} \n
	///																For posterize layers (@ref TypePosterize): @enumerateEnum{LAYER_S_PARAM_POSTER} \n
	///																For colorize layers (@ref TypeColorize): @enumerateEnum{LAYER_S_PARAM_COLORIZE} \n
	///																For clamp layers (@ref TypeClamp): @enumerateEnum{LAYER_S_PARAM_CLAMP} \n
	///																For clip layers (@ref TypeClip): @enumerateEnum{LAYER_S_PARAM_CLIP} \n
	///																For distorter layers (@ref TypeDistorter): @enumerateEnum{LAYER_S_PARAM_DISTORT} \n
	///																For transform layers (@ref TypeTransform): @enumerateEnum{LAYER_S_PARAM_TRANS} \n.
	/// @param[out] d									Assigned the parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter could be retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(Int32 id, GeData& d);

	//----------------------------------------------------------------------------------------
	/// Sets layer parameters.
	/// @param[in] id									The ID of the parameter:\n
	///																For all layers: @enumerateEnum{LAYER_S_PARAM_ALL} \n
	///																For folders (@ref TypeFolder): @enumerateEnum{LAYER_S_PARAM_FOLDER} \n
	///																For shaders (@ref TypeShader): @enumerateEnum{LAYER_S_PARAM_SHADER} \n
	///																For brightness/contrast layers (@ref TypeBrightnessContrast): @enumerateEnum{LAYER_S_PARAM_BC} \n
	///																For HSL layers (@ref TypeHSL): @enumerateEnum{LAYER_S_PARAM_HSL} \n
	///																For posterize layers (@ref TypePosterize): @enumerateEnum{LAYER_S_PARAM_POSTER} \n
	///																For colorize layers (@ref TypeColorize): @enumerateEnum{LAYER_S_PARAM_COLORIZE} \n
	///																For clamp layers (@ref TypeClamp): @enumerateEnum{LAYER_S_PARAM_CLAMP} \n
	///																For clip layers (@ref TypeClip): @enumerateEnum{LAYER_S_PARAM_CLIP} \n
	///																For distorter layers (@ref TypeDistorter): @enumerateEnum{LAYER_S_PARAM_DISTORT} \n
	///																For transform layers (@ref TypeTransform): @enumerateEnum{LAYER_S_PARAM_TRANS} \n.
	/// @param[in] d									The parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter could be set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(Int32 id, const GeData& d);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void GetParameter(Int32 lItem, const DescID& id, GeData& t_data, Int32& flags) { }

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual void SetParameter(Int32 lItem, const DescID& id, const GeData& t_data, Int32& flags) { }
};

#pragma pack (pop)
}

#endif	// C4D_LAYERSHADERLAYER_H__
