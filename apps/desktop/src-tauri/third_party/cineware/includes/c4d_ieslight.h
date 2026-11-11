/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_IESLIGHT_H__
#define C4D_IESLIGHT_H__

#include "c4d_string.h"
#include "c4d_drawport.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;
class Filename;
class BaseObject;
class BaseDocument;

/// @addtogroup IESSymbols
/// @ingroup group_enumeration
/// @{
/// @markPrivate
enum IESSymbols
{
	kIESSymbolUnknown = 0,
	kIESSymbolVersion1991,
	kIESSymbolVersion1995,
	kIESSymbolVersion2002,
	kIESSymbolTilt,
	kIESSymbolTiltNone,
	kIESSymbolTiltInclude,
	kIESSymbolInfoManufac,
	kIESSymbolInfoLumCat,
	kIESSymbolInfoLuminaire,
	kIESSymbolInfoLampCat,
	kIESSymbolInfoLamp,
};
/// @}

/// IES light file version.
#define IES_LIGHT_FILE_VERSION 0x04

/// IES light.
class IESLight
{
public:
	/// @addtogroup IESVERSION
	/// @ingroup group_enumeration
	/// @{
	/// IES light file version.
	enum IESVERSION
	{
		V1986 = 0,	///< 1986
		V1991,			///< 1991
		V1995,			///< 1995
		V2002,			///< 2002
	};
	/// @}

	/// @addtogroup IESTILT
	/// @ingroup group_enumeration
	/// @{
	/// IES light tilt type.
	enum IESTILT
	{
		UNKNOWN = 0,	///< Unknown.
		NOTILT,				///< No tilt.
		INCLUDE,			///< Include tilt angles.
		FILE					///< Tilt data file.
	};
	/// @}

	IESVERSION m_Version;	///< IES light file version.
	IESTILT		 m_Tilt;		///< IES light tilt type.

	// Info
	String m_szInfoManufacturer;		///< Luminaire manufacturer.
	String m_szInfoLumCatalog;			///< Luminaire catalog number.
	String m_szInfoLumDescription;	///< Luminaire description.
	String m_szInfoLampCatalog;			///< Lamp catalog number.
	String m_szInfoLampDescription;	///< Lamp description.

	// Size
	Int32	 m_nAreaShape;	///< Luminaire shape.
	Vector m_vAreaSize;		///< Luminaire size.

	// Angles
	Float* m_pAnglesVertical;		///< Array of vertical angles.
	Float* m_pAnglesHorizontal;	///< Array of horizontal angles.

	Int32	 m_nAnglesVertical;		///< Number of vertical angles.
	Int32	 m_nAnglesHorizontal;	///< Number of horizontal angles.

	// Data
	Float* m_pData;				///< @markPrivate
	Float	 m_fDataMax;		///< @markPrivate
	Float	 m_fDataPower;	///< @markPrivate

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	IESLight();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~IESLight();

	/// @}

	/// @name Copy
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the IES light.
	/// @return												The cloned IES light, or @formatConstant{nullptr} if an error occurred. @callerOwnsPointed{IES light}
	//----------------------------------------------------------------------------------------
	IESLight* Copy();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the IES light from a hyper file.
	/// @param[in] pHFile							The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* pHFile);

	//----------------------------------------------------------------------------------------
	/// Writes the IES light to a hyper file.
	/// @param[in] pHFile							The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* pHFile);

	/// @}

	/// @name Import from File
	/// @{

	//----------------------------------------------------------------------------------------
	/// Imports a IES light from a file.
	/// @param[in] fn									The filename of the IES light file to import.
	/// @return												The result.
	//----------------------------------------------------------------------------------------
	IMAGERESULT ImportFromFile(const Filename& fn);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void ComputePowerRatio();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Float64 ComputeLightIntensity(Vector64 vDir);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Float64 ATan2(Float64 fY, Float64 fX);

	/// @}
};

/// @addtogroup group_c4d_file_ies_light IES Light
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Sets IES data for a light object.
/// @param[in] op									The light object. @callerOwnsPointed{light object}
/// @param[in] data								The IES data to set. @callerOwnsPointed{light data}
/// @param[in] doc								The document of the light object. @callerOwnsPointed{document}
/// @return												@trueIfOtherwiseFalse{the IES light data was successfully set}
//----------------------------------------------------------------------------------------
Bool SetIESLight(BaseObject* op, IESLight* data, BaseDocument* doc);

//----------------------------------------------------------------------------------------
/// Gets the IES data for a light object.
/// @param[in] op									The light object.@callerOwnsPointed{light object}
/// @return												The IES data for the light object @formatParam{op}.
//----------------------------------------------------------------------------------------
IESLight* GetIESLight(BaseObject* op);

/// @}

#pragma pack (pop)
}

#endif	// C4D_IESLIGHT_H__
