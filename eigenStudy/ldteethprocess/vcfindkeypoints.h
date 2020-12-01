#pragma once
#include "lsalgteethprocess.h"

// 函数FUNC: void FUNC(const unsigned nToothlabel, const unsigned nKeyPointIdx)
template< typename FUNC >
void FindKeyPoints(std::vector<VFVECTOR3>& vKeyTriangles,
	const NMALG_TEETHPATCHING::VSSegmentedJaw& segmentdJaw,
	FUNC appKeyP)
{
	/*
	函数FUNC 使用例子
	unsigned nToothLabel = 1;
	unsigned nKeyPIdx = 1;
	appKeyP(nToothLabel, nKeyPIdx);
	*/
	unsigned ToothCount = segmentdJaw.nToothCount;

	unsigned m_nSurfacesCnt = segmentdJaw.mshWhole.GetSurfCount();

	for (unsigned nSIdx = 0; nSIdx < m_nSurfacesCnt; ++nSIdx)
	{
		const VNVECTOR3UI & sv = segmentdJaw.mshWhole.pSurfaces[nSIdx];
		unsigned nVLableX = segmentdJaw.cbVertexLable.pData[sv.x];
		unsigned nVLableY = segmentdJaw.cbVertexLable.pData[sv.y];
		unsigned nVLableZ = segmentdJaw.cbVertexLable.pData[sv.z];
		if ((nVLableX != nVLableY) && (nVLableX != nVLableZ) && (nVLableY != nVLableZ))
		{			
			VFVECTOR3 mVert1 = segmentdJaw.mshWhole.pVertices[sv.x];
			VFVECTOR3 mVert2 = segmentdJaw.mshWhole.pVertices[sv.y];
			VFVECTOR3 mVert3 = segmentdJaw.mshWhole.pVertices[sv.z];
			VFVECTOR3 mCentralPoint = (mVert1 + mVert2 + mVert3) / 3.0f;
			
			vKeyTriangles.push_back(mCentralPoint);
			
			if (VD_INVALID_INDEX != nVLableX)
				appKeyP(nVLableX, mCentralPoint);

			if (VD_INVALID_INDEX != nVLableY)
				appKeyP(nVLableY, mCentralPoint);

			if (VD_INVALID_INDEX != nVLableZ)
				appKeyP(nVLableZ, mCentralPoint);
		}
	}
}

