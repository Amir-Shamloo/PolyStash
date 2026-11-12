/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMDATATYPE_SKYOBJECTS_H__
#define CUSTOMDATATYPE_SKYOBJECTS_H__

#include "c4d_customdatatype.h"
#include "c4d_filename.h"
#include "private_symbols.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class DescID;
class GeData;
class BaseBitmap;
class AliasTrans;
class HyperFile;

#define SKY_II_SKY_OBJECTS_DATA 450000019

///////////////////////////////////////////////////////////////////////////////////////////////////

#define SKY_OBJCTS_DISK_LEVEL		2
#define SKY_OBJCTS_PREVIEW_SIZE 20

#define SKY_II_COMMAND_CREATE_SKY		1011145
#define SKY_II_SKY_OBJECT						1011146
#define SKY_II_SETTINGS_DIALOG			1011147
#define Mskysurface 1011149
#define SKY_II_SKY_VOLUME_MATERIAL	1011151
#define SKY_II_VIDEO_POST						1011144
#define SKY_II_PREFS								1011143
#define SKY_II_PRESET_DATA_TYPE			1011159
#define SKY_II_SCENE_HOOK						1011184
#define SKY_II_CLOUD_GROUP_NODE			1011194
#define SKY_II_CLOUD_NODE						1011196
#define SKY_II_CLOUD_GROUP_DIALOG		1011195
#define CUSTOMDATA_ACTIVE_GROUP			1011190
#define CUSTOMGUI_ACTIVE_GROUP			1011193
#define SKY_II_SET_OBJECT_TOOL			450000017
#define SKY_II_SKY_OBJECTS_GUI			450000018
#define SKY_II_CLOUD_TOOL						450000023
#define SKY_II_REALREADONLY					450000024
#define SKY_II_OBJECT								450000028
#define SKY_II_CITY_LIST						450000047
#define SKY_II_BROWSER_COMMAND			450000050
#define SKY_II_PRESET_DIALOG				/*450000051*/ 1011148
#define SKY_II_CONNECT_CLOUDS				450000057
#define SKY_II_CREATE_CLOUD_NODE		450000058
#define SKY_II_CREATE_GROUP_NODE		450000059
#define SKY_II_DIRECTORY_NODE				450000066

class SkyObject
{
public:
	SkyObject();
	virtual ~SkyObject();

	Float				m_rAltitude, m_rAzimuth, m_rAngle, m_rIntens;
	String			m_strName;
	Filename		m_fnImageName, m_fnImagePath;
	Bool				m_bIsSelected, m_bIlluminate;
	BaseBitmap* m_pPreview;
	Int32				m_lObjectID;

	SkyObject*	m_pNext;	// Only used in the tree view
	BaseBitmap* m_pRenderImage;
	Float				x, y, z, m_rMaxXYDelta;
	UChar*			m_pChShow;
	Vector			m_vRelSun, m_vUp, m_vRight, m_vNormedPos;

	// Only used for rendering
	Vector m_pvColor[MAX_THREADS * 2];
	Float	 m_prAlpha[MAX_THREADS * 2], m_prLight[MAX_THREADS * 2];

	void*	 m_pPreviewData;
	Int32	 m_lPreviewSize;
};

class SkyObjects : public CDynamicObjectArray<SkyObject>
{
public:
	SkyObjects();
	virtual ~SkyObjects();

	SkyObject* SearchObject(Int32 lID);
	Int32 m_lNewObjectID;

protected:
	virtual SkyObject* AllocObject() const;
	virtual void FreeObject(SkyObject*& pObj) const;

	virtual Bool ReadElementHF(HyperFile* pFile, SkyObject* pElement, Int32 lLevel);
	virtual Bool WriteElementHF(HyperFile* pFile, const SkyObject* pElement) const;
	virtual SkyObject* GetClone(const SkyObject* pObj, AliasTrans* pTrans) const;
	virtual Bool TestEqual(const SkyObject* pObjA, const SkyObject* pObjB) const;
};

class SkyObjectData : public CustomDataType
{
};

/// @markInternal
class SkyObjectsDataTypeClass
{
public:
	SkyObjectsDataTypeClass();

	static Int32 GetId();

	static CustomDataType*	AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* pAliasTrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* d, HyperFile* pFile);
	static Bool ReadData (CustomDataType* d, HyperFile* pFile, Int32 lLevel);

	static Bool GetParameter(const CustomDataType* data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags);
	static Bool SetDParameter(CustomDataType* data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags);
};

#pragma pack (pop)

}

#endif	// CUSTOMDATATYPE_SKYOBJECTS_H__
