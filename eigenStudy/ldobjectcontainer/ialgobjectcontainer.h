#pragma once
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <obj/lsalgtools/ilsalgtools.h>

enum E_OBJ_TYPE
{
	OBJ_CUTPATH_GEN_E = 1,
	//OBJ_STRMESH_GEN_E,
	OBJ_MESHCONVEXHULL_GEN_E,
	OBJ_MESHSIMPLIFY_GEN_E,
	OBJ_OCCLUSALPAD_GEN_E,
	OBJ_DETERMINEMARKPOSITION_GEN_E,
	OBJ_FILLWAX2_GEN_E,
	OBJ_MESHRAYINTERSECTION_GEN_E,		// ÉäÏß²â¾à
	OBJ_AUTOALINEMENT_GEN_E,	
	OBJ_CUTPATHABB_GEN_E,
	OBJ_JOINBNDRY_GEN_E,
	OBJ_DENTALARCHCURVE2_GEN_E,	
	OBJ_MESHSIMPLIFY2_GEN_E,			// ¾«¼òÍø¸ñ
	OBJ_DENTALARCHCURVENEW_GEN_E,
	OBJ_GEN_MAX_E,
};


