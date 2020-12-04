#pragma once
#include "VBTriangleArea.h"

//计算网格表面积

class VBMeshSurfaceArea
{
public:
	VBMeshSurfaceArea() {}
	~VBMeshSurfaceArea() {}

	void Build(float & area, const VSSimpleMeshF & mesh)
	{
		area = 0.f;

		for (unsigned i = 0; i < mesh.nTriangleCount; i++)
		{
			float temp = 0.f;
			VNVECTOR3UI   tri = mesh.pTriangles[i];
			m_objArea.Build(temp, mesh.pVertices[tri.x], mesh.pVertices[tri.y], mesh.pVertices[tri.z]);
			area += temp;
		}
	}

private:
	VBTriangleArea  m_objArea;
};
