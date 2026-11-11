/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_CKEY_H__
#define C4D_CKEY_H__

#include "c4d_baselist2d.h"
//#include "c4d_file.h"
#include "parameter_ids/animation/ckvalue.h"

namespace cineware
{
#pragma pack (push, 8)

/// @markInternal
#define GENAUIGKEIT 10000.0

class CCurve;

//----------------------------------------------------------------------------------------
/// Represents the animation key.\n
/// Can be inserted into the @link CCurve curve@endlink of a @link CTrack track@endlink.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class CKey : public BaseList2D
{
	INSTANCEOF(CKey, BaseList2D)

	friend class CCurve;
	friend class CTrack;
	friend class GeTempDynArray<CKey>;
	friend Bool _ConvertOldToNewAnim(BaseDocument* doc, BaseList2D* op);

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{key}
	/// @return												@allocReturn{key}
	//----------------------------------------------------------------------------------------
	static CKey* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{keys}
	/// @param[in,out] key						@theToDestruct{key}
	//----------------------------------------------------------------------------------------
	static void Free(CKey*& key);

	/// @}

	/// @name Get/Set Time
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the time of the key.
	/// @return												The time.
	//----------------------------------------------------------------------------------------
	BaseTime GetTime() const;

	//----------------------------------------------------------------------------------------
	/// Sets the time of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] t									The time to set.
	//----------------------------------------------------------------------------------------
	void SetTime(CCurve* seq, const BaseTime& t);

	//----------------------------------------------------------------------------------------
	/// Gets the left time of the key.
	/// @return												The left time.
	//----------------------------------------------------------------------------------------
	BaseTime GetTimeLeft() const;

	//----------------------------------------------------------------------------------------
	/// Sets the left time of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] t									The left time to set.
	//----------------------------------------------------------------------------------------
	void SetTimeLeft(CCurve* seq, const BaseTime& t);

	//----------------------------------------------------------------------------------------
	/// Gets the right time of the key.
	/// @return												The right time.
	//----------------------------------------------------------------------------------------
	BaseTime GetTimeRight() const;

	//----------------------------------------------------------------------------------------
	/// Sets the right time of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] t									The right time to set.
	//----------------------------------------------------------------------------------------
	void SetTimeRight(CCurve* seq, const BaseTime& t);

	/// @}

	/// @name Get/Set Value
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the value of the key.
	/// @note Use GetParameter() to read non-real values.\n
	/// @b Example: Reading of PLA keys.
	/// @code
	/// GeData data;
	/// if (!key->GetParameter(DescLevel(CK_PLA_DATA,CUSTOMDATATYPE_PLA,0), data, 0))
	///   return false;
	///
	/// PLAData* pla = (PLAData*)data.GetCustomDataType(CUSTOMDATATYPE_PLA);
	/// if (pla==nullptr)
	///   return false;
	///
	/// VariableTag	*ptag = nullptr,
	///             *htag = nullptr;
	///
	/// pla->GetVariableTags(ptag, htag);
	/// @endcode
	/// <tt>ptag</tt>/<tt>htag</tt> contain the point/tangent information.
	/// @return												The value.
	//----------------------------------------------------------------------------------------
	Float GetValue() const;

	//----------------------------------------------------------------------------------------
	/// Sets the value of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] v									The value to set.
	//----------------------------------------------------------------------------------------
	void SetValue(CCurve* seq, Float v);

	//----------------------------------------------------------------------------------------
	/// Gets the data of the key.
	/// @return												The data.
	//----------------------------------------------------------------------------------------
	const GeData& GetGeData() const;

	//----------------------------------------------------------------------------------------
	/// Sets the data of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] data								The new key data.
	//----------------------------------------------------------------------------------------
	void SetGeData(CCurve* seq, const GeData& data);

	//----------------------------------------------------------------------------------------
	/// Gets the left value of the key.
	/// @return												The left value.
	//----------------------------------------------------------------------------------------
	Float GetValueLeft() const;

	//----------------------------------------------------------------------------------------
	/// Sets the left value of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] v									The left value to set.
	//----------------------------------------------------------------------------------------
	void SetValueLeft(CCurve* seq, Float v);

	//----------------------------------------------------------------------------------------
	/// Gets the right value of the key.
	/// @return												The right value.
	//----------------------------------------------------------------------------------------
	Float GetValueRight() const;

	//----------------------------------------------------------------------------------------
	/// Sets the right value of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] v									The right value to set.
	//----------------------------------------------------------------------------------------
	void SetValueRight(CCurve* seq, Float v);

	/// @}

	/// @name Interpolation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the interpolation type of the key.
	/// @return												The interpolation type: @enumerateEnum{CINTERPOLATION}
	//----------------------------------------------------------------------------------------
	CINTERPOLATION GetInterpolation() const;

	//----------------------------------------------------------------------------------------
	/// Sets the interpolation type of the key.
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] inter							The interpolation type to set: @enumerateEnum{CINTERPOLATION}
	//----------------------------------------------------------------------------------------
	void SetInterpolation(CCurve* seq, CINTERPOLATION inter);

	//----------------------------------------------------------------------------------------
	/// Gets the quaternion interpolation type of the key.
	/// @since 18.002
	/// @return												The interpolation type: @enumerateEnum{ROTATIONINTERPOLATION}
	//----------------------------------------------------------------------------------------
	ROTATIONINTERPOLATION GetQuatInterpolation() const;

	//----------------------------------------------------------------------------------------
	/// Sets the quaternion interpolation type of the key.
	/// @since 18.002
	/// keys at the same time on other component curve will be modified
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] inter							The interpolation type to set: @enumerateEnum{ROTATIONINTERPOLATION}
	//----------------------------------------------------------------------------------------
	void SetQuatInterpolation(CCurve* seq, ROTATIONINTERPOLATION inter);
	/// @}

	/// @name Copy/Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Copies the key data to @formatParam{dest}.
	/// @param[in] destseq						The curve the destination key belongs to.
	/// @param[in] dest								The destination key.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool CopyDataTo(CCurve* destseq, CKey* dest, AliasTrans* trans) const;

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the key.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trn								An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @return												The cloned key. @callerOwnsPointed{key}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trn);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the track of the key.
	/// @return												The track.
	//----------------------------------------------------------------------------------------
	CTrack* GetTrack();

	//----------------------------------------------------------------------------------------
	/// Gets the curve of the key.
	/// @return												The curve.
	//----------------------------------------------------------------------------------------
	CCurve* GetCurve();

	//----------------------------------------------------------------------------------------
	/// Flushes the key: empties and resets data.
	//----------------------------------------------------------------------------------------
	void FlushData();

	//----------------------------------------------------------------------------------------
	/// Sets the key node bit.
	/// @param[in] bit								The node bit to set.
	/// @param[in] bitmode						The bit mode.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool ChangeNBit(NBIT bit, NBITCONTROL bitmode);

	//----------------------------------------------------------------------------------------
	/// Checks if the key is identical to @formatParam{k2}.
	/// @param[in] k2									The key to compare against. @callerOwnsPointed{key}
	/// @return												@trueIfOtherwiseFalse{the keys are identical}
	//----------------------------------------------------------------------------------------
	Bool Identical(CKey* k2);

	/// @}

	/// @name AutoTangent Mode
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the AutoTangent mode of the key.
	/// @since 17.008
	/// @return												The AutoTangent mode: @enumerateEnum{CAUTOMODE}
	//----------------------------------------------------------------------------------------
	CAUTOMODE GetAutomaticTangentMode() const;

	//----------------------------------------------------------------------------------------
	/// Sets the AutoTangent mode of the key.
	/// @since 17.008
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] autoMode						The AutoTangent mode to set: @enumerateEnum{CAUTOMODE}
	//----------------------------------------------------------------------------------------
	void SetAutomaticTangentMode(CCurve* seq, CAUTOMODE autoMode);

	/// @}

	/// @name Tangent Helpers
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets Time Left and adjusts Value so the angle stays the same.
	/// @since 17.008
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] t									The new left time to set.
	/// @return												@formatConstant{true} if successful. @formatConstant{false} if current Time Left is smaller than @c FLT_MIN and Value is not Zero (angle cannot stay the same).
	//----------------------------------------------------------------------------------------
	Bool SetTimeLeftAdjustValue(CCurve* seq, const BaseTime& t);

	//----------------------------------------------------------------------------------------
	/// Sets Time Right and adjusts Value so the angle stays the same.
	/// @since 17.008
	/// @param[in] seq								The curve the key belongs to.
	/// @param[in] t									The new right time to set.
	/// @return												@formatConstant{true} if successful. @formatConstant{false} if current Time Right is smaller than @c FLT_MIN and Value is not Zero (angle cannot stay the same).
	//----------------------------------------------------------------------------------------
	Bool SetTimeRightAdjustValue(CCurve* seq, const BaseTime& t);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

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
	void SetKeyPreset(CCurve* seq, CKEYPRESET preset);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	CKEYPRESET GetKeyPreset();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void UnifyTangent();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetQuatInterpolationI(CCurve* seq, ROTATIONINTERPOLATION inter, Bool synchronize = true);

	/// @}

protected:
	CCurve*	parentCurve;

private:
	CKey();
	~CKey();
	const CKey& operator = (const CKey& key);

	BaseTime m_time;

	// Only for value key.
	Float									m_value;
	Float									m_value_left, m_value_right;	// Only for spline mode.
	BaseTime							m_time_left, m_time_right;		// Only for spline mode.
	CINTERPOLATION				m_inter;
	CAUTOMODE							m_autoMode;
	ROTATIONINTERPOLATION m_quatInterp;

	// Note: All booleans like mute, clamp, link, lock, auto, zero are stored as NBITS.

	// Only for data key.
	GeData m_data;	// For PLA etc.
};


#pragma pack (pop)
}

#endif	// C4D_CKEY_H__
