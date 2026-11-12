/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_PLUGINNODE_H__
#define C4D_PLUGINNODE_H__

#include "c4d_baselist2d.h"
#include "c4d_nodedata.h"

namespace cineware
{
#pragma pack (push, 8)

struct PluginData
{
	Int32						id;
	NodeData				*mem;

	// Cineware SDK specific
	void*	unknownData;
	Int		unknownDataSize;
	Int32	unknownDataLevel;
};

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class PluginNode : public BaseList2D
{
	OVERRIDENINSTANCEOF(PluginNode, BaseList2D)

public:
	PluginData* plug;
	Int32				plug_cnt;

	PluginNode();
	PluginNode(Int32 pId);
	virtual ~PluginNode();

	virtual Int32 GetType() const;
	virtual Int32 GetDiskType() const;

	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trans);
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	NodeData* AllocData(Int32 id, Bool& known);

	virtual Bool Write(HyperFile* hf);
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	NodeData* GetNodeDataI(Int32 index)
	{
		if (!plug || index >= plug_cnt) return nullptr;
		return plug[index].mem;
	}

	Bool SetPluginID(Int32 *arr, Int32 cnt)
	{
		if (plug)
			FreePluginData();

		ValidatePluginData(arr, cnt);
		return plug != nullptr;
	}

	Bool SetPluginID(Int32 id)
	{
		if (plug)
			FreePluginData();

		ValidatePluginData(&id, 1);
		return plug != nullptr;
	}

	void ValidateNth(Int32 index, Int32 t_id, Bool& known)
	{
		plug[index].id = t_id;

		if (!t_id) return;

		plug[index].mem = AllocData(plug[index].id, known);
		if (!plug[index].mem)
		{
			return;
		}

		// in Cineware SDK make sure to set node before Init()
		plug[index].mem->SetNode(this);

		if (!plug[index].mem->Init())
		{
			plug[index].mem->Free();
			DeleteObj(plug[index].mem);
			plug[index].mem = nullptr;
		}
	}

	void ValidatePluginData(Int32* t_arr, Int32 t_cnt)
	{
		if (plug)
			return;

		if (t_cnt == 0)
			return;

		plug = NewMemClear(PluginData, t_cnt);
		if (!plug)
			return;

		plug_cnt = t_cnt;

		Int32 i;
		for (i = 0; i < t_cnt; i++)
		{
			plug[i].id = t_arr[i];
		}

		AllocPluginData(t_arr);
	}

	Bool AllocPluginData(Int32* ids)
	{
		Bool known = false;

		for (Int32 i = 0; i < plug_cnt; i++)
		{
			plug[i].mem = AllocData(ids[i], known);
			if (!plug[i].mem || !known)
			{
				FreePluginData();
				return false;
			}

			// in Cineware SDK make sure to set node before Init()
			plug[i].mem->SetNode(this);

			if (!plug[i].mem->Init())
			{
				FreePluginData();
				return false;
			}
		}

		return true;
	}

	void FreePluginData()
	{
		if (!plug) return;

		Int32 index;

		for (index = plug_cnt - 1; index >= 0; index--) // destruct in inverse order
		{
			if (plug[index].mem)
			{
				plug[index].mem->Free();
			}

			if (plug[index].unknownData)
			{
				DeleteMem(plug[index].unknownData);
				plug[index].unknownDataSize = 0;
				plug[index].unknownDataLevel = 0;
			}
		}

		for (index = plug_cnt - 1; index >= 0; index--) // destruct in inverse order
		{
			if (plug[index].mem)
			{
				DeleteObj(plug[index].mem);
				plug[index].mem = nullptr;
			}
		}

		DeleteMem(plug);
		plug_cnt = 0;
	}
};

#pragma pack (pop)
}

#endif	// C4D_PLUGINNODE_H__
