#include "stdafx.h"

char *program_text[1];
size_t program_length[1];
TOutput program_output;

bool make_program_path(char *szPathName, int iPathCharacters, char *szProgramName)
{
	char szPath[_MAX_PATH + 1];
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	szPathName[0] = 0; // always null-terminate the return path

	// get the current directory
	if (0 == GetCurrentDirectoryA(_countof(szPath), szPath))
		return false;

	// get the directory and program name components
	_splitpath_s(szPath, szDrive, _countof(szDrive), szDir, _countof(szDir), NULL, 0, NULL, 0);
	_splitpath_s(szProgramName, NULL, 0, NULL, 0, szFName, _countof(szFName), szExt, _countof(szExt));

	// make the new path
	_makepath_s(szPathName, iPathCharacters, szDrive, szDir, szFName, szExt);

	return true;
}
bool load_program(char *program_name, char **buf, size_t *ret_length) // *buf must be NULL or it will be freed
{
	// free any existing memory
	if (*buf != NULL)
	{
		free(*buf);
		*buf = NULL;
	}
	// open the file
	FILE *fh = fopen(program_name, "rt");
	if (fh == NULL)
		return false;
	fseek(fh, 0, SEEK_END);
	*ret_length = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	// load the file
	*buf = (char*)calloc(1, *ret_length + 1);
	if (*buf != NULL)
	{
		fread(*buf, *ret_length, 1, fh);
		(*buf)[*ret_length - 1] = 0;
	}
	fclose(fh);
	return *buf != NULL;
}

bool load_program(char *program_name)
{
	// free any existing memory
	if (program_text[0] != NULL)
	{
		free(program_text[0]);
		program_text[0] = NULL;
	}
	// open the file
	FILE *fh = fopen(program_name, "rt");
	if (fh == NULL)
		return false;
	fseek(fh, 0, SEEK_END);
	program_length[0] = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	// load the file
	program_text[0] = (char*)calloc(1, program_length[0] + 1);
	if (program_text[0] != NULL)
	{
		fread(program_text[0], program_length[0], 1, fh);
		program_text[0][program_length[0]-1] = 0;
	}
	fclose(fh);
	return program_text[0] != NULL;
}

bool test_programs(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;

	program_output.appendLine("Staring Program test:");

	// load the program
	program_output.appendLine("Loading Program program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_program.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	program_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &program_text[0], &program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		program_output.appendLine(szBuf);
		return false;
	}

	// create a Context for the Program
	context = clCreateContext(NULL, 1, &devices[iDeviceIndex], NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		program_output.appendLine("Context creation succeeded...");
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)program_text, program_length, &error);
		if (error == CL_SUCCESS)
		{
			program_output.appendLine("Program creation succeeded...");
			cl_int build_error = clBuildProgram(program, 1, &devices[iDeviceIndex], NULL, NULL, NULL);
//				program_output.appendLine("Program build succeeded...");

				// get the program build information
				char build_log[1024];
				size_t size_ret;
				error = clGetProgramBuildInfo(program, devices[iDeviceIndex], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &size_ret);
				if (error == CL_SUCCESS)
				{
					build_log[size_ret] = 0;
					program_output.appendLine(build_log);

					// get the program source code
					error = clGetProgramBuildInfo(program, devices[iDeviceIndex], CL_PROGRAM_SOURCE, sizeof(build_log), build_log, &size_ret);
					return true;
				}
		}
	}
	return false;
}
