/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_POLYGONOBJECT_H__
#define C4D_POLYGONOBJECT_H__

#include "c4d_pointobject.h"

namespace cineware
{
#pragma pack (push, 8)

class NgonBase;

//----------------------------------------------------------------------------------------
/// Represents @C4D's polygon object.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class PolygonObject : public PointObject
{
	INSTANCEOF(PolygonObject, PointObject)

	friend class BaseObject;

private:
	virtual Int32 GetDiskType() const;

protected:
	PolygonObject();

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{polygon object}
	/// @param[in] pcnt								The point count.
	/// @param[in] vcnt								The polygon count.
	/// @return												@allocReturn{polygon object}
	//----------------------------------------------------------------------------------------
	static PolygonObject* Alloc(Int32 pcnt, Int32 vcnt);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{polygon objects}
	/// @param[in,out] bl							@theToDestruct{polygon object}
	//----------------------------------------------------------------------------------------
	static void Free(PolygonObject*& bl);

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object type for a PolygonObject.
	/// @note	Always returns @ref Opolygon.
	/// @return												The object type for a PolygonObject i.e. @ref Opolygon.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	/// @}

	/// @name Polygon Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the selected polygons (visible ones only).
	/// @return												The selection of visible polygons. @theOwnsPointed{polygon object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetPolygonS();

	//----------------------------------------------------------------------------------------
	/// Gets the hidden polygons.
	/// @note	These are only hidden within the editor.
	/// @return												The selection of hidden polygons. @theOwnsPointed{polygon object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetPolygonH();

	//----------------------------------------------------------------------------------------
	/// Gets the selected edges.\n
	/// Edges are indexed by 4 * @em polygon + @em edge where @em polygon is the polygon index and @em edge is the edge index between @em 0 and @em 3.
	/// @return												The selection of visible edges. @theOwnsPointed{polygon object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetEdgeS();

	//----------------------------------------------------------------------------------------
	/// Gets the hidden edges.\n
	/// Edges are indexed by 4 * @em polygon + @em edge where @em polygon is the polygon index and @em edge is the edge index between @em 0 and @em 3.
	/// @return												The selection of hidden edges. @theOwnsPointed{polygon object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetEdgeH();

	//----------------------------------------------------------------------------------------
	/// Gets the phong break edges.\n
	/// Edges are indexed by 4 * @em polygon + @em edge where @em polygon is the polygon index and @em edge is the edge index between @em 0 and @em 3.
	/// @return												The selection of phong break edges. @theOwnsPointed{polygon object,base select}
	//----------------------------------------------------------------------------------------
	BaseSelect* GetPhongBreak();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the read-only buffer of polygons.
	/// @return												A pointer to the read-only buffer of polygons. @theOwnsPointed{polygon object,buffer}
	//----------------------------------------------------------------------------------------
	const CPolygon* GetPolygonR();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the writable array of polygons.
	/// @return												A pointer to the writable array of polygons. @theOwnsPointed{polygon object,buffer}
	//----------------------------------------------------------------------------------------
	CPolygon* GetPolygonW();

	//----------------------------------------------------------------------------------------
	/// Gets the number of polygons.
	/// @return												The number of polygons.
	//----------------------------------------------------------------------------------------
	Int32 GetPolygonCount();

	/// @}

	/// @name Resize
	/// @{

	//----------------------------------------------------------------------------------------
	/// Changes the number of points and polygons.
	/// @param[in] pcnt								The new number of points.
	/// @param[in] vcnt								The new number of polygons.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool ResizeObject(Int32 pcnt, Int32 vcnt);

	//----------------------------------------------------------------------------------------
	/// Changes the number of points, polygons and N-gons.
	/// @param[in] pcnt								The new number of points.
	/// @param[in] vcnt								The new number of polygons.
	/// @param[in] ncnt								The new number of N-gons.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool ResizeObject(Int32 pcnt, Int32 vcnt, Int32 ncnt);

	/// @}

	/// @name N-gon
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of N-gons in the polygon object, i.e. the number of polygons with more than @em 4 points.
	/// @return												The number of N-gons in the polygon object.
	//----------------------------------------------------------------------------------------
	Int32 GetNgonCount();

	//----------------------------------------------------------------------------------------
	/// Gets the NgonBase instance for the polygon object.
	/// @return												The N-gon base. @theOwnsPointed{polygon object,N-gon base}
	//----------------------------------------------------------------------------------------
	NgonBase* GetNgonBase();

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a buffer with the stored phong normals for the polygon object.\n
	/// Buffer contains 4*GetPolygonCount() vectors, one for each point of each face (including the unused fourth point of a triangle). The buffer has to be freed afterwards with DeleteMem().
	/// @note	This method is only valid if the object has a phong tag.
	/// @return												A pointer to the buffer with phong normals.
	//----------------------------------------------------------------------------------------
	Vector32* CreatePhongNormals();

	//----------------------------------------------------------------------------------------
	/// Creates a UVW tag for the object.
	/// @param[in] name								The name for the new tag.
	/// @param[in] generateFrom				A pointer to a texture tag to be used to generate or copy the UVW data from or @formatConstant{nullptr}.
	///                               Note: TEXTURETAG_PROJECTION_CAMERAMAP and TEXTURETAG_PROJECTION_FRONTAL are not supported.
	/// @return												The UVW tag, or @formatConstant{nullptr} if an error occurred. @cinewareOwnsPointed{tag}
	//----------------------------------------------------------------------------------------
	BaseTag* MakeUVWTag(const String& name, const TextureTag* generateFrom = nullptr);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_POLYGONOBJECT_H__
