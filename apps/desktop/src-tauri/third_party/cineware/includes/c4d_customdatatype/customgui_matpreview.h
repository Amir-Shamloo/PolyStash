/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef CUSTOMGUI_MATPREVIEW_H__
#define CUSTOMGUI_MATPREVIEW_H__

#include "c4d_customdatatype.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseShader;
class BaseMaterial;
class BaseBitmap;
class String;
class GeData;
class BaseContainer;
class AliasTrans;
class HyperFile;

#define MATERIAL_PREVIEW 520000000

/// %Material preview custom GUI ID.
#define CUSTOMGUI_MATPREVIEW 1011152

/// %Material preview custom data type ID.
#define CUSTOMDATATYPE_MATPREVIEW	1011153

/// @addtogroup MATPREVIEW_SETTINGS
/// @ingroup group_customguisettings
/// @{
#define MATPREVIEW_MIN_WIDTH				1000	///< Int32 Minimum width.
#define MATPREVIEW_MIN_HEIGHT				1001	///< Int32 Minimum height.
#define MATPREVIEW_NO_SIZE_POPUP		1002	///< Bool No size popup.
#define MATPREVIEW_NO_BORDER				1003	///< Bool No border.
#define MATPREVIEW_NO_ANIMATION			1004	///< Bool No animation.
#define MATPREVIEW_NO_FLOATWINDOW		1005	///< Bool No floating window.
#define MATPREVIEW_NO_SCENESETTINGS	1006	///< Bool No scene settings.
#define MATPREVIEW_RECTSELECT				1007	///< Bool Allow rectangle selection by the user, accessed with %MaterialPreviewData::GetRegion/%MaterialPreviewData::SetRegion.\n
																					///< Only used if a callback function is used to render the preview.
/// @}

/// @addtogroup MATPREVIEW
/// @ingroup group_enumeration
/// @{
/// Overriding NodeData::Message it is not recommended to check for a @formatConstant{nullptr} pointer in @c node->GetDocument() if it is a @ref MATPREVIEW message.\n
/// For those messages the document might be @formatConstant{nullptr} in some cases, but the method should not be aborted.
#define MATPREVIEW_GET_OBJECT_INFO		1011147	///< Sent by the material preview to get object information from the host. The corresponding @em data is MatPreviewObjectInfo.
#define MATPREVIEW_GENERATE_IMAGE			1011148	///< Sent by the material preview when the host object is responsible for the rendering (MatPreviewObjectInfo::bHandlePreview is @formatConstant{true}). The corresponding @em data is MatPreviewGenerateImage.
#define MATPREVIEW_MODIFY_CACHE_SCENE	1011149	///< Sent by the material preview to let the host modify the preview scene. A pointer to a scene inside the preview scene cache is sent. The corresponding @em data is MatPreviewModifyCacheScene.
#define MATPREVIEW_PREPARE_SCENE			1011142	///< Sent each time the preview wants to render the host scene. The corresponding @em data is MatPreviewPrepareScene.
#define MATPREVIEW_GET_PREVIEW_ID			1011143	///< The corresponding @em data is Int32. Set it to the ID of the material preview.
#define MATPREVIEW_GET_POPUP_OPTIONS	1011141	///< Let add extra material preview popup menu items to a custom sub-menu.\n
																							///< The corresponding @em data is a BaseContainer which represents the menu. Insert the menu items. Set the name of the sub-menu with @ref MATPREVIEW_POPUP_NAME .\n
																							///< @b Example:
																							/// @code
																							/// case MATPREVIEW_GET_POPUP_OPTIONS:
																							/// {
																							///   BaseContainer* bc = static_cast<BaseContainer*>(data);
																							///
																							///   // Element with index MATPREVIEW_POPUP_NAME is the text of the popup item.
																							///   bc->SetString(MATPREVIEW_POPUP_NAME, GeLoadString(IDS_PARTICLEVOLUME));
																							///
																							///   // A menu item. (Container entries should start with 1.)
																							///   bc->SetString(1, GeLoadString(IDS_PARTICLEVOLUME_HIGH_DENSITY) + (highDensity ? "&c&" : ""));
																							///
																							///   // Add an example separator.
																							///   bc->SetString(0, "");
																							///
																							///   // Yet another menu item.
																							///   bc->SetString(2, GeLoadString(IDS_PARTICLEVOLUME_LOW_DENSITY) + (!highDensity ? "&c&" : ""));
																							///   return true;
																							/// }
																							/// break;
																							/// @endcode
#define MATPREVIEW_HANDLE_POPUP_MSG	1011144		///< Sent when the user has selected any of the options in the material preview popup. The corresponding data is an Int32 which is the menu item ID.
#define MATPREVIEW_FREE_USERDATA		450000029	///< Sent to free the user data. (Only sent when the preview is initialized with a callback.)
#define MATPREVIEW_COPY_USERDATA		450000030	///< Sent to copy the user data. The corresponding @em data is a MatPreviewCopyUserData. Set MatPreviewCopyUserData::dst to a clone of MatPreviewCopyUserData::src. (Only sent when the preview is initialized with a callback.)
#define MATPREVIEW_DRAGNDROP_RECV		450000062	///< Preview has received drag and drop. The corresponding @em data is MatPreviewHandleDragnDrop.
#define MATPREVIEW_DRAGNDROP_START	450000063	///< Start drag and drop (fill in the MatPreviewHandleDragnDrop structure). The corresponding @em data is MatPreviewHandleDragnDrop.
#define MATPREVIEW_DRAGNDROP_END		450000064	///< Drag and drop finished, free @em data. The corresponding @em data is MatPreviewHandleDragnDrop.
#define MATPREVIEW_GET_DIRTY_COUNT	450000065	///< Let return a dirty count for the preview.

#define MATPREVIEW_POPUP_NAME	99999						///< String Popup name.

#define MATPREVIEW_NULL_SIZE_NAME	"null-size"	///<
/// @}

/// @addtogroup MATPREVIEW_FLAG
/// @ingroup group_enumeration
/// @{
#define MATPREVIEW_FLAG_HIDE_ROTATION				1		///< Hide rotation.
#define MATPREVIEW_FLAG_HIDE_SCENES					2		///< Hide scenes.
#define MATPREVIEW_FLAG_HIDE_ANIMATE				4		///< Hide animation.
#define MATPREVIEW_FLAG_HIDE_SIZE						8		///< Hide size.
#define MATPREVIEW_FLAG_HIDE_OPEN						16	///< Hide open.
#define MATPREVIEW_FLAG_HIDE_SCENE_SETTINGS	32	///< Hide scene settings.
#define MATPREVIEW_FLAG_ALLOW_DRAGNDROP			64	///< Allow drag and drop.
/// @}

class BaseChannel;
class BaseMaterial;

/// @addtogroup MatPreviewType
/// @ingroup group_enumeration
/// @{
enum MatPreviewType
{
	MatPreviewDefault = 999,						///< Default.
	MatPreviewFlat2D = 1000,						///< Flat 2D
	MatPreviewCube,											///< Cube.
	MatPreviewPlane,										///< Plane.
	MatPreviewCylinder,									///< Cylinder.
	MatPreviewTorus,										///< Torus.
	MatPreviewPlaneBack,								///< Plane Backlight.
	MatPreviewSphere,										///< Sphere.
	MatPreview2Torus,										///< Double Torus.
	MatPreviewRoundedCube,							///< Rounded Cube.
	MatPreviewKnot,											///< Knot.
	MatPreviewMultipleObjects,					///< Multiple Objects. @since R17.032
	MatPreviewUser = 1050								///< %User.
};
/// @}

/// @addtogroup MatPreviewSize
/// @ingroup group_enumeration
/// @{
enum MatPreviewSize
{
	MatPreviewSizeDefault = 2000,			///< Use system default.
	MatPreviewSizeSmall,							///< Small.
	MatPreviewSizeMedium,							///< Medium.
	MatPreviewSizeBig,								///< Big.
	MatPreviewSizeLarge								///< Large.
};
/// @}

//----------------------------------------------------------------------------------------
/// %Material preview data type (@ref CUSTOMDATATYPE_MATPREVIEW) for MaterialPreviewCustomGui.
//----------------------------------------------------------------------------------------
class MaterialPreviewData : public CustomDataType
{
private:
	MaterialPreviewData();
	~MaterialPreviewData();

public:
	/// @name Init
	/// @{

	//----------------------------------------------------------------------------------------
	/// Initializes the material preview with @formatParam{pChannel}.
	/// @param[in] pChannel						The initial preview source. @callerOwnsPointed{channel}
	/// @param[in] lCount							The dirty count. Used to detect changes.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Init(BaseChannel* pChannel, Int32 lCount);

	//----------------------------------------------------------------------------------------
	/// Initializes the material preview with @formatParam{pShader}.
	/// @param[in] pShader						The initial preview source. @callerOwnsPointed{shader}
	/// @param[in] lCount							The dirty count. Used to detect changes.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Init(BaseShader* pShader, Int32 lCount);

	//----------------------------------------------------------------------------------------
	/// Initializes the material preview with @formatParam{pMaterial}.
	/// @param[in] pMaterial					The initial preview source. @callerOwnsPointed{material}
	/// @param[in] lCount							The dirty count. Used to detect changes.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Init(BaseMaterial* pMaterial, Int32 lCount);

	//----------------------------------------------------------------------------------------
	/// Initializes the material preview with @formatParam{pImage}.
	/// @param[in] pImage							The initial preview source. @callerOwnsPointed{bitmap}
	/// @param[in] lCount							The dirty count. Used to detect changes.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool Init(BaseBitmap* pImage, Int32 lCount);

	/// @cond IGNORE
	//----------------------------------------------------------------------------------------
	/// Initializes the material preview with a custom image hook. The hook will handle all host messages.
	/// @param[in] fn									The callback for the preview.
	/// @param[in] pUserData					The user data. Passed along to the hook. May be deleted after the function has finished.
	/// @param[in] lUserID						A user ID for the material preview. Used to identify the callback.
	/// @param[in] lCount							The dirty count. Increment this whenever a parameter has been changed.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	//Bool Init(RenderPreviewImageCallback fn, void* pUserData, Int32 lUserID, Int32 lCount);
	/// @endcond

	/// @}

	//----------------------------------------------------------------------------------------
	/// Sets the preview animation state.
	/// @param[in] bAnimate						The new animation state.
	//----------------------------------------------------------------------------------------
	void AnimatePreview(Bool bAnimate);

	//----------------------------------------------------------------------------------------
	/// Checks the preview animation state.
	/// @return												The preview animation state.
	//----------------------------------------------------------------------------------------
	Bool PreviewIsAnimated();

	//----------------------------------------------------------------------------------------
	/// Sets the preview type.
	/// @param[in] t									The new preview type: @enumerateEnum{MatPreviewType}
	//----------------------------------------------------------------------------------------
	void SetPreviewType(MatPreviewType t);

	//----------------------------------------------------------------------------------------
	/// Checks the preview type.
	/// @return												The preview type: @enumerateEnum{MatPreviewType}
	//----------------------------------------------------------------------------------------
	MatPreviewType GetPreviewType();

	//----------------------------------------------------------------------------------------
	/// Sets the user preview scene name.
	/// @param[in] strName						The new user preview scene name.
	//----------------------------------------------------------------------------------------
	void SetUserPreviewSceneName(const String& strName);

	//----------------------------------------------------------------------------------------
	/// Retrieves the user preview scene name.
	/// @return												The user preview scene name.
	//----------------------------------------------------------------------------------------
	String GetUserPreviewSceneName();

	//----------------------------------------------------------------------------------------
	/// Sets the preview size.
	/// @param[in] s									The new preview size: @enumerateEnum{MatPreviewSize}
	//----------------------------------------------------------------------------------------
	void SetPreviewSize(MatPreviewSize s);

	//----------------------------------------------------------------------------------------
	/// Retrieves the preview size.
	/// @return												The preview size.
	//----------------------------------------------------------------------------------------
	MatPreviewSize GetPreviewSize();

	//----------------------------------------------------------------------------------------
	/// Sets the object matrix.
	/// @param[in] m									The new object matrix.
	//----------------------------------------------------------------------------------------
	void SetObjectMatrix(const Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Retrieves the object matrix.
	/// @param[out] m									Assigned the object matrix, if not cleared.
	/// @return												@formatConstant{true} if successful, @formatConstant{false} if the object matrix is not initialized.
	//----------------------------------------------------------------------------------------
	Bool GetObjectMatrix(Matrix& m);

	//----------------------------------------------------------------------------------------
	/// Clears the object matrix, i.e to an uninitialized state.
	//----------------------------------------------------------------------------------------
	void ClearObjectMatrix();

	//----------------------------------------------------------------------------------------
	/// Retrieves the world size.
	/// @return												The world size.
	//----------------------------------------------------------------------------------------
	Float GetWorldSize();

	//----------------------------------------------------------------------------------------
	/// Sets the world size.
	/// @param[in] r									The new world size.
	//----------------------------------------------------------------------------------------
	void SetWorldSize(Float r);

	//----------------------------------------------------------------------------------------
	/// Retrieves the preview start time.
	/// @return												The preview start time.
	//----------------------------------------------------------------------------------------
	Float GetPreviewStartTime();

	//----------------------------------------------------------------------------------------
	/// Sets the preview start time.
	/// @param[in] r									The new start time.
	//----------------------------------------------------------------------------------------
	void SetPreviewStartTime(Float r);

	//----------------------------------------------------------------------------------------
	/// Gets the environment data.
	/// @param[out] col								Assigned the environment color.
	/// @param[out] strength					Assigned the environment strength.
	//----------------------------------------------------------------------------------------
	void GetEnvironment(Vector& col, Float& strength);

	//----------------------------------------------------------------------------------------
	/// Sets the environment data.
	/// @param[in] col								The new environment color.
	/// @param[in] strength						The new environment strength.
	//----------------------------------------------------------------------------------------
	void SetEnvironment(const Vector& col, const Float& strength);

	//----------------------------------------------------------------------------------------
	/// Retrieves the dirty count for the preview.
	/// @return												The dirty count.
	//----------------------------------------------------------------------------------------
	Int32 GetDirtyCount();

	//----------------------------------------------------------------------------------------
	/// Retrieves the current selection region.
	/// @param[out] l									Assigned the left boundary, between @em 0 and @em 1.
	/// @param[out] t									Assigned the top boundary, between @em 0 and @em 1.
	/// @param[out] r									Assigned the right boundary, between @em 0 and @em 1.
	/// @param[out] b									Assigned the bottom boundary, between @em 0 and @em 1.
	/// @return												@trueIfOtherwiseFalse{the region could be retrieved}
	//----------------------------------------------------------------------------------------
	Bool GetRegion(Float& l, Float& t, Float& r, Float& b);

	//----------------------------------------------------------------------------------------
	/// Sets a selection region in the preview.
	/// @param[in] bActive						Determines if the rectangle is active or not.
	/// @param[in] l									The new left boundary, between @em 0 and @em 1.
	/// @param[in] t									The new top boundary, between @em 0 and @em 1.
	/// @param[in] r									The new right boundary, between @em 0 and @em 1.
	/// @param[in] b									The new bottom boundary, between @em 0 and @em 1.
	//----------------------------------------------------------------------------------------
	void SetRegion(Bool bActive, Float l, Float t, Float r, Float b);

	//----------------------------------------------------------------------------------------
	/// Retrieves the channel used for preview.
	/// @return												The channel: @enumerateEnum{CHANNEL}
	//----------------------------------------------------------------------------------------
	Int32 GetPreviewChannel();

	//----------------------------------------------------------------------------------------
	/// Sets the channel used for preview.
	/// @param[in] lChannel						The new channel: @enumerateEnum{CHANNEL}
	//----------------------------------------------------------------------------------------
	void SetPreviewChannel(Int32 lChannel);

	//----------------------------------------------------------------------------------------
	/// Retrieves the reflection layer used for preview.
	/// @return												The reflection layer ID.
	//----------------------------------------------------------------------------------------
	Int32 GetReflectionLayer();

	//----------------------------------------------------------------------------------------
	/// Sets the reflection layer used for preview.
	/// @param[in] lLayer							The new reflection layer ID.
	//----------------------------------------------------------------------------------------
	void SetReflectionLayer(Int32 lLayer);
};

//-------------------------------------------------------------------------------------------------
/// @markPrivate
//-------------------------------------------------------------------------------------------------
Bool SetDParameterPreview(BaseContainer* pContainer, const GeData* pData, Int32 lID);

///////////////////////////////////////////////////////////////////////////////////////////////////

/// @markInternal
class MatPreviewDataTypeClass
{
public:
	static Int32 GetId();

	static CustomDataType*  AllocData();
	static void FreeData(CustomDataType* data);

	static Bool CopyData(const CustomDataType* src, CustomDataType* dest, AliasTrans* aliastrans);
	static Int32 Compare(const CustomDataType* d1, const CustomDataType* d2);

	static Bool WriteData(const CustomDataType* d, HyperFile* hf);
	static Bool ReadData (CustomDataType* d, HyperFile* hf, Int32 level);
};

#pragma pack (pop)
}

#endif	// CUSTOMGUI_MATPREVIEW_H__
