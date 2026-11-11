/////////////////////////////////////////////////////////////
// CINEWARE SDK - command line example                     //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef ALIEN_DEF_H__
#define ALIEN_DEF_H__

#include "c4d.h"

//////////////////////////////////////////////////////////////////////////
namespace cineware
{

NodeData *AllocAlienObjectData(Int32 id, Bool &known, BaseList2D* node);
NodeData *AllocAlienTagData(Int32 id, Bool &known, BaseList2D* node);

// definition of self defined element classes  (spline,null,camera,light etc.) derived from C4D classes

class AlienBaseDocument : public BaseDocument
{
public:
	virtual Bool Execute();
};

// object list/root (list which contains all scene objects)
class AlienRootObject : public RootObject
{
public:

	virtual BaseObject *AllocObject(Int32 id);
};

// layer list/root (list which contains all scene layers)
class AlienRootLayer : public RootLayer
{
	INSTANCEOF(AlienRootLayer, RootLayer)

public:

	virtual LayerObject *AllocObject();
};

// material list/root (list which contains all scene materials)
class AlienRootMaterial : public RootMaterial
{
	INSTANCEOF(AlienRootMaterial, RootMaterial);

public:

	virtual BaseMaterial *AllocObject(Int32 Mtype);
};

class AlienRootRenderData : public RootRenderData
{
	INSTANCEOF(AlienRootRenderData, RootRenderData)

public:

	virtual RenderData *AllocObject();
};

// self-defined layer with own functions and members
class AlienLayer : public LayerObject
{
	INSTANCEOF(AlienLayer, LayerObject)

public:

	Int32 layId;

	AlienLayer() : LayerObject() { }
	virtual Bool Execute();
};

// self-defined null object data with own functions and members
class AlienNullObjectData : public NodeData
{
	INSTANCEOF(AlienNullObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined foreground object data with own functions and members
class AlienForegroundObjectData : public NodeData
{
	INSTANCEOF(AlienForegroundObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined background object data with own functions and members
class AlienBackgroundObjectData : public NodeData
{
	INSTANCEOF(AlienBackgroundObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined Floor object data with own functions and members
class AlienFloorObjectData : public NodeData
{
	INSTANCEOF(AlienFloorObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined Sky object data with own functions and members
class AlienSkyObjectData : public NodeData
{
	INSTANCEOF(AlienSkyObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined Environment object data with own functions and members
class AlienEnvironmentObjectData : public NodeData
{
	INSTANCEOF(AlienEnvironmentObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined polygon object data with own functions and members
class AlienFFDObjectData : public PointObjectData
{
	INSTANCEOF(AlienFFDObjectData, PointObjectData)

public:
	virtual Bool Execute();
};

// self-defined polygon object data with own functions and members
class AlienPolygonObjectData : public PolygonObjectData
{
	INSTANCEOF(AlienPolygonObjectData, PolygonObjectData)

public:
	Int32 layid;
	Int32 matid;
	virtual Bool Execute();
};

// self-defined camera object data with own functions and members
class AlienCameraObjectData : public CameraObjectData
{
	INSTANCEOF(AlienCameraObjectData, CameraObjectData)

public:
	float horizontalFieldOfView;

	virtual Bool Execute();
};

// self-defined primitive object data with own functions and members
class AlienPrimitiveObjectData : public NodeData
{
	INSTANCEOF(AlienPrimitiveObjectData, NodeData)

	Int32 type_id;
public:
	AlienPrimitiveObjectData(Int32 id) : type_id(id) {}
	virtual Bool Execute();
};

// self-defined spline object with own functions and members
class AlienSplineObject : public SplineObject
{
	INSTANCEOF(AlienSplineObject, SplineObject)

public:

	virtual Bool Execute();
};

// self-defined light object data with own functions and members
class AlienLightObjectData : public LightObjectData
{
	INSTANCEOF(AlienLightObjectData, LightObjectData)

public:

	virtual Bool Execute();
};

// self-defined LOD object data with own functions and members
class AlienLodObjectData : public LodObjectData
{
	INSTANCEOF(AlienLodObjectData, LodObjectData)

public:

	virtual Bool Execute();
};

// self-defined Volume Builder object data with own functions and members
class AlienVolumeBuilderData : public VolumeBuilderData
{
	INSTANCEOF(AlienVolumeBuilderData, VolumeBuilderData)

public:
	virtual Bool Execute();
};

// self-defined instance object data with own functions and members
class AlienInstanceObjectData : public InstanceObjectData
{
	INSTANCEOF(AlienInstanceObjectData, InstanceObjectData)

public:

	virtual Bool Execute();
};

// self-defined xref object data with own functions and members
class AlienXRefObjectData : public NodeData
{
	INSTANCEOF(AlienXRefObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined boolean object data with own functions and members
class AlienBoolObjectData : public NodeData
{
	INSTANCEOF(AlienBoolObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined instance object data with own functions and members
class AlienExtrudeObjectData : public NodeData
{
	INSTANCEOF(AlienExtrudeObjectData, NodeData)

public:

	virtual Bool Execute();
};

// self-defined deformer object data with own functions and members
class AlienDeformerObjectData : public NodeData
{
	INSTANCEOF(AlienDeformerObjectData, NodeData)

	Int32 deformer_id;

public:
	AlienDeformerObjectData(Int32 id) : deformer_id(id) {}
	Int32 GetDeformerType()
	{
		return deformer_id;
	};
	virtual Bool Execute();
};

// self-defined Sky shader object data with own functions and members
class AlienSkyShaderObjectData : public SkyShaderObjectData
{
	INSTANCEOF(AlienSkyShaderObjectData, SkyShaderObjectData)

public:

	virtual Bool Execute();
};

// self-defined CAJoint object data with own functions and members
class AlienCAJointObjectData : public JointObjectData
{
	INSTANCEOF(AlienCAJointObjectData, JointObjectData)

public:

	virtual Bool Execute();
};

// self-defined CASkin object data with own functions and members
class AlienCASkinObjectData : public SkinObjectData
{
	INSTANCEOF(AlienCASkinObjectData, SkinObjectData)

public:

	virtual Bool Execute();
};

// self-defined MeshDeformer object data with own functions and members
class AlienCAMeshDeformerObjectData : public MeshDeformerObjectData
{
	INSTANCEOF(AlienCAMeshDeformerObjectData, MeshDeformerObjectData)

public:

	virtual Bool Execute();
};

// self-defined material with own functions and members
class AlienMaterial : public Material
{
	INSTANCEOF(AlienMaterial, Material)

public:

	Int32 matId;

	AlienMaterial() : Material()
	{
		matId = 0;
	}
	virtual Bool Execute();
};

// self-defined material with own functions and members
class AlienBaseMaterial : public BaseMaterial
{
	INSTANCEOF(AlienBaseMaterial, BaseMaterial)

public:

	Int32 matId;

	AlienBaseMaterial(Int32 typeID)
	{
		SetID(typeID);
		matId = 0;
	}
	virtual Bool Execute();
};

// self-defined render data with own functions and members
class AlienRenderData : public RenderData
{
	INSTANCEOF(AlienRenderData, RenderData)

public:

	AlienRenderData() : RenderData() { }
};
}

#endif // ALIEN_DEF_H__
