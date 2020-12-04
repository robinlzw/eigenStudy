#include"stdafx.h"
#include"VBLaplace2DeformMat.h"

void VBLaplace2DeformMat::Modify(const VSSprsMatrixColMajor<double>& secondLap, const VSConstBuffer<char>& destVert)
{
	m_vValues.clear();
	m_vInnerIndices.clear();
	m_vValues.reserve(secondLap.cbIndice.len + secondLap.row);
	m_vInnerIndices.reserve(secondLap.cbIndice.len + secondLap.row);
	m_vOuterStarts.resize(secondLap.col + 1);
	m_vOuterStarts[0] = 0;     //起始位置为零

	m_vOrgDiagElem.resize(secondLap.row);

	unsigned innerCount = 0;
	double diagElem = 1.f;
	unsigned outSize = m_vOuterStarts.size();

	for (unsigned i = 0; i < outSize - 1; i++)
	{
		unsigned OutStarts = secondLap.cbOuterIndice.pData[i];
		unsigned end = secondLap.cbOuterIndice.pData[i + 1];

		auto pStarts = secondLap.cbIndice.pData + OutStarts;
		auto pEnd = secondLap.cbIndice.pData + end;
		auto res = std::find(pStarts, pEnd, i);
		VASSERT(res != pEnd);
		m_vOrgDiagElem[i] = secondLap.pValues[OutStarts + (res - pStarts)];
	}

	for (unsigned i = 0; i < outSize - 1; i++)
	{
		unsigned OutStarts = secondLap.cbOuterIndice.pData[i];
		unsigned end = secondLap.cbOuterIndice.pData[i + 1];
		
		for (unsigned j = 0; j < end - OutStarts; j++)
		{
			unsigned rowPos = secondLap.cbIndice.pData[OutStarts + j];
			if (destVert.pData[rowPos] && i == rowPos)
			{				
				m_vValues.push_back(diagElem);
				m_vInnerIndices.push_back(i);
				innerCount++;				
			}
			else if(!destVert.pData[rowPos])
			{
				m_vValues.push_back(secondLap.pValues[OutStarts + j] / m_vOrgDiagElem[rowPos]);
				//m_vValues.push_back(secondLap.pValues[OutStarts + j]);
				m_vInnerIndices.push_back(rowPos);
				innerCount++;
			}
		}
		m_vOuterStarts[i + 1] = innerCount;
	}

}
// VD_EXPORT_SYSTEM_SIMPLE(VBLaplace2DeformMat, VNALGMESH::VRLaplace2DeformMat);