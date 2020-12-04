#pragma once
#include <vmath/vfmesh.h>

//网格重心计算

class VBMeshBarycenter
{
public:
	VBMeshBarycenter() {}
	~VBMeshBarycenter() {}

	void Build(VFVECTOR3 & center, const VSSimpleMeshF & mesh)
	{
		VFVECTOR3  sum = VFVECTOR3::ZERO;
		for (unsigned i = 0; i < mesh.nVertCount; i++)
			sum += mesh.pVertices[i];

		center = sum / static_cast<float>(mesh.nVertCount);
	}

};
