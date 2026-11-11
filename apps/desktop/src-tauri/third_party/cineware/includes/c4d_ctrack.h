/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CTRACK_H__
#define C4D_CTRACK_H__

#include "c4d_baselist2d.h"
#include "c4d_ckey.h"
#include "c4d_ccurve.h"
#include "c4d_descid.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
class CAnimInfo
{
public:
	CAnimInfo();

	BaseDocument*	doc;		///< Document.
	BaseList2D*		op;			///< Original object.
	BaseTime			otime;	///< Original time.
	Float64				xtime;	///< Remapped time. Usually identical to the time delivered by @link CAnimInfo::doc doc@endlink->GetTime().\n
												///< Only if there are loops and/or time curves assigned the time value is remapped into the normal range between first and last key.\n
												///< It is a float value instead of a BaseTime to avoid resolution/aliasing issues.
	Float64			fac;			///< Factor of time between the first and the last key of the track.
	Float64			rel;			///< Factor between two keys. Only !=@em 0.0 if @ref k1 != @formatConstant{nullptr} && @ref k2 !=@formatConstant{nullptr}.
	const CKey*	k1;				///< Last key before or at the current time. Can be @formatConstant{nullptr}.
	const CKey*	k2;				///< Next key strictly after the current time. Can be @formatConstant{nullptr}.
	Int32				k1idx,		///< Key index for @ref k1.
							k2idx;		///< Key index for @ref k2.
	Int32				cycle;		///< @em 0 for the range from first key to last key, after last key it is +@em 1, +@em 2,etc. before first key -@em 1, -@em 2, etc.\n
												///< This takes into account things like time curves and allows to compute the offset of values for tracks with pre or post behavior (for instance looping).
	Float64			k1time;		///< Key time for @ref k1.
	Float64			k2time;		///< Key time for @ref k2.
};


//----------------------------------------------------------------------------------------
/// @markInternal
//----------------------------------------------------------------------------------------
enum PseudoTrackType
{
	PSEUDO_UNDEF	= 0,	///< Undefined.
	PSEUDO_VALUE	= 1,	///< Value track.
	PSEUDO_DATA		= 2,	///< Data track.
	PSEUDO_PLUGIN = 3,	///< Plugin track.

	_PSEUDO
};

//----------------------------------------------------------------------------------------
/// @addtogroup CTRACK_CATEGORY
/// @ingroup group_enumeration
/// @{
/// Animation Track category.
/// @see CTrack::GetTrackCategory
/// @since 17.008
#define CTRACK_CATEGORY_VALUE	 1	///< Value track.
#define CTRACK_CATEGORY_DATA	 2	///< Data track.
#define CTRACK_CATEGORY_PLUGIN 3	///< Plugin track.
/// @}

//----------------------------------------------------------------------------------------
/// Represents an animation track.\n
/// Contains @link CCurve curves@endlink holding @link CKey keys@endlink.\n
/// @b Example: Creating <i>Align to Spline</i> tag and adding track with 2 keys:
/// @code
/// // Add align to spline tag to object
/// BaseTag* tag = op->MakeTag(Taligntospline);
/// if (tag)
/// {
///   // Allocate track for align to spline tag
///   CTrack* track = CTrack::Alloc(tag, DescID(ALIGNTOSPLINETAG_POSITION), "atsTrack");
///   if (track)
///   {
///     // Add track to align to spline tag
///     tag->AppendCTrack(track);
///
///     // Get track curve
///     CCurve* curve = track->GetCurve();
///     if (curve!=nullptr)
///     {
///       // Add first key
///       CKey* key = curve->AddKey(BaseTime(0.0));
///       key->SetValue(curve, 0.0);
///       // Add second key
///       key = curve->AddKey(BaseTime(1.0));
///       key->SetValue(curve, 0.9);
///     }
///   }
/// }
/// @endcode
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class CTrack : public BaseList2D
{
	INSTANCEOF(CTrack, BaseList2D)

	friend class CKey;
	friend class CCurve;
	friend class RootCTrack;
	friend class BaseList2D;

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{track}
	/// @param[in] bl									The object to allocate the track for.
	/// @param[in] id									The description ID to allocate the track for.\n
	///                               For instance a position track is allocated with:
	///                               @code
	///                               CTrack::Alloc(op, DescID(DescLevel(ID_BASEOBJECT_POSITION, DTYPE_VECTOR, 0), DescLevel(VECTOR_X, DTYPE_REAL, 0)), "X")
	///                               @endcode
	///                               A track of type LONG is allocated with:
	///                               @code
	///                               CTrack::Alloc(op, DescLevel(ID_BASEOBJECT_VISIBILITY_EDITOR, DTYPE_LONG, 0), "Visibility")
	///                               @endcode
	///                               Plugin and special tracks are allocated passing the ID:
	///                               @code
	///                               CTrack::Alloc(op, DescLevel(ID, ID, 0), "Plugin Track")
	///                               @endcode
	///                               IDs for @C4D's special tracks are ::CTpla, ::CTmorph and ::CTdynamicspline.
	/// @param[in] tname							The track name.
	/// @return												@allocReturn{track}
	//----------------------------------------------------------------------------------------
	static CTrack* Alloc(BaseList2D* bl, const DescID& id, const String& tname);

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{tracks}
	/// @param[in] bl									@theToDestruct{track}
	//----------------------------------------------------------------------------------------
	static void Free(CTrack*& bl);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next track in the list.
	/// @return												The next track, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{track}
	//----------------------------------------------------------------------------------------
	CTrack* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous track in the list.
	/// @return												The previous track, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{track}
	//----------------------------------------------------------------------------------------
	CTrack* GetPred();

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the track.
	/// @see BaseList2D::GetType()
	/// @return												The track type ID.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the disk type of the track.
	/// @see BaseList2D::GetDiskType()
	/// @return												The track disk type ID.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	/// @name Description ID
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the description ID of the track.
	/// @return												The description ID.
	//----------------------------------------------------------------------------------------
	const DescID& GetDescriptionID() const;

	//----------------------------------------------------------------------------------------
	/// Sets the description ID of the track.
	/// @param[in] object							The object for the track. @callerOwnsPointed{object}
	/// @param[in] id									The description ID of the track to set.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetDescriptionID(BaseList2D* object, const DescID& id);

	/// @}

	/// @name Before/After Loop
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the pre-track loop type.
	/// @return												The before loop type.
	//----------------------------------------------------------------------------------------
	CLOOP GetBefore() const;

	//----------------------------------------------------------------------------------------
	/// Sets the pre-track loop type.
	/// @param[in] type								The before loop type to set.
	//----------------------------------------------------------------------------------------
	void SetBefore(CLOOP type);

	//----------------------------------------------------------------------------------------
	/// Gets the post-track loop type.
	/// @return												The after loop type.
	//----------------------------------------------------------------------------------------
	CLOOP GetAfter() const;

	//----------------------------------------------------------------------------------------
	/// Sets the post-track loop type.
	/// @param[in] type								The after loop type to set.
	//----------------------------------------------------------------------------------------
	void SetAfter(CLOOP type);

	/// @}

	/// @name Time Track
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the time track.
	/// @param[in] doc								The document for the operation.
	/// @return												The time track.
	//----------------------------------------------------------------------------------------
	CTrack* GetTimeTrack(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the time track.
	/// @param[in] track							The time track to set.
	//----------------------------------------------------------------------------------------
	void SetTimeTrack(CTrack* track);

	/// @}

	/// @name Bits
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets all of the track's bit flags at once.
	/// @param[in] bits								The bits to set.
	//----------------------------------------------------------------------------------------
	virtual void SetAllBits(Int32 bits);

	//----------------------------------------------------------------------------------------
	/// Sets the track bit flags denoted by @formatParam{mask} to @formatConstant{true}.
	/// @param[in] mask								The bit mask of the flags.
	//----------------------------------------------------------------------------------------
	virtual void SetBit(Int32 mask);

	//----------------------------------------------------------------------------------------
	/// Deletes the track bit flags denoted by @formatParam{mask}, i.e. set the corresponding bit to @formatConstant{false}.
	/// @param[in] mask								The bit mask of the flags.
	//----------------------------------------------------------------------------------------
	virtual void DelBit(Int32 mask);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Clears the data.
	/// @note	Empties and resets sequence data but not the keys.
	//----------------------------------------------------------------------------------------
	void FlushData();

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the track.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @return												The cloned track. @callerOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// Gets a curve of the track.
	/// @param[in] type								The curve type: @enumerateEnum{CCURVE}
	/// @param[in] bCreate						If @formatConstant{true} a curve is created if none exists.
	/// @return												The curve. Can be @formatConstant{nullptr} if @formatParam{type} is something else than @ref CCURVE_CURVE.
	//----------------------------------------------------------------------------------------
	CCurve* GetCurve(CCURVE type = CCURVE_CURVE, Bool bCreate = true);

	//----------------------------------------------------------------------------------------
	/// Gets the track category.
	/// @return												The track category: @ref CTRACK_CATEGORY.
	//----------------------------------------------------------------------------------------
	Int32 GetTrackCategory() const;

	//----------------------------------------------------------------------------------------
	/// Gets the host object of the track.
	/// @since 18.002
	/// @return												The host object, or @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseList2D* GetObject() const;

	//----------------------------------------------------------------------------------------
	/// Gets the value of the track at @formatParam{time}.
	/// @since 17.008 Removed parameter 'fps'.
	/// @param[in] doc								The document for the track.
	/// @param[in] time								The time.
	/// @return												The track value.
	//----------------------------------------------------------------------------------------
	Float GetValue(BaseDocument* doc, const BaseTime& time);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool RemapInternal(Float64 time, Float64* ret_time, Int32* cyc, const BaseTime& t1, const BaseTime& t2, Int32 fps) const;

	//----------------------------------------------------------------------------------------
	/// Remaps @formatParam{time}.
	/// @param[in] time								The input time.
	/// @param[out] ret_time					The output time.
	/// @param[out] ret_cycle					The output cycle.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Remap(Float64 time, Float64* ret_time, Int32* ret_cycle) const;

	//----------------------------------------------------------------------------------------
	/// Removes the track from its list.
	/// @warning	When a track is removed the owner become responsible for freeing it or passing its ownership to another list.
	//----------------------------------------------------------------------------------------
	virtual void Remove();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk (HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool GetDParameter(const DescID& id, GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool SetDParameter(const DescID& id, const GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Float GetValue(BaseDocument* doc, const BaseTime& time, Int32 fps, GetValueCache* cache);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool FindCurve(CCurve* curveToFind, CCURVE& curveType);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool IsSynchronized() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetSynchronized(Bool synch);

	/// @}

protected:
	BaseList2D* _parentObject;

private:
	CTrack();
	~CTrack();

	CCurve	m_curve;
	CCurve*	m_hle_base;
	CCurve*	m_hle_curve;
	CCurve*	m_ss_curve1;

	// Multiple snapshots
	CCurve*	 m_ss_curve2;
	CCurve*	 m_ss_curve3;
	CCurve*	 m_ss_curve4;
	CCurve*	 m_ss_curve5;
	// Scale and Move HLE curve
	CCurve*	 m_hle_move;
	CCurve*	 m_hle_scale;

	DescID	 t_description;
	CLOOP		 m_before;
	CLOOP		 m_after;
	BaseLink m_timetrack;	// Optional time track

	Int32		 m_unit;			// Cached value
	Float		 m_step;			// Cached value

	// If TL height was modified
	Int32	m_HeightTL0;
	Int32	m_HeightTL1;
	Int32	m_HeightTL2;
	Int32	m_HeightTL3;

	// Temp values
	PseudoTrackType	t_pseudo;
	Bool						_synchronized;
};

#pragma pack (pop)
}

#endif	// C4D_CTRACK_H__
