#include "stdafx.h"
#include "VBNonManifoldCheck.h"

void VBNonManifoldCheck::Check(const VSSimpleMeshF & smesh)
{
	checkRes.flag = false;
	if (mesh.read(smesh) == 0)
	{
		m_nonManifoldEdge = mesh.nonmanifoldEdge;

		FindNonManifoldSurf(smesh, m_nonManifoldEdge);

		checkRes.flag = true;
		checkRes.nonmanifoldEV.x = m_nonManifoldEdge.first;
		checkRes.nonmanifoldEV.y = m_nonManifoldEdge.second;
		checkRes.neTriangle = VD_V2CB(m_vEdgeFace);
	}
	else
	{
		m_vNonManifoldVert = HEMeshConstruct::mcheckNonManifoldVertices(mesh);

		if (m_vNonManifoldVert.size() > 0)
		{
			VertexFaceClassify(smesh);

			checkRes.flag = true;
			checkRes.nonmanifoldV = VD_V2CB(m_vNonManifoldVert);
			checkRes.nvTriangle1 = VD_V2CB(m_vVertFace1);
			checkRes.nvTriangle2 = VD_V2CB(m_vVertFace2);
		}
	}
}

void VBNonManifoldCheck::VertexFaceClassify(const VSSimpleMeshF & smesh)
{
	SearchFace(smesh);

	unsigned vNum = m_vNonManifoldVert.size();
	m_vNonManifoldFace1.resize(vNum);
	m_vNonManifoldFace2.resize(vNum);
	m_vVertFace1.resize(vNum);
	m_vVertFace2.resize(vNum);

	std::vector<bool>  triFlag;
	triFlag.resize(smesh.nTriangleCount);
	for (unsigned i = 0; i < vNum; i++)
	{
		HalfEdgeIter h = mesh.vertices[m_vNonManifoldVert[i]].he;
		do
		{
			if (h->face->index != 0)
				m_vNonManifoldFace1[i].push_back(h->face->index);
			triFlag[h->face->index] = true;
			h = h->flip->next;
		} while (h != mesh.vertices[m_vNonManifoldVert[i]].he);
		
		for (unsigned j = 0; j < m_VertAllFace[m_vNonManifoldVert[i]].size(); j++)
		{
			if (!triFlag[m_VertAllFace[m_vNonManifoldVert[i]][j]])
				m_vNonManifoldFace2[i].push_back(m_VertAllFace[m_vNonManifoldVert[i]][j]);
		}

		m_vVertFace1[i] = VD_V2CB(m_vNonManifoldFace1[i]);
		m_vVertFace2[i] = VD_V2CB(m_vNonManifoldFace2[i]);
	}	
}

void VBNonManifoldCheck::SearchFace(const VSSimpleMeshF & smesh)
{
	m_vVFlag.resize(smesh.nVertCount);

	std::vector<unsigned>    temp;
	for (unsigned i = 0; i < m_vNonManifoldVert.size(); i++)
	{
		m_vVFlag[m_vNonManifoldVert[i]] = true;
		m_VertAllFace[m_vNonManifoldVert[i]] = temp;
	}

	for (unsigned i = 0; i < smesh.nTriangleCount; i++)
	{
		if (m_vVFlag[smesh.pTriangles[i].x])
			m_VertAllFace[smesh.pTriangles[i].x].push_back(i);

		if (m_vVFlag[smesh.pTriangles[i].y])
			m_VertAllFace[smesh.pTriangles[i].y].push_back(i);

		if (m_vVFlag[smesh.pTriangles[i].z])
			m_VertAllFace[smesh.pTriangles[i].z].push_back(i);
	}
}

void VBNonManifoldCheck::FindNonManifoldSurf(const VSSimpleMeshF & smesh, const std::pair<unsigned, unsigned>& ve)
{
	std::vector<bool>   vFlag;
	vFlag.resize(smesh.nVertCount);
	vFlag[ve.first] = true;
	vFlag[ve.second] = true;

	for (unsigned i = 0; i < smesh.nTriangleCount; i++)
	{
		unsigned count = 0;
		for (unsigned j = 0; j < 3; j++)
		{
			if (vFlag[smesh.pTriangles[i][j]])
				count ++;
		}
		if (count == 2)
			m_vEdgeFace.push_back(i);
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBNonManifoldCheck, VNALGMESH::VRNonmanifoldCheck);
VD_EXPORT_SYSTEM_SIMPLE(VBNonManifoldCheck2, VNALGMESH::VRNonmanifoldCheck2);