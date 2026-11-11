/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_VERTEXCOLORTAG_H__
#define C4D_VERTEXCOLORTAG_H__

#include "c4d_basetag.h"
#include "c4d_vertexcolorstruct.h"

namespace cineware
{
#pragma pack (push, 8)

typedef void* VertexColorHandle;						///< Handle for vertex color data. See also: VertexColorTag
typedef const void* ConstVertexColorHandle;	///< Handle for read-only vertex color data. See also: VertexColorTag

//----------------------------------------------------------------------------------------
/// VertexColorTag is a Variable Tag used to store colors (RGBA) into the mesh structure. It can store data in 2 ways: per polygon vertex and per mesh vertex.
///
/// - Per polygon vertex mode is very similar to how UVWTag or NormalTag store data. Just create it using @ref Alloc() with @formatParam{count}==PolygonObject::GetPolygonCount().
/// - Per mesh vertex stores a single color value for mesh vertex and can be used even on objects without any polygon. Create it using Alloc() with @formatParam{count}==PointObject::GetPointCount().
///
/// Using per polygon vertex mode the tag provides all methods to set/get color per polygons or per vertex(average). It also provides some convenience functions to get/set just color or just alpha value in both modes.
///
/// @since 18.002
///
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class VertexColorTag : public BaseTag
{
	INSTANCEOF(VertexColorTag, BaseTag)

public:
	/// @name Constructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	VertexColorTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~VertexColorTag();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Gets the read-only data handle. It automatically recognizes if the tag is per vertex or per polygon vertex.
	/// @return												The handle to the read-only VertexColor data. @theOwnsPointed{tag,data}
	//----------------------------------------------------------------------------------------
	ConstVertexColorHandle GetDataAddressR();

	//----------------------------------------------------------------------------------------
	/// Gets the writable data handle. It automatically recognizes if the tag is per vertex or per polygon vertex.
	/// @return												The handle to the writable VertexColor data. @theOwnsPointed{tag,data}
	//----------------------------------------------------------------------------------------
	VertexColorHandle	GetDataAddressW();

	//----------------------------------------------------------------------------------------
	/// Checks if the data is stored per vertex or per polygon vertex.
	/// @return												@trueIfOtherwiseFalse{colors are stored per vertex}
	//----------------------------------------------------------------------------------------
	Bool IsPerPointColor();

	//----------------------------------------------------------------------------------------
	/// Sets wether the data is stored per vertex or per polygon vertex.
	/// @note The item count is automatically updated and the data converted if possible.
	/// @warning All handles are invalidated after this call.
	/// @param[in] perPointColor			If @formatConstant{true} the data will be stored per vertex otherwise per polygon vertex.
	/// @return												@trueIfOtherwiseFalse{successfull}
	//----------------------------------------------------------------------------------------
	Bool SetPerPointMode(Bool perPointColor);

	//----------------------------------------------------------------------------------------
	/// Retrieves the Vertex Colors for the polygon @formatParam{i} in per polygon vertex mode.\n
	/// Example:
	/// @code
	/// ConstVertexColorHandle dataptr = ((VertexColorTag*)tag)->GetDataAddressR()
	/// VertexColorStruct vcs;
	/// for (Int32 i = 0; i < faceCount; ++i)
	/// {
	///   VertexColorTag::Get(dataptr, faces[i], vcs);
	///   // Do something with vcs (you can just read values)
	///	}
	/// @endcode
	/// @param[in] dataptr						The data handle.
	/// @param[in] i									The polygon index.
	/// @param[out] res								The retrieved Vertex Color data.
	//----------------------------------------------------------------------------------------
	static void Get(ConstVertexColorHandle dataptr, Int32 i, VertexColorStruct& res);

	//----------------------------------------------------------------------------------------
	/// Sets the Vertex Colors for the polygon @formatParam{i} in per polygon vertex mode.\n
	/// Example:
	/// @code
	/// VertexColorHandle dataptr = ((VertexColorTag*)tag)->GetDataAddressW()
	/// VertexColorStruct vcs;
	/// for (Int32 i = 0; i < faceCount; ++i)
	/// {
	///   VertexColorTag::Get(dataptr, faces[i], vcs);
	///   // Do something with vcs (e.g. write values)
	///   for (Int32 a = 0; a < 4; ++a)
	///     vcs[a] = Vecor4d32(1, 0, 0, 0.5);
	///   // Do not forget to set values again
	///   VertexColorTag::Set(dataptr, faces[i], vcs);
	///	}
	/// @endcode
	/// @param[in] dataptr						The data handle.
	/// @param[in] i									The polygon index.
	/// @param[in] s									The Vertex Color data to be set to the polygon.
	//----------------------------------------------------------------------------------------
	static void Set(VertexColorHandle dataptr, Int32 i, const VertexColorStruct& s);

	//----------------------------------------------------------------------------------------
	/// @allocatesA{VertexColorTag instance}
	/// @param[in] count							The element count in the tag. Must be equal to the object polygon count if in per polygon vertex storage, otherwise must be equal to the object point count.
	/// @return												@allocReturn{VertexColorTag}
	//----------------------------------------------------------------------------------------
	static VertexColorTag* Alloc(Int32 count);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{vertex color tags}
	/// @param[in] bl									@theToDestruct{VertexColorTag instance}
	//----------------------------------------------------------------------------------------
	static void Free(VertexColorTag*& bl);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32 GetDataCount();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Resize(Int32 newcnt, Int32 datasize);

private:
	Float32* vdata;
	Int32		 vCount;

	/// @name Private
	/// @{

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_VERTEXCOLORTAG_H__
