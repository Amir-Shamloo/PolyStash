/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEDOCUMENT_H__
#define C4D_BASEDOCUMENT_H__

#include "c4d_system.h"
#include "c4d_parameter_ids.h"
#include "c4d_baselist2d.h"
#include "c4d_documentpref.h"
#include "c4d_takedata.h"

namespace cineware
{
#pragma pack (push, 8)

class	RootObject;
class	RootMaterial;
class	RootLayer;
class	RootRenderData;
class	RootViewPanel;
class	RootSceneHook;
class ViewPanel;
class BaseDraw;
class	BaseMaterial;
class	BaseSceneHook;
class RenderData;
class CKey;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct CDefaultKey
{
	CINTERPOLATION				inter;
	CAUTOMODE							automode;
	ROTATIONINTERPOLATION	quatInter;

	Bool									naam;	// Overdub option.
	Bool									mute;
	Bool									clamp;
	Bool									brek;
	Bool									aut;
	Bool									lock_o;
	Bool									lock_l;
	Bool									lock_v;
	Bool									lock_t;
	Bool									zero_o;
	Bool									zero_l;
	Bool									keepangle;
	Bool									weighted;
	Bool									removeOvershoot;
	Bool									autoWeight;
};

//----------------------------------------------------------------------------------------
/// Holds the values for axis locks and the world/object coordinate system switch.
//----------------------------------------------------------------------------------------
class AxisState
{
public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	AxisState();

	Bool x,	///< %State of the @em X-axis lock.
			 y,	///< %State of the @em Y-axis lock.
			 z,	///< %State of the @em Z-axis lock.
			 w;	///< %State of the <i>World</i>/<i>Object</i> coordinate system switch.
};

//----------------------------------------------------------------------------------------
/// Contains the complete description of a @C4D document.
//----------------------------------------------------------------------------------------
class BaseDocument : public BaseList2D
{
	INSTANCEOF(BaseDocument, BaseList2D)

	friend class CKey;
	friend class CCurve;
	friend class PrivateChunk;
	friend class BaseMaterial;
	friend BaseDocument* LoadDocument(const Filename& name, SCENEFILTER loadflags);

private:
	RootObject*			mp_ops;
	RootMaterial*		mp_mats;
	RootLayer*			mp_layers;
	RootRenderData*	mp_rdata;
	RootViewPanel*	mp_viewpanel;
	RootSceneHook*	mp_scenehook;
	Bool						caches;
	BaseBitmap			m_previewbitmap;
	Int32						loadedc4dversion;

	// main data
	BaseTime			d_time;
	Int32					d_modus, l_mode;
	Int32					d_action, l_action, t_action;
	AxisState			state[4];

	Filename			pfad, name;
	DocumentPref	docpref;

	BaseContainer	tl_data;
	BaseContainer	obj_data;
	BaseContainer	mat_data;
	BaseContainer	coo_data;
	BaseContainer	modeling_data;
	BaseContainer	modeling_plugin_data;

	TakeData			_takeData;

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	ViewPanel* GetFirstViewPanel();

	void ClearDocument();

protected:
	CDefaultKey	cdefaultkey;	// contains the default setup for a keyframe
	CDefaultKey	cCustomkey;		// contains the Latest Custom Preset Configuration
	Int32				m_loadflags;

	void CopyToPresetSettings(const CKey& fromKey);
	void RestoreFromPresetSettings(CKey& toKey) const;

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseDocument();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseDocument();

	/// @}

	/// @name Allocation/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{document}
	/// @return												@allocReturn{document}
	//----------------------------------------------------------------------------------------
	static BaseDocument* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{documents}
	/// @param[in,out] v							@theToDestruct{document}
	//----------------------------------------------------------------------------------------
	static void Free(BaseDocument*& v);

	//----------------------------------------------------------------------------------------
	/// Gets the object type for a document.
	/// @note	Always returns @ref Tbasedocument.
	/// @return												The object type for a BaseDocument i.e. @ref Tbasedocument.
	//----------------------------------------------------------------------------------------
	virtual Int32 GetType() const;

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next document in the list. Convenience version of BaseList2D::GetNext() returning a pointer to a BaseDocument.
	/// @return												The next document, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{document}
	//----------------------------------------------------------------------------------------
	BaseDocument* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous document in the list. Convenience version of GeListNode::GetPred() returning a pointer to a BaseDocument.
	/// @return												The previous document, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{document}
	//----------------------------------------------------------------------------------------
	BaseDocument* GetPred();

	/// @}

	/// @name Cloning
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the document.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @return												The cloned document. @callerOwnsPointed{document}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	/// @}

	/// @name Create from/to Cinema 4D
	/// @{

	//----------------------------------------------------------------------------------------
	/// Creates a scene from a @C4D file in the external application.\n
	/// The method goes through the scene structure (objects, material, layer) and build the scene, it calls the execute function of the Alien classes.
	/// @see The articles @ref page_implementation_cineware and @ref page_implementation_on_the_external_side.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool CreateSceneFromC4D();

	//----------------------------------------------------------------------------------------
	/// Must be overloaded by the user.\n
	/// Creates a @C4D file from the external application. The user should go through the external scene structure (objects, materials, layers) and build the @C4D document.
	/// @param[in] selectedOnly				Pass @trueOtherwiseFalse{to export the selection only}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool CreateSceneToC4D(Bool selectedOnly);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CreateObjectFromC4D(BaseObject* obj);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CreateLayerFromC4D(LayerObject* lay);

	/// @}

	/// @name Get First Object/Material/RenderData/Layer
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the first object of the document. The objects in the document can be looped through with BaseObject::GetNext(), BaseObject::GetPred(), BaseObject::GetDown() etc.
	/// @return												The first object of the document, or @formatConstant{nullptr} if there are no objects in the document. @theOwnsPointed{document,object}
	//----------------------------------------------------------------------------------------
	BaseObject* GetFirstObject();

	//----------------------------------------------------------------------------------------
	/// Gets the first material of the document. The other materials in the document can be be looped through with BaseMaterial::GetNext() and BaseMaterial::GetPred().
	/// @return												The first material of the document, or @formatConstant{nullptr} if there are no materials in the document. @theOwnsPointed{document,material}
	//----------------------------------------------------------------------------------------
	BaseMaterial* GetFirstMaterial();

	//----------------------------------------------------------------------------------------
	/// Gets the first render data/settings of the document. The other render settings in the document can be looped trough with RenderData::GetNext(), RenderData::GetPred(), RenderData::GetDown() etc.
	/// @return												The first render data/settings of the document, or @formatConstant{nullptr} if there are no render data/settings associated with the document. @theOwnsPointed{document,render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetFirstRenderData();

	//----------------------------------------------------------------------------------------
	/// Gets the first scene hook of the document. The other scene hooks in the document can be looped trough with BaseSceneHook::GetNext() and BaseSceneHook::GetPred().
	/// @since 17.008
	/// @return												The first scene hook of the document, or @formatConstant{nullptr} if there are no scene hook associated with the document. @theOwnsPointed{document,scene hook}
	//----------------------------------------------------------------------------------------
	BaseSceneHook* GetFirstSceneHook();

	//----------------------------------------------------------------------------------------
	/// Finds a scene hook by ID.
	/// @since 17.008
	/// @param[in] id									The scene hook ID.
	/// @return												The found scene hook, or @formatConstant{nullptr} if there is no scene hook with the searched @formatParam{id}. @theOwnsPointed{document, scene hook}
	//----------------------------------------------------------------------------------------
	BaseSceneHook* FindSceneHook(Int32 id);

	//----------------------------------------------------------------------------------------
	/// Gets the first layer in the document. The other layers in the document can be looped trough with LayerObject::GetNext(), LayerObject::GetPred(), LayerObject::GetDown() etc.
	/// @return												The first layer of the document, or @formatConstant{nullptr} if there are no layers in the document. @theOwnsPointed{document,layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetFirstLayer();

	/// @}

	/// @name Insert Object/Material/RenderData/Layer
	/// @{

	//----------------------------------------------------------------------------------------
	/// Inserts the object into the document's object hierarchy.\n
	/// Optionally the insertion position can either be specified by the @formatParam{parent} parameter, inserting the object as the first child of the specified @formatParam{parent}, or by the @formatParam{pred} parameter, inserting the object below the specified @formatParam{pred} object.\n
	/// If neither is given, the object is inserted at the top of the hierarchy.
	/// @note	If both @formatParam{parent} and @formatParam{pred} are provided then @formatParam{pred} has precedence.
	/// @param[in] op									The object to insert into the document. The document takes over the ownership of the pointed object.
	/// @param[in] parent							An optional parent object in the document to insert the object as a child of. Can be @formatConstant{nullptr}. @callerOwnsPointed{object}
	/// @param[in] pred								An optional object in the document to use as insertion point. @callerOwnsPointed{object}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertObject(BaseObject* op, BaseObject* parent, BaseObject* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Inserts the material into the document's material list.\n
	/// Optionally the insertion position can be specified by the @formatParam{pred} parameter, giving the material before (just to the left of) the wanted position. Otherwise the material is inserted at the first position in the list.
	/// @param[in] mat								The material to insert into the document. The document takes over the ownership of the pointed material.
	/// @param[in] pred								An optional material in the document to use as insertion point. @callerOwnsPointed{material}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertMaterial(BaseMaterial* mat, BaseMaterial* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Inserts the render data into the document's render settings hierarchy.\n
	/// Optionally the insertion position can either be specified by the @formatParam{parent} parameter, inserting the render data as the first child of the specified @formatParam{parent}, or by the @formatParam{pred} parameter, inserting the render data below the specified @formatParam{pred} render data.\n
	/// If neither is given, the render data is inserted at the top of the hierarchy.
	/// @note	If both @formatParam{parent} and @formatParam{pred} are provided then @formatParam{pred} has precedence.
	/// @param[in] rd									The render data to insert. The document takes over the ownership of the pointed render data.
	/// @param[in] parent							An optional parent to insert the render data as a child of. @callerOwnsPointed{render data}
	/// @param[in] pred								An optional render data of the document to use as insertion point. @callerOwnsPointed{render data}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertRenderData(RenderData* rd, RenderData* parent, RenderData* pred = nullptr);

	//----------------------------------------------------------------------------------------
	/// Inserts the render data as last child into the document's render data list.
	/// @param[in] rd									The render data to insert. The document takes over the ownership of the pointed render data.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertRenderDataLast(RenderData* rd);

	//----------------------------------------------------------------------------------------
	/// Inserts the layer into the document's layer list.\n
	/// The insertion position can either be specified by the @formatParam{parent} parameter, inserting the layer as the first child of the specified parent, or by the @formatParam{pred} parameter, inserting the layer below the specified @formatParam{pred} layer.\n
	/// If neither is given, the layer is inserted at the top of the hierarchy.
	/// @note	If both @formatParam{parent} and @formatParam{pred} are provided then @formatParam{pred} has precedence.
	/// @param[in] pLay								The layer to insert. The document takes over the ownership of the pointed object.
	/// @param[in] parent							An optional parent to insert the layer as a child of. @callerOwnsPointed{layer}
	/// @param[in] pred								An optional layer of the document to use as insertion point. @callerOwnsPointed{layer}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool InsertLayer(LayerObject* pLay, LayerObject* parent, LayerObject* pred = nullptr);

	/// @}

	/// @name Get/Set RenderData
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the active render data for the document. These are the settings used for rendering the document.
	/// @return												The active render data/settings. @theOwnsPointed{document,render data}
	//----------------------------------------------------------------------------------------
	RenderData* GetActiveRenderData() const;

	//----------------------------------------------------------------------------------------
	/// Sets the active render data for the document. These are the settings used for rendering the document.
	/// @param[in] rd									The render data/settings to make active. Must be in the document already. @theOwnsPointed{document,render data}
	//----------------------------------------------------------------------------------------
	void SetActiveRenderData(RenderData* rd);

	/// @}

	/// @name Time/Mode/Action/Fps

	//----------------------------------------------------------------------------------------
	/// Gets the current time for the document's Timeline.\n
	/// Here is how to get the current frame with GetTime():
	/// @code
	/// Int32 fps = doc.GetFps();
	/// BaseTime time = doc->GetTime();
	/// frame = time.GetFrame(fps);
	/// @endcode
	/// @see GetFps()
	/// @return												The current time of the Timeline.
	//----------------------------------------------------------------------------------------
	const BaseTime& GetTime() const;

	//----------------------------------------------------------------------------------------
	/// Sets the current time for the document's Timeline.
	/// @param[in] new_time						The new current time of the Timeline.
	//----------------------------------------------------------------------------------------
	void SetTime(const BaseTime& new_time);

	//----------------------------------------------------------------------------------------
	/// Gets the main editor mode.
	/// @return												The main editor mode.
	//----------------------------------------------------------------------------------------
	Int32 GetMode() const;

	//----------------------------------------------------------------------------------------
	/// Sets the main editor mode.
	/// @param[in] m									The main editor mode.
	//----------------------------------------------------------------------------------------
	void SetMode(Int32 m);

	//----------------------------------------------------------------------------------------
	/// Gets the current action tool in the editor.
	/// @return												The current action tool: ::ID_MODELING_MOVE, ::ID_MODELING_SCALE, ::ID_MODELING_ROTATE etc.
	//----------------------------------------------------------------------------------------
	Int32 GetAction() const;

	//----------------------------------------------------------------------------------------
	/// Sets the current action tool in the editor.
	/// @param[in] a									The new action tool: ::ID_MODELING_MOVE, ::ID_MODELING_SCALE, ::ID_MODELING_ROTATE etc.
	//----------------------------------------------------------------------------------------
	void SetAction(Int32 a);

	//----------------------------------------------------------------------------------------
	/// Gets the number of Frames per Second (FPS).
	/// @return												The number of Frames per Second.
	//----------------------------------------------------------------------------------------
	Int32 GetFps();

	//----------------------------------------------------------------------------------------
	/// Sets the number of Frames per Second (FPS).
	/// @param[in] fps								The new number of Frames per Second.
	//----------------------------------------------------------------------------------------
	void SetFps(Int32 fps);

	/// @}

	/// @name Editor Views
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the activate view in the editor.
	/// @return												The active view in the editor or @formatConstant{nullptr} if there is no active view. @theOwnsPointed{document,BaseDraw}
	//----------------------------------------------------------------------------------------
	BaseDraw* GetActiveBaseDraw();

	//----------------------------------------------------------------------------------------
	/// Gets the view the user has chosen as <i>Render View</i> in the editor (see @em View menu in @C4D).
	/// @return												The render view in the editor or @formatConstant{nullptr} if there is no render view. @theOwnsPointed{document,BaseDraw}
	//----------------------------------------------------------------------------------------
	BaseDraw* GetRenderBaseDraw();

	//----------------------------------------------------------------------------------------
	/// Gets the BaseDraw from one of the editor views.
	/// @param[in] num								If there are multiple views this selects the view. @em 0 <= @formatParam{num} < GetBaseDrawCount()
	/// @return												The requested view in the editor, or @formatConstant{nullptr} if it is not available. @theOwnsPointed{document,BaseDraw}
	//----------------------------------------------------------------------------------------
	BaseDraw* GetBaseDraw(Int32 num);

	//----------------------------------------------------------------------------------------
	/// Gets the number of views in the editor.
	/// @return												The view count.
	//----------------------------------------------------------------------------------------
	Int32 GetBaseDrawCount();

	//----------------------------------------------------------------------------------------
	/// Gets the default @formatParam{near} and @formatParam{far} clipping planes values from a clipping preset or from the document clipping preset.
	/// @param[out] near							Assigned the default near clipping plane value.
	/// @param[out] far								Assigned the default far clipping plane value.
	/// @param[in] d									Pass a clipping preset ID to get the near/far values for this preset instead of the values from the document preset. See @ref DOCUMENT_CLIPPING_PRESET enum in @em parameter_ids\ddoc.h.
	//----------------------------------------------------------------------------------------
	void GetDefaultClippingPlanes(Float& near, Float& far, const GeData* d = nullptr);

	/// @}

	/// @name Cineware SDK Convenience Methods
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the current scene camera.
	/// @return												The scene camera, or @formatConstant{nullptr} if no scene camera is used.
	//----------------------------------------------------------------------------------------
	BaseObject* GetSceneCamera();

	//----------------------------------------------------------------------------------------
	/// Sets a new scene camera. If @formatParam{opCam} is @formatConstant{nullptr}, the editor camera is used.
	/// @param[in] opCam							The new scene camera. If @formatConstant{nullptr}, the editor camera is used. @callerOwnsPointed{camera}
	/// @return												@trueIfOtherwiseFalse{the camera could be set}
	//----------------------------------------------------------------------------------------
	Bool SetSceneCamera(BaseObject* opCam);

	//----------------------------------------------------------------------------------------
	/// Creates a Physical Sky object within the document and returns it. Basically the same as calling the Physical Sky command in @C4D.\n
	/// <b>Example:</b>\n
	/// Adding sky shader object.
	/// @code
	/// BaseDocument *newC4DDoc = BaseDocument::Alloc();
	/// BaseObject *sky = newC4DDoc->CreateSky();
	/// @endcode
	/// @return												The newly created sky object. @theOwnsPointed{document,sky object}
	//----------------------------------------------------------------------------------------
	BaseObject* CreateSky();

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this for self defined Alien document classes. Called when Cineware SDK loads a document.
	/// <b>Example:</b>
	/// @code
	/// class AlienBaseDocument : public BaseDocument
	/// {
	/// public:
	///   virtual Bool Execute();
	/// };
	///
	/// Bool AlienBaseDocument::Execute()
	/// {
	///   // Do something with the document, for instance access the document settings
	///
	///   GeData data;
	///   if (GetParameter(DOCUMENT_DOCUNIT, data) && data.GetType() == CUSTOMDATATYPE_UNITSCALE)
	///   {
	///     UnitScaleData* ud = (UnitScaleData*)data.GetCustomDataType(CUSTOMDATATYPE_UNITSCALE);
	///     DOCUMENT_UNIT docUnit;
	///     Real docScale;
	///     ud->GetUnitScale(docScale, docUnit);
	///   }
	///
	///   return true;
	/// }
	/// @endcode
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	//----------------------------------------------------------------------------------------
	/// Gets the preview bitmap for the document.\n
	/// This is the same as the preview bitmap of the Windows Explorer / OS X Finder thumbnails.
	/// @return												The preview bitmap for the document.
	//----------------------------------------------------------------------------------------
	BaseBitmap& GetDocPreviewBitmap()
	{
		return m_previewbitmap;
	}

	//----------------------------------------------------------------------------------------
	/// Gets the document's default keying settings.
	/// @param[in] pKey								Assigned the default keying settings. @cinewareOwnsPointed{key}
	/// @param[in] bOverdub						Assigned the document's default overdub.
	/// @return												@trueIfOtherwiseFalse{the document's default keying settings were retrieved successfully}
	//----------------------------------------------------------------------------------------
	Bool GetDefaultKey(CKey* pKey, Bool& bOverdub) const;

	//----------------------------------------------------------------------------------------
	/// Sets the document's default keying settings.
	/// @since 17.008
	/// @param[in] pKey								The new default keying settings. @cinewareOwnsPointed{key}
	/// @param[in] bOverdub						The new document's default overdub.\n
	///                               If a keyframe already exists, the existing keyframe's interpolation type will be maintained when a new keyframe is recorded (only the Value Key will be overwritten).
	//----------------------------------------------------------------------------------------
	void SetDefaultKey(CKey* pKey, Bool bOverdub);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Int32 GetLastAction() const;

	//----------------------------------------------------------------------------------------
	/// Gets the state of the Axis locks and the World/Object coordinate switch.
	/// @return												The state of the Axis locks and the World/Object coordinate switch. @cinewareOwnsPointed{AxisState structure}
	//----------------------------------------------------------------------------------------
	AxisState* GetState();

	//----------------------------------------------------------------------------------------
	/// Checks if the scene has a polygonal object cache.\n
	/// @C4D has a special safe flag in the document preferences where a polygonal object cache can be saved in every file.
	/// @see The article @ref page_polygon_and_object_caches.
	/// @return												@trueIfOtherwiseFalse{the document was saved with polygonal caches}
	//----------------------------------------------------------------------------------------
	Bool HasCaches();

	//----------------------------------------------------------------------------------------
	/// Gets the take data for the document.
	/// @since 17.008
	/// @return												The take data. @theOwnsPointed{document,take data}
	//----------------------------------------------------------------------------------------
	TakeData* GetTakeData();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual BaseDocument* GetDocument() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	/// @}
};

#pragma pack (pop)
}

/// @cond IGNORE

// Implement on exchange (external application) side, not in cineware namespace.

cineware::NodeData*				AllocAlienObjectData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node);
cineware::NodeData*				AllocAlienTagData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node);
cineware::NodeData*				AllocAlienShaderData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node);
cineware::RootMaterial*		AllocAlienRootMaterial();
cineware::RootObject*			AllocAlienRootObject();
cineware::RootLayer*				AllocAlienRootLayer();
cineware::RootRenderData*	AllocAlienRootRenderData();
cineware::LayerObject*			AllocAlienLayer();
cineware::RootViewPanel*		AllocC4DRootViewPanel();

/// @endcond


//----------------------------------------------------------------------------------------
/// Implement to return the exchange/external application ID and name.
/// @param[out] id								Assign the application ID. Has to be registered at MAXON and obtained from http://www.plugincafe.com.
/// @param[out] appname						Assign the application name.
//----------------------------------------------------------------------------------------
void GetWriterInfo(cineware::Int32& id, cineware::String& appname);

#endif	// C4D_BASEDOCUMENT_H__
