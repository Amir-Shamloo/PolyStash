/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_LAYEROBJECT_H__
#define C4D_LAYEROBJECT_H__

#include "c4d_baselist4d.h"
#include "c4d_layerdata.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Represents a layer in @C4D.
/// @addAllocFreeNote
//----------------------------------------------------------------------------------------
class LayerObject : public BaseList4D
{
	INSTANCEOF(LayerObject, BaseList4D)

	friend class RootLayer;
	friend class BaseDocument;

private:
	LayerData	m_data;

	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trans);
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	virtual Bool GetDParameter(const DescID& id, GeData& data);
	virtual Bool SetDParameter(const DescID& id, const GeData& data);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	LayerObject();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~LayerObject();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{layer}
	/// @return												@allocReturn{layer}
	//----------------------------------------------------------------------------------------
	static LayerObject* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{layer}
	/// @param[in,out] v							@theToDestruct{layer}
	//----------------------------------------------------------------------------------------
	static void Free(LayerObject*& v);

	/// @}

	/// @name Navigation
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the next layer in the list.
	/// @return												The next layer, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetNext();

	//----------------------------------------------------------------------------------------
	/// Gets the previous layer in the list.
	/// @return												The previous layer, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetPred();

	//----------------------------------------------------------------------------------------
	/// Gets the parent layer of the list node.
	/// @return												The previous layer, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetUp();

	//----------------------------------------------------------------------------------------
	/// Gets the first child layer of the list node.
	/// @return												The previous layer, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetDown();

	//----------------------------------------------------------------------------------------
	/// Gets the last child layer of the list node.
	/// @return												The previous layer, or @formatConstant{nullptr} if there is none. @cinewareOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	LayerObject* GetDownLast();

	/// @}

	/// @name Clone
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a clone/copy of the layer.
	/// @param[in] flags							Flags for the clone.
	/// @param[in] trn								An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{layer}
	/// @return												The cloned layer. @callerOwnsPointed{layer}
	//----------------------------------------------------------------------------------------
	virtual BaseList2D* GetClone(COPYFLAGS flags, AliasTrans* trn);

	/// @}

	/// @name Execute
	/// @{

	//----------------------------------------------------------------------------------------
	/// Overload this method for self defined layers.
	/// @note	If an AlienLayerObject class is derived from LayerObject it will get a call in AlienLayerObject::Execute() for every created layer in CreateSceneFromC4D().
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	virtual Bool Execute();

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_LAYEROBJECT_H__
