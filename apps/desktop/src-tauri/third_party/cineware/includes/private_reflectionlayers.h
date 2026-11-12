/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef REFLECTIONLAYERS_H__
#define REFLECTIONLAYERS_H__

#include "c4d_system.h"
#include "private_ge_mtools.h"

namespace cineware
{
#pragma pack (push, 8)

class XReflectionLayer;
class BaseDocument;
class BaseContainer;
class Material;
class BaseLink;
class BaseList2D;

/// @markInternal
class XReflectionLayers : public CDynamicObjectArray<XReflectionLayer>
{
public:
	BaseList2D* _material;
	Int32 _lastLayerIndexHit;

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	XReflectionLayers();

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	virtual XReflectionLayer* AllocObject() const;

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	virtual XReflectionLayer* AllocObjectType(Int32 nType) const;

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	virtual void FreeObject(XReflectionLayer *&pObj) const;

	//----------------------------------------------------------------------------------------
	/// Adds a new reflection layer to the list.
	/// @param[in] pData							Layer's material data.
	/// @param[in] layerID						The layer ID. @see iXRLayersDataType::GetLayerID()
	/// @param[in] layerDataID				The layer data ID. @see iXRLayersDataType::GetNextDataID().
	/// @param[in] bAppend						True: append, false: add to the head.
	/// @return												The new layer.
	//----------------------------------------------------------------------------------------
	XReflectionLayer* Add(BaseContainer* pData, Int32 layerID, Int32 layerDataID, Bool bAppend, Int32 insertionIndex = 0);

	//----------------------------------------------------------------------------------------
	/// Removes a layer from the list.
	/// @param[out] pParent						Reflection layer owner.
	/// @param[out] pData							Reflection layer data container (normally is the @formatParam{pParent} material @formatParam{data} container).
	/// @param[out] pLayer						The layer to remove. Must be in the list.
	/// @param[in] bAddUndo						Set to true to add to undo stack the action, set to false to ignore undo.
	//----------------------------------------------------------------------------------------
	void Remove(Material* pParent, BaseContainer *pData, XReflectionLayer* pLayer, Bool bAddUndo);

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	virtual void Free();

	//----------------------------------------------------------------------------------------
	/// Gets the layer at index @formatParam{nIndex}.
	/// @param[in] nIndex							The layer index in range [0, @formatParam{GetElementCount()} - 1].
	/// @return												The requested layer or nullptr if index is out of range.
	//----------------------------------------------------------------------------------------
	XReflectionLayer* GetLayer(Int32 nIndex);

	//----------------------------------------------------------------------------------------
	/// Gets the layer at index @formatParam{nIndex}. (Read-only version).
	/// @param[in] nIndex							The layer index in range [0, @formatParam{GetElementCount()} - 1].
	/// @return												The requested layer or nullptr if index is out of range.
	//----------------------------------------------------------------------------------------
	const XReflectionLayer* GetLayer(Int32 nIndex) const;

	//----------------------------------------------------------------------------------------
	Int32 GetLayerIndex(const XReflectionLayer& layer) const;

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	Bool ReadElementCreate(HyperFile* pFile, XReflectionLayer* &pLayer, Int32 nLevel);
	
	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	Bool WriteElementHF(HyperFile* pFile, const XReflectionLayer* pElement) const;
	
	//----------------------------------------------------------------------------------------
	/// Compares @formatParam{pLayers} reflection layer list with this list.
	/// @param[in] pLayers						The reflection layer list to compare.
	/// @return												0 if equal, otherwise 1.
	//----------------------------------------------------------------------------------------
	Int32 Compare(XReflectionLayers* pLayers);

	//----------------------------------------------------------------------------------------
	/// Inherited.
	//----------------------------------------------------------------------------------------
	virtual Bool CopyTo(CDynamicObjectArray<XReflectionLayer>* pDest, AliasTrans* pTrans, Bool bAppend = false) const;

	//----------------------------------------------------------------------------------------
	XReflectionLayer* FindLayerDataID(Int32 id);
	
	//----------------------------------------------------------------------------------------
	/// Compares two lists structurally (item count and layer names).
	/// @param[in] pLayers						Reflection layer list to compare.
	/// @return												True if both are equal, otherwise false.
	//----------------------------------------------------------------------------------------
	Bool CompareStructure(XReflectionLayers& pLayers);
};

#pragma pack (pop)
}

#endif	// REFLECTIONLAYERS_H__
