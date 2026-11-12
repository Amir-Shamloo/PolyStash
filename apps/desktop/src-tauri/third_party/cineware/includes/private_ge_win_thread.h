/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_GE_WIN_THREAD_H__
#define PRIVATE_GE_WIN_THREAD_H__

#include "c4d_system.h"
#include "private_ge_thread.h"
#include "private_ge_sys_atomic.h"
#include "private_symbols.h"
#include "c4d_filename.h"
#include "c4d_memory.h"

namespace cineware
{
#pragma pack (push, 8)

class String;
class Filename;

Bool GeExecute(const Filename& prog, const Filename& param);
Bool GeExecuteC4D(const Filename& prog, const Filename& scene_file, Bool merge);
Bool GeExecuteC4DArguments(const Filename& prog, const Filename& file, const String& arguments);

class BaseWindow;
class BaseThread;

struct GeSpinlock;
extern void	xGeSpinLock(GeSpinlock* lock);

//////////////////////////////////////////////////////////////////////////

struct GeSpinlock
{
	GeSpinlock() : state(0) { }

	volatile Int32 state;

#ifdef MAXON_TARGET_DEBUG

	Bool AttemptLock();
	void Lock();
	void Unlock();

#else	// inline frequent cases, if the lock is taken it's just one instruction instead of a subroutine call

	// try to lock (furthermore memory barrier if successful)
	inline Bool AttemptLock()
	{
		return atomic_cas32(&state, 1, 0) == 0;	// return true if we locked successfully
	}

	inline void	Lock()
	{
		if (AttemptLock() == false)
		{
			xGeSpinLock(this);	// spin and pause handling is done in external function
		}
	}

	inline void	Unlock()
	{
#if (defined __ppc__) || (defined __ppc64__)
		asm volatile ("lwsync" : : : "memory");	// complete prior stores before unlock
#else
		_ReadWriteBarrier();										// make sure that the compiler doesn't move the following store to some point before the intended unlock
#endif
		state = 0;
	}
#endif

};

//////////////////////////////////////////////////////////////////////////

struct GeRWSpinlock
{
	GeRWSpinlock() : state(0) { }

	volatile Int32 state;

	void ReadLock();
	void ReadUnlock();
	void WriteLock();
	void WriteUnlock();
	Bool AttemptWriteLock();
};

void GeInitThreads();
void GeEndThreads();

Bool GeIsMainThread();
UInt32 GeGetCurrentThreadId();
UInt32 GeGetCurrentProcessId();
BaseThread*	GeGetCurrentThread();

void GeSetHighThreadPriority(Bool high);
void GeSleep(Int32 milliseconds);
void GeYieldCpu();

void GeThreadLock();
void GeThreadUnLock();

struct GeRegion;
typedef GeRegion* GeRegionPtr;

GeRegionPtr GeAllocRegion();
void GeFreeRegion(GeRegionPtr r);
Bool GeLockRegion(GeRegionPtr r);
Bool GeUnLockRegion(GeRegionPtr r);
Bool GeForceUnlockRegion(GeRegionPtr r);
UInt32 GeGetRegionOwner(GeRegionPtr r);

//////////////////////////////////////////////////////////////////////////

class BaseThread
{
	friend cineware::BaseThread* GeGetCurrentThread();
	friend void BeginThread(void* tp);

private:
	UInt32 state;	// was id
	UInt32 pid;
	void*	 th;
	Int32	 stop_thread;						// was StopIt
	Int32	 ktr_exit_pthread_flag;	// was StopIt2
	void*	 userData;
	void*	 ktr_signal;

protected:
	virtual void Main() = 0;
	virtual const Char* GetThreadName() = 0;

public:
	BaseThread();
	virtual ~BaseThread();

	void SetUserData(void* data)
	{
		userData = data;
	}
	void* GetUserData()
	{
		return userData;
	}

	virtual Int32 TestBreak();
	virtual Bool IsRunning();
	virtual void Wait();
	virtual void End(Bool wait = true);
	virtual Bool ItsMe();	// check if the current running thread is used of this class

	virtual Int32 Start(Bool back, Int32 priority = THREADPRIORITY_NORMAL);

	virtual Int32 ChangeThreadPriority();
};	//BaseThread

//////////////////////////////////////////////////////////////////////////

#pragma pack (pop)
}

#endif	// PRIVATE_GE_WIN_THREAD_H__
