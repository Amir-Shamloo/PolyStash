/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TOOLS_H__
#define C4D_TOOLS_H__

#include "ge_math.h"
#include "c4d_system.h"
#include "c4d_parameter_ids.h"

namespace cineware
{
#pragma pack (push, 8)

class Filename;

#define COLORTOINT_MULTIPLIER 255.99								///< Constant to convert from vectors color components to integers.
#define PERCENT								100.00								///< Constant to convert from <i>0</i>-<i>1</i> range to percentage.
#define THIRD									0.333333333333333333	///< Constant to calculate the third of a value.
#define SIXTH									0.166666666666666666	///< Constant to calculate the sixth of a value.

#define MAXRANGE		1.0e20													///< Maximum value for metric data.
#define MAXELEMENTS 100000000												///< Maximum number of points.
#define MIN_EPSILON 0.001														///< Minimum epsilon value.

/// @addtogroup c4d_tools_mathproceduralfunctions Math/Procedural Functions
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Returns @em 1.0 if @formatParam{x} is greater than or equal to @formatParam{a}, else @em 0.0.
/// @param[in] a									A single-precision floating point value.
/// @param[in] x									A single-precision floating point value.
/// @return												The step value (@em 0.0 or @em 1.0).
//----------------------------------------------------------------------------------------
inline Float32 StepEx(Float32 a, Float32 x)
{
	if (x >= a)
		return (Float32) 1.0;
	else
		return (Float32) 0.0;
}

//----------------------------------------------------------------------------------------
/// Returns @em 1.0 if @formatParam{x} is greater than or equal to @formatParam{a}, else @em 0.0.
/// @param[in] a									A double-precision floating point value.
/// @param[in] x									A double-precision floating point value.
/// @return												The step value (@em 0.0 or @em 1.0).
//----------------------------------------------------------------------------------------
inline Float64 StepEx(Float64 a, Float64 x)
{
	if (x >= a)
		return 1.0;
	else
		return 0.0;
}

//----------------------------------------------------------------------------------------
/// Returns @em 0.0 if @formatParam{x} is less than @formatParam{a} and @em 1.0 if @formatParam{x} is greater than @formatParam{b}, else returns @formatParam{x} mapped on the range [@formatParam{a},@formatParam{b}].
/// @param[in] a									A single-precision floating point value.
/// @param[in] b									A single-precision floating point value.
/// @param[in] x									A single-precision floating point value.
/// @return												The box stepped value between @em 0.0 and @em 1.0.
//----------------------------------------------------------------------------------------
inline Float32 Boxstep(Float32 a, Float32 b, Float32 x)
{
	if (b == a)
	{
		if (x < a)
			return (Float32) 0.0;
		else
			return (Float32) 1.0;
	}
	x = (x - a) / (b - a);
	return x < (Float32) 0.0 ? (Float32) 0.0 : (x > (Float32) 1.0 ? (Float32) 1.0 : x);
}

//----------------------------------------------------------------------------------------
/// Returns @em 0.0 if @formatParam{x} is less than @formatParam{a} and @em 1.0 if @formatParam{x} is greater than @formatParam{b}, else returns @formatParam{x} mapped on the range [@formatParam{a},@formatParam{b}].
/// @param[in] a									A double-precision floating point value.
/// @param[in] b									A double-precision floating point value.
/// @param[in] x									A double-precision floating point value.
/// @return												The box stepped value between @em 0.0 and @em 1.0.
//----------------------------------------------------------------------------------------
inline Float64 Boxstep(Float64 a, Float64 b, Float64 x)
{
	if (b == a)
	{
		if (x < a)
			return 0.0;
		else
			return 1.0;
	}
	x = (x - a) / (b - a);
	return x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 : x);
}

//----------------------------------------------------------------------------------------
/// Returns @em 0.0 if @formatParam{x} is less than @formatParam{a} and @em 1.0 if @formatParam{x} is greater than @formatParam{b}, else returns @formatParam{x} mapped on the range [@formatParam{a},@formatParam{b}].
/// @note The mapping is smoothed using an ease-in/ease-out curve.
/// @param[in] a									A single-precision floating point value.
/// @param[in] b									A single-precision floating point value.
/// @param[in] x									A single-precision floating point value.
/// @return												The smoothed stepped value between @em 0.0 and @em 1.0.
//----------------------------------------------------------------------------------------
inline Float32 Smoothstep(Float32 a, Float32 b, Float32 x)
{
	if (x < a)
		return (Float32) 0.0;
	if (x >= b)
		return (Float32) 1.0;

	x = (x - a) / (b - a);	// normalize to [0,1]

	return x * x * ((Float32) 3.0 - (Float32) 2.0 * x);
}

//----------------------------------------------------------------------------------------
/// Returns @em 0.0 if @formatParam{x} is less than @formatParam{a} and @em 1.0 if @formatParam{x} is greater than @formatParam{b}, else returns @formatParam{x} mapped on the range [@formatParam{a},@formatParam{b}].
/// @note The mapping is smoothed using an ease-in/ease-out curve.
/// @param[in] a									A double-precision floating point value.
/// @param[in] b									A double-precision floating point value.
/// @param[in] x									A double-precision floating point value.
/// @return												The smoothed stepped value between @em 0.0 and @em 1.0.
//----------------------------------------------------------------------------------------
inline Float64 Smoothstep(Float64 a, Float64 b, Float64 x)
{
	if (x < a)
		return 0.0;
	if (x >= b)
		return 1.0;

	x = (x - a) / (b - a);	// normalize to [0,1]

	return x * x * (3.0 - 2.0 * x);
}

//----------------------------------------------------------------------------------------
/// Returns @formatParam{a} modulo @formatParam{b}.
/// @param[in] a									A single-precision floating point value.
/// @param[in] b									A single-precision floating point value.
/// @return												The modulo value.
//----------------------------------------------------------------------------------------
inline Float32 Modulo(Float32 a, Float32 b)
{
	if (b == 0.0)
		return 0.0;
	Int32 n = (Int32) (a / b);

	a -= n * b;
	if (a < 0.0)
		a += b;

	return a;
}

//----------------------------------------------------------------------------------------
/// Returns @formatParam{a} modulo @formatParam{b}.
/// @param[in] a									A double-precision floating point value.
/// @param[in] b									A double-precision floating point value.
/// @return												The modulo value.
//----------------------------------------------------------------------------------------
inline Float64 Modulo(Float64 a, Float64 b)
{
	if (b == 0.0)
		return 0.0;
	Int32 n = (Int32) (a / b);

	a -= n * b;
	if (a < 0.0)
		a += b;

	return a;
}

//----------------------------------------------------------------------------------------
/// Returns @formatParam{a} modulo @formatParam{b} (integer modulo).
/// @param[in] a									A 32-bit integer value.
/// @param[in] b									A 32-bit integer value.
/// @return												The modulo value.
//----------------------------------------------------------------------------------------
inline Int32 LModulo(Int32 a, Int32 b)
{
	if (!b)
		return 0;
	if (a >= 0)
		return a % b;

	a -= (a / b) * b;
	if (a < 0)
		a += b;

	return a;
}

//----------------------------------------------------------------------------------------
/// Returns @formatParam{a} modulo @formatParam{b} (integer modulo).
/// @param[in] a									A 64-bit integer value.
/// @param[in] b									A 64-bit integer value.
/// @return												The modulo value.
//----------------------------------------------------------------------------------------
inline Int64 LModulo(Int64 a, Int64 b)
{
	if (!b)
		return 0;
	if (a >= 0)
		return a % b;

	a -= (a / b) * b;
	if (a < 0)
		a += b;

	return a;
}

//----------------------------------------------------------------------------------------
/// Returns the bias as the defined in the book @em "Texturing and Modeling" by Ebert.
/// @param[in] b									The bias value.
/// @param[in] x									A single-precision floating point value.
/// @return												The bias value.
//----------------------------------------------------------------------------------------
inline Float32 Bias(Float32 b, Float32 x)
{
	return Pow(x, Float32(-Ln(b) / (Float32) 0.693147180559945309));
}

//----------------------------------------------------------------------------------------
/// Returns the bias as the defined in the book @em "Texturing and Modeling" by Ebert.
/// @param[in] b									The bias value.
/// @param[in] x									A double-precision floating point value.
/// @return												The bias value.
//----------------------------------------------------------------------------------------
inline Float64 Bias(Float64 b, Float64 x)
{
	return Pow(x, Float64(-Ln(b) / 0.693147180559945309));
}

//----------------------------------------------------------------------------------------
/// Returns the next integer value towards zero.
/// @param[in] x									The 32-bit floating point value to truncate.
/// @return												The truncated value.
//----------------------------------------------------------------------------------------
inline Float32 Truncate(Float32 x)
{
	if (x >= (Float32) 0.0)
		return Floor(x);
	else
		return Ceil(x);
}

//----------------------------------------------------------------------------------------
/// Returns the next integer value towards zero.
/// @param[in] x									The 64-bit floating point value to truncate.
/// @return												The truncated value.
//----------------------------------------------------------------------------------------
inline Float64 Truncate(Float64 x)
{
	if (x >= 0.0)
		return Floor(x);
	else
		return Ceil(x);
}

/// @}

/// @addtogroup c4d_tools_matrixvectortoolsfuncs Matrix/Vector Tools Functions
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Creates a translation matrix.
/// @param[in] t									The translation vector.
/// @return												The translation matrix.
//----------------------------------------------------------------------------------------
Matrix MatrixMove(const Vector& t);

//----------------------------------------------------------------------------------------
/// Creates a scaling matrix.
/// @param[in] s									The scaling vector for the axes.
/// @return												The scaling matrix.
//----------------------------------------------------------------------------------------
Matrix MatrixScale(const Vector& s);

//----------------------------------------------------------------------------------------
/// Creates a rotation matrix on the X axis.
/// @param[in] w									The angle around X.
/// @return												The rotation matrix.
//----------------------------------------------------------------------------------------
Matrix MatrixRotX(Float w);

//----------------------------------------------------------------------------------------
/// Creates a rotation matrix on the Y axis.
/// @param[in] w									The angle around Y.
/// @return												The rotation matrix.
//----------------------------------------------------------------------------------------
Matrix MatrixRotY(Float w);

//----------------------------------------------------------------------------------------
/// Creates a rotation matrix on the Z axis.
/// @param[in] w									The angle around Z.
/// @return												The rotation matrix.
//----------------------------------------------------------------------------------------
Matrix MatrixRotZ(Float w);

//----------------------------------------------------------------------------------------
/// Calculates Euler angles from a matrix.
/// @param[in] m									The rotation matrix.
/// @param[in] rot_order					The rotation order.
/// @return												The rotation HPB.
//----------------------------------------------------------------------------------------
Vector MatrixToHPB(const Matrix& m, ROTATIONORDER rot_order);

//----------------------------------------------------------------------------------------
/// Calculates Euler angles from a vector.
/// @note The bank is always set to @em 0.0.
/// @param[in] p									The vector to find the HPB for.
/// @return												The rotation HPB.
//----------------------------------------------------------------------------------------
Vector VectorToHPB(const Vector& p);

//----------------------------------------------------------------------------------------
/// Constructs a matrix from Euler angles.
/// @param[in] hpb								The input HPB.
/// @param[in] rot_order					The rotation order.
/// @return												The rotation matrix.
//----------------------------------------------------------------------------------------
Matrix HPBToMatrix(const Vector& hpb, ROTATIONORDER rot_order);

//----------------------------------------------------------------------------------------
/// Calculates rotation axis and angle from a matrix.
/// @param[in] m									The rotation matrix.
/// @param[out] v									Assigned the rotation axis.
/// @param[out] w									Assigned the rotation angle.
//----------------------------------------------------------------------------------------
void MatrixToRotAxis(const Matrix& m, Vector* v, Float* w);

//----------------------------------------------------------------------------------------
/// Calculates matrix from rotation axis and angle.
/// @param[in] v									The rotation axis.
/// @param[in] w									The rotation angle.
/// @return												The rotation matrix.
//----------------------------------------------------------------------------------------
Matrix RotAxisToMatrix(const Vector& v, Float w);

//----------------------------------------------------------------------------------------
/// Recalculates a matrix making it orthogonal if one or more of its vectors is collapsed (scale @em 0.0).
/// @param[in] m									The matrix to rebuild.
/// @return												The recalculated matrix with the collapsed components fixed and unit length.
//----------------------------------------------------------------------------------------
Matrix RebuildMatrix(const Matrix& m);

/// @}

//----------------------------------------------------------------------------------------
/// Modifies @formatParam{hpb_new} so that the "distance" to the last angle @formatParam{hpb_old} is at minimum.
/// @note Helps to avoid HPB singularity effects.
/// @param[in] hpb_old						The old HPB.
/// @param[in] hpb_new						The new HPB.
/// @param[in] order							The rotation order.
/// @return												The optimal angle.
//----------------------------------------------------------------------------------------
Vector GetOptimalAngle(const Vector& hpb_old, const Vector& hpb_new, ROTATIONORDER order);

//----------------------------------------------------------------------------------------
/// Calculates the distance from a point to a line.
/// @param[in] p0									The starting point of the line.
/// @param[in] v									The line vector.
/// @param[in] p									The point.
/// @return												The point-line vector.
//----------------------------------------------------------------------------------------
Vector PointLineDistance(const Vector& p0, const Vector& v, const Vector& p);

//----------------------------------------------------------------------------------------
/// Finds the ray vector after a reflection on a surface normal.
/// @param[in] v									The incoming ray.
/// @param[in] n									The surface normal.
/// @return												The reflected ray.
//----------------------------------------------------------------------------------------
Vector64 ReflectRay(const Vector64& v, const Vector64& n);

//-------------------------------------------------------------------------------------------------
/// A class to construct a bounding box around points.
//-------------------------------------------------------------------------------------------------
class LMinMax
{
private:
	Vector64 min, max;
	Bool		 used;

public:
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	LMinMax()
	{
		min	 = Vector64(MAXVALUE_FLOAT64);
		max	 = Vector64(MINVALUE_FLOAT64);
		used = false;
	}

	//----------------------------------------------------------------------------------------
	/// Constructs with vector @formatParam{v} set for both the minimum and maximum.
	/// @param[in] v									The vector for minimum and maximum.
	//----------------------------------------------------------------------------------------
	LMinMax(const Vector64& v)
	{
		min	 = max = v;
		used = true;
	}

	//-------------------------------------------------------------------------------------------------
	/// Resets the minimum and maximum to the largest values possible.
	//-------------------------------------------------------------------------------------------------
	inline void Init()
	{
		min	 = Vector64(MAXVALUE_FLOAT64);
		max	 = Vector64(MINVALUE_FLOAT64);
		used = false;
	}

	//----------------------------------------------------------------------------------------
	/// Initializes the minimum and maximum with vector @formatParam{v}.
	/// @param[in] v									The value to initialize minimum and maximum.
	//----------------------------------------------------------------------------------------
	inline void Init(const Vector64& v)
	{
		min	 = max = v;
		used = true;
	}

	//----------------------------------------------------------------------------------------
	/// Adds point @formatParam{p} and recalculates the minimum and maximum.
	/// @param[in] p									The point to add into the bounds.
	//----------------------------------------------------------------------------------------
	inline void AddPoint(const Vector64& p)
	{
		if (used)
		{
			if (p.x < min.x)
				min.x = p.x;
			if (p.y < min.y)
				min.y = p.y;
			if (p.z < min.z)
				min.z = p.z;
			if (p.x > max.x)
				max.x = p.x;
			if (p.y > max.y)
				max.y = p.y;
			if (p.z > max.z)
				max.z = p.z;
		}
		else
		{
			min	 = max = p;
			used = true;
		}
	}

	//----------------------------------------------------------------------------------------
	/// Adds points @formatParam{a} and @formatParam{b} and recalculates the minimum and maximum.
	/// @param[in] a									The minimum point to add into the bounds.
	/// @param[in] b									The maximum point to add into the bounds.
	//----------------------------------------------------------------------------------------
	inline void AddPoints(const Vector64& a, const Vector64& b)
	{
		used = true;
		if (a.x < min.x)
			min.x = a.x;
		if (a.y < min.y)
			min.y = a.y;
		if (a.z < min.z)
			min.z = a.z;
		if (b.x > max.x)
			max.x = b.x;
		if (b.y > max.y)
			max.y = b.y;
		if (b.z > max.z)
			max.z = b.z;
	}

	//----------------------------------------------------------------------------------------
	/// Sets the minimum and maximum of the bounding box.
	/// @param[in] a									The minimum.
	/// @param[in] b									The maximum.
	//----------------------------------------------------------------------------------------
	inline void Set(const Vector64& a, const Vector64& b)
	{
		used = true;
		min	 = a;
		max	 = b;
	}

	//----------------------------------------------------------------------------------------
	/// Checks if any points were added.
	/// @return												@trueIfOtherwiseFalse{if any points were added to the bounds}
	//----------------------------------------------------------------------------------------
	inline Bool Content() const
	{
		return used;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the minimum of the bounding box.
	/// @return												The minimum position of the bounds.
	//----------------------------------------------------------------------------------------
	inline const Vector64& GetMin() const
	{
		extern Vector64 l_empty;
		if (used)
			return min;
		else
			return l_empty;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the maximum of the bounding box.
	/// @return												The maximum position of the bounds.
	//----------------------------------------------------------------------------------------
	inline const Vector64& GetMax() const
	{
		extern Vector64 l_empty;
		if (used)
			return max;
		else
			return l_empty;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the middle between the minimum and maximum (the center).
	/// @return												The center of the bounds.
	//----------------------------------------------------------------------------------------
	inline Vector64 GetMp() const
	{
		if (used)
			return (min + max) * 0.5;
		else
			return Vector64(0.0);
	}

	//----------------------------------------------------------------------------------------
	/// Gets half the distance between the minimum and the maximum (the size).
	/// @return												The size of the bounds.
	//----------------------------------------------------------------------------------------
	inline Vector64 GetRad() const
	{
		if (used)
			return (max - min) * 0.5;
		else
			return Vector64(0.0);
	}

	//----------------------------------------------------------------------------------------
	/// Gets both the center and size of the bounding box.
	/// @param[out] mp								Assigned the center of the bounds.
	/// @param[out] rad								Assigned the size of the bounds.
	//----------------------------------------------------------------------------------------
	inline void GetMpRad(Vector64* mp, Vector64* rad) const
	{
		if (used)
		{
			*mp	 = (min + max) * 0.5;
			*rad = max - *mp;
		}
		else
		{
			*mp = *rad = Vector64(0.0);
		}
	}
};

//-------------------------------------------------------------------------------------------------
/// A class to construct a bounding box around points.
//-------------------------------------------------------------------------------------------------
class SMinMax
{
private:
	Vector32 min, max;
	Bool		 used;

public:
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	SMinMax()
	{
		min	 = Vector32(MAXVALUE_FLOAT32);
		max	 = Vector32(MINVALUE_FLOAT32);
		used = false;
	}

	//----------------------------------------------------------------------------------------
	/// Constructs with vector @formatParam{v} set for both the minimum and maximum.
	/// @param[in] v									The vector for minimum and maximum.
	//----------------------------------------------------------------------------------------
	SMinMax(const Vector32& v)
	{
		min	 = max = v;
		used = true;
	}

	//-------------------------------------------------------------------------------------------------
	/// Resets the minimum and maximum to the largest values possible.
	//-------------------------------------------------------------------------------------------------
	inline void Init()
	{
		min	 = Vector32(MAXVALUE_FLOAT32);
		max	 = Vector32(MINVALUE_FLOAT32);
		used = false;
	}

	//----------------------------------------------------------------------------------------
	/// Initializes the minimum and maximum with vector @formatParam{v}.
	/// @param[in] v									The value to initialize minimum and maximum.
	//----------------------------------------------------------------------------------------
	inline void Init(const Vector32& v)
	{
		min	 = max = v;
		used = true;
	}

	//----------------------------------------------------------------------------------------
	/// Adds point @formatParam{p} and recalculates the minimum and maximum.
	/// @param[in] p									The point to add into the bounds.
	//----------------------------------------------------------------------------------------
	inline void AddPoint(const Vector32& p)
	{
		if (used)
		{
			if (p.x < min.x)
				min.x = p.x;
			if (p.y < min.y)
				min.y = p.y;
			if (p.z < min.z)
				min.z = p.z;
			if (p.x > max.x)
				max.x = p.x;
			if (p.y > max.y)
				max.y = p.y;
			if (p.z > max.z)
				max.z = p.z;
		}
		else
		{
			min	 = max = p;
			used = true;
		}
	}

	//----------------------------------------------------------------------------------------
	/// Adds points @formatParam{a} and @formatParam{b} and recalculates the minimum and maximum.
	/// @param[in] a									The minimum point to add into the bounds.
	/// @param[in] b									The maximum point to add into the bounds.
	//----------------------------------------------------------------------------------------
	inline void AddPoints(const Vector32& a, const Vector32& b)
	{
		used = true;
		if (a.x < min.x)
			min.x = a.x;
		if (a.y < min.y)
			min.y = a.y;
		if (a.z < min.z)
			min.z = a.z;
		if (b.x > max.x)
			max.x = b.x;
		if (b.y > max.y)
			max.y = b.y;
		if (b.z > max.z)
			max.z = b.z;
	}

	//----------------------------------------------------------------------------------------
	/// Sets the minimum and maximum of the bounding box.
	/// @param[in] a									The minimum.
	/// @param[in] b									The maximum.
	//----------------------------------------------------------------------------------------
	inline void Set(const Vector32& a, const Vector32& b)
	{
		used = true;
		min	 = a;
		max	 = b;
	}

	//----------------------------------------------------------------------------------------
	/// Checks if any points were added.
	/// @return												@trueIfOtherwiseFalse{if any points were added to the bounds}
	//----------------------------------------------------------------------------------------
	inline Bool Content() const
	{
		return used;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the minimum of the bounding box.
	/// @return												The minimum position of the bounds.
	//----------------------------------------------------------------------------------------
	inline const Vector32& GetMin() const
	{
		extern Vector32 s_empty;
		if (used)
			return min;
		else
			return s_empty;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the maximum of the bounding box.
	/// @return												The maximum position of the bounds.
	//----------------------------------------------------------------------------------------
	inline const Vector32& GetMax() const
	{
		extern Vector32 s_empty;
		if (used)
			return max;
		else
			return s_empty;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the middle between the minimum and maximum (the center).
	/// @return												The center of the bounds.
	//----------------------------------------------------------------------------------------
	inline Vector32 GetMp() const
	{
		if (used)
			return (min + max) * (Float32) 0.5;
		else
			return Vector32(0.0);
	}

	//----------------------------------------------------------------------------------------
	/// Gets half the distance between the minimum and the maximum (the size).
	/// @return												The size of the bounds.
	//----------------------------------------------------------------------------------------
	inline Vector32 GetRad() const
	{
		if (used)
			return (max - min) * (Float32) 0.5;
		else
			return Vector32(0.0);
	}

	//----------------------------------------------------------------------------------------
	/// Gets both the center and size of the bounding box.
	/// @param[out] mp								Assigned the center of the bounds.
	/// @param[out] rad								Assigned the size of the bounds.
	//----------------------------------------------------------------------------------------
	inline void GetMpRad(Vector32* mp, Vector32* rad) const
	{
		if (used)
		{
			*mp	 = (min + max) * (Float32) 0.5;
			*rad = max - *mp;
		}
		else
		{
			*mp = *rad = Vector32(0.0);
		}
	}
};

#ifdef MAXON_TARGET_SINGLEPRECISION
typedef SMinMax MinMax;
#else
typedef LMinMax MinMax;
#endif

//-------------------------------------------------------------------------------------------------
/// A class to generate random number sequences.\n
/// Uniform and Gaussian deviates can be generated.
//-------------------------------------------------------------------------------------------------
class Random
{
private:
	UInt32 seed;
	Int32	 iset;
	Float	 gset;

public:
	//-------------------------------------------------------------------------------------------------
	/// Default constructor.
	//-------------------------------------------------------------------------------------------------
	Random();

	//----------------------------------------------------------------------------------------
	/// Initializes the random number sequence from a starting seed. Each seed will give a different sequence of random numbers.
	/// @param[in] s									The initial seed to start the random number sequence.
	//----------------------------------------------------------------------------------------
	void Init(UInt32 s);

	//----------------------------------------------------------------------------------------
	/// Gets a random value between @em 0 and @em 1.
	/// @return												A random value in the range [@em 0, @em 1].
	//----------------------------------------------------------------------------------------
	Float Get01();

	//----------------------------------------------------------------------------------------
	/// Gets a random value between @em -1 and @em +1.
	/// @return												A random value in the range [@em -1, @em +1].
	//----------------------------------------------------------------------------------------
	Float Get11();

	//----------------------------------------------------------------------------------------
	/// Gets a random value between @em 0 and @em 1 with Gaussian distribution.
	/// @return												A random value in the range [@em 0, @em 1] with Gaussian distribution.
	//----------------------------------------------------------------------------------------
	Float GetG01();

	//----------------------------------------------------------------------------------------
	/// Gets a random value between @em -1 and @em +1 with Gaussian distribution.
	/// @return												A random value in the range [@em -1, @em +1] with Gaussian distribution.
	//----------------------------------------------------------------------------------------
	Float GetG11();

	//----------------------------------------------------------------------------------------
	/// Gets the seed representing the current state of the random number generator.
	/// @return												The current seed.
	//----------------------------------------------------------------------------------------
	Int32 GetSeed()
	{
		return seed;
	}
};


/// @addtogroup c4d_tools_rgbhsvhsl RGB/HSV/HSL Conversions
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Converts RGB into the HSV color space.
/// @param[in] col								The RGB color to convert.
/// @return												The converted HSV color.
//----------------------------------------------------------------------------------------
Vector RGBToHSV(const Vector& col);

//----------------------------------------------------------------------------------------
/// Converts HSV into the RGB color space.
/// @param[in] col								The HSV color to convert.
/// @return												The converted RGB color.
//----------------------------------------------------------------------------------------
Vector HSVToRGB(const Vector& col);

//----------------------------------------------------------------------------------------
/// Converts RGB into the HSL color space.
/// @param[in] col								The RGB color to convert.
/// @return												The converted HSL color.
//----------------------------------------------------------------------------------------
Vector RGBToHSL(const Vector& col);

//----------------------------------------------------------------------------------------
/// Converts HSL into the RGB color space.
/// @param[in] col								The HSL color to convert.
/// @return												The converted RGB color.
//----------------------------------------------------------------------------------------
Vector HSLtoRGB(const Vector& col);

/// @}

/// @addtogroup c4d_tools_matrixrectnorm Matrix Rectangular/Normed
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Checks if a matrix is rectangular.
/// @param[in] m									The matrix to check.
/// @return												@trueIfOtherwiseFalse{the passed matrix is rectangular}
//----------------------------------------------------------------------------------------
Bool IsMatrixRectangular(const Matrix& m);

//----------------------------------------------------------------------------------------
/// Checks if a matrix is normed.
/// @param[in] m									The matrix to check.
/// @return												@trueIfOtherwiseFalse{the passed matrix is normed}
//----------------------------------------------------------------------------------------
Bool IsMatrixNormed(const Matrix& m);

/// @}

#pragma pack (pop)
}

#endif	// C4D_TOOLS_H__
