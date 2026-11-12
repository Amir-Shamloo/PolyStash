/////////////////////////////////////////////////////////////
// Render Connection                                       //
/////////////////////////////////////////////////////////////
// (c) 1989-2009 MAXON Computer GmbH, all rights reserved  //
/////////////////////////////////////////////////////////////
// this basic code demonstrates how to use the TCP/IP      //
// render connection to CINEMA 4D                          //
/////////////////////////////////////////////////////////////

#include <stdio.h>
#include "c4d_renderconnection.h"
#include "c4d.h"

#define BITMAPTESTPATH "maxon.bmp"

using namespace cineware;

// this is a unique ID of your application registered at www.plugincafe.com
#define UNIQUE_APPLICATION_ID	300001067

#define DEFAULT_IP "127.0.0.1:1234"

// forward declarations for image display and messages
void WinImageUpdate(void *userdata, Int32 xpos, Int32 ypos, Int32 xcnt, ColorResolution bpp, UChar *rgba_data);
void RedrawWindow();
void ClearBitmap();
void ShowMessage(Int32 id);
void ChangeTitle(const String &title);
void CallCommand(Int32 type, const Filename &filename);
void FreeRenderhost();

UChar memorytexture[]=
{
	0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0xB2, 0x49, 0x4C, 0x42, 0x4D, 0x42, 0x4D, 0x48, 0x44, 0x00, 0x00, 0x00, 0x14, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x80,
	0x00, 0x00, 0x16, 0x1A, 0x02, 0xD4, 0x01, 0x90, 0x43, 0x4D, 0x41, 0x50, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xD9, 0x63, 0xDF, 0x00, 0x00, 0xB9, 0x00, 0xFF, 0x00, 0xEE,
	0xBB, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xCC, 0xCC, 0x47, 0x52, 0x41, 0x42, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x01, 0x43, 0x52, 0x4E, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00, 0x43, 0x52, 0x4E, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x43, 0x52, 0x4E, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00, 0x43, 0x52, 0x4E, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x43, 0x41, 0x4D, 0x47, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x90, 0x04,
	0x42, 0x4F, 0x44, 0x59, 0x00, 0x00, 0x00, 0x12, 0xA0, 0x00, 0xE0, 0x00, 0xA0, 0x00, 0x60, 0x00, 0x80, 0x00, 0xA0, 0x00, 0x80, 0x00, 0xC0, 0x00, 0x20, 0x00
};

Random rnd;

class SketchStyles
{
private:
	BaseDocument style_doc;

public:
	SketchStyles() {}

	Bool Init(const Filename &style_path)
	{
		HyperFile C4Dfile;
		if (!C4Dfile.Open(DOC_IDENT, style_path, FILEOPEN_READ))
			return false;
		if (!style_doc.ReadObject(&C4Dfile, true))
			return false;
		C4Dfile.Close();

		return true;
	}

	String GetStyleName()
	{
		BaseMaterial *mat;
		for (mat = style_doc.GetFirstMaterial(); mat; mat = mat->GetNext())
		{
			if (mat->GetType() == Msketch)
				return mat->GetName();
		}

		return String();
	}

	// adjusts for the document 'doc' the sketch style set with init()
	// (clones sketch material and sketch videopost effect, and copies sketch tag to all root objects)
	// if unsuccessful nullptr will be returned
	// if successful, the sketch material will be returned
	BaseMaterial *AdjustStyle(BaseDocument *doc)
	{
		BaseObject *op, *nop;
		BaseTag *orig_tag = nullptr;

		// find first object with sketch tag
		for (op = style_doc.GetFirstObject(); op; op = op->GetNext())
		{
			orig_tag = op->GetFirstTag();
			while (orig_tag && orig_tag->GetType() != Tsketch)
				orig_tag = orig_tag->GetNext();

			if (orig_tag)
				break;
		}
		if (!orig_tag)
			return nullptr; // style document incorrect

		AliasTrans trn;
		if (!trn.Init())
			return nullptr; // out of memory

		GeData dat;
		if (!orig_tag->GetParameter(OUTLINEMAT_LINE_DEFAULT_MAT_V, dat))
			return nullptr; // style document incorrect

		BaseList2D *orig_mat = dat.GetLink();
		if (!orig_mat || orig_mat->GetType() != Msketch)
			return nullptr; // style document incorrect

		BaseMaterial *sketch_mat = (BaseMaterial*)orig_mat->GetClone(COPYFLAGS_0, &trn);
		if (!sketch_mat)
			return nullptr; // out of memory

		doc->InsertMaterial(sketch_mat);

		// apply a sketch tag with cloned sketch mat to all root objects
		for (nop = doc->GetFirstObject(); nop; nop = nop->GetNext())
		{
			BaseTag *sketch_tag = nop->MakeTag(Tsketch);
			if (!sketch_tag)
				return nullptr; // out of memory
			if (!orig_tag->CopyTo(sketch_tag, COPYFLAGS_0, &trn))
				return nullptr; // out of memory
		}

		// get render data and copy sketch video post effect
		RenderData *orig_rdat = style_doc.GetActiveRenderData();
		if (!orig_rdat)
			return nullptr; // style document incorrect

		RenderData *rdat = doc->GetActiveRenderData();
		if (!rdat)
			return nullptr; // should never happen

		BaseVideoPost *sketch_vp = rdat->GetFirstVideoPost(), *orig_vp = orig_rdat->GetFirstVideoPost();

		while (sketch_vp && sketch_vp->GetType() != VPsketch)
			sketch_vp = sketch_vp->GetNext();
		if (sketch_vp)
			DeleteObj(sketch_vp); // delete any existing sketch videopost

		while (orig_vp && orig_vp->GetType() != VPsketch) // find the videopost effect from the style document
			orig_vp = orig_vp->GetNext();

		if (!orig_vp)
			return nullptr; // style document incorrect

		sketch_vp = (BaseVideoPost*)orig_vp->GetClone(COPYFLAGS_0, &trn);
		if (!sketch_vp)
			return nullptr; // out of memory
		rdat->InsertVideoPostLast(sketch_vp);

		trn.Zap(false); // translate all scene links

		return sketch_mat;
	}
};

// create an example scene in memory
static Bool CreateSceneInMemory(BaseDocument *doc, GeMarker &stamp)
{
	/*
		Int32 cnt;
		for (cnt = 0; cnt < 1000; cnt++)
		{
			cineware::PolygonObjectData *newData = (cineware::PolygonObjectData*)doc->AllocObject(Opolygon);
			if (!newData) return FALSE;

			cineware::PluginObject * newObject = newData->GetNode();

		#define XRES 100
		#define YRES 100

			newData->ResizeObject (XRES*YRES, (XRES-1)*(YRES-1), NOTOK);

			cineware::Vector *padr = newData->GetPoint();
			cineware::CPolygon *vadr = newData->GetPolygon();
			if (!vadr || !padr)
				return false;

			Int32 x, y, off, ind;
			for (x = 0; x < XRES; x++)
			{
				for (y = 0; y < YRES; y++)
				{
					padr[x*YRES+y] = Vector(x-XRES/2, y-YRES/2, 0.0);
				}
			}

			for (x = 0; x < XRES-1; x++)
			{
				for (y = 0; y < YRES-1; y++)
				{
					ind = x*(YRES-1)+y;
					off = x*(YRES-1);
					vadr[ind].a = off+y;
					vadr[ind].b = off+y+1;
					vadr[ind].c = off+YRES-1+y+1;
					vadr[ind].d = off+YRES-1+y;
				}
			}

			// add to XScene
			doc->InsertObject (newObject, nullptr);
		}
	*/

	Material *mat = NewObj(Material);
	if (!mat)
		return false;

	doc->InsertMaterial(mat);
	mat->SetParameter(MATERIAL_COLOR_COLOR, cineware::Vector(1.0, 1.0, 1.0));
	mat->SetParameter(MATERIAL_COLOR_BRIGHTNESS, 1.0);
	mat->SetChannelState(CHANNEL_COLOR, true);

	mat->MakeBitmapShader(MATERIAL_COLOR_SHADER, Filename(BITMAPTESTPATH));

	BaseShader *shd = mat->GetFirstShader();
	if (!shd)
		return false;

	shd->SetParameter(BITMAPSHADER_INTERPOLATION, BITMAPSHADER_INTERPOLATION_CIRCLE);

	/*
		// turn on luminance channel and create a shader effect "Xflame" for it
		mat->SetChannelState(CHANNEL_LUMINANCE, true);
		BaseShader *shFlame = BaseShader::Alloc(Xflame);
		if (!shFlame) return FALSE;
		mat->InsertShaderLast(shFlame);
		BaseLink *myLink = new BaseLink;
		myLink->SetLink(shFlame);
		mat->SetParameter(MATERIAL_LUMINANCE_SHADER, *myLink);
	*/
	BaseLink link;
	link.SetLink(mat);

	const Char* marker_bytes = nullptr;
	Int32	i;
	Int marker_size;
	for (i = 0; i < 50; i++)
	{
		// create a cube object
		BaseObject *pNewObject = BaseObject::Alloc(Ocube);
		if (!pNewObject)
			return false;

		// create a phong tag for the cube object
		BaseTag *tag = pNewObject->MakeTag(Tphong);
		if (!tag)
			return false;

		tag = pNewObject->MakeTag(Ttexture);
		if (!tag)
			return false;

		tag->SetParameter(TEXTURETAG_PROJECTION, TEXTURETAG_PROJECTION_SPHERICAL);
		tag->SetParameter(TEXTURETAG_TILE, true);
		tag->SetParameter(TEXTURETAG_LENGTHX, 0.2);
		tag->SetParameter(TEXTURETAG_LENGTHY, 0.2);
		tag->SetParameter(TEXTURETAG_MATERIAL, link);

		// set some data and properties of the cube
		pNewObject->SetParameter(PRIM_CUBE_DOFILLET, true);
		pNewObject->SetParameter(PRIM_CUBE_LEN, cineware::Vector(rnd.Get01()*50.0+11.0, rnd.Get01()*50.0+11.0, rnd.Get01()*50.0+11.0));
		pNewObject->SetParameter(PRIM_CUBE_FRAD, 10.0);
		pNewObject->SetParameter(ID_BASEOBJECT_REL_POSITION, cineware::Vector((rnd.Get01()-0.5)*300.0, (rnd.Get01()-0.5)*300.0, (rnd.Get01()-0.5)*300.0));

		//pNewObject->GetNodeData()->SetParameter( ID_BASEOBJECT_COLOR, cineware::Vector(rnd.Get(),rnd.Get(),rnd.Get()));
		//pNewObject->GetNodeData()->SetParameter( ID_BASEOBJECT_USECOLOR, ID_BASEOBJECT_USECOLOR_ALWAYS);

		// insert object into scene
		doc->InsertObject(pNewObject, nullptr);

		//break;
	}

	// saver marker (or unique ID) of first element in scene
	//	BaseObject *op = doc->GetFirstObject();
	BaseMaterial *op = doc->GetFirstMaterial();
	if (op)
	{
		if (!op->FindUniqueID(MAXON_CREATOR_ID, marker_bytes, marker_size) || !marker_bytes)
			return false;
		CopyMem(marker_bytes, &stamp, LMin(sizeof(stamp), (Int32)marker_size));
	}

	// create render data information
	RenderData *rdat = doc->GetActiveRenderData(); //NewObj(RenderData;
	if (!rdat)
		return false;

	doc->SetParameter(DOCUMENT_LINEARWORKFLOW, true);
	doc->SetParameter(DOCUMENT_COLORPROFILE, DOCUMENT_COLORPROFILE_SRGB);

	//doc->InsertRenderData(rdat);

	//BaseVideoPost *vp = NewObj(BaseVideoPost(VPglobalillumination);
	//rdat->InsertVideoPostLast(vp);

	//rdat->SetParameter(RDATA_ANTIALIASING, RDATA_ANTIALIASING_BEST);

	//rdat->SetParameter(RDATA_STEREO, true);
	//rdat->SetParameter(RDATA_STEREO_MODE, RDATA_STEREO_MODE_ANAGLYPH);

	//rdat->SetParameter(RDATA_RENDERENGINE, RDATA_RENDERENGINE_PHYSICAL);

	//rdat->SetParameter(RDATA_GLOBALSAVE, true);
	//rdat->SetParameter(RDATA_SAVEIMAGE, true);
	//rdat->SetParameter(RDATA_PATH,Filename(mypath));
	//rdat->SetParameter(RDATA_FORMAT,FILTER_QTVRSAVER_PANORAMA);
	//rdat->SetParameter(RDATA_YRES,320);

	/*
		SketchStyles styles;
		if (styles.Init(Filename("C:\\overshoot.c4d"))) // file could be found, initialization succeeded
		{
			BaseMaterial *sketch_material = styles.AdjustStyle(doc);
			if (!sketch_material) return FALSE; // check if style adjustment failed

			// adjust parameters - depending on style
	//		sketch_material->SetParameter(OUTLINEMAT_THICKNESS,2.0);             // line thickness
	//		sketch_material->SetParameter(OUTLINEMAT_COLOR,Vector(1.0,0.0,0.0)); // line color
	//		sketch_material->SetParameter(OUTLINEMAT_OVERSHOOT_START,10.0);      // overshoot amount (start)
	//		sketch_material->SetParameter(OUTLINEMAT_OVERSHOOT_END,10.0);        // overshoot amount (end)
		}
	*/

	// enable to write the document to a temporary path
#if 0
	{
		HyperFile newC4Dfile;
		if (!newC4Dfile.Open(DOC_IDENT,Filename("c:\\test.c4d"),FILEOPEN_WRITE))
			return false;
		doc->Write(&newC4Dfile);
		newC4Dfile.Close();
	}
#endif
  
	return true;
}

static Bool CreateUpdateSceneInMemory(BaseDocument *doc, GeMarker &stamp)
{
	// replace material in scene with marker 'stamp' by new material

	//BaseObject *pNewObject = BaseObject::Alloc(Ofigure);
	BaseMaterial *pNewObject = BaseMaterial::Alloc(Mmaterial);
	if (!pNewObject)
		return false;

	if (!pNewObject->AddUniqueID(MAXON_CREATOR_ID, (const Char*) &stamp, sizeof(stamp)))
		return false;

	//doc->InsertObject(pNewObject,nullptr);

	pNewObject->SetParameter(MATERIAL_COLOR_COLOR, Vector(0.0, 1.0, 0.0));

	doc->InsertMaterial(pNewObject, nullptr);

	return true;
}

struct ExternalReference
{
	Filename  absolute_texture_path;
	Filename	new_name;
};

static Bool VisitShader(const Filename &documentpath, const String &materialname, GeTempDynArray<ExternalReference> &references, BaseShader *shader)
{
	while (shader)
	{
		if (!VisitShader(documentpath, materialname, references, shader->GetDown()))
			return false;

		if (shader->GetType() == Xbitmap)
		{
			BaseContainer *bc = shader->GetDataInstance();

			Filename bitmapname = bc->GetFilename(BITMAPSHADER_FILENAME);
			Filename directory = bitmapname.GetDirectory();

			if (!directory.IsPopulated()) // no absolute path
			{
				if (GeFExist(documentpath + Filename("tex") + bitmapname)) // texture is in scene's 'tex' folder
					bitmapname = documentpath + Filename("tex") + bitmapname;
				else if (GeFExist(documentpath + bitmapname)) // texture is in scene's folder
					bitmapname = documentpath + bitmapname;
			}

			if (!GeFExist(bitmapname))
			{
				// texture could not be found - this can happen if the texture was not sent with the scene file
				// here a file chooser could ask for the original path of the texture and set this in 'bitmapname'
				// if the user cancels the file chooser FALSE should be returned so that the whole action is canceled
				CriticalStop();
			}
			else
			{
				Bool clash, finished;
				Int32 i, counter = 2;
				ExternalReference *exref = NewObj(ExternalReference);
				if (!exref)
					return false;

				exref->absolute_texture_path = bitmapname;
				exref->new_name = Filename(materialname+"_"+bitmapname.GetFileString());

				while (true)
				{
					clash = finished = false;
					for (i = 0; i < references.GetCount(); i++)
					{
						ExternalReference *cmp = references.GetIndex(i);
						if (!cmp)
							continue;

						if (cmp->absolute_texture_path == bitmapname) // found an existing match - texture already imported
						{
							bc->SetFilename(BITMAPSHADER_FILENAME, cmp->new_name);
							finished = true;
							break;
						}
						else if (cmp->new_name == exref->new_name) // absolute path is different, but newly created path clashes
						{
							clash = true;
							// do not break - as absolute path could still be the same
						}
					}

					if (finished)
					{
						DeleteObj(exref);
						break; // leave while loop
					}
					else if (clash)
					{
						// collision, so we need to change the name of the output texture

						exref->new_name = Filename(materialname+"_"+String::IntToString(counter)+"_"+bitmapname.GetFileString());
						counter++;
					}
					else // no match found, new texture needs to be inserted
					{
						references.Append(exref);
						bc->SetFilename(BITMAPSHADER_FILENAME, exref->new_name);
						break; // leave while loop
					}
				}
			}
		}

		shader = shader->GetNext();
	}
	return true;
}

static void ReplaceIllegalCharacters(String& str, UInt16 c, UInt16 r)
{
	Int32 pos = 0;
	while (str.FindFirst(c, &pos))
		str[pos] = r;
}

static void ReplaceIllegalCharacters(String& str)
{
	ReplaceIllegalCharacters(str,'/','_');
	ReplaceIllegalCharacters(str,'\\','_');
	ReplaceIllegalCharacters(str,'\"','_');
	ReplaceIllegalCharacters(str,':','_');
	ReplaceIllegalCharacters(str,'?','_');
	ReplaceIllegalCharacters(str,'>','_');
	ReplaceIllegalCharacters(str,'<','_');
	ReplaceIllegalCharacters(str,'|','_');
	ReplaceIllegalCharacters(str,'*','_');
}

static BaseMaterial *RestoreMaterialFile(void *material_mem, Int material_size)
{
	Filename fn;
	fn.SetMemoryReadMode(material_mem, material_size);

	BaseDocument tmp_doc;
	HyperFile C4Dfile;
	if (!C4Dfile.Open(DOC_IDENT, fn, FILEOPEN_READ))
		return nullptr;
	if (!tmp_doc.ReadObject(&C4Dfile, true))
		return nullptr;
	C4Dfile.Close();

	BaseMaterial *mat = tmp_doc.GetFirstMaterial();
	if (!mat)
		return nullptr;

	mat->Remove();
	return mat;
}

static Bool ImportMaterialFile(const Filename &filename)
{
	Int32 i;
	BaseDocument materialdoc;
	HyperFile C4Dfile;
	if (!C4Dfile.Open(DOC_IDENT, filename, FILEOPEN_READ))
		return false; // no C4D file
	if (!materialdoc.ReadObject(&C4Dfile, true))
		return false; // loading failed
	C4Dfile.Close();

	BaseMaterial*	mat = materialdoc.GetFirstMaterial();
	String				internal_materialname;
	Filename			documentpath = filename.GetDirectory();
	GeTempDynArray<ExternalReference> references;

	// for each material in the scene...
	while (mat)
	{
		internal_materialname = mat->GetName();
		ReplaceIllegalCharacters(internal_materialname);
		// make sure to modify 'internal_materialname' in a way that it is unique - as VisitShader will rename all textures with the name of the material as prefix
		// if the material name is not unique the textures might collide

		if (!VisitShader(documentpath, internal_materialname, references, mat->GetFirstShader()))
		{
			references.FlushAll();
			return false;
		}

		// now we have a complete list with external references and renamings
		for (i = 0; i < references.GetCount(); i++)
		{
			ExternalReference *exref = references.GetIndex(i);
			if (!exref)
				continue;

			// load this texture with the path 'exref->absolute_texture_path' and save it into the database under the name 'exref->new_name.GetString()'
		}
		references.FlushAll(); // flush the reference list

		BaseDocument tmpdoc;
		mat->Remove();
		tmpdoc.InsertMaterial(mat);

		MemoryFileStruct mfs;
		Filename fn;
		fn.SetMemoryWriteMode(mfs);

		HyperFile newC4Dfile;
		if (!newC4Dfile.Open(DOC_IDENT, fn, FILEOPEN_WRITE))
			return false;
		tmpdoc.Write(&newC4Dfile);
		newC4Dfile.Close();
		if (newC4Dfile.GetError() != FILEERROR_NONE)
			return false;

		// mfs now contains the material as a binary file with address 'mfs.t_mem' and size 'mfs.t_size'
		// add binary data to database here under the name 'internal_materialname'

		// as a test we'll decode the material here:
		void *t_mem = nullptr;
		Int t_size = 0;
		if (!mfs.GetSingleBlock(t_mem, t_size))
			return false;

		BaseMaterial *decoded = RestoreMaterialFile(t_mem, t_size);
		if (decoded)
		{
			// everything ok
			BaseMaterial::Free(decoded);
		}
		else
		{
			// loading failed
			return false;
		}

		mat = materialdoc.GetFirstMaterial();
	}
	return true;
}

class ActionThread1 : public IpCommunicationThread
{
public:
	Filename transferfile;
	GeMarker modification_marker;

	virtual void Main(void)
	{
		Int32 rendererror = RENDERERROR_OK, renderphase = PROGRESSTYPE_BEFORERENDERING;
		Float percent_done = 0.0;
		Bool running = true;

		// clear the output bitmap
		ClearBitmap();

		// open communication to the server
		if (!Open(DEFAULT_IP, UNIQUE_APPLICATION_ID, 10, 300))
		{
			ShowMessage(0);
			return;
		}

		if (!transferfile.IsPopulated())
		{
			BaseDocument doc;

			if (!CreateSceneInMemory(&doc, modification_marker))
				return;

			// transmit the scene file
			if (!SendScenefile(Filename("test"),&doc))
				return;
		}
		else
		{
			// transmit the scene file
			if (!SendScenefile(transferfile))
			{
				ChangeTitle("Sending scene file failed !");
				return;
			}
		}

		if (!SendTexture(BITMAPTESTPATH, memorytexture, sizeof(memorytexture), false))
			return;

		Int32 i;
		for (i = 0; i < 4; i++)
		{
			// start a rendering with resolution 640x480
			// when starting floating point rendering don't forget to set RDATA_FORMATDEPTH to RDATA_FORMATDEPTH_32 and
			// the save format (even though no saving is activated) to a format that supports 32 bit (e.g. RDATA_FORMAT to FILTER_TIF). Otherwise no 32 bit data will be generated during rendering

			if (!StartRender(640, 480, COLORRESOLUTION_FLOAT, false, false))
				return;

			// receive image updates
			running = true;
			while (!TestBreak() && running)
			{
				if (!GetImageUpdate(WinImageUpdate, nullptr, &running))
				{
					rendererror = RENDERERROR_CONNECTIONFAILED;
					break;
				}

				if (!GetRenderStatus(nullptr, &rendererror, &percent_done, &renderphase))
				{
					rendererror = RENDERERROR_CONNECTIONFAILED;
					break;
				}

				if (renderphase == PROGRESSTYPE_DURINGRENDERING)
					ChangeTitle(String("Rendering... ") + String::FloatToString(percent_done*100.0) + String("%"));
				else if (renderphase == PROGRESSTYPE_BEFORERENDERING)
					ChangeTitle(String("Preparing... ") + String::FloatToString(percent_done*100.0) + String("%"));
				else if (renderphase == PROGRESSTYPE_GLOBALILLUMINATION)
					ChangeTitle(String("Calculating Global Illumination... ") + String::FloatToString(percent_done*100.0) + String("%"));
				else if (renderphase == PROGRESSTYPE_AMBIENTOCCLUSION)
					ChangeTitle(String("Calculating Ambient Occlusion... ") + String::FloatToString(percent_done*100.0) + String("%"));
				else if (renderphase == PROGRESSTYPE_AFTERRENDERING)
					ChangeTitle(String("Finishing... ") + String::FloatToString(percent_done*100.0) + String("%"));
 				else if (renderphase == PROGRESSTYPE_QUICK_PREVIEW)
 					ChangeTitle(String("Quick Preview... ") + String::FloatToString(percent_done*100.0) + String("%"));

				RedrawWindow();
				if (running)
					GeSleep(400); // update every 400 ms to reduce computing time for updates
			}

			RedrawWindow();

			if (rendererror != RENDERERROR_OK)
				break; // could be user break etc.
			if (i == 3 || transferfile.IsPopulated())
				break; // render different cameras only for our scene in memory

#if 0
        // set this to 1 to only render one view
				break;
#endif
      
#if 0
      {
        // modify existing render settings on server - testcode
				BaseDocument tmpdoc;
				RenderData *rdat = tmpdoc.GetActiveRenderData();
				if (!rdat)
					break; // should not happen
				rdat->SetParameter(RDATA_LOD, 0.25+i*0.25);
				UpdateRenderSettings(rdat);
			}
#endif
      
      
#if 0
			{
        // modify existing camera on server - testcode
				BaseDocument tmpdoc;
				BaseObject *camera = BaseObject::Alloc(Ocamera);
				if (!camera)
					break; // should not happen
				camera->SetAbsPos(Vector(i*100.0, i*100.0, -i*100.0));
				UpdateCamera(camera);
			}
#endif
      
#if 1
			{
        // modify existing scene on server - testcode
				if (!transferfile.IsPopulated())
				{
					// build update document
					BaseDocument tmpdoc;
					if (!CreateUpdateSceneInMemory(&tmpdoc, modification_marker))
						return;

					// send update - and use standard C4D markers
					// transmit the scene file
					if (!UpdateScenefile(MAXON_CREATOR_ID, &tmpdoc))
						return;
				}
			}
#endif
      
			UpdateView(Vector(0.0), Vector(0.0), 36.0, 36.0, 2.0, i == 0?Pleft:(i == 1?Ptop:Pfront)); // modify view in scene on server

			ClearBitmap();
		}

		if (rendererror != RENDERERROR_OK)
			ChangeTitle("Rendering failed");
		else
			ChangeTitle("Rendering finished");

		// free the session on the server
		FreeSession();

		// close communications with the server
		Close(false);
	}
};

class ActionThread2 : public IpCommunicationThread
{
public:
	virtual void Main(void)
	{
		if (!Open(DEFAULT_IP, UNIQUE_APPLICATION_ID, 10, 60))
			ShowMessage(0);
		else
			Close(true);
	}
};

class ActionThread3 : public IpCommunicationThread
{
public:
	virtual void Main(void)
	{
		if (!Open(DEFAULT_IP, UNIQUE_APPLICATION_ID, 10, 60))
			ShowMessage(0);
		else
		{
			StopRender();
			Close(false);
		}
	}
};

class ActionThreadLicenseManager : public IpCommunicationThread
{
public:
	virtual void Main(void)
	{
		if (!Open(DEFAULT_IP, UNIQUE_APPLICATION_ID, 10, 60))
		{
			ShowMessage(0);
		}
		else
		{
			OpenLicenseManager();
			Close(false);
		}
	}
};

class ActionThreadSignOut : public IpCommunicationThread
{
public:
	virtual void Main(void)
	{
		if (!Open(DEFAULT_IP, UNIQUE_APPLICATION_ID, 10, 60))
		{
			ShowMessage(0);
		}
		else
		{
			SignOut();
			Close(false);
		}
	}
};

static ActionThread1 *th1 = nullptr;
static ActionThread2 *th2 = nullptr;
static ActionThread3 *th3 = nullptr;
static ActionThreadLicenseManager* thLogin = nullptr;
static ActionThreadSignOut* thLogout = nullptr;

void CallCommand(Int32 type, const Filename &filename)
{
	if (type == 0)
	{
		// if no render thread is active start a new one
		if (!th1)
			th1 = NewObj(ActionThread1);
		if (!th1 || th1->IsRunning())
			return;
		th1->transferfile = Filename();
		th1->Start(true);
	}
	else if (type == 1)
	{
		// if no shutdown thread is active start a new one
		if (!th2)
			th2 = NewObj(ActionThread2);
		if (!th2 || th2->IsRunning())
			return;
		th2->Start(true);
	}
	else if (type == 2)
	{
		// if no render thread is active start a new one
		if (!th1)
			th1 = NewObj(ActionThread1);
		if (!th1 || th1->IsRunning())
			return;
		th1->transferfile = filename;
		th1->Start(true);
	}
	else if (type == 3)
	{
		// the current process ID (pid) will be send too and the render server will automatically shut down if your process ends
		if (!StartRenderServer(filename, 1234))
			ShowMessage(2);
	}
	else if (type == 4)
	{
		// if no stop thread is active start a new one
		if (!th3)
			th3 = NewObj(ActionThread3);
		if (!th3 || th3->IsRunning())
			return;
		th3->Start(true);
	}
	else if (type == 5)
	{
		ImportMaterialFile(filename);
	}
	else if (type == 6)
	{
		BaseDocument *doc = BaseDocument::Alloc();
		if (!doc)
			return;

		BaseObject *op = BaseObject::Alloc(Olight);
		if (!op)
			return;

		doc->InsertObject(op, nullptr, nullptr);

		op->SetParameter(DescLevel(LIGHT_PHOTOMETRIC_FILE), filename);

		Filename fn = filename;
		fn.ClearSuffix();
		fn.SetSuffix("c4d");

		SaveDocument(doc, fn, SAVEDOCUMENTFLAGS_0);

		BaseDocument::Free(doc);
	}
	else if (type == 7)
	{
		if (!thLogin)
			thLogin = NewObj(ActionThreadLicenseManager);
		if (!thLogin || thLogin->IsRunning())
			return;
		thLogin->Start(true);

	}
	else if (type == 8)
	{
		if (!thLogout)
			thLogout = NewObj(ActionThreadSignOut);
		if (!thLogout || thLogout->IsRunning())
			return;
		thLogout->Start(true);
	}
}

// end and free all threads
void FreeRenderhost()
{
	DeleteObj(th1);
	DeleteObj(th2);
	DeleteObj(th3);
	DeleteObj(thLogin);
	DeleteObj(thLogout);
}

// cineware application ID
void GetWriterInfo(Int32 &id, String &appname)
{
	id = UNIQUE_APPLICATION_ID;
	appname = "Cineware Renderhost Example";
}
