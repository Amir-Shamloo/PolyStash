/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_RENDERDATA_H__
#define C4D_RENDERDATA_H__

#include "c4d_baselist4d.h"
#include "c4d_rootvideopost.h"
#include "c4d_rootmultipass.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseVideoPost;
class MultipassObject;

//----------------------------------------------------------------------------------------
/// Contains a container with all the render settings/options.\n
/// Several render data instances can be added to a document. The active one will be used for rendering.
/// @addAllocFreeNote
/// @see	@C4D Render Settings dialog and its documentation for more information.
///				Render settings in @C4D are description based and are defined in @em parameter_ids\drendersettings.h
//----------------------------------------------------------------------------------------
class RenderData : public BaseList4D
{
	INSTANCEOF(RenderData, BaseList4D)

	RootVideoPost mp_posteffect;
	RootMultipass mp_multipass;

	BaseContainer inheritance_container;

protected:
	RenderData();

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{render data}
	/// @return												@allocReturn{render data}
	//----------------------------------------------------------------------------------------
	static RenderData* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{render data}
	/// @param[in,out] rd							@theToDestruct{render data}
	//----------------------------------------------------------------------------------------
	static void Free(RenderData*& rd);

	//----------------------------------------------------------------------------------------
	/// Gets the object type for a render data.
	/// @note	Always returns @ref Rbase.
	/// @return												The object type for a RenderData.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next render data in the list.
	/// @return												The next render data, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous render data in the list.
	/// @return												The previous render data, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetPred();

	//----------------------------------------------------------------------------------------
	/// Gets the parent render data of the list node.
	/// @return												The parent render data, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child render data of the list node.
	/// @return												The first child render data, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child render data of the list node.
	/// @return												The last child render data, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetDownLast();

	/// @}

	/// @name VideoPost List
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the first video post of the render settings.
	/// @return												The first video post. @theOwnsPointed{render data,video post}
	//----------------------------------------------------------------------------------------
	BaseVideoPost* GetFirstVideoPost();

	//----------------------------------------------------------------------------------------
	/// Inserts video post @formatParam{pvp} into the render settings.\n
	/// The insertion position can be specified by the @formatParam{pred} parameter, inserting video post @formatParam{pvp} below it.
	/// @param[in] pvp								The video post to insert. The render data takes over the ownership of the pointed video post.
	/// @param[in] pred								The video post to insert @formatParam{pvp} after, or @formatConstant{nullptr} to insert it first. @callerOwnsPointed{video post}
	//----------------------------------------------------------------------------------------
	void InsertVideoPost(BaseVideoPost* pvp, BaseVideoPost* pred);

	//----------------------------------------------------------------------------------------
	/// Inserts @formatParam{pvp} as the last video post in the render settings.
	/// @param[in] pvp								The video post to insert. The render data takes over the ownership of the pointed video post.
	//----------------------------------------------------------------------------------------
	void InsertVideoPostLast(BaseVideoPost* pvp);

	/// @}

	/// @name Multipass List
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the first multipass channel of the render settings.\n.
	/// @return												The first multipass channel, or @formatConstant{nullptr} if no multipass channels are contained in the render setting. @theOwnsPointed{render data,multipass channel}
	//----------------------------------------------------------------------------------------
	MultipassObject* GetFirstMultipass();

	//----------------------------------------------------------------------------------------
	/// Inserts multipass channel @formatParam{pmp} into the render settings.\n
	/// The insertion position can be specified by the @formatParam{pred} parameter, inserting multipass channel @formatParam{pmp} below it.
	/// @param[in] pmp								The multipass channel to insert. The render data takes over the ownership of the pointed object.
	/// @param[in] pred								The multipass channel to insert @formatParam{pvp} after, or @formatConstant{nullptr} to insert it first. @callerOwnsPointed{multipass channel}
	//----------------------------------------------------------------------------------------
	void InsertMultipass(MultipassObject* pmp, MultipassObject* pred);

	//----------------------------------------------------------------------------------------
	/// Inserts @formatParam{pmp} as last the multipass channel into the render settings.
	/// @param[in] pmp								The multipass channel to insert. The render data takes over the ownership of the pointed multipass channel.
	//----------------------------------------------------------------------------------------
	void InsertMultipassLast(MultipassObject* pmp);

	/// @}

	/// @name Multipass List
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the resolution and aspect ratio of the render settings.
	/// @param[out] width							Assigned the render width.
	/// @param[out] height						Assigned the render height.
	/// @param[out] pixelAspect				Assigned the pixel aspect ratio.
	/// @param[out] filmAspect				Assigned the film aspect ratio.
	//----------------------------------------------------------------------------------------
	void GetResolution(Float& width, Float& height, Float& pixelAspect, Float& filmAspect);

	//----------------------------------------------------------------------------------------
	/// Sets the resolution and aspect ratio of the render settings.
	/// @param[in] width							The render width to set.
	/// @param[in] height							The render height to set.
	/// @param[in] pixelAspect				The render pixel aspect ratio to set.
	/// @param[in] filmAspect					The render film aspect ratio to set.
	//----------------------------------------------------------------------------------------
	void SetResolution(Float width, Float height, Float pixelAspect, Float filmAspect);

	/// @}

	/// @name Multipass List
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the animation settings of the render data.
	/// @param[out] fseq							Assigned the frame sequence: @ref RDATA_FRAMESEQUENCE.
	/// @param[out] fps								Assigned the frame rate (Frames per Second).
	/// @param[out] start							Assigned the start time.
	/// @param[out] end								Assigned the end time.
	/// @param[out] field							Assigned the interlace field: @ref RDATA_FIELD.
	//----------------------------------------------------------------------------------------
	void GetAnimationSettings(Int32& fseq, Float& fps, BaseTime& start, BaseTime& end, Int32& field);

	//----------------------------------------------------------------------------------------
	/// Sets the animation settings of the render data.
	/// @param[in] fseq								The frame sequence to set set: @ref RDATA_FRAMESEQUENCE.
	/// @param[in] fps								The frame rate (Frames per Second) to set.
	/// @param[in] start							The start time to set.
	/// @param[in] end								The end time to set.
	/// @param[in] field							The interlace field to set: @ref RDATA_FIELD.
	//----------------------------------------------------------------------------------------
	void SetAnimationSettings(Int32 fseq, Float fps, BaseTime start, BaseTime end, Int32 field);

	/// @}

	// LONG fseq:  RDATA_FRAMESEQUENCE_MANUAL, RDATA_FRAMESEQUENCE_CURRENTFRAME, RDATA_FRAMESEQUENCE_ALLFRAMES, RDATA_FRAMESEQUENCE_PREVIEWRANGE
	// LONG field: RDATA_FIELD_NONE, RDATA_FIELD_EVEN, RDATA_FIELD_ODD

	/// @name Color Profile
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the RenderData to use @C4D default linear color profile.
	//----------------------------------------------------------------------------------------
	void SetImageColorProfileToDefaultLinearRGB();

	//----------------------------------------------------------------------------------------
	/// Sets the RenderData to use @C4D default linear grayscale color profile.
	//----------------------------------------------------------------------------------------
	void SetImageColorProfileToDefaultLinearGray();

	//----------------------------------------------------------------------------------------
	/// Sets the RenderData to use @C4D default sRGB color profile.
	//----------------------------------------------------------------------------------------
	void SetImageColorProfileToDefaultSRGB();

	//----------------------------------------------------------------------------------------
	/// Sets the RenderData to use @C4D default sRGB grayscale color profile.
	//----------------------------------------------------------------------------------------
	void SetImageColorProfileToDefaultSGray();

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the render data.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trn								An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												The cloned render data. @callerOwnsPointed{render data}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trn);

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
	virtual Bool HandleSubChunk (HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_RENDERDATA_H__
