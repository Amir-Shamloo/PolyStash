/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_SMART_LINK_H__
#define C4D_SMART_LINK_H__

#include "b3d_file.h"

namespace cineware
{
#pragma pack (push, 8)

class C4DAtom;
class BaseDocument;
class GeListNode;

class BaseLink;
class AliasTrans;

class GeAliasXGoal;
class GeAliasXLink;
class GeAliasXTrans;

//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
struct GeAliasID
{
	GeAliasID()
	{
		fid = 0;
	}

	inline Bool IsEqual(const GeAliasID* cmp) const
	{
		return fid == cmp->fid;
	}

	inline Bool IsZero() const
	{
		return fid == 0;
	}

	UInt64 fid;
};


//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class BrowseGoal
{
	friend class GeAliasXGoal;
	GeAliasXLink*	l;

public:
	BrowseGoal();
};


//----------------------------------------------------------------------------------------
/// @markPrivate
//----------------------------------------------------------------------------------------
class GeAliasGoal
{
	friend class GeAliasXGoal;
	friend class GeAliasXLink;
	friend class GeAliasXTrans;

private:
	void*					org_ptr;
	Bool					isVisible;

	GeAliasXGoal*	xgoal;

public:
	GeAliasGoal();
	virtual ~GeAliasGoal();

	void Init(void* ptr);
	void* GetPtr();

	void SetVisibility(Bool isVis);

	virtual Bool GetVisibility();
	virtual Bool IsAliasGoal();
	virtual BaseDocument* GetDocument();

	void RemoveLinks();
	void XRemoveLinks();

	void GetAliasID(GeAliasID* id) const;
	void SetAliasID(const GeAliasID& id, AliasTrans& trans);

	void XTransfer(GeAliasGoal* dest);
	void Transfer(GeAliasGoal* dest);

	Bool Read(HyperFile* hf);
	Bool Write(HyperFile* hf);

	C4DAtom* GetFirstLink(BrowseGoal& browse, Bool debug = false);
	C4DAtom* GetNextLink(BrowseGoal& browse, Bool debug = false);

	void CopyAliasTrans(GeAliasGoal* dest, AliasTrans* trans);
};

//----------------------------------------------------------------------------------------
/// A dynamic link to a BaseList2D object.\n
/// A link can be easily stored in containers and saved to disk, and is even preserved when duplicating hierarchies.
//----------------------------------------------------------------------------------------
class BaseLink
{
	friend class GeAliasXGoal;
	friend class GeAliasXLink;
	friend class GeAliasXTrans;
	friend struct tagInExcludeElement;

private:
	String*				cachepath;
	GeAliasXLink*	xlink;
	C4DAtom*			up_pointer;

	BaseLink(const BaseLink& s);
	const BaseLink& operator = (const BaseLink& s);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	BaseLink();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	virtual ~BaseLink();

	/// @}

	/// @name Alloc/Free
	/// @{

	//----------------------------------------------------------------------------------------
	/// @allocatesAN{alias link}
	/// @allocReturn{alias link}
	//----------------------------------------------------------------------------------------
	static BaseLink* Alloc();

	//----------------------------------------------------------------------------------------
	/// @destructsAlloc{alias links}
	/// @param[in,out] ptr						@theToDestruct{alias link}
	//----------------------------------------------------------------------------------------
	static void Free(BaseLink*& ptr);

	/// @}

	/// @name Read/Write
	/// @{

	//----------------------------------------------------------------------------------------
	/// Reads the alias link from a hyper file.
	/// @param[in] hf									The hyper file to read from. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the alias link was read}
	//----------------------------------------------------------------------------------------
	Bool Read(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// Writes the alias link to a hyper file.
	/// @param[in] hf									The hyper file to write to. @callerOwnsPointed{hyper file}
	/// @return												@trueIfOtherwiseFalse{the alias link was written}
	//----------------------------------------------------------------------------------------
	Bool Write(HyperFile* hf);

	/// @}

	/// @name Clone/Copy
	/// @{

	//----------------------------------------------------------------------------------------
	/// Gets a copy of the alias link.
	/// @param[in] aliastrans					An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias link}
	/// @return												The cloned alias link. @callerOwnsPointed{alias link}
	//----------------------------------------------------------------------------------------
	virtual BaseLink* GetClone(AliasTrans* aliastrans) const;

	//----------------------------------------------------------------------------------------
	/// Copies the link to @formatParam{dest}.
	/// @param[out] dest							The destination alias link. @callerOwnsPointed{alias link}
	/// @param[in] aliastrans					An alias translator for the operation. Can be @formatConstant{nullptr}. @callerOwnsPointed{alias link}
	//----------------------------------------------------------------------------------------
	virtual void CopyData(BaseLink* dest, AliasTrans* aliastrans) const;

	/// @}

	/// @name Link
	/// @{

	//----------------------------------------------------------------------------------------
	/// Sets the link to point to @formatParam{bl}.
	/// @param[in] bl									The new link. @callerOwnsPointed{list node}
	//----------------------------------------------------------------------------------------
	void SetLink(GeListNode* bl);

	//----------------------------------------------------------------------------------------
	/// Evaluates and gets the link in its document.
	/// @return												The link, or @formatConstant{nullptr} if the link is broken. @theOwnsPointed{document,}
	//----------------------------------------------------------------------------------------
	void* GetLink() const;

	//----------------------------------------------------------------------------------------
	/// Transfers the link to @formatParam{dest}.
	/// @param[out] dest							The destination alias link. @callerOwnsPointed{alias link}
	//----------------------------------------------------------------------------------------
	void Transfer(BaseLink* dest);

	/// @}

	/// @name Private
	/// @{

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeListNode* GetLinkAtom(const BaseDocument* doc, Int32 instanceof = 0) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void XCopyData(BaseLink* dest, AliasTrans* aliastrans) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void XSetLink(GeAliasGoal& org);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetLink(GeAliasGoal& org);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeAliasGoal* GetLinkGoal() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	GeAliasGoal* XGetLinkGoal() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void XRemoveLink();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void RemoveLink();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void GetAliasID(GeAliasID* id) const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetAliasID(const GeAliasID& id, AliasTrans& trans, GeAliasGoal* old_goal);

	//-------------------------------------------------------------------------------------------------
	/// @markPrivate
	//-------------------------------------------------------------------------------------------------
	Bool IsCacheLink() const;

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetUpPointer(C4DAtom* up_pointer);

	/// @}
};


//----------------------------------------------------------------------------------------
/// The alias translator class takes care about reconnecting BaseLink links when cloning, copying, reading or writing objects.\n
/// @code
/// AliasTrans trn;
/// if (!trn.Init())
///   return nullptr; // Out of memory
///
/// BaseMaterial* clone = (BaseMaterial*)mat->GetClone(COPYFLAGS_NONE, &trn);
/// if (!clone)
///   return nullptr; // Out of memory
/// @endcode
//----------------------------------------------------------------------------------------
class AliasTrans
{
	friend class GeAliasGoal;
	friend class BaseLink;

private:
	GeAliasXTrans* xtrans;

	void XSetGoalFromID(const GeAliasID& id, GeAliasGoal* goal);
	void XSetAliasFromID(const GeAliasID& id, BaseLink* link, GeAliasGoal* old_goal);

	void SetGoalFromID(const GeAliasID& id, GeAliasGoal* goal);
	void SetAliasFromID(const GeAliasID& id, BaseLink* link, GeAliasGoal* old_goal);

public:
	/// @name Constructor/Destructor
	/// @{

	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	AliasTrans();

	//----------------------------------------------------------------------------------------
	/// Default destructor.
	//----------------------------------------------------------------------------------------
	~AliasTrans();

	/// @}

	//----------------------------------------------------------------------------------------
	/// Initializes the alias translator.
	/// @return												@trueIfOtherwiseFalse{the alias translator was initialized}
	//----------------------------------------------------------------------------------------
	Bool Init();

	//----------------------------------------------------------------------------------------
	/// Translates the links in all objects that the translator has come across.
	/// @param[in] connect_oldgoals		Pass @formatConstant{true} to connect old goals. For example:\n
	///																Take a cube with an instance linked to this cube, select both and duplicate them in one action.\n
	///																The new cube is linked to the new instance if @formatParam{connect_oldgoals} is @formatConstant{true}. If it is @formatConstant{false} the new instance is linked to the old cube.
	//----------------------------------------------------------------------------------------
	void Zap(Bool connect_oldgoals);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	void SetAliasGoal(const GeAliasID& id, GeAliasGoal* goal);
};


#pragma pack (pop)
}

#endif	// C4D_SMART_LINK_H__
