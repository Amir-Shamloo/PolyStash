/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_GE_THREAD_H__
#define PRIVATE_GE_THREAD_H__

#ifdef MAXON_TARGET_WINDOWS
	#include "private_ge_win_thread.h"
#elif defined(MAXON_TARGET_OSX) || defined(MAXON_TARGET_IOS)
	#include "private_mac_pthread.h"
#elif defined MAXON_TARGET_LINUX
	#include "private_linux_pthread.h"
#elif defined MAXON_TARGET_ANDROID
	#include "private_android_pthread.h"
#else
	#error "Unkown OS"
#endif

namespace cineware
{
#pragma pack (push, 8)

Int32 GeGetTimer();

#pragma pack (pop)
}

#endif	// PRIVATE_GE_THREAD_H__
