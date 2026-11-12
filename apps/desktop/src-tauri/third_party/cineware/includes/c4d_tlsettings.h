/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TLSETTINGS_H__
#define C4D_TLSETTINGS_H__

#include "c4d_basesettings.h"
#include "c4d_basetime.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

// Time Line settings
class TLSettings : public BaseSettings
{
public:
	//IndividualModeClass m_Individual;
	String						m_sSoundName;
	BaseTime					m_tStartFrame, m_tEndFrame, m_tDisSnap;
	VisualisationMode m_VisMode;
	Float							m_GridValue;
	Float							m_rFCScale, m_rFCMove, m_rDisSnap;
	BaseLink*					m_SoundTrack;
	GRID_TYPE					m_GridType;
	Int32							m_lSnap, m_lFlags, m_lHLEType, m_lFlags2, m_lMVSettings;
	Int32							m_MarkerCount;
	Bool							m_bLoaded;

	static NodeData* Alloc()
	{
		return NewObj(TLSettings);
	}

	virtual Bool BS_Init();
	virtual void BS_Free()
	{
		if (m_SoundTrack)
			BaseLink::Free(m_SoundTrack);

		m_SoundTrack = nullptr;
	}
	virtual Bool BS_CopyTo(BaseSettings* dest);
	virtual Bool BS_ReadfromContainer(BaseContainer* bc, BaseDocument* doc);
	virtual Bool BS_WriteToContainer(BaseContainer* bc, BaseDocument* doc);
};

#pragma pack (pop)
}

#endif	// C4D_TLSETTINGS_H__
