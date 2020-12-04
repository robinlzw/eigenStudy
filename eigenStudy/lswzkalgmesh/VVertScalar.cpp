#include"stdafx.h"

#include"VVertScalar.h"

float VVertScalar::threshold = 0.5f;

void VVertScalar::MatRight(const VSGraphMesh & gm, const VSConstBuffer< std::pair<unsigned, float> > & VertScalar)
{
	unsigned vNum = gm.GetVertexCount();
	m_vR.resize(vNum);

	/*std::ifstream  f("D:\\vt.txt");	
	for (unsigned i = 0; i < m_vR.size(); i++)
	{
		f >> m_vR[i];
	}
	f.close();
	for (unsigned i = 0; i < m_vR.size(); i++)
	{		
		m_vR[i] = -threshold * m_vR[i];
	}*/

	for (unsigned i = 0; i < VertScalar.len; i++)
	{
		const unsigned vidx = VertScalar.pData[i].first;
		m_vR[vidx] = -threshold * VertScalar.pData[i].second;
	}
}

void VVertScalar::TriTanSqr(const VSGraphMesh & gm)
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

void VVertScalar::VertNbrSurfArea(const VSGraphMesh & gm)
{
	unsigned vNum = gm.GetVertexCount();
	m_vVertArea.clear();
	m_vVertArea.resize(vNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		float areaSum = 0.f;
		unsigned sCount = gm.GetSurfCountOfVetex(i);
		for (unsigned j = 0; j < sCount; j++)
		{
			const unsigned & sidx = gm.GetSurfIndxOfVertex(i, j);
			areaSum += m_vTriInfo[sidx].triSqure;
		}
		m_vVertArea[i] = areaSum;
	}
}

void VVertScalar::LapNoDiagElem(const VSGraphMesh & gm)
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
			float cotLeft = m_vTriInfo[soe[0]].triCot[(vLeft + 2) % 3];
			float cotRight = m_vTriInfo[soe[1]].triCot[(vRight + 1) % 3];
			m_vFirstLapTripl[2 * i].row = voe[0];
			m_vFirstLapTripl[2 * i].col = voe[1];
			m_vFirstLapTripl[2 * i].val = 1.5f * (cotLeft + cotRight) / m_vVertArea[voe[0]];
			m_vFirstLapTripl[2 * i + 1].row = voe[1];
			m_vFirstLapTripl[2 * i + 1].col = voe[0];
			m_vFirstLapTripl[2 * i + 1].val = 1.5f * (cotLeft + cotRight) / m_vVertArea[voe[1]];
		}
		else
		{
			float cotRight = m_vTriInfo[soe[0]].triCot[(vLeft + 2) % 3];
			m_vFirstLapTripl[2 * i].row = voe[0];
			m_vFirstLapTripl[2 * i].col = voe[1];
			m_vFirstLapTripl[2 * i].val = 1.5f * cotRight / m_vVertArea[voe[0]];
			m_vFirstLapTripl[2 * i + 1].row = voe[1];
			m_vFirstLapTripl[2 * i + 1].col = voe[0];
			m_vFirstLapTripl[2 * i + 1].val = 1.5f * cotRight / m_vVertArea[voe[1]];
		}
	}
}

void VVertScalar::LapDiagElem(const VSGraphMesh & gm)
{	
	unsigned vNum = gm.GetVertexCount();
	unsigned eNum = gm.GetEdgeCount();
	m_vDiagTemp.resize(vNum);
	for (unsigned i = 0; i < vNum; i++)
	{
		unsigned sCount = gm.GetSurfCountOfVetex(i);
		for (unsigned j = 0; j < sCount; j++)
		{
			unsigned pos = VD_INVALID_INDEX;
			unsigned sidx = gm.GetSurfIndxOfVertex(i, j);

			for (unsigned k = 0; k < 3; k++)
			{
				if (gm.pSurfaces[sidx][k] == i)
				{
					pos = k;
					break;
				}
			}
			float cotSum = m_vTriInfo[sidx].triCot[(pos + 1) % 3] +
				m_vTriInfo[sidx].triCot[(pos + 2) % 3];
			m_vFirstLapTripl[2 * eNum + i].val += cotSum;
		}
		m_vFirstLapTripl[2 * eNum + i].val *= (-1.5f / m_vVertArea[i]);
		m_vFirstLapTripl[2 * eNum + i].row = i;
		m_vFirstLapTripl[2 * eNum + i].col = i;
		m_vDiagTemp[i] = m_vFirstLapTripl[2 * eNum + i].val;    //取出对角线元素
	}
}

void VVertScalar::EqSolver(const VSGraphMesh & gm, const VSConstBuffer< std::pair<unsigned, float> > & VertScalar)
{
	MatRight(gm, VertScalar);
	TriTanSqr(gm);
	VertNbrSurfArea(gm);
	LapNoDiagElem(gm);
	LapDiagElem(gm);		

	unsigned vNum = gm.GetVertexCount();
	unsigned eNum = gm.GetEdgeCount();
	m_vVertFlag.resize(vNum);
	for (unsigned i = 0; i < VertScalar.len; i++)
	{
		m_vVertFlag[VertScalar.pData[i].first] = true;
	}
	for (unsigned i = 0; i < 2 * eNum; i++)
	{		
		unsigned vRow = m_vFirstLapTripl[i].row;		
		if (m_vVertFlag[vRow])
			m_vFirstLapTripl[i].val = - m_vFirstLapTripl[i].val / m_vDiagTemp[vRow] * (1.f - threshold);
		else
			m_vFirstLapTripl[i].val = -m_vFirstLapTripl[i].val / m_vDiagTemp[vRow];
	}
	for (unsigned i = 0; i < vNum; i++)
	{
		m_vFirstLapTripl[2 * eNum + i].val = -1.f;
	}

	A.row = vNum;
	A.col = vNum;
	A.val = VD_V2CB(m_vFirstLapTripl);
	b.row = vNum;
	b.col = 1;
	b.pData = &m_vR.front();
}

VD_EXPORT_SYSTEM_SIMPLE(VVertScalar, VNALGMESH::VRVertScalar);