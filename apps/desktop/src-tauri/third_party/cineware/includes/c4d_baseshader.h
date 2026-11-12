/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASESHADER_H__
#define C4D_BASESHADER_H__

#include "c4d_baselist4d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a shader in the shader tree of a channel. Call Material::GetShader() to retrieve an instance.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseShader : public BaseList4D
{
	INSTANCEOF(BaseShader, BaseList4D)

private:
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

protected:
	virtual Int32 GetInstanceType() const { return Xbase; }

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseShader();

	//----------------------------------------------------------------------------------------
	/// Constructs a base shader of the given @formatParam{type}.
	/// @param[in] type								A shader type ID.
	//----------------------------------------------------------------------------------------
	BaseShader(Int32 type);

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base shader}
	/// @param[in] type								A shader type ID.
	/// @return												@allocReturn{base shader}
	//----------------------------------------------------------------------------------------
	static BaseShader* Alloc(Int32 type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base shaders}
	/// @param[in,out] s							@theToDestruct{base shader}
	//----------------------------------------------------------------------------------------
	static void Free(BaseShader* s);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next base shader in the list.
	/// @return												The next base shader, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous base shader in the list.
	/// @return												The previous base shader, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetPred();

	//----------------------------------------------------------------------------------------
	/// Gets the parent base shader of the list node.
	/// @return												The parent base shader, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child base shader of the list node.
	/// @return												The first child base shader, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child base shader of the list node.
	/// @return												The last child base shader, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetDownLast();

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the shader.
	/// @warning	This must be used to make sure that the derived shader really is of the right type before casting it and accessing its members.
	/// @note			Plugin shader return their unique plugin IDs.
	/// @return												The shader type, for example @em Xvariation.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the shader's unique disk type.
	/// @note	Normally this is the same as GetType(), but there are exceptions.
	/// @return												The shader's unique disk type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the base shader.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trn								An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned base shader. @callerOwnsPointed{base shader}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trn);

	/// @}


	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if @c *this is similar to @formatParam{*dst}.
	/// @param[in] dst								The shader to compare with. @callerOwnsPointed{shader}
	/// @return												@trueIfOtherwiseFalse{the shaders are the same}
	//----------------------------------------------------------------------------------------
	Bool Compare(BaseShader* dst);

	/// @}

	/// @name Bitmap Shader
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the texture path for bitmap shaders.
	/// @see ::Xbitmap.
	/// @return												The texture path.
	//----------------------------------------------------------------------------------------
	Filename GetFileName();

	//----------------------------------------------------------------------------------------
	/// Sets the texture path for bitmap shaders.
	/// @see ::Xbitmap.
	/// @param[in] name								The texture path to set.
	//----------------------------------------------------------------------------------------
	void SetFileName(Filename name);

	//----------------------------------------------------------------------------------------
	/// Checks if the texture path is set for bitmap shaders.
	/// @see ::Xbitmap.
	/// @return												@trueIfOtherwiseFalse{the texture path is set for bitmap shaders}
	//----------------------------------------------------------------------------------------
	Bool HasContent();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASESHADER_H__
