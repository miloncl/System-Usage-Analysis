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

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <process.h> // for _beginthreadex
#include <windows.h>
#include <tchar.h>
#include <Psapi.h> // GetProcessImageFileName
// Kernel32.dll //getpackagefullname
#include "foreground_window.h"

//-----------------------------------------------------------------------------
// generic variables
//-----------------------------------------------------------------------------
unsigned long long x = 0;
unsigned long long y = 0;

//-----------------------------------------------------------------------------
// Mouse-hook Event listener variables
//-----------------------------------------------------------------------------
BOOL f_stop = FALSE;
HHOOK h_mouse_hook = NULL;
POINT click_position = { 0 };
CRITICAL_SECTION cs = { NULL };
CRITICAL_SECTION prev_app_cs = { NULL };
BOOL f_app_cs_initialized = FALSE;
HANDLE h_click_detected = NULL;

//-----------------------------------------------------------------------------
// Custom event-listeners thread data
//-----------------------------------------------------------------------------
HANDLE time_tick_thread_handle = NULL;
HANDLE mouse_hook_thread = NULL;

DWORD time_tick_thread_id = 0;
DWORD mouse_hook_thread_id = 0;

HANDLE h_stop = NULL;
DWORD last_pid = 0;
wchar_t last_exe[STRING_BUFFERS_SIZE] = L"";

//-----------------------------------------------------------------------------
// Path Separators
//-----------------------------------------------------------------------------
wchar_t* PATH_SEPARATOR_CHAR_UNI = L"\\";
wchar_t* DISPLAY_SEPARATOR_CHAR = L".";

/*-----------------------------------------------------------------------------
Function: modeler_init_inputs
Purpose : return the inputs count.
In      : pointer to an unsigned integer.
Out     : modified unsigned integer.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_init_inputs(
	unsigned int* p,
	int* pfd,
	int* pfe,
	char* po,
	size_t so
) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);
	assert(pfd != NULL);
	assert(pfe != NULL);

	SIGNAL_PURE_EVENT_DRIVEN_MODE;
	SET_INPUTS_COUNT(INPUTS_COUNT);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}

/*-----------------------------------------------------------------------------
Function: modeler_open_inputs
Purpose : open inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_open_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	unsigned int i = 0;

	//-------------------------------------------------------------------------
	// Input descriptions.
	//-------------------------------------------------------------------------
	static char* descriptions[INPUTS_COUNT] = {
		INPUT_DESCRIPTION_STRINGS
	};
	static INTEL_MODELER_INPUT_TYPES types[INPUTS_COUNT] = {
		INPUT_TYPES
	};

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);
	InitializeCriticalSection(&prev_app_cs);
	f_app_cs_initialized = TRUE;
	//-------------------------------------------------------------------------
	// Set input information.
	//-------------------------------------------------------------------------
	SET_INPUTS_NAME(INPUT_NAME_STRING);
	//-------------------------------------------------------------------------
	for (i = 0; i < INPUTS_COUNT; i++) {
		SET_INPUT_DESCRIPTION(
			i,
			descriptions[i]
		);
		SET_INPUT_TYPE(
			i,
			types[i]
		);
	} // for i (each input)

	//-------------------------------------------------------------------------
	// Start the pure event-driven thread
	//-------------------------------------------------------------------------
	time_tick_thread_handle = (HANDLE)_beginthreadex(
		NULL,
		0,
		time_tick_thread,
		(void*)p,
		0,
		(unsigned int*)&time_tick_thread_id
	);
	if (time_tick_thread_handle == NULL) {
		goto modeler_open_inputs_error;
	}

	//-------------------------------------------------------------------------
	// Start the pure event-driven thread
	//-------------------------------------------------------------------------
	mouse_hook_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		mouse_hook_listener_thread,
		(void*)p,
		0,
		(unsigned int*)&mouse_hook_thread_id
	);
	if (mouse_hook_thread == NULL) {
		goto modeler_open_inputs_error;
	}

	//-------------------------------------------------------------------------
	// Create custom mouse hook exit event.
	//-------------------------------------------------------------------------
	h_stop = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_stop == NULL) {
		goto modeler_open_inputs_error;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

		modeler_open_inputs_error:

	return(ESRV_FAILURE);

}

/*-----------------------------------------------------------------------------
Function: modeler_close_inputs
Purpose : close inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_close_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	BOOL bret = FALSE;

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);
	if (f_app_cs_initialized == TRUE) {
		DeleteCriticalSection(&prev_app_cs);
	}
	//-------------------------------------------------------------------------
	// Stop our event thread.
	//-------------------------------------------------------------------------
	if (h_stop != NULL) {
		bret = SetEvent(h_stop);
		if (bret == FALSE) {
			goto modeler_close_inputs_error;
		}
	}

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_stop != NULL) {
		bret = CloseHandle(h_stop);
		if (bret == FALSE) {
			goto modeler_close_inputs_error;
		}
		h_stop = NULL;
	}
	//-------------------------------------------------------------------------

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

		modeler_close_inputs_error:

	return(ESRV_FAILURE);

}

/*-----------------------------------------------------------------------------
Function: modeler_read_inputs
Purpose : collect all inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_read_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_listen_inputs
Purpose : listen for all inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_listen_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_process_dctl
Purpose : process DCTL commands on DCTL interrupt notification.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_process_dctl(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_process_lctl
Purpose : process LCTL commands on LCTL interrupt notification.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_process_lctl(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Function: time_tick_thread
Purpose : implement the pure event listener thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall time_tick_thread(void* px) {
	\

		//-------------------------------------------------------------------------
		// Generic variables.
		//-------------------------------------------------------------------------
		DWORD dwret = 0;
	DWORD pid = 0;
	DWORD tid = 0;
	BOOL bret = FALSE;
	BOOL window_is_immersive = FALSE;
	int window_is_same_exe = FALSE;

	//-------------------------------------------------------------------------
	// Foreground window handle and process
	//-------------------------------------------------------------------------
	HWND f_handle = NULL;
	HANDLE f_process = NULL;
	HANDLE curr_process = NULL;

	//-------------------------------------------------------------------------
	// String buffers
	//-------------------------------------------------------------------------
	wchar_t uni_title_buffer[STRING_BUFFERS_SIZE] = { L'\0' };
	wchar_t uni_module_name_buffer[STRING_BUFFERS_SIZE_2] = { L'\0' };
	wchar_t uni_class_name_buffer[STRING_BUFFERS_SIZE] = { L'\0' };

	//-------------------------------------------------------------------------
	// Tokenizer Pointers
	//-------------------------------------------------------------------------
	wchar_t* uni_title_token = NULL;
	wchar_t* uni_title_next_token = NULL;
	wchar_t* uni_title = NULL;

	wchar_t* uni_module_name_token = NULL;
	wchar_t* uni_module_name_next_token = NULL;
	wchar_t* uni_module_name = NULL;

	wchar_t* display_name = NULL;
	wchar_t* display_name_token = NULL;
	wchar_t* display_name_next_token = NULL;

	//-------------------------------------------------------------------------
	// Window Rectangle and Style
	//-------------------------------------------------------------------------
	RECT window_rect = { 0 };
	LONG_PTR window_style = NULL;
	LONG_PTR window_extended_style = NULL;

	//-------------------------------------------------------------------------
	// Monitor Variables.
	//-------------------------------------------------------------------------
	DWORD monitor_dw_flags = MONITOR_DEFAULTTONEAREST;
	MONITORINFOEXW monitor_info = { 0 };
	HANDLE monitor_handle = NULL;

	//-------------------------------------------------------------------------
	// Watchdog variables.
	//-------------------------------------------------------------------------
	WATCHDOG_VARIABLES

		//-------------------------------------------------------------------------
		// Access helper variables.
		//-------------------------------------------------------------------------
		PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(px != NULL);
	p = (PINTEL_MODELER_INPUT_TABLE)px;
	assert(p != NULL);

	//-------------------------------------------------------------------------
	// Name this thread (for debug mode only).
	//-------------------------------------------------------------------------
	INPUT_DIAGNOSTIC_NAME_THIS_THREAD(
		p,
		"time_tick_thread"
	);

	//-------------------------------------------------------------------------
	// Register this thread with watchdog.
	//-------------------------------------------------------------------------
	INPUT_REGISTER_EVENT_LOCKED_THREAD_WITH_WATCHDOG(
		p,
		"time_tick_thread",
		time_tick_thread_handle,
		time_tick_thread_id,
		STOP_SIGNAL,
		time_tick_thread_exit
	);

	while (STOP_REQUEST == 0) {

		//---------------------------------------------------------------------
		// Pause to simulate event triggering.
		// Note:
		//    Rather than using a sleep, which would lock the event listener 
		//    thread, we recommend using the method shown below. In general
		//    developers of event-driven input libraries should add into the
		//    end condition the event / semaphore via the STOP_SIGNAL macro 
		//    (also - but not instead - use the STOP_REQUEST macro).
		//---------------------------------------------------------------------
		dwret = WaitForSingleObject(
			STOP_SIGNAL,
			INPUT_PAUSE_IN_S * 1000
		);
		switch (dwret) {
		case WAIT_OBJECT_0:
			goto time_tick_thread_exit; // time to leave!
			break;
		case WAIT_TIMEOUT:
			break; // all good, time to make a measurement
		default:
			goto time_tick_thread_exit; // error condition
		} // switch

		//---------------------------------------------------------------------
		// Retrieve the foreground window, tid, and PID
		//---------------------------------------------------------------------
		f_handle = GetForegroundWindow();

		if (f_handle == NULL) {
			continue;
		}

		tid = GetWindowThreadProcessId(
			f_handle,
			&pid
		);

		//---------------------------------------------------------------------
		// If we identify the same process, we skip this sample.
		// Note: It is unlikely that the same process ID would be recycled 
		// immediately, but two handle might have the same PID.
		//---------------------------------------------------------------------

		curr_process = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			pid
		);
		if (curr_process == NULL) {
			continue;
		}

		//---------------------------------------------------------------------
		// Get the name of the window and class name
		//---------------------------------------------------------------------
		dwret = GetProcessImageFileNameW(
			curr_process,
			uni_module_name_buffer,
			sizeof(uni_module_name_buffer)
		);
		uni_module_name_token = wcstok_s(
			uni_module_name_buffer,
			PATH_SEPARATOR_CHAR_UNI,
			&uni_module_name_next_token
		);
		while (uni_module_name_token != NULL)
		{
			uni_module_name = uni_module_name_token;
			uni_module_name_token = wcstok_s(
				NULL,
				PATH_SEPARATOR_CHAR_UNI,
				&uni_module_name_next_token
			);
		}
		bret = CloseHandle(curr_process);
		curr_process = NULL;

		if (
			(uni_module_name != NULL) &&
			(last_exe != NULL)
			) {
			window_is_same_exe = wcscmp(
				last_exe,
				uni_module_name
			);
		}

		/*if (
			(last_pid == pid) ||
			(window_is_same_exe == TRUE)
		) {
			window_is_same_exe = FALSE;
			continue;
		}*/
		if (window_is_same_exe == 0) {
			window_is_same_exe = FALSE;
			continue;
		}
		/*if (last_pid == pid) {
			continue;
		}*/
		last_pid = pid;
		wcscpy_s(
			last_exe,
			sizeof(last_exe) / sizeof(wchar_t),
			uni_module_name
		);

		(void)GetWindowTextW(
			f_handle,
			uni_title_buffer,
			sizeof(uni_title_buffer) / sizeof(wchar_t)
		);
		uni_title_token = wcstok_s(
			uni_title_buffer,
			PATH_SEPARATOR_CHAR_UNI,
			&uni_title_next_token
		);
		while (uni_title_token != NULL)
		{
			uni_title = uni_title_token;
			uni_title_token = wcstok_s(
				NULL,
				PATH_SEPARATOR_CHAR_UNI,
				&uni_title_next_token
			);
		}

		// Find the classname
		(void)GetClassNameW(
			f_handle,
			uni_class_name_buffer,
			sizeof(uni_class_name_buffer) / sizeof(wchar_t)
		);

		//---------------------------------------------------------------------
		// Find the upper-left and lower-right corner of the window rectangle
		//---------------------------------------------------------------------
		(void)GetWindowRect(
			f_handle,
			&window_rect
		);

		//---------------------------------------------------------------------
		// Find the monitor the window belongs to and extract information
		//---------------------------------------------------------------------
		monitor_handle = MonitorFromWindow(
			f_handle,
			monitor_dw_flags
		);

		monitor_info.cbSize = sizeof(MONITORINFOEXW);
		bret = GetMonitorInfo(
			monitor_handle,
			&monitor_info
		);
		if (bret == FALSE) { //GetMonitorInfo failed
			// Placeholder to log this error message
			continue;
		};

		//---------------------------------------------------------------------
		// Set the monitor values
		//---------------------------------------------------------------------
		display_name = monitor_info.szDevice;
		display_name_token = wcstok_s(
			display_name,
			DISPLAY_SEPARATOR_CHAR,
			&display_name_next_token
		);
		while (display_name_token != NULL)
		{
			display_name = display_name_token;
			display_name_token = wcstok_s(
				NULL,
				DISPLAY_SEPARATOR_CHAR,
				&display_name_next_token);
		}

		//---------------------------------------------------------------------
		// See if the window is a Windows store app
		//---------------------------------------------------------------------
		window_is_immersive = IsImmersiveProcess(f_handle);

		//---------------------------------------------------------------------
		// Find the window style
		//---------------------------------------------------------------------
		window_style = GetWindowLongPtr(f_handle, GWL_STYLE);
		window_extended_style = GetWindowLongPtr(f_handle, GWL_EXSTYLE);

		//---------------------------------------------------------------------
		// Set values
		//---------------------------------------------------------------------
		SET_INPUT_ULL_VALUE(
			PID_INDEX,
			pid
		);
		SET_INPUT_ULL_VALUE(
			TID_INDEX,
			tid
		);
		if (uni_title != NULL) {
			SET_INPUT_UNICODE_STRING_ADDRESS(
				WINDOW_TITLE_INDEX,
				uni_title
			);
		}
		SET_INPUT_UNICODE_STRING_ADDRESS(
			WINDOW_MODULE_INDEX,
			uni_module_name
		);
		if (uni_class_name_buffer != NULL) {
			SET_INPUT_UNICODE_STRING_ADDRESS(
				WINDOW_CLASS_INDEX,
				uni_class_name_buffer
			);
		}
		SET_INPUT_UNICODE_STRING_ADDRESS(
			WINDOW_DISPLAY_INDEX,
			display_name
		)
			SET_INPUT_ULL_VALUE(
				WINDOW_LEFT_INDEX,
				window_rect.left
			);
		SET_INPUT_ULL_VALUE(
			WINDOW_TOP_INDEX,
			window_rect.top
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_RIGHT_INDEX,
			window_rect.right
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_BOTTOM_INDEX,
			window_rect.bottom
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_WIDTH_INDEX,
			window_rect.right - window_rect.left
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_HEIGHT_INDEX,
			window_rect.bottom - window_rect.top
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_STYLE_INDEX,
			window_style
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_EXTENDED_STYLE_INDEX,
			window_extended_style
		);
		/*SET_INPUT_ULL_VALUE(
			WINDOW_IS_IMMERSIVE,
			window_is_immersive
		);*/

		//---------------------------------------------------------------------
		// Trigger logging.
		//---------------------------------------------------------------------
		LOG_INPUT_VALUES;

	} // while

time_tick_thread_exit:

	//-------------------------------------------------------------------------
	// Un-register this thread with watchdog.
	//-------------------------------------------------------------------------
	INPUT_UNREGISTER_THREAD_WITH_WATCHDOG(
		p,
		time_tick_thread,
		time_tick_thread_id
	);

	//-------------------------------------------------------------------------
	// Success return hub.
	//-------------------------------------------------------------------------
	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

		//-------------------------------------------------------------------------
		// Failure return hub.
		//-------------------------------------------------------------------------
		return(ESRV_FAILURE);

}

/*-----------------------------------------------------------------------------
Function: mouse_hook_listener_thread
Purpose : implement the pure event listener thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall mouse_hook_listener_thread(void* px) {

	//-------------------------------------------------------------------------
	// Hook thread variables.
	//-------------------------------------------------------------------------
	HANDLE h_msg_loop_thread = NULL;
	DWORD msg_loop_thread_id = 0;
	HANDLE h_collector_thread = NULL;
	DWORD collector_thread_id = 0;

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(px != NULL);

	//-------------------------------------------------------------------------
	// Setup threads and synch data.
	//-------------------------------------------------------------------------
	InitializeCriticalSection(&cs);
	//-------------------------------------------------------------------------
	h_click_detected = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_click_detected == NULL) {
		goto mouse_hook_listener_thread_exit;
	}
	//-------------------------------------------------------------------------
	h_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		get_object_info,
		px,
		0,
		(unsigned int*)&collector_thread_id
	);
	if (h_collector_thread == NULL) {
		goto mouse_hook_listener_thread_exit;
	}
	//-------------------------------------------------------------------------
	h_msg_loop_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		mouse_messages_loop,
		NULL,
		0,
		(unsigned int*)&msg_loop_thread_id
	);
	if (h_msg_loop_thread == NULL) {
		goto mouse_hook_listener_thread_exit;
	}

	//-------------------------------------------------------------------------
	// Run the message loop!
	//-------------------------------------------------------------------------
	if (h_msg_loop_thread != NULL) {
		WaitForSingleObject(
			h_msg_loop_thread,
			INFINITE
		);
	}

mouse_hook_listener_thread_exit:

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_click_detected != NULL) {
		CloseHandle(h_click_detected);
		h_click_detected = NULL;
	}
	DeleteCriticalSection(&cs);
	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}

/*-----------------------------------------------------------------------------
Function: get_object_info
Purpose : measure and and store input data.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall get_object_info(void* pv) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	DWORD dwret = 0;
	DWORD pid = 0;
	DWORD tid = 0;
	BOOL bret = FALSE;
	int window_is_same_exe = FALSE;

	//-------------------------------------------------------------------------
	// Metrics extraction variables.
	//-------------------------------------------------------------------------
	POINT pt = { 0 };
	HWND h_root = NULL;
	HWND h_clicked = NULL;
	WINDOWINFO wi = { 0 };
	//-------------------------------------------------------------------------
	HWND handle = NULL;
	HANDLE h_process = NULL;
	HANDLE monitor_handle = NULL;

	//-------------------------------------------------------------------------
	// String buffers
	//-------------------------------------------------------------------------
	wchar_t uni_title_buffer[STRING_BUFFERS_SIZE] = { L'\0' };
	wchar_t uni_module_name_buffer[STRING_BUFFERS_SIZE] = { L'\0' };
	wchar_t uni_class_name_buffer[STRING_BUFFERS_SIZE] = { L'\0' };

	//-------------------------------------------------------------------------
	// Tokenizer Pointers
	//-------------------------------------------------------------------------
	wchar_t* uni_title_token = NULL;
	wchar_t* uni_title_next_token = NULL;
	wchar_t* uni_title = NULL;

	wchar_t* uni_module_name_token = NULL;
	wchar_t* uni_module_name_next_token = NULL;
	wchar_t* uni_module_name = NULL;

	wchar_t* display_name = NULL;
	wchar_t* display_name_token = NULL;
	wchar_t* display_name_next_token = NULL;

	//-------------------------------------------------------------------------
	// Window Rectangle and Style
	//-------------------------------------------------------------------------
	RECT window_rect = { 0 };
	LONG_PTR window_style = NULL;
	LONG_PTR window_extended_style = NULL;

	//-------------------------------------------------------------------------
	// Monitor Variables.
	//-------------------------------------------------------------------------
	DWORD monitor_dw_flags = MONITOR_DEFAULTTONEAREST;
	MONITORINFOEXW monitor_info = { 0 };

	//-------------------------------------------------------------------------
	// Ease access variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------
	// Wait variables.
	//-------------------------------------------------------------------------
	HANDLE wait_events[WAIT_EVENTS_COUNT] = { NULL, NULL };

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		//-------------------------------------------------------------------------
		// Get PILT pointer.
		//-------------------------------------------------------------------------
		assert(pv != NULL);
	p = (PINTEL_MODELER_INPUT_TABLE)pv;
	assert(p != NULL);

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[CLICK_EVENT_INDEX] = h_click_detected;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[CLICK_EVENT_INDEX] != NULL);

	//-------------------------------------------------------------------------
	// Waiting for the end of run.
	//-------------------------------------------------------------------------
	while (f_stop == FALSE) {

		//---------------------------------------------------------------------
		// Waiting for mouse event thread's signal.
		//---------------------------------------------------------------------
		dwret = WaitForMultipleObjects(
			WAIT_EVENTS_COUNT,
			wait_events,
			FALSE,
			INFINITE
		);
		switch (dwret) {
		case WAIT_OBJECT_0 + STOP_EVENT_INDEX:
			goto get_object_info_exit; // time to leave!
			break;
		case WAIT_OBJECT_0 + CLICK_EVENT_INDEX:
			break; // all good
		default:
			goto get_object_info_exit; // error condition
		} // switch

		//---------------------------------------------------------------------
		// Get click data - as fast as possible.
		//---------------------------------------------------------------------
		bret = TryEnterCriticalSection(&cs);
		if (bret == FALSE) {
			// Log the error here
			continue;
		}
		x = pt.x = click_position.x;
		y = pt.y = click_position.y;
		LeaveCriticalSection(&cs);
		//---------------------------------------------------------------------

		//---------------------------------------------------------------------
		// We have a bit more time to extract the inputs data now. Start with
		// the clicked window's handle.
		//---------------------------------------------------------------------
		h_clicked = WindowFromPoint(pt);
		//---------------------------------------------------------------------
		tid = GetWindowThreadProcessId(
			h_clicked,
			&pid
		);

		h_process = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			pid
		);
		if (h_process == NULL) {
			continue;
		}
		dwret = GetProcessImageFileNameW(
			h_process,
			uni_module_name_buffer,
			sizeof(uni_module_name_buffer)
		);
		uni_module_name_token = wcstok_s(
			uni_module_name_buffer,
			PATH_SEPARATOR_CHAR_UNI,
			&uni_module_name_next_token
		);

		if (
			(uni_module_name != NULL) &&
			(last_exe != NULL)
			) {
			window_is_same_exe = wcscmp(
				last_exe,
				uni_module_name
			);
		}

		/*if (
			(last_pid == pid) ||
			(window_is_same_exe == TRUE)
		) {
			window_is_same_exe = FALSE;
			continue;
		}*/
		if (window_is_same_exe == 0) {
			window_is_same_exe = FALSE;
			continue;
		}
		/*if (last_pid == pid) {
			continue;
		}*/
		last_pid = pid;
		wcscpy(
			last_exe,
			sizeof(last_exe) / sizeof(wchar_t),
			uni_module_name
		);

		//---------------------------------------------------------------------
		// Get the name of the window and class name
		//---------------------------------------------------------------------
		(void)GetWindowTextW(
			h_clicked,
			uni_title_buffer,
			sizeof(uni_title_buffer) / sizeof(wchar_t)
		);
		uni_title_token = wcstok_s(
			uni_title_buffer,
			PATH_SEPARATOR_CHAR_UNI,
			&uni_title_next_token
		);
		while (uni_title_token != NULL)
		{
			uni_title = uni_title_token;
			uni_title_token = wcstok_s(
				NULL,
				PATH_SEPARATOR_CHAR_UNI,
				&uni_title_next_token
			);
		}

		while (uni_module_name_token != NULL)
		{
			uni_module_name = uni_module_name_token;
			uni_module_name_token = wcstok_s(
				NULL,
				PATH_SEPARATOR_CHAR_UNI,
				&uni_module_name_next_token
			);
		}

		// Find the classname
		(void)GetClassNameW(
			h_clicked,
			uni_class_name_buffer,
			sizeof(uni_class_name_buffer) / sizeof(wchar_t)
		);
		bret = CloseHandle(h_process);
		h_process = NULL;

		//---------------------------------------------------------------------
		// Find the upper-left and lower-right corner of the window rectangle
		//---------------------------------------------------------------------
		(void)GetWindowRect(
			h_clicked,
			&window_rect
		);

		//---------------------------------------------------------------------
		// Find the monitor the window belongs to and extract information
		//---------------------------------------------------------------------
		monitor_handle = MonitorFromWindow(
			h_clicked,
			monitor_dw_flags
		);

		monitor_info.cbSize = sizeof(MONITORINFOEXW);
		bret = GetMonitorInfo(
			monitor_handle,
			&monitor_info
		);
		if (bret == FALSE) { //GetMonitorInfo failed
			// Placeholder to log this error message
			continue;
		};

		//---------------------------------------------------------------------
		// Set the monitor values
		//---------------------------------------------------------------------
		display_name = monitor_info.szDevice;
		display_name_token = wcstok_s(
			display_name,
			DISPLAY_SEPARATOR_CHAR,
			&display_name_next_token
		);
		while (display_name_token != NULL)
		{
			display_name = display_name_token;
			display_name_token = wcstok_s(
				NULL,
				DISPLAY_SEPARATOR_CHAR,
				&display_name_next_token);
		}

		//---------------------------------------------------------------------
		// Find the window style
		//---------------------------------------------------------------------
		window_style = GetWindowLongPtr(h_clicked, GWL_STYLE);
		window_extended_style = GetWindowLongPtr(h_clicked, GWL_EXSTYLE);

		//---------------------------------------------------------------------
		// Set values
		//---------------------------------------------------------------------
		SET_INPUT_ULL_VALUE(
			PID_INDEX,
			pid
		);
		SET_INPUT_ULL_VALUE(
			TID_INDEX,
			tid
		);
		if (uni_title != NULL) {
			SET_INPUT_UNICODE_STRING_ADDRESS(
				WINDOW_TITLE_INDEX,
				uni_title
			);
		}
		SET_INPUT_UNICODE_STRING_ADDRESS(
			WINDOW_MODULE_INDEX,
			uni_module_name
		);
		if (uni_title != NULL) {
			SET_INPUT_UNICODE_STRING_ADDRESS(
				WINDOW_CLASS_INDEX,
				uni_class_name_buffer
			);
		}
		SET_INPUT_UNICODE_STRING_ADDRESS(
			WINDOW_DISPLAY_INDEX,
			display_name
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_LEFT_INDEX,
			window_rect.left
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_TOP_INDEX,
			window_rect.top
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_RIGHT_INDEX,
			window_rect.right
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_BOTTOM_INDEX,
			window_rect.bottom
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_WIDTH_INDEX,
			window_rect.right - window_rect.left
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_HEIGHT_INDEX,
			window_rect.bottom - window_rect.top
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_STYLE_INDEX,
			window_style
		);
		SET_INPUT_ULL_VALUE(
			WINDOW_EXTENDED_STYLE_INDEX,
			window_extended_style
		);
		//SET_INPUT_ULL_VALUE(
		//	WINDOW_IS_IMMERSIVE,
		//	window_is_immersive
		//);

		//---------------------------------------------------------------------
		// Trigger logging.
		//---------------------------------------------------------------------
		LOG_INPUT_VALUES;
	}

get_object_info_exit:

	return(0);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: mouse_messages_loop
Purpose : mouse message hook function.
In      : pointer - as void - to a message structure.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall mouse_messages_loop(void* pv) {

	//-------------------------------------------------------------------------
	// Message handling variables.
	//-------------------------------------------------------------------------
	MSG message = { 0 };
	HINSTANCE h_instance = GetModuleHandle(NULL);

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		h_mouse_hook = SetWindowsHookEx(
			WH_MOUSE_LL,
			process_mouse_messages,
			h_instance,
			0
		);

	while (
		GetMessage(
			&message,
			NULL,
			0,
			0
		)
		) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(h_mouse_hook);

	return(0);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}

/*-----------------------------------------------------------------------------
Function: process_mouse_messages
Purpose : mouse event handler.
In      : mouse hook message data.
Out     : updated click position data.
Return  : status.
-----------------------------------------------------------------------------*/
LRESULT CALLBACK process_mouse_messages(
	int code,
	WPARAM wparam,
	LPARAM lparam
) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	BOOL bret = FALSE;

	//-------------------------------------------------------------------------
	// Message handling variables.
	//-------------------------------------------------------------------------
	MOUSEHOOKSTRUCT* p_mouse_struct = (MOUSEHOOKSTRUCT*)lparam;

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		//-------------------------------------------------------------------------
		// Do as MSDN says!
		//-------------------------------------------------------------------------
		if (code < 0) {
			goto process_mouse_messages_exit;
		}

	//-------------------------------------------------------------------------
	// Capture the data.
	//-------------------------------------------------------------------------
	if (
		(p_mouse_struct != NULL) &&
		(
			(wparam == WM_LBUTTONDOWN) ||
			(wparam == WM_RBUTTONDOWN)
			)
		) {
		bret = TryEnterCriticalSection(&cs);
		if (bret == FALSE) {
			goto process_mouse_messages_exit;
		}
		click_position.x = p_mouse_struct->pt.x;
		click_position.y = p_mouse_struct->pt.y;
		LeaveCriticalSection(&cs);
		SetEvent(h_click_detected);
	}

process_mouse_messages_exit:

	return(
		CallNextHookEx(
			h_mouse_hook,
			code,
			wparam,
			lparam
		)
		);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}
