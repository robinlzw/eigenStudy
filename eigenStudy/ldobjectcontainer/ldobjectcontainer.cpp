// ldteethprocess.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
#include "ldalgobjectcontainer.h"
#include <pkg/ldobjectcontainer.h>

static NMALG_OBJECTCONTAINER::VObjManager g_objManager;

LDOBJECTCONTAINER_API  void OBJ_Release(IVObject* pObj)
{
	g_objManager.ReleaseObj(pObj);
}


LDOBJECTCONTAINER_API void* OBJ_Create(const unsigned nObjEnum)
{
	return g_objManager.CreateObj(nObjEnum);
}
