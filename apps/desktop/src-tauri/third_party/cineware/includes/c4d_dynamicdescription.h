/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DYNAMICDESCRIPTION_H__
#define C4D_DYNAMICDESCRIPTION_H__

#include "c4d_basecontainer.h"

namespace cineware
{
#pragma pack (push, 8)

class HyperFile;

//----------------------------------------------------------------------------------------
/// The dynamic description class stores user data values.\n
/// The information about each description parameter is stored in a BaseContainer with the following values. See @c DESC_* container IDs.
//----------------------------------------------------------------------------------------
class DynamicDescription
{
private:
	BaseContainer dynamicdescriptions;

public:
	//----------------------------------------------------------------------------------------
	/// Allocates a new dynamic description.
	/// @param[in] datadescription		Settings for the new user data.
	/// @return												The ID of the new dynamic description.
	//----------------------------------------------------------------------------------------
	DescID Alloc(const BaseContainer& datadescription);

	//----------------------------------------------------------------------------------------
	/// Inserts a new user data with the specified ID.
	/// @param[in] descid							ID of the new dynamic description.
	/// @param[in] datadescription		Settings for the new dynamic description.
	/// @param[in] bl									private.
	/// @return												@trueIfOtherwiseFalse{the user data was inserted}
	//----------------------------------------------------------------------------------------
	Bool Set(const DescID& descid, const BaseContainer& datadescription, BaseList2D* bl);

	//----------------------------------------------------------------------------------------
	/// Finds a dynamic description by ID.
	/// @param[in] descid							ID of the dynamic description.
	/// @return												Pointer to the found settings, or @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	const BaseContainer* Find(const DescID& descid);

	//----------------------------------------------------------------------------------------
	/// Removes dynamic description by ID.
	/// @param[in] descid							ID of the dynamic description to remove.
	/// @return												@trueIfOtherwiseFalse{the user data was removed}
	//----------------------------------------------------------------------------------------
	Bool Remove(const DescID& descid);

	//----------------------------------------------------------------------------------------
	/// Copies the dynamic description to @formatParam{dest}.
	/// @param[in] dest								Destination dynamic description. @callerOwnsPointed{dynamic description}
	/// @return												@trueIfOtherwiseFalse{the description was copied}
	//----------------------------------------------------------------------------------------
	Bool CopyTo(DynamicDescription* dest);

	//----------------------------------------------------------------------------------------
	/// Fills the default settings container for a certain data @formatParam{type} with the specified @formatParam{name}.
	/// @param[in] res								Assigned the default settings.
	/// @param[in] type								The datatype ID. Either a custom ID or a data type ID.
	/// @param[in] name								Name of the dynamic description.
	/// @return												@trueIfOtherwiseFalse{the settings container was filled}
	//----------------------------------------------------------------------------------------
	Bool FillDefaultContainer(BaseContainer& res, Int32 type, const String& name);

	/// @name Browse
	/// @{

	//----------------------------------------------------------------------------------------
	/// Starts browsing the parameters linearly.
	/// @warning	BrowseFree() must called afterward!
	/// @return												Browse handle.
	//----------------------------------------------------------------------------------------
	void* BrowseInit();

	//----------------------------------------------------------------------------------------
	/// Gets the next parameter in a browse sequence initiated with BrowseInit().
	/// @param[in] handle							Browse handle returned by BrowseInit().
	/// @param[in] id									Assigned the settings for the retrieved parameter. (See above.) @callerOwnsPointed{container}
	/// @param[in] data								Assigned the ID of the retrieved parameter.
	/// @return												@formatConstant{true} if a new parameter was retrieved, otherwise @formatConstant{false} if the browse sequence is finished.
	//----------------------------------------------------------------------------------------
	Bool BrowseGetNext(void* handle, DescID* id, const BaseContainer** data);

	//----------------------------------------------------------------------------------------
	/// Free handles from BrowseInit().
	/// @warning	BrowseFree() must called always called after BrowseInit() and browsing description parameters with BrowseGetNext() is finished!
	/// @param[in,out] handle					The handle to free. Assigned @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	void BrowseFree(void*& handle);

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
	Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_DYNAMICDESCRIPTION_H__
