/**
*** INTEL CONFIDENTIAL
***
*** Copyright (March 2011) (March 2011) Intel Corporation All Rights Reserved.
*** The source code contained or described herein and all documents related to the
*** source code ("Material") are owned by Intel Corporation or its suppliers or
*** licensors. Title to the Material remains with Intel Corporation or its
*** suppliers and licensors. The Material contains trade secrets and proprietary
*** and confidential information of Intel or its suppliers and licensors.
*** The Material is protected by worldwide copyright and trade secret laws
*** and treaty provisions. No part of the Material may be used, copied,
*** reproduced, modified, published, uploaded, posted, transmitted, distributed,
*** or disclosed in any way without Intel's prior express written permission.
***
*** No license under any patent, copyright, trade secret or other intellectual
*** property right is granted to or conferred upon you by disclosure or delivery
*** of the Materials, either expressly, by implication, inducement, estoppel or
*** otherwise. Any license under such intellectual property rights must be
*** express and approved by Intel in writing.
***
*** Input Library: Foreground window
*** Group 3: Thy, Milon, Pranav
**/
#ifdef _MSC_VER
// Quiet PCH Warning: header stop cannot be in a macro or #if block.
#pragma once 
#endif // _MSC_VER

#ifndef __INCLUDE_FOREGROUND_WINDOW__
#define __INCLUDE_FOREGROUND_WINDOW__

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#if defined __PL_WINDOWS__
#include <windows.h>
#endif // __PL_WINDOWS__

#include "pub_intel_modeler.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-----------------------------------------------------------------------------
// Defines.
//-----------------------------------------------------------------------------
#define INPUT_NAME_STRING "FOREGROUND-WINDOW"

#define INPUT_PAUSE_IN_MS 1000
#define STRING_BUFFERS_SIZE 1024
#define STRING_BUFFERS_SIZE_2 2048
#define PATH_SEPARATOR_CHAR "\\"
#define NAME_SEPARATOR_CHAR "-"
#define CLASS_NAME_SEPARATOR_CHAR "["

#define INPUTS_COUNT 12
#define ROOT_ID 0
#define PROCESS_ID 1
#define THREAD_ID 2
#define WINDOW_NAME 3
#define IMAGE_NAME 4
#define CLASS_NAME 5
#define X_UPPER_LEFT 6
#define X_LOWER_RIGHT 7
#define Y_UPPER_LEFT 8
#define Y_LOWER_RIGHT 9
#define IS_HUNG 10
#define IS_IMMERSIVE 11

#define INPUT_DESCRIPTION_STRINGS \
		"Foreground Window Root ID", \
		"Foreground Window Process ID", \
		"Foreground Window Thread ID", \
		"Foreground Window Name", \
		"Foreground Window Image Name", \
		"Foreground Window Class Name", \
		"Window Upper Left X Coordinate", \
		"Window Lower Right X Coordinate", \
		"Window Upper Left Y Coordinate", \
		"Window Lower Right Y Coordinate", \
		"Check if the App is Hung or Not", \
		"Check if the App is Immersive or Not"

#define INPUT_TYPES \
		ULL_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER, \
		STRING_COUNTER, \
		STRING_COUNTER, \
		STRING_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER, \
		ULL_COUNTER

//-----------------------------------------------------------------------------
// Function prototypes.
//-----------------------------------------------------------------------------
ESRV_API ESRV_STATUS modeler_init_inputs(
	unsigned int*,
	int*,
	int*,
	char*,
	size_t
);
ESRV_API ESRV_STATUS modeler_open_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_API ESRV_STATUS modeler_close_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_read_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_listen_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_process_dctl(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_process_lctl(PINTEL_MODELER_INPUT_TABLE);

/*-----------------------------------------------------------------------------*/
#ifdef __PL_WINDOWS__
ESRV_API unsigned int __stdcall foreground_window_event_listener_thread(void*);
#endif // __PL_WINDOWS__

#if defined (__PL_LINUX__) || (__PL_SOLARIS__) || (__PL_MACOSX__)
ESRV_API void* foreground_window_event_listener_thread(void*);
#endif // __PL_LINUX__ || __PL_SOLARIS__ || __PL_MACOSX__
/*-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INCLUDE_FOREGROUND_WINDOW__
