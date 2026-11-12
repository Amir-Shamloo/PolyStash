/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEMATERIAL_H__
#define C4D_BASEMATERIAL_H__

#include "c4d_baselist2d.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// The material base class that all materials in @C4D are derived from.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseMaterial : public BaseList2D
{
	INSTANCEOF(BaseMaterial, BaseList2D)

private:
	BaseBitmap preview_image;
	BaseContainer _materialCache;

	NodeData* AllocData(Int32 id, Bool &known);

protected:
	BaseMaterial(Int32 typeID = NOTOK, Int32 pID = NOTOK);

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	virtual Int32 GetInstanceType() const { return Mbase; }
	virtual Int32 GetChunkLevel() { return 3; }

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base material}
	/// @param[in] type								A material type ID.
	//----------------------------------------------------------------------------------------
	static BaseMaterial* Alloc(Int32 type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base materials}
	/// @param[in,out] bl							@theToDestruct{base material}
	//----------------------------------------------------------------------------------------
	static void Free(BaseMaterial*& bl);

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the base material.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned base material. @callerOwnsPointed{base material}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next base material in the list.
	/// @return												The next base material, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base material}
	//----------------------------------------------------------------------------------------
	BaseMaterial* GetNext()
	{
		return (BaseMaterial*)SUPER::GetNext();
	}

	//----------------------------------------------------------------------------------------
	/// Gets the previous base material in the list.
	/// @return												The previous base material, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{base material}
	//----------------------------------------------------------------------------------------
	BaseMaterial* GetPred()
	{
		return (BaseMaterial*)SUPER::GetPred();
	}

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the base material.
	/// @warning	Must be used to make sure that the derived tag really is of the right type before casting it and accessing its members.
	/// @note			Plugin materials return their unique plugin IDs.
	/// @return												The base material type, for example @em Mbanzi.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Check if the base material is a node material.
	/// @since R20
	/// @return												@trueIfOtherwiseFalse{it is a node material}
	//----------------------------------------------------------------------------------------
	Bool HasNimbusRef() const;

	//----------------------------------------------------------------------------------------
	/// Gets the base material's unique disk type.
	/// @note	Normally this is the same as GetType(), but there are exceptions.
	/// @return												The base material's unique disk type.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	/// @name Preview
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the preview picture of the base material.
	/// @return												The preview picture, or @formatConstant{nullptr} if none has been calculated. @theOwnsPointed{base material,bitmap}
	//----------------------------------------------------------------------------------------
	BaseBitmap* GetPreview();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Gets the material cache of the base material.
	/// @return												The BaseContainer containing the properties for Standard Surface material description.
	//----------------------------------------------------------------------------------------
	BaseContainer& GetMaterialCache();

	/// @}

	/// @name Alien Overload
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this for self defined plugin materials.
	/// @code
	/// Bool AlienBaseMaterial::Execute()
	/// {
	///   Char* pName = GetName().GetCStringCopy();
	///   if (pName)
	///   {
	///     printf("\n - AlienPluginMaterial (%d): %s\n", (int)GetType(), pName);
	///     DeleteMem(pName);
	///   }
	///   else
	///     printf("\n - AlienPluginMaterial (%d): <noname>\n", (int)GetType());
	///
	///   PrintUniqueIDs(this);
	///
	///   // Assign a id to the material
	///   id = tempmatid;
	///
	///   tempmatid++;
	///
	///   switch(GetType())
	///   {
	///     case Mmaterial: printf("Cinema Material\n");
	///       break;
	///     case Msketch: printf("Sketch & Toon Material\n");
	///       break;
	///     case Mdanel: printf("Danel Shader Material\n");
	///       break;
	///     case Mbanji: printf("Banji Shader Material\n");
	///       break;
	///     case Mbanzi: printf("Banzi Shader Material\n");
	///       break;
	///     case Mcheen: printf("Cheen Shader Material\n");
	///       break;
	///     case Mmabel: printf("Mabel Shader Material\n");
	///       break;
	///     case Mnukei: printf("Nukei Shader Material\n");
	///       break;
	///     case SKY_II_SKY_SURFACE_MATERIAL: printf("Sky Material (hidden)\n");
	///       break;
	///     case Mfog: printf("Fog Shader Material\n");
	///       break;
	///     case Mterrain: printf("Terrain Shader Material\n");
	///       break;
	///     case Mhair: printf("Hair Shader Material\n");
	///       break;
	///     default: printf("unknown plugin material\n");
	///   }
	///
	///   return true;
	/// }
	/// @endcode
	/// @return												@trueIfOtherwiseFalse{the material type is supported by the external application}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}

protected:
	// using a Bool instead of unknown NimbusRef
	Bool _hasNimbus;
	Int _nimbusXmlUnknownDataLength;
	void* _nimbusXmlUnknownData;
	Int _nimbusNodesUnknownDataLength;
	void* _nimbusNodesUnknownData;
};

#pragma pack (pop)
}

#endif	// C4D_BASEMATERIAL_H__
