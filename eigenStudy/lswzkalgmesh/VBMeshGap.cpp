#include "stdafx.h"
#include "VBMeshGap.h"

float VBMeshGap::Gap(const VSSimpleMeshF & mesh1, const VSSimpleMeshF & mesh2)
{
	VFVECTOR3 bary1, bary2, temp;
	float gap = 0.f;
	m_oBary.Build(temp, mesh1);
	bary1 = temp;
	m_oBary.Build(temp, mesh2);
	bary2 = temp;

	VFVECTOR3   v12 = bary2 - bary1;
	v12.Normalize();

	ProjectDist(m_vDist1, bary1, v12, mesh1);
	ProjectDist(m_vDist2, bary1, v12, mesh2);

	//float min2 = *std::min_element(m_vDist2.begin(), m_vDist2.end());
	//float max1 = *std::max_element(m_vDist1.begin(), m_vDist1.end());
	float dist = *std::min_element(m_vDist2.begin(), m_vDist2.end()) - *std::max_element(m_vDist1.begin(), m_vDist1.end());
	return dist;
}

void VBMeshGap::ProjectDist(std::vector<float>& vec, const VFVECTOR3 & orgin, const VFVECTOR3 & direct, const VSSimpleMeshF & mesh)
{
	vec.resize(mesh.nVertCount);

	VFVECTOR3 temp = VFVECTOR3::ZERO;
	for (unsigned i = 0; i < mesh.nVertCount; i++)
	{
		temp = mesh.pVertices[i] - orgin;
		vec[i] = temp.Dot(direct);
	}
}
