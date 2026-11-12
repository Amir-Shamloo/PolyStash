/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SEGMENTTAG_H__
#define C4D_SEGMENTTAG_H__

#include "c4d_basetag.h"

namespace cineware
{
#pragma pack (push, 8)

struct Segment;

//----------------------------------------------------------------------------------------
/// Represents @C4D's hidden segment tag.
//----------------------------------------------------------------------------------------
class SegmentTag : public BaseTag
{
	INSTANCEOF(SegmentTag, BaseTag)

private:
	Segment* pSegmentArray;
	Int32		 lSegmentCount;

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	SegmentTag();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~SegmentTag();

	/// @}

	/// @name Resize
	/// @{

	//----------------------------------------------------------------------------------------
	/// Changes the number of points and segments.
	/// @param[in] pcnt								The new point count.
	/// @param[in] scnt								The new segment count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool ResizeObject(Int32 pcnt, Int32 scnt);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of segments.
	/// @return												The number of segments.
	//----------------------------------------------------------------------------------------
	Int32 GetCount();

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the segment buffer.
	/// @return												A pointer to the segment buffer. @theOwnsPointed{segment tag,buffer}
	//----------------------------------------------------------------------------------------
	Segment* GetSegment();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Int32 GetInfo();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_SEGMENTTAG_H__
