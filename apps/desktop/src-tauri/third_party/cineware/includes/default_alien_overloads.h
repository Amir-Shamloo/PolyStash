/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef DEFAULT_ALIEN_OVERLOADS_H__
#define DEFAULT_ALIEN_OVERLOADS_H__

#include "c4d.h"

#ifndef DONT_INCLUDE_MEMORY_OVERLOADS

// Default "alien" and memory function overloads.
// Use this code in your project and modify it to your purposes.
// At least make sure to overload GetWriterInfo() and return your own unique application ID.

namespace cineware
{
#pragma pack (push, 8)

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Memory allocation functions.
/// Overload MemAlloc() / MemFree() for custom memory management.

void* MemAllocNC(Int size)
{
	void* mem = malloc(size);
	return mem;
}

void* MemAlloc(Int size)
{
	void* mem = MemAllocNC(size);
	if (!mem)
		return nullptr;
	memset(mem, 0, size);
	return mem;
}

void* MemRealloc(void* orimem, Int size)
{
	void* mem = realloc(orimem, size);
	return mem;
}

void MemFree(void*& mem)
{
	if (!mem)
		return;

	free(mem);
	mem = nullptr;
}

#pragma pack (pop)
}

#endif	// DONT_INCLUDE_MEMORY_OVERLOADS

///////////////////////////////////////////////////////////////////////////////////////////////////

/// @addtogroup group_alienimplementatation Alien Alloc() Implementation
/// @ingroup group_topic Topics
/// @{

//----------------------------------------------------------------------------------------
/// Allocates the document root for the material.
/// @return												The "alien" root material.
//----------------------------------------------------------------------------------------
cineware::RootMaterial* AllocAlienRootMaterial()
{
	return NewObj(cineware::RootMaterial);
}

//----------------------------------------------------------------------------------------
/// Allocates the document root for the object.
/// @return												The "alien" root object.
//----------------------------------------------------------------------------------------
cineware::RootObject* AllocAlienRootObject()
{
	return NewObj(cineware::RootObject);
}

//----------------------------------------------------------------------------------------
/// Allocates the document root for the layer.
/// @return												The "alien" root layer.
//----------------------------------------------------------------------------------------
cineware::RootLayer* AllocAlienRootLayer()
{
	return NewObj(cineware::RootLayer);
}

//----------------------------------------------------------------------------------------
/// Allocates the document root for the render data.
/// @return												The "alien" root render data.
//----------------------------------------------------------------------------------------
cineware::RootRenderData* AllocAlienRootRenderData()
{
	return NewObj(cineware::RootRenderData);
}

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
cineware::RootViewPanel* AllocC4DRootViewPanel()
{
	return NewObj(cineware::RootViewPanel);
}

//----------------------------------------------------------------------------------------
/// Allocates a layer.
/// @return												A "alien" layer.
//----------------------------------------------------------------------------------------
cineware::LayerObject* AllocAlienLayer()
{
	return NewObj(cineware::LayerObject);
}

/// @}

///////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/// Allocates the object types.
/// @param[in] id									The object data ID to allocate.
/// @param[out] known							Assign @formatConstant{false} to tell @CINEWARESDK the object data ID is known.
/// @param[in] node								The parent node the data gets assigned to. @cinewareOwnsPointed{node} Since R19.
/// @return												The allocated object node data.
//----------------------------------------------------------------------------------------
cineware::NodeData* AllocAlienObjectData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node)
{
	cineware::NodeData* m_data = nullptr;
	known = true;
	switch (id)
	{
		case Ocamera:
			m_data = NewObj(cineware::CameraObjectData);
			break;
		case Olight:
			m_data = NewObj(cineware::LightObjectData);
			break;
		case Opolygon:
			m_data = NewObj(cineware::PolygonObjectData);
			break;
		case Olod:
			m_data = NewObj(cineware::LodObjectData);
			break;
		case Oinstance:
			m_data = NewObj(cineware::InstanceObjectData);
			break;
		case Ovolumebuilder:
			m_data = NewObj(cineware::VolumeBuilderData);
			break;
		case Ovolumeset:
			m_data = NewObj(cineware::VolumeSetData);
			break;
		case ID_MOTIONFRACTUREVORONOI:
			m_data = NewObj(cineware::MoGraphFractureVoronoiObject);
			break;
		case Ovoronoipointgenerator:
			m_data = NewObj(cineware::PointGenerator);
			break;

		case Osphere:	// Fall through all
		case Ocube:
		case Oplane:
		case Ocone:
		case Otorus:
		case Ocylinder:
		case Opyramid:
		case Oplatonic:
		case Odisc:
		case Otube:
		case Ofigure:
		case Ofractal:
		case Ocapsule:
		case Ooiltank:
		case Orelief:
		case Osinglepoly:
			m_data = NewObj(cineware::NodeData);
			break;

		case Obend:	// Fall through all
		case Otwist:
		case Obulge:
		case Oshear:
		case Otaper:
		case Obone:
		case Oformula:
		case Owind:
		case Oexplosion:
		case Oexplosionfx:
		case Omelt:
		case Oshatter:
		case Owinddeform:
		case Opolyreduction:
		case Ospherify:
		case Osplinedeformer:
		case Osplinerail:
			m_data = NewObj(cineware::NodeData);
			break;

		case Offd:
			m_data = NewObj(cineware::PointObjectData);
			break;

		case Onull:
			m_data = NewObj(cineware::NodeData);
			break;
		case Ofloor:
			m_data = NewObj(cineware::NodeData);
			break;
		case Oforeground:
			m_data = NewObj(cineware::NodeData);
			break;
		case Obackground:
			m_data = NewObj(cineware::NodeData);
			break;
		case Osky:
			m_data = NewObj(cineware::NodeData);
			break;
		case Oenvironment:
			m_data = NewObj(cineware::NodeData);
			break;
		case Oboole:
			m_data = NewObj(cineware::NodeData);
			break;
		case Oextrude:
			m_data = NewObj(cineware::NodeData);
			break;

		case Oxref:
			m_data = NewObj(cineware::NodeData);
			break;
		case SKY_II_SKY_OBJECT:
			m_data = NewObj(cineware::SkyShaderObjectData);
			break;
		case Ocloud:
			m_data = NewObj(cineware::CloudData);
			break;
		case Ocloudgroup:
			m_data = NewObj(cineware::CloudGroupData);
			break;
		case Ojoint:
			m_data = NewObj(cineware::JointObjectData);
			break;
		case Oskin:
			m_data = NewObj(cineware::SkinObjectData);
			break;
		case CA_MESH_DEFORMER_OBJECT_ID:
			m_data = NewObj(cineware::MeshDeformerObjectData);
			break;
		default:
			known = false;
			break;
	}

	return m_data;
}

//
//----------------------------------------------------------------------------------------
/// Allocates the tag types.
/// @param[in] id									The tag data ID to allocate.
/// @param[out] known							Assign @formatConstant{false} to tell @CINEWARESDK the tag data ID is known.
/// @param[in] node								The parent node the data gets assigned to. @cinewareOwnsPointed{node} Since R19.
/// @return												The allocated tag node data.
//----------------------------------------------------------------------------------------
cineware::NodeData* AllocAlienTagData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node)
{
	cineware::NodeData* m_data = nullptr;
	known = true;
	switch (id)
	{
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
		case Tposemorph:
			m_data = cineware::PoseMorphTagData::Alloc();
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
/// @param[in] node								The parent node the data gets assigned to. @cinewareOwnsPointed{node} Since R19.
/// @return												The allocated shader node data.
//----------------------------------------------------------------------------------------
cineware::NodeData* AllocAlienShaderData(cineware::Int32 id, cineware::Bool& known, cineware::BaseList2D* node)
{
	cineware::NodeData* m_data = nullptr;
	known = true;
	switch (id)
	{
		case Xvariation:
			m_data = cineware::VariationShaderData::Alloc();
			break;
		default:
			known = false;
			break;
	}

	return m_data;
}

/// @}

///////////////////////////////////////////////////////////////////////////////////////////////////

cineware::Bool cineware::BaseDocument::CreateSceneToC4D(cineware::Bool selectedOnly)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif	// DEFAULT_ALIEN_OVERLOADS_H__
