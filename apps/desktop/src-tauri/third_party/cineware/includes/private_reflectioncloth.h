/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef REFLECTIONCLOTH_H__
#define REFLECTIONCLOTH_H__

#include "c4d_file.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class XRClothWeavePattern;

// Data structure describing the properties of a single yarn
class XRClothYarn
{
public:
	Float	m_fTwist;			// psi
	Float	m_fMaxAngle;	// umax
	Float	m_fCurvature;	// kappa

	Float	m_fWidth;
	Float	m_fLength;
	Float	m_fWidthHalf;
	Float	m_fLengthHalf;
	Float	m_fWidthInv;
	Float	m_fLengthInv;

	Float	m_fCenterU;
	Float	m_fCenterV;

	Int32	m_nIndex;

public:
	XRClothYarn(Float fTwist, Float fMaxAngle, Float fCurvature, Float fWidth, Float fLength, Float fCenterU, Float fCenterV, Int32 nIndex);
	virtual ~XRClothYarn() { }
#if 0
	virtual void GetUV(const XRClothWeavePattern* pWeave, const Vector& vCenter, const Vector& vXY, Vector* pUV, Float* pUMaxMod, Float* pNoise, Float fQuality, Bool bFirst) const = 0;
	virtual Float	EvalIntegrand(const XRClothWeavePattern* pWeave, Vector& vIncident, Vector& vReflected, const Vector& vUV, Float fUMaxMod, Float fHighlights) const = 0;

	Float	EvalFilamentIntegrand(const XRClothWeavePattern* pWeave, const Vector& vIncident, const Vector& vReflected, Float fU, Float fV, Float fUMaxMod, Float fHighlights) const;
	Float	EvalStapleIntegrand(const XRClothWeavePattern* pWeave, const Vector& vIncident, const Vector& vReflected, Float fU, Float fV, Float fUMaxMod, Float fHighlights) const;

	Float	ComputeVonMises(Float fCosX, Float fB) const;
	Float	ComputeSeeliger(Float fCos1, Float fCos2, Float fA, Float fS) const;
	Float	RadiusOfCurvature(Float fU, Float fUmaxMod) const;
#endif
};

class XRClothWarp : public XRClothYarn
{
public:
	XRClothWarp(Float fTwist, Float fMaxAngle, Float fCurvature, Float fWidth, Float fLength, Float fCenterU, Float fCenterV, Int32 nIndex);
#if 0
	virtual void GetUV(const XRClothWeavePattern* pWeave, const Vector& vCenter, const Vector& vXY, Vector* pUV, Float* pUMaxMod, Float* pNoise, Float fQuality, Bool bFirst) const;
	virtual Float	EvalIntegrand(const XRClothWeavePattern* pWeave, Vector& vIncident, Vector& vReflected, const Vector& vUV, Float fUMaxMod, Float fHighlights) const;
#endif
};

class XRClothWeft : public XRClothYarn
{
public:
	XRClothWeft(Float fTwist, Float fMaxAngle, Float fCurvature, Float fWidth, Float fLength, Float fCenterU, Float fCenterV, Int32 nIndex);
#if 0
	virtual void GetUV(const XRClothWeavePattern* pWeave, const Vector& vCenter, const Vector& vXY, Vector* pUV, Float* pUMaxMod, Float* pNoise, Float fQuality, Bool bFirst) const;
	virtual Float	EvalIntegrand(const XRClothWeavePattern* pWeave, Vector& vIncident, Vector& vReflected, const Vector& vUV, Float fUMaxMod, Float fHighlights) const;
#endif
};

typedef BaseArray<Int32>				XRClothPattern;
typedef BaseArray<XRClothYarn*>	XRClothYarns;

class XRClothWeavePattern
{
public:
	// Intensity Adjustment
	Float	m_fSpecularRatio;

	// Scale/Repeat
	Float	m_fGlobalU;
	Float	m_fGlobalV;

	// Scattering parameter (uniform and forward)
	Float	m_fAlpha;
	Float	m_fBeta;

	// Filament smoothing
	Float	m_fSmoothing;

	// Combined area taken up by the warp & weft
	Float	m_fWarpArea;
	Float	m_fWeftArea;

	// Size of the weave pattern
	Int32	m_nTileWidth;
	Int32	m_nTileHeight;

	// Noise-related parameters
	Float	m_fNoiseIntensity;
	Float	m_fNoiseScale;
	Float	m_fNoiseShapeWarp;
	Float	m_fNoiseShapeWeft;
	Float	m_fNoiseShapeScale;

	// Sampling
	Int32	m_nSamples;

	// Detailed weave pattern
	XRClothPattern m_Pattern;

	// List of all yarns referenced in pattern
	XRClothYarns m_Yarns;

public:
	XRClothWeavePattern();
	~XRClothWeavePattern();
#if 0
	void SetupPattern(Int32 nPattern);

	void Setup(Int32 nQuality, Float fAlpha, Float fBeta, Float fSmoothing);
	void SetupNoise(Float fIntensity, Float fScale, Float fShapeWarp, Float fShapeWeft, Float fShapeScale);

	void ComputeSpecularRatio();

	XRClothYarn*							GetYarn(Float fU, Float fV, Vector* pUV, Float* pUMax, Float* pScale, Float* pNoise, Float fQuality, Bool bFirst);
#endif
};

#pragma pack (pop)
}

#endif	// REFLECTIONCLOTH_H__
