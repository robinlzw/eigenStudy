#include "stdafx.h"
#include "VBFindHoleEdge.h"

void VBFindHoleEdge::FindAll(const VSGraphMesh & gm)
{
	EdgeVertFlag(gm);
	while (BndryEdgeNum > 0)
	{
		unsigned eid = FindBndryEdge(gm);
		if (eid != VD_INVALID_INDEX)
		{
			SingleBndryProc(gm, eid);
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

void VBFindHoleEdge::SingleBndryProc(const VSGraphMesh & gm, unsigned eid)
{
	unsigned curID = VD_INVALID_INDEX, orgID = VD_INVALID_INDEX;
	VNVECTOR2UI  voe = gm.GetVertIndxOfEdge(eid);
	orgID = voe[1];
	m_vTemp.resize(0);
	m_vTemp.push_back(orgID);
	m_vEdgeFlag[eid] = false;
	BndryEdgeNum --;
	unsigned temp = voe[0];
	m_vTemp.push_back(temp);
	while (curID != orgID)
	{
		unsigned eNum = gm.GetEdgeCountOfVetex(temp);
		for (unsigned i = 0; i < eNum; i++)
		{
			unsigned eidT = gm.GetEdgeIndxOfVertex(temp, i);
			VNVECTOR2UI  soe = gm.GetSurfIndxOfEdge(eidT);
			voe = gm.GetVertIndxOfEdge(eidT);
			if (soe[1] == VD_INVALID_INDEX && voe[1] == temp && m_vEdgeFlag[eidT])
			{
				m_vEdgeFlag[eidT] = false;
				BndryEdgeNum --;
				if (voe[0] != orgID)
				{
					temp = voe[0];
					m_vTemp.push_back(temp);
					if (m_vDegIDFlag[temp])
						m_vVertDegree[temp] --;
					break;
				}
				else
				{
					curID = voe[0];
					m_vVertDegree[orgID] --;
					break;
				}
			}
		}
	}
}

void VBFindHoleEdge::EdgeVertFlag(const VSGraphMesh & gm)
{
	DegFlag = false;
	BndryEdgeNum = 0;
	unsigned vNum = gm.GetVertexCount(), eNum = gm.GetEdgeCount();
	m_vVertDegree.resize(vNum);
	m_vDegIDFlag.resize(vNum);
	m_vEdgeFlag.resize(eNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		unsigned eCount = gm.GetEdgeCountOfVetex(i);
		unsigned sCount = gm.GetSurfCountOfVetex(i);
		m_vVertDegree[i] = eCount - sCount;
		if (m_vVertDegree[i] > 1)
		{
			m_vDegID.push_back(i);
			m_vDegIDFlag[i] = true;
		}
	}
	for (unsigned i = 0; i < eNum; i++)
	{
		VNVECTOR2UI soe = gm.GetSurfIndxOfEdge(i);
		if (soe[1] == VD_INVALID_INDEX)
		{
			m_vEdgeFlag[i] = true;
			BndryEdgeNum ++;
		}
	}
}

unsigned VBFindHoleEdge::FindBndryEdge(const VSGraphMesh & gm)
{
	VNVECTOR2I   temp(2, -1);
	if (!DegFlag)
	{
		for (unsigned i = 0; i < m_vDegID.size(); i++)
		{
			if (temp.x <= (int)m_vVertDegree[m_vDegID[i]])
			{
				temp.x = m_vVertDegree[m_vDegID[i]];
				temp.y = m_vDegID[i];
			}
			if (m_vVertDegree[m_vDegID[i]] == 1)
				m_vDegIDFlag[m_vDegID[i]] = false;
		}
		if (temp.y == -1)
			DegFlag = true;
	}
	if (temp.y != -1)
	{
		unsigned eNum = gm.GetEdgeCountOfVetex(temp.y);
		for (unsigned i = 0; i < eNum; i++)
		{
			unsigned eid = gm.GetEdgeIndxOfVertex(temp.y, i);
			VNVECTOR2UI  soe = gm.GetSurfIndxOfEdge(eid);
			VNVECTOR2UI  voe = gm.GetVertIndxOfEdge(eid);
			if (soe[1] == VD_INVALID_INDEX && voe[1] == temp.y && m_vEdgeFlag[eid] == true)
				return eid;
		}
	}
	else
	{
		for (unsigned i = 0; i < m_vEdgeFlag.size(); i++)
		{
			if (m_vEdgeFlag[i])
				return i;
		}
	}
	return VD_INVALID_INDEX;
}

VD_EXPORT_SYSTEM_SIMPLE(VBFindHoleEdge, VNALGMESH::VRFindHoleEdge);