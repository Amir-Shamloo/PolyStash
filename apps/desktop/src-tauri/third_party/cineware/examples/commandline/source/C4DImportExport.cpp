/////////////////////////////////////////////////////////////
// CINEWARE SDK - command line example                     //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#include "C4DImportExport.h"
#include "c4d_browsecontainer.h"
#include "parameter_ids/material/mbase.h"

// here you should use the cineware namespace
using namespace cineware;

// global temporary variables for this export example of c4d scene file
BaseDocument* g_myInternalDoc = nullptr;	// pointer to the imported C4D document use to demonstrate the export, this should your own type of document
Int32 g_tempLayID = 0;										// also only for demonstration purposes, used to enumerate IDs and to assign as Layer IDs
Int32 g_tempmatid = 0;										// also only for demonstration purposes, used to enumerate IDs and to assign as Material IDs

// memory allocation functions inside cineware namespace (if you have your own memory management you can overload these functions)
namespace cineware
{
// alloc memory no clear
void* MemAllocNC(Int size)
{
	void *mem = malloc(size);
	return mem;
}

// alloc memory set to 0
void* MemAlloc(Int size)
{
	void *mem = MemAllocNC(size);
	memset(mem, 0, size);
	return mem;
}

// realloc existing memory
void* MemRealloc(void* orimem, Int size)
{
	void *mem = realloc(orimem, size);
	return mem;
}

// free memory and set pointer to null
void MemFree(void*& mem)
{
	if (!mem)
		return;

	free(mem);
	mem = nullptr;
}
}

// overload this function and fill in your own unique data
void GetWriterInfo(Int32& id, String& appname)
{
	// register your own pluginid once for your exporter and enter it here under id
	// this id must be used for your own unique ids
	// 	Bool AddUniqueID(Int32 appid, const Char *const mem, Int32 bytes);
	// 	Bool FindUniqueID(Int32 appid, const Char *&mem, Int32 &bytes) const;
	// 	Bool GetUniqueIDIndex(Int32 idx, Int32 &id, const Char *&mem, Int32 &bytes) const;
	// 	Int32 GetUniqueIDCount() const;
	id = 0;
	appname = "Cineware Commandline Example";
}

//////////////////////////////////////////////////////////////////////////

// print UIDs for all existing application IDs
static void PrintUniqueIDs(BaseList2D* op)
{
	// actual UIDs - size can be different from application to application
	Int32 t = 0;
	Int32 i = 0;
	Int32 cnt = op->GetUniqueIDCount();

	for (i = 0; i < cnt; i++)
	{
		Int32 appid = 0;
		Int		bytes = 0;
		const Char *mem = nullptr;
		if (op->GetUniqueIDIndex(i, appid, mem, bytes))
		{
			if (!op->FindUniqueID(appid, mem, bytes))
			{
				CriticalStop();
				continue;
			}
			printf("   - UniqueID (%d Byte): ",(int)bytes);
			for (t = 0; t < bytes; t++)
			{
				printf("%2.2x",(UChar)mem[t]);
			}
			printf(" [AppID: %d]",(int)appid);
			printf("\n");
		}
	}
}

static void PrintUniqueIDs(NodeData *op)
{
	if (op)
		PrintUniqueIDs(op->GetNode());
}

// shows how to access user data
static void PrintUserData(BaseList2D* bl)
{
	if (!bl)
		return;

	// get the user data
	DynamicDescription* desc = bl->GetDynamicDescription();
	if (desc)
	{
		// temporary id and container for browsing
		DescID id;
		const BaseContainer* bc;

		// init the browse class
		void* handle = desc->BrowseInit();

		// go through all entries
		while (desc->BrowseGetNext(handle, &id, &bc))
		{
			// get the name of the user data
			Char* name = bc->GetString(DESC_NAME).GetCStringCopy();

			// get the default value and type of the user data
			GeData data = bc->GetData(DESC_DEFAULT);
			Int32 type = data.GetType();
			Bool defaultValue = true;

			// if no default value is defined
			if (type <= 0)
			{
				// get the stored value and this type
				if (bl->GetDataInstanceRef().GetParameter(id, data))
					type = data.GetType();

				defaultValue = false;
			}

			// decide by type how to get the values
			if (type == DA_VECTOR)
			{
				// Vector
				Vector vecDef = defaultValue ? data.GetVector() : Vector(0);
				Vector vecVal = Vector(0);

				if (bl->GetDataInstanceRef().GetParameter(id, data) && data.GetType() == DA_VECTOR)
					vecVal = data.GetVector();

				printf("   - User Data: %s (type=%d) - %f/%f/%f (default=%f/%f/%f)\n", name, type, vecVal.x, vecVal.y, vecVal.z, vecDef.x, vecDef.y, vecDef.z);
			}
			else if (type == DA_LONG)
			{
				// Int32
				Int32 intDef = defaultValue ? data.GetInt32() : 0;
				Int32 intVal = 0;

				if (bl->GetDataInstanceRef().GetParameter(id, data) && data.GetType() == DA_LONG)
					intVal = data.GetInt32();

				printf("   - User Data: %s (type=%d) - %d (default=%d)\n", name, type, intVal, intDef);
			}
			else if (type == DA_REAL)
			{
				// Float
				Float floatDef = defaultValue ? data.GetFloat() : 0.0;
				Float floatVal = 0.0;

				if (bl->GetDataInstanceRef().GetParameter(id, data) && data.GetType() == DA_REAL)
					floatVal = data.GetFloat();

				printf("   - User Data: %s (type=%d) - %f (default=%f)\n", name, type, floatVal, floatDef);
			}
			else if (type == DA_STRING)
			{
				// String
				char* strDef = defaultValue ? data.GetString().GetCStringCopy() : nullptr;
				char* strVal = nullptr;

				if (bl->GetDataInstanceRef().GetParameter(id, data) && data.GetType() == DA_STRING)
					strVal = data.GetString().GetCStringCopy();

				if (strVal && strDef)
					printf("   - User Data: %s (type=%d) - \"%s\" (default=\"%s\")\n", name, type, strVal, strDef);
				else if (strVal)
					printf("   - User Data: %s (type=%d) - \"%s\"\n", name, type, strVal);
				else
					printf("   - User Data: %s (type=%d) - \"\"\n", name, type);

				DeleteMem(strDef);
				DeleteMem(strVal);
			}
			else
			{
				// types we do not cover here
				printf("   - User Data: %s - (%d) <unknown type>\n", name, type);
			}

			DeleteMem(name);
		}

		// free the browse data
		desc->BrowseFree(handle);

		printf("\n");
	}
}

// prints tag infos to the console
static void PrintTagInfo(BaseObject *obj)
{
	if (!obj)
		return;

	char *pChar = nullptr, buffer[256];
	GeData data;

	// browse all tags and access data regarding type
	BaseTag* btag = obj->GetFirstTag();
	for (; btag; btag = (BaseTag*)btag->GetNext())
	{
		// name for all tag types
		pChar = btag->GetName().GetCStringCopy();
		if (pChar)
		{
			printf("   - %s \"%s\"", GetObjectTypeName(btag->GetType()), pChar);
			DeleteMem(pChar);
		}
		else
		{
			printf("   - %s \"\"", GetObjectTypeName(btag->GetType()));
		}

		// compositing tag
		if (btag->GetType() == Tcompositing)
		{
			if (btag->GetParameter(COMPOSITINGTAG_MATTEOBJECT, data) && data.GetInt32())
			{
				if (btag->GetParameter(COMPOSITINGTAG_MATTECOLOR, data))
					printf("     + Matte - Color R %d G %d B %d", (int)(data.GetVector().x*255.0), (int)(data.GetVector().y*255.0), (int)(data.GetVector().z*255.0));
				else
					printf("     + Matte - Color NOCOLOR");
			}
			if (btag->GetParameter(COMPOSITINGTAG_ENABLECHN4, data) && data.GetInt32())
			{
				if (btag->GetParameter(COMPOSITINGTAG_IDCHN4, data))
					printf("     + Objectbuffer Channel 5 enabled - ID = %d", (int)data.GetInt32());
			}
		}
		// phong tag
		if (btag->GetType() == Tphong)
		{
			if (btag->GetParameter(PHONGTAG_PHONG_ANGLE, data))
				printf(" - Phong Angle = %f", data.GetFloat()*180.0/PI);
		}

		// archi grass tag
		if (btag->GetType() == Tarchigrass)
		{
			AlienMaterial *mat = nullptr;
			if (btag->GetParameter(GRASS_MATLINK, data))
				mat = (AlienMaterial*)data.GetLink();
			if (mat)
			{
				char *pCharGM = mat->GetName().GetCStringCopy();
				if (pCharGM)
				{
					printf(" - material: \"%s\"", pCharGM);
					DeleteMem(pCharGM);
				}
				else
				{
					printf(" - material: <grass mat>");
				}

				printf("\n    ");
				if (mat->GetParameter(GRASS_LENGTH, data))
					printf(" - Len = %f", data.GetFloat());
				if (mat->GetParameter(GRASS_DENSITY, data))
					printf(" - Den = %f", data.GetFloat());
				if (mat->GetParameter(GRASS_WIDTH, data))
					printf(" - Wth = %f", data.GetFloat());
			}
		}

		// target expression
		if (btag->GetType() == Ttargetexpression)
		{
			BaseList2D* targetObj = btag->GetData().GetLink(TARGETEXPRESSIONTAG_LINK);
			if (targetObj)
			{
				targetObj->GetName().GetCString(buffer, sizeof buffer);
				printf(" - linked to \"%s\"", buffer);
			}
		}

		// align to path
		if (btag->GetType() == Taligntopath)
		{
			if (btag->GetParameter(ALIGNTOPATHTAG_LOOKAHEAD, data))
			{
				printf(" - Look Ahead: %f", data.GetTime().Get());
			}
		}

		// vibrate
		if (btag->GetType() == Tvibrate)
		{
			if (btag->GetParameter(VIBRATEEXPRESSION_RELATIVE, data))
			{
				printf(" - Relative: %d", (int)data.GetInt32());
			}
		}

		// coffee
		if (btag->GetType() == Tcoffeeexpression)
		{
			if (btag->GetParameter(1000, data))
			{
				pChar = data.GetString().GetCStringCopy();
				printf("     Code:\n---\n%s\n---\n", pChar);
				DeleteMem(pChar);
			}
		}

		// WWW tag
		if (btag->GetType() == Twww)
		{
			if (btag->GetParameter(WWWTAG_URL, data))
			{
				pChar = data.GetString().GetCStringCopy();
				printf("     URL:  \"%s\"", pChar);
				DeleteMem(pChar);
			}
			if (btag->GetParameter(WWWTAG_INFO, data))
			{
				pChar = data.GetString().GetCStringCopy();
				printf("     INFO: \"%s\"", pChar);
				DeleteMem(pChar);
			}
		}

		// weight tag
		if (btag->GetType() == Tweights)
		{
			WeightTagData *wt = (WeightTagData*)btag->GetNodeData();
			if (wt)
			{
				Int32 pCnt = 0;
				Int32 jCnt = wt->GetJointCount();
				printf(" - Joint Count: %d\n", (int)jCnt);

				BaseObject *jOp = nullptr;
				// print data for 3 joints and 3 points only
				for (Int32 j = 0; j < jCnt && j < 3; j++)
				{
					jOp = wt->GetJoint(j, obj->GetDocument());
					if (jOp)
					{
						pChar = jOp->GetName().GetCStringCopy();
						printf("     Joint %d: \"%s\"\n", (int)j, pChar);
						DeleteMem(pChar);
					}
					printf("     Joint Weight Count:  %d\n", (int)wt->GetWeightCount(j));
					if (obj->GetType() == Opolygon)
					{
						pCnt = ((PolygonObject*)obj)->GetPointCount();
						for (Int32 p = 0; p < pCnt && p < 3; p++)
						{
							printf("     Weight at Point %d: %f\n", (int)p, wt->GetWeight(j,p));
						}
						if (pCnt >= 3)
							printf("     ...\n");
					}
				}
				if (jCnt >= 3 && pCnt < 3)
					printf("     ...\n");
			}
		}

		// SDS weight tag
		if (btag->GetType() == Tsds)
		{
			printf("\n");
			HNWeightTagData* sds = (HNWeightTagData*)btag->GetNodeData();
			if (sds)
			{
				HNData sdsData;
				if (sds->GetTagData(&sdsData) && sdsData.pointweight)
				{
					for (Int32 p = 0; p < *sdsData.points && p < 6; p++)
					{
						printf("     SDS Weight at Vertex %d: %f\n", (int)p, (*sdsData.pointweight)[p]);
					}
					if (*sdsData.points >= 6)
						printf("     ...\n");
				}
			}
		}

		// texture tag
		if (btag->GetType() == Ttexture)
		{
			// detect the material
			AlienMaterial *mat = nullptr;
			if (btag->GetParameter(TEXTURETAG_MATERIAL, data))
				mat = (AlienMaterial*)data.GetLink();
			if (mat)
			{
				char *pCharMat = nullptr, *pCharSh = nullptr;
				Vector col = Vector(0.0, 0.0, 0.0);
				if (mat->GetParameter(MATERIAL_COLOR_COLOR, data))
					col = data.GetVector();

				pCharMat = mat->GetName().GetCStringCopy();
				if (pCharMat)
				{
					printf(" - material: \"%s\" (%d/%d/%d)", pCharMat, int(col.x*255), int(col.y*255), int(col.z*255));
					DeleteMem(pCharMat);
				}
				else
					printf(" - material: <noname> (%d/%d/%d)", int(col.x*255), int(col.y*255), int(col.z*255));
				// detect the shader
				BaseShader* sh = mat->GetShader(MATERIAL_COLOR_SHADER);
				if (sh)
				{
					pCharSh = sh->GetName().GetCStringCopy();
					if (pCharSh)
					{
						printf(" - color shader \"%s\" - Type: %s", pCharSh, GetObjectTypeName(sh->GetType()));
						DeleteMem(pCharSh);
					}
					else
						printf(" - color shader <noname> - Type: %s", GetObjectTypeName(sh->GetType()));
				}
				else
					printf(" - no shader");
			}
			else
				printf(" - no material");
		}

		// normal tag
		if (btag->GetType() == Tnormal)
		{
			printf("\n");
			Int32 count = ((NormalTag*)btag)->GetDataCount();
			const CPolygon *pAdr = nullptr;
			if (obj->GetType() == Opolygon)
				pAdr = ((PolygonObject*)obj)->GetPolygonR();
			ConstNormalHandle normalsPtr = ((NormalTag*)btag)->GetDataAddressR();
			NormalStruct pNormals;
			// print normals of 2 polys
			for (Int32 n = 0; normalsPtr && n < count && n < 2; n++)
			{
				NormalTag::Get(normalsPtr, n, pNormals);
				printf("     Na %d: %.6f / %.6f / %.6f\n", (int)n, pNormals.a.x, pNormals.a.y, pNormals.a.z);
				printf("     Nb %d: %.6f / %.6f / %.6f\n", (int)n, pNormals.b.x, pNormals.b.y, pNormals.b.z);
				printf("     Nc %d: %.6f / %.6f / %.6f\n", (int)n, pNormals.c.x, pNormals.c.y, pNormals.c.z);
				if (!pAdr || pAdr[n].c != pAdr[n].d)
					printf("     Nd %d: %.6f / %.6f / %.6f\n", (int)n, pNormals.d.x, pNormals.d.y, pNormals.d.z);
			}
		}

		// UVW tag
		if (btag->GetType() == Tuvw)
		{
			printf("\n");
			UVWStruct uvw;
			Int32 uvwCount = ((UVWTag*)btag)->GetDataCount();
			// print for 4 polys uvw infos
			for (Int32 u = 0; u < uvwCount && u < 4; u++)
			{
				((UVWTag*)btag)->Get(((UVWTag*)btag)->GetDataAddressR(), u, uvw);
				printf("     Poly %d: %.2f %.2f %.2f / %.2f %.2f %.2f / %.2f %.2f %.2f / %.2f %.2f %.2f \n", (int)u, uvw.a.x, uvw.a.y, uvw.a.z, uvw.b.x, uvw.b.y, uvw.b.z, uvw.c.x, uvw.c.y, uvw.c.z, uvw.d.x, uvw.d.y, uvw.d.z);
			}
		}

		// Polygon Selection Tag
		if (btag->GetType() == Tpolygonselection && obj->GetType() == Opolygon)
		{
			printf("\n");
			BaseSelect *bs = ((SelectionTag*)btag)->GetBaseSelect();
			if (bs)
			{
				Int32 s = 0;
				for (s = 0; s<((PolygonObject*)obj)->GetPolygonCount() && s < 5; s++)
				{
					if (bs->IsSelected(s))
						printf("     Poly %d: selected\n", (int)s);
					else
						printf("     Poly %d: NOT selected\n", (int)s);
				}
				if (s < ((PolygonObject*)obj)->GetPolygonCount())
					printf("     ...\n");
			}
		}

		// Vertex Map tag
		if (btag->GetType() == Tvertexmap)
		{
			printf("\n");
			Int32 vCount = ((VertexMapTag*)btag)->GetDataCount();
			// print the vertex map infos for the first 3 points
			const Float32* values = ((VertexMapTag*)btag)->GetDataAddressR();
			if (values)
			{
				for (Int32 v = 0; v < vCount && v < 3; v++)
				{
					printf("     Point %d: %f\n", (int)v, values[v]);
				}
			}
		}

		// Vertex Color tag
		if (btag->GetType() == Tvertexcolor)
		{
			printf("\n");
			Int32 pCount = ((PolygonObject*)obj)->GetPolygonCount();
			const CPolygon* polys = ((PolygonObject*)obj)->GetPolygonR();
			// print the vertex map infos for the first 3 points
			ConstVertexColorHandle vcPtr = ((VertexColorTag*)btag)->GetDataAddressR();
			VertexColorStruct vcs;
			if (vcPtr && !((VertexColorTag*)btag)->IsPerPointColor())
			{
				for (Int32 pIdx = 0; pIdx < pCount && pIdx < 3; pIdx++)
				{
					((VertexColorTag*)btag)->Get(vcPtr, pIdx, vcs);
					printf("     Poly %d Vertex 1: %f / %f / %f\n", pIdx, vcs.a.x, vcs.a.y, vcs.a.z);
					printf("     Poly %d Vertex 2: %f / %f / %f\n", pIdx, vcs.b.x, vcs.b.y, vcs.b.z);
					printf("     Poly %d Vertex 3: %f / %f / %f\n", pIdx, vcs.c.x, vcs.c.y, vcs.c.z);
					if (polys[pIdx].c != polys[pIdx].d)
						printf("     Poly %d Vertex 4: %f / %f / %f\n", pIdx, vcs.d.x, vcs.d.y, vcs.d.z);
				}
			}
		}

		// Display tag
		if (btag->GetType() == Tdisplay)
		{
			printf("\n");

      btag->GetParameter(DISPLAYTAG_AFFECT_LEVELOFDETAIL, data);
      const Bool aLod = data.GetBool();
      btag->GetParameter(DISPLAYTAG_LEVELOFDETAIL, data);
      const Float vLod = data.GetFloat();
      btag->GetParameter(DISPLAYTAG_AFFECT_VISIBILITY, data);
      const Bool aVis = data.GetBool();
      btag->GetParameter(DISPLAYTAG_VISIBILITY, data);
      const Float vVis = data.GetFloat();
      btag->GetParameter(DISPLAYTAG_AFFECT_BACKFACECULLING, data);
      const Bool aBfc = data.GetBool();
      btag->GetParameter(DISPLAYTAG_BACKFACECULLING, data);
      const Bool eBfc = data.GetBool();
      btag->GetParameter(DISPLAYTAG_AFFECT_TEXTURES, data);
      const Bool aTex = data.GetBool();
      btag->GetParameter(DISPLAYTAG_TEXTURES, data);
      const Bool eTex = data.GetBool();
      btag->GetParameter(DISPLAYTAG_AFFECT_HQ_OGL, data);
      const Bool aGl = data.GetBool();
      btag->GetParameter(DISPLAYTAG_HQ_OGL, data);
      const Bool eGl = data.GetBool();
			printf("     (Used/Value) LOD:%d/%d VIS:%d/%d BFC:%d/%d TEX:%d/%d OGL:%d/%d\n", aLod, Int32(vLod * 100.0), aVis, Int32(vVis * 100.0), aBfc, eBfc, aTex, eTex, aGl, eGl);
		}

		PrintUserData(btag);

		printf("\n");
	}
}

// shows how to access parameters of 3 different shader types and prints it to the console
static void PrintShaderInfo(BaseShader *shader, Int32 depth = 0)
{
	BaseShader *sh = shader;
	while (sh)
	{
		for (Int32 s=0; s<depth; s++) printf(" ");

		// type layer shader
		if (sh->GetType() == Xlayer)
		{
			printf("LayerShader - %d\n", (int)sh->GetType());

			BaseContainer* pData = sh->GetDataInstance();
			GeData blendData = pData->GetData(SLA_LAYER_BLEND);
			iBlendDataType* d = (iBlendDataType*)blendData.GetCustomDataType(CUSTOMDATA_BLEND_LIST);
			if (d)
			{
				LayerShaderLayer *lsl = (LayerShaderLayer*)(d->m_BlendLayers.GetObject(0));

				while (lsl)
				{
					for (Int32 s=0; s<depth; s++) printf(" ");

					printf(" LayerShaderLayer - %s (%d)\n", GetObjectTypeName(lsl->GetType()), lsl->GetType());

					// folder ?
					if (lsl->GetType() == TypeFolder)
					{
						LayerShaderLayer *subLsl = (LayerShaderLayer*)((BlendFolder*)lsl)->m_Children.GetObject(0);
						while (subLsl)
						{
							for (Int32 s=0; s<depth; s++) printf(" ");
							printf("  Shader - %s (%d)\n", GetObjectTypeName(subLsl->GetType()), subLsl->GetType());

							// base shader ?
							if (subLsl->GetType() == TypeShader)
								PrintShaderInfo((BaseShader*)((BlendShader*)subLsl)->m_pLink->GetLink(), depth+1);

							subLsl = subLsl->GetNext();
						}
					}
					else if (lsl->GetType() == TypeShader)
						PrintShaderInfo((BaseShader*)((BlendShader*)lsl)->m_pLink->GetLink(), depth+2);

					lsl = lsl->GetNext();
				}
			}
		}
		// type bitmap shader (texture)
		else if (sh->GetType() == Xbitmap)
		{
			Char *pCharShader =  sh->GetFileName().GetString().GetCStringCopy();
			if (pCharShader)
			{
				printf("Shader - %s (%d) : %s\n", GetObjectTypeName(sh->GetType()), (int)sh->GetType(), pCharShader);
				DeleteMem(pCharShader);
				pCharShader =  sh->GetFileName().GetFileString().GetCStringCopy();
				if (pCharShader)
				{
					for (Int32 s=0; s<depth; s++) printf(" ");
					printf("texture name only: \"%s\"\n", pCharShader);
					DeleteMem(pCharShader);
				}
			}
			else
			{
				printf("Shader - %s (%d) : ""\n", GetObjectTypeName(sh->GetType()), (int)sh->GetType());
			}
		}
		// type gradient shader
		else if (sh->GetType() == Xgradient)
		{
			printf("Shader - %s (%d) : ", GetObjectTypeName(sh->GetType()), (int)sh->GetType());
			GeData data;
			sh->GetParameter(SLA_GRADIENT_GRADIENT, data);
			Gradient *pGrad = (Gradient*)data.GetCustomDataType(CUSTOMDATATYPE_GRADIENT);
			Int32 kcnt = pGrad->GetKnotCount();
			printf(" %d Knots\n", (int)kcnt);
			for (Int32 k = 0; k < kcnt; k++)
			{
				GradientKnot kn = pGrad->GetKnot(k);
				for (Int32 s=0; s<depth; s++) printf(" ");
				printf("   -> %d. Knot: %.1f/%.1f/%.1f\n", (int)k, kn.col.x*255.0, kn.col.y*255.0, kn.col.z*255.0);
			}
		}
		// type variation shader
		else if (sh->GetType() == Xvariation)
		{
			printf("Shader - %s (%d)\n", GetObjectTypeName(sh->GetType()), (int)sh->GetType());
			VariationShaderData* vData = (VariationShaderData*)sh->GetNodeData();
			if (vData)
			{
				Int32 texCnt = vData->GetTextureCount();
				for (Int32 t = 0; t < texCnt; t++)
				{
					TextureLayer lay = vData->GetTextureLayer(t);
					printf("   -> Layer: type: %s  active: %d  prop: %f\n", lay._shader ? GetObjectTypeName(lay._shader->GetType()) : "<nullptr>", lay._active, lay._probability);
					if (lay._shader)
					{
						if (lay._shader->GetType() == Xbitmap)
						{
							Char *pChar = lay._shader->GetFileName().GetString().GetCStringCopy();
							if (pChar)
							{
								printf("       -> Texture: %s\n", pChar);
								DeleteMem(pChar);
							}
						}
					}
				}
			}
			else
			{
				printf("Shader - %s (%d)\n", GetObjectTypeName(sh->GetType()), (int)sh->GetType());

				PrintShaderInfo(sh->GetDown(), depth+1);
			}
		}

		sh = sh->GetNext();
	}
}

// shows how to access render data parameter and prints it to the console
static void PrintRenderDataInfo(RenderData *rdata)
{
	if (!rdata)
		return;

	printf("\n\n # Render Data #\n");

	GeData data;
	// renderer
	if (rdata->GetParameter(RDATA_RENDERENGINE, data))
	{
		switch (data.GetInt32())
		{
			case RDATA_RENDERENGINE_PREVIEWSOFTWARE:
				printf(" - Renderengine - PREVIEWSOFTWARE\n");
				break;

			case RDATA_RENDERENGINE_PREVIEWHARDWARE:
				printf(" - Renderengine - PREVIEWHARDWARE\n");
				break;

			case RDATA_RENDERENGINE_CINEMAN:
				printf(" - Renderengine - CINEMAN\n");
				break;

			case RDATA_RENDERENGINE_STANDARD:
				printf(" - Renderengine - STANDARD\n");

			default:
				printf(" - Renderengine - Unknown ID: %d\n", data.GetInt32());
		}
	}

	// save option on/off ?
	if (rdata->GetParameter(RDATA_GLOBALSAVE, data) && data.GetInt32())
	{
		printf(" - Global Save - ENABLED\n");
		if (rdata->GetParameter(RDATA_SAVEIMAGE, data) && data.GetInt32())
		{
			if (rdata->GetParameter(RDATA_PATH, data))
			{
				Char *pChar = data.GetFilename().GetString().GetCStringCopy();
				if (pChar)
				{
					printf("   + Save Image - %s\n", pChar);
					DeleteMem(pChar);
				}
				else
					printf("   + Save Image\n");
			}
			else
				printf("   + Save Image\n");
		}
		// save options: alpha, straight alpha, separate alpha, dithering, sound
		if (rdata->GetParameter(RDATA_ALPHACHANNEL, data) && data.GetInt32())
			printf("   + Alpha Channel\n");
		if (rdata->GetParameter(RDATA_STRAIGHTALPHA, data) && data.GetInt32())
			printf("   + Straight Alpha\n");
		if (rdata->GetParameter(RDATA_SEPARATEALPHA, data) && data.GetInt32())
			printf("   + Separate Alpha\n");
		if (rdata->GetParameter(RDATA_TRUECOLORDITHERING, data) && data.GetInt32())
			printf("   + 24 Bit Dithering\n");
		if (rdata->GetParameter(RDATA_INCLUDESOUND, data) && data.GetInt32())
			printf("   + Include Sound\n");
	}
	else
		printf(" - Global Save = false\n");

	// multi pass enabled ?
	if (rdata->GetParameter(RDATA_MULTIPASS_ENABLE, data) && data.GetInt32())
	{
		printf(" - Multi pass - ENABLED\n");
		if (rdata->GetParameter(RDATA_MULTIPASS_SAVEIMAGE, data) && data.GetInt32())
		{
			if (rdata->GetParameter(RDATA_MULTIPASS_FILENAME, data))
			{
				Char *pChar = data.GetFilename().GetString().GetCStringCopy();
				if (pChar)
				{
					printf("   + Save Multi pass Image - %s\n", pChar);
					DeleteMem(pChar);
				}
				else
					printf("   + Save Multi pass Image\n");
			}
			else
				printf("   + Save Multi pass Image\n");
		}

		if (rdata->GetParameter(RDATA_MULTIPASS_STRAIGHTALPHA, data) && data.GetInt32())
			printf("   + Multi pass Straight Alpha\n");
		MultipassObject *mobj = rdata->GetFirstMultipass();

		if (mobj)
		{
			while (mobj)
			{
				if (mobj->GetParameter(MULTIPASSOBJECT_TYPE, data))
				{
					printf("   + Multi pass Channel: %d", (int)data.GetInt32());
					if (data.GetInt32() == VPBUFFER_OBJECTBUFFER)
					{
						if (mobj->GetParameter(MULTIPASSOBJECT_OBJECTBUFFER, data))
							printf(" Group ID %d", (int)data.GetInt32());
					}

					printf("\n");
					mobj = (MultipassObject*)mobj->GetNext();
				}
			}
		}
	}
	// print out enabled post effects
	BaseVideoPost *vp = rdata->GetFirstVideoPost();
	if (vp)
		printf( " - VideoPostEffects:\n");
	while (vp)
	{
		// enabled / disabled ?
		printf("   + %s ", vp->GetBit(BIT_VPDISABLED) ? "[OFF]" : "[ON ]");

		switch (vp->GetType())
		{
			case VPambientocclusion:
				printf("Ambient Occlusion");
				if (vp->GetParameter(VPAMBIENTOCCLUSION_ACCURACY, data) && data.GetType() == DA_REAL)
					printf(" (Accuracy = %f)", (100.0 * data.GetFloat()));
				break;
			case VPcomic:
				printf("Celrender");
				if (vp->GetParameter(VP_COMICOUTLINE, data) && data.GetType() == DA_LONG)
					printf(" (Outline = %s)", data.GetInt32() ? "true" : "false");
				break;
			case VPcolorcorrection:
				printf("Color correction");
				if (vp->GetParameter(ID_PV_FILTER_CONTRAST, data) && data.GetType() == DA_REAL)
					printf(" (Contrast = %f)", (100.0 * data.GetFloat()));
				break;
			case VPcolormapping:
				printf("Color mapping");
				if (vp->GetParameter(COLORMAPPING_BACKGROUND, data) && data.GetType() == DA_LONG)
					printf(" (Affect Background = %s)", data.GetInt32() ? "true" : "false");
				break;
			case VPcylindricallens:
				printf("Cylindrical Lens");
				if (vp->GetParameter(CYLINDERLENS_VERTICALSIZE, data) && data.GetType() == DA_REAL)
					printf(" (Vertical Size = %f)", data.GetFloat());
				break;
			case VPopticsuite_depthoffield:
				printf("Depth of Field");
				if (vp->GetParameter(DB_DBLUR, data) && data.GetType() == DA_REAL)
					printf(" (Distance Blur = %f)", 100*data.GetFloat());
				break;
			case VPglobalillumination:
				printf("Global Illumination");
				if (vp->GetParameter(GI_SETUP_DATA_EXTRA_REFLECTIVECAUSTICS, data) && data.GetType() == DA_LONG)
					printf(" (Reflective Caustics = %s)", data.GetInt32() ? "true" : "false");
				if (vp->GetParameter(GI_SETUP_DATA_PRESETS, data) && data.GetType() == DA_LONG)
					printf(" (GI Preset = %d)", data.GetInt32());
				break;
			case VPopticsuite_glow:
				printf("Glow");
				if (vp->GetParameter(GW_LUM, data) && data.GetType() == DA_REAL)
					printf(" (Luminosity = %f)", 100*data.GetFloat());
				break;
			case VPhair:
				printf("Hair");
				if (vp->GetParameter(HAIR_RENDER_SHADOW_DIST_ACCURACY, data) && data.GetType() == DA_REAL)
					printf(" (Depth Threshold = %f)", 100*data.GetFloat());
				break;
			case VPopticsuite_highlights:
				printf("Highlights");
				if (vp->GetParameter(HLIGHT_SIZE, data) && data.GetType() == DA_REAL)
					printf(" (Flare Size = %f)", 100*data.GetFloat());
				break;
			case VPlenseffects:
				printf("Lens effects");
				break;
			case VPmedianfilter:
				printf("Median filter");
				if (vp->GetParameter(VP_MEDIANFILTERSTRENGTH, data) && data.GetType() == DA_REAL)
					printf(" (Strength = %f)", 100*data.GetFloat());
				break;
			case VPobjectglow:
				printf("Object glow");
				break;
			case VPobjectmotionblur:
				printf("Object motion blur");
				if (vp->GetParameter(VP_OMBSTRENGTH, data) && data.GetType() == DA_REAL)
					printf(" (Strength = %f)", 100*data.GetFloat());
				break;
			case VPsharpenfilter:
				printf("Sharpen filter");
				if (vp->GetParameter(VP_SHARPENFILTERSTRENGTH, data) && data.GetType() == DA_REAL)
					printf(" (Strength = %f)", 100*data.GetFloat());
				break;
			case VPscenemotionblur:
				printf("Scene motion blur");
				if (vp->GetParameter(VP_SMBDITHER, data) && data.GetType() == DA_REAL)
					printf(" (Dithering = %f)", 100*data.GetFloat());
				vp->SetParameter(VP_SMBDITHER, 12.3*0.01); // set test
				break;
			case VPremote:
				printf("Remote");
				if (vp->GetParameter(VP_REMOTEPATH, data) && data.GetType() == DA_FILENAME)
				{
					Char *tmp = data.GetFilename().GetString().GetCStringCopy();
					printf(" (Ext. Appl. = \'%s\')", tmp);
					DeleteMem(tmp);
				}
				break;
			case VPsketch:
				printf("Sketch & Toon");
				if (vp->GetParameter(OUTLINEMAT_LINE_INTERSECTION, data) && data.GetType() == DA_LONG)
					printf(" (Intersections = %s)", data.GetInt32() ? "true" : "false");
				break;
			case VPsoftfilter:
				printf("Soft filter");
				if (vp->GetParameter(VP_SOFTFILTERSTRENGTH, data) && data.GetType() == DA_REAL)
					printf(" (Strength = %f)", 100*data.GetFloat());
				break;
			case VPvectormotionblur:
				printf("Vector motion blur");
				if (vp->GetParameter(MBLUR_SAMPLES, data) && data.GetType() == DA_LONG)
					printf(" (Samples = %d)", (int) data.GetInt32());
				break;
			case VPToneMapping:
				printf("Tome-Mapper");
				break;
			default:
				printf("Unknown ID: %d", vp->GetType());
				break;
		}
		printf("\n");

		vp = (BaseVideoPost*)vp->GetNext();
	}

	printf("\n");
}

// prints animation track and key infos to the console
static void PrintAnimInfo(BaseList2D* bl)
{
	if (!bl || !bl->GetFirstCTrack())
		return;

	printf("\n   # Animation Info #");

	Int32 tn = 0;
	CTrack* ct = bl->GetFirstCTrack();
	while (ct)
	{
		// CTrack name
		Char* pChar = ct->GetName().GetCStringCopy();
		if (pChar)
		{
			printf("\n   %d. CTrack \"%s\"!\n", (int)++tn, pChar);
			DeleteMem(pChar);
		}
		else
			printf("\n   %d. CTrack !\n", (int)++tn);

		// time track
		CTrack* tt = ct->GetTimeTrack(bl->GetDocument());
		if (tt)
		{
			printf("\n   -> has TimeTrack !\n");
			CCurve* tcc = ct->GetCurve();
			if (tcc)
			{
				printf("    Has CCurve with %d Keys\n", (int)tcc->GetKeyCount());

				CKey* ck = nullptr;
				BaseTime t;
				for (Int32 k = 0; k < tcc->GetKeyCount(); k++)
				{
					ck = tcc->GetKey(k);
					t = ck->GetTime();
					printf("     %d. Key : %d - %f\n", (int)k + 1, (int)t.GetFrame(25), ck->GetValue());
				}
			}
		}

		// get DescLevel id
		DescID testID = ct->GetDescriptionID();
		DescLevel lv = testID[0];
		printf("   DescID->DescLevel->ID: %d\n", (int)lv.id);

		// CTrack type
		switch (ct->GetTrackCategory())
		{
		case PSEUDO_VALUE:
			printf("   VALUE - Track found!\n");
			break;

		case PSEUDO_DATA:
			printf("   DATA - Track found!\n");
			break;

		case PSEUDO_PLUGIN:
			if (ct->GetType() == CTpla)
			{
				printf("   PLA - Track found!\n");

				CCurve* cc = ct->GetCurve();
				if (cc && cc->GetKeyCount() > 0)
				{
					// get first key frame
					CKey* kk = cc->GetKey(0);
					if (!kk)
						return;

					// get time and calculate frame
					BaseTime time = kk->GetTime();
					Int32 frame = time.GetFrame(bl->GetDocument()->GetFps());

					// get pla custom data
					GeData plaData;
					kk->GetParameter(DescLevel(CK_PLA_DATA, CUSTOMDATATYPE_PLA, 0), plaData);
					PLAData* pla = (PLAData*)plaData.GetCustomDataType(CUSTOMDATATYPE_PLA);
					if (!pla)
						return;

					// get tags from pla data
					PointTag* pTag = nullptr;
					TangentTag* tTag = nullptr;

					pla->GetVariableTags(pTag, tTag);

					// get points from point tag
					if (pTag)
					{
						Int32 pointCount = pTag->GetCount();
						Vector* pAdr = pTag->GetPointAdr();

						if (pointCount <= 0 || pAdr == nullptr)
							return;

						// get first point
						Vector point = pAdr[0];
						printf("    1. Point of Key 1: %f %f %f\n", point.x, point.y, point.z);
					}
				}

			}
			else if (ct->GetType() == CTdynamicspline)
				printf("   Dynamic Spline Data - Track found!\n");
			else if (ct->GetType() == CTmorph)
				printf("   MORPH - Track found!\n");
			else
				printf("   unknown PLUGIN - Track found!\n");
			break;

		case PSEUDO_UNDEF:
		default:
			printf("   UNDEFINDED - Track found!\n");
		}

		// get CCurve and print key frame data
		CCurve* cc = ct->GetCurve();
		if (cc)
		{
			printf("   Has CCurve with %d Keys\n", (int)cc->GetKeyCount());

			CKey* ck = nullptr;
			BaseTime t;
			for (Int32 k = 0; k < cc->GetKeyCount(); k++)
			{
				ck = cc->GetKey(k);

				t = ck->GetTime();
				if (ct->GetTrackCategory() == PSEUDO_VALUE)
				{
					printf("    %d. Key : %d - Value (Float): %f\n", (int)k + 1, (int)t.GetFrame(25), ck->GetValue());
				}
				else if (ct->GetTrackCategory() == PSEUDO_DATA)
				{
					GeData data = ck->GetGeData();

					if (data.GetType() == DA_LONG)
					{
						printf("    %d. Key : %d - Data Long: %d\n", (int)k + 1, (int)t.GetFrame(25), data.GetInt32());
					}
					else if (data.GetType() == DA_REAL)
					{
						printf("    %d. Key : %d - Data Float: %f\n", (int)k + 1, (int)t.GetFrame(25), data.GetFloat());
					}
				}
				else if (ct->GetTrackCategory() == PSEUDO_PLUGIN && ct->GetType() == CTpla)
				{
					GeData ptData;
					printf("    %d. Key : %d - ", (int)k + 1, (int)t.GetFrame(25));

					// bias
					if (ck->GetParameter(CK_PLA_BIAS, ptData) && ptData.GetType() == DA_REAL)
						printf("Bias = %.2f - ", ptData.GetFloat());

					// smooth
					if (ck->GetParameter(CK_PLA_CUBIC, ptData) && ptData.GetType() == DA_LONG)
						printf("Smooth = %d - ", (int)ptData.GetInt32());

					// pla data
					if (ck->GetParameter(CK_PLA_DATA, ptData))
					{
						PLAData* plaData = (PLAData*)ptData.GetCustomDataType(CUSTOMDATATYPE_PLA);
						PointTag* poiTag;
						TangentTag* tanTag;
						plaData->GetVariableTags(poiTag, tanTag);
						if (poiTag && poiTag->GetCount() > 0)
						{
							Vector* a = poiTag->GetPointAdr();
							// print values for first point only
							printf("%.3f / %.3f / %.3f", a[0].x, a[0].y, a[0].z);
						}
						else
							printf("no points?");
					}

					printf("\n");
				}
				else if (ct->GetTrackCategory() == PSEUDO_PLUGIN && ct->GetType() == CTmorph)
				{
					GeData mtData;
					printf("    %d. Key : %d - ", (int)k + 1, (int)t.GetFrame(25));

					// bias
					if (ck->GetParameter(CK_MORPH_BIAS, mtData) && mtData.GetType() == DA_REAL)
						printf("Bias = %.2f - ", mtData.GetFloat());

					// smooth
					if (ck->GetParameter(CK_MORPH_CUBIC, mtData) && mtData.GetType() == DA_LONG)
						printf("Smooth = %d - ", (int)mtData.GetInt32());

					// link to target object
					if (ck->GetParameter(CK_MORPH_LINK, mtData))
					{
						BaseObject* targetObject = (BaseObject*)mtData.GetLink();
						if (targetObject)
						{
							Char* pTargetChar = targetObject->GetName().GetCStringCopy();
							if (pTargetChar)
							{
								printf("Target Object = %s", pTargetChar);
								DeleteMem(pTargetChar);
							}
							else
								printf("no target object name");
						}
						else
							printf("no target object defined...");
					}

					printf("\n");
				}
			} // for

		}
		ct = ct->GetNext();
	}
	printf("\n");
}

// print matrix data to the console
static void PrintMatrix(Matrix m)
{
	printf("   - Matrix:");
	Int32 size = 6;
	Float f = m.v1.x;
	if (f == 0.0)
		f = 0.0;
	if (f<0.0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v1.y;
	if (f == 0.0)
		f = 0.0;
	if (f<0.0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v1.z;
	if (f == 0.0)
		f = 0.0;
	if (f<0.0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f\n",f);

	printf("           :");
	size = 6;
	f = m.v2.x;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v2.y;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v2.z;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f\n",f);

	printf("           :");
	size = 6;
	f = m.v3.x;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v3.y;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.v3.z;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f\n",f);

	printf("           :");
	size = 6;
	f = m.off.x;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.off.y;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f",f);
	size = 6;
	f = m.off.z;
	if (f == 0.0)
		f = 0.0;
	if (f<0)
		size--;
	if (f >= 10.0 || f <= -10.0)
		size--;
	if (f >= 100.0 || f <= -100.0)
		size--;
	if (f >= 1000.0 || f <= -1000.0)
		size--;
	if (f >= 10000.0 || f <= -10000.0)
		size--;
	for (Int32 s=0; s<size; s++) printf(" ");
	printf("%f\n",f);
}

// example function to get the load progress
inline void myLoadProgressFunction(Int32 status, void *udata)
{
	Int32 test = status;	// read percentage of the file (0 - 100)
	printf("%d",(int)test);
	printf("%%");
	printf(".");
}

// example function to get the save progress
inline void mySaveProgressFunction(Int32 status, void *udata)
{
	Int32 test = status;	// actual written file size in bytes
	printf("%d",(int)test);
	printf(".");
}

// loads the scene with name <fn> and if cache exist saves it as <fnback>
static Bool LoadSaveC4DScene(const char *fn, const char *fnback)
{
	if (!fn)
		return false;

	// alloc C4D document and file
	AlienBaseDocument *C4Ddoc = NewObj(AlienBaseDocument);
	HyperFile *C4Dfile = NewObj(HyperFile);

	if (!C4Ddoc || !C4Dfile)
	{
		DeleteObj(C4Ddoc);
		DeleteObj(C4Dfile);
		return false;
	}

	// set a callback function for load progress
	//C4Dfile->SetLoadStatusCallback(myLoadProgressFunction,nullptr);

	printf("\n # Dokument #\n");
	printf(" - File: \"%s\"", fn);

	// check if the file exists
	if (!GeFExist(fn))
	{
		printf("\n   file is not existing\n   aborting...");

		DeleteObj(C4Ddoc);
		DeleteObj(C4Dfile);

		return false;
	}

	LocalFileTime mt;
	if (GeGetFileTime(Filename(fn), GE_FILETIME_CREATED, &mt))
		printf("\n   + created    : %02d/%02d/%d", mt.month, mt.day, mt.year);
	if (GeGetFileTime(Filename(fn), GE_FILETIME_MODIFIED, &mt))
		printf("\n   + modified   : %02d/%02d/%d", mt.month, mt.day, mt.year);

	// do we know the file type ?
	if (GeIdentifyFile(fn) != IDENTIFYFILE_SCENE)
	{
		printf("\n   + identified : unknown file type\n   aborting...");

		DeleteObj(C4Ddoc);
		DeleteObj(C4Dfile);

		return false;
	}
	else
		printf("\n   + identified : Cinema4D Project File\n");

	// open the file for read
	// If you do not need file informations you can just use LoadDocument() without the need of a hyperfile !
	if (C4Dfile->Open(DOC_IDENT, fn, FILEOPEN_READ))
	{
		// read all chunks
		if (C4Ddoc->ReadObject(C4Dfile, true))
			printf("\n   Read scene: SUCCEEDED (%d/%d bytes - fileversion: %d)\n", (int)C4Dfile->GetPosition(),(int)C4Dfile->GetLength(),(int)C4Dfile->GetFileVersion());
		else
			printf("\n   Read scene: FAILED (Error:%d) (%d/%d bytes - fileversion: %d)\n",(int)C4Dfile->GetError(), (int)C4Dfile->GetPosition(),(int)C4Dfile->GetLength(),(int)C4Dfile->GetFileVersion());
	}
	else
	{
		// failed to open
		if (C4Dfile->GetError() == FILEERROR_WRONG_VALUE)
			printf("\n   Read scene: FAILED (Error:%d)- unknown filetype or version:\n               \"%s\"\n", (int)C4Dfile->GetError(), fn);
		else
			printf("\n   Read scene: FAILED (Error:%d)- Could not open file \"%s\"\n", (int)C4Dfile->GetError(), fn);

		DeleteObj(C4Ddoc);
		DeleteObj(C4Dfile);

		return false;
	}

	// close the file
	C4Dfile->Close();

	// free the file
	DeleteObj(C4Dfile);

	// checks for existing polygon caches (the option you can find in cinema Preferences->File->Option "Save Poylgons for Cineware")
	if (C4Ddoc->HasCaches())
		printf("   Has caches: true\n");
	else
		printf("   Has caches: false\n");

	// calls all Execute() functions of the document (and prints scene information which is for demonstration purposes only)
	C4Ddoc->CreateSceneFromC4D();

	// if we have a fnback name save the file
	if (fnback)
	{
		// if we have a preview image we can access and save it
		if (C4Ddoc->GetDocPreviewBitmap().GetBw() > 0)
		{
			Filename fnimage(fnback);
			fnimage.SetSuffix("jpg");
			C4Ddoc->GetDocPreviewBitmap().Save(fnimage, FILTER_JPG, nullptr, SAVEBIT_0);
		}

		// create new file for export
		HyperFile *newC4Dfile = NewObj(HyperFile);
		if (!newC4Dfile)
		{
			DeleteObj(C4Ddoc);
			return false;
		}

		// open file for write
		if (!newC4Dfile->Open(DOC_IDENT, fnback, FILEOPEN_WRITE))
		{
			DeleteObj(C4Ddoc);
			DeleteObj(newC4Dfile);
			return false;
		}

		// set a callback function for save progress
		//newC4Dfile->SetSaveStatusCallback(mySaveProgressFunction,nullptr);

		// create new C4D base document (for export we don't need a alien document)
		BaseDocument *newC4DDoc = BaseDocument::Alloc();
		if (!newC4DDoc)
		{
			DeleteObj(C4Ddoc);
			newC4Dfile->Close();
			DeleteObj(newC4Dfile);
			return false;
		}

		// to copy a whole C4D document you can use this CopyTo() function (which is also available for every object)
		//C4Ddoc->CopyTo( (PrivateChunk*)newC4DDoc, COPYFLAGS_0, nullptr );

		// use the just loaded C4D document as data source
		g_myInternalDoc = C4Ddoc;

		// fill the document (copy data from loaded objects to new objects)
		newC4DDoc->CreateSceneToC4D(true);

		// write the document
		newC4DDoc->Write(newC4Dfile);

		// close the C4D file
		newC4Dfile->Close();

		// clear global document pointer
		g_myInternalDoc = nullptr;

		// free the new created and exported C4D document
		BaseDocument::Free(newC4DDoc);

		// free the new C4D file
		DeleteObj(newC4Dfile);
	}

	// free the loaded C4D alien document
	DeleteObj(C4Ddoc);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// ALIEN ALLOCATION FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// allocate a layer for the root/list
LayerObject *AlienRootLayer::AllocObject()
{
	LayerObject *baselayer = nullptr;
	baselayer = NewObj(AlienLayer);
	return baselayer;
}

// allocate a material for the root/list
BaseMaterial *AlienRootMaterial::AllocObject(Int32 Mtype)
{
	BaseMaterial *basematerial = nullptr;

	if (Mtype == Mplugin)
		basematerial = NewObj(AlienBaseMaterial, Mplugin);
	else
		basematerial = NewObj(AlienMaterial);

	return basematerial;
}

// allocate a renderdata for the root/list
RenderData *AlienRootRenderData::AllocObject()
{
	RenderData *baserenderdata = nullptr;
	baserenderdata = NewObj(AlienRenderData);
	return baserenderdata;
}

// allocate a object/element for the root/list (this is fileformat related, to support/load all objects you should not change this function)
BaseObject *AlienRootObject::AllocObject(Int32 id)
{
	BaseObject *baseobject = nullptr;

	switch (id)
	{
		case Oline:
		// SWITCH_FALLTHROUGH
		case Ospline:
			baseobject = (BaseObject*) NewObj(AlienSplineObject);
			if (baseobject)
			{
				// spline/line objects are a special case because these are no plugin objects
				// we need to alloc point data and link Node with Data manually
				NodeData* nData = NewObj(PointObjectData);
				if (nData)
				{
					((AlienSplineObject*)baseobject)->SetNodeData(nData);
					nData->SetNode((AlienSplineObject*)baseobject);
				}
				else
				{
					// delete object to return NULL in error case
					BaseObject::Free(baseobject);
				}
			}
			break;

		// this works for Oplugin, Opluginpolygon where the ID is read later in the PLUGINLAYER chunk
		// also try this for older non plugin objects JU100330
		default:
			baseobject = BaseObject::Alloc(id);
	}

	return baseobject;
}

// allocate the list/root elements itself
RootMaterial *AllocAlienRootMaterial()
{
	return NewObj(AlienRootMaterial);
}
RootObject *AllocAlienRootObject()
{
	return NewObj(AlienRootObject);
}
RootLayer *AllocAlienRootLayer()
{
	return NewObj(AlienRootLayer);
}
RootRenderData *AllocAlienRootRenderData()
{
	return NewObj(AlienRootRenderData);
}
RootViewPanel *AllocC4DRootViewPanel()
{
	return NewObj(RootViewPanel);
}
LayerObject *AllocAlienLayer()
{
	return NewObj(AlienLayer);
}

// allocate the scene objects (primitives, camera, light, etc.)
// here you can remove all the objects you don't want to support
NodeData *AllocAlienObjectData(Int32 id, Bool &known, BaseList2D* node)
{
	NodeData *m_data = nullptr;
	known = true;
	switch (id)
	{
		// supported element types
		case Ocamera:
			m_data = NewObj(AlienCameraObjectData);
			break;
		case Olight:
			m_data = NewObj(AlienLightObjectData);
			break;
		case Opolygon:
			m_data = NewObj(AlienPolygonObjectData);
			break;
		case Olod:
			m_data = NewObj(AlienLodObjectData);
			break;
		case Ovolumebuilder:
			m_data = NewObj(AlienVolumeBuilderData);
			break;
		case Ovolumeset:
			m_data = NewObj(VolumeSetData);
			break;
		case ID_MOTIONFRACTUREVORONOI:
			m_data = NewObj(MoGraphFractureVoronoiObject);
			break;
		case Ovoronoipointgenerator:
			m_data = NewObj(PointGenerator);
			break;

		case Osphere:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ocube:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Oplane:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ocone:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Otorus:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ocylinder:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Opyramid:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Oplatonic:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Odisc:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Otube:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ofigure:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ofractal:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ocapsule:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Ooiltank:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Orelief:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;
		case Osinglepoly:
			m_data = NewObj(AlienPrimitiveObjectData, id);
			break;

		// deformer
		case Obend:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Otwist:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Obulge:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Oshear:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Otaper:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Obone:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Oformula:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Owind:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Oexplosion:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Oexplosionfx:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Omelt:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Oshatter:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Owinddeform:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Opolyreduction:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Ospherify:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Osplinedeformer:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;
		case Osplinerail:
			m_data = NewObj(AlienDeformerObjectData, id);
			break;

		case Offd:
			m_data = NewObj(AlienFFDObjectData);
			break;

		case Onull:
			m_data = NewObj(AlienNullObjectData);
			break;
		case Ofloor:
			m_data = NewObj(AlienFloorObjectData);
			break;
		case Oforeground:
			m_data = NewObj(AlienForegroundObjectData);
			break;
		case Obackground:
			m_data = NewObj(AlienBackgroundObjectData);
			break;
		case Osky:
			m_data = NewObj(AlienSkyObjectData);
			break;
		case Oenvironment:
			m_data = NewObj(AlienEnvironmentObjectData);
			break;
		case Oinstance:
			m_data = NewObj(AlienInstanceObjectData);
			break;
		case Oboole:
			m_data = NewObj(AlienBoolObjectData);
			break;
		case Oextrude:
			m_data = NewObj(AlienExtrudeObjectData);
			break;

		case Oxref:
			m_data = NewObj(AlienXRefObjectData);
			break;
		case SKY_II_SKY_OBJECT:
			m_data = NewObj(AlienSkyShaderObjectData);
			break;
		case Ocloud:
			m_data = NewObj(CloudData);
			break;
		case Ocloudgroup:
			m_data = NewObj(CloudGroupData);
			break;
		case Ojoint:
			m_data = NewObj(AlienCAJointObjectData);
			break;
		case Oskin:
			m_data = NewObj(AlienCASkinObjectData);
			break;
		case CA_MESH_DEFORMER_OBJECT_ID:
			m_data = NewObj(AlienCAMeshDeformerObjectData);
			break;
		default:
			known = false;
			break;
	}

	return m_data;
}

// allocate the plugin tag elements data (only few tags have additional data which is stored in a NodeData)
NodeData *AllocAlienTagData(Int32 id, Bool &known, BaseList2D* node)
{
	NodeData *m_data = nullptr;
	known = true;
	switch (id)
	{
		// supported plugin tag types
		case Tweights:
			m_data = cineware::WeightTagData::Alloc();
			break;
		case Thairlight:
			m_data = cineware::HairLightTagData::Alloc();
			break;
		case Tsds:
			m_data = cineware::HNWeightTagData::Alloc();
			break;
		case Texpresso:
			m_data = cineware::GvExpressionData::Alloc();
			break;
		default:
			known = false;
			break;
	}

	return m_data;
}

//
//----------------------------------------------------------------------------------------
/// Allocates the shader types.
/// @param[in] id									The shader data ID to allocate.
/// @param[out] known							Assign @formatConstant{false} to tell @CINEWARESDK the shader data ID is known.
/// @return												The allocated shader node data.
//----------------------------------------------------------------------------------------
NodeData* AllocAlienShaderData(cineware::Int32 id, cineware::Bool& known, BaseList2D* node)
{
	NodeData* m_data = nullptr;
	known = true;
	switch (id)
	{
		case Xvariation:
			m_data = VariationShaderData::Alloc();
			break;
		default:
			known = false;
			break;
	}

	return m_data;
}

//////////////////////////////////////////////////////////////////////////
// IMPORT Example
// the following are the definitions of the "Alien" EXECUTE functions
//////////////////////////////////////////////////////////////////////////

// this Execute function will be called first after calling CreateSceneFromC4D()
// e.g. this can be used to handle document informations, units, renderdata or fps as shown below
Bool AlienBaseDocument::Execute()
{
	// print document informations creator name, write name, author, copyright text, date created and saved
	Char *s = m_bc.GetString(DOCUMENT_INFO_PRGCREATOR_NAME).GetCStringCopy();
	printf(" - Creator (Program): %s %d\n",(s?s:""),(int)m_bc.GetInt32(DOCUMENT_INFO_PRGCREATOR_ID));
	DeleteMem(s);

	s = m_bc.GetString(DOCUMENT_INFO_PRGWRITER_NAME).GetCStringCopy();
	printf("   Writer  (Program): %s %d\n",(s?s:""),(int)m_bc.GetInt32(DOCUMENT_INFO_PRGWRITER_ID));
	DeleteMem(s);

	s = m_bc.GetString(DOCUMENT_INFO_AUTHOR).GetCStringCopy();
	printf("   Author           : %s\n",(s?s:""));
	DeleteMem(s);

	s = m_bc.GetString(DOCUMENT_INFO_COPYRIGHT).GetCStringCopy();
	printf("   Copyright        : %s\n",(s?s:""));
	DeleteMem(s);

	Int64 _datecreated = m_bc.GetInt64(DOCUMENT_INFO_DATECREATED);
	if (_datecreated)
	{
		Float64 datecreated = *(Float64*)&_datecreated;
		if (datecreated)
		{
			DateTime local, dc = FromJulianDay(datecreated);
			if (GMTimeToLocal(dc, local))
			{
				String ss = FormatTime("%Y/%m/%d %H:%M:%S",local);
				s = ss.GetCStringCopy();
			}
		}
	}
	printf("   Date Created     : %s\n",(s?s:""));
	DeleteMem(s);

	_datecreated = m_bc.GetInt64(DOCUMENT_INFO_DATESAVED);
	if (_datecreated)
	{
		Float64 datecreated = *(Float64*)&_datecreated;
		if (datecreated)
		{
			DateTime local, dc = FromJulianDay(datecreated);
			if (GMTimeToLocal(dc, local))
			{
				String ss = FormatTime("%Y/%m/%d %H:%M:%S",local);
				s = ss.GetCStringCopy();
			}
		}
	}
	printf("   Date Saved       : %s\n",(s?s:""));
	DeleteMem(s);

	// getting unit and scale
	GeData data;
	if (GetParameter(DOCUMENT_DOCUNIT, data) && data.GetType() == CUSTOMDATATYPE_UNITSCALE)
	{
		UnitScaleData *ud = (UnitScaleData*)data.GetCustomDataType(CUSTOMDATATYPE_UNITSCALE);
		DOCUMENT_UNIT docUnit;
		Float docScale;
		ud->GetUnitScale(docScale, docUnit);
		printf(" - Unitscale: %f / %d\n", docScale, docUnit);
	}

	// print render data information
	PrintRenderDataInfo(GetFirstRenderData());

	// how to get fps, starttime, endtime, startframe and endframe
	GeData mydata;
	Float start_time = 0.0, end_time = 0.0;
	Int32 start_frame = 0, end_frame = 0, fps = 0;
	// get fps
	if (GetParameter(DOCUMENT_FPS, mydata))
		fps = mydata.GetInt32();
	// get start and end time
	if (GetParameter(DOCUMENT_MINTIME, mydata))
		start_time = mydata.GetTime().Get();
	if (GetParameter(DOCUMENT_MAXTIME, mydata))
		end_time = mydata.GetTime().Get();

	// calculate frames
	start_frame = start_time	* fps;
	end_frame		= end_time		* fps;
	printf(" - FPS: %d / %d - %d\n", (int)fps, (int)start_frame, (int)end_frame);

	return true;
}

// Execute function for the self defined Deformer objects
Bool AlienDeformerObjectData::Execute()
{
	Char *pChar = GetNode()->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienDeformerObjectData (%s): \"%s\"\n", GetObjectTypeName(GetDeformerType()), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienDeformerObjectData (%s): <noname>\n", GetObjectTypeName(GetDeformerType()));

	PrintUniqueIDs(this);

	return false;
}

// Execute function for the self defined FFD object
Bool AlienFFDObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienFFDObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienFFDObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return false;
}

// Execute function for the self defined Null object
Bool AlienNullObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienNullObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienNullObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	Matrix m = op->GetMg();

	PrintMatrix(m);

	PrintTagInfo(op);

	return true;
}

// Execute function for the self defined Foreground object
Bool AlienForegroundObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienForegroundObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienForegroundObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Background object
Bool AlienBackgroundObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienBackgroundObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienBackgroundObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Floor object
Bool AlienFloorObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienFloorObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienFloorObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Sky object
Bool AlienSkyObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienSkyObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienSkyObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Sky shader object
Bool AlienSkyShaderObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienSkyShaderObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienSkyShaderObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined CAJoint object
Bool AlienCAJointObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienCAJointObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienCAJointObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined CASkin object
Bool AlienCASkinObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienCASkinObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienCASkinObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined MeshDeformer object
Bool AlienCAMeshDeformerObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienCAMeshDeformerObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienCAMeshDeformerObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	GeData memUsage;
	if (op->GetParameter(ID_CA_MESH_DEFORMER_OBJECT_MEM, memUsage))
	{
		Char* str = memUsage.GetString().GetCStringCopy();
		if (str)
		{
			printf("\n     Memory Usage in Bytes : (%s)\n", str);
			DeleteMem(str);
		}
	}

	return true;
}
// Execute function for the self defined Environment object
Bool AlienEnvironmentObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienEnvironmentObjectData (%d): \"%s\"", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienEnvironmentObjectData (%d): <noname>", (int)op->GetType());

	GeData data;
	Vector col = Vector(0.0, 0.0, 0.0);
	if (op->GetParameter(ENVIRONMENT_AMBIENT, data))
	{
		col = data.GetVector();
		printf(" - Color: %d / %d / %d", (int)(col.x*255),(int)(col.y*255),(int)(col.z*255));
	}
	printf("\n");

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Environment object
Bool AlienBoolObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienBoolObjectData (%d): \"%s\"", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienBoolObjectData (%d): <noname>", (int)op->GetType());

	GeData data;
	Int32 boolType = 0;
	if (op->GetParameter(BOOLEOBJECT_TYPE, data))
	{
		boolType = data.GetInt32();
		printf(" - Boolean Type: %d", (int)boolType);
	}

	printf("\n");

	PrintUniqueIDs(this);

	// if you KNOW a "bool object" you have to handle the necessary children by yourself (this is for all generator object!)
	/*
	// you have to delete the control bit of the 2 first children to get a execute() call of these objects!
	BaseObject *op1 = op ? op->GetDown() : nullptr;
	BaseObject *op2 = op1 ? op1->GetNext() : nullptr;
	if (op1) op1->DelBit(BIT_CONTROLOBJECT);
	if (op2) op2->DelBit(BIT_CONTROLOBJECT);
	return true;
	*/

	// returning false means we couldn't tranform the object to our own objects
	// we will be called again in AlienPolygonObjectData::Execute() to get the same objects as mesh (only if the scene was written with polygon caches of course)
	return false;
}

// Execute function for the self defined Environment object
Bool AlienExtrudeObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienExtrudeObjectData (%d): %s\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienExtrudeObjectData (%d): <noname>\n", (int)op->GetType());

	GeData data;
	Vector movement = Vector(0.0, 0.0, 0.0);
	if (op->GetParameter(EXTRUDEOBJECT_MOVE, data))
		movement = data.GetVector();
	printf("\n   - GetMovement(): %.2f / %.2f / %.2f \n", movement.x, movement.y, movement.z);

	Int32 endSteps = 0;
	if (op->GetParameter(CAP_ENDSTEPS, data))
		endSteps = data.GetInt32();
	printf("\n   - GetEndCapSteps(): %d \n", (int)endSteps);

	PrintUniqueIDs(this);

	// returning false means we couldn't tranform the object to our own objects
	// we will be called again in AlienPolygonObjectData::Execute() to get the same objects as mesh (only if the scene was written with polygon caches of course)
	return false;
}

// Execute function for the self defined instance object
Bool AlienInstanceObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienInstanceObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienInstanceObjectData (%d): <noname>\n", (int)op->GetType());

	Matrix m = op->GetMg();
	PrintMatrix(m);

	GeData data;
	if (op->GetParameter(ID_BASEOBJECT_USECOLOR, data) && data.GetInt32())
		printf("   - UseDisplayColor: ON (Automatic/Layer...)\n");
	else
		printf("   - UseDisplayColor: OFF\n");

	// detect the source (linked) object
	BaseObject* sourceObj = nullptr;
	if (op->GetParameter(INSTANCEOBJECT_LINK, data))
		sourceObj = (BaseObject*)data.GetLink();
	if (sourceObj)
	{
		pChar = sourceObj->GetName().GetCStringCopy();
		if (pChar)
		{
			printf("   - Source: %s", pChar);
			DeleteMem(pChar);
		}
		else
			printf("   - Source: <noname>");
	}
	printf("\n");

	PrintUniqueIDs(this);

	PrintTagInfo((BaseObject*)GetNode());

	return true;
}

// Execute function for the self defined XRef object
Bool AlienXRefObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienXRefObjectData (%d): %s\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienXRefObjectData (%d): <noname>\n", (int)op->GetType());

	GeData data;
	if (op->GetParameter(1000, data) && data.GetType() == DA_FILENAME)
	{
		pChar = data.GetFilename().GetString().GetCStringCopy();
		if (pChar)
		{
			printf("   - external scene: %s\n", pChar);
			DeleteMem(pChar);
		}
	}

	PrintUniqueIDs(this);

	return true;
}

// Execute function for the self defined Polygon object
Bool AlienPolygonObjectData::Execute()
{
	// get object pointer
	PolygonObject *op = (PolygonObject*)GetNode();

	GeData data;

	// get point and polygon array pointer and counts
	const Vector *vAdr = op->GetPointR();
	Int32 pointcnt = op->GetPointCount();
	const CPolygon *polyAdr = op->GetPolygonR();
	Int32 polycnt = op->GetPolygonCount();

	// get name of object as string copy (free it after usage!)
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienPolygonObjectData (%d): %s\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienPolygonObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	printf("   - PointCount: %d PolygonCount: %d\n", (int)pointcnt, (int)polycnt);

	PrintMatrix(op->GetMg());

	PrintUserData(op);

	// polygon object with no points/polys allowed
	if (pointcnt == 0 && polycnt == 0)
		return true;

	if (!vAdr || (!polyAdr && polycnt > 0))
		return false;

	// print 4 points
	for (Int32 p = 0; p < pointcnt && p < 4; p++)
		printf("     P %d: %.1f / %.1f / %.1f\n",(int)p, vAdr[p].x, vAdr[p].y, vAdr[p].z);

	// Ngons
	Int32 ncnt = op->GetNgonCount();
	if (ncnt > 0)
	{
		printf("\n   - %d Ngons found\n", (int)ncnt);
		for (Int32 n = 0; n < ncnt && n < 3; n++) // show only 3
		{
			printf("     Ngon %d with %d Edges\n", (int)n, (int)op->GetNgonBase()->GetNgons()[n].GetCount());
			for (Int32 p = 0; p < polycnt && p < 3; p++)
			{
				Int32 polyid = op->GetNgonBase()->FindPolygon(p);
				if (polyid != NOTOK)
					printf("     Polygon %d is included in Ngon %d\n", (int)p, (int)polyid);
				else
					printf("     Polygon %d is NOT included in any Ngon\n", (int)p);
				if (p == 2)
					printf("     ...\n");
			}
			for (Int32 e = 0; e<op->GetNgonBase()->GetNgons()[n].GetCount() && e < 3; e++)
			{
				PgonEdge *pEdge = op->GetNgonBase()->GetNgons()[n].GetEdge(e);
				printf("     Edge %d: eidx: %d pid: %d sta: %d f:%d e:%d\n", (int)e, (int)pEdge->EdgeIndex(), (int)pEdge->ID(), (int)pEdge->State(), (int)pEdge->IsFirst(), (int)pEdge->IsSegmentEnd());
				if (e == 2)
					printf("     ...\n");
			}
			if (ncnt == 2)
				printf("     ...\n");
		}
		printf("\n");
	}

	// tag info
	PrintTagInfo(op);

	// detect the assigned material (it only supports the single material of the first found texture tag, there can be more materials/texture tags assigned to an object)
	BaseTag *pTex = op->GetTag(Ttexture);
	if (pTex)
	{
		AlienMaterial *pMat = nullptr;
		if (pTex->GetParameter(TEXTURETAG_MATERIAL, data))
			pMat = (AlienMaterial*)data.GetLink();
		if (pMat)
			matid = pMat->matId;
	}

	// detect the assigned layer
	AlienLayer *pLay = (AlienLayer*)op->GetLayerObject();
	if (pLay)
	{
		layid = pLay->layId;
		pChar = pLay->GetName().GetCStringCopy();
		if (pChar)
		{
			printf("   - Layer (%d): %s\n", (int)pLay->GetType(), pChar);
			DeleteMem(pChar);
		}
		else
			printf("   - Layer (%d): <noname>\n", (int)pLay->GetType());
	}

	if (op->GetFirstCTrack())
		PrintAnimInfo(this->GetNode());

	// here we return true in any case, there is no simpler description of an object than geometry
	return true;
}

// Execute function for the self defined Layer
Bool AlienLayer::Execute()
{
	Char *pChar = GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienLayer: \"%s\"", pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienLayer: <noname>");

	// access and print layer data
	GeData data;
	Bool s, m, a, g, d, e, v, r, l;
	Vector c;
	GetParameter(DescID(ID_LAYER_SOLO), data);
	s = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_MANAGER), data);
	m = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_ANIMATION), data);
	a = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_GENERATORS), data);
	g = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_DEFORMERS), data);
	d = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_EXPRESSIONS), data);
	e = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_VIEW), data);
	v = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_RENDER), data);
	r = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_LOCKED), data);
	l = (data.GetInt32() != 0);
	GetParameter(DescID(ID_LAYER_COLOR), data);
	c = data.GetVector();
	printf(" - S%d V%d R%d M%d L%d A%d G%d D%d E%d C%d/%d/%d\n",s,v,r,m,l,a,g,d,e,(int)(c.x*255.0),(int)(c.y*255.0),(int)(c.z*255.0));

	PrintUniqueIDs(this);

	// assign a id to the layer
	layId = g_tempLayID;

	g_tempLayID++;

	return true;
}

// Execute function for the self defined Camera object
Bool AlienCameraObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienCameraObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienCameraObjectData (%d): <nonema>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	PrintAnimInfo(op);

	// tags
	PrintTagInfo(op);

	PrintMatrix(op->GetMg());

	// stereo camera
	GeData camData;
	if (op->GetParameter(CAMERAOBJECT_STEREO_MODE, camData) && camData.GetInt32() != CAMERAOBJECT_STEREO_MODE_MONO)
		printf("   STEREO mode: %d \n", (int)camData.GetInt32());

	// white balance
	if (op->GetParameter(CAMERAOBJECT_WHITE_BALANCE, camData))
		printf("   White Balance Preset: %d \n", (int)camData.GetInt32());

	// target ?
	if (op->GetParameter(CAMERAOBJECT_USETARGETOBJECT, camData) && camData.GetInt32() != 0)
		printf("   Target object is used!\n");

	// vert. FOV
	if (op->GetParameter(CAMERAOBJECT_FOV_VERTICAL, camData))
		printf("   Vertical FOV: %f \n", RadToDeg(camData.GetFloat()));

	return true;
}

// Execute function for the self defined Spline object
Bool AlienSplineObject::Execute()
{
	Char *pChar = GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienSplineObject (%d): %s\n", (int)GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienSplineObject (%d): <noname>\n", (int)GetType());

	PrintUniqueIDs(this);

	PrintMatrix(GetMg());

	Int32 iType = -1;
	GeData data;
	if (GetParameter(SPLINEOBJECT_INTERPOLATION, data))
		iType = data.GetInt32();
	switch (iType)
	{
		case SPLINEOBJECT_INTERPOLATION_NONE:
			printf("   - Interpolation Type: SPLINEOBJECT_INTERPOLATION_NONE\n");
			break;
		case SPLINEOBJECT_INTERPOLATION_NATURAL:
			printf("   - Interpolation Type: SPLINEOBJECT_INTERPOLATION_NATURAL\n");
			break;
		case SPLINEOBJECT_INTERPOLATION_UNIFORM:
			printf("   - Interpolation Type: SPLINEOBJECT_INTERPOLATION_UNIFORM\n");
			break;
		case SPLINEOBJECT_INTERPOLATION_ADAPTIVE:
			printf("   - Interpolation Type: SPLINEOBJECT_INTERPOLATION_ADAPTIVE\n");
			break;
		case SPLINEOBJECT_INTERPOLATION_SUBDIV:
			printf("   - Interpolation Type: SPLINEOBJECT_INTERPOLATION_SUBDIV\n");
			break;
	}

	// spline data
	if (GetParameter(SPLINEOBJECT_TYPE, data))
		printf("   - SplineType: %d\n", (int)data.GetInt32());
	printf("   - PointCount: %d\n", (int)GetPointCount());
	printf("   - SegmentCount: %d\n", (int)GetSegmentCount());

	// tags
	PrintTagInfo(this);

	return true;
}

// Execute function for the self defined Primitive objects
Bool AlienPrimitiveObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienPrimitiveObjectData (%d): %s\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienPrimitiveObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	GeData data;
	if (this->type_id == Ocube)
	{
		if (op->GetParameter(PRIM_CUBE_LEN, data))
			printf("   - Type: Cube - Len = %f / %f / %f\n", data.GetVector().x,  data.GetVector().y, data.GetVector().z);
	}

	PrintMatrix(op->GetMg());

	// tags
	PrintTagInfo(op);

	// detect the layer
	AlienLayer *pLay = (AlienLayer*)op->GetLayerObject();
	if (pLay)
	{
		pChar = pLay->GetName().GetCStringCopy();
		if (pChar)
		{
			printf("   - Layer: \"%s\"\n", pChar);
			DeleteMem(pChar);
		}
		else
			printf("   - Layer: <noname>\n");
	}

	PrintAnimInfo(op);
	PrintUserData(op);

	return true;
}

// Execute function for the self defined light objects
Bool AlienLightObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienLightObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienLightObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	PrintMatrix(op->GetMg());

	GeData data;
	if (op->GetParameter(LIGHT_TYPE, data) && data.GetType() == DA_LONG)
		printf("\n -   Type: (%d)\n", data.GetInt32());
	else
		printf("\n -   Error getting light type !\n");

	return true;
}

// Execute function for LOD objects
Bool AlienLodObjectData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char *pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienLodObjectData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienLodObjectData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	PrintMatrix(op->GetMg());

	Float minDis = 200.0;
	Float maxDis = 500.0;

	// set min & max distances
	GeData data;
	if (op->GetParameter(LOD_CAMERA_MINDIST, data) && data.GetType() == DA_REAL)
		minDis = data.GetFloat();
	if (op->GetParameter(LOD_CAMERA_MAXDIST, data) && data.GetType() == DA_REAL)
		maxDis = data.GetFloat();
	Int32 lodMode = op->GetDataInstance()->GetInt32(LOD_MODE);
	Int32 lodCriteria = op->GetDataInstance()->GetInt32(LOD_CRITERIA);
	Int32 levelCnt = GetLevelCount();
	printf("\n   - Mode         : %d\n", lodMode);
	printf("   - Criteria     : %d\n", lodCriteria);
	printf("   - Level Count  : %d\n", levelCnt);
	printf("   - Current Level: %d\n", GetCurrentLevel());

	RangeData* lodRange = (RangeData*)op->GetDataInstance()->GetCustomDataType(LOD_BAR, CUSTOMDATATYPE_RANGE);
	if (lodRange)
	{
		printf("\n -   Ranges:\n");
		for (Int32 k = 0; k <= lodRange->GetKnotsCount(); k++)
		{
			Vector col = lodRange->GetRangeColor(k);
			Float value1 = k == 0 ? 0.0 : lodRange->GetKnotValue(k - 1);
			Float value2 = k < lodRange->GetKnotsCount() ? lodRange->GetKnotValue(k) : 1.0;
			printf("     -> Range %d - Value: %f to %f - Color: %d %d %d\n", k, minDis + value1 * (maxDis - minDis), minDis + value2 * (maxDis - minDis), Int32(col.x * 255.0), Int32(col.y * 255.0), Int32(col.z * 255.0));
		}
	}
	else
	{
		printf("\n -   Error getting LOD ranges !\n");
	}

	PrintUserData(op);

	return true;
}

// Execute function for Volume Builder objects
Bool AlienVolumeBuilderData::Execute()
{
	BaseObject* op = (BaseObject*)GetNode();
	Char*				pChar = op->GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienVolumeBuilderData (%d): \"%s\"\n", (int)op->GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienVolumeBuilderData (%d): <noname>\n", (int)op->GetType());

	PrintUniqueIDs(this);

	PrintMatrix(op->GetMg());

	Int32 volMode = op->GetDataInstanceRef().GetInt32(ID_VOLUMEBUILDER_VOLUMETYPE);
	printf("\n -   Volume Mode: %d\n", volMode);

	return true;
}

static void PrintMateriaCache(BaseContainer& bc)
{
	BrowseContainer browse(&bc);
	Int32 paramId;
	GeData* contData;

	while (browse.GetNext(&paramId, &contData))
	{
		if (contData->GetType() == DA_CONTAINER)
		{
			BaseContainer* subBc = contData->GetContainer();
			if (!subBc)
				continue;

			printf("   - (%d) ", paramId);

			BrowseContainer subBrowse(subBc);
			Int32 subParamId;
			GeData* data;

			while (subBrowse.GetNext(&subParamId, &data))
			{
				if (subParamId == CINEWARE_MATERIAL_STANDARD_SURFACE_NAME)
				{
					Char* name = data->GetString().GetCStringCopy();
					if (name)
					{
						printf("\"%s\"  ", name);
						DeleteMem(name);
					}
				}
				else // CINEWARE_MATERIAL_STANDARD_SURFACE_VALUE, CINEWARE_MATERIAL_STANDARD_SURFACE_EXTENT, CINEWARE_MATERIAL_STANDARD_SURFACE_TEXTURE
				{
					if (data->GetType() == DA_LONG)
					{
						printf("Int: %d  ", data->GetInt32());
					}
					else if (data->GetType() == DA_LLONG)
					{
						printf("Int64: %d  ", data->GetInt64());
					}
					else if (data->GetType() == DA_REAL)
					{
						printf("Float: %f  ", data->GetFloat());
					}
					else if (data->GetType() == DA_VECTOR)
					{
						printf("Vector: %f %f %f  ", data->GetVector().x, data->GetVector().y, data->GetVector().z);
					}
					else if (data->GetType() == DA_STRING)
					{
						Char* str = data->GetString().GetCStringCopy();
						if (str)
						{
							printf("String: %s  ", str);
							DeleteMem(str);
						}
					}
					else if (data->GetType() == DA_FILENAME)
					{
						Char* fn = data->GetFilename().GetString().GetCStringCopy();
						{
							printf("Filename: %s  ", fn);
							DeleteMem(fn);
						}
					}
				}
			}

			printf("\n");
		}
	}
}

// Execute function for the self defined Material
Bool AlienMaterial::Execute()
{
	Char *pChar = GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienMaterial (%d): %s\n", (int)GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienMaterial (%d): <noname>\n", (int)GetType());

	PrintUniqueIDs(this);

	// assign a id to the material
	matId = g_tempmatid;

	g_tempmatid++;

	// material preview custom data type
	GeData mData;
	if (GetParameter(MATERIAL_PREVIEW, mData))
	{
		MaterialPreviewData *mPreview = (MaterialPreviewData*)mData.GetCustomDataType(CUSTOMDATATYPE_MATPREVIEW);
		if (mPreview)
		{
			MatPreviewType mPreviewType = mPreview->GetPreviewType();
			MatPreviewSize mPreviewSize = mPreview->GetPreviewSize();
			printf("   MaterialPreview: Type:%d Size:%d\n", mPreviewType, mPreviewSize);
		}
	}

	PrintShaderInfo(GetShader(MATERIAL_COLOR_SHADER), 4);

	// print out reflectance layer data
	BaseContainer* reflectanceData = GetDataInstance();
	for (Int32 refLayIdx = 0; refLayIdx < GetReflectionLayerCount(); refLayIdx++)
	{
		ReflectionLayer* refLay = GetReflectionLayerIndex(refLayIdx);
		if (refLay)
		{
			Char* refName = refLay->GetName().GetCStringCopy();
			Int32 dataId = refLay->GetDataID();
			Int32 refType = reflectanceData->GetInt32(dataId + REFLECTION_LAYER_MAIN_DISTRIBUTION);
			printf(" - reflectance layer '%s' of type: %d\n", refName, refType);
			DeleteMem(refName);
		}
	}

	printf("   Material Cache (Standard Surface Properties):\n");
	PrintMateriaCache(GetMaterialCache());

	PrintAnimInfo(this);

	return true;
}

// Execute function for the self defined plugin material
Bool AlienBaseMaterial::Execute()
{
	Char *pChar = GetName().GetCStringCopy();
	if (pChar)
	{
		printf("\n - AlienPluginMaterial (%d): %s\n", (int)GetType(), pChar);
		DeleteMem(pChar);
	}
	else
		printf("\n - AlienPluginMaterial (%d): <noname>\n", (int)GetType());

	PrintUniqueIDs(this);

	// assign a id to the material
	matId = g_tempmatid;

	g_tempmatid++;

	switch(GetType())
	{
		case Mmaterial:
			printf("   Cinema Material\n");
			break;
		case Msketch:
			printf("   Sketch & Toon Material\n");
			break;
		case Mdanel:
			printf("   Daniel Shader Material\n");
			break;
		case Mbanji:
			printf("   Banji Shader Material\n");
			break;
		case Mbanzi:
			printf("   Banzi Shader Material\n");
			break;
		case Mcheen:
			printf("   Cheen Shader Material\n");
			break;
		case Mmabel:
			printf("   Mabel Shader Material\n");
			break;
		case Mnukei:
			printf("   Nukei Shader Material\n");
			break;
		case Xskyshader:
			printf("   Sky Material (hidden)\n");
			break;
		case Mfog:
			printf("   Fog Shader Material\n");
			break;
		case Mterrain:
			printf("   Terrain Shader Material\n");
			break;
		case Mhair:
			printf("   Hair Shader Material\n");
			break;
		case Marchigrass:
			printf("   ArchiGrass Material\n");
			{
				const GeData &shader = GetDataInstance()->GetData(GRASS_DENSITY_SHD);
				PrintShaderInfo((BaseShader*)shader.GetLink(), 4);
			}
			break;
		default:
			printf("   unknown plugin material\n");
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// EXPORT example
// the following functions are regarding EXPORT of a C4D scene file

//////////////////////////////////////////////////////////////////////////

// creates a new C4D Material and set some parameters
static Bool BuildMaterialToC4D(AlienMaterial* pOrgMat, BaseDocument* doc)
{
	if (!doc)
		return false;

	AlienMaterial	*newMaterial = nullptr;

	// Create new material
	newMaterial = NewObj(AlienMaterial);
	if (!newMaterial)
		return false;

	// Set some Material properties
	String str = pOrgMat->GetName();
	newMaterial->SetName(str);

	// we only support data of type Mmaterial here
	if (pOrgMat->GetType() == Mmaterial)
	{
		// copy color and set the color values
		GeData data;
		if (pOrgMat->GetParameter(MATERIAL_COLOR_COLOR, data))
			newMaterial->SetParameter(MATERIAL_COLOR_COLOR, data.GetVector());
		newMaterial->SetChannelState(CHANNEL_COLOR, pOrgMat->GetChannelState(CHANNEL_COLOR));

		// Get original shader
		BaseShader* orgShader = pOrgMat->GetShader(MATERIAL_COLOR_SHADER);
		if (orgShader)
		{
			// create new shader on new material
			newMaterial->MakeBitmapShader(MATERIAL_COLOR_SHADER, orgShader->GetFileName());
		}

		// copy id -> needed for reassignmet of materials in the new scene
		newMaterial->matId = pOrgMat->matId;
	}

	// add material to document
	doc->InsertMaterial(newMaterial);

	return true;
}

// go through all original Layers and create new Layers and copy the color
static Bool BuildLayerToC4D(AlienLayer* myInternalLayer, AlienLayer* parentLayer, BaseDocument* doc  )
{
	AlienLayer *newLayer = nullptr;

	// go through all layer
	while (myInternalLayer)
	{
		// create new layer
		newLayer = NewObj(AlienLayer);
		if (!newLayer)
			return false;

		// set layer color property
		newLayer->SetName(myInternalLayer->GetName());
		GeData data;
		if (myInternalLayer->GetParameter(ID_LAYER_COLOR, data))
			newLayer->SetParameter(ID_LAYER_COLOR, data);

		// copy id -> needed for reassignment of layers in the new scene
		newLayer->layId = myInternalLayer->layId;

		// add layer to C4D export scene
		doc->InsertLayer(newLayer, parentLayer);

		// also add children of the layer
		BuildLayerToC4D((AlienLayer*)myInternalLayer->GetDown(), newLayer, doc);

		// go to next layer
		myInternalLayer = (AlienLayer*)myInternalLayer->GetNext();
	}

	return true;
}

// go through all original Objects and create new Objects (only a few type are handled)
static BaseObject* CreateObjectToC4D(BaseObject* myInternalObject, BaseObject* pParent, BaseDocument* doc)
{
	if (!myInternalObject || !doc)
		return nullptr;

	BaseObject *newObject = nullptr;
	Int32 i;

	// copy spline object
	if (myInternalObject->GetType() == Ospline)
	{
		// create new spline object
		newObject = BaseObject::Alloc(Ospline);
		if (!newObject)
			return nullptr;

		SplineObject* pOrgSpline = (SplineObject*)myInternalObject;
		SplineObject* pNewSpline = (SplineObject*)newObject;

		// set some spline parameter
		GeData data;
		if (pOrgSpline->GetParameter(SPLINEOBJECT_TYPE, data))
			pNewSpline->SetParameter(SPLINEOBJECT_TYPE, data.GetInt32());
		if (pOrgSpline->GetParameter(SPLINEOBJECT_CLOSED, data))
			pNewSpline->SetParameter(SPLINEOBJECT_CLOSED, data.GetInt32());

		// copy the spline points from the original spline to the copy
		pNewSpline->ResizeObject(pOrgSpline->GetPointCount(), pOrgSpline->GetSegmentCount());

		// get array pointer
		Vector *vadrnew = pNewSpline->GetPointW();
		const Vector *vadr = pOrgSpline->GetPointR();
		if (!vadrnew || !vadr)
			return nullptr;

		// fill point array
		for (i = 0; i < pOrgSpline->GetPointCount(); i++)
		{
			vadrnew[i] = vadr[i];
		}
	}
	// create polygon object and set points & polygons
	else if (myInternalObject->GetType() == Opolygon)
	{
		// create new polygon object
		newObject = BaseObject::Alloc(Opolygon);
		if (!newObject)
			return nullptr;

		// copy the polygons and points from the original polygon object to the copy
		((PolygonObject*)newObject)->ResizeObject(((PolygonObject*)myInternalObject)->GetPointCount(), ((PolygonObject*)myInternalObject)->GetPolygonCount(), 0);

		// get point addresses
		Vector *vadrnew = ((PolygonObject*)newObject)->GetPointW();
		const Vector *vadr = ((PolygonObject*)myInternalObject)->GetPointR();
		if (!vadrnew || !vadr)
			return nullptr;

		// get polygon addresses
		CPolygon *padrnew = ((PolygonObject*)newObject)->GetPolygonW();
		const CPolygon *padr = ((PolygonObject*)myInternalObject)->GetPolygonR();
		if (!padrnew || !padr)
			return nullptr;

		// fill point array
		for (i = 0; i < ((PolygonObject*)myInternalObject)->GetPointCount(); i++)
		{
			vadrnew[i] = vadr[i];
		}

		// fill polygon array
		for (i = 0; i < ((PolygonObject*)myInternalObject)->GetPolygonCount(); i++)
		{
			padrnew[i] = padr[i];
		}

		// add texture tag and material if the internal element had one
		if (myInternalObject->GetTag(Ttexture))
		{
			TextureTag *myInternalTag = (TextureTag*)myInternalObject->GetTag(Ttexture);
			TextureTag *tNewTag = (TextureTag*)newObject->GetTag(Ttexture);

			if (!tNewTag)
			{
				tNewTag = NewObj(TextureTag);
				if (tNewTag)
					newObject->InsertTag(tNewTag);
			}

			if (!tNewTag)
				return nullptr;

			// add material
			GeData data;
			AlienMaterial *mat = nullptr;
			if (myInternalTag->GetParameter(TEXTURETAG_MATERIAL, data))
				mat = (AlienMaterial*)data.GetLink();
			if (mat)
			{
				// get mat id (stored in the data of the object)
				PolygonObjectData* pData = (PolygonObjectData*)myInternalObject->GetNodeData();
				Int32 matID = ((AlienPolygonObjectData*)pData)->matid;

				// link the material with the matid and id of the material
				AlienMaterial* pMat = (AlienMaterial*)doc->GetFirstMaterial();
				for (; pMat; pMat = (AlienMaterial*)pMat->GetNext())
				{
					if (pMat->matId == matID)
						break;
				}
				if (pMat)
				{
					BaseLink link;
					link.SetLink(pMat);
					tNewTag->SetParameter(TEXTURETAG_MATERIAL, link);
				}
			}
		}

		// add layer if the internal element had one
		if (myInternalObject->GetLayerObject())
		{
			// get mat id (stored in the data of the object)
			PolygonObjectData* pData = (PolygonObjectData*)myInternalObject->GetNodeData();
			Int32 layID = ((AlienPolygonObjectData*)pData)->layid;

			// link the layer with id of material and id of layer
			AlienLayer* pLay = (AlienLayer*)doc->GetFirstLayer();
			for (; pLay; pLay = (AlienLayer*)pLay->GetNext())
			{
				if (pLay->layId == layID)
					break;
			}
			if (pLay)
				newObject->SetLayerObject(pLay);
		}
	}
	// create light object and set color
	else if (myInternalObject->GetType() == Olight)
	{
		// create new light object
		newObject = BaseObject::Alloc(Olight);
		if (!newObject)
			return nullptr;

		GeData data;
		if (myInternalObject->GetParameter(LIGHT_COLOR, data))
			newObject->SetParameter(LIGHT_COLOR, data.GetVector());
	}
	// create camera object and set projection
	else if (myInternalObject->GetType() == Ocamera)
	{
		// create new camera object
		newObject = BaseObject::Alloc(Ocamera);
		if (!newObject)
			return nullptr;

		GeData data;
		if (myInternalObject->GetParameter(CAMERA_PROJECTION, data))
			newObject->SetParameter(CAMERA_PROJECTION, data);
	}
	// create floor object
	else if (myInternalObject->GetType() == Ofloor)
	{
		// create new floor object
		newObject = BaseObject::Alloc(Ofloor);
		if (!newObject)
			return nullptr;
	}
	// create foreground object
	else if (myInternalObject->GetType() == Oforeground)
	{
		// create new object
		newObject = BaseObject::Alloc(Oforeground);
		if (!newObject)
			return nullptr;
	}
	// create background object
	else if (myInternalObject->GetType() == Obackground)
	{
		// create new object
		newObject = BaseObject::Alloc(Obackground);
		if (!newObject)
			return nullptr;
	}
	// create sky object
	else if (myInternalObject->GetType() == Osky)
	{
		// create new object
		newObject = BaseObject::Alloc(Osky);
		if (!newObject)
			return nullptr;
	}
	// create environment object
	else if (myInternalObject->GetType() == Oenvironment)
	{
		// create new object
		newObject = BaseObject::Alloc(Oenvironment);
		if (!newObject)
			return nullptr;
	}
	// create Null object
	else if (myInternalObject->GetType() == Onull)
	{
		// create new null object
		newObject = BaseObject::Alloc(Onull);
		if (!newObject)
			return nullptr;
	}

	// we don't know the type
	if (!newObject)
		return nullptr;

	// add the new element to the scene (under the parent element)
	doc->InsertObject(newObject, pParent);

	// set the name
	newObject->SetName(myInternalObject->GetName());

	// set the object matrices
	newObject->SetMl(myInternalObject->GetMl());
	newObject->SetMg(myInternalObject->GetMg());

	return newObject;
}

// go through all elements and build the objects for export
static BaseObject* BuildObjectsToC4D(BaseObject* obj, BaseObject* pNewParent, BaseDocument* doc)
{
	BaseObject* pNewObj = nullptr;

	while (obj)
	{
		// create object
		pNewObj = CreateObjectToC4D(obj, pNewParent, doc);

		// children
		BuildObjectsToC4D((BaseObject*)obj->GetDown(), pNewObj, doc);

		// next object
		obj = (BaseObject*)obj->GetNext();
	}

	return pNewParent;
}

// create objects, material and layers for the new C4D scene file
Bool BaseDocument::CreateSceneToC4D(Bool selectedonly)
{
	// we need the pointer to the loaded C4D scene file as we don't have our own internal data for export
	if (!g_myInternalDoc)
		return false;

	// create MATERIALS
	AlienMaterial* pMat = (AlienMaterial*)g_myInternalDoc->GetFirstMaterial();
	for (pMat = (AlienMaterial*)g_myInternalDoc->GetFirstMaterial(); pMat; pMat = (AlienMaterial*)pMat->GetNext())
		if (!BuildMaterialToC4D(pMat, this))
			return false;

	// create LAYERS
	if (!BuildLayerToC4D((AlienLayer*)g_myInternalDoc->GetFirstLayer(), nullptr, this))
		return false;

	// create OBJECTS
	if (!BuildObjectsToC4D(g_myInternalDoc->GetFirstObject(), nullptr, this))
		return false;

	printf("\n scene build was successful\n");

	return true;
}


//////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
//////////////////////////////////////////////////////////////////////////

#ifdef __MAC
#include <uuid/uuid.h>
#endif

int main(int argc, Char* argv[])
{
// security check for OSX
#ifdef __MAC
	Int32 s1 = sizeof(uuid_t);
	Int32 s2 = sizeof(GeMarker);
	if (s1 != s2)
		CriticalStop();
#endif

	Char* versionStr = GetLibraryVersion().GetCStringCopy();
	printf("\n ______________________________________\n");
	printf("\n   %s Commandline Tool", versionStr);
	printf("\n ______________________________________\n");
	DeleteMem(versionStr);

  // filename for import and export C4D files
	const char *fnLoad = "cube.c4d";
	const char *fnSave = "cube_saved.c4d";

	// load and save a c4d scene file
	Bool res = LoadSaveC4DScene(fnLoad, fnSave);
	if (res && fnSave)
	{
		// load the just saved c4d file again (testing purposes only)
		printf("\n\n ##### ReRead saved file: \n\n");
		LoadSaveC4DScene(fnSave, nullptr);
	}

	// to keep the console window open wait here for input
	char value;
	printf("\n <press return to exit>");
	scanf("%c", &value);
}

//////////////////////////////////////////////////////////////////////////
