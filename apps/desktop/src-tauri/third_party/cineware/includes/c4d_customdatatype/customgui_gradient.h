/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_GRADIENT_H__
#define CUSTOMGUI_GRADIENT_H__

#include "c4d_customdatatype.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

class AliasTrans;
class HyperFile;

/// Gradient data type ID.
#define CUSTOMDATATYPE_GRADIENT	1000481

///////////////////////////////////////////////////////////////////////////////////////////////////

/// @addtogroup GRADIENTPROPERTY_CUSTOMGUISETTINGS
/// @ingroup CUSTOMGUISETTINGS
/// @{
#define GRADIENTPROPERTY_ALPHA_WITH_COLOR		10000			///< Bool If @ref GRADIENTPROPERTY_ALPHA is set to true, this creates a Gradient with Color and Alpha, and shows controls to handle both gradients.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { COLOR; }
																											/// @endcode
#define GRADIENTPROPERTY_ALPHA							10001			///< Bool Creates an Alpha gradient.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { ALPHA; }
																											/// @endcode
#define GRADIENTPROPERTY_NOEDITCOLOR				10002			///< Bool No color editing.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOEDITCOLOR; }
																											/// @endcode
#define GRADIENTPROPERTY_NOPRESETS					10003			///< Bool No load/save preset buttons.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOPRESETS; }
																											/// @endcode
#define GRADIENTPROPERTY_NOINTERPOLATION		10004			///< Bool No Interpolation parameter.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOINTERPOLATION; }
																											/// @endcode
#define GRADIENTPROPERTY_NOKNOTPOSITION			10005			///< Bool No Knot Position parameter.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOKNOTPOSITION; }
																											/// @endcode
#define GRADIENTPROPERTY_NOBIASPOSITION			10006			///< Bool No Bias Position parameter.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOBIASPOSITION; }
																											/// @endcode
#define GRADIENTPROPERTY_NOBRIGHTNESS				10007			///< Bool No Brightness parameter.
																											///< @note The property is used like this in a @em res file:
																											/// @code
																											/// GRADIENT MYGRADIENT { NOBRIGHTNESS; }
																											/// @endcode
/// @}

/// @addtogroup GRADIENT_INTERPOLATION_TYPE
/// @ingroup group_enumeration
/// @{
/// Gradient interpolation types.
enum GRADIENT_INTERPOLATION_TYPE
{
	CUBICKNOT		= 0,		///< Cubic knot.
	SMOOTHKNOT	= 2,		///< Smooth knot.
	LINEARKNOT	= 3,		///< Linear knot.
	NONE				= 5,		///< Step.
	EXP_UP			= 6,		///< Exponential up (legacy).
	EXP_DOWN		= 7, 		///< Exponential down (legacy).
	BLEND				= 8			///< Blend.
} ENUM_END_LIST(GRADIENT_INTERPOLATION_TYPE);
/// @}

/// @addtogroup GRADIENTMODE
/// @ingroup group_enumeration
/// @{
#define GRADIENTMODE_COLOR			0	///< Color only.
#define GRADIENTMODE_ALPHA			1	///< Alpha only.
#define GRADIENTMODE_COLORALPHA	2	///< Color and alpha.
/// @}

/// @addtogroup GRADIENT_CONTAINERIDS
/// @ingroup group_containerid
/// @{
#define GRADIENT_KNOT						1001			///< BaseContainer Holds the gradient knots as sub-containers: @enumerateEnum{GRADIENTKNOT_CONTAINERIDS}
#define GRADIENT_MODE						1002			///< Int32 Gradient mode: @enumerateEnum{GRADIENTMODE}
#define GRADIENT_UNCLAMPED			1003			///< Bool Unclamped.
/// @}

/// @addtogroup GRADIENTKNOT_CONTAINERIDS
/// @ingroup group_containerid
/// @{
#define GRADIENTKNOT_COLOR					1000			///< Vector Color.
#define GRADIENTKNOT_BRIGHTNESS			1001			///< Float Brightness.
#define GRADIENTKNOT_POSITION				1002			///< Float Position.
#define GRADIENTKNOT_BIAS						1003			///< Float Bias.
#define GRADIENTKNOT_ID							1004			///< Int32 Index.
#define GRADIENTKNOT_INTERPOLATION	1005			///< Int32 Interpolation.
/// @}


//----------------------------------------------------------------------------------------
/// Represents a knot in a gradient.
//----------------------------------------------------------------------------------------
struct GradientKnot
{
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	GradientKnot()
	{
		col = Vector(1.0);
		brightness = 1.0;
		pos		= 0.0;
		bias	= 0.5;
		index = 0;
		interpolation = SMOOTHKNOT;
	}

	Vector col;						///< Color.
	Float	 brightness;		///< Brightness.
	Float	 pos;						///< Position.
	Float	 bias;					///< Bias.
	Int32	 index;					///< Index.
	Int32	 interpolation;	///< Interpolation
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/// Gradient data type (@ref CUSTOMDATATYPE_GRADIENT).
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class Gradient : public CustomDataType
{
public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{gradient data}
	/// @return												@allocReturn{gradient data}
	//----------------------------------------------------------------------------------------
	static Gradient* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{gradient data}
	/// @param[in,out] grad						@theToDestruct{gradient data}
	//----------------------------------------------------------------------------------------
	static void Free(Gradient*& grad);

	/// @}

	/// @name Knots
	/// @{

	//----------------------------------------------------------------------------------------
	/// Inverts the knots.
	/// @return												@trueIfOtherwiseFalse{knots were successfully inverted}
	//----------------------------------------------------------------------------------------
	Bool InvertKnots();

	//----------------------------------------------------------------------------------------
	/// Doubles the knots.
	/// @return												@trueIfOtherwiseFalse{knots were successfully doubled}
	//----------------------------------------------------------------------------------------
	Bool DoubleKnots();

	//----------------------------------------------------------------------------------------
	/// Flushes all knots.
	//----------------------------------------------------------------------------------------
	void FlushKnots();

	/// @}

	/// @name Knot
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of knots.
	/// @return												The knot count.
	//----------------------------------------------------------------------------------------
	Int32 GetKnotCount();

	//----------------------------------------------------------------------------------------
	/// Inserts a knot.
	/// @param[in] knot								The knot to insert.
	/// @return												The index of the inserted knot. GetKnotCount() - @em 1
	//----------------------------------------------------------------------------------------
	Int32 InsertKnot(const GradientKnot& knot);

	//----------------------------------------------------------------------------------------
	/// Removes a knot.
	/// @param[in] index							The knot index. @em 0 <= @formatParam{index} < GetKnotCount()
	/// @return												@trueIfOtherwiseFalse{knot was successfully removed}
	//----------------------------------------------------------------------------------------
	Bool RemoveKnot(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Gets the knot at @formatParam{index}.
	/// @param[in] index							The knot index. @em 0 <= @formatParam{index} < GetKnotCount()
	/// @return												The knot data.
	//----------------------------------------------------------------------------------------
	GradientKnot GetKnot(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Sets the data for a knot at @formatParam{index}.
	/// @param[in] index							The knot index. @em 0 <= @formatParam{index} < GetKnotCount()
	/// @param[in] k									The knot information.
	//----------------------------------------------------------------------------------------
	void SetKnot(Int32 index, const GradientKnot& k);

	/// @}

	/// @name Render
	/// @{

	//----------------------------------------------------------------------------------------
	/// Initializes the gradient data for renderer.
	/// @return												@trueIfOtherwiseFalse{the gradient data was successfully initialized for renderer}
	//----------------------------------------------------------------------------------------
	Bool InitRender();

	//-------------------------------------------------------------------------------------------------
	/// Frees the gradient data from render.
	/// @note Call after gradient calculation is finished.
	//-------------------------------------------------------------------------------------------------
	void FreeRender();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the knot at @formatParam{index} between InitRender() and FreeRender().
	/// @param[in] index							The knot index. @em 0 <= @formatParam{index} < GetKnotCount()
	/// @return												The knot at @formatParam{index}. @theOwnsPointed{gradient,knot}
	//----------------------------------------------------------------------------------------
	GradientKnot* GetRenderKnot(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Gets the number of knots for GetRenderKnot().
	/// @return												The render knot count.
	//----------------------------------------------------------------------------------------
	Int32 GetRenderKnotCount();

	//----------------------------------------------------------------------------------------
	/// Calculates a gradient pixel.
	/// @param[in] pos								The X position.
	/// @return												The calculated gradient pixel.
	//----------------------------------------------------------------------------------------
	Vector CalcGradientPixel(Float pos);

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets data from the gradient container.
	/// @param[in] id									The gradient data ID: @ref GRADIENT_CONTAINERIDS.
	/// @return												The data.
	//----------------------------------------------------------------------------------------
	GeData GetData(Int32 id) const;

	//----------------------------------------------------------------------------------------
	/// Sets data in the gradient container.
	/// @param[in] id									The gradient data ID: @ref GRADIENT_CONTAINERIDS.
	/// @param[in] data								The data.
	/// @return												@trueIfOtherwiseFalse{the gradient data was successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetData(Int32 id, const GeData& data);

	/// @}

	/// @name Alpha
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the alpha gradient.
	/// @return												Alpha gradient. @theOwnsPointed{gradient,knot}
	//----------------------------------------------------------------------------------------
	Gradient* GetAlphaGradient();

	//----------------------------------------------------------------------------------------
	/// Allocates an alpha gradient.
	/// @return												The allocated alpha gradient, or @formatConstant{nullptr} if the allocation failed.
	//----------------------------------------------------------------------------------------
	Gradient* AllocAlphaGradient();

	//----------------------------------------------------------------------------------------
	/// Converts this gradient to an alpha gradient.
	//----------------------------------------------------------------------------------------
	void ConvertToAlphaGradient();

	//----------------------------------------------------------------------------------------
	/// Copies all gradient data from the source gradient.
	/// @since R19
	/// @param[in] src								The source gradient.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool CopyFrom(const Gradient* src);

	//----------------------------------------------------------------------------------------
	/// Converts a gradient within a container into an alpha gradient.
	/// @param[in] bc									The container holding the gradient. @callerOwnsPointed{container}
	/// @param[in] id									The ID of the gradient within the container.
	//----------------------------------------------------------------------------------------
	static void ConvertToAlphaGradient(BaseContainer* bc, Int32 id);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a checksum of the whole gradient state, including the needles, colors and interpolation type.
	/// @return												The checksum.
	//----------------------------------------------------------------------------------------
	UInt32 GetChecksum();

	/// @}

private:
	Gradient();
	~Gradient();
};

/// @markInternal
class GradientDataTypeClass
{
public:
	static Int32 GetId();

	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);

	static Bool GetParameter(const CustomDataType* data, const DescID& id, GeData& t_data, Int32& flags);
	static Bool SetDParameter(CustomDataType* data, const DescID& id, const GeData& t_data, Int32& flags);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_GRADIENT_H__
