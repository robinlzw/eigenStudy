#pragma once
#include <queue>
#include <vmath\vfmesh.h>
#include <sys/alg/isysmesh.h>

//网格连通性检查

class VBConnectivityCheck
{
public:
	VBConnectivityCheck() {}
	~VBConnectivityCheck() {}

	void Build(VNWZKALG::VSConnectivity & sIndex, const VSGraphMesh & gm)
	{
		RegionCheck(gm);

		sIndex.conn = VD_V2CB(m_vRegionTri);
	}

private:
	void RegionCheck(const VSGraphMesh & gm);

private:
	std::vector<VNWZKALG::VSSingleConnectivity>   m_vRegionTri;
	std::vector<std::vector<unsigned>>            m_vRegion;
	std::vector<unsigned>                         m_vTemp;
	std::vector<bool>                             m_vTriFlag;
	std::vector<bool>                             m_vPushFlag;
};