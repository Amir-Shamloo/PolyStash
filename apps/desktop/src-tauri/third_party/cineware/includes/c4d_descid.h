/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DESCID_H__
#define C4D_DESCID_H__

#include "c4d_desclevel.h"
#include "c4d_customdatatype.h"
#include "c4d_baselink.h"

namespace cineware
{
#pragma pack (push, 8)

struct DescLevel;
class HyperFile;

//----------------------------------------------------------------------------------------
/// An ID class for description parameters. Contains a stack of DescLevel objects.\n
/// The description ID is used to exactly identify a parameter.\n
/// For first-level values (like a Float or Int value) it only consists of one DescLevel. For more complex datatypes like ::Vector and Gradient it can consist of 2 or more levels.
//----------------------------------------------------------------------------------------
class DescID : public iCustomDataType<DescID>
{
private:
	Int32			 level;
	DescLevel* subids;

	void InitStructure();
	void FreeStructure();
	friend const DescID operator + (const DescID& d1, const DescID& d2);
	const DescID& operator += (const DescID& d);
	const DescID& operator += (const DescLevel& d);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	DescID();

	//----------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param[in] src								Source DescID.
	//----------------------------------------------------------------------------------------
	DescID(const DescID& src);

	//----------------------------------------------------------------------------------------
	/// Creates a description ID with one level.
	/// @param[in] id1								First level.
	//----------------------------------------------------------------------------------------
	DescID(Int32 id1);

	//----------------------------------------------------------------------------------------
	/// Creates a description ID with one level.
	/// @param[in] id1								First level.
	//----------------------------------------------------------------------------------------
	DescID(const DescLevel& id1);

	//----------------------------------------------------------------------------------------
	/// Creates a description ID with two levels.
	/// @param[in] id1								First level.
	/// @param[in] id2								Second level.
	//----------------------------------------------------------------------------------------
	DescID(const DescLevel& id1, const DescLevel& id2);

	//----------------------------------------------------------------------------------------
	/// Creates an ID with three levels.
	/// @param[in] id1								First level.
	/// @param[in] id2								Second level.
	/// @param[in] id3								Third level.
	//----------------------------------------------------------------------------------------
	DescID(const DescLevel& id1, const DescLevel& id2, const DescLevel& id3);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~DescID();

	/// @}

	/// @name Set/Push/Pop Level
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the highest level to @formatParam{subid}.
	/// @param[in] subid							New top level.
	//----------------------------------------------------------------------------------------
	void SetId(const DescLevel& subid);

	//----------------------------------------------------------------------------------------
	/// Pushes a new level onto the stack.
	/// @param[in] subid							Level to push.
	//----------------------------------------------------------------------------------------
	void PushId(const DescLevel& subid);

	//----------------------------------------------------------------------------------------
	/// Pop the highest level from the stack.
	//----------------------------------------------------------------------------------------
	void PopId();

	/// @}

	/// @name Operator
	/// @{

	//----------------------------------------------------------------------------------------
	/// Accesses the level at position @formatParam{pos} in the stack.
	/// @param[in] pos								The position. @em 0 <= @formatParam{pos} < GetDepth()
	/// @return												The level at the specified position.
	//----------------------------------------------------------------------------------------
	const DescLevel& operator[] (Int32 pos) const;

	//----------------------------------------------------------------------------------------
	/// Assignment operator. Assign @formatParam{id} to the description ID.
	/// @param[in] id									Right operand. Source description ID.
	/// @return												Left operant.
	//----------------------------------------------------------------------------------------
	const DescID& operator = (const DescID& id);

	//----------------------------------------------------------------------------------------
	/// Equality operator. Checks if all levels are equal.
	/// @param[in] d									Right operand description ID.
	/// @return												@trueIfOtherwiseFalse{description IDs are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (const DescID& d) const;

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Checks if any level is different.
	/// @param[in] d									Right operand description ID.
	/// @return												@trueIfOtherwiseFalse{description IDs are not equal}
	//----------------------------------------------------------------------------------------
	Bool operator != (const DescID& d) const;

	//----------------------------------------------------------------------------------------
	/// Gets the result of popping levels from the bottom of the stack.
	/// @param[in] shift							Number of levels to pop. @em 0 <= @formatParam{shift} < GetDepth().
	/// @return												Resulting description ID after popping level(s).
	//----------------------------------------------------------------------------------------
	const DescID operator << (Int32 shift) const;

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf) const;

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the depth of the stack, i.e. the number of levels.
	/// @return												The depth of the stack.
	//----------------------------------------------------------------------------------------
	Int32 GetDepth() const;

	//----------------------------------------------------------------------------------------
	/// Checks if the description ID is part of @formatParam{cmp} and assign the length of the match to @formatParam{diff}.
	/// @param[in] cmp								The super description ID.
	/// @param[in] diff								If not @formatParam{nullptr} this is assigned the length of the match.
	/// @return												@trueIfOtherwiseFalse{the description ID matches the lowest part of @formatParam{cmp}}
	//----------------------------------------------------------------------------------------
	Bool IsPartOf(const DescID& cmp, Int32* diff) const;

	/// @}
};

/// @markInternal
class DescIDDataTypeClass
{
public:
	static Int32 GetId();
	static CustomDataType* AllocData();
	static void FreeData(CustomDataType* data);
	static Bool CopyData(const CustomDataType* src, CustomDataType* dst, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);
	static Bool WriteData(const CustomDataType* t_d, HyperFile* hf);
	static Bool ReadData(CustomDataType* t_d, HyperFile* hf, Int32 level);
	static const Char* GetResourceSym();
};

#pragma pack (pop)
}

#endif	// C4D_DESCID_H__
