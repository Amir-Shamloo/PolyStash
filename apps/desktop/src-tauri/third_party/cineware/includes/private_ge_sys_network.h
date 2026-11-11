/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef PRIVATE_GE_SYS_NETWORK_H__
#define PRIVATE_GE_SYS_NETWORK_H__

#include "private_ge_thread.h"

namespace cineware
{
#pragma pack (push, 8)

#ifdef MAXON_TARGET_WINDOWS
	typedef UInt SOCKET;
#else
	typedef int SOCKET;
#endif

class BaseThread;

enum ConnectionType
{
	IpIsListener,
	IpIsIncoming,
	IpIsOutgoing
};

class IpConnection
{
public:
	IpConnection(BaseThread* bt, ConnectionType t, Int32 t_timeout, Bool t_dontwait);
	~IpConnection();

	int SetAddresses();

	ConnectionType type;
	Int32					 ltime;

	BaseThread*		 thread;
	SOCKET				 sock;

	unsigned int	 host_addr;
	int						 host_port;
	unsigned int	 remote_addr;
	int						 remote_port;

	Int32					 timeout;
	Bool					 dontwait;

	IpConnection*	 prev;
	IpConnection*	 next;
};

#pragma pack (pop)
}

#endif	// PRIVATE_GE_SYS_NETWORK_H__
