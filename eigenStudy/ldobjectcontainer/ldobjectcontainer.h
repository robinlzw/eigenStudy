#pragma once
#include <vector>
#include <string>
#include <iostream>
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LDCKUPDATER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LDCKUPDATER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef LDOBJECTCONTAINER_EXPORTS
#define LDOBJECTCONTAINER_API  __declspec(dllexport)
#else
#define LDOBJECTCONTAINER_API  __declspec(dllimport)
#endif

#include <obj/alg/ialgobjectcontainer.h>

#ifdef __cplusplus
	extern "C" {
#endif	
		LDOBJECTCONTAINER_API void* OBJ_Create(const unsigned nObjEnum);

		LDOBJECTCONTAINER_API void OBJ_Release(IVObject* pObj);
#ifdef __cplusplus
	}
#endif	
