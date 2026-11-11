/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_BASEDRAW_H__
#define C4D_BASEDRAW_H__

#include "c4d_baselist2d.h"
#include "c4d_basedrawpref.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a @C4D view window that can be drawn into.
///
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class BaseDraw : public BaseList2D
{
	INSTANCEOF(BaseDraw, BaseList2D)

	friend class BaseDocument;
	friend class ViewPanel;

private:
	//		Filename smerk;
	//		BaseBitmap merk;
	//		BaseTime tmerk;

	//		BaseDocument *t_doc;
	//		BaseThread *thread;

	//		PainterBuffer *t_pbuf;
	//		ToolPlugin *t_tool;

	//		Bool only_grid;
	//		Bool only_back;

	//		Int32 displayfilter;
	//		Bool draw_zbuf;
	//		Matrix matrix_mg;
	//		MatrixMode matrix_mode;
	//		BaseObject *matrix_lastobject;
	//		MatrixStore matrix_object;
	//		MatrixStore matrix_camera;
	//		Bool slowlines,c4dlight,dualplanes;
	//		DrawObjectList postpass,postpass_back;
	//		String tempname;
	//		Bool m_bIsRenderAsEditor;
	//		CDynArray<LightShadowInfo> m_arLightShadowInfo;
	//		CDynamicSortPointerSet m_arCallbackHooks;
	//		Bool m_bShadowPass, m_bOGLPrepass;
	//		LightShadowInfo* m_pShadowInfo;
	//		PickObjectInfo* m_pPickObjectInfo;
	//		OITInfo m_OITInfo;
	//		Vector m_Mp, m_Rad;
	//		Bool m_bAllowHQ, m_bFramebufferRender;

	BaseDrawPref _bpref;
	BaseLink		 s_link;

	// editor camera
	BaseObject*	ed_camera;

	//		variables for shading
	//		RayLgt **light,*autolight;
	//		Int32 lightcnt;
	//		Vector ambient;
	//		BaseObject	*showpaintermesh;
	//		Bool paintermeshenabled;
	//		Bool singleactiveobject;
	//		const BaseDrawPref *m_bpref;
	//		Bool m_bLightsChanged;
	//		UInt32 m_ulLightstate;
	//		Int32 m_lLastLightList;
	//		GlFrameCache m_FrameCache;

public:
	//		Int32 a_merk_display;

	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseDraw();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~BaseDraw();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{base draw}
	/// @return												@allocReturn{base draw}
	//----------------------------------------------------------------------------------------
	static BaseDraw* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{base draws}
	/// @param[in,out] bd							@theToDestruct{base draw}
	//----------------------------------------------------------------------------------------
	static void Free(BaseDraw*& bd);

	/// @}

	/// @name Get/Set Parameter
	/// @{

	//----------------------------------------------------------------------------------------
	/// Convenience method to get base draw parameters:
	/// @code
	/// GeData BaseDraw::GetParameterData(Int32 id)
	/// {
	///   GeData t_data;
	///   BaseList2D::GetParameter(DescLevel(id), t_data, 0);
	///   return t_data;
	/// }
	/// @endcode
	///
	/// @param[in] id									A parameter ID. See @em dbasedraw.h description file.
	/// @param[in] data								Optionally assign the parameter data.
	/// @return												The parameter data for @formatParam{id}.
	//----------------------------------------------------------------------------------------
	GeData GetParameterData(Int32 id, const GeData& data = GeData());

	//----------------------------------------------------------------------------------------
	/// Gets a base draw parameter.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] t_data							Assigned the parameter data.
	/// @return												@trueIfOtherwiseFalse{the parameter was retrieved}
	//----------------------------------------------------------------------------------------
	virtual Bool GetParameter(const DescID& id, GeData& t_data);

	//----------------------------------------------------------------------------------------
	/// Sets a base draw parameter.
	/// @param[in] id									The ID of the parameter.
	/// @param[in] t_data							The parameter data to set.
	/// @return												@trueIfOtherwiseFalse{the parameter was set}
	//----------------------------------------------------------------------------------------
	virtual Bool SetParameter(const DescID& id, const GeData& t_data);

	/// @}

	/// @name Clone/Copy
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the base draw.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trans							An alias translator for the operation. Can be nullptr. @callerOwnsPointed{alias translator}
	/// @return												The cloned base draw. @callerOwnsPointed{base draw}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trans);

	/// @}

	/// @name Editor Camera
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the current scene camera from the passed document.
	/// @since 17.008
	/// @param[in] doc								The document to get the scene camera from. @callerOwnsPointed{document}
	/// @return												The scene camera, or @formatConstant{nullptr} if no scene camera is used.
	//----------------------------------------------------------------------------------------
	BaseObject* GetSceneCamera(const BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets a new scene camera. If @formatParam{op} is @formatConstant{nullptr}, the editor camera is used.
	/// @since 17.008
	/// @param[in] op									The new camera.
	/// @param[in] animate						@markPrivate
	//----------------------------------------------------------------------------------------
	void SetSceneCamera(BaseObject* op, Bool animate = false);

	//----------------------------------------------------------------------------------------
	/// Gets the editor camera.
	/// @return												The editor camera. @cinewareOwnsPointed{camera}
	//----------------------------------------------------------------------------------------
	BaseObject* GetEditorCamera();

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	const BaseDrawPref& GetBaseDrawPrefs();

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

#endif	// C4D_BASEDRAW_H__
