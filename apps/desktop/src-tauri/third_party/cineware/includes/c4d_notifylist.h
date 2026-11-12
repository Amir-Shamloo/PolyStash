/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_NOTIFYLIST_H__
#define C4D_NOTIFYLIST_H__

#include "private_ge_thread.h"
#include "c4d_parameter_ids.h"
#include "c4d_baselink.h"
#include "c4d_basecontainer.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class NotifyData
{
public:
	NotifyData() { m_pLink = nullptr; m_ID = NOTIFY_EVENT_NONE; m_Flags = NOTIFY_EVENT_FLAG_0; }
	~NotifyData() { DeleteObj(m_pLink); }

	NOTIFY_EVENT			m_ID;
	BaseLink*					m_pLink;
	BaseContainer			m_Data;
	NOTIFY_EVENT_FLAG m_Flags;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class NotifyList
{
public:
	NotifyList() { m_InSend = 0; }
	~NotifyList() { m_List.Free(); }

	//	Bool AddEvent(BaseList2D *bl, NOTIFY_EVENT eventid, NOTIFY_EVENT_FLAG flags, const BaseContainer *data);
	//	Bool RemoveEvent(BaseDocument *doc, BaseList2D *bl, NOTIFY_EVENT eventid);
	//	Bool FindEvent(BaseDocument *doc, BaseList2D *bl, NOTIFY_EVENT eventid);
	//	void SendNotify(BaseDocument *doc, BaseList2D *eventbl, NOTIFY_EVENT eventid, void *edata);
	Bool CopyTo(NotifyList* dst, COPYFLAGS flags, AliasTrans* trans);
	void Read(HyperFile* hf);
	void Write(HyperFile* hf);

	GeSpinlock									m_Lock;
	DynamicArray<NotifyData, 8> m_List;
	Int32												m_InSend;
};

#pragma pack (pop)
}

#endif	// C4D_NOTIFYLIST_H__
