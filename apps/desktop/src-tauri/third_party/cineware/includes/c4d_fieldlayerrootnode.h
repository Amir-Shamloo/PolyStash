/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_FIELDLAYERROOTNODE_H__
#define C4D_FIELDLAYERROOTNODE_H__

#include "c4d_rootpluginnode.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// @markPrivate
/// @since R20
//----------------------------------------------------------------------------------------
class FieldlayerRootNode : public RootPluginNode
{
	INSTANCEOF(FieldlayerRootNode, RootPluginNode)

private:
	virtual Bool CopyToX(PrivateChunk* dst, COPYFLAGS flags, AliasTrans* trn);

public:
	virtual Bool HandleSubChunk(HyperFile* hf, Int32 id, Int32 level);
	virtual Bool Write(HyperFile* hf);

	RootPluginNode* GetClone(COPYFLAGS flags, AliasTrans* trans);
};

#pragma pack (pop)
}

#endif	// C4D_FIELDLAYERROOTNODE_H__
