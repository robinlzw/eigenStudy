#include"stdafx.h"
#include"VBDijkstra.h"

float VBDijkstra::maxDis = 100000.f;

void VBDijkstra::FindNbrVert(const VSGraphMesh & gm, const unsigned & vidx)
{
	unsigned eNum = gm.GetEdgeCountOfVetex(vidx);
	m_vNbrVert.resize(eNum);
	for (unsigned i = 0; i < eNum; i++)
	{
		auto eov      = gm.GetEdgeOfVertex(vidx, i).Opposite();
		unsigned temp = gm.GetVertIndxOfEdge(eov.edgeIndx)[eov.vertInEdge];
		m_vNbrVert[i] = temp;		
	}
}

void VBDijkstra::FindPath(const VSGraphMesh & gm, const unsigned & begVertIdx, const unsigned & endVertIdx)
{
	unsigned vNum = gm.GetVertexCount();
	m_vVertWt.resize(vNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		m_vVertWt[i] = maxDis;
	}
	m_vVertWt[begVertIdx] = 0.f;
	
	m_vSinglePath.clear();

	DijkstraPath(gm, begVertIdx, endVertIdx);

	unsigned pathCount = m_vPathInfo.size();
	VSPathInfo temp = m_vPathInfo[pathCount - 1];
	m_vSinglePath.push_back(temp);

	while (temp.pathVert != begVertIdx)
	{
		for (unsigned i = 0; i < pathCount; i++)
		{
			if (m_vPathInfo[i].pathVert == temp.pathPreVert)
			{
				temp = m_vPathInfo[i];
				m_vSinglePath.push_back(temp);
				break;
			}
		}
	}

	for (unsigned i = 0; i < pathCount; i++)
	{
		if (m_vPathInfo[i].pathVert == temp.pathPreVert)
		{
			temp = m_vPathInfo[i];
			m_vSinglePath.push_back(temp);
			if (temp.pathVert == begVertIdx)
				break;
		}
	}
}

void VBDijkstra::DijkstraPath(const VSGraphMesh & gm, const unsigned & begVertIdx, const unsigned & endVertIdx)
{
	unsigned currVert = VD_INVALID_INDEX, nbrVert = VD_INVALID_INDEX;
	DijkElem temp;
	VSPathInfo tempPathInfo;

	unsigned vNum = gm.GetVertexCount();

	m_vPathInfo.clear();
	m_heap.clear();
	m_vVertFlag.clear();

	m_heap.resize(vNum);
	m_vVertFlag.resize(vNum);	

	temp.vertex = begVertIdx;
	temp.distance = 0.f;
	m_heap[0] = temp;
	heap<DijkElem, DDComp> H(&m_heap[0], 1, gm.GetEdgeCount());    // Create heap

	for (unsigned i = 0; i < vNum; i++)                   // Now, get distances
	{
		do
		{
			if (H.size() == 0) return;         // Nothing to remove
			temp = H.removefirst();
			currVert = temp.vertex;
		} while (m_vVertFlag[currVert] == 1);

		tempPathInfo.pathLen = temp.distance;
		tempPathInfo.pathVert = temp.vertex;
		tempPathInfo.pathPreVert = temp.preVert;
		m_vPathInfo.push_back(tempPathInfo);
		if (temp.vertex == endVertIdx)
			return;

		m_vVertFlag[currVert] = 1;
		if (m_vVertWt[currVert] == maxDis) return;                               // Unreachable vertices
		FindNbrVert(gm, currVert);
		for (unsigned j = 0; j < m_vNbrVert.size(); j ++)
		{
			const unsigned & vnidx = m_vNbrVert[j];
			float  elen = (gm.pVertices[currVert] - gm.pVertices[vnidx]).Magnitude();
			if (m_vVertWt[vnidx] >(m_vVertWt[currVert] + elen))
			{                                                  // Update Distance
				m_vVertWt[vnidx] = m_vVertWt[currVert] + elen;
				temp.distance = m_vVertWt[vnidx];
				temp.vertex   = vnidx;
				temp.preVert  = currVert;
				H.insert(temp);                               // Insert new distance in heap            
			}
		}
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBDijkstra, VNALGMESH::VRDijkstra);