#pragma once
#include <vmath/vfmesh.h>
#include "VBTriangleArea.h"
#include <sys\alg\isysmesh.h>

//ÍË»¯Èý½ÇÆ¬¼ì²é

class VBDegenerateCheck
{
public:
	VBDegenerateCheck():S(0.f) {}
	~VBDegenerateCheck() {}

	void Build(VSConstBuffer<unsigned> & ds, const VNWZKALG::VSDegenerate & deg)
	{
		for (unsigned i = 0; i < deg.smesh.nTriangleCount; i++)
		{
			VNVECTOR3UI  vos = deg.smesh.pTriangles[i];
			area.Build(S, deg.smesh.pVertices[vos[0]], deg.smesh.pVertices[vos[1]], deg.smesh.pVertices[vos[2]]);
			if (S <= deg.threshold)
				m_vDegenerateTri.push_back(i);
		}

		ds = VD_V2CB(m_vDegenerateTri);
	}

private:
	VBTriangleArea    area;
	std::vector<unsigned>      m_vDegenerateTri;
	float S;
};
