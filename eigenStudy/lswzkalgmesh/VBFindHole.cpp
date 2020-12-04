#include "stdafx.h"
#include "VBFindHole.h"

void VBFindHole2::FindAll(const VSPerfTopoGraph & gm)
{
	FlagBndryVert(gm);
	while (count > 0)
	{
		unsigned vid = FindBndryVert();
		if (vid != VD_INVALID_INDEX)
		{
			SingleBndryProc(gm, vid);
			unsigned holeNum = m_vHoleBndry.size();
			m_vHoleBndry.resize(holeNum + 1);
			m_vHoleBndry[holeNum].resize(m_vTemp.size());
			memcpy(&m_vHoleBndry[holeNum].front(), &m_vTemp.front(), sizeof(unsigned) * m_vTemp.size());
		}		
	}

	unsigned num = m_vHoleBndry.size();
	m_vBndryTotal.resize(num);
	for (unsigned i = 0; i < num; i++)
		m_vBndryTotal[i].orderedVert = VD_V2CB(m_vHoleBndry[i]);
}

void VBFindHole2::FlagBndryVert(const VSPerfTopoGraph & gm)
{
	unsigned vNum = gm.GetVertexCount();
	m_vVertFlag.resize(vNum);
	m_vHoleBndry.clear();
	m_vVertBndry.clear();
	m_vVertBndry.reserve(vNum);
	unsigned eNum, sNum;
	for (unsigned i = 0; i < vNum; i++)
	{
		eNum = gm.GetEdgeCountOfVetex(i);
		sNum = gm.GetSurfCountOfVetex(i);
		if (eNum != sNum)
		{
			count ++;
			m_vVertFlag[i] = true;
			m_vVertBndry.push_back(i);
		}
	}
	m_vVertBndry.resize(m_vVertBndry.size());
}

void VBFindHole2::SingleBndryProc(const VSPerfTopoGraph & gm, unsigned vid)
{
	unsigned curID = VD_INVALID_INDEX, orgID = vid;
	m_vTemp.clear();
	m_vTemp.reserve(gm.GetVertexCount());
	m_vVertFlag[ orgID ] = false;
	m_vTemp.push_back(orgID);
	count --;
	while (curID != orgID)
	{
		unsigned eNum = gm.GetEdgeCountOfVetex(vid);
		for (unsigned i = 0; i < eNum; i++)
		{
			unsigned eid = gm.GetEdgeIndxOfVertex(vid, i);
			VNVECTOR2UI  soe = gm.GetSurfIndxOfEdge(eid);
			VNVECTOR2UI  voe = gm.GetVertIndxOfEdge(eid);
			if (soe[1] == VD_INVALID_INDEX && voe[1] == vid)
			{				
				if (voe[0] != orgID)
				{
					m_vVertFlag[ voe[0] ] = false;
					vid = voe[0];
					m_vTemp.push_back(voe[0]);
					count--;
					break;
				}
				else
				{
					curID = voe[0];
					break;
				}
			}			
		}		
	}
}

unsigned VBFindHole2::FindBndryVert()
{
	for (unsigned i = 0; i < m_vVertBndry.size(); i++)
	{
		if (m_vVertFlag[m_vVertBndry[i]])
			return m_vVertBndry[i];
	}

	return VD_INVALID_INDEX;
}

VD_EXPORT_SYSTEM_SIMPLE(VBFindHole  , VNALGMESH::VRFindHole  );
VD_EXPORT_SYSTEM_SIMPLE(VBFindHole2 , VNALGMESH::VRFindHole2 );