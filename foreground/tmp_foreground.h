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
**/

#ifndef __INCLUDE_FOREGROUND_WINDOW_INPUT__
#define __INCLUDE_FOREGROUND_WINDOW_INPUT__

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "pub_intel_modeler.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	/*--------------------------------------------------------------------------*/

	//-----------------------------------------------------------------------------
	// Defines.
	//-----------------------------------------------------------------------------

#define INPUT_PAUSE_IN_S 3
#define PATH_SEPARATOR_CHAR '\\'
#define STRING_BUFFERS_SIZE 1024
#define STRING_BUFFERS_SIZE_2 2048

#define INPUTS_COUNT 14
#define PID_INDEX 0
#define TID_INDEX 1
#define WINDOW_TITLE_INDEX 2
#define WINDOW_MODULE_INDEX 3
#define WINDOW_CLASS_INDEX 4
#define WINDOW_DISPLAY_INDEX 5
#define WINDOW_LEFT_INDEX 6
#define WINDOW_TOP_INDEX 7
#define WINDOW_RIGHT_INDEX 8
#define WINDOW_BOTTOM_INDEX 9
#define WINDOW_WIDTH_INDEX 10
#define WINDOW_HEIGHT_INDEX 11
#define WINDOW_STYLE_INDEX 12
#define WINDOW_EXTENDED_STYLE_INDEX 13

#define INPUT_NAME_STRING "FWND"

#define INPUT_DESCRIPTION_STRINGS \
	"OS:DWM:PROCESS:PID::", \
	"OS:DWM:PROCESS:TID::", \
	"OS:DWM:WINDOW:TITLE::", \
	"OS:DWM:WINDOW:MODULE::", \
	"OS:DWM:WINDOW:CLASS::", \
	"OS:DWM:WINDOW:DISPLAY::", \
	"OS:DWM:WINDOW:LEFT:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:TOP:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:RIGHT:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:BOTTOM:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:WIDTH:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:HEIGHT:LOGICAL_UNIT:", \
	"OS:DWM:WINDOW:STYLE::", \
	"OS:DWM:WINDOW:EXTENDED_STYLE::", \

#define INPUT_TYPES \
	ULL_COUNTER, \
	ULL_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER

#define WAIT_EVENTS_COUNT (2)
#define STOP_EVENT_INDEX (0)
#define CLICK_EVENT_INDEX (1)

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
ESRV_API unsigned int __stdcall time_tick_thread(void*);
ESRV_API unsigned int __stdcall mouse_hook_listener_thread(void*);
unsigned int __stdcall get_object_info(void*);
unsigned int __stdcall mouse_messages_loop(void*);
LRESULT CALLBACK process_mouse_messages(int, WPARAM, LPARAM);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INCLUDE_FOREGROUND_WINDOW_INPUT__
