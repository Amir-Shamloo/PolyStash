/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASESELECT_H__
#define C4D_BASESELECT_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct BaseSelectData
{
	Int32 a, b;
};

//----------------------------------------------------------------------------------------
/// Used to keep track of point and polygon selections, it may also be used to track other types of element selections.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseSelect
{
	friend class ItemListDataTypeClass;

protected:
	BaseSelectData* data;
	Int32						cnt;
	Int32						lseg;
	Int32						dirtycnt;
	Int32						lelement;

	Bool FindNum(Int32 num, Int32* last, Bool protect = true) const;
	Bool FlushData(Int32 num);
	Bool InsertData(Int32 num, Int32 a, Int32 b);

	void Write_API(HyperFile* hf);
	Bool Read_API (HyperFile* hf);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseSelect();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseSelect();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base selection}
	/// @return												@allocReturn{base selection}
	//----------------------------------------------------------------------------------------
	static BaseSelect* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base selections}
	/// @param[in,out] bs							@theToDestruct{base selection}
	//----------------------------------------------------------------------------------------
	static void Free(BaseSelect*& bs);

	/// @}

	/// @name Selection/Segment Count
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of selected elements.
	/// @return												The number of selected elements.
	//----------------------------------------------------------------------------------------
	Int32 GetCount();

	//----------------------------------------------------------------------------------------
	/// Gets the number of segments that contain elements.\n
	/// For example: The selections @em 0..@em 4, @em 6..@em 7, @em 9..@em 12 would be three segments. GetSegments() would return @em 3 and GetCount() would return @em 11.
	/// @return												The number of segments with selected elements.
	//----------------------------------------------------------------------------------------
	Int32 GetSegments();

	/// @}

	/// @name Select/Deselect/Toggle
	/// @{

	//----------------------------------------------------------------------------------------
	/// Selects an element.
	/// @param[in] num								The element index to select.
	/// @return												@trueIfOtherwiseFalse{the element was already selected}
	//----------------------------------------------------------------------------------------
	Bool Select(Int32 num);

	//----------------------------------------------------------------------------------------
	/// Selects all elements in the given range.
	/// @note	All previous selections are cleared.
	/// @param[in] min								The first element to select.
	/// @param[in] max								The last element in the range to select.
	/// @return												Success of selecting the elements.
	//----------------------------------------------------------------------------------------
	Bool SelectAll(Int32 min, Int32 max);

	//----------------------------------------------------------------------------------------
	/// Deselects an element.
	/// @param[in] num								The element index to deselect.
	/// @return												@trueIfOtherwiseFalse{the element was already deselected}
	//----------------------------------------------------------------------------------------
	Bool Deselect(Int32 num);

	//----------------------------------------------------------------------------------------
	/// Deselect all elements.
	/// @return												Success of deselecting all elements.
	//----------------------------------------------------------------------------------------
	Bool DeselectAll();

	//----------------------------------------------------------------------------------------
	/// Toggles the selection state of an element.
	/// @param[in] num								The element index to toggle.
	/// @return												Success of changing the selection state of the element.
	//----------------------------------------------------------------------------------------
	Bool Toggle(Int32 num);

	//----------------------------------------------------------------------------------------
	/// Toggles the selection state of all elements in the given range.
	/// @param[in] min								The first element to toggle.
	/// @param[in] max								The last element to toggle in the range.
	/// @return												Success of changing the selection state.
	//----------------------------------------------------------------------------------------
	Bool ToggleAll(Int32 min, Int32 max);

	//----------------------------------------------------------------------------------------
	/// Checks if all elements from @em 0 to @formatParam{num}-@em 1 are selected.
	/// @param[in] num								The last element to check.
	/// @return												@trueIfOtherwiseFalse{all the element sin the range @em 0 to @formatParam{num}-@em 1 are selected}
	//----------------------------------------------------------------------------------------
	Bool IsAllSelected(Int32 num) const;

	//----------------------------------------------------------------------------------------
	/// Gets the selected elements contained in a segment.
	/// @param[in] seg								The segment to get the elements for. @em 0 <= @formatParam{seg} < GetSegments()
	/// @param[out] a									Assigned the index of the first selected element.
	/// @param[out] b									Assigned the index of the last selected element.
	/// @return												Success of getting the range; only @formatConstant{false} if @formatParam{seg} is not @em 0 <= @formatParam{seg} < GetSegments()\n
	///																The spans are always sorted (spans with higher index have higher numbers for @formatParam{a}/@formatParam{b}), also @formatParam{b} is always >= @formatParam{a}.
	//----------------------------------------------------------------------------------------
	Bool GetRange(Int32 seg, Int32* a, Int32* b) const;

	//----------------------------------------------------------------------------------------
	/// Check the selection state of an element.\n
	/// To efficiently go through selections use the following code:
	/// @code
	/// Int32 seg = 0, a, b, i;
	///
	/// while (bs->GetRange(seg++, &a, &b))
	/// {
	///   for (i=a; i<=b; ++i)
	///   {
	///     // Do something. 'i' is the selected element.
	///   }
	/// }
	/// @endcode
	/// This is faster than:
	/// @code
	/// for (i=0; i<maxelements; i++)
	/// {
	///   if (bs->IsSelected(i))
	///   {
	///     // Do something.
	///   }
	/// }
	/// @endcode
	/// @param[in] num								The element index to get the state for.
	/// @return												@trueIfOtherwiseFalse{the element @formatParam{num} is selected}
	//----------------------------------------------------------------------------------------
	Bool IsSelected(Int32 num) const;

	//----------------------------------------------------------------------------------------
	/// Calculates which segment contain the element @formatParam{id} and assign it to @formatParam{segment}.
	/// @param[in] id									An element number.
	/// @param[in] segment						Assigned the found segment index.
	/// @return												Success of finding the segment for element @formatParam{id}.
	//----------------------------------------------------------------------------------------
	Bool FindSegment(Int32 id, Int32* segment);

	/// @}

	/// @name Copy/Merge/Cross
	/// @{

	//----------------------------------------------------------------------------------------
	/// Copies the selection elements to another BaseSelect.
	/// @param[out] dest							The destination selection.
	/// @return												Success of copying the selection elements.
	//----------------------------------------------------------------------------------------
	Bool CopyTo(BaseSelect* dest) const;

	//----------------------------------------------------------------------------------------
	/// Selects all elements that are selected in @formatParam{src}.
	/// @param[in] src								The source selection.
	/// @return												Success of merging the selection elements.
	//----------------------------------------------------------------------------------------
	Bool Merge(BaseSelect* src);

	//----------------------------------------------------------------------------------------
	/// Deselects all elements that are selected in @formatParam{src}.
	/// @param[in] src								The source selection.
	/// @return												Success of deselecting the selection elements.
	//----------------------------------------------------------------------------------------
	Bool Deselect(BaseSelect* src);

	//----------------------------------------------------------------------------------------
	/// Intersects all elements in @formatParam{src}.
	/// @param[in] src								The source selection.
	/// @return												Success of crossing the selection elements.
	//----------------------------------------------------------------------------------------
	Bool Cross(const BaseSelect* src);

	//----------------------------------------------------------------------------------------
	/// Makes a duplicate of the selection with its elements.
	/// @return												The cloned BaseSelect or @formatConstant{nullptr} if failed.
	//----------------------------------------------------------------------------------------
	BaseSelect* GetClone() const;

	//----------------------------------------------------------------------------------------
	/// Compares the selection with another selection.
	/// @param[in] compare						The selection to compare with.
	/// @return												@trueIfOtherwiseFalse{the elements in the selections are exactly the same}
	//----------------------------------------------------------------------------------------
	Bool IsEqual(const BaseSelect& compare) const;

	/// @}

	/// @name Conversion from/to Array
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a number of selected elements from an array. The elements in the array are interpreted as ::Bool: @em 0 means the element is unselected, and @em 1 means it is selected.
	/// @note	The old selection will completely be overridden.
	/// @param[in] selection					An array of elements to select. @callerOwnsPointed{array}
	/// @param[in] count							The number of elements in the array.
	/// @return												Success of selecting the elements from the array.
	//----------------------------------------------------------------------------------------
	Bool FromArray(UChar* selection, Int32 count);

	//----------------------------------------------------------------------------------------
	/// Gets an array of selected elements. The elements in the array are interpreted as ::Bool: @em 0 means the element is unselected, and @em 1 means it is selected.\n
	/// The array is created with NewMemClear() and must be freed with DeleteMem() afterward.
	/// @param[in] count							The number of elements to place into the array.
	/// @return												The array containing the selected elements or @formatConstant{nullptr} if failed. @callerOwnsPointed{array}
	//----------------------------------------------------------------------------------------
	UChar* ToArray(Int32 count);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32* GetData();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool CopyFrom(BaseSelectData* ndata, Int32 ncnt);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASESELECT_H__
