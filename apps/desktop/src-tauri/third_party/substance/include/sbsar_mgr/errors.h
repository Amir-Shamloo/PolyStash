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

#ifndef _ALG_SBSAR_MANAGER_ERROR_H
#define _ALG_SBSAR_MANAGER_ERROR_H

typedef enum SBSARMgrError {
	SBSARMgr_Error_NoError,

	// Errors from inputs
	SBSARMgr_Error_InvalidInputXML,
	SBSARMgr_Error_InvalidInputType,
	SBSARMgr_Error_InvalidInputValue,
	SBSARMgr_Error_InvalidInputElements,
	SBSARMgr_Error_InputHasNoDefaultValue,

	// Errors from outputs
	SBSARMgr_Error_InvalidOutputXML,

	// Errors from presets
	SBSARMgr_Error_InvalidPresetXML,
	SBSARMgr_Error_InvalidPresetElements,
	SBSARMgr_Error_InvalidPresetFileName,
	SBSARMgr_Error_CannotOpenPresetFile,
	SBSARMgr_Error_CannotWritePresetFile,
	SBSARMgr_Error_InvalidPresetName,
	SBSARMgr_Error_InvalidPresetNameCount,
	SBSARMgr_Error_InvalidPresetNames,
	SBSARMgr_Error_InvalidPresetCollection,

	// Errors from graphs
	SBSARMgr_Error_InvalidGraphXML,
	SBSARMgr_Error_DuplicateGraphInputs,
	SBSARMgr_Error_DuplicateGraphOutputs,
	SBSARMgr_Error_DuplicateGraphPresets,
	SBSARMgr_Error_PresetNotFound,
	SBSARMgr_Error_PresetAlreadyExists,
	SBSARMgr_Error_PresetInputNotFoundInGraph,
	SBSARMgr_Error_PresetInputIsOfDifferentType,
	SBSARMgr_Error_PresetInputValueCouldNotBeApplied,
	SBSARMgr_Error_UnableToAddPreset,
	SBSARMgr_Error_InvalidGraphName,
	SBSARMgr_Error_InvalidGraphNameCount,
	SBSARMgr_Error_InvalidGraphNames,
	SBSARMgr_Error_InvalidGraphInfoPointer,
	SBSARMgr_Error_InvalidGraphInfoLabel,
	SBSARMgr_Error_InvalidGraphCountPointer,
	SBSARMgr_Error_InvalidGraphIndex,

	// Errors related to metadata
	SBSARMgr_Error_InvalidMetadata,
	SBSARMgr_Error_InvalidMetadataType,
	SBSARMgr_Error_InvalidMetadataSource,
	SBSARMgr_Error_InvalidMetadataBuffer,
	SBSARMgr_Error_InvalidMetadataTypeSourceCombination,
	SBSARMgr_Error_NoPreExistingMetadata,

	// SBSAR-related errors
	SBSARMgr_Error_InvalidSBSARFileName,
	SBSARMgr_Error_CannotOpenSBSARFile,
	SBSARMgr_Error_CannotReadSBSARFile,
	SBSARMgr_Error_CannotDecodeSBSARFile,
	SBSARMgr_Error_CannotGetSBSARFileCount,
	SBSARMgr_Error_CannotGetSBSARInternalFilePath,
	SBSARMgr_Error_CannotGetSBSARInternalFileContent,
	SBSARMgr_Error_CannotDecodeSBSASM,
	SBSARMgr_Error_UnsupportedSBSASMFeatures,
	SBSARMgr_Error_DuplicateInputsInSBSASM,
	SBSARMgr_Error_CannotParseSBSARInternalXML,
	SBSARMgr_Error_CannotCompressSBSAR,
	SBSARMgr_Error_CannotCreateTempFile,
	SBSARMgr_Error_CannotWriteTempFile,
	SBSARMgr_Error_CannotCopyTempFileToOutputSBSARFile,
	SBSARMgr_Error_InputNotFound,
	SBSARMgr_Error_GraphNotFound,
	SBSARMgr_Error_NoWayToSelectGraph,

	// Icon-related errors
	SBSARMgr_Error_GraphHasNoIcon,
	SBSARMgr_Error_GraphAlreadyHasAnIcon,
	SBSARMgr_Error_CannotOpenIconFile,
	SBSARMgr_Error_CannotReadIconFile,
	SBSARMgr_Error_CannotWriteIconData,
	SBSARMgr_Error_InvalidIconName,

	// API-related errors
	SBSARMgr_Error_InvalidHandle,
	SBSARMgr_Error_InvalidHandlePointer,
	SBSARMgr_Error_CouldNotAllocateMemoryForHandle,

} SBSARMgrError;

#endif // _ALG_SBSAR_MANAGER_ERROR_H
