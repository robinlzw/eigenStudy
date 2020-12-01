#pragma once
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <sys/trn50/isyscut.h>
#include <obj/alg/ialgteethpatching.h>
//////////////////////////////////////////////////////////////////////////
void PatchToothSide(std::vector<VFVECTOR3>& vAddVert, std::vector<unsigned>& ctrlPoints, 
	const VSConstBuffer<unsigned>& cbBndry, const VSPerfectMesh& pm);