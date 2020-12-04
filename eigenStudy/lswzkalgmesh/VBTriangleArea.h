#pragma once
#include <vmath/vfmesh.h>

//计算三角片的面积

class VBTriangleArea
{
public:
	VBTriangleArea() {}
	~VBTriangleArea() {}

	void Build(float & area, const VFVECTOR3 & v0, const VFVECTOR3 & v1, const VFVECTOR3 & v2)
	{
		float len0 = (v0 - v1).Magnitude();
		float len1 = (v1 - v2).Magnitude();
		float len2 = (v2 - v0).Magnitude();
		float C = (len0 + len1 + len2) / 2.f;
		float SS = C * (C - len0) * (C - len1) * (C - len2);

		area = sqrtf(SS);
	}

private:

};
