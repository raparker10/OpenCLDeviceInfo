#pragma once

#include "resource.h"


struct TCLInfo {
	const char *cl_device_info_name;	// text name of value being requested / stored
	const int cl_device_info;			// which device parameter is being requested /stored
	const clTypeID type_id;				// type of data expected in .val
	size_t size_ret;					// number of bytes returned by system
	clVal val;							// value returned by system
	char s[1024];						// textual representation of .val
	int num_lines;							// number of lines of text in .s
};

