#pragma once
#include "VBMeshBarycenter.h"

//计算两个网格之间的间距
class VBMeshGap
{
public:
	VBMeshGap() {}
	~VBMeshGap() {}

	void Build(float & dist, const VSSimpleMeshF & mesh1, const VSSimpleMeshF & mesh2)
	{
		dist = Gap(mesh1, mesh2);
	}

private:
	float Gap(const VSSimpleMeshF & mesh1, const VSSimpleMeshF & mesh2);
	void  ProjectDist(std::vector<float> & vec, const VFVECTOR3 & orgin, const VFVECTOR3 & direct, const VSSimpleMeshF & mesh);

private:
	VBMeshBarycenter   m_oBary;

private:
	std::vector<float>    m_vDist1;
	std::vector<float>    m_vDist2;
};
