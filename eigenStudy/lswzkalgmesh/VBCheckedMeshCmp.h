#pragma once
#include "VBEDCmpCombHandle.h"
#ifndef IN_CHTEST
#define IN_CHTEST
#include <pkg/ldcktest.h>
#endif

//网格压缩结果结构转换

class VBCheckedMeshCmp
{
public:
	VBCheckedMeshCmp() {}
	~VBCheckedMeshCmp() {}

	void Build(VNMesh::VSCompressedMesh & cmp, const VSSimpleMeshF & smesh, const unsigned & startCorner, const float & coeff)
	{
		Process(smesh, startCorner, coeff);

		cmp = checkedRes;
	}

private:
	void Process(const VSSimpleMeshF & smesh, const unsigned & startCorner, const float & coeff);

private:
	VBEDCmpCombHandle    m_objCmpCombHandle;

private:
	VNWZKALG::VSEdgeBreakerDecInfo  res;
	VNMesh::VSCompressedMesh     checkedRes;
};
