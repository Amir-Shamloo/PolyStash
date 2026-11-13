/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________ *
*  Copyright 2020-2025 Adobe
*  All Rights Reserved.
* * NOTICE:  All information contained herein is, and remains
* the property of Adobe and its suppliers, if any. The intellectual
* and technical concepts contained herein are proprietary to Adobe
* and its suppliers and are protected by all applicable intellectual
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe.
**************************************************************************/

#ifndef _SUBSTANCE_AIR_FRAMEWORK_CHANNEL_H
#define _SUBSTANCE_AIR_FRAMEWORK_CHANNEL_H

#include "typedefs.h"

namespace SubstanceAir
{

//! @brief The list of channels roles available in Designer
//!        ALIAS declarations MUST be located below ITEM declarations
//!        The coatOpacity/coatWeight and the specularColor/specularEdgeColor
//!        special cases are ok because they alias to the value right above
//!        so there's no enum restart. Padding enum values are introduced to
//!        keep the legacy values
#ifndef AIR_PROCESS_CHANNEL_LIST
#define AIR_PROCESS_CHANNEL_LIST \
	AIR_PROCESS_CHANNEL_ITEM(diffuse,Diffuse) \
	AIR_PROCESS_CHANNEL_ITEM(baseColor,BaseColor) \
	AIR_PROCESS_CHANNEL_ITEM(opacity,Opacity) \
	AIR_PROCESS_CHANNEL_ITEM(emissive,Emissive) \
	AIR_PROCESS_CHANNEL_ITEM(emissiveIntensity,EmissiveIntensity) \
	AIR_PROCESS_CHANNEL_ITEM(ambient,Ambient) \
	AIR_PROCESS_CHANNEL_ITEM(ambientOcclusion,AmbientOcclusion) \
	AIR_PROCESS_CHANNEL_ITEM(mask,Mask) \
	AIR_PROCESS_CHANNEL_ITEM(normal,Normal) \
	AIR_PROCESS_CHANNEL_ITEM(bump,Bump) \
	AIR_PROCESS_CHANNEL_ITEM(height,Height) \
	AIR_PROCESS_CHANNEL_ITEM(heightScale,HeightScale) \
	AIR_PROCESS_CHANNEL_ITEM(heightLevel,HeightLevel) \
	AIR_PROCESS_CHANNEL_ITEM(displacement,Displacement) \
	AIR_PROCESS_CHANNEL_ITEM(specular,Specular) \
	AIR_PROCESS_CHANNEL_ITEM(specularLevel,SpecularLevel) \
	AIR_PROCESS_CHANNEL_ITEM(specularColor,SpecularColor) \
	AIR_PROCESS_CHANNEL_ALIAS(specularEdgeColor,SpecularEdgeColor,SpecularColor) \
	AIR_PROCESS_CHANNEL_ITEM(padding_donotuse,Padding_DoNotUse1) \
	AIR_PROCESS_CHANNEL_ITEM(glossiness,Glossiness) \
	AIR_PROCESS_CHANNEL_ITEM(roughness,Roughness) \
	AIR_PROCESS_CHANNEL_ITEM(anisotropyLevel,AnisotropyLevel) \
	AIR_PROCESS_CHANNEL_ITEM(anisotropyAngle,AnisotropyAngle) \
	AIR_PROCESS_CHANNEL_ITEM(transmissive,Transmissive) \
	AIR_PROCESS_CHANNEL_ITEM(reflection,Reflection) \
	AIR_PROCESS_CHANNEL_ITEM(refraction,Refraction) \
	AIR_PROCESS_CHANNEL_ITEM(coatWeight,CoatWeight) \
	AIR_PROCESS_CHANNEL_ALIAS(coatOpacity,CoatOpacity,CoatWeight) \
	AIR_PROCESS_CHANNEL_ITEM(padding_donotuse,Padding_DoNotUse2) \
	AIR_PROCESS_CHANNEL_ITEM(coatColor,CoatColor) \
	AIR_PROCESS_CHANNEL_ITEM(coatRoughness,CoatRoughness) \
	AIR_PROCESS_CHANNEL_ITEM(coatIOR,CoatIOR) \
	AIR_PROCESS_CHANNEL_ITEM(coatSpecularLevel,CoatSpecularLevel) \
	AIR_PROCESS_CHANNEL_ITEM(coatNormal,CoatNormal) \
	AIR_PROCESS_CHANNEL_ITEM(environment,Environment) \
	AIR_PROCESS_CHANNEL_ITEM(IOR,IOR) \
	AIR_PROCESS_CHANNEL_ITEM(dispersion,Dispersion) \
	AIR_PROCESS_CHANNEL_ITEM(translucency,Translucency) \
	AIR_PROCESS_CHANNEL_ITEM(absorptionColor,AbsorptionColor) \
	AIR_PROCESS_CHANNEL_ITEM(absorptionDistance,AbsorptionDistance) \
	AIR_PROCESS_CHANNEL_ITEM(scattering,Scattering) \
	AIR_PROCESS_CHANNEL_ITEM(scatteringColor,ScatteringColor) \
	AIR_PROCESS_CHANNEL_ITEM(scatteringDistance,ScatteringDistance) \
	AIR_PROCESS_CHANNEL_ITEM(scatteringDistanceScale,ScatteringDistanceScale) \
	AIR_PROCESS_CHANNEL_ITEM(scatteringRedShift,ScatteringRedShift) \
	AIR_PROCESS_CHANNEL_ITEM(scatteringRayleigh,ScatteringRayleigh) \
	AIR_PROCESS_CHANNEL_ITEM(scattering0,SCATTERING0) \
	AIR_PROCESS_CHANNEL_ITEM(scattering1,SCATTERING1) \
	AIR_PROCESS_CHANNEL_ITEM(scattering2,SCATTERING2) \
	AIR_PROCESS_CHANNEL_ITEM(scattering3,SCATTERING3) \
	AIR_PROCESS_CHANNEL_ITEM(any,Any) \
	AIR_PROCESS_CHANNEL_ITEM(metallic,Metallic) \
	AIR_PROCESS_CHANNEL_ITEM(sheenOpacity,SheenOpacity) \
	AIR_PROCESS_CHANNEL_ITEM(sheenColor,SheenColor) \
	AIR_PROCESS_CHANNEL_ITEM(sheenRoughness,SheenRoughness) \
	AIR_PROCESS_CHANNEL_ITEM(panorama,Panorama) \
	AIR_PROCESS_CHANNEL_ITEM(physicalSize,PhysicalSize) \
	AIR_PROCESS_CHANNEL_ITEM(baseWeight,BaseWeight) \
	AIR_PROCESS_CHANNEL_ITEM(baseDiffuseRoughness,BaseDiffuseRoughness) \
	AIR_PROCESS_CHANNEL_ITEM(specularWeight,SpecularWeight) \
	AIR_PROCESS_CHANNEL_ITEM(transmissionDispersionAbbeNumber,TransmissionDispersionAbbeNumber) \
	AIR_PROCESS_CHANNEL_ITEM(transmissionDispersionScale,TransmissionDispersionScale) \
	AIR_PROCESS_CHANNEL_ITEM(transmissionScatter,TransmissionScatter) \
	AIR_PROCESS_CHANNEL_ITEM(transmissionScatterAnisotropy,TransmissionScatterAnisotropy) \
	AIR_PROCESS_CHANNEL_ITEM(transmissionWeight,TransmissionWeight) \
	AIR_PROCESS_CHANNEL_ITEM(subsurfaceScatterAnisotropy,SubsurfaceScatterAnisotropy) \
	AIR_PROCESS_CHANNEL_ITEM(coatRoughnessAnisotropy,CoatRoughnessAnisotropy) \
	AIR_PROCESS_CHANNEL_ITEM(coatDarkening,CoatDarkening) \
	AIR_PROCESS_CHANNEL_ITEM(coatTangent,CoatTangent) \
	AIR_PROCESS_CHANNEL_ITEM(emissionWeight,EmissionWeight) \
	AIR_PROCESS_CHANNEL_ITEM(emissionLuminance,EmissionLuminance) \
	AIR_PROCESS_CHANNEL_ITEM(thinFilmWeight,ThinFilmWeight) \
	AIR_PROCESS_CHANNEL_ITEM(thinFilmThickness,ThinFilmThickness) \
	AIR_PROCESS_CHANNEL_ITEM(thinFilmIOR,ThinFilmIOR) \
	AIR_PROCESS_CHANNEL_ITEM(thinWalled,ThinWalled) \
	AIR_PROCESS_CHANNEL_ITEM(tangent,Tangent) \
	AIR_PROCESS_CHANNEL_ITEM(UNKNOWN,UNKNOWN) \
	AIR_PROCESS_CHANNEL_ITEM(internalCount_donotuse,INTERNAL_COUNT) \
	/* Keep ITEM declarations above ALIAS declarations */ \
	AIR_PROCESS_CHANNEL_ALIAS(metalness,Metalness,Metallic) \
	AIR_PROCESS_CHANNEL_ALIAS(specularRoughness,SpecularRoughness,Roughness) \
	AIR_PROCESS_CHANNEL_ALIAS(specularRoughnessAnisotropy,SpecularRoughnessAnisotropy,AnisotropyLevel) \
	AIR_PROCESS_CHANNEL_ALIAS(specularIOR,SpecularIOR,IOR) \
	AIR_PROCESS_CHANNEL_ALIAS(transmissionColor,TransmissionColor,AbsorptionColor) \
	AIR_PROCESS_CHANNEL_ALIAS(transmissionDepth,TransmissionDepth,AbsorptionDistance) \
	AIR_PROCESS_CHANNEL_ALIAS(subsurfaceWeight,SubsurfaceWeight,Translucency) \
	AIR_PROCESS_CHANNEL_ALIAS(subsurfaceColor,SubsurfaceColor,ScatteringColor) \
	AIR_PROCESS_CHANNEL_ALIAS(subsurfaceRadius,SubsurfaceRadius,ScatteringDistance) \
	AIR_PROCESS_CHANNEL_ALIAS(subsurfaceRadiusScale,SubsurfaceRadiusScale,ScatteringDistanceScale) \
	AIR_PROCESS_CHANNEL_ALIAS(fuzzWeight,FuzzWeight,SheenOpacity) \
	AIR_PROCESS_CHANNEL_ALIAS(fuzzColor,FuzzColor,SheenColor) \
	AIR_PROCESS_CHANNEL_ALIAS(fuzzRoughness,FuzzRoughness,SheenRoughness) \
	AIR_PROCESS_CHANNEL_ALIAS(emissionColor,EmissionColor,Emissive)
#endif

#define AIR_PROCESS_CHANNEL_ITEM(channelString,channelEnum) Channel_ ## channelEnum,
#define AIR_PROCESS_CHANNEL_ALIAS(channelString,channelEnumA,channelEnumB) Channel_ ## channelEnumA = Channel_ ## channelEnumB,
enum ChannelUse
{
	AIR_PROCESS_CHANNEL_LIST
};
#undef AIR_PROCESS_CHANNEL_ITEM
#undef AIR_PROCESS_CHANNEL_ALIAS

//! @brief List of components to match with a particular channel usage
enum ChannelComponent
{
	ChannelComponent_RGBA,
	ChannelComponent_RGB,
	ChannelComponent_R,
	ChannelComponent_G,
	ChannelComponent_B,
	ChannelComponent_A,
	ChannelComponent_INTERNAL_COUNT
};

struct ChannelMapping {
	const char* channelName;
	ChannelUse channelEnum;
};

//! @brief Enumeration of color space interpretation
//! Used as argument of each external resources output
enum ColorSpace
{
	//! @brief Interpreted as Linear color space image
	//! @note Equivalent to Passthruinterpreted as-is.
	ColorSpace_Linear,

	//! @brief Interpreted as sRGB color space image
	//! @warning L8 format stored as sRGB8, sL8 is not supported.
	ColorSpace_sRGB,

	//! @brief Interpreted as raw values, NOT color/luminance space.
	//! Passthru of normalized/float values.
	ColorSpace_Passthru,

	//! @brief Interpreted as signed normalized raw values.
	//! Signed -1..1 values stored as 0..1 normalized values.
	ColorSpace_SNorm,

	//! @brief Interpreted as XYZ Direct normal stored in RGB channel.
	//! Right handed syscoord: OpenGL
	ColorSpace_NormalXYZRight,

	//! @brief Interpreted as XYZ Indirect normal stored in RGB channel.
	//! Left handed syscoord: Direct3D
	ColorSpace_NormalXYZLeft,

	//! @brief Used when the color space cannot be parsed
	ColorSpace_UNKNOWN,

	ColorSpace_COUNT,
	ColorSpace_INTERNAL_COUNT = ColorSpace_COUNT
};


//! @brief Complete Channel usage description
//! Accessible for inputs and outputs
struct ChannelFullDesc
{
	//! @brief Describes which particular components hold the information from this channel usage
	ChannelComponent mComponent;

	//! @brief Channel usage in ChannelUse form
	ChannelUse mUsage;

	//! @brief Channel usage in string form
	//! To be used when mUsage is Channel_UNKNOWN
	string mUsageStr;

	//! @brief Colorspace in which the data should be interpreted, in ColorSpace form
	ColorSpace mColorSpace;

	//! @brief Colorspace in which the data should be interpreted, in string form
	//! To be used then mColorSpace is ColorSpace_UNKNOWN
	string mColorSpaceStr;
};


//! @brief Helper: get channel names
const char* const* getChannelNames();

//! @brief Helper: get a pointer to the table of channel mappings
const ChannelMapping* getChannelMappingTable();

//! @brief Helper: convert a channel name to a ChannelUse enum value
//! @return Returns the matching ChannelUse value, or Channel_UNKNOWN
//!         if the provided name does not match any known channel name
ChannelUse getChannelUseFromChannelName(const char* channelName);

//! @brief Helper: convert a channel name to the canonical name for this channel
//! @return Returns the canonical name for a given channel name, or "UNKNOWN" if
//!         the provided name does not match any known channel name
//!         This can be used to de-ambiguate channel aliases
const char* getCanonicalChannelNameFromChannelName(const char* channelName);

//! @brief Helper: convert a ChannelUse enum value to a channel name
//! @return Returns the matching channel name, or "UNKNOWN" if the
//!         provided enum value does not match any known channel usage
const char* getChannelNameFromChannelUse(ChannelUse channelUse);

//! @brief Helper: get colorspace names corresponding to ColorSpace
//! @return Return array of ColorSpace_INTERNAL_COUNT strings
const char* const* getColorSpaceNames();


//! @brief Choose color space automatically for the given channel and format.
//! @param channel the ChannelUse enum
//! @param isFloatFormat must be false for 8 or 16 bits integer formats, true otherwise.
//!
//!  * Diffuse, Emissive, Specular
//!       -> Integer formats : sRGB
//!       -> Float formats   : Linear
//!  * Height, Displacement
//!       -> Integer formats : SNorm
//!       -> Float formats   : Passthru
//!  * Normal
//!       -> All formats     : NormalXYZLeft
//!  * Others
//!       -> All formats     : Passthru
ColorSpace getDefaultColorSpace(ChannelUse channel, bool isFloatFormat);

} // namespace SubstanceAir

#endif //_SUBSTANCE_AIR_FRAMEWORK_CHANNEL_H
