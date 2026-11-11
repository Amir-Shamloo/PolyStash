/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_STRING_H__
#define C4D_STRING_H__

#include "c4d_system.h"
#include "ge_math.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup STRINGENCODING
/// @ingroup group_enumeration
/// @{
/// String encoding types.
enum STRINGENCODING
{
	STRINGENCODING_XBIT		 = 0,	///< An OS dependent encoding. Only use this if passing data to OS functions. The actual coding is unspecified.
	STRINGENCODING_8BIT		 = 1,	///< 8-bit ASCII. Discards Unicode information.
	STRINGENCODING_7BIT		 = 2,	///< 7-bit ASCII. Does not understand any international characters, e.g. \"&aring;&auml;&uuml;&ouml;\".
	STRINGENCODING_7BITHEX = 3,	///< 7-bit ASCII, with any non <i>7</i>-bit characters encoded in the text as e.g. "\uEFA0".
	STRINGENCODING_UTF8		 = 4,	///< UTF-8.
	STRINGENCODING_HTML		 = 5	///< HTML encoding.
} ENUM_END_LIST(STRINGENCODING);
/// @}

//----------------------------------------------------------------------------------------
/// A String is a variable-length sequence of characters. String provides methods and operators to simplify using character sequences.
/// @note	@C4D strings are fully using Unicode characters.
//----------------------------------------------------------------------------------------
class String
{
private:
	Int32		Size;									///< The actual allocated size. @markPrivate @markInternal
	Int32		Len;									///< The number of characters. @markPrivate @markInternal
	UInt16*	Txt;									///< The string text data. @markPrivate @markInternal
	void*		future_enhancements;	///< @markPrivate @markInternal

protected:
	void IncSize(Int32 nSize);	///< @markPrivate @markInternal

public:
	//----------------------------------------------------------------------------------------
	/// A PChar represents a character in a String.
	//----------------------------------------------------------------------------------------
	class PChar
	{
		friend class String;

private:
		String*	_str;										///< @markPrivate @markInternal
		Int32		_pos;										///< @markPrivate @markInternal

		PChar(String* str, Int32 pos);	///< @markPrivate @markInternal

public:
		//----------------------------------------------------------------------------------------
		/// Assigns the character.
		/// @param[in] rhs								A character.
		//----------------------------------------------------------------------------------------
		PChar& operator = (const PChar& rhs);

		//----------------------------------------------------------------------------------------
		/// Assigns the character.
		/// @param[in] c									A character.
		//----------------------------------------------------------------------------------------
		PChar& operator = (UInt16 c);

		//----------------------------------------------------------------------------------------
		/// Gets the character.
		/// @return												The character.
		//----------------------------------------------------------------------------------------
		operator UInt16() const;
	};

	friend class String::PChar;

	/// @name Constructors/Destructors
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	String();

	//----------------------------------------------------------------------------------------
	/// Copy constructor.
	/// @param[in] str								The characters used to construct the String.
	//----------------------------------------------------------------------------------------
	String(const String& str);

	//----------------------------------------------------------------------------------------
	/// Constructs the String with the characters in @formatParam{Cstr}.
	/// @param[in] cStr								The character array to initialize the string from. Must be formatted using the @ref STRINGENCODING_XBIT scheme. The pointed array is copied.
	/// @param[in] type								The string encoding.
	//----------------------------------------------------------------------------------------
	String(const Char* cStr, STRINGENCODING type = STRINGENCODING_XBIT);

	//----------------------------------------------------------------------------------------
	/// Constructs the string with the Unicode characters in @formatParam{Ustr}.
	/// @param[in] uStr								The Unicode character array to initialize the string from. The pointed array is copied.
	//----------------------------------------------------------------------------------------
	String(const UInt16* uStr);

	//----------------------------------------------------------------------------------------
	/// Constructs the string with the Unicode characters in @formatParam{Ustr}.
	/// @param[in] uStr								The Unicode character array to initialize the string from. The pointed array is copied.
	/// @param[in] count							The length of @formatParam{Ustr} string.
	//----------------------------------------------------------------------------------------
	String(const UInt16* uStr, Int32 count);

	//----------------------------------------------------------------------------------------
	/// Constructs a string filled with the same character.
	/// @param[in] count							The number of characters.
	/// @param[in] fillChar						The fill character.
	//----------------------------------------------------------------------------------------
	String(Int32 count, UInt16 fillChar);

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~String();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Gets the number of characters in the string.
	/// @return												The number of characters in the string.
	//----------------------------------------------------------------------------------------
	Int32 GetLength() const;

	//----------------------------------------------------------------------------------------
	/// Checks if the string has any characters in it.
	/// @since R20 Was Content() before.
	/// @return												@trueIfOtherwiseFalse{the string is not empty}
	//----------------------------------------------------------------------------------------
	Bool IsPopulated() const;

	//----------------------------------------------------------------------------------------
	/// Initializes the string.
	/// @param[in] count							The length of the string.
	/// @param[in] fillChar						The fill character.
	//----------------------------------------------------------------------------------------
	void Init(Int32 count = 0, UInt16 fillChar = 0);

	//----------------------------------------------------------------------------------------
	/// Resizes the string according to its length. Optimizes the memory usage after delete, initialize etc.
	//----------------------------------------------------------------------------------------
	void ShrinkToLen();

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Assigns the characters from @formatParam{Str}.
	/// @warning	The assignment operators makes string copies.
	/// @param[in] str								The source string to assign.
	/// @return												The string that was set. Assigned to the left-hand operand.
	//----------------------------------------------------------------------------------------
	const String& operator = (const String& str);

	//----------------------------------------------------------------------------------------
	/// Concatenates two strings and return the resulting string.
	/// @param[in] str1								The left-hand operand string.
	/// @param[in] str2								The right-hand operand string.
	/// @return												The concatenated string.
	//----------------------------------------------------------------------------------------
	friend const String operator + (const String& str1, const String& str2);

	//----------------------------------------------------------------------------------------
	/// Concatenates the strings and set the left-hand operand to the result.
	/// @warning	The assignment operators makes string copies.
	/// @param[in] str								The right-hand operand string.
	/// @return												The concatenated string. Assigned to the left-hand operand.
	//----------------------------------------------------------------------------------------
	const String& operator += (const String& str);

	//----------------------------------------------------------------------------------------
	/// Compares the strings to see if the the left-hand string is less than the right-hand string.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator < (const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Compares the strings to see if the the left-hand string is greater than the right-hand string.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator > (const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Compares the strings to see if the the left-hand string is less than or the same as right-hand string.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator <= (const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Compares the strings to see if the left-hand string is greater than or the same as the right-hand string.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator >= (const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Check if strings are equal.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												@trueIfOtherwiseFalse{strings are equal}
	//----------------------------------------------------------------------------------------
	Bool operator == (const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Checks if strings are different.
	/// @note	The comparison is done alphanumerically and case-sensitive.
	/// @param[in] str								The right-hand operand string.
	/// @return												@trueIfOtherwiseFalse{strings are different}
	//----------------------------------------------------------------------------------------
	Bool operator != (const String& str) const;

	/// @}

	/// @name Comparison
	/// @{

	//----------------------------------------------------------------------------------------
	/// Compares the string with another string and returns their relationship.
	/// @note	Comparison is case sensitive.
	/// @param[in] str								The string to compare against.
	/// @return												@em 0 if the strings are identical, < @em 0 if the string is less than @formatParam{str}, or > @em 0 if the string is greater than @formatParam{str}.
	//----------------------------------------------------------------------------------------
	Int32 Compare(const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Compares part of the string, specified by the position @formatParam{pos} and the length @formatParam{cnt}, with @formatParam{str}. Equivalent to <code>SubStr(pos, cnt).Compare(str)</code>.
	/// @param[in] str								The string to compare against.
	/// @param[in] count							The number of characters of the part.
	/// @param[in] pos								The position of the part.
	/// @return												@em 0 if the strings are identical, < @em 0 if the string is less than @formatParam{str}, or > @em 0 if the string is greater than @formatParam{str}.
	//----------------------------------------------------------------------------------------
	Int32 ComparePart(const String& str, Int32 count, Int32 pos) const;

	//----------------------------------------------------------------------------------------
	/// Compares the String object with another string and returns their relationship, this is case insensitive.
	/// @note	Comparison is case insensitive. Special characters (ASCII value >= @em 128) are not handled case-insensitive (like @em '&Ouml;', @em '&auml;', @em '&szlig;').
	/// @param[in] str								The string to compare against.
	/// @return												@em 0 if the strings are identical, < @em 0 if the string is less than @formatParam{Str}, or > @em 0 if the string is greater than @formatParam{Str}.
	//----------------------------------------------------------------------------------------
	Int32 LexCompare(const String& str) const;

	//----------------------------------------------------------------------------------------
	/// Incremental compare: the bigger the absolute return value, the bigger the difference.
	/// @param[in] str								The string to compare against.
	/// @return												@em 0 if the strings are identical, < @em 0 if the string is less than @formatParam{Str}, or > @em 0 if the string is greater than @formatParam{Str}.
	//----------------------------------------------------------------------------------------
	Int32 RelCompare(const String& str) const;

	/// @}

	/// @name Find
	/// @{

	//----------------------------------------------------------------------------------------
	/// Searches the string for the first match of the substring.
	/// @param[in] str								Substring to search for.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Starting position in the string for the search. Must be @em 0 <= @formatParam{Start} < GetLength().
	/// @return												@trueIfOtherwiseFalse{the substring was found}
	//----------------------------------------------------------------------------------------
	Bool FindFirst(const String& str, Int32* pos, Int32 start = 0) const;

	//----------------------------------------------------------------------------------------
	/// Searches the string for the last match of the substring.
	/// @param[in] str								Substring to search for.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Position to start from when searching backwards,@em -1 means at the end. Must be @em 0 <= @formatParam{Start} < GetLength() or @em -1 for the end.
	/// @return												@trueIfOtherwiseFalse{the substring was found}
	//----------------------------------------------------------------------------------------
	Bool FindLast(const String& str, Int32* pos, Int32 start = -1) const;

	//----------------------------------------------------------------------------------------
	/// Searches the string for the first match of character @formatParam{ch}.
	/// @param[in] ch									The character to find.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Starting position in the string for the search. Must be @em 0 <= @formatParam{Start} < GetLength().
	/// @return												@trueIfOtherwiseFalse{the character was found}
	//----------------------------------------------------------------------------------------
	Bool FindFirst(UInt16 ch, Int32* pos, Int32 start = 0) const;

	//----------------------------------------------------------------------------------------
	/// Search the string for the last match of character @formatParam{ch}.
	/// @param[in] ch									The character to find.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Position to start from when searching backwards,@em -1 means at the end. Must be @em 0 <= @formatParam{Start} < GetLength() or @em -1 for the end.
	/// @return												@trueIfOtherwiseFalse{the character was found}
	//----------------------------------------------------------------------------------------
	Bool FindLast(UInt16 ch, Int32* pos, Int32 start = -1) const;

	//----------------------------------------------------------------------------------------
	/// Searches the string for the first match of the upper-case substring.
	/// @param[in] findUpper					Upper-case substring to search for.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Starting position in the string for the search. Must be @em 0 <= @formatParam{Start} < GetLength().
	/// @return												@trueIfOtherwiseFalse{the uppar-case substring was found}
	//----------------------------------------------------------------------------------------
	Bool FindFirstUpper(const String& findUpper, Int32* pos, Int32 start) const;

	//----------------------------------------------------------------------------------------
	/// Searches the string for the last match of the upper-case substring.
	/// @param[in] findUpper					Upper-case substring to search for.
	/// @param[in] pos								Pointer to an ::Int32 to receive the found position in the string. @callerOwnsPointed{integer}.
	/// @param[in] start							Position to start from when searching backwards,@em -1 means at the end. Must be @em 0 <= @formatParam{Start} < GetLength() or @em -1 for the end.
	/// @return												@trueIfOtherwiseFalse{the uppar-case substring was found}
	//----------------------------------------------------------------------------------------
	Bool FindLastUpper(const String& findUpper, Int32* pos, Int32 start) const;

	/// @}

	/// @name Delete/Insert/SubStr
	/// @{

	//----------------------------------------------------------------------------------------
	/// Removes a section from the string.
	/// @param[in] pos								Starting position to remove. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @param[in] count							Number of characters to remove. Must be 0 <= @formatParam{Count} < GetLength()-@formatParam{Pos}.
	//----------------------------------------------------------------------------------------
	void Delete(Int32 pos, Int32 count);

	//----------------------------------------------------------------------------------------
	/// Inserts another string into the string.
	/// @param[in] pos								Position to insert the string. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @param[in] str								String to insert.
	/// @param[in] start							Start position within the inserted string to only insert parts of the string.
	/// @param[in] end								End position within the inserted string to only insert parts of the string.
	//----------------------------------------------------------------------------------------
	void Insert(Int32 pos, const String& str, Int32 start = -1, Int32 end = -1);

	//----------------------------------------------------------------------------------------
	/// Inserts a character into the string.
	/// @param[in] pos								Position to insert the character. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @param[in] ch									Character to insert.
	//----------------------------------------------------------------------------------------
	void Insert(Int32 pos, UInt16 ch);

	//----------------------------------------------------------------------------------------
	/// Extracts a substring from the String.
	/// @param[in] start							Position to extract the substring from. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @param[in] count							Number of characters of the substring. Must be 0 <= @formatParam{Count} < GetLength()-@formatParam{Start}.
	/// @return												The substring.
	//----------------------------------------------------------------------------------------
	const String SubStr(Int32 start, Int32 count) const;

	//----------------------------------------------------------------------------------------
	/// Extracts a substring from the left of the string.
	/// @markDeprecated This is equivalent to maxon::String::GetPart(-count, maxon::StringEnd()).
	/// @param[in] count							The number of characters of the substring: @em 0 <= @formatParam{count} < GetLength()
	/// @return												The substring.
	//----------------------------------------------------------------------------------------
	String Left(Int count) const;

	//----------------------------------------------------------------------------------------
	/// Extracts a substring from the right of the string.
	/// @markDeprecated This is equivalent to maxon::String::GetPart(-count, maxon::StringEnd()).
	/// @param[in] count							The number of characters of the substring: @em 0 <= @formatParam{count} < GetLength()
	/// @return												The substring.
	//----------------------------------------------------------------------------------------
	String Right(Int count) const;
	/// @}

	/// @name Parse to Number
	/// @{

	//Float ParseToFloat(Int32 *error = nullptr, Int32 Einheit = 0, Int32 WinkelArt = 0, Int32 Basis = 10) const;	// not implemented yet
	Int32 ParseToInt32(Int32* error = nullptr) const;

	/// @}

	/// @name Conversion from Number
	/// @{

	//----------------------------------------------------------------------------------------
	/// Converts a ::Float into a String.
	/// @param[in] v									The Float value to convert.
	/// @param[in] vvk								The number of digits in front of the comma.
	/// @param[in] nnk								The number of digits after the comma.
	/// @param[in] e									Set to @formatConstant{true} to use exponential writing.
	/// @param[in] xchar							The leading chars will be filled with this character. e.g. the value @em 100 with @formatParam{vvk}==@em 5 will looks like this: <i>00100</i>.
	/// @return												The ::Float as a String.
	//----------------------------------------------------------------------------------------
	static String FloatToString(Float v, Int32 vvk = -1, Int32 nnk = -1, Bool e = false, UInt16 xchar = '0');

	//----------------------------------------------------------------------------------------
	/// Converts an ::Int32 into a String.
	/// @param[in] v									The 32-bit integer value to convert.
	/// @return												The ::Int32 as a String.
	//----------------------------------------------------------------------------------------
	static String IntToString(Int32 v);

	//----------------------------------------------------------------------------------------
	/// Converts a ::UInt32 into a String.
	/// @param[in] v									The 32-bit unsigned integer value to convert.
	/// @return												The ::UInt32 as a String.
	//----------------------------------------------------------------------------------------
	static String UIntToString(UInt32 v);

	//----------------------------------------------------------------------------------------
	/// Converts an ::Int64 into a String.
	/// @param[in] v									The 64-bit integer value to convert.
	/// @return												The ::Int64 as a String.
	//----------------------------------------------------------------------------------------
	static String IntToString(Int64 v);

	//----------------------------------------------------------------------------------------
	/// Converts an ::UInt64 to a String.
	/// @param[in] v									The 64-bit unsigned integer value to convert.
	/// @return												The ::UInt64 as a String.
	//----------------------------------------------------------------------------------------
	static String UIntToString(UInt64 v);

	//----------------------------------------------------------------------------------------
	/// Converts a Vector32 to a String.
	/// @param[in] v									The 32-bit vector value to convert.
	/// @param[in] nnk								The number of digits after the comma.
	/// @return												The Vector32 as a String.
	//----------------------------------------------------------------------------------------
	static String VectorToString(const Vector32& v, Int32 nnk = -1);

	//----------------------------------------------------------------------------------------
	/// Converts a Vector64 to a String.
	/// @param[in] v									The 64-bit vector value to convert.
	/// @param[in] nnk								The number of digits after the comma.
	/// @return												The Vector64 as a String.
	//----------------------------------------------------------------------------------------
	static String VectorToString(const Vector64& v, Int32 nnk = -1);

	/// @}

	/// @name Convert to Upper/Lower Case
	/// @{

	//----------------------------------------------------------------------------------------
	/// Converts the characters of the string to uppercase. This will only work with ANSI characters less than character code @em 128, all other characters will remain unchanged.
	/// @note	Returns a copy of the string.
	/// @return												The converted uppercase string.
	//----------------------------------------------------------------------------------------
	const String ToUpper() const;

	//----------------------------------------------------------------------------------------
	/// Converts the characters of the string to lowercase. This will only work with ANSI characters less than character code @em 128, all other characters will remain unchanged.
	/// @note	Returns a copy of the string.
	/// @return												The converted lowercase string.
	//----------------------------------------------------------------------------------------
	const String ToLower() const;

	/// @}

	/// @name C and Unicode String Conversion
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the expected length of the string after encoding.
	/// @param[in] type								Type of string encoding.
	/// @return												The length of the string.
	//----------------------------------------------------------------------------------------
	Int32 GetCStringLen(STRINGENCODING type = STRINGENCODING_XBIT) const;

	//----------------------------------------------------------------------------------------
	/// Gets the string after encoding. This routine can be used to convert @C4D Unicode strings into standard @em C strings.
	/// @param[out] cStr							Buffer for the encoded string. The size of the buffer must be at least @formatParam{Max}. @callerOwnsPointed{buffer}.
	/// @param[in] max								Maximum number of characters for the buffer @formatParam{Cstr}. Should be GetCStringLen() + @em 1.
	/// @param[in] type								Type of string encoding.
	/// @return												The length of the string.
	//----------------------------------------------------------------------------------------
	Int32 GetCString(Char* cStr, Int32 max, STRINGENCODING type = STRINGENCODING_XBIT) const;

	//----------------------------------------------------------------------------------------
	/// Gets the string from an encoded string. This routine converts a standard @em C string into a @C4D Unicode string.
	/// @param[out] cStr							Buffer containing the encoded string. @callerOwnsPointed{buffer}.
	/// @param[in] count							The number of characters in the encoded string, or @em -1 to use the @em '\0' termination.
	/// @param[in] type								Type of string encoding.
	/// @return												The length of the string.
	//----------------------------------------------------------------------------------------
	Int32 SetCString(const Char* cStr, Int32 count = -1, STRINGENCODING type = STRINGENCODING_XBIT);

	//----------------------------------------------------------------------------------------
	/// Gets a standard @em C string copy of the string.
	/// @warning	The return @em C string has to be freed by the caller after use.
	/// @param[in] type								Type of string encoding.
	/// @return												The C string copy of the string. <b>Important: The @em C string buffer has to be freed after usage.</b>
	//----------------------------------------------------------------------------------------
	Char* GetCStringCopy(STRINGENCODING type = STRINGENCODING_XBIT) const;

	//----------------------------------------------------------------------------------------
	/// Gets the Unicode data of the string directly, without having to access the @em [] operator (which is slower).
	/// @note	This version adds a '\0' character at the end of the string.
	/// @see GetUcBlock()
	/// @param[out] ubc								Should point to a buffer to be filled with the Unicode data of the string. @callerOwnsPointed{buffer}.
	/// @param[in] max								Maximum number of elements to fill in @formatParam{Ubc}, including the '\0' terminator.
	//----------------------------------------------------------------------------------------
	void GetUcBlockNull(UInt16* ubc, Int32 max) const;

	//----------------------------------------------------------------------------------------
	/// Gets the copy of the Unicode data, without having to access the @em [] operator (which is slower).
	/// @note	This version adds a '\0' character at the end of the string.
	/// @see GetUcBlock()
	/// @return												The copy of the Unicode data of the string. <b>Important: The memory buffer has to be freed after usage.</b>
	//----------------------------------------------------------------------------------------
	UInt16* GetUcBlockNullCopy() const;

	//----------------------------------------------------------------------------------------
	/// Gets the Unicode data of the string directly, without having to access the @em [] operator (which is slower).
	/// @note	This version does not add a '\0' character at the end of the string.
	/// @see GetUcBlockNull()
	/// @param[in] ubc								Should point to a buffer to be filled with the Unicode data of the string. @callerOwnsPointed{buffer}.
	/// @param[in] max								Maximum number of elements to fill in @formatParam{Ubc}.
	//----------------------------------------------------------------------------------------
	void GetUcBlock(UInt16* ubc, Int32 max) const;

	//----------------------------------------------------------------------------------------
	/// Sets the Unicode data into the string directly.
	/// @param[in] ubc								A buffer with the Unicode data to set. @callerOwnsPointed{buffer}.
	/// @param[in] count							The number of elements in @formatParam{Ubc}.
	//----------------------------------------------------------------------------------------
	void SetUcBlock(const UInt16* ubc, Int32 count);

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the string data.
	/// @param[out] outPtr						Assigned a pointer to the string data.
	/// @return												The length of the string.
	//----------------------------------------------------------------------------------------
	inline Int32 GetDataPtr(const UInt16** outPtr) const
	{
		*outPtr = Txt;
		return Len;
	}

	/// @}

	/// Get/Set Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a constant Unicode character from the string at position @formatParam{pos}.
	/// @param[in] pos								The position within the string to get the character at. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @return												The character.
	//----------------------------------------------------------------------------------------
	const PChar operator[] (Int32 pos) const;

	//----------------------------------------------------------------------------------------
	/// Gets a non-constant Unicode character from the string at a position. This makes it possible to assign to a character position:
	/// @code
	/// String s("Hello World!");
	/// s[1] = 'a';
	/// @endcode
	/// @param[in] pos								The position within the string to get the character at. Must be @em 0 <= @formatParam{Pos} < GetLength().
	/// @return												The character.
	//----------------------------------------------------------------------------------------
	PChar operator[] (Int32 pos);

	/// @}
};

//const String String::FloatToString(Float v, Int32 vvk=-1, Int32 nnk=-1, Bool e=false, UInt16 xchar='0');
//const String String::IntToString(Int32 l);

#pragma pack (pop)
}

#endif	// C4D_STRING_H__
