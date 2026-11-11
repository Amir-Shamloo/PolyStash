/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEOBJECT_H__
#define C4D_BASEOBJECT_H__

#include "ge_math.h"
#include "c4d_roottag.h"
#include "c4d_baselist4d.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseTag;
class UVWTag;
class TextureTag;
class RootObject;

/// @addtogroup ANIMATEFLAGS
/// @ingroup group_enumeration
/// @{
enum ANIMATEFLAGS
{
	ANIMATEFLAGS_0						= 0,				///< None.
	ANIMATEFLAGS_NO_PARTICLES	= (1 << 2),	///< Ignore particles.
	ANIMATEFLAGS_NO_CHILDREN	= (1 << 6),	///< Do not animate children.
	ANIMATEFLAGS_INRENDER			= (1 << 7),	///< Prepare to render scene.
	ANIMATEFLAGS_NO_MINMAX		= (1 << 8),	///< @markPrivate
	ANIMATEFLAGS_NO_NLA				= (1 << 9),	///< @markPrivate
	ANIMATEFLAGS_NLA_SUM			= (1 << 10)	///< @markPrivate
} ENUM_END_FLAGS(ANIMATEFLAGS);
/// @}

/// @addtogroup group_object Object
/// @ingroup group_topic Topics
/// @{

#define BASEOBJECT_FLAGS_FROZEN_POS		(1 << 1)																																										///< @markPrivate
#define BASEOBJECT_FLAGS_FROZEN_ROT		(1 << 2)																																										///< @markPrivate
#define BASEOBJECT_FLAGS_FROZEN_SCALE	(1 << 3)																																										///< @markPrivate
#define BASEOBJECT_FLAGS_FROZEN				(BASEOBJECT_FLAGS_FROZEN_POS | BASEOBJECT_FLAGS_FROZEN_ROT | BASEOBJECT_FLAGS_FROZEN_SCALE)	///< @markPrivate

//----------------------------------------------------------------------------------------
/// Data structure for object color properties.
//----------------------------------------------------------------------------------------
struct ObjectColorProperties
{
	Int32	 usecolor;	///< The color mode. @see ::ID_BASEOBJECT_USECOLOR
	Vector color;			///< The color.
	Bool	 xray;			///< The X-ray mode.
};

//----------------------------------------------------------------------------------------
/// All objects in @C4D are derived from the BaseObject class.\n
/// Contains methods for the object's position, scale and rotation, as well as for accessing the sub-items of the object (e.g. tags and animation tracks).
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseObject : public BaseList4D
{
	INSTANCEOF(BaseObject, BaseList4D)

	friend class RootObject;
	friend class BaseDraw;
	friend class BaseDocument;
	friend class NodeData;

	mutable Matrix ml_cache;
	mutable Bool	 ml_changed;

private:
	Vector				t_pos;
	Vector				t_rot;
	Vector				t_scale;
	Vector				f_pos, f_scale, f_rot, tf_rot, tf_pos, tf_scale;
	Matrix				frozenm, ifrozenm;
	UInt32				boflags;
	ROTATIONORDER	t_rotation_order;

	Matrix				intern;
	Matrix				comp;
	Matrix				m_ModelingAxis;
	RootTag				m_tags;
	Int32					e_mode;
	Int32					r_mode;
	Int32					t_mode;
	Bool					t_xray;
	Vector				t_color;
	Int32					t_usecolor;
	Int32					t_shadingcolormode;
	Vector				t_shadingcolor;
	RootObject*		m_deformcacheobjects;
	RootObject*		m_cacheobjects;
	Int32					_disktype;

	NodeData* AllocObjectData(Int32 type, Bool& known);

	UInt32 GetLastCrc();
	void SetLastCrc(UInt32 chkSum);
	void CalculateCrc(Crc32& chkSum);
	void UpdateInternalCrc();

	const Matrix& _GetMl() const;

	virtual Int32 GetDiskType() const;
	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
	BaseObject* GetCloneObject(const Bool childs, const COPYFLAGS flags = COPYFLAGS_0, AliasTrans* trans = nullptr);	// Clone complete object. (If childs==true clone also child objects.)

protected:
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual Int32 GetInstanceType() const { return Obase; }

	BaseObject();
	BaseObject(Int32 id);
	virtual ~BaseObject();

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base object}
	/// @param[in] type								The object type.
	/// @return												@allocReturn{base object}
	//----------------------------------------------------------------------------------------
	static BaseObject* Alloc(Int32 type);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base objects}
	/// @param[in,out] bl							@theToDestruct{base object}
	//----------------------------------------------------------------------------------------
	static void Free(BaseObject*& bl);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next object in the list.
	/// @return												The next object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{ object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous object in the list.
	/// @return												The previous base object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetPred();

	//----------------------------------------------------------------------------------------
	/// Gets the parent object of the list node.
	/// @return												The parent object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child object of the list node.
	/// @return												The first child object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child base object of the list node.
	/// @return												The last child object, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetDownLast();

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the object.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{layer}
	/// @return												The cloned object. @callerOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	/// @}

	/// @name Abs Pos/Scale/Rot
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the absolute position of the object.\n
	/// These are the absolute local coordinates within the parent object.\n
	/// If the object has no parent then these are world coordinates.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's absolute position.
	//----------------------------------------------------------------------------------------
	Vector GetAbsPos() const;

	//----------------------------------------------------------------------------------------
	/// Sets the absolute local position of the object within its parent.\n
	/// These are the absolute local coordinates within the parent object.\n
	/// If the object has no parent then these are world coordinates.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's absolute position to set.
	//----------------------------------------------------------------------------------------
	void SetAbsPos(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the absolute scale of the object.\n
	/// This is relative to the object's parent if it has one.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's absolute scale.
	//----------------------------------------------------------------------------------------
	Vector GetAbsScale() const;

	//----------------------------------------------------------------------------------------
	/// Sets the absolute scale of the object.\n
	/// This is relative to the object's parent if it has one.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's absolute scale to set.
	//----------------------------------------------------------------------------------------
	void SetAbsScale(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the absolute HPB rotation of the object.\n
	/// This is relative to the object's parent if it has one.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's absolute HPB rotation.
	//----------------------------------------------------------------------------------------
	Vector GetAbsRot() const;

	//----------------------------------------------------------------------------------------
	/// Sets the absolute HPB rotation of the object.\n
	/// This is relative to the object's parent if it has one.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's absolute HPB rotation to set.
	//----------------------------------------------------------------------------------------
	void SetAbsRot(const Vector& v);

	/// @}

	/// @name Frozen Pos/Scale/Rot
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the frozen position of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's frozen position.
	//----------------------------------------------------------------------------------------
	Vector GetFrozenPos() const;

	//----------------------------------------------------------------------------------------
	/// Sets the frozen position of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's frozen position to set.
	//----------------------------------------------------------------------------------------
	void SetFrozenPos(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the frozen scale of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's frozen scale.
	//----------------------------------------------------------------------------------------
	Vector GetFrozenScale() const;

	//----------------------------------------------------------------------------------------
	/// Sets the frozen scale of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's frozen scale to set.
	//----------------------------------------------------------------------------------------
	void SetFrozenScale(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the frozen HPB rotation of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's frozen HPB rotation.
	//----------------------------------------------------------------------------------------
	Vector GetFrozenRot() const;

	//----------------------------------------------------------------------------------------
	/// Sets the frozen HPB rotation of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's frozen HPB rotation to set.
	//----------------------------------------------------------------------------------------
	void SetFrozenRot(const Vector& v);

	/// @}

	/// @name Rel Pos/Scale/Rot
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the relative position of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's relative position.
	//----------------------------------------------------------------------------------------
	Vector GetRelPos() const;

	//----------------------------------------------------------------------------------------
	/// Sets the relative position of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's relative position to set.
	//----------------------------------------------------------------------------------------
	void SetRelPos(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the relative scale of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's relative scale.
	//----------------------------------------------------------------------------------------
	Vector GetRelScale() const;

	//----------------------------------------------------------------------------------------
	/// Sets the relative scale of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's relative scale to set.
	//----------------------------------------------------------------------------------------
	void SetRelScale(const Vector& v);

	//----------------------------------------------------------------------------------------
	/// Gets the relative HPB rotation of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's relative scale.
	//----------------------------------------------------------------------------------------
	Vector GetRelRot() const;

	//----------------------------------------------------------------------------------------
	/// Sets the relative HPB rotation of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] v									The object's relative rotation to set.
	//----------------------------------------------------------------------------------------
	void SetRelRot(const Vector& v);

	/// @}

	/// @name Ml/Mg Matrix
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the local matrix representing the object's position, scale and rotation.
	/// @return												The object's local matrix.
	//----------------------------------------------------------------------------------------
	const Matrix& GetMl() const;

	//----------------------------------------------------------------------------------------
	/// Sets the local matrix representing the object's position, scale and rotation.
	/// @param[in] m									The object's local matrix to set.
	//----------------------------------------------------------------------------------------
	void SetMl(const Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Gets the frozen and normalized matrix of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's frozen and normalized matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetFrozenMln() const;

	//----------------------------------------------------------------------------------------
	/// Gets the relative and normalized matrix of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's relative and normalized matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetRelMln() const;

	//----------------------------------------------------------------------------------------
	/// Gets the relative matrix of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @return												The object's relative matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetRelMl() const;

	//----------------------------------------------------------------------------------------
	/// Sets the relative matrix of the object.
	/// @see The article @link page_freeze_transformation Freeze Transformation@endlink
	/// @param[in] m									The object's relative matrix to set.
	//----------------------------------------------------------------------------------------
	void SetRelMl(const Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Gets the world (global) matrix representing the object's position, scale and rotation.
	/// @warning Only valid if the object is attached to a document. Virtual objects in caches and deform caches are not attached to a document, so this cannot be used for those objects.
	/// @return												The object's world matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetMg();

	//----------------------------------------------------------------------------------------
	/// Sets the world (global) matrix representing the object's position, scale and rotation.
	/// @param[in] m									The object's world matrix to set.
	//----------------------------------------------------------------------------------------
	void SetMg(const Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Gets the local normalized matrix representing the object's position, scale and rotation.
	/// @return												The object's normalized local matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetMln() const;

	//----------------------------------------------------------------------------------------
	/// Gets the world (global) normalized matrix representing the object's position, scale and rotation.
	/// @return												The object's normalized world matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetMgn();

	//----------------------------------------------------------------------------------------
	/// Gets the world (global) matrix of the parent object representing the parent's position, scale and rotation.\n
	/// If the object has no parent object then the method returns a unit matrix.
	/// @return												The parent object's world matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetUpMg();

	//----------------------------------------------------------------------------------------
	/// Copies the object's matrix to another object.
	/// @param[in,out] dest						The destination object to copy the matrix to.
	//----------------------------------------------------------------------------------------
	void CopyMatrixTo(BaseObject* dest);

	/// @}

	/// @name Rotation Order
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the rotation order of the object.
	/// @param[in] order							The rotation order to set for the object: @enumerateEnum{ROTATIONORDER}
	//----------------------------------------------------------------------------------------
	void SetRotationOrder(ROTATIONORDER order);

	//----------------------------------------------------------------------------------------
	/// Gets the rotation order of the object.
	/// @return												The rotation order of the object: @enumerateEnum{ROTATIONORDER}
	//----------------------------------------------------------------------------------------
	ROTATIONORDER GetRotationOrder() const;

	/// @}

	/// @name Synchronized Rotation Mode
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the Synchronized Rotation Mode of the object.
	/// @since 18.002
	/// @param[in] active							Enable/Disable the Synchronized Rotation mode for the object
	//----------------------------------------------------------------------------------------
	void SetQuaternionRotationMode(Bool active);

	//----------------------------------------------------------------------------------------
	/// Checks if Object Rotation is interpolated in %Quaternion Mode.
	/// @since 18.002
	/// @return												@trueIfOtherwiseFalse{Rotation Interpolation is Quaternion}
	//----------------------------------------------------------------------------------------
	Bool IsQuaternionRotationMode();

	//----------------------------------------------------------------------------------------
	/// Makes sure that the track Curves component are synchronized (keys on each component). If a component track is found, the other track components will be synchronized.
	/// @since 18.002
	/// @param[in] vectorTrackID			id of the Vector Track to synchronize
	/// @param[in] startRange					(optional)Start operation at given time
	/// @param[in] endRange						(optional)End operation at given time
	/// @return												@trueIfOtherwiseFalse{Track Synchronization was successfull}
	//----------------------------------------------------------------------------------------
	Bool SynchronizeVectorTrackKeys(Int32 vectorTrackID, BaseTime startRange = BaseTime(-108000, 1), BaseTime endRange = BaseTime(108000, 1));

	//----------------------------------------------------------------------------------------
	/// Tries to find the best Euler Angle according to the previous key. The object must have keys on each component.
	/// @since 18.002
	/// @param[in] rotationTrackID		Rotation Track ID to manage (Relative, global, absolute, frozen)
	/// @param[in] bAdjustTangent			Try to adjust the tangent with new value, if false auto is used
	/// @param[in] startRange					(optional)Start operation at given time
	/// @param[in] endRange						(optional)End operation at given time
	/// @return												@trueIfOtherwiseFalse{Rotation Synchronization was successfull}
	//----------------------------------------------------------------------------------------
	Bool FindBestEulerAngle(Int32 rotationTrackID, Bool bAdjustTangent, BaseTime startRange = BaseTime(-108000, 1), BaseTime endRange = BaseTime(108000, 1));

	/// @}

	/// @name Editor/Render/Deform Mode
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the mode of the editor dot for the object.
	/// @return												The editor mode.
	//----------------------------------------------------------------------------------------
	Int32 GetEditorMode();

	//----------------------------------------------------------------------------------------
	/// Sets the mode for of editor dot for the object.
	/// @param[in] mode								The editor mode to set.
	//----------------------------------------------------------------------------------------
	void SetEditorMode(Int32 mode);

	//----------------------------------------------------------------------------------------
	/// Gets the mode of the render dot for the object.
	/// @return												The render mode.
	//----------------------------------------------------------------------------------------
	Int32 GetRenderMode();

	//----------------------------------------------------------------------------------------
	/// Sets the mode of the render dot for the object.
	/// @param[in] mode								The render mode to set.
	//----------------------------------------------------------------------------------------
	void SetRenderMode(Int32 mode);

	//----------------------------------------------------------------------------------------
	/// Gets the enabled mode of the deformation/generator tick.
	/// @return												@trueIfOtherwiseFalse{the deformer/generator is enabled}
	//----------------------------------------------------------------------------------------
	Bool GetDeformMode();

	//----------------------------------------------------------------------------------------
	/// Sets the enabled mode of the deformation/generator tick.
	/// @param[in] mode								@trueOtherwiseFalse{to enable the generator/deformer object}
	//----------------------------------------------------------------------------------------
	void SetDeformMode(Bool mode);

	/// @}

	/// @name Tag
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the first tag associated with the object.
	/// @return												The first tag, or @formatConstant{nullptr} if no tags available. @theOwnsPointed{object,tag}
	//----------------------------------------------------------------------------------------
	BaseTag* GetFirstTag();

	//----------------------------------------------------------------------------------------
	/// Gets a tag of a certain type associated with the object.
	/// @param[in] type								The type of tag to fetch from the object.
	/// @param[in] nr									Optionally specify a starting tag index to begin the search. Since 17.008\n
	///                               If the requested number is not available then @formatConstant{nullptr} will be returned even if there is a tag of that type with a different index.
	/// @return												The requested tag, or @formatConstant{nullptr} if no tags of the requested type are available. @theOwnsPointed{object,tag}
	//----------------------------------------------------------------------------------------
	BaseTag* GetTag(Int32 type, Int32 nr = 0);

	//----------------------------------------------------------------------------------------
	/// Creates and associates a tag with the object.
	/// @param[in] type								The type of tag to create.
	/// @param[in] pred								The previous tag to insert the tag after, or @formatConstant{nullptr} if the tag should be inserted at the start of the tag list. @callerOwnsPointed{tag}
	/// @return												The new tag, or @formatConstant{nullptr} if failed. @theOwnsPointed{object,tag}
	//----------------------------------------------------------------------------------------
	BaseTag* MakeTag(Int32 type, BaseTag* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Inserts tag into the object's tag list.
	/// @param[in] tp									The tag to insert. The object takes over the ownership of the pointed tag.
	/// @param[in] pred								The previous tag to insert the tag after, or @formatConstant{nullptr} if the tag should be inserted at the start of the tag list. @callerOwnsPointed{tag} Since 18.002
	//----------------------------------------------------------------------------------------
	void InsertTag(BaseTag* tp, BaseTag* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Removes a tag from the object and frees its resources.
	/// @since 18.002
	/// @param[in] type								The type of tag to remove.
	/// @param[in] nr									Optionally specify a starting tag index to begin the search.
	//----------------------------------------------------------------------------------------
	void KillTag(Int32 type, Int32 nr = 0);

	/// @}

	/// @name Cache
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object from the previously built cache.\n
	/// Situations can be quite complex in @C4D. For instance GetCache() could return a list of objects.\n
	/// For example the default Sweep Generator internally has some built-in caches for the sweep surface and for the caps.\n
	/// The following helper routine can be used to make things easier. It browses through this rather complex hierarchy of objects and caches recursively:
	/// @code
	/// void DoRecursion(BaseObject *op)
	/// {
	///   BaseObject* tp = op->GetDeformCache();
	///   if (tp)
	///   {
	///     DoRecursion(tp);
	///   }
	///   else
	///   {
	///     tp = op->GetCache(nullptr);
	///     if (tp)
	///     {
	///       DoRecursion(tp);
	///     }
	///     else
	///     {
	///       if (!op->GetBit(BIT_CONTROLOBJECT))
	///       {
	///         if (op->IsInstanceOf(Opolygon))
	///         {
	///           ...
	///         }
	///       }
	///     }
	///   }
	///
	///   for (tp = op->GetDown(); tp; tp=tp->GetNext())
	///   {
	///     DoRecursion(tp);
	///   }
	/// }
	/// @endcode
	/// @b Remember:<br>
	/// Use the above routine only if the caches are already built.
	/// @see	GetDeformCache()
	/// @return												The object's previously built cache or @formatConstant{nullptr}. @theOwnsPointed{object,object's cache}\n
	///																May return @formatConstant{nullptr} if the cache is not available or is not yet built.
	//----------------------------------------------------------------------------------------
	BaseObject* GetCache();

	//----------------------------------------------------------------------------------------
	/// Gets the previously built cache that has been deformed by an active deformer.\n
	/// It is important to understand the concept how the deformer cache operates.\n
	/// For each object in the hierarchy that generates a polygonal cache a deformer cache could also have been created by an active deformer object.\n
	/// A simple example will help to see how this works:
	/// @image html cache_arraycube1.png
	/// Take the simple hierarchy shown above, the array generator object creates a virtual hierarchy in the cache, this can be retrieved using GetCache().\n
	/// This hierarchy is:
	/// @image html cache_arraycube2.png
	/// From each of the Cube objects a further cache is generated, this time polygonal:
	/// @image html cache_arraycube3.png
	/// From these the deformer object generates a deformed polygon cache:
	/// @image html cache_arraycube4.png
	/// So the final hierarchy is:
	/// @image html cache_arraycube5.png
	/// When a deformer becomes active every object/cache object gets a deform cache (if it was a polygonal object). The deformer cache is always polygonal and is only ever a single object.\n
	/// @b Important:<br>
	/// The caches are always built after all plugins and expressions have been called.
	/// @return												The objects previously built deformed cache or @formatConstant{nullptr}. @theOwnsPointed{object,object's cache}\n
	///																May return @formatConstant{nullptr} if the cache is not available or is not yet built.
	//----------------------------------------------------------------------------------------
	BaseObject* GetDeformCache();

	/// @}

	/// @name Color Properties
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object's color properties and fill @formatParam{prop}.
	/// @param[in] prop								Filled with the color properties. @callerOwnsPointed{color properties}
	//----------------------------------------------------------------------------------------
	void GetColorProperties(ObjectColorProperties* prop);

	//----------------------------------------------------------------------------------------
	/// Sets the object's color properties from @formatParam{prop}.
	/// @param[in] prop								The color properties to set. @callerOwnsPointed{color properties}
	//----------------------------------------------------------------------------------------
	void SetColorProperties(ObjectColorProperties* prop);

	/// @}

	/// @name Execute
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this method for self defined objects.
	/// @return												@trueIfOtherwiseFalse{the object type is supported by the external application (e.g. no polygonal cache is returned)}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the object data has changed.
	/// @return												@trueIfOtherwiseFalse{the object data has changed}
	//----------------------------------------------------------------------------------------
	Bool HasChanged();

	/// @}
};

/// @}

//----------------------------------------------------------------------------------------
/// Generates a UVW tag for an object.
/// @param[in] op									The object to generate the UVW coordinates for. @callerOwnsPointed{base object}
/// @param[in] opmg								The object's global matrix.
/// @param[in] tp									The texture tag to generate the UVW coordinates from. @callerOwnsPointed{texture tag}
///                               Note: TEXTURETAG_PROJECTION_CAMERAMAP and TEXTURETAG_PROJECTION_FRONTAL are not supported.
/// @param[in] texopmg						The global matrix of the object that carries the texture tag.
/// @return												The created UVW tag or @formatConstant{nullptr} if the texture type is already UV or if failed.
/// 															If @formatConstant{nullptr} then the UVW tag must retrieved from the object itself. @callerOwnsPointed{UVW tag}
//----------------------------------------------------------------------------------------
UVWTag* GenerateUVW(BaseObject* op, const Matrix& opmg, TextureTag* tp, const Matrix& texopmg);

#pragma pack (pop)
}

#endif	// C4D_BASEOBJECT_H__
