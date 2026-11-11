/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef GV_NODEMASTER_H__
#define GV_NODEMASTER_H__

#include "c4d_basedocument.h"
#include "c4d_rootobject.h"

namespace cineware
{
#pragma pack (push, 8)

#define GV_NODE_MASTER_NR_OF_CONNECTIONS_ID 100
#define GV_NODE_MASTER_FIRST_CONNECTION_ID	200

class GvRealNode;
class GvRealPort;

typedef GvRealNode* (*GvGetNode)(void *root, Int32 id);

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealNodeMasterData : public NodeData
{
	INSTANCEOF(GvRealNodeMasterData, NodeData)

	friend class GvRealNodeMaster;

	public:
		GvRealNodeMasterData();
		virtual ~GvRealNodeMasterData();

	public:
		RootPluginNode*				head;

		virtual Bool					Init();
		virtual Bool					CopyToX(PrivateChunk *dest, COPYFLAGS flags, AliasTrans *trn);
		virtual Bool					Read(HyperFile *hf, Int32 id, Int32 level);
		virtual Bool					Write(HyperFile *hf);
		virtual Int32					GetChunkLevel();

		static NodeData*			Alloc();
		GvRealNode*						GetRoot() const;

	private:
		void									CountConnections(GvRealNode *node, Int32 &count);
		void									WriteConnections(GvRealNode *node, BaseContainer *bc, Int32 &id);
		Bool									ReadConnections(GvRealNode *node, BaseContainer *bc, BaseDocument *doc);

		void									SetNodeIDs(GvRealNode *node, Int32 &id);
		Bool									CopyAllConnections(GvRealNode* sroot, GvRealNode* droot, GvRealNode* snode, GvRealNode* dnode);
		Int32									GetPort(GvRealNode *node, GvRealPort *port, Int32 &io);
		Bool									CopyNodeConnections(GvRealNode *snode, GvRealNode *dnode, GvGetNode get_node, void *root, Int32 offset = 0);
		static GvRealNode*		GetNode(void *root, Int32 id);
};

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealNodeMaster : public BaseList2D
{
	friend class GvExpressionData;

	protected:
		GvRealNodeMaster();
		~GvRealNodeMaster();

	public:
		GvRealNode*						AllocNode(Int32 id, Bool clone = false);

		GvRealNode*						GetRoot();
		GvRealNodeMasterData* GetMasterData() { return (GvRealNodeMasterData*)GetNodeData(); }
};

#pragma pack (pop)
}

#endif // GV_NODEMASTER_H__
