/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_INSTANCEOBJECTDATA_H__
#define C4D_INSTANCEOBJECTDATA_H__

#include "c4d_nodedata.h"
#include "c4d_misc/datastructures/basearray.h"

namespace cineware
{
#pragma pack (push, 8)

class BaseDocument;

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's instance object.
/// @since R20
//----------------------------------------------------------------------------------------
class InstanceObjectData : public NodeData
{
	INSTANCEOF(InstanceObjectData, NodeData)

	friend class BaseObject;

public:
	/// @name Multi-instance Reference Object
	/// @{

	//----------------------------------------------------------------------------------------
	/// Return the instanciated object.
	/// @note Same as accessing the INSTANCEOBJECT_LINK element in the object's data container.
	/// @param[in] doc								The document the instance object belongs to.
	/// @return												The reference object. Null if the instance object is unassigned.
	//----------------------------------------------------------------------------------------
	BaseObject* GetReferenceObject(BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the instanciated object used for the multiple instances.
	/// @param[in] refObj							 The reference object to set.
	//----------------------------------------------------------------------------------------
	void SetReferenceObject(BaseObject* refObj);

	/// @}

	/// @name Multi-instance Matrix and Instance Count
	/// @{

	//----------------------------------------------------------------------------------------
	/// Checks if the instance object is in multi-instance mode.
	/// @return												@trueIfOtherwiseFalse{in multi-instance mode}
	//----------------------------------------------------------------------------------------
	Bool IsMultiInstance();

	//----------------------------------------------------------------------------------------
	/// Sets the instance matrices.
	/// @note The size of @formatParam{matrices} determines the instance count.
	/// @param[in] matrices						The instance matrix array.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetInstanceMatrices(const BaseArray<Matrix> &matrices);

	//----------------------------------------------------------------------------------------
	/// Returns the instance matrices.
	/// @return												The pointer to the matrix array. Owned by the instance object.
	//----------------------------------------------------------------------------------------
	BaseArray<Matrix>* GetInstanceMatrices();

	//----------------------------------------------------------------------------------------
	/// Returns the global matrix of the instance at the specified @formatParam{index}.
	/// @param[in] index							The instance index.
	/// @return												The instance matrix.
	//----------------------------------------------------------------------------------------
	Matrix GetInstanceMatrix(UInt index) const;

	//----------------------------------------------------------------------------------------
	/// Returns the instance count (number of instances and matrices).
	/// @return												The instance count.
	//----------------------------------------------------------------------------------------
	UInt GetInstanceCount();

	/// @}

	/// @name Multi-instance Color
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the instance colors.
	/// @param[in] colors							The array of instance colors to set.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetInstanceColors(const BaseArray<Color64> &colors);

	//----------------------------------------------------------------------------------------
	/// Returns the instance colors.
	/// @note If the array is null or empty, the default color is used.
	/// @return												The pointer to the color array. Owned by the instance object.
	//----------------------------------------------------------------------------------------
	BaseArray<Color64>* GetInstanceColors();

	//----------------------------------------------------------------------------------------
	/// Returns the color of the instance at the specified @formatParam{index}.
	/// @note If no color is found or @formatParam{index} is invalid, the default color is returned.
	/// @param[in] index							The instance index.
	/// @return												The instance color.
	//----------------------------------------------------------------------------------------
	Color64 GetInstanceColor(UInt index);

	/// @}

protected:
	static NodeData* Alloc();

private:
	virtual Bool Init();
	virtual void Free();

	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);
	virtual Bool GetDParameter(const DescID& id, GeData& t_data);

	virtual Int32 GetChunkLevel();

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	void FlushArrays();

	BaseArray<Matrix>		objectMatrices;	// Data Given by system/user
	BaseArray<Color64>	objectColors;		// Data Given by system/user
};

#pragma pack (pop)
}

#endif	// C4D_INSTANCEOBJECTDATA_H__
