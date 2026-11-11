/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CCURVE_H__
#define C4D_CCURVE_H__

#include "c4d_baselist2d.h"
#include "c4d_rootckey.h"
#include "c4d_ckey.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct GetValueCache
{
	GetValueCache()
	{
		i1 = NOTOK;
		i2 = NOTOK;
	};

	Int32 i1;
	Int32 i2;
};

//----------------------------------------------------------------------------------------
/// Represents the animation curve.\n
/// Belongs to a @link CTrack track@endlink and contains @link CKey keys@endlink.
/// @note Cannot be allocated.
//----------------------------------------------------------------------------------------
class CCurve : public BaseList2D
{
	INSTANCEOF(CCurve, BaseList2D)

	friend class CKey;
	friend class CTrack;

public:
	/// @name Key
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of keys in the curve.
	/// @return												The number of keys.
	//----------------------------------------------------------------------------------------
	Int32 GetKeyCount() const;

	//----------------------------------------------------------------------------------------
	/// Gets the const key at @formatParam{index} in the curve.
	/// @param[in] index							The key index: @em 0 <= @formatParam{index} < GetKeyCount()
	/// @return												The const key, or @formatConstant{nullptr}. @theOwnsPointed{curve,key}
	//----------------------------------------------------------------------------------------
	const CKey* GetKey(Int32 index) const;

	//----------------------------------------------------------------------------------------
	/// Gets the key at @formatParam{index} in the curve.
	/// @param[in] index							The key index: @em 0 <= @formatParam{index} < GetKeyCount()
	/// @return												The key, or @formatConstant{nullptr}. @theOwnsPointed{curve,key}
	//----------------------------------------------------------------------------------------
	CKey* GetKey(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Finds the const key at the given @formatParam{time}.
	/// @param[in] time								The time to find the key at.
	/// @param[out] idx								Assigned the index of the found key.
	/// @param[in] match							The search method: @enumerateEnum{FINDANIM}
	/// @return												The found const key, or @formatConstant{nullptr}. @theOwnsPointed{curve,key}
	//----------------------------------------------------------------------------------------
	const CKey* FindKey(const BaseTime& time, Int32* idx = nullptr, FINDANIM match = FINDANIM_EXACT) const;

	//----------------------------------------------------------------------------------------
	/// Finds the key at the given @formatParam{time}.
	/// @param[in] time								The time to find the key at.
	/// @param[out] idx								Assigned the index of the found key.
	/// @param[in] match							The search method: @enumerateEnum{FINDANIM}
	/// @return												The found key, or @formatConstant{nullptr}. @theOwnsPointed{curve,key}
	//----------------------------------------------------------------------------------------
	CKey* FindKey(const BaseTime& time, Int32* idx = nullptr, FINDANIM match = FINDANIM_EXACT);

	//----------------------------------------------------------------------------------------
	/// Adds a key to the curve.
	/// @param[in] time								The time to add the key at.
	/// @param[out] nidx							Assigned the index of the added key.
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	/// @return												The added key. @theOwnsPointed{curve,key}
	//----------------------------------------------------------------------------------------
	CKey* AddKey(const BaseTime& time, Int32* nidx = nullptr, Bool synchronizeKeys = false);

	//----------------------------------------------------------------------------------------
	/// Adds a key to the curve but retains the curve's current curvature.
	/// @since 17.008 Removed parameter 'bUndo'.
	/// @param[in] time								The time to add the key at.
	/// @param[out] nidx							Assigned the new key index.
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	/// @return												The added key, or @formatConstant{nullptr} if it failed.
	//----------------------------------------------------------------------------------------
	CKey* AddKeyAdaptTangent(const BaseTime& time, Int32* nidx = nullptr, Bool synchronizeKeys = false);

	//----------------------------------------------------------------------------------------
	/// Inserts a key into the curve.
	/// @param[in] ckey								The key to insert. The curve takes over the ownership of the pointed key.
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	/// @return												@trueIfOtherwiseFalse{the key was inserted}
	//----------------------------------------------------------------------------------------
	Bool InsertKey(const CKey* ckey, Bool synchronizeKeys = false);

	//----------------------------------------------------------------------------------------
	/// Deletes a key from the curve.
	/// @param[in] index							The index of the key to delete: @em 0 <= @formatParam{index} < GetKeyCount()
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	/// @return												@trueIfOtherwiseFalse{the key was deleted}
	//----------------------------------------------------------------------------------------
	Bool DelKey(Int32 index, Bool synchronizeKeys = false);

	//----------------------------------------------------------------------------------------
	/// Moves a key in the curve.
	/// @param[in] time								The time to move the key to.
	/// @param[in] idx								The index of the key to move: @em 0 <= @formatParam{index} < GetKeyCount()
	/// @param[in] dseq								An optional destination curve to move the key to.
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	/// @return												The new index of the moved key.
	//----------------------------------------------------------------------------------------
	Int32 MoveKey(const BaseTime& time, Int32 idx, CCurve* dseq = nullptr, Bool synchronizeKeys = false);

	//----------------------------------------------------------------------------------------
	/// Removes all keys from the curve.
	/// @param[in] synchronizeKeys		If @formatConstant{true} the routine is called on other components of the vector (if valid). Curve must be part of a Track. Since 18.002
	//----------------------------------------------------------------------------------------
	void FlushKeys(Bool synchronizeKeys = false);

	/// @}

	/// @name Calculation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Calculates the Hermite spline between two sets of key values.
	/// @note Does not take into account any modes (like zero length), so this method should normally not be used. Call GetTangents() instead.
	/// @warning Only valid for 'Spline' interpolation curves.
	/// @param[in] time								The time to evaluate.
	/// @param[in] t1									The first time.
	/// @param[in] t2									The second time.
	/// @param[in] val1								The first value.
	/// @param[in] val2								The second value.
	/// @param[in] tan1_val						The first tangent value.
	/// @param[in] tan2_val						The second tangent value.
	/// @param[in] tan1_t							The first tangent time.
	/// @param[in] tan2_t							The second tangent time.
	/// @param[in] deriv							If @formatConstant{true} the derivative is calculated instead of the value.
	/// @return												The spline value at the given coordinates/parameters.
	//----------------------------------------------------------------------------------------
	Float64 CalcHermite(Float64 time, Float64 t1, Float64 t2, Float64 val1, Float64 val2, Float64 tan1_val, Float64 tan2_val, Float64 tan1_t, Float64 tan2_t, Bool deriv) const;

	//----------------------------------------------------------------------------------------
	/// Calculates the soft tangents (i.e. auto interpolation) around a key.
	/// @param[in] kidx								The key index: @em 0 <= @formatParam{kidx} < GetKeyCount()
	/// @param[out] vl								Assigned the left value.
	/// @param[out] vr								Assigned the right value.
	/// @param[out] tl								Assigned the left time.
	/// @param[out] tr								Assigned the right time.
	//----------------------------------------------------------------------------------------
	void CalcSoftTangents(Int32 kidx, Float* vl, Float* vr, BaseTime* tl, BaseTime* tr) const;

	//----------------------------------------------------------------------------------------
	/// Computes the tangents of a key, taking into account all options like zero slope, link slope etc.
	/// @param[in] kidx								The key index: @em 0 <= @formatParam{kidx} < GetKeyCount()
	/// @param[out] vl								Assigned the left value.
	/// @param[out] vr								Assigned the right value.
	/// @param[out] tl								Assigned the left time.
	/// @param[out] tr								Assigned the right time.
	//----------------------------------------------------------------------------------------
	void GetTangents(Int32 kidx, Float64* vl, Float64* vr, Float64* tl, Float64* tr) const;

	//----------------------------------------------------------------------------------------
	/// Gets the value calculated at @formatParam{time}, taking into account things like time curves.
	/// @since 17.008 Removed parameter 'fps'.
	/// @param[in] time								The time to calculate the value at.
	/// @return												The calculated value.
	//----------------------------------------------------------------------------------------
	Float GetValue(const BaseTime& time) const;

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the track of the curve.
	/// @return												The track.
	//----------------------------------------------------------------------------------------
	CTrack* GetTrack();

	//----------------------------------------------------------------------------------------
	/// Sets the defaults for key @formatParam{kidx} of the curve.\n
	/// This includes lock, mute, clamp, break, auto properties, interpolation and tangents.\n
	/// This setup a value and complete the missing properties with the defaults.
	/// @since 17.008
	/// @param[in] doc								The curve's document.
	/// @param[in] kidx								The key index.
	//----------------------------------------------------------------------------------------
	void SetKeyDefault(BaseDocument* doc, Int32 kidx);

	/// @}

	/// @name Start/End Time
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the start time of the curve.
	/// @return												The start time of the curve.
	//----------------------------------------------------------------------------------------
	BaseTime GetStartTime() const;

	//----------------------------------------------------------------------------------------
	/// Gets the end time of the curve.
	/// @return												The end time of the curve.
	//----------------------------------------------------------------------------------------
	BaseTime GetEndTime() const;

	/// @}

	/// @name Unmuted
	/// @{

	//----------------------------------------------------------------------------------------
	/// Finds the next unmuted key (read-only).
	/// @param[in] idx								The key index to start the search from: 0 <= @formatParam{idx} < GetKeyCount()
	/// @param[out] ret_idx						Assigned the found key index.
	/// @return												The next unmuted key, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	const CKey* FindNextUnmuted(Int32 idx, Int32* ret_idx = nullptr) const;

	//----------------------------------------------------------------------------------------
	/// Finds the next unmuted key (writable).
	/// @param[in] idx								The key index to start the search from: 0 <= @formatParam{idx} < GetKeyCount()
	/// @param[out] ret_idx						Assigned the found key index.
	/// @return												The next unmuted key, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	CKey* FindNextUnmuted(Int32 idx, Int32* ret_idx = nullptr);

	//----------------------------------------------------------------------------------------
	/// Finds the previous unmuted key (read-only).
	/// @param[in] idx								The key index to start the search from: 0 <= @formatParam{idx} < GetKeyCount()
	/// @param[out] ret_idx						Assigned the found key index.
	/// @return												The previous unmuted key, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	const CKey* FindPrevUnmuted(Int32 idx, Int32* ret_idx = nullptr) const;

	//----------------------------------------------------------------------------------------
	/// Finds the previous unmuted key (writable).
	/// @param[in] idx								The key index to start the search from: 0 <= @formatParam{idx} < GetKeyCount()
	/// @param[out] ret_idx						Assigned the found key index.
	/// @return												The previous unmuted key, or @formatConstant{nullptr} if there is none.
	//----------------------------------------------------------------------------------------
	CKey* FindPrevUnmuted(Int32 idx, Int32* ret_idx = nullptr);

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
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Float	GetValueCached(const BaseTime& time, Float64 _time, GetValueCache* cache) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void FlushKeysI();

	/// @}

protected:
	void CalcRemoveOvershoot(Int32 kidx, Float* vl, Float* vr, BaseTime* tl, BaseTime* tr) const;
	void AdjustTangentAutoWeight(Int32 keyIndex);
	void CalcAutoWeight(Int32 kidx, Float* vl, Float* vr, BaseTime* tl, BaseTime* tr) const;

private:
	CCurve();
	~CCurve();

	const CCurve& operator = (const CCurve& seq);

	GeTempDynArray<CKey> m_keys;
	RootCKey						 m_root_key;
	CTrack*							 _parentTrack;

	Bool _LoadKeysHack();
	Float64 GetPercent(Float64 prevValue, Float64 nextValue) const;
	CKey* InsertKeyI(CKey* ckey, Int32* idx);

	virtual void InsertBefore(GeListNode* op) { }
	virtual void InsertAfter(GeListNode* op) { }
	virtual void Remove() { }
	virtual void InsertUnder(GeListNode* op) { }
	virtual void InsertUnderLast(GeListNode* op) { }
	virtual void FlushChilds() { }
};

#pragma pack (pop)
}

#endif	// C4D_CCURVE_H__
