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

#ifndef ALG_SBSAR_MANAGER_EXPORT_MACROS_H
#define ALG_SBSAR_MANAGER_EXPORT_MACROS_H

/* C/C++ macros */
#ifndef SBSAR_MGR_EXTERNC
	#ifdef __cplusplus
		#define SBSAR_MGR_EXTERNC extern "C"
	#else
		#define SBSAR_MGR_EXTERNC
	#endif
#endif

/* SBSAR_MGR_EXPORT macros */
#ifndef SBSAR_MGR_EXPORT
	#ifdef SBSAR_MGR_DLL
		#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
			#define SBSAR_MGR_EXPORT SBSAR_MGR_EXTERNC __declspec(dllexport)
		#elif defined(__GNUC__)
			#define SBSAR_MGR_EXPORT SBSAR_MGR_EXTERNC __attribute__((visibility("default")))
		#else
			#error Unknown compiler
		#endif
	#else
		#define SBSAR_MGR_EXPORT SBSAR_MGR_EXTERNC
	#endif
#endif

#endif /* ALG_SBSAR_MANAGER_EXPORT_MACROS_H */
