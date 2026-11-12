/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef GV_NODE_H__
#define GV_NODE_H__

#include "c4d_pluginnode.h"
#include "gv_port.h"
#include "gv_nodemaster.h"
#include "c4d_operatordata.h"

namespace cineware
{
#pragma pack (push, 8)

typedef Int32 GvDataID;					///< Data type ID.

/// @addtogroup GvNodeFlags
/// @ingroup group_enumeration
/// @{
enum GvNodeFlags
{
	GV_NODE_ADDED_TO_TABLE			= (1 << 0),
	GV_NODE_FAILURE							=	(1 << 1),
	GV_NODE_ADDED_TO_DRAW_LIST	= (1 << 2),
	GV_NODE_INTERNAL_SELECTED		= (1 << 3),
	GV_NODE_OPEN								= (1 << 4)
};
/// @}

/// @addtogroup GvOperatorType
/// @ingroup group_enumeration
/// @{
enum GvOperatorType
{
	GV_OPERATOR_TYPE_INVALID = 0,						///< Invalid.
	GV_OPERATOR_TYPE_GROUP,									///< Group.
	GV_OPERATOR_TYPE_NODE,									///< Node.
	GV_OPERATOR_TYPE_DUMMY,									///< Dummy.
	GV_OPERATOR_TYPE_GENERAL,								///< General.
	GV_OPERATOR_TYPE_CALCULATE,							///< Calculate.
	GV_OPERATOR_TYPE_ITERATOR,							///< Iterator.
	GV_OPERATOR_TYPE_BOOL,									///< Bool.
	GV_OPERATOR_TYPE_LOGICAL,								///< Logical.
	GV_OPERATOR_TYPE_ADAPTER,								///< Adapter.
	GV_OPERATOR_TYPE_THINKINGPARTICLES			///< Thinking particles.
};
/// @}

#define GV_NODE_STATE_ID						100
#define GV_NODE_COLOR_ID						101
#define GV_NODE_POS_X								102
#define GV_NODE_POS_Y								103

#define GV_NODE_IN_PORT_COUNT_ID		200
#define GV_NODE_OUT_PORT_COUNT_ID		201

#define GV_NODE_IN_PORT_FIRST_ID		1000000
#define GV_NODE_OUT_PORT_FIRST_ID		2000000

#define GV_NODE_DATA_FIRST_ID				100

class GvRealNode;
struct GvShapeData;
class GvPortArray : public GeTempDynArray <GvRealPort> { };

class GvPortDescriptionCache;

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
struct GvColor
{
	Vector v;
	GvColor() : v(0.0) { }
	void Set(Int32 r, Int32 g, Int32 b) { v.x = (Float)r / 255.0; v.y = (Float)g / 255.0; v.z = (Float)b / 255.0; }
	void Set(Vector p) { v = p; }
	const Vector& Get() { return v; }
	void Set255(Vector p) { v = Vector(p.x / 255.0, p.y / 255.0, p.z / 255.0); }
	const Vector Get255() { return Vector(v.x * 255.0, v.y * 255.0, v.z * 255.0); }
};

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealNodeData : public NodeData
{
	INSTANCEOF(GvRealNodeData, NodeData)

	friend class GvRealNode;
	friend class GvRealNodeMaster;

	public:
		GvRealNodeData();
		virtual ~GvRealNodeData();

	protected:
		GvPortArray			in_ports;
		GvPortArray			out_ports;
		Int32						_index;
		Int32						owner;
		UInt32					_flags;
		UInt32					state;
		UInt32					busymask;
		UInt32					recursion;

		Vector					color;
		Bool						changedColor;	// true if user changed default head color

	public:
		Int32						init_pos_x;
		Int32						init_pos_y;
		Bool						old_coords;

	public:
		virtual Bool Init();
		virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
		virtual Bool Write(HyperFile* hf);
		virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);
		virtual Bool IsInstanceOf(const GeListNode *node, Int32 type) const;
		virtual Int32 GetChunkLevel() { return 5; }
		static	NodeData *Alloc() { return NewObj(GvRealNodeData); }

		virtual Int32					GetType() const;

 		Bool									CopyData(GvRealNodeData *dest, AliasTrans *trn = nullptr);
 		Int32									GetGvNodeIndex() const { return _index; }

 		GvPortArray&					GetInPorts() { return in_ports; }
 		GvPortArray&					GetOutPorts() { return out_ports; }

 		Int32									GetInPortCount() const { return in_ports.GetCount(); }
 		Int32									GetOutPortCount() const { return out_ports.GetCount(); }
		GvRealPort*						GetInPort(Int32 n) const { return in_ports.GetIndex(n); }
		GvRealPort*						GetOutPort(Int32 n) const { return out_ports.GetIndex(n); }
 		GvRealPort*						GetInPortSubID(Int32 id) const;
 		GvRealPort*						GetOutPortSubID(Int32 id) const;
 		GvRealPort*						GetPortSubID(Int32 id) const;
 		GvRealPort*						AddPortIntern(GvPortIO io, Int32 id, GvPortFlags flags, Int32 sub_id, Int32 data_id);
 		Int32									GetUniquePortID();

	public:

 		void SetGvNodeIndex(Int32 t_index)
 		{
 			_index = t_index;
 		}
 };

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealGroupData : public GvRealNodeData
{
	public:
		GvRealGroupData() {}
		virtual ~GvRealGroupData() {}

	public:
		virtual Int32				GetType() const
		{
			return ID_GV_GROUPDATA;
		}

		static NodeData *Alloc() { return NewObj(GvRealGroupData); }
};

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealNode : public BaseList2D
{
	friend class GvRealNodeMaster;

	public:
 		Bool							InitOperatorContainer();
 		BaseContainer*		GetOperatorContainerInstance();

 		GvOperatorData* GetGvOperatorData() const
 		{
			return (GvOperatorData*)((PluginNode*)this)->GetNodeDataI(1);
 		}

 		GvRealNodeData* GetGvNodeData()
		{
			return (GvRealNodeData*)((PluginNode*)this)->GetNodeDataI(0);
 		}

 		Int32 GetGvOperatorID() const
 		{
 			return GetNodeID(1);
 		}

 		GvOperatorType GetGvOperatorType() const
 		{
 			return (GetNodeID(1) == ID_GV_OPERATOR_GROUP) ? GV_OPERATOR_TYPE_GROUP : GV_OPERATOR_TYPE_NODE;
 		}

		Bool InitShapeContainer()
		{
			BaseContainer *bc = GetDataInstance(); if (!bc) return false;
			BaseContainer *sc = bc->GetContainerInstance(ID_SHAPECONTAINER);
			if (!sc)
			{
				bc->SetContainer(ID_SHAPECONTAINER, BaseContainer());
				sc = bc->GetContainerInstance(ID_SHAPECONTAINER);
				if (!sc) return false;
			}
			return true;
		}
};

#pragma pack (pop)
}

#endif // GV_NODE_H__

