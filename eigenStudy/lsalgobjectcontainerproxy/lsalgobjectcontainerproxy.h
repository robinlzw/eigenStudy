#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <obj/ialgmesh/ialgmesh.h>
#include <sys/alg/isysmesh.h>
#include <sys/trn50/isyscut.h>
#include <obj/alg/ialgobjectcontainerproxy.h>

namespace NMALG_OBJECTCONTAINERPROXY
{
	typedef void* (FUNCCreateObj)(const unsigned nObjEnum);
	typedef void (FUNCReleaseObj)(void* pObj);	
}
