/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BROWSECONTAINER_H__
#define C4D_BROWSECONTAINER_H__

#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct GeDataList
{
	GeDataList() : Id(0), next(nullptr)
	{
	}

	Int32				Id;
	GeData			Data;
	GeDataList*	next;
};

//----------------------------------------------------------------------------------------
/// Helper class to browse a BaseContainer.
//----------------------------------------------------------------------------------------
class BrowseContainer
{
	GeDataList*					 handle;
	const BaseContainer* bc;

public:
	//----------------------------------------------------------------------------------------
	/// Constructs the browser for the provided container.
	/// @param[in] t_bc								The container to browse. @callerOwnsPointed{container}
	//----------------------------------------------------------------------------------------
	BrowseContainer(const BaseContainer* t_bc);

	//----------------------------------------------------------------------------------------
	/// Gets the next item in the container.
	/// @param[in] id									Assigned the ID of the next item.
	/// @param[in] data								Assigned a pointer to the item's data. @theOwnsPointed{container, data}
	/// @return												@trueIfOtherwiseFalse{a new item was retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetNext(Int32* id, GeData** data);

	//----------------------------------------------------------------------------------------
	/// Resets the browser to its construction state.
	//----------------------------------------------------------------------------------------
	void Init();
};

#pragma pack (pop)
}

#endif	// C4D_BROWSECONTAINER_H__
