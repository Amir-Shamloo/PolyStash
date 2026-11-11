/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASELIST2D_H__
#define C4D_BASELIST2D_H__

#include "c4d_gelistnode.h"
#include "c4d_gemarker.h"
#include "c4d_dynamicdescription.h"
#include "c4d_rootctrack.h"
#include "c4d_rootpluginnode.h"
#include "c4d_rootshader.h"

namespace cineware
{
#pragma pack (push, 8)

class UniqueIDArray;
class NotifyList;
class NodeData;
class DescIdStore;
class CTrack;
struct LayerData;
class BaseDocument;
class BaseOverride;
class BaseShader;
class LayerObject;
class BaseMaterial;
class BaseSceneHook;
class RenderData;

//----------------------------------------------------------------------------------------
/// Represents a node in a 2D list.
//----------------------------------------------------------------------------------------
class BaseList2D : public GeListNode
{
	INSTANCEOF(BaseList2D, GeListNode)

	friend class RootList2D;
	friend class RootShader;
	friend class RootCKey;
	friend class RootCTrack;
	friend class RootObject;
	friend class RootMultipass;
	friend class RootVideoPost;
	friend class BaseDocument;
	friend class RootRenderData;
	friend class BaseShader;
	friend class BaseList4D;
	friend class BaseObject;
	friend class WeightTagData;
	friend class RootBaseDraw;
	friend class RootPluginNode;
	friend class BaseOverrideData;
	friend class BaseSceneHook;
	friend class PluginNode;

private:
	String						 m_name;
	BaseLink					 mp_layer;
	GeMarker					 m_marker;
	UniqueIDArray*		 l_uniquearray;

	DynamicDescription dynamicdescription;

protected:
	Int32			 m_id;
	RootShader mp_rootshader;
	RootShader mp_rootshader_hidden;
	RootShader root_shader_repository;

	Int32			 l_bitfeld;
	void SetID(Int32 id);

	RootCTrack		 mp_rootctrack;
	RootPluginNode root_override;

	NotifyList*		 notify_events;
	DescIdStore*	 keyframestore;
	DescIdStore*	 propstore;

	BaseContainer	 m_bc;

	NodeData*			 m_data;

	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);
	virtual Int32 GetInstanceType() const;

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseList2D();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseList2D();

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next base list in the list.
	/// @return												The next node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseList2D}
	//----------------------------------------------------------------------------------------
	BaseList2D* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous base list in the list.
	/// @return												The previous node, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{BaseList2D}
	//----------------------------------------------------------------------------------------
	BaseList2D* GetPred();

	/// @}

	/// @name Node Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the node data of the object.
	/// @return												The node data, or @formatParam{nullptr}. @theOwnsPointed{object,node data}
	//----------------------------------------------------------------------------------------
	NodeData* GetNodeData();

	//----------------------------------------------------------------------------------------
	/// Sets the node data of the object.
	/// @param[in] data								The node data to set. The object takes over the ownership of the pointed node data.
	//----------------------------------------------------------------------------------------
	void SetNodeData(NodeData* data);

	/// @}

	/// @name Bit
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the base list bit flags denoted by @formatParam{mask} to @formatConstant{true}.
	/// @param[in] mask								Bit mask of the flags.
	//----------------------------------------------------------------------------------------
	void SetBit(Int32 mask);

	//----------------------------------------------------------------------------------------
	/// Gets the state of the base list bit flags denoted by @formatParam{mask}.
	/// @param[in] mask								Bit mask of the flags.
	/// @return												@trueIfOtherwiseFalse{the flags denoted by @formatParam{mask} are set}
	//----------------------------------------------------------------------------------------
	Bool GetBit(Int32 mask);

	//----------------------------------------------------------------------------------------
	/// Deletes the base list bit flags denoted by @formatParam{mask}, i.e. sets the corresponding bit to @formatConstant{false}.
	/// @param[in] mask								Bit mask of the flags.
	//----------------------------------------------------------------------------------------
	void DelBit(Int32 mask);

	//----------------------------------------------------------------------------------------
	/// Toggles the state of the base list bit flags denoted by @formatParam{mask}.
	/// @param[in] mask								Bit mask of the flags.
	//----------------------------------------------------------------------------------------
	void ToggleBit(Int32 mask);

	//----------------------------------------------------------------------------------------
	/// Gets all of the base list's bit flags.
	/// @return												The flags.
	//----------------------------------------------------------------------------------------
	Int32 GetAllBits();

	//----------------------------------------------------------------------------------------
	/// Sets all of the base list's bit flags at once.
	/// @param[in] bits								New flags.
	//----------------------------------------------------------------------------------------
	void SetAllBits(Int32 bits);

	/// @}

	/// @name Data
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the base list's container.
	/// @return												The base list's container.
	//----------------------------------------------------------------------------------------
	BaseContainer GetData();

	//void SetData(const BaseContainer &bc, Bool add=true);

	//----------------------------------------------------------------------------------------
	/// Gets a pointer to the base list's container that can be changed directly.
	/// @return												A pointer to the internal base list's container. @theOwnsPointed{base list,container}
	//----------------------------------------------------------------------------------------
	BaseContainer* GetDataInstance();

	//----------------------------------------------------------------------------------------
	/// Retrieves a reference to the object container that can be changed directly.
	/// @see GetData()
	/// @since R20
	/// @return												A reference to the internal container. The node owns the referenced container.
	//----------------------------------------------------------------------------------------
	BaseContainer& GetDataInstanceRef();

	/// @}

	/// @name Name
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the name of the base list.
	/// @return												The name.
	//----------------------------------------------------------------------------------------
	const String& GetName();

	//----------------------------------------------------------------------------------------
	/// Sets the name of the object.
	/// @param[in] new_name						The new name.
	//----------------------------------------------------------------------------------------
	void SetName(const String& new_name);

	/// @}

	/// @name Marker
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the object's marker.
	/// @return												The object's marker.
	//----------------------------------------------------------------------------------------
	const GeMarker& GetMarker() const;

	//----------------------------------------------------------------------------------------
	/// Sets the object's marker.
	/// @since 18.002
	/// @param[in] m									The new object's marker.
	//----------------------------------------------------------------------------------------
	void SetMarker(const GeMarker& m);

	/// @}

	/// @name Unique ID
	/// @{

	//----------------------------------------------------------------------------------------
	/// Adds unique ID data to the base list.
	/// @note	A unique ID can be used for instance to identify scenes written by external applications.
	/// @param[in] appid							Unique application ID. Has to be registered at MAXON and obtained from http://www.plugincafe.com.
	/// @param[in] mem								Pointer to a block of memory. Used for instance to store the name of a software vendor.
	/// @param[in] bytes							Length of @formatParam{mem} memory block.
	/// @return												@trueIfOtherwiseFalse{the unique ID was added}
	//----------------------------------------------------------------------------------------
	Bool AddUniqueID(Int32 appid, const Char* const mem, Int bytes);

	//----------------------------------------------------------------------------------------
	/// Checks for a specific unique application ID data.
	/// @note	A unique ID can be used for instance to identify scenes written by external applications.
	/// @param[in] appid							Unique application ID. Has to be registered at MAXON and obtained from http://www.plugincafe.com.
	/// @param[in] mem								Assigned a pointer to a block of memory. Used for instance to read the name of a software vendor.
	/// @param[in] bytes							Assigned the length of @formatParam{mem} block.
	/// @return												@trueIfOtherwiseFalse{the unique ID could be found}
	//----------------------------------------------------------------------------------------
	Bool FindUniqueID(Int32 appid, const Char*& mem, Int& bytes) const;

	//----------------------------------------------------------------------------------------
	/// Gets the number of unique IDs.
	/// @note	A unique ID can be used for instance to identify scenes written by external applications.
	/// @return												The number of unique IDs.
	//----------------------------------------------------------------------------------------
	Int32 GetUniqueIDCount() const;

	//----------------------------------------------------------------------------------------
	/// Gets the @formatParam{idx}-th unique ID data.\n
	/// @note	A unique ID can be used for instance to identify scenes written by external applications.
	/// @param[in] idx								Unique ID index.
	/// @param[in] id									Assigned the unique ID.
	/// @param[in] mem								Assigned a pointer to a block of memory. Used for instance to read the name of a software vendor.
	/// @param[in] bytes							Assigned the length of @formatParam{mem} block.
	/// @return												@trueIfOtherwiseFalse{the unique ID index could be found}
	//----------------------------------------------------------------------------------------
	Bool GetUniqueIDIndex(Int32 idx, Int32& id, const Char*& mem, Int& bytes) const;

	/// @}

	/// @name Shader
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the first shader of the base list.\n
	/// @note	Normally you do not have to deal with the shader list. But if you want to create or modify shader trees you need to read the following lines:\n
	///				If you are programming shader links you need to make sure that every shader is referenced only once; it is not allowed that a shader is referenced multiple times.\n
	///				If the referencing object is a shader, the referenced shader must be child of it, otherwise it must be inserted via InsertShader().
	///
	/// Example 1: A tag references a shader. The shader must be inserted into the tag using InsertShader();.
	/// Example 2: A Shader (A) references another shader (B): the shader B must be child of the shader A.
	///
	/// @return												The first shader of the base list, or @formatConstant{nullptr} if there are none. @theOwnsPointed{object, shader}
	//----------------------------------------------------------------------------------------
	BaseShader* GetFirstShader();

	//----------------------------------------------------------------------------------------
	/// Inserts a shader in the base list's shader list.
	/// @code
	/// BaseMaterial *mat = nullptr;
	/// mat = doc->GetFirstMaterial();
	/// if (!mat) return true;
	///
	/// BaseContainer *data = mat->GetDataInstance();
	/// if (!data) return true;
	///
	/// BaseShader *shd = BaseShader::Alloc(Xbitmap);
	/// if (!shd) return false;
	///
	/// data->SetLink(MATERIAL_COLOR_SHADER, shd);
	/// mat->InsertShader(shd);
	/// mat->Message(MSG_UPDATE);
	/// mat->Update(true, true);
	///
	/// EventAdd();
	/// @endcode
	/// @param[in] shader							The shader to insert. The object takes over the ownership of the pointed shader.
	/// @param[in] pred								The shader to insert the new shader after, or @formatConstant{nullptr} (default) to insert the new shader first. The pointed shader must already be inserted into the list.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertShader(BaseShader* shader, BaseShader* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Inserts a shader in the base list's shader list as last shader.
	/// @see InsertShader()
	/// @param[in] shader							The shader to insert. The object takes over the ownership of the pointed shader.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertShaderLast(BaseShader* shader);

	/// @}

	/// @name CTrack
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the track root of the base list.
	///
	/// @return												The track root.
	//----------------------------------------------------------------------------------------
	RootCTrack* GetCTrackRoot();

	//----------------------------------------------------------------------------------------
	/// Gets the first track of the base list.
	/// @return												The first track, or @formatConstant{nullptr} if there are no tracks.
	//----------------------------------------------------------------------------------------
	CTrack* GetFirstCTrack();

	//----------------------------------------------------------------------------------------
	/// Finds the track for the specified description @formatParam{id}.
	/// @param[in] id									The description ID for the searched track.
	/// @return												The found track, or @formatConstant{nullptr} if there was no match.
	//----------------------------------------------------------------------------------------
	CTrack* FindCTrack(const DescID& id);

	//----------------------------------------------------------------------------------------
	/// Appends a track and automatically places it at the right place (so that Y comes after X for example).
	/// @param[in] t									The track to append.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool AppendCTrack(CTrack* t);

	//----------------------------------------------------------------------------------------
	/// Inserts a track and automatically places it at the right place (so that Y comes after X for example).
	/// @since 18.002
	/// @param[in] track							The track to insert.
	//----------------------------------------------------------------------------------------
	void InsertTrackSorted(CTrack* track);

	/// @}

	/// @name Type
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the type of the base list.
	/// @warning	This must be used to make sure that the derived object is really of the right type before casting it and accessing its members.
	/// @note			Plugin nodes return their unique plugin IDs.
	/// @return												The type, for example @em Ocube.
	/// @markTODO{Link to objects type IDs}
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	//----------------------------------------------------------------------------------------
	/// Gets the unique disk type of the base list.
	/// @note	Normally this is the same as GetType() but there are exceptions.
	/// @return												The disk type, for example @em Ocube.
	/// @markTODO{Link to objects type IDs}
	//----------------------------------------------------------------------------------------
	virtual Int32 GetDiskType() const;

	/// @}

	/// @name Layer
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the layer for the base list.
	/// @return												The layer, or @formatConstant{nullptr}. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetLayerObject();

	//----------------------------------------------------------------------------------------
	/// Sets the layer for the base list.
	/// @param[in] layer							The layer to set. The object takes over the ownership of the pointed layer.
	//----------------------------------------------------------------------------------------
	void SetLayerObject(LayerObject* layer);

	//----------------------------------------------------------------------------------------
	/// Gets the layer data for the object.
	/// @since 17.008
	/// @param[in] doc								The document for the operation.
	/// @param[in] rawdata						Usually GetLayerData() takes special global modes like @em SOLO layer automatically into account.\n
	///                               To get the original layer values without any additional global changes set @formatParam{rawdata} to @formatConstant{true}.
	/// @return												The layer data, or @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	const LayerData* GetLayerData(BaseDocument* doc, Bool rawdata = false);

	//----------------------------------------------------------------------------------------
	/// Sets the layer data for the object.
	/// @since 17.008
	/// @param[in] doc								The document for the operation.
	/// @param[in] data								The new layer data.
	/// @return												@trueIfOtherwiseFalse{the layer data was successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetLayerData(BaseDocument* doc, const LayerData& data);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the base list.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @return												The cloned base list. @callerOwnsPointed{object}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	//----------------------------------------------------------------------------------------
	/// Gets the dynamic description of the base list. Also known as the <i>User Data</i> part of the <i>Attribute Manager</i>.
	/// @return												The dynamic description. @theOwnsPointed{base list,dynamic description}
	//----------------------------------------------------------------------------------------
	DynamicDescription* GetDynamicDescription();

	//----------------------------------------------------------------------------------------
	/// Checks if it is a base list.
	/// @return												@trueIfOtherwiseFalse{a base list}
	//----------------------------------------------------------------------------------------
	virtual Bool IsBaseList();

	//----------------------------------------------------------------------------------------
	/// Removes the node from its list.
	/// @warning	When a base list is removed the owner become responsible for freeing it or passing its ownership to another list.
	//----------------------------------------------------------------------------------------
	virtual void Remove();

	//----------------------------------------------------------------------------------------
	/// Sets the state for the description @formatParam{id}.
	/// @since 17.008
	/// @param[in] id									The description ID.
	/// @param[in] descidstate				The new description ID state: @enumerateEnum{DESCIDSTATE}
	/// @return												@trueIfOtherwiseFalse{the description ID state was successfully set}
	//----------------------------------------------------------------------------------------
	Bool SetDescIDState(const DescID& id, DESCIDSTATE descidstate);

	//----------------------------------------------------------------------------------------
	/// Gets the state of a description @formatParam{id}.
	/// @since 17.008
	/// @param[in] id									The description ID.
	/// @param[in] tolerant						If set to @formatConstant{true} a valid state is returned even though the description ID do not match-completly.\n
	///                               This can be useful for descriptions with several levels. Take this description ID for example:
	///                               @code
	///                               DescID(DescLevel(1000, DTYPE_VECTOR, 0), DescLevel(1, DTYPE_REAL, 0))
	///                               @endcode
	///                               If @formatParam{tolerant} is @formatConstant{true} a valid state is returned even though only the following description ID is passed:
	///                               @code
	///                               DescID(DescLevel(1000, DTYPE_VECTOR, 0))
	///                               @endcode
	/// @return												The description ID state: @enumerateEnum{DESCIDSTATE}
	//----------------------------------------------------------------------------------------
	DESCIDSTATE GetDescIDState(const DescID& id, Bool tolerant) const;

	//-------------------------------------------------------------------------------------------------
	/// @markPrivate
	//-------------------------------------------------------------------------------------------------
	RootPluginNode/*GeListHead*/* GetOverrideRoot();

	//-------------------------------------------------------------------------------------------------
	/// @markPrivate
	//-------------------------------------------------------------------------------------------------
	BaseOverride* GetFirstOverride();

	//-------------------------------------------------------------------------------------------------
	/// @markPrivate
	//-------------------------------------------------------------------------------------------------
	virtual Bool TranslateDescID(const DescID& id, DescID& res_id, GeListNode*& res_at);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_BASELIST2D_H__
