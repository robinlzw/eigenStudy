// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//#define SPEED_DEBUG

#ifdef SPEED_DEBUG
#define VD_PROFILE_START( profile ) VSFProfile::StartProfile(#profile,"E:/testdata/profile.txt");
#define VD_PROFILE_FINISH( profile ) VSFProfile::FinishProfile(#profile,"E:/testdata/profile.txt");
#else
#define VD_PROFILE_START( profile )
#define VD_PROFILE_FINISH( profile )
#endif
// TODO: reference additional headers your program requires here
