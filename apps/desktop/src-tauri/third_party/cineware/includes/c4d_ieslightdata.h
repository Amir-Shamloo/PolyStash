/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_IESLIGHTDATA_H__
#define C4D_IESLIGHTDATA_H__

namespace cineware
{
#pragma pack (push, 8)

class IESLight;

//----------------------------------------------------------------------------------------
/// Wrapper for IESLight.
//----------------------------------------------------------------------------------------
struct IESLightData
{
	IESLight*	ieslight;	///< Pointer to the IES light information.

	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	IESLightData();

	//----------------------------------------------------------------------------------------
	/// Copy constructor.
	//----------------------------------------------------------------------------------------
	IESLightData(const IESLightData& Data);

	//----------------------------------------------------------------------------------------
	/// Destructor.
	//----------------------------------------------------------------------------------------
	~IESLightData();

	/// @}

	/// @name Operator
	/// @{

	//----------------------------------------------------------------------------------------
	/// Assignment operator.
	/// @param[in] Data								The left-hand operand IES light data to assign.
	/// @return												Right-hand operand assigned @formatParam{Data}.
	//----------------------------------------------------------------------------------------
	const IESLightData& operator =(const IESLightData& Data);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_IESLIGHTDATA_H__
