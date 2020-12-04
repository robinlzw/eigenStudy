#pragma once
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <obj/lsalgtools/ilsalgtools.h>

enum E_OBJ_TYPE
{
	OBJ_CUTPATH_GEN_E = 1,				// 计算切割路径。
	OBJ_MESHCONVEXHULL_GEN_E,			// ？？？生成网格凸包？？？
	OBJ_MESHSIMPLIFY_GEN_E,				// 精简网格
	OBJ_OCCLUSALPAD_GEN_E,				//
	OBJ_DETERMINEMARKPOSITION_GEN_E,
	OBJ_FILLWAX2_GEN_E,					// 填蜡	
	OBJ_MESHRAYINTERSECTION_GEN_E,		// 射线测距
	OBJ_AUTOALINEMENT_GEN_E,			// 
	OBJ_CUTPATHABB_GEN_E,
	OBJ_JOINBNDRY_GEN_E,				// ？？？补洞？？？
	OBJ_DENTALARCHCURVE2_GEN_E,	
	OBJ_MESHSIMPLIFY2_GEN_E,			// 精简网格
	OBJ_DENTALARCHCURVENEW_GEN_E,
	OBJ_GEN_MAX_E,
};


