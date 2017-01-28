#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "output.h"

TOutput::TOutput()
{
	pszLines = (char**)malloc(sizeof(char*) * 1024);
	int iLineCount = 0;
	int iCurrentLine = 0;

}
TOutput::~TOutput()
{
	for (int i = 0; i < iLineCount; ++i)
	{
		free(pszLines[i]);
		pszLines[i] = NULL;
	}
}

bool TOutput::appendLine(const char * const pszLine)
{
	if (iLineCount < 1024)
	{
		pszLines[iLineCount++] = strdup(pszLine);
		return true;
	}
	return false;
}
int TOutput::appendMultiLine(const char * const pszLines)
{
	char *s = strdup(pszLines);
	int iLines = 0;
	char *ptr = strtok(s, "\n");
	while (ptr != NULL)
	{
		iLines++;
		appendLine(strdup(ptr));
		ptr = strtok(NULL, "\n");
	}
	return iLines;
}
