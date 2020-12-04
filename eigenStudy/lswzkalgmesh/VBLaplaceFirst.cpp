#include"stdafx.h"
#include"VBLaplaceFirst.h"

void VBLaplaceFirst::TriAngleSqr(const VSGraphMesh & gm)
{
	unsigned sNum = gm.GetSurfCount();
	m_vTriInfo.clear();
	m_vTriInfo.resize(sNum);


	for (unsigned i = 0; i < sNum; i++)
	{
		const VNVECTOR3UI & vos = gm.pSurfaces[i];

		VFVECTOR3 v01 = gm.pVertices[vos[1]] - gm.pVertices[vos[0]];
		VFVECTOR3 v02 = gm.pVertices[vos[2]] - gm.pVertices[vos[0]];
		VFVECTOR3 v12 = gm.pVertices[vos[2]] - gm.pVertices[vos[1]];

		m_vTriInfo[i].triSqure = 0.5f * v01.Cross(v02).Magnitude();
		m_vTriInfo[i].triCot[0] = v01.Dot(v02) / (v01.Cross(v02)).Magnitude();
		m_vTriInfo[i].triCot[1] = v12.Dot(-v01) / (v12.Cross(-v01)).Magnitude();
		m_vTriInfo[i].triCot[2] = (-v12).Dot(-v02) / ((-v12).Cross(-v02)).Magnitude();
	}
}

void VBLaplaceFirst::VertNbrSurfArea(const VSGraphMesh & gm)
{
	unsigned vNum = gm.GetVertexCount();
	m_vVertArea.clear();
	m_vVertArea.resize(vNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		double areaSum = 0.f;
		unsigned sCount = gm.GetSurfCountOfVetex(i);
		for (unsigned j = 0; j < sCount; j++)
		{
			const unsigned & sidx = gm.GetSurfIndxOfVertex(i, j);
			areaSum += m_vTriInfo[sidx].triSqure;
		}
		m_vVertArea[i] = areaSum;
	}
}

void VBLaplaceFirst::NoDiagElem(const VSGraphMesh & gm)
{
	unsigned vLeft = 0, vRight = 0;

	unsigned sNum = gm.GetSurfCount();
	unsigned vNum = gm.GetVertexCount();
	unsigned eNum = gm.GetEdgeCount();
	m_vFirstLapTripl.clear();
	m_vFirstLapTripl.resize((2 * eNum + vNum));    //前2*eNum个元素记录非对角元素信息，后vNum记录对角元素信息

	for (unsigned i = 0; i < eNum; i++)
	{
		const VNVECTOR2UI & voe = gm.GetVertIndxOfEdge(i);
		const VNVECTOR2UI & soe = gm.GetSurfIndxOfEdge(i);

		VNVECTOR3UI sidx0 = gm.pSurfaces[soe[0]];
		VNVECTOR3UI sidx1 = gm.pSurfaces[soe[1]];

		for (unsigned j = 0; j < 3; j++)
		{
			if (gm.pSurfaces[soe[0]][j] == voe[0])
				vLeft = j;
			if (gm.pSurfaces[soe[1]][j] == voe[0])
				vRight = j;
		}

		//对角矩阵,每条边记录两次
		if (soe[1] < sNum)
		{
			double cotLeft = m_vTriInfo[soe[0]].triCot[(vLeft + 2) % 3];
			double cotRight = m_vTriInfo[soe[1]].triCot[(vRight + 1) % 3];
			m_vFirstLapTripl[2 * i].row = voe[0];
			m_vFirstLapTripl[2 * i].col = voe[1];
			m_vFirstLapTripl[2 * i].val = 1.5f * (cotLeft + cotRight) / m_vVertArea[voe[0]];
			m_vFirstLapTripl[2 * i + 1].row = voe[1];
			m_vFirstLapTripl[2 * i + 1].col = voe[0];
			m_vFirstLapTripl[2 * i + 1].val = 1.5f * (cotLeft + cotRight) / m_vVertArea[voe[1]];
		}
		else
		{
			double cotRight = m_vTriInfo[soe[0]].triCot[(vLeft + 2) % 3];
			m_vFirstLapTripl[2 * i].row = voe[0];
			m_vFirstLapTripl[2 * i].col = voe[1];
			m_vFirstLapTripl[2 * i].val = 1.5f * cotRight / m_vVertArea[voe[0]];
			m_vFirstLapTripl[2 * i + 1].row = voe[1];
			m_vFirstLapTripl[2 * i + 1].col = voe[0];
			m_vFirstLapTripl[2 * i + 1].val = 1.5f * cotRight / m_vVertArea[voe[1]];
		}

	}
}

void VBLaplaceFirst::DiagElem(const VSGraphMesh & gm)
{
	unsigned vNum = gm.GetVertexCount();
	unsigned eNum = gm.GetEdgeCount();
	//m_vDiagTemp.resize(vNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		unsigned sCount = gm.GetSurfCountOfVetex(i);
		for (unsigned j = 0; j < sCount; j++)
		{
			unsigned pos = VD_INVALID_INDEX;
			unsigned sidx = gm.GetSurfIndxOfVertex(i, j);

			//VNVECTOR3UI vos = gm.pSurfaces[sidx];

			for (unsigned k = 0; k < 3; k++)
			{
				if (gm.pSurfaces[sidx][k] == i)
				{
					pos = k;
					break;
				}
			}
			double cotSum = m_vTriInfo[sidx].triCot[(pos + 1) % 3] +
				m_vTriInfo[sidx].triCot[(pos + 2) % 3];
			m_vFirstLapTripl[2 * eNum + i].val += cotSum;
		}
		m_vFirstLapTripl[2 * eNum + i].val *= (-1.5f / m_vVertArea[i]);
		m_vFirstLapTripl[2 * eNum + i].row = i;
		m_vFirstLapTripl[2 * eNum + i].col = i;
		//m_vDiagTemp[i] = m_vFirstLapTripl[2 * eNum + i];    //取出对角线元素
	}
}

void VBLaplaceFirst::nonNormalization(const VSGraphMesh & gm)
{
	TriAngleSqr(gm);
	VertNbrSurfArea(gm);
	NoDiagElem(gm);
	DiagElem(gm);

	/*unsigned vNum = gm.GetVertexCount();
	unsigned eNum = gm.GetEdgeCount();
	for (unsigned i = 0; i < 2 * eNum; i++)
	{
		unsigned vRow = m_vFirstLapTripl[i].row;
		m_vFirstLapTripl[i].val = m_vFirstLapTripl[i].val / m_vDiagTemp[vRow].val;
	}
	for (unsigned i = 0; i < vNum; i++)
	{
		m_vFirstLapTripl[2 * eNum + i].val = 1.f;
	}*/
}

//VD_EXPORT_SYSTEM_SIMPLE(VBLaplaceFirst, VNALGMESH::VRLaplaceFirst);