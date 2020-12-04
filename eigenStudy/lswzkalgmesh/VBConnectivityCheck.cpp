#include "stdafx.h"
#include "VBConnectivityCheck.h"

void VBConnectivityCheck::RegionCheck(const VSGraphMesh & gm)
{
	std::queue<unsigned>    m_queue;
	unsigned sCount = 0, sNum = gm.GetSurfCount();
	m_vTriFlag.resize(sNum);
	m_vPushFlag.resize(sNum);

	while (sCount < sNum)
	{
		unsigned seed = VD_INVALID_INDEX;
		for (unsigned i = 0; i < sNum; i++)
		{
			if (!m_vTriFlag[i])
			{
				VASSERT(m_queue.empty());
				m_queue.push(i);
				m_vPushFlag[i] = true;
				break;
			}
		}

		m_vTemp.clear();
		while (!m_queue.empty())
		{
			seed = m_queue.front();
			m_vTriFlag[seed] = true;
			VNVECTOR3UI  eos = gm.GetEdgeIndxOfSurf(seed);
			for (unsigned i = 0; i < 3; i++)
			{
				VNVECTOR2UI  soe = gm.GetSurfIndxOfEdge(eos[i]);
				unsigned sid = (soe.x == seed ? soe.y : soe.x);
				if (sid != VD_INVALID_INDEX && !m_vPushFlag[sid])
				{
					m_queue.push(sid);
					m_vPushFlag[sid] = true;
				}
			}
			sCount++;
			m_vTemp.push_back(seed);
			m_queue.pop();
		}
		m_vRegion.push_back(m_vTemp);
	}
	
	m_vRegionTri.resize(m_vRegion.size());
	for (unsigned i = 0; i < m_vRegion.size(); i++)
		m_vRegionTri[i].triID = VD_V2CB(m_vRegion[i]);
}

VD_EXPORT_SYSTEM_SIMPLE(VBConnectivityCheck, VNALGMESH::VRConnectivityCheck);