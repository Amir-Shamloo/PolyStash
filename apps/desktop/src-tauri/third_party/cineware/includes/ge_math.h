/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef GE_MATH_H__
#define GE_MATH_H__

#include "c4d_system.h"

#ifndef GCC34T
	#define GCC34T
#endif

namespace cineware
{
#pragma pack (push, 8)

typedef UChar PIX;			///< 8-bit integer pixel type.
typedef UChar PIX_C;		///< 8-bit integer pixel type.
typedef UInt16 PIX_W;		///< 16-bit integer pixel type.
typedef Float32 PIX_F;	///< 32-bit float pixel type.

/// @addtogroup group_comparecheckrepair_float Compare/Check/Repair Float
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Checks if two floats are close to each other on a bit basis (rather than a fixed epsilon).\n
/// Single precision version.
/// @param[in] a									A float value.
/// @param[in] b									A float value.
/// @return												@trueIfOtherwiseFalse{a and b are sufficiently close to each other}
//----------------------------------------------------------------------------------------
Bool CompareFloatTolerant(Float32 a, Float32 b);

//----------------------------------------------------------------------------------------
/// Checks if two floats are close to each other on a bit basis (rather than a fixed epsilon).\n
/// Double precision version.
/// @param[in] a									A float value.
/// @param[in] b									A float value.
/// @return												@trueIfOtherwiseFalse{a and b are sufficiently close to each other}
//----------------------------------------------------------------------------------------
Bool CompareFloatTolerant(Float64 a, Float64 b);

//----------------------------------------------------------------------------------------
/// Checks if @formatParam{r} is a valid floating point number. Single precision version.
/// @param[in] r									The value to check.
/// @return												@trueIfOtherwiseFalse{@formatParam{r} is a valid floating point number}\n
///																@formatConstant{true} for @em FP_NORMAL, @em FP_SUBNORMAL and @em FP_ZERO.\n
///																@formatConstant{false} for @em FP_NAN, @em FP_INFINITE.
//----------------------------------------------------------------------------------------
inline Bool CheckFloat(Float32 r)
{
	union
	{
		Float32 f;
		UInt32	u;
	} u = { r };

	return ((u.u >> 23) & 0xff) != 0xff;
}

//----------------------------------------------------------------------------------------
/// Checks if @formatParam{r} is a valid floating point number.\n
/// Double precision version.
/// @param[in] r									The value to check.
/// @return												@trueIfOtherwiseFalse{@formatParam{r} is a valid floating point number}\n
///																@formatConstant{true} for @em FP_NORMAL, @em FP_SUBNORMAL and @em FP_ZERO.\n
///																@formatConstant{false} for @em FP_NAN, @em FP_INFINITE.
//----------------------------------------------------------------------------------------
inline Bool CheckFloat(Float64 r)
{
	union
	{
		Float64 f;
		UInt64	u;
	} u = { r };

	return ((~u.u >> 52) & 0x7ff) != 0;
}

//----------------------------------------------------------------------------------------
/// Repairs floating point values.\n
/// Single precision version.
/// @warning	Only NaNs and Infinity are corrected. Denormalized numbers are not.
/// @note			Useful when importing other formats where the floating point data is not always valid.
/// @param[in] r									The float value to repair.
/// @return												The repaired value.
//----------------------------------------------------------------------------------------
inline Float32 RepairFloat(Float32 r)
{
	if (!CheckFloat(r))
		return 0.0;

	return r;
}

//----------------------------------------------------------------------------------------
/// Repairs floating point values.\n
/// Double precision version.
/// @warning	Only NaNs and Infinity are corrected. Denormalized numbers are not.
/// @note			Useful when importing other formats where the floating point data is not always valid.
/// @param[in] r									The float value to repair.
/// @return												The repaired value.
//----------------------------------------------------------------------------------------
inline Float64 RepairFloat(Float64 r)
{
	if (!CheckFloat(r))
		return 0.0;

	return r;
}

/// @}


extern Bool __NET;
extern Bool __SERVER;
extern Bool __CLIENT;
extern Bool __BODYPAINT;
extern Bool __STUDENT;
extern Bool __PARALLEL;
extern Bool __UPDATER;
extern Bool __INSTALLER;
extern Bool __LICENSESERVER;
extern Bool __MEDIZINI;
extern Bool __LITE;


#define C4D_FOUR_BYTE(x1, x2, x3, x4)			((x1 << 24) | (x2 << 16) | (x3 << 8) | x4)
#define C4D_RESERVE_PRIVATE_TYPE(type, x) type x


struct Vector64;
struct Matrix64;
struct Vector32;
struct Matrix32;


enum _DONTCONSTRUCT
{
	DC	///< Pass this to invoke the no-op constructor.
};

enum _EMPTYCONSTRUCT
{
	EC	///< Pass this to invoke the no-op empty constructor.
};

// Necessary to import in exactly this order
#include "ge_vector.h"
#include "ge_lvector.h"
#include "ge_matrix.h"
#include "ge_lmatrix.h"

#ifdef MAXON_TARGET_SINGLEPRECISION
typedef Vector32 Vector;
typedef Matrix32 Matrix;
typedef Vector32 Color;
#else
typedef Vector64 Vector;
typedef Matrix64 Matrix;
typedef Vector64 Color;
#endif

typedef Vector64 Color64;

#pragma pack (pop)
}

#endif	// GE_MATH_H__
