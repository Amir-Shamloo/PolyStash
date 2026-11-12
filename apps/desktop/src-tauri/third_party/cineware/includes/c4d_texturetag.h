/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_TEXTURETAG_H__
#define C4D_TEXTURETAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Texture tag.
//----------------------------------------------------------------------------------------
class TextureTag : public BaseTag
{
	INSTANCEOF(TextureTag, BaseTag)

private:
	Bool	 precomp_valid;
	Matrix precomp;

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	TextureTag();

	/// @}

	/// @name Get/Set Pos/Scale/Rot/Ml
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the position of the texture project.
	/// @return												The texture XYZ position.
	//----------------------------------------------------------------------------------------
	Vector GetPos();

	//----------------------------------------------------------------------------------------
	/// Gets the scale of the texture projection.
	/// @return												The scale of the texture.
	//----------------------------------------------------------------------------------------
	Vector GetScale();

	//----------------------------------------------------------------------------------------
	/// Gets the HPB rotation of the texture projection.
	/// @return												The HPB rotation of the texture.
	//----------------------------------------------------------------------------------------
	Vector GetRot();

	//----------------------------------------------------------------------------------------
	/// Gets the texture projection's coordinate system as a matrix.
	/// @return												The texture projection's coordinate system.
	//----------------------------------------------------------------------------------------
	Matrix GetMl();

	//----------------------------------------------------------------------------------------
	/// Sets the position of the texture projection.
	/// @param[in] v									The position of the texture.
	//----------------------------------------------------------------------------------------
	void SetPos(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Sets the scale of the texture projection.
	/// @param[in] v									The scale of the texture.
	//----------------------------------------------------------------------------------------
	void SetScale(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Sets the HPB rotation of the texture projection.
	/// @param[in] w									The HPB rotation of the texture.
	//----------------------------------------------------------------------------------------
	void SetRot(const Vector& w);

	//----------------------------------------------------------------------------------------
	/// Sets the texture projection's coordinate system matrix.
	/// @param[in] m									The texture projection's coordinate system.
	//----------------------------------------------------------------------------------------
	void SetMl(const Matrix& m);

	/// @}

	/// @name Get/Set Pos/Scale/Rot/Ml
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the material associated with the tag.
	/// @return												The material for the tag. @cinewareOwnsPointed{material}
	//----------------------------------------------------------------------------------------
	BaseMaterial* GetMaterial();

	//----------------------------------------------------------------------------------------
	/// Sets the material associated with the tag.
	/// @param[in] mat								The material for the tag. @callerOwnsPointed{material}
	//----------------------------------------------------------------------------------------
	void SetMaterial(BaseMaterial* mat);

	/// @}

	/// @name Get/Set Parameter
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] t_data							Assigned the parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter was retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// Sets a parameter of the tag.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] t_data							The parameter data to set.
	/// @return												@trueIfOtherwiseFalse{the parameter was set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToV(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_TEXTURETAG_H__
