#include"stdafx.h"

#include"VJEdgeToSimpleMesh.h"

void VJEdgeToSimpleMesh::AddOrgTri(const VSPerfTopoGraph & mesh, const VSConstBuffer<bool> & eflag)   
{
	unsigned sCount = mesh.GetSurfCount();

	for (unsigned i = 0; i < sCount; i++)
	{
		VNVECTOR3UI eidx = mesh.GetEdgeIndxOfSurf(i);
		if (eflag.pData[eidx[0]] == 0 && eflag.pData[eidx[1]] == 0 && eflag.pData[eidx[2]] == 0)
		{
			m_vSurfTriangle.push_back(mesh.pSurfaces[i]);
		}
	}
}



void VJEdgeToSimpleMesh::SplitTriToSimpMesh(const VSPerfTopoGraph & mesh, const VSConstBuffer<bool> & eflag)   
{
	AddOrgTri(mesh, eflag);

	unsigned sCount = mesh.GetSurfCount();
	unsigned vCount = mesh.GetVertexCount();

	VNVECTOR3UI temp;
	for (unsigned i = 0; i < sCount; i++)
	{
		int splitEdgeNum = -1, e0 = -1;
		VNVECTOR3UI eidxs = mesh.GetEdgeIndxOfSurf(i);
		for (unsigned j = 0; j < 3; j++)       //确定三角形中要分裂的边的数量
		{
			splitEdgeNum = (eidxs[j] == 1) ? (splitEdgeNum + 1) : splitEdgeNum;
		}

		//三种情况
		if (splitEdgeNum == 1)
		{
			vCount += m_vNewVertOnEdge.size();

			for (unsigned j = 0; j < 3; j++)
			{
				e0 = (eidxs[j] == 1) ? j : e0;     //e0为要分裂的边
			}
			m_vNewVertOnEdge.push_back(eidxs[e0]);

			temp[0] = mesh.pSurfaces[i][e0];
			temp[1] = mesh.pSurfaces[i][(e0 + 1) % 3];
			temp[2] = vCount;
			m_vSurfTriangle.push_back(temp);

			temp[0] = mesh.pSurfaces[i][e0];
			temp[1] = vCount;
			temp[2] = mesh.pSurfaces[i][(e0 + 2) % 3];
			m_vSurfTriangle.push_back(temp);
		}
		else if (splitEdgeNum == 2)
		{
			vCount += m_vNewVertOnEdge.size();

			for (unsigned j = 0; j < 3; j++)
			{
				e0 = (eidxs[j] == 0) ? j : e0;      //e0为不分裂的边
			}
			m_vNewVertOnEdge.push_back(eidxs[(e0 + 1) % 3]);
			m_vNewVertOnEdge.push_back(eidxs[(e0 + 2) % 3]);

			temp[0] = mesh.pSurfaces[i][e0];
			temp[1] = vCount + 1;
			temp[2] = vCount;
			m_vSurfTriangle.push_back(temp);

			temp[0] = mesh.pSurfaces[i][(e0 + 1) % 3];
			temp[1] = vCount;
			temp[2] = vCount + 1;
			m_vSurfTriangle.push_back(temp);

			temp[0] = mesh.pSurfaces[i][(e0 + 1) % 3];
			temp[1] = mesh.pSurfaces[i][(e0 + 2) % 3];
			temp[2] = vCount;
			m_vSurfTriangle.push_back(temp);
		}
		else if (splitEdgeNum == 3)
		{
			e0 = 0;
			vCount += m_vNewVertOnEdge.size();

			m_vNewVertOnEdge.push_back(eidxs[e0]);
			m_vNewVertOnEdge.push_back(eidxs[e0 + 1]);
			m_vNewVertOnEdge.push_back(eidxs[e0 + 2]);

			temp[0] = mesh.pSurfaces[i][e0];
			temp[1] = vCount + 2;
			temp[2] = vCount + 1;
			m_vSurfTriangle.push_back(temp);

			temp[0] = mesh.pSurfaces[i][(e0 + 1) % 3];
			temp[1] = vCount;
			temp[2] = vCount + 2;
			m_vSurfTriangle.push_back(temp);

			temp[0] = mesh.pSurfaces[i][(e0 + 2) % 3];
			temp[1] = vCount + 1;
			temp[2] = vCount;
			m_vSurfTriangle.push_back(temp);

			temp[0] = vCount;
			temp[1] = vCount + 1;
			temp[2] = vCount + 2;
			m_vSurfTriangle.push_back(temp);
		}
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VJEdgeToSimpleMesh, VNALGMESH::VREdgeToSimpleMesh);