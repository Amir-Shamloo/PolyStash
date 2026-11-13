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

/*
	Used by Substance Linker to generate dedicated binaries and by engines
	version structure (see version.h).
*/

#ifndef _SUBSTANCE_ENGINEID_H
#define _SUBSTANCE_ENGINEID_H


/** Basic type defines */
#include <stddef.h>


/** @brief Substance engine ID enumeration */
typedef enum
{
	Substance_EngineID_INVALID      = 0x0,    /**< Unknown engine, do not use */

	Substance_EngineID_ogl3_arm64    = 0x0F,    /**< OpenGL3 (arm64) */
	Substance_EngineID_ogl3_x64      = 0x0C,    /**< OpenGL3 (x64)   */
	Substance_EngineID_ogl3m1        = Substance_EngineID_ogl3_arm64, /**< Deprecated */
	Substance_EngineID_ogl3          = Substance_EngineID_ogl3_x64,   /**< Deprecated */

	Substance_EngineID_d3d11_arm64   = 0x07,    /**< Direct3D 11 Windows (arm64) */
	Substance_EngineID_d3d11_x64     = 0x0B,    /**< Direct3D 11 Windows (x64) */
	Substance_EngineID_d3d11pc       = Substance_EngineID_d3d11_x64, /**< Deprecated */

	Substance_EngineID_stdc          = 0x10,   /**< Pure Software (at least) engine */
	Substance_EngineID_sse2          = 0x13,   /**< SSE2 (minimum) CPU engine */
	Substance_EngineID_neon_m        = 0x1B,   /**< NEON CPU engine for mobile targets (iOS, Android) */
	Substance_EngineID_neon          = Substance_EngineID_neon_m, /**< Deprecated */
	Substance_EngineID_neon_d        = 0x16,   /**< NEON CPU engine for desktop machines */
	Substance_EngineID_neonm1        = Substance_EngineID_neon_d, /**< Deprecated */

	Substance_EngineID_vk_arm64      = 0x17,   /**< Vulkan (arm64) */
	Substance_EngineID_vk_x64        = 0x12,   /**< Vulkan (x64)   */
	Substance_EngineID_vkm1          = Substance_EngineID_vk_arm64,     /**< Deprecated */
	Substance_EngineID_vk            = Substance_EngineID_vk_x64,       /**< Deprecated */

	Substance_EngineID_mtl_arm64     = 0x03,   /**< Metal (arm64) */
	Substance_EngineID_mtl_x64       = 0x02,   /**< Metal (x64)   */
	Substance_EngineID_mtlm1         = Substance_EngineID_mtl_arm64,     /**< Deprecated */
	Substance_EngineID_mtl           = Substance_EngineID_mtl_x64,       /**< Deprecated */

	Substance_Engine_Platform_BLEND    = 0x0,  /**< BLEND platform API    */
	Substance_Engine_Platform_d3d11    = 0x11, /**< Direct3D platform API */
	Substance_Engine_Platform_ogl3     = 0x19, /**< OpenGL platform API   */
	Substance_Engine_Platform_vulkan   = 0x14, /**< Vulkan platform API   */
	Substance_Engine_Platform_metal    = 0x18, /**< Metal platform API    */

	/* etc. */

	Substance_EngineID_FORCEDWORD   = 0xFFFFFFFF /**< Force DWORD enumeration */

} SubstanceEngineIDEnum;


#endif /* ifndef _SUBSTANCE_ENGINEID_H */
