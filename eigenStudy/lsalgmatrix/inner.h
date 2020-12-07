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
#include <obj/alg/ialggumgenerating.h>
#include <pkg/ldckupdater.h>
#include <pkg/ldwzkupdater.h>

//////////////////////////////////////////////////////////////////////////
enum E_VERT_TYPE
{
	E_VTYPE_CENTER = 1,
	E_VTYPE_CTRLPOINT1,
	E_VTYPE_CTRLPOINT2,
	E_VTYPE_BTMLINE,
	E_VTYPE_VAR,
};