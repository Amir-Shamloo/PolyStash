/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_GENERAL_H__
#define C4D_GENERAL_H__

#undef  _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0

#include <stddef.h>
#include <string.h>
#include <math.h>

namespace cineware
{
#pragma pack (push, 8)

#ifdef MAXON_TARGET_WINDOWS
	#define	C4D_MISC_DLL_PUBLIC	__declspec(dllexport)
	#define	C4D_MISC_NO_INLINE	__declspec(noinline)
	#define C4D_MISC_ALIGN(_x_, _a_) __declspec(align(_a_)) _x_
	#define	C4D_MISC_ATTRIBUTE_USED
#elif defined MAXON_TARGET_ANDROID
	#define	C4D_MISC_DLL_PUBLIC	__attribute__((visibility("default")))
	#define	C4D_MISC_NO_INLINE	__attribute__ ((noinline))
	#define C4D_MISC_ALIGN(_x_, _a_) _x_ __attribute__((aligned(_a_)))
	#define	C4D_MISC_ATTRIBUTE_USED	__attribute__((used))
#else
	#define	C4D_MISC_DLL_PUBLIC	__attribute__((visibility("default")))
	#define	C4D_MISC_NO_INLINE	__attribute__ ((noinline))
	#define C4D_MISC_ALIGN(_x_, _a_) _x_ __attribute__((aligned(_a_)))

// use C4D_MISC_ATTRIBUTE_USED to tell the linker it must not dead-strip a function or static variable (even if it determines that the rest of the program does not refer to the object)
	#define	C4D_MISC_ATTRIBUTE_USED	__attribute__((used))
#endif


static const Int InvalidArrayIndex = -1;	///< invalid array index (e.g. for array index out of bounds)

/// This statement needs to be used for any case in a switch the contains code, but does not call break.
/// An example:
/// @code
/// switch(condition)
/// {
///   case 0:
///     ...do something...
///   // SWITCH_FALLTHROUGH
///
///   case 1:
///     ...do something...
///		  break;
///  }
/// @endcode
#define SWITCH_FALLTHROUGH

#ifndef DISALLOW_COPY_AND_ASSIGN
/// This statement needs to be used at the beginning of any class that cannot be copied
/// An example:
/// @code
/// class MyClass
/// {
///   DISALLOW_COPY_AND_ASSIGN(MyClass);
///	  ...
///	};
///	@endcode

	#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName &); \
	void operator =(const TypeName&);
#endif

#ifndef MOVE_ASSIGNMENT_OPERATOR
/// This statement automatically creates a move assignment operator from an existing move constructor. It needs to be written in the public: section.
/// An example:
/// @code
/// class MyClass
/// {
///   DISALLOW_COPY_AND_ASSIGN(MyClass);
/// public:
///   MOVE_ASSIGNMENT_OPERATOR(MyClass);
///	  ...
///	};
///	@endcode

	#define MOVE_ASSIGNMENT_OPERATOR(TypeName) \
	TypeName& operator =(TypeName&& src) \
	{ \
		if (this != &src) \
		{ \
			this->~TypeName(); \
			new (this)TypeName(::std::move(src)); \
		} \
		return *this; \
	}
#endif

//----------------------------------------------------------------------------------------
/// Automatic detection of CopyFrom() method using SFINAE.
/// If a class implements Bool CopyFrom(T& src) the constant TestForCopyFromMember<T>::IsSupported
/// will be true.
/// For this case the template argument deduction prefers the conversion of 0 to a pointer
/// (using Char TestForCopyFromFunction(TestForCopyFromWrapper<T, &T::CopyFrom>*)) over
/// binding to an ellipsis parameter (Int16 TestForCopyFromFunction(...)).
/// If a class does not implement CopyFrom() only Int16 TestForCopyFromFunction(...) is available.
/// Both cases can be distinguished by having different size return values.
//----------------------------------------------------------------------------------------
template <typename T, Bool(T::* f) (const T&)> struct TestForCopyFromWrapper { };
template <typename T> Char TestForCopyFromFunction(TestForCopyFromWrapper<T, & T::CopyFrom>*);	// template argument can only be dededuced if T contains a CopyFrom() method
template <typename T> Int16 TestForCopyFromFunction(...);																				// default case for classes without CopyFrom(), must have different size return value
template <typename T> struct TestForCopyFromMember
{
	static const bool isSupported = sizeof(TestForCopyFromFunction<T>(0)) == sizeof(Char);
};

template <typename T, Bool hasCopyFrom> class ObjectConstructor
{
};

// for classes with copy constructor that cannot fail (may or may not support move semantics)
template <typename T> class ObjectConstructor<T, false>
{
public:
	//----------------------------------------------------------------------------------------
	/// Construct an object using new (dst) T(src), cannot fail.
	/// @param[out] dst								Copied object will be placed here (can be nullptr)
	/// @param[in] src								Object to be copied.
	/// @return												Same as dst.
	//----------------------------------------------------------------------------------------
	static T* Copy(T* dst, const T& src)
	{
		return new (dst) T(src);
	}

	//----------------------------------------------------------------------------------------
	/// Copy assignment, cannot fail.
	/// @param[out] dst								Copy will be placed here (dst must have already been constructed)
	/// @param[in] src								Object to be copied.
	/// @return												Always true.
	//----------------------------------------------------------------------------------------
	static Bool AssignCopy(T& dst, const T& src)
	{
		dst = src;
		return true;
	}
};

// for classes with complex copy constructor (implements CopyFrom() and move constructor)
template <typename T> class ObjectConstructor<T, true>
{
public:
	//----------------------------------------------------------------------------------------
	/// Construct an object using CopyFrom() (like new (dst) T(src) but with error check)
	/// If copy construction fails the object will be automatically destructed, dst will
	/// point to uninitialized memory like before the call.
	/// @param[out] dst								Copied object will be placed here (can be nullptr)
	/// @param[in] src								Object to be copied.
	/// @return												Same as dst or nullptr (CopyFrom() failed)
	//----------------------------------------------------------------------------------------
	static T* Copy(T* dst, const T& src)
	{
		if (dst)
		{
			new (dst) T();										// initialize T
			if (dst->CopyFrom(src) == false)	// copy failed?
			{
				dst->~T();											// destruct
				dst = nullptr;
			}
		}
		return dst;
	}

	//----------------------------------------------------------------------------------------
	/// Copy assignment using CopyFrom()
	/// @param[out] dst								Copy will be placed here (dst must have already been constructed)
	/// @param[in] src								Object to be moved.
	/// @return												True if successful.
	//----------------------------------------------------------------------------------------
	static Bool AssignCopy(T& dst, const T& src)
	{
		return dst.CopyFrom(src);
	}
};

//----------------------------------------------------------------------------------------
/// call new (dst) T(src) or dst->CopyFrom(src) for non-trivial objector copy constructor
//----------------------------------------------------------------------------------------
template <typename T> inline T* NewCopy(T* dst, const T& src)
{
	return ObjectConstructor<T, TestForCopyFromMember<T>::isSupported>::Copy(dst, src);
}

//----------------------------------------------------------------------------------------
/// copy assignment
//----------------------------------------------------------------------------------------
template <typename T> inline Bool AssignCopy(T& dst, const T& src)
{
	return ObjectConstructor<T, TestForCopyFromMember<T>::isSupported>::AssignCopy(dst, src);
}

template <typename T> struct AlwaysFalse
{
	static bool const value = false;
};

/// @cond IGNORE
inline Int Align(Int value, Int alignment)
{
	return (value + (alignment - 1)) & ~(alignment - 1);
}

template <typename T> inline T* Align(T* value, Int alignment)
{
	return (T*) Align((Int) value, alignment);
}

// use this in a statement like
// DerivedFromAssertion<T, B>();
// to assert at compile-time that T is derived from B (or the same)
template <typename T, typename B> struct DerivedFromAssertion
{
	static void Check(B* b) { }
	DerivedFromAssertion()
	{
		Check((T*) nullptr);
	}
};


typedef Char FalseType;
typedef Int TrueType;

#define IS_TRUE_TYPE(E) (sizeof(E) == sizeof(cineware::TrueType))

// use Select<b, T, F>::Type to choose type T for b==true and type F for b==false
template <Bool, typename, typename> class Select;

template <typename T, typename F> class Select<false, T, F>
{
public:
	typedef F Type;
};

template <typename T, typename F> class Select<true, T, F>
{
public:
	typedef T Type;
};


// use ConstIf<b, T>::Type to choose type const T for b==true and type T for b==false
template <Bool, typename> class ConstIf;

template <typename T> class ConstIf<true, T>
{
public:
	typedef const T Type;
};

template <typename T> class ConstIf<false, T>
{
public:
	typedef T Type;
};


template <typename T, typename Check> class SFINAEHelper
{
public:
	typedef T Type;
};



// the following types and functions are needed for the NewObj macro

struct IsAllocType_False
{
	template <typename T> IsAllocType_False(const T&);
	IsAllocType_False();
};

struct IsAllocType_True : public IsAllocType_False
{
};

FalseType IsAllocType(IsAllocType_False);
TrueType	IsAllocType(IsAllocType_True);


struct IsImplementationType_False
{
	template <typename T> IsImplementationType_False(const T&);
	IsImplementationType_False();
};

struct IsImplementationType_True : public IsImplementationType_False
{
};

FalseType IsImplementationType(IsImplementationType_False);
TrueType	IsImplementationType(IsImplementationType_True);


// use C4D_MISC_ISEMPTY(__VA_ARGS__) to check if the arguments are empty (result will be 1) or not (result will be 0)

#ifdef _MSC_VER

// from http://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
	#define C4D_MISC_ISEMPTY(...)			_C4D_MISC_ISEMPTY((_C4D_MISC_ISEMPTY_PREFIX ## __VA_ARGS__ ## _C4D_MISC_ISEMPTY_POSTFIX, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0))
	#define _C4D_MISC_ISEMPTY(__args) _C4D_MISC_ISEMPTY2 __args
	#define _C4D_MISC_ISEMPTY_PREFIX_MISC_ISEMPTY_POSTFIX , , , , , , , , , , 1
	#define _C4D_MISC_ISEMPTY2(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9, __n, ...) __n

#else

// from http://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
	#define C4D_MISC_ISEMPTY(...) _C4D_MISC_ISEMPTY( \
		_C4D_MISC_HAS_COMMA(__VA_ARGS__),																		/* test if there is just one argument, eventually an empty one */ \
		_C4D_MISC_HAS_COMMA(_C4D_MISC_TRIGGER_PARENTHESIS_ __VA_ARGS__),		/* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma (if __VA_ARGS__ is parenthesized) */ \
		_C4D_MISC_HAS_COMMA(__VA_ARGS__(~)),																/* test if the argument together with a parenthesis adds a comma */ \
		_C4D_MISC_HAS_COMMA(_C4D_MISC_TRIGGER_PARENTHESIS_ __VA_ARGS__(~)))	/* this will have a comma if __VA_ARGS__ is empty (but also for other cases) */

	#define _C4D_MISC_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
	#define _C4D_MISC_HAS_COMMA(...)																																	 _C4D_MISC_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
	#define _C4D_MISC_TRIGGER_PARENTHESIS_(...)																												 ,
	#define _C4D_MISC_PASTE5(_0, _1, _2, _3, _4)																											 _0 ## _1 ## _2 ## _3 ## _4
	#define _C4D_MISC_ISEMPTY(_0, _1, _2, _3)																													 _C4D_MISC_HAS_COMMA(_C4D_MISC_PASTE5(_C4D_MISC_IS_EMPTY_CASE_, _0, _1, _2, _3))
	#define _C4D_MISC_IS_EMPTY_CASE_0001 ,

#endif


#define _C4D_MISC_PASTE(x, y) x ## y
#define C4D_MISC_PASTE(x, y)	_C4D_MISC_PASTE(x, y)


typedef void (* DeleteFunction) (void* object);

template <typename T> inline DeleteFunction ToDeleteFunction(void (* fn) (T*))
{
	return (DeleteFunction) fn;
}
/// @endcond

#if defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_LINUX)
	#define __debugbreak() __asm__("int $3\n" : :);
#elif defined(MAXON_TARGET_IOS)
#if defined(__i386__) || defined(__x86_64__)
		#define __debugbreak() __asm__("int $3\n" : :);
#else
		#define __debugbreak()	//__asm__ __volatile__ ("mov r0, %0\nmov r1, %1\nmov r12, #37\nswi 128\n" : : "r" (getpid ()), "r" (SIGINT) : "r12", "r0", "r1", "cc");
#endif
#elif !defined(MAXON_TARGET_WINDOWS)
	#define __debugbreak() { }
#endif

#pragma pack (pop)
}

#endif	// C4D_GENERAL_H__
