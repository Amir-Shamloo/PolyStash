/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________ *
*  Copyright 2020-2025 Adobe
*  All Rights Reserved.
* * NOTICE:  All information contained herein is, and remains
* the property of Adobe and its suppliers, if any. The intellectual
* and technical concepts contained herein are proprietary to Adobe
* and its suppliers and are protected by all applicable intellectual
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe.
**************************************************************************/

#ifndef ALG_SBSAR_MANAGER_H
#define ALG_SBSAR_MANAGER_H

#include "errors.h"
#include "exportmacros.h"

// Forward declaration
typedef struct SBSARManagerHandle SBSARManagerHandle;


// SBSAR Manager handle-related functions

/**
	@brief Initializes a SBSARManager handle (opaque pointer)
	@param[in,out] pHandle       The pointer to the opaque handle (filled by the function)
	@param[in]     sbsarFileName The name of the SBSAR file to open
	@note The opaque handle must be released by calling @link sbsarManagerRelease @endlink
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerInit(
	SBSARManagerHandle** pHandle,
	const char*          sbsarFileName);

/**
	@brief Saves a modified SBSAR (modified inputs, added/deleted icons, added/deleted presets)
	@param[in] handle         The SBSARManager handle to save
	@param[in] outputFileName The name of the output SBSAR file
	@note This will overwrite the destination file!
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerSave(
	SBSARManagerHandle* handle,
	const char*         outputFileName);

/**
	@brief Releases an SBSARManager handle
	@param[in] handle The SBSARManager handle to release
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerRelease(
	SBSARManagerHandle* handle);


// Input-related functions

/**
	@brief Changes a graph input's default value, looking up the input by its name
	@param[in] handle    The SBSARManager handle to use
	@param[in] graphName The name of the graph to use (can be NULL)
	@param[in] inputName The name of the input to modify
	@param[in] inputType The type of the input to modify (follows the SubstanceIOType enum)
	@param[in] newData   A pointer to the new data to use
	@note For strings, newData must point to the beginning of a 0-terminated string
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerChangeInputValueByName(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         inputName,
	const unsigned int  inputType,
	const void*         newData);

/**
	@brief Changes a graph input's default value, looking up the input by its UID
	@param[in] handle    The SBSARManager handle to use
	@param[in] graphName The name of the graph to use (can be NULL)
	@param[in] inputUID  The UID of the input to modify
	@param[in] inputType The type of the input to modify (follows the SubstanceIOType enum)
	@param[in] newData   A pointer to the new data to use
	@note For strings, newData must point to the beginning of a 0-terminated string
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerChangeInputValueByUID(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const unsigned int  inputUID,
	const unsigned int  inputType,
	const void*         newData);


// Preset-related functions

/**
	@brief Applies a graph's preset to the graph
	@param[in] handle     The SBSARManager handle to use
	@param[in] graphName  The name of the graph to use (can be NULL)
	@param[in] presetName The name of the preset to apply
	@note 'Internal' preset are the ones embedded in the SBSAR's XML file
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerApplyInternalPreset(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         presetName);

/**
	@brief Applies a preset from s SBSPRS file to the graph
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file which contains the preset to apply
	@param[in] presetName     The name of the preset to apply
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerApplyExternalPreset(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName,
	const char*         presetName);

/**
	@brief Exports all of a graph's presets to an SBSPRS file
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file to which the presets will be written
	@note This will overwrite the SBSPRS file!
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerExportAllPresets(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName);

/**
	@brief Exports some of a graph's presets to an SBSPRS file
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file to which the preset will be written
	@param[in] presetCount    The number of presets to save
	@param[in] presetNames    The names of the presets to save
	@note This will overwrite the SBSPRS file!
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerExportSomePresets(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName,
	const unsigned int  presetCount,
	const char**        presetNames);

/**
	@brief Exports a graph preset to an SBSPRS file
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file to which the preset will be written
	@param[in] presetName     The name of the preset to save
	@note This will overwrite the SBSPRS file!
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerExportPreset(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName,
	const char*         presetName);

/**
	@brief Adds all presets from an SBSPRS file to a graph's list of presets
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file which contains the presets to add
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerAddAllPresets(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName);

/**
	@brief Adds some presets from an SBSPRS file to a graph's list of presets
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file which contains the preset to add
	@param[in] presetCount    The number of presets to add
	@param[in] presetNames    The names of the presets to add
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerAddSomePresets(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName,
	const unsigned int  presetCount,
	const char**        presetNames);

/**
	@brief Adds a preset from an SBSPRS file to a graph's list of presets
	@param[in] handle         The SBSARManager handle to use
	@param[in] graphName      The name of the graph to use (can be NULL)
	@param[in] sbsprsFileName The name of the SBSPRS file which contains the preset to add
	@param[in] presetName     The name of the preset to add
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerAddPreset(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         sbsprsFileName,
	const char*         presetName);

/**
	@brief Deletes all of a graph's presets
	@param[in] handle    The SBSARManager handle to use
	@param[in] graphName The name of the graph to use (can be NULL)
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeleteAllPresets(
	SBSARManagerHandle* handle,
	const char*         graphName);

/**
	@brief Deletes some of a graph's presets
	@param[in] handle      The SBSARManager handle to use
	@param[in] graphName   The name of the graph to use (can be NULL)
	@param[in] presetCount The number of presets to delete
	@param[in] presetNames The names of the presets to delete
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeleteSomePresets(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const unsigned int  presetCount,
	const char**        presetNames);

/**
	@brief Deletes a graph's preset
	@param[in] handle     The SBSARManager handle to use
	@param[in] graphName  The name of the graph to use (can be NULL)
	@param[in] presetName The name of the preset to delete
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeletePreset(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         presetName);


// Icon-related functions

/**
	@brief Saves each graph's icon to a specific folder
	@param[in] handle       The SBSARManager handle to use
	@param[in] outputFolder The name of the folder where the icon files will be saved
	@note Icon files are named after their associated graph
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerSaveAllIcons(
	SBSARManagerHandle* handle,
	const char*         outputFolder);

/**
	@brief Saves somes graphs' icons to a specific folder
	@param[in] handle       The SBSARManager handle to use
	@param[in] graphCount   The number of graph icons to save
	@param[in] graphNames   The names of the graphs whose icons should be saved
	@param[in] outputFolder The name of the folder where the icon file will be saved
	@note Icon files are named after their associated graph
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerSaveSomeIcons(
	SBSARManagerHandle* handle,
	const unsigned int  graphCount,
	const char**        graphNames,
	const char*         outputFolder);

/**
	@brief Saves a graph's icon to a specific folder
	@param[in] handle       The SBSARManager handle to use
	@param[in] graphName    The name of the graph to use (can be NULL)
	@param[in] outputFolder The name of the folder where the icon file will be saved
	@note Icon files are named after their associated graph
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerSaveIcon(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         outputFolder);

/**
	@brief Deletes each graph's icon
	@param[in] handle       The SBSARManager handle to use
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeleteAllIcons(
	SBSARManagerHandle* handle);

/**
	@brief Deletes some graphs' icons
	@param[in] handle     The SBSARManager handle to use
	@param[in] graphCount The number of graph icons to delete
	@param[in] graphNames The names of the graphs whose icons should be deleted
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeleteSomeIcons(
	SBSARManagerHandle* handle,
	const unsigned int  graphCount,
	const char**        graphNames);

/**
	@brief Deletes a specific graph's icon
	@param[in] handle    The SBSARManager handle to use
	@param[in] graphName The name of the graph to use (can be NULL)
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerDeleteIcon(
	SBSARManagerHandle* handle,
	const char*         graphName);

/**
	@brief Adds (but does not replace) a specific graph's icon
	@param[in] handle       The SBSARManager handle to use
	@param[in] graphName    The name of the graph to use (can be NULL)
	@param[in] iconFileName The name of the icon file to associate to the graph
	@note This will fail if the graph already has an icon associated to it
	@note No check is performed regarding the validity of the icon file data
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerAddIcon(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         iconFileName);

/**
	@brief Adds/replaces a specific graph's icon
	@param[in] handle       The SBSARManager handle to use
	@param[in] graphName    The name of the graph to use (can be NULL)
	@param[in] iconFileName The name of the icon file to associate to the graph
	@note If the graph already has an icon, this will be replaced with the provided one
	@note No check is performed regarding the validity of the icon file data
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerAddOrReplaceIcon(
	SBSARManagerHandle* handle,
	const char*         graphName,
	const char*         iconFileName);


// Graph info related functions

typedef struct {
	const char* label;
	const char* description;
	const char* category;
	const char* keywords;
	const char* author;
	const char* authorurl;
	const char* usertag;
	const char* graphtype;
	const char* materialmodel;
} SBSARMgrGraphInfo;

/**
	@brief Get the number of graphs in a package
	@param[in]  handle     The SBSARManager handle to use
	@param[out] graphCount A pointer to the graph count to fill
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerGetGraphCount(
	SBSARManagerHandle* handle,
	unsigned int*       graphCount);

/**
	@brief Gather a graph's descriptive information
	@param[in] handle    The SBSARManager handle to use
	@param[in] graphName The name of the graph to use (can be NULL)
	@param[in] graphInfo A pointer to the SBSARMgrGraphInfo structure to fill
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerGetGraphInfo(
	SBSARManagerHandle* handle,
	const char*         graphName,
	SBSARMgrGraphInfo*  graphInfo);

/**
	@brief Gather a graph's descriptive information (lookup by graph index)
	@param[in] handle     The SBSARManager handle to use
	@param[in] graphIndex The index of the graph to use
	@param[in] graphInfo  A pointer to the SBSARMgrGraphInfo structure to fill
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/

SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerGetGraphInfoByIndex(
	SBSARManagerHandle* handle,
	const unsigned int  graphIndex,
	SBSARMgrGraphInfo*  graphInfo);

/**
	@brief Modify a graph's descriptive information
	@param[in]    handle    The SBSARManager handle to use
	@param[in]    graphName The name of the graph to use (can be NULL)
	@param[inout] graphInfo A pointer to the SBSARMgrGraphInfo structure to use
	@note To delete a particular description field, use nullptr in the SBSARMgrGraphInfo structure
	@note Only the fields whose length is non-0 will be written to the graph's descriptive information
	@note The graph type and material model cannot be modified
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerModifyGraphInfo(
	SBSARManagerHandle*      handle,
	const char*              graphName,
	const SBSARMgrGraphInfo* graphInfo);


// Metadata related functions

typedef enum {
	SBSARMGR_METADATA_PACKAGE,
	SBSARMGR_METADATA_GRAPH,
	SBSARMGR_METADATA_INPUT,
	SBSARMGR_METADATA_OUTPUT
} SBSARMgrMetadataType;

/**
	@brief Read the metadata of a package/graph/input/output
	@param[in]    handle             The SBSARManager handle to use
	@param[in]    metadataType       The type of entity to read from
	@param[in]    graphName          The name of the graph to read from (can be NULL)
	@param[in]    metadataSourceName The name of the entity to read from (can be NULL for 'package')
	@param[inout] metadata           A pointer to the returned metadata
	                                 NULL is returned if no metadata is present for the given entity
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerGetMetadata(
	SBSARManagerHandle*        handle,
	const SBSARMgrMetadataType metadataType,
	const char*                graphName,
	const char*                metadataSourceName,
	const char**               metadata);

/**
	@brief Set the metadata of a package/graph/input/output
	@param[in] handle             The SBSARManager handle to use
	@param[in] metadataType       The type of entity to write to
	@param[in] graphName          The name of the graph to write to (can be NULL)
	@param[in] metadataSourceName The name of the entity to write to (can be NULL for 'package')
	@param[in] metadata           A pointer to the metadata to write
	@return The error code, or SBSARMgr_Error_NoError if no error occurred
*/
SBSAR_MGR_EXPORT SBSARMgrError sbsarManagerSetMetadata(
	SBSARManagerHandle*        handle,
	const SBSARMgrMetadataType metadataType,
	const char*                graphName,
	const char*                metadataSourceName,
	const char*                metadata);

#endif // ALG_SBSAR_MANAGER_H
