/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef GV_PORT_H__
#define GV_PORT_H__

////////////////////////////////

#include "private_ge_mtools.h"
#include "gv_nodemaster.h"
#include "c4d_string.h"
#include "c4d_gedata.h"

namespace cineware
{
#pragma pack (push, 8)

/// @addtogroup DataTypes
/// @ingroup group_enumeration
/// @{
typedef Int32 GvOperatorID;			///< Operator ID.
typedef Int32 GvOpClassID;			///< Operator class ID.
typedef Int32 GvOpGroupID;			///< Operator group ID.
typedef Int32 GvValGroupID;			///< Value group ID.
typedef Int32 GvDataID;					///< Data type ID.
typedef Int32 GvValueID;				///< Value type ID.
typedef Int32 GvUserDataID;			///< %User data ID.
/// @}

typedef Int32 GvValueIndex;
typedef Int32 GvDataIndex;
typedef Int32 GvValGroupIndex;

class GvRealNode;
class GvRealPort;
class GvRealNodeData;

#define GV_RESET_RUN_ID -1
#define INDEX_GV_VALUE_TYPE_UNDEFINED 0

/// @addtogroup GvPortIO
/// @ingroup group_enumeration
/// @{
enum GvPortIO
{
	GV_PORT_INVALID = 0,				///< Invalid.
	GV_PORT_INPUT,							///< Input.
	GV_PORT_OUTPUT,							///< Output.
	GV_PORT_INPUT_OR_GEDATA			///< Input or GeData.
};
/// @}

/// @addtogroup GvPortFlags
/// @ingroup group_enumeration
/// @{
enum GvPortFlags
{
	GV_PORT_FLAG_NONE = 0,						///< No flag.
	GV_PORT_FLAG_IS_VISIBLE = (1 << 0),			///< Visible port.
	GV_PORT_FLAG_IS_ITERATE = (1 << 1)			///< Iteration port.
};
/// @}

/// @addtogroup ID_GV_DATA_TYPE
/// @ingroup group_enumeration
/// @{
#define ID_GV_DATA_TYPE_NONE							0							///< No data. (DA_NIL)
#define ID_GV_DATA_TYPE_UNDEFINED					400006000			///< Undefined.
#define ID_GV_DATA_TYPE_BOOL							400006001			///< Boolean.
#define ID_GV_DATA_TYPE_INTEGER						15						///< Integer. (DA_LONG)
#define ID_GV_DATA_TYPE_REAL							19						///< Float. (DA_REAL)
#define ID_GV_DATA_TYPE_VECTOR						23						///< Vector (DA_VECTOR)
#define ID_GV_DATA_TYPE_NORMAL						400006005			///< Normal vector.
#define ID_GV_DATA_TYPE_MATRIX						25						///< Matrix. (DA_MATRIX)
#define ID_GV_DATA_TYPE_TIME							22						///< Time. (DA_TIME)
#define ID_GV_DATA_TYPE_STRING						130						///< String. (DA_STRING)
#define ID_GV_DATA_TYPE_GENERAL_OBJECT		400006009			///< General object.
#define ID_GV_DATA_TYPE_GENERAL_TAG				400006010			///< General tag.
#define ID_GV_DATA_TYPE_GENERAL_MATERIAL	400006011			///< General material.
#define ID_GV_DATA_TYPE_GENERAL_INSTANCE	400006012			///< General instance.
/// @}

//----------------------------------------------------------------------------------------
/// Connections destination data.
/// @since R20
//----------------------------------------------------------------------------------------
struct GvDestination
{
	//----------------------------------------------------------------------------------------
	/// Constructor. Sets pointers to @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	GvDestination() : node(nullptr), port(nullptr) { }

	GvRealNode*	node;			///< Destination node.
	GvRealPort*	port;			///< Destination port.
};

struct GvCalcInfo
{
	GvCalcInfo() : run_id(GV_RESET_RUN_ID), valid(false) { }
	Int32	run_id;
	Bool	valid;
};

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvPortValue
{
	public:
		GvValueIndex	type;
		Int32					nr_of_cpus;
		GvCalcInfo*		info;

	public:
		GvPortValue() : type(INDEX_GV_VALUE_TYPE_UNDEFINED), nr_of_cpus(0), info(nullptr) { }
		~GvPortValue() { Free(); }

		Bool Init(GvValueIndex type, Int32 nr_of_cpus);
		void Free();
};

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvConnection
{
	friend class GvRealPort;

	private:
		GvRealNode*	from_realnode;
		GvRealNodeData*	from_nodedata;
		GvRealNode*	to_realnode;
		GvRealNodeData*	to_nodedata;
		GvRealPort*	from_port;
		GvRealPort*	to_port;


	public:
		GvConnection() : from_realnode(nullptr), from_nodedata(nullptr), to_realnode(nullptr), to_nodedata(nullptr), from_port(nullptr), to_port(nullptr) { }

		void Set(GvRealNode* f_node, GvRealPort* f_port, GvRealNode* t_node, GvRealPort* t_port);

		void Clear()
		{
			from_realnode = nullptr;
			from_nodedata = nullptr;
			from_port     = nullptr;
			to_realnode   = nullptr;
			to_nodedata   = nullptr;
			to_port       = nullptr;
		}

	public:

		GvRealPort*	FromPort() const { return from_port; }
		GvRealPort*	ToPort() const { return to_port; }
		GvRealNode* FromRealNode() const { return from_realnode; }
		GvRealNode* ToRealNode() const { return to_realnode; }
		GvRealNodeData* FromNodeData() const { return from_nodedata; }
		GvRealNodeData* ToNodeData() const { return to_nodedata; }
};

class GvDestArray : public GeTempDynArray <GvDestination> { };

//----------------------------------------------------------------------------------------
/// @since R20
//----------------------------------------------------------------------------------------
class GvRealPort
{
	friend class GvRealNodeData;
	friend class GvRealNodeMasterData;
	friend class GvRealValue;

	private:
		GvPortValue						value;
		GvValueID							value_type_id;
		GvValueIndex					value_type_idx;
		GvDataID							data_type;
		Int32									nr_of_cpus;
		GvPortIO							port_type;
		GvPortFlags						flags;
		GvConnection					incoming;
		GvDestArray						outgoing;
		Int32									main_id;
		Int32									sub_id;
		Int32									user_id;
		String								name;
		GeData								userData;

	public:
		GvRealPort(GvPortIO io);
		virtual ~GvRealPort();

		Bool								Init(Int32 port_main_id, Int32 port_sub_id, GvDataID type = ID_GV_DATA_TYPE_UNDEFINED);

		GvPortFlags					GetFlags() const;
		void								SetFlags(GvPortFlags v);
 		Bool								GetVisible() const;
 		void								SetVisible(Bool v);
 		Bool								GetIterate() const;
 		void								SetIterate(Bool v);

 		GvPortIO						GetIO() const;
 		void								SetIO(GvPortIO io);
		Int32								GetMainID() const;
		void								SetMainID(Int32 id);
		Int32								GetSubID() const;
		void								SetSubID(Int32 id);
 		Int32								GetUserID() const;
 		void								SetUserID(Int32 id);
		Bool								SetValueType(GvValueID type);
		GvValueID						GetValueType() const;
		Bool								SetDataType(GvDataID type);
		GvDataID						GetDataType() const;

 		GeData*							GetUserData();
 		void								SetUserData(const GeData& data);

 		const String				GetName(GvRealNode *node) const;
 		void								SetName(const String &n);

		Bool InitValue(Int32 cpu_count)
		{
			return value.Init(value_type_idx, cpu_count);
		}

		void FreeValue()
		{
			value.Free();
		}

 		Bool								SetIncoming(GvRealNode *source_node, GvRealPort *source_port, GvRealNode *dest_node, GvRealPort *dest_port);
 		void								RemoveIncoming();
 		Bool								GetIncomingDestination(GvRealNode *&node, GvRealPort *&port) const;

		Bool IsIncomingConnected() const
		{
			return incoming.from_port != nullptr && incoming.from_realnode != nullptr;
		}

		Bool GetIncomingSource(GvRealNode *&node, GvRealPort *&port) const
		{
			node = incoming.from_realnode;
			port = incoming.from_port;
			return node && port;
		}

		Bool GetIncomingSource(GvRealNodeData *&node, GvRealPort *&port) const
		{
			node = incoming.from_nodedata;
			port = incoming.from_port;
			return node && port;
		}

		void								RemoveConnections(Bool remove_incoming = true, Bool remove_outgoing = true);

	private:
		Bool								AddOutgoing(GvRealNode *node, GvRealPort *port);
		void								RemoveOutgoing(GvRealNode *node, GvRealPort *port);
		Bool								OutgoingExists(GvRealNode *node, GvRealPort *port);
};

inline GvValueID GvRealPort::GetValueType() const
{
	return value_type_id;
}

inline Bool GvRealPort::SetDataType(GvDataID type)
{
	data_type = type;
	return true;
}

inline GvDataID GvRealPort::GetDataType() const
{
	return data_type;
}

inline GvPortIO GvRealPort::GetIO() const
{
	return port_type;
}

inline void GvRealPort::SetIO(GvPortIO io)
{
	port_type = io;
}

inline Int32 GvRealPort::GetMainID() const
{
	return main_id;
}

inline void GvRealPort::SetMainID(Int32 id)
{
	main_id = id;
}

inline Int32 GvRealPort::GetSubID() const
{
	return sub_id;
}

inline void GvRealPort::SetSubID(Int32 id)
{
	sub_id = id;
}

inline Int32 GvRealPort::GetUserID() const
{
	return user_id;
}

inline void GvRealPort::SetUserID(Int32 id)
{
	user_id = id;
}

inline GvPortFlags GvRealPort::GetFlags() const
{
	return flags;
}

inline void GvRealPort::SetFlags(GvPortFlags v)
{
	flags = v;
}

inline Bool GvRealPort::GetVisible() const
{
	return 0 != (flags & GV_PORT_FLAG_IS_VISIBLE);
}

inline void GvRealPort::SetVisible(Bool v)
{
	flags = v ? (GvPortFlags)(flags | GV_PORT_FLAG_IS_VISIBLE) : (GvPortFlags)(flags & ~GV_PORT_FLAG_IS_VISIBLE);
}

inline Bool GvRealPort::GetIterate() const
{
	return 0 != (flags & GV_PORT_FLAG_IS_ITERATE);
}

inline void GvRealPort::SetIterate(Bool v)
{
	flags = v ? (GvPortFlags)(flags |  GV_PORT_FLAG_IS_ITERATE) : (GvPortFlags)(flags & ~ GV_PORT_FLAG_IS_ITERATE);
}

#pragma pack (pop)
}

#endif // GV_PORT_H__

////////////////////////////////
