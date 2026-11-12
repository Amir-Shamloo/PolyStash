/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef __SYSTHREAD_H
	#define __SYSTHREAD_H
#endif

#ifndef PRIVATE_MAC_PTHREAD_H__
	#define PRIVATE_MAC_PTHREAD_H__

	#include "private_symbols.h"
	#include "private_ge_thread.h"
	#include <sched.h>	// sched_yield()
	#include "private_ge_sys_atomic.h"
	#include "c4d_filename.h"
	#include "c4d_memory.h"

namespace cineware
{
#pragma pack (push, 8)

Bool GeExecute(const Filename& prog, const String& param);
Bool GeExecuteC4D(const Filename& prog, const Filename& scene_file, Bool merge);
Bool GeExecuteC4DArguments(const Filename& prog, const Filename& file, const String& arguments);

class BaseWindow;
class BaseThread;


#ifndef __GE_SPINLOCK
		#define __GE_SPINLOCK

struct GeSpinlock;
extern void	xGeSpinLock(GeSpinlock* lock);

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
	inline Bool	AttemptLock()
	{
#ifdef MAXON_TARGET_IOS
		return true;
#else
		return atomic_cas32(&state, 1, 0) == 0;	// return TRUE if we locked successfully
#endif
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
		asm volatile ("" : : : "memory");
#endif
		state = 0;
	}
#endif

};

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

#endif


void GeInitThreads();
void GeEndThreads();

Bool GeIsMainThread();
UInt32 GeGetCurrentThreadId();
UInt32 GeGetCurrentProcessId();
BaseThread* GeGetCurrentThread();

void GeSetHighThreadPriority(Bool high);
void GeSleep(Int32 milliseconds);
inline void	GeYieldCpu()
{
	sched_yield();
}

void GeThreadLock();
void GeThreadUnLock();


struct GeRegion
{
#ifndef MAXON_TARGET_IOS
	pthread_mutex_t mutex;
	pthread_t				owner;
#endif
	BaseThread*			bt;
	Int32						level;
};

typedef GeRegion* GeRegionPtr;

GeRegionPtr GeAllocRegion();
void GeFreeRegion(GeRegionPtr r);
Bool GeLockRegion(GeRegionPtr r);
Bool GeUnLockRegion(GeRegionPtr r);
Bool GeForceUnlockRegion(GeRegionPtr r);
UInt32 GeGetRegionOwner(GeRegionPtr r);

class BaseThread
{
	friend class IpKillThread;

private:
	BaseThread* prev;
	BaseThread* next;

	void*				th;					// pthread_t
	UInt32			state;			// was id
	UInt32			thread_id;	// was pid;
	Int32				Prio;

	void*				userData;

	//		void	*crash_info;	// CrashInfo

	void* ktr_signal;
	Bool	stop_thread;	// was LONG StopIt;

	// state
	enum
	{
		NotRunning	= 0,	// no valid pthread id (th)
		Stopping		= 1,	// pthread was asked to stop itself
		Sleeping		= 2,	// pthread is sleeping
		Starting		= 3,	// pthread has been created an is being started
		Running			= 4,	// pthread is running
		Synchronous = 5		// synchronously running thread
	};

public:
	Bool keepthreadrunning;

protected:
	virtual void Main() = 0;
	virtual const Char* GetThreadName() = 0;
	void Init();
	void StopAsync();

public:
	BaseThread();
	~BaseThread();

	void SetUserData(void* data)
	{
		userData = data;
	}
	void* GetUserData()
	{
		return userData;
	}

	virtual Int32 TestBreak();

	virtual Int32 Start(Bool back, Int32 Priority = THREADPRIORITY_NORMAL);
	virtual void End(Bool wait = true);

	virtual void Wait();
	virtual Bool IsRunning();
	virtual Bool ItsMe();

	virtual Int32 ChangeThreadPriority();

	void Action();
	void* GetCrashInfo();
	UInt32 GetThreadId();

	static BaseThread* GetCurrentThread();
	static UInt32	GetCurrentThreadId();
	static Bool	StopAllThreads();

	// thread_id
	enum
	{
		UnknownThreadId = 0,
		MainThreadId		= 1,	// main thread id must not be zero, otherwise the OpenGL thread would cause a deadlock in BaseThread::Wait()
		FirstThreadId		= 256
	};

};	//BaseThread

class C4DSignal
{
private:
	void* signal;

public:
	C4DSignal();
	~C4DSignal();

	Bool Init();

	void Set();
	Bool Wait(Int32 timeout);	// returns TRUE in case of timeout
};

#pragma pack (pop)
}

#endif	// PRIVATE_MAC_PTHREAD_H__

