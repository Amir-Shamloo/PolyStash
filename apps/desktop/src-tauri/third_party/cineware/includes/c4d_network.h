/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_NETWORK_H__
#define C4D_NETWORK_H__

#include "c4d_file.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup group_c4d_network_network Network
/// @ingroup group_topic Topics
/// @{

class IpConnection;

//----------------------------------------------------------------------------------------
/// Initializes the network.
/// @note	Must be called before using the network functions.
/// @return												@em 0 if there was an error and @em 1 for a successful initialization.
//----------------------------------------------------------------------------------------
Int32 GeIpInitNetwork();

//----------------------------------------------------------------------------------------
/// Closes the network.
/// @note	Must be called after using the network functions.
//----------------------------------------------------------------------------------------
void GeIpCloseNetwork();

//----------------------------------------------------------------------------------------
/// Opens an IP listener.
/// @param[in] ipAddr							The IP address of the network interface to use for the listener, or @em 0 to use the default interface.
/// @param[in] port								The port of the listener.
/// @param[in] thread							The thread to use for the connection. @callerOwnsPointed{thread}
/// @param[in] timeout						The time after which the listener closes an unresponsive connection.
/// @param[in] dontwait						If @formatConstant{true}, the TCP/IP flag @em TCP_NODELAY is set.\n
///																<i>Quote from the WinSock documentation:</i>\n
///																"The TCP_NODELAY option disables the Nagle algorithm. The Nagle algorithm is used to reduce the number of small packets sent by a host by buffering unacknowledged send data until a full-size packet can be sent.\n
///																However, for some applications this algorithm can impede performance, and TCP_NODELAY may be used to turn it off. Application writers should not set TCP_NODELAY unless the impact of doing so is well-understood and desired, since setting TCP_NODELAY can have a significant negative impact of network performance.\n
///																TCP_NODELAY is the only supported socket option which uses level IPPROTO_TCP; all other options use level SOL_SOCKET."
/// @param[in] ferr								The function error status. Assigned a value different than @em 0 to indicate failure.
/// @return												The IP connection handle for the listener. Must be freed with GeIpCloseConnection(). @callerOwnsPointed{IpConnection}
//----------------------------------------------------------------------------------------
IpConnection* GeIpOpenListener(UInt32 ipAddr, Int32 port, BaseThread* thread, Int32 timeout, Bool dontwait, Int32* ferr);

//----------------------------------------------------------------------------------------
/// Waits for an incoming connection to listener.
/// @param[in] listener						The listener IP connection to wait for. Needs to be freed with GeIpCloseConnection(). @callerOwnsPointed{IpConnection}
/// @param[in] connection					The thread to use for the connection. @callerOwnsPointed{thread}
/// @param[in] ferr								The function error status. Assigned a value different than @em 0 to indicate failure.
/// @return												The IP connection handle for the waiting connection. Must be freed with GeIpCloseConnection(). @callerOwnsPointed{IpConnection}
//----------------------------------------------------------------------------------------
IpConnection* GeIpWaitForIncoming(IpConnection* listener, BaseThread* connection, Int32* ferr);

//----------------------------------------------------------------------------------------
/// Opens an outgoing IP connection.
/// @param[in] hostname						The hostname to connect to. Should include port, e.g. @em "www.maxon.de:80" or @em "192.168.0.37:80".
/// @param[in] thread							The thread to use for the connection. @callerOwnsPointed{thread}
/// @param[in] initial_timeout		The timeout for creating the connection.
/// @param[in] timeout						The timeout for the data transfer.
/// @param[in] dontwait						If @formatConstant{true}, the TCP/IP flag @em TCP_NODELAY is set.\n
///																<i>Quote from the WinSock documentation:</i>\n
///																"The TCP_NODELAY option disables the Nagle algorithm. The Nagle algorithm is used to reduce the number of small packets sent by a host by buffering unacknowledged send data until a full-size packet can be sent.\n
///																However, for some applications this algorithm can impede performance, and TCP_NODELAY may be used to turn it off. Application writers should not set TCP_NODELAY unless the impact of doing so is well-understood and desired, since setting TCP_NODELAY can have a significant negative impact of network performance.\n
///																TCP_NODELAY is the only supported socket option which uses level IPPROTO_TCP; all other options use level SOL_SOCKET."
/// @param[in] ferr								The function error status. Assigned a value different than @em 0 to indicate failure.
/// @param[in] retries						Defines how many times the function tries to connect.
/// @return												IP connection handle for the outgoing connection. Must be freed with GeIpCloseConnection(). @callerOwnsPointed{IpConnection}
//----------------------------------------------------------------------------------------
IpConnection* GeIpOpenOutgoing(const Char* hostname, BaseThread* thread, Int32 initial_timeout, Int32 timeout, Bool dontwait, Int32* ferr, Int32 retries);

//----------------------------------------------------------------------------------------
/// Closes and frees an IP connection.
/// @param[in] ipc								The IP connection to close. Assigned @formatConstant{nullptr} afterwards.
//----------------------------------------------------------------------------------------
void GeIpCloseConnection(IpConnection*& ipc);

//----------------------------------------------------------------------------------------
/// Kills an IP connection.
/// @note	This function calls End() for the connection thread.
/// @param[in] ipc								The IP connection to kill. @callerOwnsPointed{IpConnection}
//----------------------------------------------------------------------------------------
void GeIpKillConnection(IpConnection* ipc);

//----------------------------------------------------------------------------------------
/// Checks how many bytes are in the input buffer of a IP connection.
/// @param[in] ipc								The IP connection to check. @callerOwnsPointed{IpConnection}
/// @return												The number of bytes in the input buffer.
//----------------------------------------------------------------------------------------
Int32 GeIpBytesInInputBuffer(IpConnection* ipc);

//----------------------------------------------------------------------------------------
/// Reads bytes from an IP connection.
/// @param[in] ipc								The IP connection to read from. @callerOwnsPointed{IpConnection}
/// @param[in] buf								The buffer to read to. @callerOwnsPointed{buffer}
/// @param[in] size								The size of the buffer.
/// @return												The number of bytes read.
//----------------------------------------------------------------------------------------
Int GeIpReadBytes(IpConnection* ipc, void* buf, Int size);

//----------------------------------------------------------------------------------------
/// Sends bytes to a IP connection.
/// @param[in] ipc								The IP connection to send to. @callerOwnsPointed{IpConnection}
/// @param[in] buf								The buffer to send from. @callerOwnsPointed{buffer}
/// @param[in] size								The size of the buffer.
/// @return												The number of bytes sent.
//----------------------------------------------------------------------------------------
Int GeIpSendBytes(IpConnection* ipc, void* buf, Int size);

//----------------------------------------------------------------------------------------
/// Gets the host address of a IP connection.
/// @param[in] ipc								The IP connection. @callerOwnsPointed{IpConnection}
/// @param[in] buf								The buffer to take the host address. @callerOwnsPointed{buffer}
/// @param[in] bufsize						The size of the buffer.
//----------------------------------------------------------------------------------------
void GeIpGetHostAddr(IpConnection* ipc, Char* buf, Int32 bufsize);

//----------------------------------------------------------------------------------------
/// Gets the remote address of a IP connection.
/// @param[in] ipc								The IP connection. @callerOwnsPointed{IpConnection}
/// @param[in] buf								The buffer to take the remote address.@callerOwnsPointed{buffer}
/// @param[in] bufsize						The size of the buffer.
//----------------------------------------------------------------------------------------
void GeIpGetRemoteAddr(IpConnection* ipc, Char* buf, Int32 bufsize);

//----------------------------------------------------------------------------------------
/// Random sleep timer.
/// @param[in] min								The minimum time in milliseconds.
/// @param[in] max								The maximum time in milliseconds.
//----------------------------------------------------------------------------------------
void GeRandomSleep(Int32 min, Int32 max);

//----------------------------------------------------------------------------------------
/// @markUnused
//----------------------------------------------------------------------------------------
String GetTimeString();

//----------------------------------------------------------------------------------------
/// @markUnused
//----------------------------------------------------------------------------------------
Bool GetMACaddress(UChar* buf, Int32 bufsize);

/// @}

#pragma pack (pop)
}

#endif	// C4D_NETWORK_H__
