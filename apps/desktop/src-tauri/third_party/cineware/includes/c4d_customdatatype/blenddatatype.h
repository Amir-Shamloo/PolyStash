/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef BLENDDATATYPE_H__
#define BLENDDATATYPE_H__

#include "blendlayers.h"
#include "c4d_system.h"
#include "c4d_file.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Custom data type holding the Layer Shader layers.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class iBlendDataType : public iCustomDataType<iBlendDataType>
{
public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	iBlendDataType();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~iBlendDataType();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32 GetNextID();

	/// @}

	CBlendLayers m_BlendLayers;	///< Layers list.

	Int32				 m_lLastID;			///< @markPrivate
};

//----------------------------------------------------------------------------------------
/// @markPrivate @markInternal
//----------------------------------------------------------------------------------------
class BlendDataTypeClass
{
public:
	static Int32 GetId();

	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData(CustomDataType* d, HyperFile* hf, Int32 level);

	static Bool GetParameter(const CustomDataType* data, const DescID& id, GeData& t_data, Int32& flags);
	static Bool SetDParameter(CustomDataType* data, const DescID& id, const GeData& t_data, Int32& flags);
};

#pragma pack (pop)
}

#endif	// BLENDDATATYPE_H__
