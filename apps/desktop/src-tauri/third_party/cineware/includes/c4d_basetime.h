/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASETIME_H__
#define C4D_BASETIME_H__

#include "c4d_system.h"

namespace cineware
{
#pragma pack (push, 8)


class String;

//----------------------------------------------------------------------------------------
/// @C4D uses a sophisticated system for specifying time values.\n
/// If only frame numbers were used, changing the frame rate would cause keys either to overlap or disappear.\n
/// If only float values (seconds) were used instead, there would be problems because of the limited data precision. For example when using @em 30 fps the frame @em 29 = @em 29/30 could easily be misinterpreted as frame @em 28.
///
/// @C4D has a time class that internally stores the time values as exact fractions independent of the frame rate. For example frame @em 29 is stored as fraction with nominator @em 29 and denominator @em 30.\n
/// The class always tries to keep the nominator and denominator as small as possible. Hence @em 15/@em 30 is stored as @em 1/2. If using @em 30 fps GetFrame() would return @em 15, but if using @em 24 fps it would return frame @em 12.
//----------------------------------------------------------------------------------------
class BaseTime
{
protected:
	Float numerator;
	Float denominator;
	void Reduce();

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseTime();

	//----------------------------------------------------------------------------------------
	/// Initializes the internal time value from a float value in seconds.
	/// Constructor will multiply the seconds by 1000.0 and store it as a fraction with @em 1000.0 as denominator; after this the fraction will be reduced to its lowest form.
	/// @param[in] r									The time in seconds.
	//----------------------------------------------------------------------------------------
	explicit BaseTime(Float r);

	//----------------------------------------------------------------------------------------
	/// Initializes the internal time to the given fraction of @formatParam{z}/@formatParam{n}.
	/// @param[in] z									The nominator.
	/// @param[in] n									The denominator.
	//----------------------------------------------------------------------------------------
	explicit BaseTime(Float z, Float n);

	/// @}

	/// @name Numerator/Denominator
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the time in seconds.
	/// @return												The time in seconds.
	//----------------------------------------------------------------------------------------
	Float Get() const;

	//----------------------------------------------------------------------------------------
	/// Gets the numerator part of the internally stored time.
	/// @return												The numerator.
	//----------------------------------------------------------------------------------------
	Float GetNumerator() const;

	//----------------------------------------------------------------------------------------
	/// Gets the denominator part of the internally stored time.
	/// @return												The denominator.
	//----------------------------------------------------------------------------------------
	Float GetDenominator() const;

	//----------------------------------------------------------------------------------------
	/// Sets the numerator part of the internally stored time.
	/// @param[in] r									The numerator.
	//----------------------------------------------------------------------------------------
	void SetNumerator(Float r);

	//----------------------------------------------------------------------------------------
	/// Sets the denominator part of the internally stored time.
	/// @param[in] r									The denominator.
	//----------------------------------------------------------------------------------------
	void SetDenominator(Float r);

	/// @}

	/// @name Frames
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the number of frames equivalent to the time for the given number of Frames per Second.
	/// @param[in] fps								The Frames per Second to use to calculate the frame number for the time.
	/// @return												The frame number for the time.
	//----------------------------------------------------------------------------------------
	Int32 GetFrame(Float fps) const;

	//----------------------------------------------------------------------------------------
	/// Quantizes the internally stored value so that it is a multiple of the given number of Frames per Second.
	/// @param[in] fps								The number of Frames per Second to make the time a multiple of.
	//----------------------------------------------------------------------------------------
	void Quantize(Float fps);

	/// @}

	/// @name Math Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Multiplies @formatParam{t1} and @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the multiplication.
	//----------------------------------------------------------------------------------------
	friend const BaseTime operator * (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Divides @formatParam{t1} and @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the division.
	//----------------------------------------------------------------------------------------
	friend const BaseTime operator / (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Adds @formatParam{t1} and @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the addition.
	//----------------------------------------------------------------------------------------
	friend const BaseTime operator + (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Subtracts @formatParam{t1} and @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the subtraction.
	//----------------------------------------------------------------------------------------
	friend const BaseTime operator - (const BaseTime& t1, const BaseTime& t2);

	/// @}

	/// @name Comparison Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Equality operator. Checks if @formatParam{t1} and @formatParam{t2} are equal.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator == (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Checks if @formatParam{t1} and @formatParam{t2} are not equal.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator != (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Less than operator. Checks if @formatParam{t1} is less than @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator < (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Greater than operator. Checks if @formatParam{t1} is greater than @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator > (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Less than or equal operator. Checks if @formatParam{t1} is less than or equal to @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator <= (const BaseTime& t1, const BaseTime& t2);

	//----------------------------------------------------------------------------------------
	/// Greater than or equal operator. Checks if @formatParam{t1} is greater than or equal to @formatParam{t2}.
	/// @param[in] t1									Left-operand time.
	/// @param[in] t2									Right-operand time.
	/// @return												Result of the comparison.
	//----------------------------------------------------------------------------------------
	friend Bool operator >= (const BaseTime& t1, const BaseTime& t2);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks which is the largest between the time and @formatParam{t2}.
	/// @param[in] t2									The time to compare with.
	/// @return												@em -1 if the time is < @formatParam{t2}, @em 0 if they are equal and @em 1 if the time is > @formatParam{t2}.
	//----------------------------------------------------------------------------------------
	Int32 TimeDif(const BaseTime& t2) const;

	/// @}
};


inline const BaseTime operator * (const BaseTime& t1, const BaseTime& t2)
{
	return BaseTime(t1.numerator * t2.numerator, t1.denominator * t2.denominator);
}

inline const BaseTime operator / (const BaseTime& t1, const BaseTime& t2)
{
	return BaseTime(t1.numerator * t2.denominator, t1.denominator * t2.numerator);
}

inline const BaseTime operator + (const BaseTime& t1, const BaseTime& t2)
{
	return BaseTime(t1.numerator * t2.denominator + t2.numerator * t1.denominator, t1.denominator * t2.denominator);
}

inline const BaseTime operator - (const BaseTime& t1, const BaseTime& t2)
{
	return BaseTime(t1.numerator * t2.denominator - t2.numerator * t1.denominator, t1.denominator * t2.denominator);
}

inline Bool operator == (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return true;	// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) == Floor(t2.numerator * t1.denominator);
}

inline Bool operator != (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return false;		// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) != Floor(t2.numerator * t1.denominator);
}

inline Bool operator < (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return false;		// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) < Floor(t2.numerator * t1.denominator);
}

inline Bool operator > (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return false;		// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) > Floor(t2.numerator * t1.denominator);
}

inline Bool operator <= (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return true;	// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) <= Floor(t2.numerator * t1.denominator);
}

inline Bool operator >= (const BaseTime& t1, const BaseTime& t2)
{
	if (t1.numerator == t2.numerator && t1.denominator == t2.denominator)
		return true;	// Massive problems otherwise (compiler miscalculating floor).
	return Floor(t1.numerator * t2.denominator) >= Floor(t2.numerator * t1.denominator);
}


//----------------------------------------------------------------------------------------
/// Represents a date time.
//----------------------------------------------------------------------------------------
struct DateTime
{
	//----------------------------------------------------------------------------------------
	/// Default constructor. (Set the time to @em 2000-01-01 @em 00:00:00.)
	//----------------------------------------------------------------------------------------
	DateTime()
	{
		lYear	 = 2000;
		lMonth = lDay = 1;
		lHour	 = lMinute = lSecond = 0;
	}

	Int32 lYear,		///< Date year.
				lMonth,		///< Date month.
				lDay;			///< Date day in month.

	Int32 lHour,		///< Time hour.
				lMinute,	///< Time minute.
				lSecond;	///< Time second.

	/// @name Operators
	/// @{

	//----------------------------------------------------------------------------------------
	/// Equality operator. Check if the date time is equal to another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator == (const DateTime& b) const;

	//----------------------------------------------------------------------------------------
	/// Inequality operator. Check if the date time is not equal to another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator != (const DateTime& b) const;

	//----------------------------------------------------------------------------------------
	/// Less than operator. Check if the date time is less than another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator < (const DateTime& b) const;

	//----------------------------------------------------------------------------------------
	/// Greater than operator. Check if the date time is greater than another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator > (const DateTime& b) const;

	//----------------------------------------------------------------------------------------
	/// Less than or equal operator. Check if the date time is less than or equal to another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator <= (const DateTime& b) const;

	//----------------------------------------------------------------------------------------
	/// Greater than or equal operator. Check if the date time is greater than equal to another.
	///
	/// @param[in] b									The right-operand DateTime.
	/// @return												The result of the comparison.
	//----------------------------------------------------------------------------------------
	Bool operator >= (const DateTime& b) const;

	/// @}
};


/// @addtogroup group_c4d_basetime_time Time
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Compare date times @formatParam{a} and @formatParam{b}.
///
/// @param[in] a									A date time.
/// @param[in] b									B date time.
/// @return												Less than @em 0 if @formatParam{a} < @formatParam{b}, equal to @em 0 if @formatParam{a} == @formatParam{b} and greater than @em 0 if @formatParam{a} > @formatParam{b}.
//----------------------------------------------------------------------------------------
Int32 CompareDateTime(const DateTime& a, const DateTime& b);


/// Week days.
enum DayOfWeek
{
	Monday		= 0,
	Tuesday		= 1,
	Wednesday	= 2,
	Thursday	= 3,
	Friday		= 4,
	Saturday	= 5,
	Sunday		= 6
} ENUM_END_LIST(DayOfWeek);


//----------------------------------------------------------------------------------------
/// Gets the Modified Julian Date (http://tycho.usno.navy.mil/mjd.html) from @formatParam{t}.
/// @param[in] t									A date time.
/// @return												The Modified Julian Date. To get the correct Julian day, take the integer of this value and subtract @em 0.5
//----------------------------------------------------------------------------------------
Float64 GetJulianDay(const DateTime& t);

//----------------------------------------------------------------------------------------
/// Gets a date time from the Modified Julian Date (http://tycho.usno.navy.mil/mjd.html) @formatParam{j}.
/// @param[in] j									A Modified Julian Date.
/// @return												The date time.
//----------------------------------------------------------------------------------------
DateTime FromJulianDay(Float64 j);

//----------------------------------------------------------------------------------------
/// Gets the current system date time.
/// @param[out] t									Filled with the current system time.
//----------------------------------------------------------------------------------------
void GetDateTimeNow(DateTime& t);

//----------------------------------------------------------------------------------------
/// Gets the current system date time in @em GMT.
/// @param[out] t									Filled with the current system @em GMT time.
//----------------------------------------------------------------------------------------
void GetDateTimeNowGMT(DateTime& t);

//----------------------------------------------------------------------------------------
/// Converts local time to @em GMT depending on the OS time zone settings.
/// @warning	The function will fail if the local date is before Jan 1, 1970 2.01 am or after Jan 18, 2038 7 pm.
/// @param[in] tLocal							The local time.
/// @param[out] tGMT							Filled with the calculated @em GMT time, or @formatParam{tLocal} if an error occurred.
/// @return												@trueIfOtherwiseFalse{successful}
//----------------------------------------------------------------------------------------
Bool LocalToGMTime(const DateTime& tLocal, DateTime& tGMT);

//----------------------------------------------------------------------------------------
/// Converts @em GMT time to local depending on the OS time zone settings.
/// @param[in] tGMT								The @em GMT time.
/// @param[out] tLocal						Filled with the calculated local time, or @formatParam{tGMT} if an error occurred.
/// @return												@trueIfOtherwiseFalse{successful}
//----------------------------------------------------------------------------------------
Bool GMTimeToLocal(const DateTime& tGMT, DateTime& tLocal);

//----------------------------------------------------------------------------------------
/// Gets the day of the week of the date @formatParam{lYear}-@formatParam{lMonth}-@formatParam{lDay}.
///
/// @param[in] lYear							The year.
/// @param[in] lMonth							The month number. (@em 1 means January.)
/// @param[in] lDay								The day in the month.
/// @return												The day of the week.
//----------------------------------------------------------------------------------------
DayOfWeek GetDayOfWeek(Int32 lYear, Int32 lMonth, Int32 lDay);

//----------------------------------------------------------------------------------------
/// Formats the given date time @formatParam{t} and output it as a string.
/// @param[in] pszFormat					The format options (same as @em asctime() format).
/// @param[in] t									The date time to format.
/// @return												The formatted string for the date time.
//----------------------------------------------------------------------------------------
String FormatTime(const char* pszFormat, const DateTime& t);

/// @}


#pragma pack (pop)
}

#endif	// C4D_BASETIME_H__
