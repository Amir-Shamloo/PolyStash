/////////////////////////////////////////////////////////////
// Cineware SDK                                            //
/////////////////////////////////////////////////////////////
// (c) MAXON Computer GmbH, all rights reserved            //
/////////////////////////////////////////////////////////////

#ifndef C4D_DOCUMENTPREF_H__
#define C4D_DOCUMENTPREF_H__

#include "c4d_gelistnode.h"

namespace cineware
{
#pragma pack (push, 8)

//----------------------------------------------------------------------------------------
/// Access document preferences.
//----------------------------------------------------------------------------------------
class DocumentPref : public GeListNode
{
	friend class BaseDocument;

private:
	Float		 d_lod;
	Int32		 d_fps;
	BaseTime d_min, d_max;
	BaseTime d_loop_min, d_loop_max;
	Bool		 d_renderlod;
	Bool		 use_animation, use_expressions, use_generators, use_deformers;

public:
	//----------------------------------------------------------------------------------------
	/// Default constructor.
	//----------------------------------------------------------------------------------------
	DocumentPref();

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Write(HyperFile* hf);

	//----------------------------------------------------------------------------------------
	/// @markPrivate
	//----------------------------------------------------------------------------------------
	virtual Bool Read(HyperFile* hf, Int32 id, Int32 level);

	//----------------------------------------------------------------------------------------
	/// Gets the Level Of Detail.
	/// @return												The Level Of Detail. The default LOD values are:
	/// - Low = @em 0.25
	/// - Medium = @em 0.5
	/// - High = @em 1.0
	//----------------------------------------------------------------------------------------
	Float GetLOD();

	//----------------------------------------------------------------------------------------
	/// Checks if the Level Of Detail for rendering should be used in the editor.
	/// @return												@trueIfOtherwiseFalse{using the render LOD}
	//----------------------------------------------------------------------------------------
	Bool GetRenderLod();

	//----------------------------------------------------------------------------------------
	/// Gets the number of Frames Per Second.
	/// @return												The number of FPS.
	//----------------------------------------------------------------------------------------
	Int32 GetFps();

	//----------------------------------------------------------------------------------------
	/// Gets the start time.
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @return												The minimum time of the document.
	//----------------------------------------------------------------------------------------
	BaseTime GetMinTime();

	//----------------------------------------------------------------------------------------
	/// Gets the end time.
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @return												The maximum time of the document.
	//----------------------------------------------------------------------------------------
	BaseTime GetMaxTime();

	//----------------------------------------------------------------------------------------
	/// Gets the time of the left boundary for the preview range (loop range).
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @return												The minimum time of the document's loop.
	//----------------------------------------------------------------------------------------
	BaseTime GetLoopMinTime();

	//----------------------------------------------------------------------------------------
	/// Gets the time of the right boundary for the preview range (loop range).
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @return												The maximum time of the document's loop.
	//----------------------------------------------------------------------------------------
	BaseTime GetLoopMaxTime();

	//----------------------------------------------------------------------------------------
	/// Sets the Level Of Detail.
	/// @param[in] lod								The Level Of Detail. The default LOD values are:
	/// - Low = @em 0.25
	/// - Medium = @em 0.5
	/// - High = @em 1.0
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetLOD(Float lod, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets if the Level Of Detail for rendering should be used in the editor.
	/// @param[in] lod								@trueIfOtherwiseFalse{the render LOD should be used}
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetRenderLod(Bool lod, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the number of Frames Per Second.
	/// @param[in] newfps							The number of FPS.
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetFps(Int32 newfps, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the start time.
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @param[in] new_min						The minimum time.
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetMinTime(const BaseTime& new_min, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the end time.
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @param[in] new_max						The maximum time.
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetMaxTime(const BaseTime& new_max, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the time of the left boundary for the preview range (loop range).
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @param[in] new_min						The minimum time of the document's loop.
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetLoopMinTime(const BaseTime& new_min, BaseDocument* doc);

	//----------------------------------------------------------------------------------------
	/// Sets the time of the right boundary for the preview range (loop range).
	/// @see	BaseTime for conversion of FPS or other time units.
	///				GetFps()
	/// @param[in] new_max						The maximum time of the document's loop.
	/// @param[in] doc								The document.
	//----------------------------------------------------------------------------------------
	void SetLoopMaxTime(const BaseTime& new_max, BaseDocument* doc);
};

#pragma pack (pop)
}

#endif	// C4D_DOCUMENTPREF_H__
