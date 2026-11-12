/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DESCIDSTORE_H__
#define C4D_DESCIDSTORE_H__

#include "c4d_descid.h"
#include "c4d_misc/datastructures/basearray.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct DescIdEntry
{
	DescID id;
	GeData data;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class DescIdStore
{
public:
	Int32 GetCount() const;
	const DescIdEntry* GetIndex(Int32 i) const;

	void FlushAll();
	const DescIdEntry* FindId(const DescID& id, Bool tolerant) const;
	DescIdEntry* FindId(const DescID& id, Bool tolerant);
	const GeData& GetData(const DescID& id, Bool tolerant) const;
	Bool SetId(const DescID& id, const GeData& data);
	void RemoveId(const DescID& id);

	Bool CopyTo(DescIdStore* dest, AliasTrans* trans);

private:
	BaseArray<DescIdEntry> _store;
};

void DescIdStore_Free(DescIdStore*& store);
Bool DescIdStore_CopyTo(DescIdStore*& store, DescIdStore*& dest, AliasTrans* trans);
Bool DescIdStore_Read(DescIdStore*& store, HyperFile* hf, Bool readdata);
Bool DescIdStore_Write(DescIdStore*& store, HyperFile* hf, Bool writedata);

#pragma pack (pop)
}

#endif	// C4D_DESCIDSTORE_H__
