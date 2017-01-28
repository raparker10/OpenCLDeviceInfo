// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>


// TODO: reference additional headers your program requires here
#include <CL\cl.h>
#include "output.h"
#include "canvas.h"
#include "clTranslateType.h"
#include "clDeviceInfo.h"
#include "clPlatform.h"
#include "clContext.h"
#include "clProgram.h"
#include "clKernel.h"
#include "clBuffer.h"
#include "clMathScalar.h"
#include "clMathInteger.h"
#include "clMathVector.h"
#include "clImage.h"
#include "clEvent.h"
