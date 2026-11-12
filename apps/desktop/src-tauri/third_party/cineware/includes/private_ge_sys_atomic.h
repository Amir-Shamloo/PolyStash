/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_GE_SYS_ATOMIC_H__
#define PRIVATE_GE_SYS_ATOMIC_H__

#ifndef MAXON_TARGET_IOS

#include "ge_math.h"	// basic type definitions

#ifdef MAXON_TARGET_WINDOWS
	typedef long LONG;
#endif

#if _MSC_VER >= 1400
	#include <intrin.h>
#endif

#if defined(MAXON_TARGET_ANDROID) || defined(MAXON_TARGET_CPU_ARM)
	#include <atomic>
#endif

namespace cineware
{
#pragma pack (push, 8)

#ifdef MAXON_TARGET_WINDOWS

#if _MSC_VER >= 1400
#if !defined(__ICL) && !defined(__llvm__)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_ReadWriteBarrier)

#ifdef MAXON_TARGET_64BIT
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedExchange64)
#endif
#endif
#endif

#define	atomic_cas32(dst, xchg, cmp)	((Int32)	_InterlockedCompareExchange((volatile LONG*) (dst), (Int32) xchg, (Int32) cmp))
#define	atomic_swap_add32(dst, value)	((Int32)	_InterlockedExchangeAdd((volatile LONG*) (dst), (Int32) value))
#define	atomic_swap_or32(dst, mask)		((Int32)	_InterlockedOr((volatile LONG*) (dst), (Int32) mask))
#define	atomic_swap_and32(dst, mask)	((Int32)	_InterlockedAnd((volatile LONG*) (dst), (Int32) mask))
#define	atomic_swap32(dst, value)			((Int32)	_InterlockedExchange((volatile LONG*) (dst), (Int32) value))

#ifdef MAXON_TARGET_64BIT
	#define	atomic_cas64(dst, xchg, cmp)	 ((Int64)  _InterlockedCompareExchange64((volatile Int64*) (dst), (Int64) xchg, (Int64) cmp))
	#define	atomic_swap_add64(dst, value)	 ((Int64)  _InterlockedExchangeAdd64((volatile Int64*) (dst), (Int64) value))
	#define	atomic_swap64(dst, value)			 ((Int64)  _InterlockedExchange64((volatile Int64*) (dst), (Int64) value))
	#define	AtomicCASPtr(dst, xchg, cmp)	 ((void*)	_InterlockedCompareExchange64((volatile Int64*) (dst), (Int64) xchg, (Int64) cmp))
	#define	AtomicSwapPtr(dst, value)			 ((void*)	_InterlockedExchange64((volatile Int64*) (dst), (Int64) value))
	#define	AtomicCASVLONG(dst, xchg, cmp) ((Int)  _InterlockedCompareExchange64((volatile Int64*) (dst), (Int64) xchg, (Int64) cmp))
	#define	AtomicSwapAddVLONG(dst, value) ((Int)  _InterlockedExchangeAdd64((volatile Int64*) (dst), (Int64) value))
	#define	AtomicSwapVLONG(dst, value)		 ((Int)  _InterlockedExchange64((volatile Int64*) (dst), (Int64) value))
#else
	#define	AtomicCASPtr(dst, xchg, cmp)		((void*) _InterlockedCompareExchange((volatile LONG*) (dst), (Int32) xchg, (Int32) cmp))
	#define	AtomicSwapPtr(dst, value)				((void*) _InterlockedExchange((volatile LONG*) (dst), (Int32) value))
	#define	AtomicCASVInt32(dst, xchg, cmp)	((Int)	_InterlockedCompareExchange((volatile LONG*) (dst), (Int32) xchg, (Int32) cmp))
	#define	AtomicSwapAddVInt32(dst, value)	((Int)	_InterlockedExchangeAdd((volatile LONG*) (dst), (Int32) value))
	#define	AtomicSwapVInt32(dst, value)		((Int)	_InterlockedExchange((volatile LONG*) (dst), (Int32) value))
#endif

#else	// Mac, Linux, Android

#if defined(MAXON_TARGET_ANDROID) || defined(MAXON_TARGET_CPU_ARM)
	static_assert(sizeof(std::atomic<Int32>) == 4, "wrong std::atomic 32 bit size");
	static_assert(sizeof(std::atomic<Int64>) == 8, "wrong std::atomic 64 bit size");
#endif

#if defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS)
#include <libkern/OSAtomic.h>
#endif

#ifdef MAXON_TARGET_64BIT
	#define	AtomicCASPtr(dst, xchg, cmp)	 ((void*)	atomic_cas64((volatile Int64*) (dst), (Int64) xchg, (Int64) cmp))
	#define	AtomicSwapPtr(dst, xchg)			 ((void*)	atomic_swap64((volatile Int64*) (dst), (Int64) xchg))
	#define	AtomicCASVLONG(dst, xchg, cmp) ((Int)  atomic_cas64((volatile Int64*) (dst), (Int64) xchg, (Int64) cmp))
	#define	AtomicSwapAddVLONG(dst, value) ((Int)  atomic_swap_add64((volatile Int64*) (dst), (Int64) value))
	#define	AtomicSwapVLONG(dst, xchg)		 ((void*)	atomic_swap64((volatile Int64*) (dst), (Int64) xchg))
#else
	#define	AtomicCASPtr(dst, xchg, cmp)	 ((void*)	atomic_cas32((volatile Int32*) (dst), (Int32) xchg, (Int32) cmp))
	#define	AtomicSwapPtr(dst, xchg)			 ((void*)	atomic_swap32((volatile Int32*) (dst), (Int32) xchg))
	#define	AtomicCASVLONG(dst, xchg, cmp) ((Int)  atomic_cas32((volatile Int32*) (dst), (Int32) xchg, (Int32) cmp))
	#define	AtomicSwapAddVLONG(dst, value) ((Int)  atomic_swap_add32((volatile Int32*) (dst), (Int32) value))
	#define	AtomicSwapVLONG(dst, xchg)		 ((void*)	atomic_swap32((volatile Int32*) (dst), (Int32) xchg))
#endif

//----------------------------------------------------------------------------------------
// Atomic Compare And Swap: Exchange memory location content with new value if its equal to comparision value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// xchg:							new value
// cmp:								comparison value
//----------------------------------------------------------------------------------------
inline Int32 atomic_cas32(volatile Int32* dst, Int32 xchg, Int32 cmp)
{
	Int32	prev;

#if (defined __ppc__) || (defined __ppc64__)
#ifndef __GE_SYS_ATOMIC_CAS_USE_OSATOMIC__
	Int32	tmp;

	asm volatile ("lwsync\n\t"	// write barrier, complete prior stores to make sure lwarx reads the correct value

								"0:\n\t"
								"lwarx	%0,0,%2\n\t"	// load memory location content, reserve for write
								"addi		%1,r1,-8\n\t"	// address in red zone
								"cmpw		%0,%4\n\t"		// matches comparison value?
								"beq+   2f\n\t"

								"stwcx. %3,0,%1\n\t"	// on 970 must release reservation, store into red zone to release
								"b			3f\n\t"

								"2:"
								"stwcx. %3,0,%2\n\t"	// store swap value if the reservation is still valid
								"bne-   0b\n\t"				// retry if the memory location has been modified in between
								"isync\n\t"						// read barrier, make sure that prefetches of following instructions are discarded

								"3:"
								: "=&r" (prev), "=&r" (tmp)					// output
								: "b" (dst), "r" (xchg), "r" (cmp)	// inputs
								: "memory", "cc");									// memory and condition code register are modified
#else
	do
	{
		prev = *dst;

		if (OSAtomicCompareAndSwap32Barrier((int32_t) cmp, (int32_t) xchg, (volatile int32_t*) dst))
		{
			prev = cmp;
			break;
		}
	} while (prev == cmp);
#endif
#elif defined(MAXON_TARGET_CPU_INTEL)

	asm volatile ("lock; cmpxchgl %2,%1"
								: "=a" (prev), "=m" (*dst)	// output: from eax
								: "r" (xchg), "0" (cmp)			// inputs: cmp takes the same register as output (eax)
								: "memory");								// memory is modified

#else
	std::atomic<Int32>& rdst = *(std::atomic<Int32>*)dst;

	if (rdst.compare_exchange_strong(cmp, xchg, std::memory_order_seq_cst, std::memory_order_relaxed))
			return cmp;
	else
	{
		// This sucks. Ideally it should return a bool, but now we have to fiddle with the value to make sure the caller
		prev = *dst;
		if (prev == cmp)
			prev = ~cmp;
	}
#endif

	return prev;	// previous memory content
}

//----------------------------------------------------------------------------------------
// Atomic Add: Add value to memory location content and return the previous value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// value:							value to add
//----------------------------------------------------------------------------------------
inline Int32 atomic_swap_add32(volatile Int32* dst, Int32 value)
{
#if (defined __ppc__) || (defined __ppc64__)
#ifndef __GE_SYS_ATOMIC_ADD_USE_OSATOMIC__
	Int32	prev, tmp;

	asm volatile ("lwsync\n\t"								// write barrier, complete prior stores to make sure lwarx reads the correct value
								"0:\n\t"
								"lwarx	%0,0,%2\n\t"				// load memory location content, reserve for write
								"add		%1,%0,%3\n\t"				// tmp = prev + value
								"stwcx.	%1,0,%2\n\t"				// store new value (tmp) if the reservation is still valid
								"bne-		0b\n\t"							// retry if the memory location has been modified in between
								"isync\n\t"									// read barrier, make sure that prefetches of following instructions are discarded

								: "=&r" (prev), "=&r" (tmp)	// output
								: "b" (dst), "r" (value)		// input
								: "memory", "cc");					// memory and condition code register are modified

	return prev;
#else
	return OSAtomicAdd32Barrier((int32_t) value, (volatile int32_t*) dst) - value;
#endif
#elif defined(MAXON_TARGET_CPU_INTEL)

	asm volatile ("lock; xaddl %0,%1"
								: "=r" (value), "=m" (*dst)	// output: dst is modified
								: "0" (value)								// input: value takes the same register as on output
								: "memory");								// memory and condition code register are modified

	return value;
#else
	std::atomic<Int32>& rdst = *(std::atomic<Int32>*)dst;

	return rdst.fetch_add(value, std::memory_order_seq_cst);
#endif
}

//----------------------------------------------------------------------------------------
// Atomic Swap: Store a value and return the memory location's previous value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// value:							new value
//----------------------------------------------------------------------------------------
inline Int32 atomic_swap32(volatile Int32* dst, Int32 value)
{
	Int32	prev;

#if (defined __ppc__) || (defined __ppc64__)
#ifndef __GE_SYS_ATOMIC_SWAP_USE_OSATOMIC__

	asm volatile ("lwsync\n\t"	// write barrier, complete prior stores to make sure lwarx reads the correct value

								"0:\n\t"
								"lwarx	%0,0,%1\n\t"			// load memory location content, reserve for write
								"stwcx. %2,0,%1\n\t"			// store swap value if the reservation is still valid
								"bne-   0b\n\t"						// retry if the memory location has been modified in between
								"isync\n\t"								// read barrier, make sure that prefetches of following instructions are discarded

								: "=&r" (prev)						// output
								: "b" (dst), "r" (value)	// inputs
								: "memory", "cc");				// memory and condition code register are modified

#else

	while (1)
	{
		prev = *dst;
		if (OSAtomicCompareAndSwap32Barrier((int32_t) prev, (int32_t) value, (volatile int32_t*) dst))
			break;
	}

#endif
#elif defined(MAXON_TARGET_CPU_INTEL)

	asm volatile ("lock; xchgl %0,%1"
								: "=r" (prev), "=m" (*dst)
								: "0" (value)
								: "memory");

#else
	std::atomic<Int32>& rdst = *(std::atomic<Int32>*)dst;

	return rdst.exchange(value);
#endif

	return prev;
}

#ifdef MAXON_TARGET_64BIT
//----------------------------------------------------------------------------------------
// Atomic Compare And Swap: Exchange memory location content with new value if its equal to comparision value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// xchg:							new value
// cmp:								comparison value
//----------------------------------------------------------------------------------------
inline Int64 atomic_cas64(volatile Int64* dst, Int64 xchg, Int64 cmp)
{
	Int64	prev;

#ifdef __ppc64__
	Int64	tmp;

	asm volatile ("lwsync\n\t"	// write barrier, complete prior stores to make sure lwarx reads the correct value

								"0:\n\t"
								"ldarx	%0,0,%2\n\t"	// load memory location content, reserve for write
								"addi		%1,r1,-8\n\t"	// address in red zone
								"cmpd		%0,%4\n\t"		// matches comparison value?
								"beq+   2f\n\t"

								"stdcx. %3,0,%1\n\t"	// on 970 must release reservation, store into red zone to release
								"b			3f\n\t"

								"2:"
								"stdcx. %3,0,%2\n\t"	// store swap value if the reservation is still valid
								"bne-   0b\n\t"				// retry if the memory location has been modified in between
								"isync\n\t"						// read barrier, make sure that prefetches of following instructions are discarded

								"3:"
								: "=&r" (prev), "=&r" (tmp)					// output
								: "b" (dst), "r" (xchg), "r" (cmp)	// inputs
								: "memory", "cc");									// memory and condition code register are modified

#elif defined(__x86_64__)

	asm volatile ("lock; cmpxchgq %2,%1"
								: "=a" (prev), "=m" (*dst)
								: "q" (xchg), "0" (cmp)
								: "memory");

#else
	do
	{
		prev = *dst;

		if (OSAtomicCompareAndSwap64Barrier((int64_t) cmp, (int64_t) xchg, (volatile int64_t*) dst))
		{
			prev = cmp;
			break;
		}
	} while (prev == cmp);

#endif

	return prev;
}

//----------------------------------------------------------------------------------------
// Atomic Add: Add value to memory location content and return the previous value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// value:							value to add
//----------------------------------------------------------------------------------------
inline Int64 atomic_swap_add64(volatile Int64* dst, Int64 value)
{
	Int64	prev;

#ifdef __ppc64__
	Int64	tmp;

	asm volatile ("lwsync\n\t"								// write barrier, complete prior stores to make sure lwarx reads the correct value
								"0:\n\t"
								"ldarx	%0,0,%2\n\t"				// load memory location content, reserve for write
								"add		%1,%0,%3\n\t"				// tmp = prev + value
								"stdcx.	%1,0,%2\n\t"				// store new value (tmp) if the reservation is still valid
								"bne-		0b\n\t"							// retry if the memory location has been modified in between
								"isync\n\t"									// read barrier, make sure that prefetches of following instructions are discarded

								: "=&r" (prev), "=&r" (tmp)	// output
								: "b" (dst), "r" (value)		// input
								: "memory", "cc");					// memory and condition code register are modified
#elif defined(__x86_64__)

	asm volatile ("lock; xaddq %0,%1"
								: "=r" (prev), "=m" (*dst)	// output: dst is modified
								: "0" (value)								// input: value takes the same register as on output
								: "memory");								// memory is modified

#else
	return OSAtomicAdd64Barrier((int64_t) value, (volatile int64_t*) dst) - value;
#endif

	return prev;
}

//----------------------------------------------------------------------------------------
// Atomic Swap: Store a value and return the memory location's previous value
// This function implicitely creates a memory barrier
// Function result:		previous memory location content
// dst:								memory location
// value:							new value
//----------------------------------------------------------------------------------------
inline Int64 atomic_swap64(volatile Int64* dst, Int64 value)
{
	Int64	prev;

#ifdef __ppc64__
	asm volatile ("lwsync\n\t"	// write barrier, complete prior stores to make sure lwarx reads the correct value

								"0:\n\t"
								"ldarx	%0,0,%1\n\t"			// load memory location content, reserve for write
								"stdcx. %2,0,%1\n\t"			// store swap value if the reservation is still valid
								"bne-   0b\n\t"						// retry if the memory location has been modified in between
								"isync\n\t"								// read barrier, make sure that prefetches of following instructions are discarded

								: "=&r" (prev)						// output
								: "b" (dst), "r" (value)	// inputs
								: "memory", "cc");				// memory and condition code register are modified
#elif defined(__x86_64__)

	asm volatile ("lock; xchgq %0,%1"
								: "=r" (prev), "=m" (*dst)
								: "0" (value)
								: "memory");

#else
	while (1)
	{
		prev = *dst;
		if (OSAtomicCompareAndSwap64Barrier((int64_t) prev, (int64_t) value, (volatile int64_t*) dst))
			break;
	}
#endif

	return prev;
}

#endif

//----------------------------------------------------------------------------------------
// Atomic Or: Or value to memory location content and return the previous value
// Function result:		previous memory location content
// dst:								memory location
// mask:							mask for bitwise or
//----------------------------------------------------------------------------------------
inline Int32 atomic_swap_or32(volatile Int32* dst, Int32 mask)
{
	Int32	old_value;
	Int32	new_value;

	do
	{
		old_value = *dst;
		new_value = old_value | mask;
	} while (atomic_cas32(dst, new_value, old_value) != old_value);

	return old_value;
}

//----------------------------------------------------------------------------------------
// Atomic And: And value to memory location content and return the previous value
// Function result:		previous memory location content
// dst:								memory location
// mask:							mask for bitwise and
//----------------------------------------------------------------------------------------
inline Int32 atomic_swap_and32(volatile Int32* dst, Int32 mask)
{
	Int32	old_value;
	Int32	new_value;

	do
	{
		old_value = *dst;
		new_value = old_value & mask;
	} while (atomic_cas32(dst, new_value, old_value) != old_value);

	return old_value;
}

#endif

//----------------------------------------------------------------------------------------
// Atomic pointer class
// CompareAndSwap() returns the previous value and changes the memory location if it's the same as the comparison value
//----------------------------------------------------------------------------------------
template <class T> class AtomicPtr
{
	volatile T* ptr;

public:
	inline T* Get()
	{
		return (T*) ptr;
	}
	inline void	Set(T* new_value)
	{
		ptr = new_value;
	}
	inline T* CompareAndSwap(T* new_value, T* compare)
	{
		return (T*) AtomicCASPtr(&ptr, new_value, compare);
	}
	inline Bool	TryCompareAndSwap(T* new_value, T* compare)
	{
		return (T*) AtomicCASPtr(&ptr, new_value, compare) == compare;
	}
	inline T* Swap(T* new_value)
	{
		return (T*) AtomicSwapPtr(&ptr, new_value);
	}
};

typedef AtomicPtr<void>		AtomicVoidPtr;

//----------------------------------------------------------------------------------------
// Atomic 32 bit integer class
// CompareAndSwap() returns the previous value and changes the memory location if it's the same as the comparison value
// SwapAdd(), SwapIncrement() and SwapDecrement() return the previous value
//----------------------------------------------------------------------------------------
template <class T> class Atomic32
{
	volatile T value;

public:
	inline T Get()
	{
		return value;
	}
	inline void	Set(T new_value)
	{
		value = new_value;
	}
	inline T CompareAndSwap(T new_value, T compare)
	{
		return (T) atomic_cas32((volatile Int32*) &value, (Int32) new_value, (Int32) compare);
	}
	inline Bool	TryCompareAndSwap(T new_value, T compare)
	{
		return atomic_cas32((volatile Int32*) &value, (Int32) new_value, (Int32) compare) == (Int32) compare;
	}
	inline T Swap(T new_value)
	{
		return (T) atomic_swap32((volatile Int32*) &value, (Int32) new_value);
	}
	static inline T	Swap(volatile T* v, T new_value)
	{
		return (T) atomic_swap32((volatile Int32*) v, (Int32) new_value);
	}
	inline T SwapAdd(T add)
	{
		return (T) atomic_swap_add32((volatile Int32*) &value, (Int32) add);
	}
	inline T SwapIncrement()
	{
		return (T) atomic_swap_add32((volatile Int32*) &value, 1);
	}
	inline T SwapDecrement()
	{
		return (T) atomic_swap_add32((volatile Int32*) &value, -1);
	}
	static inline T	SwapAdd(volatile T* v, T add)
	{
		return (T) atomic_swap_add32((volatile Int32*) v, (Int32) add);
	}
	static inline T	SwapIncrement(volatile T* v)
	{
		return (T) atomic_swap_add32((volatile Int32*) v, 1);
	}
	static inline T	SwapDecrement(volatile T* v)
	{
		return (T) atomic_swap_add32((volatile Int32*) v, -1);
	}
	static inline T	SwapIncrement(Atomic32* v)
	{
		return (T) atomic_swap_add32(&v->value, 1);
	}
	static inline T	SwapDecrement(Atomic32* v)
	{
		return (T) atomic_swap_add32(&v->value, -1);
	}
	inline void	UnsafeAdd(T add)
	{
		value += add;
	}
	inline void	UnsafeIncrement()
	{
		value += 1;
	}
	inline void	UnsafeDecrement()
	{
		value -= 1;
	}
};

typedef Atomic32<Int32>		AtomicLONG;
typedef Atomic32<UInt32>	AtomicULONG;
#ifndef MAXON_TARGET_64BIT
typedef Atomic32<Int>		AtomicVLONG;
typedef Atomic32<UInt>	AtomicVULONG;
#endif


//----------------------------------------------------------------------------------------
// Atomic 64 bit integer class
// CompareAndSwap() returns the previous value and changes the memory location if it's the same as the comparison value
// SwapAdd(), SwapIncrement() and SwapDecrement() return the previous value
//----------------------------------------------------------------------------------------
#ifdef MAXON_TARGET_64BIT
template <class T> class Atomic64
{
	volatile T value;

public:
	inline T Get()
	{
		return value;
	}
	inline void	Set(T new_value)
	{
		value = new_value;
	}
	inline T CompareAndSwap(T new_value, T compare)
	{
		return (T) atomic_cas64((volatile Int64*) &value, (Int64) new_value, (Int64) compare);
	}
	inline Bool	TryCompareAndSwap(T new_value, T compare)
	{
		return atomic_cas64((volatile Int64*) &value, (Int64) new_value, (Int64) compare) == (Int64) compare;
	}
	inline T Swap(T new_value)
	{
		return (T) atomic_swap64((volatile Int64*) &value, (Int64) new_value);
	}
	static inline T	Swap(volatile T* v, T new_value)
	{
		return (T) atomic_swap64((volatile Int64*) v, (Int64) new_value);
	}
	inline T SwapAdd(T add)
	{
		return (T) atomic_swap_add64((volatile Int64*) &value, (Int64) add);
	}
	inline T SwapIncrement()
	{
		return (T) atomic_swap_add64((volatile Int64*) &value, 1);
	}
	inline T SwapDecrement()
	{
		return (T) atomic_swap_add64((volatile Int64*) &value, -1);
	}
	static inline T	SwapAdd(volatile T* v, T add)
	{
		return (T) atomic_swap_add64((volatile Int64*) v, add);
	}
	static inline T	SwapIncrement(volatile T* v)
	{
		return (T) atomic_swap_add64((volatile Int64*) v, 1);
	}
	static inline T	SwapDecrement(volatile T* v)
	{
		return (T) atomic_swap_add64((volatile Int64*) v, -1);
	}
	static inline T	SwapIncrement(Atomic64* v)
	{
		return (T) atomic_swap_add64(&v->value, 1);
	}
	static inline T	SwapDecrement(Atomic64* v)
	{
		return (T) atomic_swap_add64(&v->value, -1);
	}
	inline void	UnsafeAdd(T add)
	{
		value += add;
	}
	inline void	UnsafeIncrement()
	{
		value += 1;
	}
	inline void	UnsafeDecrement()
	{
		value -= 1;
	}
};

typedef Atomic64<Int64>		AtomicInt64;
typedef Atomic64<UInt64>	AtomicUInt64;
typedef Atomic64<Int>			AtomicInt;
typedef Atomic64<UInt>		AtomicUInt;

#endif

#pragma pack (pop)
}

#endif

#endif	// PRIVATE_GE_SYS_ATOMIC_H__
