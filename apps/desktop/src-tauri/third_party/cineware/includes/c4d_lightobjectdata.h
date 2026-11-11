/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LIGHTOBJECTDATA_H__
#define C4D_LIGHTOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_vnstruct.h"
#include "c4d_vlstruct.h"
#include "c4d_shadowstruct.h"
#include "c4d_lfstruct.h"
#include "c4d_causticstruct.h"
#include "c4d_ieslightdata.h"

namespace cineware
{
#pragma pack (push, 8)

class IESLight;

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Light object.
//----------------------------------------------------------------------------------------
class LightObjectData : public NodeData
{
	INSTANCEOF(LightObjectData, NodeData)

	friend Bool SetIESLight(BaseObject* op, IESLight* data, BaseDocument* doc);

protected:
	Float	outerradius;	// General page
	Int32	falloff;			// Detail page
	Float	outerdist;		// Detail page

private:
	// General page
	Vector f;
	Float	 bright;
	Int32	 type, stype, vtype, ntype;
	Bool	 showli, showvl, showclip, dark;

	Float	 innerangle, outerangle;
	Float	 innerradius;

	// Detail page
	Bool					ambient, nodiffuse, nospecular, innercone, innercolor, colfalloff, separatepass;
	Float					contrast, aspectratio;
	Float					innerdist;
	Bool					nearclip, farclip;
	Float					nearfrom, nearto, farfrom, farto;

	VNStruct			vn;
	VLStruct			vl;
	ShadowStruct	shadow;
	LFStruct			lf;
	CausticStruct	caustic;

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	Int32 GetChunkLevel();

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	LightObjectData();

	//----------------------------------------------------------------------------------------
	/// Destructor.
	//----------------------------------------------------------------------------------------
	~LightObjectData();

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the light object data from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @param[in] id									The type ID of the light object to read.
	/// @param[in] level							The disklevel of the object to read.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Writes the light object data to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}

	IESLightData ieslightdata;	///< Pointer to the IES light data wrapper.
};

#pragma pack (pop)
}

#endif	// C4D_LIGHTOBJECTDATA_H__
