/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_WEIGHTTAGDATA_H__
#define C4D_WEIGHTTAGDATA_H__

#include "c4d_nodedata.h"
#include "c4d_skinjoint.h"

namespace cineware
{
#pragma pack (push, 8)

class ItemTreeNode;
class ItemTreeData;

/// Weight tag ID.
#define CA_WEIGHT_TAG_ID Tweights

//----------------------------------------------------------------------------------------
/// Joint rest state data.
/// @see WeightTagData::GetJointRestState()/@ref WeightTagData::SetJointRestState()
//----------------------------------------------------------------------------------------
struct JointRestState
{
	/// Default constructor.
	JointRestState()
	{
		m_Len = 0.0;
	}

	Matrix m_bMg,	///< Global matrix of the bone between two joints with the Z axis (V3) along the bone.
				 m_bMi;	///< Inverse matrix of the bone between two joints with the Z axis (V3) along the bone.
	Matrix m_oMg,	///< Global matrix of the actual Joint object.
				 m_oMi;	///< Inverse matrix of the actual Joint object.
	Float	 m_Len;	///< Bone rest length.
};

//----------------------------------------------------------------------------------------
/// Node data class for @C4D's Weight tag.
//----------------------------------------------------------------------------------------
class WeightTagData : public NodeData
{
	INSTANCEOF(WeightTagData, NodeData)

private:
	UInt32										 m_jDirty;
	Matrix										 m_bMg, m_iMg;
	Int32											 m_iLevel;

	DynamicArray<SkinJoint, 8> m_Joints;

	virtual Int32 GetChunkLevel();

	WeightTagData();
	~WeightTagData();

	virtual Bool Init();
	virtual void Free();

	virtual Bool GetDParameter(const DescID& id, GeData& t_data);
	virtual Bool SetDParameter(const DescID& id, const GeData& t_data);

	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);
	virtual Bool CopyToX(PrivateChunk* dest, COPYFLAGS flags, AliasTrans* trn);

	void UpdateList(BaseTag* tag, BaseDocument* doc, BaseContainer* bc);
	void ValidateTree(BaseDocument* doc, ItemTreeNode* node, ItemTreeData* itree);
	void ValidateTreeEffectors(BaseDocument* doc, ItemTreeNode* enode, Int32 jindex, ItemTreeData* itree);
	Int32 FindEffector(BaseDocument* doc, Int32 jindex, BaseObject* op);

public:
	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesA{weight tag node data}
	/// @return												@allocReturn{weight tag node data}
	//----------------------------------------------------------------------------------------
	static NodeData* Alloc();

	/// @}

	/// @name Joint
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the joint object at the given @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] doc								The document for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{document}
	/// @return												The joint object, or @formatConstant{nullptr}.
	//----------------------------------------------------------------------------------------
	BaseObject* GetJoint(Int32 index, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Gets the total number of joint objects.
	/// @note	The joint count can also be found and changed on the weight tag using GetParameter()/SetParameter() with ::ID_CA_WEIGHT_TAG_JOINTS_LIST.
	/// @return												The joint count.
	//----------------------------------------------------------------------------------------
	Int32 GetJointCount();	// Get total joints;

	//----------------------------------------------------------------------------------------
	/// Gets the index of the given host object @formatParam{op}.
	/// @param[in] op									The host object. @callerOwnsPointed{object}
	/// @param[in] doc								The document for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{document}
	/// @return												The Joint index, or @ref NOTOK if not found.
	//----------------------------------------------------------------------------------------
	Int32 FindJoint(BaseObject* op, BaseDocument* doc);


	//----------------------------------------------------------------------------------------
	/// Adds a joint object to the weight tag's @em Joints list.
	/// @param[in] op									The joint object to add.
	/// @return												The joint object's index in the @em Joints list.
	//----------------------------------------------------------------------------------------
	Int32 AddJoint(BaseObject* op);

	//----------------------------------------------------------------------------------------
	/// Removes a joint object from the weight tag's @em Joints list.
	/// @param[in] op									The Joint object to remove.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool RemoveJoint(BaseObject* op);

	//----------------------------------------------------------------------------------------
	/// Gets the rest state for the joint at the given @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @return												The rest state.
	//----------------------------------------------------------------------------------------
	JointRestState GetJointRestState(Int32 index);	// Get the rest state for the joint at index.

	//----------------------------------------------------------------------------------------
	/// Sets the rest state for the joint at the given @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] state							The rest state to set.
	//----------------------------------------------------------------------------------------
	void SetJointRestState(Int32 index, const JointRestState& state);

	/// @}

	/// @name Weight
	/// @{

	//----------------------------------------------------------------------------------------
	/// Fills in @formatParam{map} with the weights for the joint at the given @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[out] map								Filled with the weight data. Must be allocated with @formatParam{cnt} elements.
	/// @param[in] cnt								The point count.
	//----------------------------------------------------------------------------------------
	void GetWeightMap(Int32 index, Float* map, Int32 cnt);

	//----------------------------------------------------------------------------------------
	/// Sets the weight @formatParam{map} for the joint at the given @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] map								The weight map to set. Must contain @formatParam{cnt} elements.
	/// @param[in] cnt								The point count.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetWeightMap(Int32 index, Float* map, Int32 cnt);	// Set the entire weight map using map.

	//----------------------------------------------------------------------------------------
	/// Gets the total number of stored weights for the joint at the given @formatParam{index}.
	/// @note	Zero weights are not stored.
	/// @param[in] index							The joint index.
	/// @return												The weight count.
	//----------------------------------------------------------------------------------------
	Int32 GetWeightCount(Int32 index);

	//----------------------------------------------------------------------------------------
	/// Gets the weight value and point index at weight index @formatParam{windex} for the joint @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] windex							The weight index.
	/// @param[out] pntindex					Assigned the point index.
	/// @param[out] weight						Assigned the weight.
	//----------------------------------------------------------------------------------------
	void GetIndexWeight(Int32 index, Int32 windex, Int32& pntindex, Float& weight);

	//----------------------------------------------------------------------------------------
	/// Gets the weight for the point @formatParam{pntindex} of the joint @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] pntindex						The point index.
	/// @return												The weight.
	//----------------------------------------------------------------------------------------
	Float GetWeight(Int32 index, Int32 pntindex);

	//----------------------------------------------------------------------------------------
	/// Sets the weight for the point @formatParam{pntindex} of the joint @formatParam{index}.
	/// @param[in] index							The joint index.
	/// @param[in] pntindex						The point index.
	/// @param[in] weight							The weight to set.
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool SetWeight(Int32 index, Int32 pntindex, Float weight);

	//UInt32 GetWeightDirty(); // Get the dirty state of the weights.

	//----------------------------------------------------------------------------------------
	/// Marks the weight dirty.
	//----------------------------------------------------------------------------------------
	void WeightDirty();

	//----------------------------------------------------------------------------------------
	/// Transfers the weights from one weight tag to another.\n
	/// @note	Pass @ref NOTOK for @formatParam{sindex}, @formatParam{dindex} and @formatParam{cnt} if they should not be evaluated.
	/// @param[in] doc								The document containing the weight tags. @callerOwnsPointed{document}
	/// @param[in] dst								The destination weight tag. @callerOwnsPointed{weight tag}
	/// @param[in] sindex							Index of the source joint.
	/// @param[in] dindex							Index of the destination joint.
	/// @param[in] offset							Offset to move the weight indices when copied to the destination.
	/// @param[in] cnt								The number of weights to be transfered.
	/// @param[in] trans							An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias translator}
	/// @return												@trueIfOtherwiseFalse{successful}
	//----------------------------------------------------------------------------------------
	Bool TransferWeightMap(BaseDocument* doc, WeightTagData* dst, Int32 sindex, Int32 dindex, Int32 offset, Int32 cnt, AliasTrans* trans);

	/// @}

	/// @name Bind Geometry Matrix
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets the global matrix for the bind geometry.\n
	/// @note	Use this with the global matrices of the joints to get the local transforms.
	/// @return												The global matrix for the bind geometry.
	//----------------------------------------------------------------------------------------
	Matrix GetGeomMg();

	//----------------------------------------------------------------------------------------
	/// Sets the global matrix for the bind geometry.
	/// @param[in] mg									The global matrix to set for the bind geometry.
	//----------------------------------------------------------------------------------------
	void SetGeomMg(const Matrix& mg);

	/// @}

	/// @name Miscellaneous
	/// @{

	//----------------------------------------------------------------------------------------
	/// Helper function to initialize the joint at @formatParam{index}.\n
	/// Fills in the bind state (JointRestState::m_oMg and JointRestState::m_oMi) and then call the method to calculate JointRestState::m_bMg, JointRestState::m_bMi and JointRestState::m_Len.
	/// @param[in] index							The joint index. Set to @ref NOTOK to process all joints.
	//----------------------------------------------------------------------------------------
	void CalculateBoneStates(Int32 index);

	/// @}
};

#pragma pack (pop)
}

#endif	// C4D_WEIGHTTAGDATA_H__
