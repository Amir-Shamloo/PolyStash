/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SYSTEM_H__
#define C4D_SYSTEM_H__

// COMPILER DETECTION
#if defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1200
	#define MAXON_COMPILER_INTEL __INTEL_COMPILER
#elif defined(__clang__)
	#define MAXON_COMPILER_CLANG ((__clang_major__ * 100) + (__clang_minor__ * 10) + __clang_patchlevel__)
#elif _MSC_VER
	#define MAXON_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
	#define MAXON_COMPILER_GCC ((__GNUC__ * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)
#else
	#error "Unsupported Compiler"
#endif

// Target Detection, in case compiler settings are not properly adjusted
#if (__LP64__ || _WIN64) && !defined(MAXON_TARGET_64BIT)
  #define MAXON_TARGET_64BIT
#endif

#if (_WIN32 || _MSC_VER) && !defined(MAXON_TARGET_WINDOWS) && !defined MAXON_TARGET_ANDROID
  #define MAXON_TARGET_WINDOWS
#elif (__APPLE__)
  #include <TargetConditionals.h>
  #if defined(__has_feature)
    // TODO: (Seb) Add __has_feature for Ubsan. This does not exist yet.
    #if __has_feature(address_sanitizer) && !defined(MAXON_TARGET_SANITIZE)
      #define MAXON_TARGET_SANITIZE
    #endif
  #endif
  #if TARGET_OS_MAC && (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR) && !defined(MAXON_TARGET_IOS)
    #define MAXON_TARGET_IOS
  #elif TARGET_OS_MAC && !defined(MAXON_TARGET_MACOS) && !defined(MAXON_TARGET_IOS)
    #define MAXON_TARGET_MACOS
  #elif !defined(MAXON_TARGET_MACOS) && !defined(MAXON_TARGET_IOS)
    #error "Unsupported Target"
  #endif
  #if defined(MAXON_TARGET_MACOS) && !defined(MAXON_TARGET_OSX)
    #define MAXON_TARGET_OSX
  #endif
#elif defined(__GNUC__) && !defined(MAXON_TARGET_LINUX) && !defined MAXON_TARGET_ANDROID
  #define MAXON_TARGET_LINUX
#elif !defined(MAXON_TARGET_WINDOWS) && !defined(MAXON_TARGET_LINUX) && !defined MAXON_TARGET_ANDROID
  #error "Unsupported Target"
#endif

#if !defined(MAXON_TARGET_DEBUG) && !defined(MAXON_TARGET_RELEASE)
  #if !defined(NDEBUG) && (defined(_DEBUG) || (!defined(MAXON_TARGET_WINDOWS) && defined(DEBUG) && DEBUG == 1))
    #define MAXON_TARGET_DEBUG
  #else
    #define MAXON_TARGET_RELEASE
  #endif
#endif

#if defined(MAXON_TARGET_LINUX) || defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS) || defined(MAXON_TARGET_ANDROID)
	#define GCC34T this->
#endif

// CPU DETECTION
#ifdef MAXON_TARGET_WINDOWS
  #define MAXON_TARGET_CPU_INTEL
  #ifdef MAXON_TARGET_64BIT
    #define MAXON_TARGET_ABI_WIN_X64
  #else
    #define MAXON_TARGET_ABI_WIN_X86
  #endif
#elif defined(MAXON_TARGET_MACOS) || defined(MAXON_TARGET_LINUX) || defined(MAXON_TARGET_IOS) || defined(MAXON_TARGET_ANDROID)
  #if defined(__i386__) || defined(__x86_64__)
    #define MAXON_TARGET_CPU_INTEL
    #ifdef MAXON_TARGET_64BIT
      #define MAXON_TARGET_ABI_SYSTEMV_AMD64
    #endif
  #elif defined(__ppc__) || defined(__ppc64__)
    #define MAXON_TARGET_CPU_PPC
  #else
    #define MAXON_TARGET_CPU_ARM
  #endif
#endif

#if defined(MAXON_COMPILER_INTEL) || defined(MAXON_COMPILER_GCC) || defined(MAXON_COMPILER_CLANG)
	#define MAXON_ASSEMBLY_GCC
#endif

#if defined(MAXON_TARGET_OSX)
	#include <CoreServices/CoreServices.h>
#elif defined MAXON_TARGET_WINDOWS
	#pragma warning(disable:4244)
	#pragma warning(disable:4996)
#endif
#include <math.h>
#include <utility>

// standard int types available since VS2013
#if !defined MAXON_TARGET_WINDOWS || _MSC_VER >= 1800
	#include <inttypes.h>
#endif

namespace cineware
{
#pragma pack (push, 8)

// macros for instanceof
#define INSTANCEOFROOT(X)     \
	public:                      \
		virtual Bool IsInstanceOf(cineware::Int32 id) const \
		{                           \
			return id == X::GetType(); \
		}                           \
	private:

#define INSTANCE(X, Y) \
	public:               \
		typedef Y SUPER;     \
	private:

#define INSTANCEOF(X, Y)                \
	public:                                \
		typedef Y SUPER;                      \
		virtual Bool IsInstanceOf(cineware::Int32 id) const \
		{                                     \
			if (id == X::GetType()) return true; \
			if (id == X::GetInstanceType()) return true; \
			return Y::IsInstanceOf(id);          \
		}                                     \
	private:

#define OVERRIDENINSTANCEOF(X, Y)         \
	public:                                \
		typedef Y SUPER;                     \
		virtual Bool IsInstanceOf(Int32 xid) const;  \
		virtual Int64 GetInternalMemUsage() const \
		{                                    \
			return sizeof(X);                  \
		}																		 \
	private:

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
	#define ALLOC_TYPENAME
#else
	#define ALLOC_TYPENAME typename
#endif

#if !defined(_LIBCPP_UTILITY) && (defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS))	// Workaround for stdlib without move support
#if !(MAXON_TARGET_LINUX && MAXON_COMPILER_INTEL >= 1210)
#include <tr1/type_traits>																																// Must use std::tr1 for type traits
#endif

namespace std	// Add required C++11 support to old stdlib
{
template <typename T> struct remove_reference
{
	typedef T type;
};
template <typename T> struct remove_reference<T&>
{
	typedef T type;
};
template <typename T> struct remove_reference<T&&>
{
	typedef T type;
};
template <typename T> struct remove_pointer
{
	typedef T type;
};
template <typename T> struct remove_pointer<T*>
{
	typedef T type;
};
}
#endif


/// @addtogroup c4d_system_datatypes Data Types
/// @ingroup group_topic
/// @{
#if defined MAXON_TARGET_WINDOWS && _MSC_VER < 1800
typedef short int	Int16;						///< 16 bit signed integer data type.
typedef unsigned short int UInt16;	///< 16 bit unsigned integer data type.
typedef signed int Int32;						///< 32 bit signed integer data type.
typedef unsigned int UInt32;				///< 32 bit unsigned integer data type.
typedef signed long long Int64;			///< 64 bit signed integer data type.
typedef unsigned long long UInt64;	///< 64 bit unsigned integer data type.
#else
using Int16	 = int16_t;							///< 16 bit signed integer datatype.
using UInt16 = uint16_t;						///< 16 bit unsigned integer datatype.
using Int32	 = int32_t;							///< 32 bit signed integer datatype.
using UInt32 = uint32_t;						///< 32 bit unsigned integer datatype.
using Int64	 = int64_t;							///< 64 bit signed integer datatype.
using UInt64 = uint64_t;						///< 64 bit unsigned integer datatype.
#endif

typedef bool Bool;						///< Boolean type, possible values are only @formatConstant{false}/@formatConstant{true}, 8 bit.
typedef float	Float32;				///< 32 bit floating point value (float).
typedef double Float64;				///< 64 bit floating point value (double).
typedef char Char;						///< Signed 8 bit character.
typedef unsigned char	UChar;	///< Unsigned 8 bit character.

#ifdef MAXON_TARGET_64BIT
typedef Int64	Int;		///< Signed 32/64 bit int, size depends on the platform.
typedef UInt64 UInt;	///< Unsigned 32/64 bit int, size depends on the platform.
#else
typedef Int32	Int;		///< Signed 32/64 bit int, size depends on the platform
typedef UInt32 UInt;	///< Unsigned 32/64 bit int, size depends on the platform
#endif

/// Current floating point model.
/// Right now it is adjusted to Float64==64 bit but it may be redefined to Float32 anytime.
typedef Float64 Float;

//----------------------------------------------------------------------------------------
/// 16 bit unicode character.
/// UniChar is the datatype for one 16 bit unicode character
//----------------------------------------------------------------------------------------
#ifdef MAXON_TARGET_WINDOWS
typedef wchar_t UniChar;
#else
typedef char16_t UniChar;
#endif
/// @}

void* MemAllocNC(Int size);
void MemFree(void*& mem);

#define NewObj(T, ...) new (cineware::MemAllocNC(sizeof(T)))T(__VA_ARGS__)
#define DeleteObj(obj) \
	do { \
		ALLOC_TYPENAME ::std::remove_reference<decltype(obj)>::type* objectToDelete = &(obj); \
		if (*objectToDelete) { \
			typedef ALLOC_TYPENAME ::std::remove_pointer<ALLOC_TYPENAME ::std::remove_pointer<decltype(objectToDelete)>::type>::type CtorType; \
			((CtorType*) (*objectToDelete))->~CtorType(); \
			void* tmpToDelete = *objectToDelete; \
			cineware::MemFree(tmpToDelete); \
			*objectToDelete = nullptr; \
		} \
	} while (false)	// While ensures the user has to set a semicolon after DeleteObj

Int32 GeGetCurrentThreadCount();

void _GeDebugBreak(Int32 line, const Char* file);
#define CriticalStop() _GeDebugBreak(__LINE__, __FILE__)

#ifndef DebugAssert
#if defined MAXON_TARGET_DEBUG && !defined MAXON_TARGET_LINUX
		#define DebugAssert(a) if (!(a)) { CriticalStop(); }
#else
		#define DebugAssert(a) { }
#endif
#endif

/// @addtogroup EPSILON
/// @ingroup group_enumeration
/// @{
static const Float EPSILON		= 0.0001;	///< @em 0.0001
static const Float EPSILON2		= 0.01;		///< @em 1e-2 (@em 0.01) @since R20
static const Float EPSILON3		= 0.001;	///< @em 1e-3 (@em 0.001) @since R20
static const Float EPSILON5		= 1e-5;		///< @em 1e-5 (@em 0.00001)
static const Float EPSILON6		= 1e-6;		///< @em 1e-6
static const Float EPSILON7		= 1e-7;		///< @em 1e-7
static const Float EPSILON8		= 1e-8;		///< @em 1e-8
static const Float EPSILON10	= 1e-10;	///< @em 1e-10
/// @}


/// @addtogroup c4d_system_minmaxvalue MINVALUE/MAXVALUE
/// @ingroup group_miscellaneous
/// @{
static const Float32 MINVALUE_FLOAT32 = -3.402823466e+38f;						///< Minimum value a Float32 can represent.
static const Float32 MAXVALUE_FLOAT32 = 3.402823466e+38f;							///< Maximum value a Float32 can represent.

static const Float64 MINVALUE_FLOAT64 = -1.7976931348623158e+308;			///< Minimum value a Float64 can represent.
static const Float64 MAXVALUE_FLOAT64 = 1.7976931348623158e+308;			///< Maximum value a Float64 can represent.

static const Float32 MINVALUE_INT32_FLOAT32 = -2147483520.0f;					///< Minimum Float32 value that can be represented by Int32 (-0x7FFFFF80). Lower values will results in an overflow.
static const Float32 MAXVALUE_INT32_FLOAT32	= 2147483520.0f;					///< Maximum Float32 value that can be represented by Int32 ( 0x7FFFFF80). Higher values will results in an overflow.

static const Float64 MINVALUE_INT64_FLOAT64 = -9223372036854775295.0;	///< Minimum Float64 value that can be represented by Int64 (-0x7ffffffffffffdff). Lower values will results in an overflow.
static const Float64 MAXVALUE_INT64_FLOAT64 = 9223372036854775295.0;	///< Maximum Float64 value that can be represented by Int64 ( 0x7ffffffffffffdff). Higher values will results in an overflow.

static const Float32 MINRANGE_FLOAT32	= -1.0e10f;											///< 'Safe' minimum range for Float32. Guarantees that multiplication of two numbers doesn't produce an overflow.
static const Float32 MAXRANGE_FLOAT32	= 1.0e10f;											///< 'Safe' maximum range for Float32. Guarantees that multiplication of two numbers doesn't produce an overflow.

static const Float64 MINRANGE_FLOAT64 = -1.0e100;											///< 'Safe' minimum range for Float. Guarantees that multiplication of two numbers doesn't produce an overflow.
static const Float64 MAXRANGE_FLOAT64	= 1.0e100;											///< 'Safe' maximum range for Float. Guarantees that multiplication of two numbers doesn't produce an overflow.

#ifdef MAXON_TARGET_SINGLEPRECISION
static const Float MINVALUE_FLOAT = MINVALUE_FLOAT32;	///< Minimum value a Float can represent.
static const Float MAXVALUE_FLOAT = MAXVALUE_FLOAT32;	///< Maximum value a Float can represent.

static const Float MINRANGE_FLOAT = MINRANGE_FLOAT32;	///< 'Safe' minimum range for Float64. Guarantees that multiplication of two numbers doesn't produce an overflow.
static const Float MAXRANGE_FLOAT = MAXRANGE_FLOAT32;	///< 'Safe' maximum range for Float64. Guarantees that multiplication of two numbers doesn't produce an overflow.
#else
static const Float MINVALUE_FLOAT = MINVALUE_FLOAT64;	///< Minimum value a Float can represent.
static const Float MAXVALUE_FLOAT = MAXVALUE_FLOAT64;	///< Maximum value a Float can represent.

static const Float MINRANGE_FLOAT = MINRANGE_FLOAT64;	///< 'Safe' minimum range for Float64. Guarantees that multiplication of two numbers doesn't produce an overflow.
static const Float MAXRANGE_FLOAT = MAXRANGE_FLOAT64;	///< 'Safe' maximum range for Float64. Guarantees that multiplication of two numbers doesn't produce an overflow.
#endif
/// @}


/// @addtogroup c4d_system_limit LIMIT
/// @ingroup group_miscellaneous
/// @{

/// Class to determine the limits of a datatype. Use LIMIT<type>::Min() or LIMIT<type>::Max() to return the minimum or maximum values that can be represented by the datatype.
/// Use the LIMIT<type>::MIN or LIMIT<type>::MAX for integer datatypes (this does not work for floating point types though).
template <typename T> class LIMIT
{
};

template <> class LIMIT<Int64>
{
public:
	static const Int64 MIN = -9223372036854775807LL - 1;
	static const Int64 MAX = 9223372036854775807LL;

	static Int64 Min()
	{
		return -9223372036854775807LL - 1;
	}
	static Int64 Max()
	{
		return 9223372036854775807LL;
	}
};

template <> class LIMIT<UInt64>
{
public:
	static const UInt64 MIN = 0;
	static const UInt64 MAX = 0xffffffffffffffffULL;

	static UInt64 Min()
	{
		return 0;
	}
	static UInt64 Max()
	{
		return 0xffffffffffffffffULL;
	}
};

template <> class LIMIT<Int32>
{
public:
	static const Int32 MIN = -2147483647L - 1;
	static const Int32 MAX = 2147483647L;

	static Int32 Min()
	{
		return -2147483647L - 1;
	}
	static Int32 Max()
	{
		return 2147483647L;
	}
};

template <> class LIMIT<UInt32>
{
public:
	static const UInt32 MIN = 0;
	static const UInt32 MAX = 0xffffffffULL;

	static UInt32 Min()
	{
		return 0;
	}
	static UInt32 Max()
	{
		return 0xffffffffULL;
	}
};

template <> class LIMIT<Int16>
{
public:
	static const Int16 MIN = -32767 - 1;
	static const Int16 MAX = 32767;

	static Int16 Min()
	{
		return -32767 - 1;
	}
	static Int16 Max()
	{
		return 32767;
	}
};

template <> class LIMIT<UInt16>
{
public:
	static const UInt16 MIN = 0;
	static const UInt16 MAX = 0xffff;

	static UInt16 Min()
	{
		return 0;
	}
	static UInt16 Max()
	{
		return 0xffff;
	}
};

template <> class LIMIT<Char>
{
public:
	static const Char MIN = -127 - 1;
	static const Char MAX = 127;

	static Char Min()
	{
		return -127 - 1;
	}
	static Char Max()
	{
		return 127;
	}
};

template <> class LIMIT<UChar>
{
public:
	static const UChar MIN = 0;
	static const UChar MAX = 0xff;

	static UChar Min()
	{
		return 0;
	}
	static UChar Max()
	{
		return 0xff;
	}
};

template <> class LIMIT<Float32>
{
public:
	static Float32 Min()
	{
		return -3.402823466e+38f;
	}
	static Float32 Max()
	{
		return 3.402823466e+38f;
	}
};

template <> class LIMIT<Float64>
{
public:
	static Float64 Min()
	{
		return -1.7976931348623158e+308;
	}
	static Float64 Max()
	{
		return 1.7976931348623158e+308;
	}
};

/// @}


#ifndef NOTOK
	static const Int NOTOK = -1;	///< constant used for special cases.
#endif

/// @addtogroup c4d_system_math Math
/// @ingroup group_topic
/// @{

/// Constant for PI
static const Float64 PI = 3.1415926535897932384626433832795;

/// Constant for 1.0 / PI
static const Float64 PI_INV = 0.31830988618379067153776752674508;

/// Constant for 2.0 * PI
static const Float64 PI2 = 6.283185307179586476925286766559;

/// Constant for 1.0 / (2.0 * PI)
static const Float64 PI2_INV = 0.15915494309189533576888376337251;

/// Constant for 0.5 * PI
static const Float64 PI05 = 1.5707963267948966192313216916398;


//----------------------------------------------------------------------------------------
/// Gets the minimum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The minimum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Float32 FMin(Float32 a, Float32 b)
{
	if (a < b)
		return a;
	return b;
}

//----------------------------------------------------------------------------------------
/// Gets the minimum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The minimum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Float64 FMin(Float64 a, Float64 b)
{
	if (a < b)
		return a;
	return b;
}

//----------------------------------------------------------------------------------------
/// Gets the minimum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The minimum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Int32 LMin(Int32 a, Int32 b)
{
	if (a < b)
		return a;
	return b;
}

//----------------------------------------------------------------------------------------
/// Gets the minimum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The minimum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Int VMin(Int a, Int b)
{
	if (a < b)
		return a;
	return b;
}

//----------------------------------------------------------------------------------------
/// Gets the maximum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The maximum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Float32 FMax(Float32 a, Float32 b)
{
	if (a < b)
		return b;
	return a;
}

//----------------------------------------------------------------------------------------
/// Gets the maximum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The maximum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Float64 FMax(Float64 a, Float64 b)
{
	if (a < b)
		return b;
	return a;
}

//----------------------------------------------------------------------------------------
/// Gets the maximum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The maximum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Int32 LMax(Int32 a, Int32 b)
{
	if (a < b)
		return b;
	return a;
}

//----------------------------------------------------------------------------------------
/// Gets the maximum value of @formatParam{a} and @formatParam{b} parameters.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The maximum value of @formatParam{a} and @formatParam{b}.
//----------------------------------------------------------------------------------------
inline Int VMax(Int a, Int b)
{
	if (a < b)
		return b;
	return a;
}

//----------------------------------------------------------------------------------------
/// Limits the value of @formatParam{a} between @formatParam{b} and @formatParam{c}.
/// @param[in] a									A value.
/// @param[in] b									Lower bound.
/// @param[in] c									Upper bound.
/// @return												@formatParam{b} if @formatParam{a} < @formatParam{b}, @formatParam{c} if @formatParam{a} > @formatParam{c}, otherwise @formatParam{a}.
//----------------------------------------------------------------------------------------
inline Int32 LCut(Int32 a, Int32 b, Int32 c)
{
	if (a < b)
		return b;
	if (a > c)
		return c;
	return a;
}

//----------------------------------------------------------------------------------------
/// Limits the value of @formatParam{a} between @formatParam{b} and @formatParam{c}.
/// @param[in] a									A value.
/// @param[in] b									Lower bound.
/// @param[in] c									Upper bound.
/// @return												@formatParam{b} if @formatParam{a} < @formatParam{b}, @formatParam{c} if @formatParam{a} > @formatParam{c}, otherwise @formatParam{a}.
//----------------------------------------------------------------------------------------
inline Int VCut(Int a, Int b, Int c)
{
	if (a < b)
		return b;
	if (a > c)
		return c;
	return a;
}

//----------------------------------------------------------------------------------------
/// Limits the value of @formatParam{a} between @formatParam{b} and @formatParam{c}.
/// @param[in] a									A value.
/// @param[in] b									Lower bound.
/// @param[in] c									Upper bound.
/// @return												@formatParam{b} if @formatParam{a} < @formatParam{b}, @formatParam{c} if @formatParam{a} > @formatParam{c}, otherwise @formatParam{a}.
//----------------------------------------------------------------------------------------
inline Float32 FCut(Float32 a, Float32 b, Float32 c)
{
	if (a < b)
		return b;
	if (a > c)
		return c;
	return a;
}

//----------------------------------------------------------------------------------------
/// Limits the value of @formatParam{a} between @formatParam{b} and @formatParam{c}.
/// @param[in] a									A value.
/// @param[in] b									Lower bound.
/// @param[in] c									Upper bound.
/// @return												@formatParam{b} if @formatParam{a} < @formatParam{b}, @formatParam{c} if @formatParam{a} > @formatParam{c}, otherwise @formatParam{a}.
//----------------------------------------------------------------------------------------
inline Float64 FCut(Float64 a, Float64 b, Float64 c)
{
	if (a < b)
		return b;
	if (a > c)
		return c;
	return a;
}

//----------------------------------------------------------------------------------------
/// Clips a single-precision floating point number against the lower limit @em 0 and the upper limit @em 0 1.
/// @param[in] a									A value.
/// @return												@formatParam{a} value clamped.
//----------------------------------------------------------------------------------------
inline Float32 Clamp01(Float32 a)
{
	if (a < 0.0f)
		return 0.0f;
	if (a > 1.0f)
		return 1.0f;
	return a;
}

//----------------------------------------------------------------------------------------
/// Clips a single-precision floating point number against the lower limit @em 0 and the upper limit @em 1.
/// @param[in] a									A value.
/// @return												@formatParam{a} value clamped.
//----------------------------------------------------------------------------------------
inline Float64 Clamp01(Float64 a)
{
	if (a < 0.0)
		return 0.0;
	if (a > 1.0)
		return 1.0;
	return a;
}


#if defined MAXON_TARGET_OSX || defined MAXON_TARGET_IOS
inline Float32 Sin(Float32 val)
{
	return sinf(val);
}
inline Float64 Sin(Float64 val)
{
	return sin(val);
}

inline Float32 Cos(Float32 val)
{
	return cosf(val);
}
inline Float64 Cos(Float64 val)
{
	return cos(val);
}

inline Float32 Tan(Float32 val)
{
	return tanf(val);
}
inline Float64 Tan(Float64 val)
{
	return tan(val);
}

inline Float32 ATan(Float32 val)
{
	return atanf(val);
}
inline Float64 ATan(Float64 val)
{
	return atan(val);
}

inline Float32 ATan2(Float32 valA, Float32 valB)
{
	return atan2f(valA, valB);
}
inline Float64 ATan2(Float64 valA, Float64 valB)
{
	return atan2(valA, valB);
}

inline Float32 Exp(Float32 val)
{
	return expf(val);
}
inline Float64 Exp(Float64 val)
{
	return exp(val);
}

inline Float32 Ln(Float32 val)
{
	return logf(val);
}
inline Float64 Ln(Float64 val)
{
	return log(val);
}

inline Float32 Log10(Float32 val)
{
	return log10f(val);
}
inline Float64 Log10(Float64 val)
{
	return log10(val);
}

inline Float32 Log2(Float32 val)
{
	return log2f(val);
}
inline Float64 Log2(Float64 val)
{
	return log2(val);
}

inline Float32 Sqrt(Float32 val)
{
	return sqrtf(val);
}
inline Float64 Sqrt(Float64 val)
{
	return sqrt(val);
}

inline Float32 Floor(Float32 val)
{
	return floorf(val);
}
inline Float64 Floor(Float64 val)
{
	return floor(val);
}

inline Float32 Ceil(Float32 val)
{
	return ceilf(val);
}
inline Float64 Ceil(Float64 val)
{
	return ceil(val);
}

inline Float32 Pow(Float32 v1, Float32 v2)
{
	return powf(v1, v2);
}
inline Float64 Pow(Float64 v1, Float64 v2)
{
	return pow(v1, v2);
}

inline Float32 Sinh(Float32 val)
{
	return sinhf(val);
}
inline Float64 Sinh(Float64 val)
{
	return sinh(val);
}

inline Float32 Cosh(Float32 val)
{
	return coshf(val);
}
inline Float64 Cosh(Float64 val)
{
	return cosh(val);
}

inline Float32 Tanh(Float32 val)
{
	return tanhf(val);
}
inline Float64 Tanh(Float64 val)
{
	return tanh(val);
}

inline Float32 FMod(Float32 v1, Float32 v2)
{
	return fmodf(v1, v2);
}
inline Float64 FMod(Float64 v1, Float64 v2)
{
	return fmod(v1, v2);
}

inline Float32 Abs(Float32 val)
{
	return fabsf(val);
}
inline Float64 Abs(Float64 val)
{
	return fabs(val);
}
#else

//----------------------------------------------------------------------------------------
/// Calculates the sine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated sine.
//----------------------------------------------------------------------------------------
inline Float32 Sin(Float32 val)
{
	return sin(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the sine of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated sine.
//----------------------------------------------------------------------------------------
inline Float64 Sin(Float64 val)
{
	return sin(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the cosine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated sine.
//----------------------------------------------------------------------------------------
inline Float32 Cos(Float32 val)
{
	return cos(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the cosine of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated sine.
//----------------------------------------------------------------------------------------
inline Float64 Cos(Float64 val)
{
	return cos(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the tangent of @formatParam{val}.
/// @note	The input value needs to be checked for proper range, so that no exceptions will be generated.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated tangent.
//----------------------------------------------------------------------------------------
inline Float32 Tan(Float32 val)
{
	return tan(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the tangent of @formatParam{val}.
/// @note	The input value needs to be checked for proper range, so that no exceptions will be generated.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated tangent.
//----------------------------------------------------------------------------------------
inline Float64 Tan(Float64 val)
{
	return tan(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the arctangent of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated arctangent.
//----------------------------------------------------------------------------------------
inline Float32 ATan(Float32 val)
{
	return atan(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the arctangent of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated arctangent.
//----------------------------------------------------------------------------------------
inline Float64 ATan(Float64 val)
{
	return atan(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the arctangent2 of @formatParam{valA} and @formatParam{valB}.
/// @param[in] valA								A single-precision floating point value.
/// @param[in] valB								A single-precision floating point value.
/// @return												The calculated arctangent2.
//----------------------------------------------------------------------------------------
inline Float32 ATan2(Float32 valA, Float32 valB)
{
	return atan2(valA, valB);
}

//----------------------------------------------------------------------------------------
/// Calculates the arctangent2 of @formatParam{valA} and @formatParam{valB}.
/// @param[in] valA								A double-precision floating point value.
/// @param[in] valB								A double-precision floating point value.
/// @return												The calculated arctangent2.
//----------------------------------------------------------------------------------------
inline Float64 ATan2(Float64 valA, Float64 valB)
{
	return atan2(valA, valB);
}

//----------------------------------------------------------------------------------------
/// Calculates e^@formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated exponential value.
//----------------------------------------------------------------------------------------
inline Float32 Exp(Float32 val)
{
	return exp(val);
}

//----------------------------------------------------------------------------------------
/// Calculates e^@formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated exponential value.
//----------------------------------------------------------------------------------------
inline Float64 Exp(Float64 val)
{
	return exp(val);
}

//----------------------------------------------------------------------------------------
/// Calculates logarithm of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated logarithm value.
//----------------------------------------------------------------------------------------
inline Float32 Ln(Float32 val)
{
	return log(val);
}

//----------------------------------------------------------------------------------------
/// Calculates logarithm of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated logarithm value.
//----------------------------------------------------------------------------------------
inline Float64 Ln(Float64 val)
{
	return log(val);
}

//----------------------------------------------------------------------------------------
/// Calculates logarithm with base @em 10 of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated logarithm with base @em 10 value.
//----------------------------------------------------------------------------------------
inline Float32 Log10(Float32 val)
{
	return log10(val);
}


//----------------------------------------------------------------------------------------
/// Calculates logarithm with base @em 10 of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated logarithm with base @em 10 value.
//----------------------------------------------------------------------------------------
inline Float64 Log10(Float64 val)
{
	return log10(val);
}

//----------------------------------------------------------------------------------------
/// Calculates logarithm with base @em 2 of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated logarithm with base @em 2 value.
//----------------------------------------------------------------------------------------
inline Float32 Log2(Float32 val)
{
	return log(val) * 1.4426950408889634073599246810019f;
}

//----------------------------------------------------------------------------------------
/// Calculates logarithm with base @em 2 of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated logarithm with base @em 2 value.
//----------------------------------------------------------------------------------------
inline Float64 Log2(Float64 val)
{
	return log(val) * 1.4426950408889634073599246810019;
}

//----------------------------------------------------------------------------------------
/// Calculates square root of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated square root value.
//----------------------------------------------------------------------------------------
inline Float32 Sqrt(Float32 val)
{
	return sqrt(val);
}


//----------------------------------------------------------------------------------------
/// Calculates square root of @formatParam{val}.
/// @note	The input needs to be positive, so that no exceptions will be generated.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated square root value.
//----------------------------------------------------------------------------------------
inline Float64 Sqrt(Float64 val)
{
	return sqrt(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the largest previous integer number of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated floor value.
//----------------------------------------------------------------------------------------
inline Float32 Floor(Float32 val)
{
	return floor(val);
}


//----------------------------------------------------------------------------------------
/// Calculates the largest previous integer number of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated floor value.
//----------------------------------------------------------------------------------------
inline Float64 Floor(Float64 val)
{
	return floor(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the smallest following integer number of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated ceil value.
//----------------------------------------------------------------------------------------
inline Float32 Ceil(Float32 val)
{
	return ceil(val);
}

//----------------------------------------------------------------------------------------
/// Calculates the smallest following integer number of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated ceil value.
//----------------------------------------------------------------------------------------
inline Float64 Ceil(Float64 val)
{
	return ceil(val);
}

//----------------------------------------------------------------------------------------
/// Calculates @formatParam{v1}^@formatParam{v2}.
/// @param[in] v1									A single-precision floating point value.
/// @param[in] v2									A single-precision floating point value.
/// @return												The calculated pow value.
//----------------------------------------------------------------------------------------
inline Float32 Pow(Float32 v1, Float32 v2)
{
	return pow(v1, v2);
}

//----------------------------------------------------------------------------------------
/// Calculates @formatParam{v1}^@formatParam{v2}.
/// @param[in] v1									A double-precision floating point value.
/// @param[in] v2									A double-precision floating point value.
/// @return												The calculated pow value.
//----------------------------------------------------------------------------------------
inline Float64 Pow(Float64 v1, Float64 v2)
{
	return pow(v1, v2);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic sine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated hyperbolic sine value.
//----------------------------------------------------------------------------------------
inline Float32 Sinh(Float32 val)
{
	return sinh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic sine of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated hyperbolic sine value.
//----------------------------------------------------------------------------------------
inline Float64 Sinh(Float64 val)
{
	return sinh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic cosine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated hyperbolic cosine value.
//----------------------------------------------------------------------------------------
inline Float32 Cosh(Float32 val)
{
	return cosh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic cosine of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated hyperbolic cosine value.
//----------------------------------------------------------------------------------------
inline Float64 Cosh(Float64 val)
{
	return cosh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic tangent of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated hyperbolic tangent value.
//----------------------------------------------------------------------------------------
inline Float32 Tanh(Float32 val)
{
	return tanh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates hyperbolic tangent of @formatParam{val}.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated hyperbolic tangent value.
//----------------------------------------------------------------------------------------
inline Float64 Tanh(Float64 val)
{
	return tanh(val);
}

//----------------------------------------------------------------------------------------
/// Calculates floating point modulo @formatParam{v1}%@formatParam{v2}.
/// @param[in] v1									A single-precision floating point value. The quotient numerator.
/// @param[in] v2									A single-precision floating point value. The quotient denominator.
/// @return												The remainder of dividing  @formatParam{v1} and @formatParam{v2}.
//----------------------------------------------------------------------------------------
inline Float32 FMod(Float32 v1, Float32 v2)
{
	return fmod(v1, v2);
}

//----------------------------------------------------------------------------------------
/// Calculates floating point modulo @formatParam{v1}%@formatParam{v2}.
/// @param[in] v1									A double-precision floating point value. The quotient numerator.
/// @param[in] v2									A double-precision floating point value. The quotient denominator.
/// @return												The remainder of dividing  @formatParam{v1} and @formatParam{v2}.
//----------------------------------------------------------------------------------------
inline Float64 FMod(Float64 v1, Float64 v2)
{
	return fmod(v1, v2);
}

//----------------------------------------------------------------------------------------
/// Calculates the absolute value of floating point number @formatParam{f}.
/// @param[in] f									A single-precision floating point value.
/// @return												The calculated absolute value.
//----------------------------------------------------------------------------------------
inline Float32 Abs(Float32 f)
{
	return fabs(f);
}

//----------------------------------------------------------------------------------------
/// Calculates the absolute value of floating point number @formatParam{f}.
/// @param[in] f									A double-precision floating point value.
/// @return												The calculated absolute value.
//----------------------------------------------------------------------------------------
inline Float64 Abs(Float64 f)
{
	return fabs(f);
}
#endif

//----------------------------------------------------------------------------------------
/// Calculates the reciprocal (multiplicative inverse) of @formatParam{val}.
/// @note	If the input value is zero, zero will be returned for safety to avoid exceptions.
/// @param[in] f									A single-precision floating point value.
/// @return												The calculated inverse value.
//----------------------------------------------------------------------------------------
inline Float32 Inverse(Float32 f)
{
	return (f == 0.0f) ? 0.0f : 1.0f / f;
}

//----------------------------------------------------------------------------------------
/// Calculates the reciprocal (multiplicative inverse) of @formatParam{val}.
/// @note	If the input value is zero, zero will be returned for safety to avoid exceptions.
/// @param[in] f									A double-precision floating point value.
/// @return												The calculated inverse value.
//----------------------------------------------------------------------------------------
inline Float64 Inverse(Float64 f)
{
	return (f == 0.0) ? 0.0 : 1.0 / f;
}

#ifndef SIZEOF
	/// Calculates the size of a data type or element.
	#define SIZEOF(x) ((cineware::Int)sizeof(x))
#endif

//----------------------------------------------------------------------------------------
/// Calculates the absolute value of @formatParam{f}.
/// @param[in] f									The value to get its absolute.
/// @return												The absolute value.
//----------------------------------------------------------------------------------------
template <typename X> inline X Abs(X f)
{
	if (f < 0)
		return -f;
	return f;
}

//----------------------------------------------------------------------------------------
/// Calculates the minimum of @formatParam{a} and @formatParam{b}.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The minimum.
//----------------------------------------------------------------------------------------
template <typename X> inline X Min(X a, X b)
{
	if (a < b)
		return a;
	return b;
}


//----------------------------------------------------------------------------------------
/// Calculates the maximum @formatParam{a} and @formatParam{b}.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The maximum.
//----------------------------------------------------------------------------------------
template <typename X> inline X Max(X a, X b)
{
	if (a < b)
		return b;
	return a;
}

//----------------------------------------------------------------------------------------
/// Swaps values @formatParam{a} and @formatParam{b}.
/// @note	If available, move semantics will be used.
/// @param[in] a									A value.
/// @param[in] b									A value.
//----------------------------------------------------------------------------------------
template <typename X> inline void Swap(X& a, X& b)
{
	X c(std::move(a));
	a = std::move(b);
	b = std::move(c);
}

//----------------------------------------------------------------------------------------
/// Clips a value against a lower and upper limit.
/// @param[in] value							The value to clamp.
/// @param[in] lowerLimit					The lower limit.
/// @param[in] upperLimit					The upper limit.
/// @return												The clamped value.
//----------------------------------------------------------------------------------------
template <typename X> inline X ClampValue(X value, X lowerLimit, X upperLimit)
{
	if (value < lowerLimit)
		return lowerLimit;
	if (value > upperLimit)
		return upperLimit;
	return value;
}

//----------------------------------------------------------------------------------------
/// Blends values @formatParam{value1} and @formatParam{value2}.
/// @param[in] value1							The first value in the range to blend.
/// @param[in] value2							The second value in the range to blend.
/// @param[in] blendValue					The blend value.
/// @return												If @formatParam{blendValue} is @em 0.0 @formatParam{value1} is returned.\n
///																If @formatParam{blendValue} is @em 1.0 @formatParam{value2} is returned.\n
///																No clipping below @em 0.0 or @em 1.0 takes place. In that case use BoxStep.
//----------------------------------------------------------------------------------------
template <typename X, typename Y> inline X Blend(const X& value1, const X& value2, Y blendValue)
{
	return value1 + (value2 - value1) * blendValue;
}

//----------------------------------------------------------------------------------------
/// Calculates square difference of @formatParam{a} and @formatParam{b}.
/// @param[in] a									A value.
/// @param[in] b									A value.
/// @return												The square difference.
//----------------------------------------------------------------------------------------
template <typename X> inline Float Sqr(X a, X b)
{
	X tmp = a - b;
	return tmp * tmp;
}

//----------------------------------------------------------------------------------------
/// Calculates square of @formatParam{a}.
/// @param[in] a									A value.
/// @return												The square.
//----------------------------------------------------------------------------------------
template <typename X> inline Float Sqr(X a)
{
	return a * a;
}

//----------------------------------------------------------------------------------------
/// Calculates arc sine of @formatParam{val}.
/// @note	The input value is clipped for safety to avoid exceptions.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated arcsine value.
//----------------------------------------------------------------------------------------
inline Float32 ASin(Float32 val)
{
	if (val >= 1.0f)
		return Float32(PI05);
	else if (val <= -1.0f)
		return Float32(-PI05);
	return asinf(val);
}

//----------------------------------------------------------------------------------------
/// Calculates arc sine of @formatParam{val}.
/// @note	The input value is clipped for safety to avoid exceptions.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated arcsine value.
//----------------------------------------------------------------------------------------
inline Float64 ASin(Float64 val)
{
	if (val >= 1.0f)
		return Float32(PI05);
	else if (val <= -1.0f)
		return Float32(-PI05);
	return asin(val);
}

//----------------------------------------------------------------------------------------
/// Calculates arc cosine of @formatParam{val}.
/// @note	The input value is clipped for safety to avoid exceptions.
/// @param[in] val								A single-precision floating point value.
/// @return												The calculated arc cosine value.
//----------------------------------------------------------------------------------------
inline Float32 ACos(Float32 val)
{
	if (val >= 1.0f)
		return 0.0f;
	else if (val <= -1.0f)
		return Float32(PI);
	return acosf(val);
}

//----------------------------------------------------------------------------------------
/// Calculates arc cosine of @formatParam{val}.
/// @note	The input value is clipped for safety to avoid exceptions.
/// @param[in] val								A double-precision floating point value.
/// @return												The calculated arc cosine value.
//----------------------------------------------------------------------------------------
inline Float64 ACos(Float64 val)
{
	if (val >= 1.0f)
		return 0.0f;
	else if (val <= -1.0f)
		return Float32(PI);
	return acos(val);
}

//----------------------------------------------------------------------------------------
/// Converts @formatParam{val} from degrees to radians.
/// @param[in] r									A single-precision floating point value.
/// @return												The converted degrees to radians value.
//----------------------------------------------------------------------------------------
inline Float32 DegToRad(Float32 r)
{
	return (r) * Float32(PI) / 180.0f;
}

//----------------------------------------------------------------------------------------
/// Converts @formatParam{val} from degrees to radians.
/// @param[in] r									A double-precision floating point value.
/// @return												The converted degrees to radians value.
//----------------------------------------------------------------------------------------
inline Float64 DegToRad(Float64 r)
{
	return (r) * PI / 180.0;
}

//----------------------------------------------------------------------------------------
/// Converts  @formatParam{val} from radians to degrees.
/// @param[in] r									A single-precision floating point value.
/// @return												The converted radians to degrees value.
//----------------------------------------------------------------------------------------
inline Float32 RadToDeg(Float32 r)
{
	return (r) * 180.0f / Float32(PI);
}

//----------------------------------------------------------------------------------------
/// Converts @formatParam{val} from radians to degrees.
/// @param[in] r									A double-precision floating point value.
/// @return												The converted radians to degrees value.
//----------------------------------------------------------------------------------------
inline Float64 RadToDeg(Float64 r)
{
	return (r) * 180.0 / PI;
}

//----------------------------------------------------------------------------------------
/// Calculates both sine and cosine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @param[out] sn								Assigned the calculated sine value.
/// @param[out] cs								Assigned the calculated cosine value.
//----------------------------------------------------------------------------------------
inline void SinCos(Float32 val, Float32& sn, Float32& cs)
{
	sn = Sin(val);
	cs = Cos(val);
}

//----------------------------------------------------------------------------------------
/// Calculates both sine and cosine of @formatParam{val}.
/// @param[in] val								A single-precision floating point value.
/// @param[out] sn								Assigned the calculated sine value.
/// @param[out] cs								Assigned the calculated cosine value.
//----------------------------------------------------------------------------------------
inline void SinCos(Float64 val, Float64& sn, Float64& cs)
{
	sn = Sin(val);
	cs = Cos(val);
}

/// @}


/// @addtogroup c4d_system_safeintuint SAFEINT/SAFEUINT
/// @ingroup group_miscellaneous
/// @{

//----------------------------------------------------------------------------------------
/// Safely converts a 32-bit float value into ::Int32.
/// @param[in] x									A single-precision floating point value.
/// @return												The converted ::Int32. The resulting value is clipped against the ::Int32 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int32 SAFEINT32(Float32 x)
{
	if (x <= MINVALUE_INT32_FLOAT32)
		return LIMIT<Int32>::MIN;
	else if (x >= MAXVALUE_INT32_FLOAT32)
		return LIMIT<Int32>::MAX;
	return Int32(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 64-bit float value into ::Int32.
/// @param[in] x									A double-precision floating point value.
/// @return												The converted ::Int32. The resulting value is clipped against the ::Int32 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int32 SAFEINT32(Float64 x)
{
	if (x <= (Float64)LIMIT<Int32>::MIN)
		return LIMIT<Int32>::MIN;
	else if (x >= (Float64)LIMIT<Int32>::MAX)
		return LIMIT<Int32>::MAX;
	return Int32(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 64-bit float value into ::UInt32.
/// @param[in] x									A double-precision floating point value.
/// @return												The converted ::UInt32. The resulting value is clipped against the ::Int32 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline UInt32 SAFEUINT32(Float64 x)
{
	if (x <= (Float64)LIMIT<UInt32>::MIN)
		return LIMIT<UInt32>::MIN;
	else if (x >= (Float64)LIMIT<UInt32>::MAX)
		return LIMIT<UInt32>::MAX;
	return UInt32(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 32-bit float value into ::Int64.
/// @param[in] x									A single-precision floating point value to convert.
/// @return												The converted ::Int64. The resulting value is clipped against the ::Int64 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int64 SAFEINT64(Float32 x)
{
	if (x <= MINVALUE_INT32_FLOAT32)
		return (Int64)LIMIT<Int32>::MIN;
	else if (x >= MAXVALUE_INT32_FLOAT32)
		return (Int64)LIMIT<Int32>::MAX;
	return Int64(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 64-bit float value into ::Int64.
/// @param[in] x									A double-precision floating point value to convert.
/// @return												The converted ::Int64. The resulting value is clipped against the ::Int64 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int64 SAFEINT64(Float64 x)
{
	if (x <= MINVALUE_INT64_FLOAT64)
		return LIMIT<Int64>::MIN;
	else if (x >= MAXVALUE_INT64_FLOAT64)
		return LIMIT<Int64>::MAX;
	return Int64(x);
}

#ifdef MAXON_TARGET_64BIT
//----------------------------------------------------------------------------------------
/// Safely converts a 64-bit float value into ::Int64.
/// @param[in] x									A double-precision floating point value to convert.
/// @return												The converted ::Int64. The resulting value is clipped against the ::Int64 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int SAFEINT(Float64 x)
{
	return SAFEINT64(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 32-bit float value into ::Int64.
/// @param[in] x									A single-precision floating point value to convert.
/// @return												The converted ::Int64. The resulting value is clipped against the ::Int64 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int SAFEINT(Float32 x)
{
	return SAFEINT64(x);
}
#else
//----------------------------------------------------------------------------------------
/// Safely converts a 64-bit float value into ::Int32.
/// @param[in] x									A double-precision floating point value.
/// @return												The converted ::Int32. The resulting value is clipped against the ::Int32 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int SAFEINT(Float64 x)
{
	return SAFEINT32(x);
}

//----------------------------------------------------------------------------------------
/// Safely converts a 32-bit float value into ::Int32.
/// @param[in] x									A single-precision floating point value.
/// @return												The converted ::Int32. The resulting value is clipped against the ::Int32 boundaries, without raising an exception.
//----------------------------------------------------------------------------------------
inline Int SAFEINT(Float32 x)
{
	return SAFEINT32(x);
}
#endif
/// @}


/// @cond IGNORE

class IllegalEnumAccess
{
public:
	IllegalEnumAccess() { }
};

class IllegalFlagAccess
{
public:
	IllegalFlagAccess() { }
};

#define ENUM_END_FLAGS(X) ; \
	inline X operator | (X a, X b) { return X(UInt32(a) | UInt32(b)); } \
	inline X operator & (X a, X b) { return X(UInt32(a) & UInt32(b)); } \
	inline X operator ^ (X a, X b) { return X(UInt32(a) ^ UInt32(b)); } \
	inline X operator >> (X a, X b) { return X(UInt32(a) << UInt32(b)); } \
	inline X operator << (X a, X b) { return X(UInt32(a) >> UInt32(b)); } \
	inline X operator ~ (X a) { return X(~UInt32(a)); } \
	inline X& operator |= (X& a, const X& b) { a = X(UInt32(a) | UInt32(b)); return a; } \
	inline X& operator &= (X& a, const X& b) { a = X(UInt32(a) & UInt32(b)); return a; } \
	inline X& operator ^= (X& a, const X& b) { a = X(UInt32(a) ^ UInt32(b)); return a; } \
	inline IllegalFlagAccess operator ++ (X a) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator ++ (X a, int) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator -- (X a) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator -- (X a, int) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator != (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator != (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator == (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator == (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator < (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator < (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator < (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator <= (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator <= (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator <= (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator > (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator > (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator > (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator >= (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator >= (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator >= (Int32 s2, X s1) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator + (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator + (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator + (Int32 s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator - (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator - (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator - (Int32 s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator * (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator * (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator * (Int32 s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator / (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator / (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator / (Int32 s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator % (X s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator % (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator % (Int32 s1, X s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator ^ (X s1, Int32 s2) { return IllegalFlagAccess(); } \
	inline IllegalFlagAccess operator ^ (Int32 s1, X s2) { return IllegalFlagAccess(); }	// -V524

#define ENUM_END_LIST(X) ; \
	inline IllegalEnumAccess operator | (X a, X b) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator & (X a, X b) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ~ (X a) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ++ (X a) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ++ (X a, int) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator -- (X a) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator -- (X a, int) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator == (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator == (Int32 s2, X s1) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator != (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator != (Int32 s2, X s1) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ! (X a) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator + (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator + (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator - (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator - (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator * (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator * (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator / (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator / (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator % (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator % (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator < (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator < (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator < (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator > (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator > (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator > (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator <= (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator <= (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator <= (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator >= (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator >= (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator >= (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator << (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator << (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator >> (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator >> (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ^ (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator ^ (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator || (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator || (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator || (Int32 s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator && (X s1, X s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator && (X s1, Int32 s2) { return IllegalEnumAccess(); } \
	inline IllegalEnumAccess operator && (Int32 s1, X s2) { return IllegalEnumAccess(); }	// -V524

/// @endcond

#pragma pack (pop)
}

#endif	// C4D_SYSTEM_H__
