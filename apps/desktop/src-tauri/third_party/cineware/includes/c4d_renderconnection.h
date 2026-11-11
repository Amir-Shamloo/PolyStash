/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_RENDERCONNECTION_H__
#define C4D_RENDERCONNECTION_H__

#include "c4d_network.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDraw;
class RenderData;

/// @addtogroup group_c4d_renderconnection Render Connection
/// @ingroup group_topic Topics
/// @{

/// @addtogroup RENDERCONNECTION
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define RENDERCONNECTION_ID			 'sl4v'
#define RENDERCONNECTION_VERSION 1007
/// @}

/// @addtogroup RENDERCONNECTION_COMMAND
/// @ingroup group_enumeration
/// @{
/// @markPrivate
#define RENDERCONNECTION_COMMAND_SENDSCENE_EX				 10000
#define RENDERCONNECTION_COMMAND_CLOSECONNECTION		 10003
#define RENDERCONNECTION_COMMAND_STARTRENDER				 10004
#define RENDERCONNECTION_COMMAND_FREESESSION				 10006
#define RENDERCONNECTION_COMMAND_SHUTDOWN_EX				 10007
#define RENDERCONNECTION_COMMAND_GETIMAGEUPDATE			 10008
#define RENDERCONNECTION_COMMAND_STOPRENDER					 10009
#define RENDERCONNECTION_COMMAND_GETFULLIMAGE				 10010
#define RENDERCONNECTION_COMMAND_GETRENDERSTATUS		 10011
#define RENDERCONNECTION_COMMAND_SENDTEXTURE_EXEX		 10013
#define RENDERCONNECTION_COMMAND_UPDATEVIEW_EX			 10014
#define RENDERCONNECTION_COMMAND_STREAMSCENE_EX			 10015
#define RENDERCONNECTION_COMMAND_EXPORTSCENE				 10016
#define RENDERCONNECTION_COMMAND_GETALLTEXTURES			 10017
#define RENDERCONNECTION_COMMAND_ISSCENEALIVE				 10018
#define RENDERCONNECTION_COMMAND_UPDATEVIEW					 10019
#define RENDERCONNECTION_COMMAND_USETEXTURECACHE		 10020
#define RENDERCONNECTION_COMMAND_CLIENTCONNECT			 10021
#define RENDERCONNECTION_COMMAND_SHUTDOWN						 10022	///< Reference counting version.
#define RENDERCONNECTION_COMMAND_GETC4DVERSION			 10023
#define RENDERCONNECTION_COMMAND_GETALLASSETS				 10024
#define RENDERCONNECTION_COMMAND_UPDATELAYERS_AFX		 10025
#define RENDERCONNECTION_COMMAND_SENDTEXTURE_EX			 10026
#define RENDERCONNECTION_COMMAND_RESETRENDERSERVER	 10027
#define RENDERCONNECTION_COMMAND_GETINFO						 10028
#define RENDERCONNECTION_COMMAND_GETC4DVERSIONTYPE	 10029	///< Since @C4D R16.SP2 >= #110916
#define RENDERCONNECTION_COMMAND_SENDSCENE					 10030	///< Support for files >2GB. Since @CR R14/@CR R15/@C4D R16 >= #114968
#define RENDERCONNECTION_COMMAND_CONVERTPARAMS			 10031
#define RENDERCONNECTION_COMMAND_BUILDDESCRIPTION_EX 10032
#define RENDERCONNECTION_COMMAND_FLUSHTEXTURE				 10033
#define RENDERCONNECTION_COMMAND_SENDTEXTURE				 10034	///< Support for textures >2GB. Since @CR R15/@C4D R16 >= #122284
#define RENDERCONNECTION_COMMAND_GETERRORSTRING			 10035	///< Since @CR R14/@CR R15/@C4D R16 >= #125125
#define RENDERCONNECTION_COMMAND_SETCURRENTTAKE_AFX	 10036
#define RENDERCONNECTION_COMMAND_SENDTEXTURE_RAW		 10037	///< Since @CR R16/@C4D R17.SP2
#define RENDERCONNECTION_COMMAND_GETPREFSPATH				 10038	///< Since @CR R16.053/@C4D R17.SP3
#define RENDERCONNECTION_COMMAND_BUILDDESCRIPTION		 10039	///< Since @CR R16.053/@C4D R17.SP3
#define RENDERCONNECTION_COMMAND_FRAMESCENE					 10040	///< Since @CR R19.000/@C4D R19.SP1
#define RENDERCONNECTION_COMMAND_SELECTOBJECT				 10041	///< Since @CR R19.000/@C4D R19.SP1
#define RENDERCONNECTION_COMMAND_RENDERMATERIALS		 10042	///< Since @CR R20.018/@C4D R20.SP1
#define RENDERCONNECTION_COMMAND_OPENLICENSEMANAGER	 10043	///< Since @CW R22/@C4D R22.1
#define RENDERCONNECTION_COMMAND_SIGNOUT						 10044	///< Since @CW R22/@C4D R22.1
#define RENDERCONNECTION_COMMAND_FINDPLUGIN					 10045	///< Since @CW R22/@C4D R22.1

#define RENDERCONNECTION_COMMAND_GETFULLIMAGE_VPBUFFER	 10100
#define RENDERCONNECTION_COMMAND_GETIMAGEUPDATE_VPBUFFER 10101
/// @}


/// @addtogroup RENDERERROR
/// @ingroup group_enumeration
/// @{
/// Render error codes.
#define RENDERERROR_OK								0			///< No error.
#define RENDERERROR_NOMEM							1			///< Out of memory situation.
#define RENDERERROR_TEXMISSING				2			///< At least one of the required textures was missing.
#define RENDERERROR_MP								4			///< MP threads could not be created (OS routines to create threads/signals failed).
#define RENDERERROR_IMAGE							5			///< Image could not be saved.
#define RENDERERROR_USERBREAK					6			///< User/host stopped calculation.
#define RENDERERROR_GICACHE						7			///< GI cannot save GI cache.
#define RENDERERROR_CONNECTIONFAILED	8			///< TCP/IP connection interrupted, protocol problem.
#define RENDERERROR_PROJECTNOTFOUND		1000	///< Project was not found.
#define RENDERERROR_LOADINGPROJECT		1001	///< There was an error while loading the project.
#define RENDERERROR_NOOUTPUTSPECIFIED	1002	///< Output was not specified.
/// @}

/// @addtogroup PROGRESSTYPE
/// @ingroup group_enumeration
/// @{
/// Render progress type.
#define PROGRESSTYPE_BEFORERENDERING		0	///< Before rendering.
#define PROGRESSTYPE_DURINGRENDERING		1	///< During rendering.
#define PROGRESSTYPE_AFTERRENDERING			2	///< After rendering.
#define PROGRESSTYPE_GLOBALILLUMINATION	3	///< GI prepass.
#define PROGRESSTYPE_QUICK_PREVIEW			4	///< Quick Preview rendering.
#define PROGRESSTYPE_AMBIENTOCCLUSION		5	///< AO prepass (Supported since @CR 17.054 / @C4D R18.021). @since 17.008
/// @}

class BaseDocument;

/// @addtogroup IPCOMMANDRESULT
/// @ingroup group_enumeration
/// @{
/// @markPrivate Used internally by BaseCommunicationThread.
enum IPCOMMANDRESULT
{
	IPCOMMANDRESULT_OK,
	IPCOMMANDRESULT_NOT_SUPPORTED,
	IPCOMMANDRESULT_ERROR
} ENUM_END_LIST(IPCOMMANDRESULT);
/// @}

/// @addtogroup SELECTION
/// @ingroup group_enumeration
/// @{
#define SELECTION_NEW 0			///< Starts a new selection.
#define SELECTION_ADD 1			///< Adds to the current selection.
#define SELECTION_SUB 2			///< Subtracts from the current selection.
/// @}

//----------------------------------------------------------------------------------------
/// @markPrivate Communication class. Not meant to be used directly.
//----------------------------------------------------------------------------------------
class BaseCommunicationThread : public BaseThread
{
protected:
	IpConnection*	ipc;

public:
	BaseCommunicationThread();

	IpConnection* GetIpc()
	{
		return ipc;
	}

protected:
	Bool ReadBytes(void* adr, Int cnt);
	Bool WriteBytes(void* adr, Int cnt);

	Bool ReadString(String* v);
	Bool WriteString(const String& v);

	Bool ReadContainer(BaseContainer& bc);
	Bool WriteContainer(const BaseContainer& bc);

	Bool WriteChar(Char num);
	Bool WriteUChar(UChar num);
	Bool WriteInt32(Int32 num);
	Bool WriteInt64(Int64 num);
	Bool WriteFloat32(Float32 num);

	Bool ReadUChar(UChar& num);
	Bool ReadInt16(Int16& num);
	Bool ReadChar(Char& num);
	Bool ReadInt32(Int32& num);
	Bool ReadInt64(Int64& num);
	Bool ReadFloat32(Float32& num);
	IPCOMMANDRESULT SendCommand(Int32 command, Bool closeConnectionOnError = true);
	Bool Acknowledged();

	virtual const Char* GetThreadName();

	Bool SetError();
};


/// @addtogroup ColorResolution
/// @ingroup group_enumeration
/// @{
enum ColorResolution
{
	COLORRESOLUTION_CHAR	= 1,	///< 8 bit.
	COLORRESOLUTION_WORD	= 2,	///< 16 bit.
	COLORRESOLUTION_FLOAT	= 4		///< 32 bit.
};
/// @}

/// @addtogroup FILEFORMAT
/// @ingroup group_enumeration
/// @{
enum FILEFORMAT
{
	FILEFORMAT_FBX			 = 1026370,	///< FBX
	FILEFORMAT_COLLADA15 = 1025755,	///< COLLADA 1.5
	FILEFORMAT_COLLADA14 = 1022316,	///< COLLADA 1.4
	FILEFORMAT_CINEMA_4D = 1001026	///< @C4D
} ENUM_END_LIST(FILEFORMAT);
/// @}

/// @addtogroup OVERVIEW
/// @ingroup group_enumeration
/// @{
/// @since R19
enum OVERVIEW
{
	OVERVIEW_DEFAULT	= 0,	///< Default view.
	OVERVIEW_ACTIVE		= 1,	///< Selected elements.
	OVERVIEW_SCENE		= 2,	///< Geometry.
	OVERVIEW_COMPLETE	= 3,	///< All.
	OVERVIEW_SELECTED	= 4		///< Selected objects.
} ENUM_END_LIST(OVERVIEW);
/// @}

//----------------------------------------------------------------------------------------
/// Update image callback.
/// @see	IpCommunicationThread::GetImageUpdate()\n
///				IpCommunicationThread::GetFullImage() IpCommunicationThread::GetFullImageMP()
/// @param[in] userdata						A pointer to a local structure that contains for example the image buffer address. Can be @formatConstant{nullptr}. @callerOwnsPointed{data}
/// @param[in] xpos								Starting X-position of the RGBA data.
/// @param[in] ypos								Starting Y-position of the RGBA data.
/// @param[in] xcnt								The number of pixels of the image buffer.
/// @param[in] bpp								The color depth of the RGBA image.
/// @param[in] rgba_data					Contains the RGBA image data (4 bytes per pixel).
//----------------------------------------------------------------------------------------
typedef void UpdateImage (void* userdata, Int32 xpos, Int32 ypos, Int32 xcnt, ColorResolution bpp, UChar* rgba_data);


//----------------------------------------------------------------------------------------
/// Communication thread to establish a connection to a render server and to control the rendering process.
//----------------------------------------------------------------------------------------
class IpCommunicationThread : public BaseCommunicationThread
{
public:
	/// @name Open/Close
	/// @{

	//----------------------------------------------------------------------------------------
	/// Opens a connection to the render server at @formatParam{ip_address}.
	/// @param[in] ip_address					IP address of the render server. The default IP (@em 127.0.0.1:1234) is used if @formatConstant{nullptr} is passed.
	/// @param[in] application_id			A unique ID provided by the external application. Provide different (unique) IDs to maintain multiple sessions at the same time.
	/// @param[in] initial_timeout		Timeout in seconds after which the connection attempts shall be stopped (default @em 10 seconds).
	/// @param[in] connection_timeout	Timeout in seconds after which the connection will be closed if no data has been sent since then (default @em 60 seconds).
	/// @param[in] retries						Define how many times the function tries to connect.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool Open(const Char* ip_address, Int32 application_id, Int32 initial_timeout, Int32 connection_timeout, Int32 retries = 50);

	//----------------------------------------------------------------------------------------
	/// Closes the connection to the render server.
	/// @param[in] shutdown						The connection is closed if set to @formatConstant{false} but the session is still valid, so it is possible to open a new connection.\n
	///																In that case rendering will start then immediately, without the need to transfer the scene data again.\n
	///																The render server terminates if set to @formatConstant{true}. All open sessions will be freed.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool Close(Bool shutdown = true);

	/// @}

	/// @name License
	/// @{

	//----------------------------------------------------------------------------------------
	/// Opens the License Manager window.
	//----------------------------------------------------------------------------------------
	Bool OpenLicenseManager();

	//----------------------------------------------------------------------------------------
	/// Sign-Out.
	//----------------------------------------------------------------------------------------
	Bool SignOut();

	/// @}

	/// @name Send File
	/// @{

	//----------------------------------------------------------------------------------------
	/// Transfers a file on disc to the render server/session.
	/// @param[in] filepath						The path to the file to transfer.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool SendScenefile(const Filename& filepath);

	//----------------------------------------------------------------------------------------
	/// Transfers a document from memory to the render server/session.
	/// @param[in] filepath						Filename for the scene.
	/// @param[in] doc								Document to transfer. @callerOwnsPointed{document}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool SendScenefile(const Filename& filepath, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Frames the active camera to the selected objects or elements, geometry only or whole scene.
	/// @since R19
	/// @param[in] mode								Defines what should be zoomed to: @enumerateEnum{OVERVIEW}
	/// @param[out] newPos						New local position of the active camera.
	/// @param[out] newRot						New local rotation of the active camera.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool FrameScene(OVERVIEW mode, Vector& newPos, Vector& newRot);

	//----------------------------------------------------------------------------------------
	/// Modifies the current object multi-selection on server side depending on @formatParam{mode}.
	/// @since R19
	/// @param[in] appid							The unique application ID under which the UID of the object should be searched for.
	/// @param[in] mem								Points to a block of memory containing the UID data. Scene including the object must been sent already.
	/// @param[in] size								The length of @formatParam{mem} block.
	/// @param[in] mode								The selection mode: @enumerateEnum{SELECTION}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool SetActiveObject(Int32 appid, const void* mem, Int32 size, Int32 mode = SELECTION_NEW);

	/// @}

	/// @name Update
	/// @{

	//----------------------------------------------------------------------------------------
	/// Updates an existing scene file on the render server.
	/// @param[in] unique_id_type			Unique ID, should usually be set to @em MAXON_CREATOR_ID unless an own system for unique IDs is used.
	/// @param[in] doc								Document to update. @callerOwnsPointed{document}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateScenefile(Int32 unique_id_type, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Updates the scene or editor camera.
	/// @param[in] pos								New camera position.
	/// @param[in] rot								New camera rotation.
	/// @param[in] focal_length				New focal length.
	/// @param[in] aperture						New camera aperture.
	/// @param[in] zoom								New camera zoom.
	/// @param[in] projection					New camera projection type, for instance @em Pleft or @em Pperspective.
	/// @param[in] bNewCamera					If @trueIfOtherwiseFalse{true}, creates a new camera object.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateView(const Vector& pos, const Vector& rot, Float focal_length, Float aperture, Float zoom, Int32 projection, Bool bNewCamera = false);

	//----------------------------------------------------------------------------------------
	/// Updates an existing scene camera with @formatConstant{op}.
	/// @param[in] op									New camera object. @callerOwnsPointed{camera object}
	/// @param[in] bNewCamera					If @trueIfOtherwiseFalse{true}, creates a new camera object.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateCamera(BaseObject* op, Bool bNewCamera = false);

	//----------------------------------------------------------------------------------------
	/// Sets an existing camera as active scene camera.
	/// @param[in] applicationUID			Application UID under which the cameras UIDs should be searched for.
	/// @param[in] cameraUID					UID of the camera object that should be set as active.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateSetActiveCamera(Int32 applicationUID, GeMarker cameraUID);

	//----------------------------------------------------------------------------------------
	/// Updates the display settings of the given @formatParam{draw} view.
	/// @param[in] draw								View to update. @callerOwnsPointed{view}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateDisplaySettings(BaseDraw* draw);

	//----------------------------------------------------------------------------------------
	/// Updates the render settings with @formatParam{rdat}.
	/// @param[in] rdat								New render settings. @callerOwnsPointed{render data}
	/// @param[in] bKeepVP						Pass @formatConstant{true} to keep Video Post.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateRenderSettings(RenderData* rdat, Bool bKeepVP = false);

	//----------------------------------------------------------------------------------------
	/// Updates the document to the given @formatParam{time}.
	/// @param[in] time								New time.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool UpdateTime(BaseTime time);

	/// @}

	/// @name Texture
	/// @{

	//----------------------------------------------------------------------------------------
	/// Controls if the texture cache should be kept in memory or freed after rendering.
	/// @param[in] bKeepTextures			Pass @formatConstant{true} to keep textures in memory after render is finished, @formatConstant{false} to free them.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool UseTextureCache(Bool bKeepTextures);

	//----------------------------------------------------------------------------------------
	/// Transfers a texture from memory to the render server/session.
	/// @param[in] filepath						Texture path (if only the name is passed the texture will be saved in the preferences folder under tex/).
	/// @param[in] mem								Pointer to the memory block containing the texture bitmap. Can contain any @C4D readable image format.
	/// @param[in] len								Size of the memory block.
	/// @param[in] memoryOnly					If @formatConstant{true} the texture will be saved to memory and not written to disk (requires @C4D version >= <i>15.059</i>).
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool SendTexture(const Filename& filepath, void* mem, Int64 len, Bool memoryOnly);

	//----------------------------------------------------------------------------------------
	/// Transfers a texture from memory to the render server/session.
	/// @param[in] filepath						Texture name - must be unique to this render session.
	/// @param[in] data								Pointer to the memory block containing the data.
	/// @param[in] mode								The format of the data, must adhere to known C4D orders.
	/// @param[in] width							The width in pixels of the texture.
	/// @param[in] height							The height in pixels of the texture.
	/// @param[in] rowBytes						The number of bytes per scanline\n
	///																<b>Note:</b> It is not always the same as width*sizeof(PIXEL) in some applications.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool SendTextureRaw(const Filename& filepath, void* data, const COLORMODE& mode, const Int64& width, const Int64& height, const Int64& rowBytes);

	//----------------------------------------------------------------------------------------
	/// Frees a specific texture on render server/session and deletes the file on disk (requires @C4D build >= <i>#119398</i>).
	/// @param[in] filepath						Texture path (if only the name is passed the texture file will be searched in the preferences folder under tex/).
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool FlushTexture(const Filename& filepath);

	/// @}

	/// @name Start/Stop Render
	/// @{

	//----------------------------------------------------------------------------------------
	/// Starts rendering the open session.
	/// @note	The call returns immediately. It does not wait until rendering is finished.
	/// @param[in] xres								The width of the rendered image in pixels.
	/// @param[in] yres								The height of the rendered image in pixels.
	/// @param[in] colorresolution		Determine the color depth the image that will be streamed\n
	///																<b>Note:</b> It is not enough for an image to be streamed as float data if the render settings are not adjusted to float resolution.
	/// @param[in] allow_saving				Pass @formatConstant{false} to set ::RDATA_GLOBALSAVE to @formatConstant{false} before rendering.
	/// @param[in] high_priority			Pass @formatConstant{true} to start rendering with different priorities (low/high priority).
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool StartRender(Int32 xres, Int32 yres, ColorResolution colorresolution, Bool allow_saving, Bool high_priority);

	//----------------------------------------------------------------------------------------
	/// Stops rendering of the open session.
	/// @note	Does not free the session.
	/// @see FreeSession()
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool StopRender();

	/// @}

	/// @name Image Callback
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets image updates.
	/// @param[in] imgupdate					Callback for image updates.
	/// @param[in] userdata						User data for @formatParam{imgupdate} callback. Can be @formatConstant{nullptr}.
	/// @param[out] is_running				Assigned the render running status. Can be @formatConstant{nullptr} if not needed.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetImageUpdate(UpdateImage imgupdate, void* userdata, Bool* is_running);

	//----------------------------------------------------------------------------------------
	/// Gets multipass image updates.
	/// @param[in] vpbuffid						Selects the multipass image channel ID.
	/// @param[in] vpbuffsubid				Selects the multipass image channel sub ID.
	/// @param[in] imgupdate					Callback for image updates.
	/// @param[in] userdata						User data for @formatParam{imgupdate} callback. Can be @formatConstant{nullptr}.
	/// @param[out] is_running				Assigned the render running status. Can be @formatConstant{nullptr} if not needed.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetImageUpdateMP(Int32 vpbuffid, Int32 vpbuffsubid, UpdateImage imgupdate, void* userdata, Bool* is_running);

	//----------------------------------------------------------------------------------------
	/// Gets full image updates.
	/// @param[in] imgupdate					Callback to get the full image.
	/// @param[in] userdata						User data for @formatParam{imgupdate} callback. Can be @formatConstant{nullptr}.
	/// @param[out] is_running				Assigned the render running status. Can be @formatConstant{nullptr} if not needed.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetFullImage(UpdateImage imgupdate, void* userdata, Bool* is_running);

	//----------------------------------------------------------------------------------------
	/// Gets full multipass image updates.
	/// @param[in] vpbuffid						Selects the multipass image channel ID.
	/// @param[in] vpbuffsubid				Selects the multipass image channel sub ID.
	/// @param[in] imgupdate					Callback to get the full image.
	/// @param[in] userdata						User data for @formatParam{imgupdate} callback. Can be @formatConstant{nullptr}.
	/// @param[out] is_running				Assigned the render running status. Can be @formatConstant{nullptr} if not needed.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetFullImageMP(Int32 vpbuffid, Int32 vpbuffsubid, UpdateImage imgupdate, void* userdata, Bool* is_running);

	/// @}

	/// @name Textures/Assets
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets all the textures used in render.
	/// @param[out] bc								Filled with all textures filenames. @callerOwnsPointed{container}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetAllTextures(BaseContainer* bc);

	//----------------------------------------------------------------------------------------
	/// Gets all the assets used in render.
	/// @param[in] bc									Filled with all assets. @callerOwnsPointed{container}
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetAllAssets(BaseContainer* bc);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Exports the scene into @formatParam{filepath} in the given format.
	/// @param[in] filepath						The path to the file to export.
	/// @param[in] format							The file format.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool ExportScene(const Filename& filepath, FILEFORMAT format);

	//----------------------------------------------------------------------------------------
	/// Frees the session (scene, image etc.).
	/// @note	The session is still open afterwards.\n
	///				As old scenes are automatically freed when new ones are uploaded this is mostly used to quit the external application, but keep the render server open.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool FreeSession();

	//----------------------------------------------------------------------------------------
	/// Frees all sessions (all scenes, images, textures in memory etc., requires Cinema4D version >= 15.059).
	/// @note	All sessions are still open afterwards.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool ResetRenderServer();

	//----------------------------------------------------------------------------------------
	/// Gets the running @C4D version of the render server.
	/// @param[in] version						Assigned the running @C4D version of the render server (for older servers where the command isn't supported this will be set to 1).
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetC4dVersion(Int32* version);

	//----------------------------------------------------------------------------------------
	/// Gets the running @C4D version type of the render server.
	/// @param[in] type								Assigned the running @C4D version type of the render server (for older servers where the command isn't supported this will be set to VERSIONTYPE_UNKNOWN).
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetC4dVersionType(VERSIONTYPE* type);

	//----------------------------------------------------------------------------------------
	/// Gets the path to the preference folder of the render server.
	/// @param[out] prefsPath					Assigned the preference path as String.\n
	///																<b>Note:</b> The string will be empty if the server version is too old and doesn't support the call.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetPreferencePath(String& prefsPath);

	//----------------------------------------------------------------------------------------
	/// Checks if the renderer is running.
	/// @note	Any of the passed parameter can be set to @formatConstant{nullptr} if its information is not needed.
	/// @param[out] is_running				Assigned the render running status.
	/// @param[out] rendererror				Assigned a render error code: @ref RENDERERROR.
	/// @param[out] progress_percent	Assigned the render progress in percentage.
	/// @param[out] progress_type			Assigned the render progress type: @ref PROGRESSTYPE.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetRenderStatus(Bool* is_running, Int32* rendererror, Float* progress_percent, Int32* progress_type);

	//----------------------------------------------------------------------------------------
	/// Gets additional error information after an error has occurred.
	/// @note	The returned String might be empty if no additional data is available, if connected to a legacy server which doesn't support the call or if there was no error at all.
	/// @param[out] errorText					Assigned the render error text as String. Depends on the error which has occurred before.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool GetRenderErrorString(String& errorText);

	//----------------------------------------------------------------------------------------
	/// Checks if a scene file was already sent to the render server.
	/// @param[in] is_alive						Assigned @formatConstant{true} only if a scene was already transfered.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool IsSceneAlive(Bool* is_alive);

	//----------------------------------------------------------------------------------------
	/// Converts legacy parameters of a node into actual parameters of the running @C4D version.
	/// @param[in] node								The node containing the legacy parameter. Types of BaseObject, BaseMaterial, BaseTag and RenderData are supported.
	/// @param[in] version						The C4D version the parameter data was valid for.
	/// @return												A new node with converted parameters. If an error occurs nullptr is returned, connection to the server will NOT be closed automatically.
	//----------------------------------------------------------------------------------------
	BaseList2D* ConvertParameters(BaseList2D* node, CINEMA4D_VERSION version);

	//----------------------------------------------------------------------------------------
	/// Returns the description data of the node related to the running @C4D version.
	/// @param[in] pluginId						The plugin ID of the node.
	/// @param[out] bc								The existing BaseContainer which will be filled with the parameter of the description.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	///																If @formatConstant{false}, connection to the server will NOT be closed automatically.
	//----------------------------------------------------------------------------------------
	Bool BuildDescription(Int32 pluginId, BaseContainer& bc);

	//----------------------------------------------------------------------------------------
	/// Renders the materials previews of all included materials.
	/// @since R20
	/// @param[in] doc								The document containing the materials.
	/// @return												A new document including the materials with updated previews.
	///																The document must be freed by the caller.
	//----------------------------------------------------------------------------------------
	BaseDocument* RenderMaterials(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Checks if the Plug-In with the provided ID is available.
	/// @param[in] pluginId						The plugin ID of the node.
	/// @param[out] found							True if the plugin was found.
	/// @return												@trueIfOtherwiseFalse{successful}\n
	//----------------------------------------------------------------------------------------
	Bool FindPlugin(Int32 pluginId, Bool& found);

	/// @}

	/// @name Private. For AFX only.
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool UpdateLayer(BaseArray<GeMarker>& layerMarkers, Bool enableDefaultLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool UpdateLayerEx(Int32* array, Int32 count, Bool enableDefaultLayer);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool SetCurrentTake(const GeMarker& takeMarker);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	Bool GetInfo(Int32 pluginID, Int32* info);

	/// @}
};


//----------------------------------------------------------------------------------------
/// Checks if a render server at @formatParam{ip_address} is present.
/// @note	Should be called only once before the first render.
/// @param[in] ipAddress					IP address of the render server. The default IP (@em 127.0.0.1:1234) is used if @formatConstant{nullptr} is passed.
/// @param[in] timeOut						Timeout in seconds to open the connection with the render server. Pass @em 0 to use the default behaviour. Since R19.
/// @return												@trueIfOtherwiseFalse{successful}
//----------------------------------------------------------------------------------------
Bool IsServerStarted(const Char* ipAddress, Int32 timeOut = 0);

//----------------------------------------------------------------------------------------
/// Starts the render server.
/// @note	Alternatively call GeExecute(filename,"-listen") to start the render server.
/// @param[in] filename						The path to the render server executable.
/// @param[in] listen_port				Port of the render server.
/// @param[in] forceNewServer			If @formatConstant{false}, the client connects to an already running server process, otherwise it forces a new server process.
/// @param[in] startUpWaitTime		If ::NOTOK the function returns immediately after the process was created, otherwise it waits until the server process is responding. Since R19.
/// @param[in] args								Optional commdand line arguments to be added to the execution or if the given timeout in seconds is reached. Since R19.
/// @return												@trueIfOtherwiseFalse{successful}
//----------------------------------------------------------------------------------------
Bool StartRenderServer(const Filename& filename, Int32 listen_port = 1234, Bool forceNewServer = true, Int32 startUpWaitTime = NOTOK, const String& args = String());

/// @}

#pragma pack (pop)
}

#endif	// C4D_RENDERCONNECTION_H__
