#pragma once
#include "lsalgteethprocess.h"
//////////////////////////////////////////////////////////////////////////

template<typename FUNC>
static void CheckVertAttrEx(const VSConstBuffer<unsigned>& cbBndryLineOnJawMesh,
	const VSPerfectMesh& pm, const unsigned mStart, const unsigned mEnd, FUNC f)
{
	for (unsigned j = mStart; j <= mEnd; j++)
	{
		unsigned nVIdx = cbBndryLineOnJawMesh.pData[j];
		unsigned EdgeNum = pm.GetEdgeCountOfVetex(nVIdx);
		for (unsigned nEIdx = 0; nEIdx < EdgeNum; nEIdx++)
		{
			VTopoGraph::VOE voe = pm.GetEdgeOfVertex(nVIdx, nEIdx).Opposite();
			unsigned nOppoV = pm.GetVertIndxOfEdge(voe.edgeIndx)[voe.vertInEdge];			
			f(nOppoV, j);
		}
	}
}

static void CheckVertAttr(unsigned nToothLabel, const VSConstBuffer<unsigned>& cbBndryLineOnJawMesh,
	const NMALG_TEETHPATCHING::VSSegmentedJaw& segmentedJaw,
	std::vector<std::vector<unsigned>>& otherToothLabel, 
	const unsigned mStart,  const unsigned mEnd, std::vector<unsigned> vertStatus)
{
	const VSPerfectMesh& pm = segmentedJaw.mshWhole;
	for (unsigned j = mStart; j < mEnd; j++)
	{
		unsigned nVIdx = cbBndryLineOnJawMesh.pData[j];
		unsigned EdgeNum = pm.GetEdgeCountOfVetex(nVIdx);
		for (unsigned nEIdx = 0; nEIdx <= EdgeNum; nEIdx++)
		{
			VTopoGraph::VOE voe = pm.GetEdgeOfVertex(nVIdx, nEIdx).Opposite();
			unsigned nOppoV = pm.GetVertIndxOfEdge(voe.edgeIndx)[voe.vertInEdge];
			unsigned VertAttr = segmentedJaw.cbVertexLable.pData[nOppoV];
			if ((nToothLabel != VertAttr) && (VD_INVALID_INDEX != VertAttr))
			{				
				if (VD_INVALID_INDEX == vertStatus[nOppoV])
				{
					otherToothLabel[VertAttr].push_back(nOppoV);
					vertStatus[nOppoV] = 1;
				}
			}
		}
	}
}



template<typename FuncGetNbrBndryLine>
void FindBndryToPatch(unsigned nToothLabel, const VSConstBuffer<unsigned>& cbNbrKeyPoints,
	const VSConstBuffer<unsigned>& cbBndryLineOnJawMesh, const NMALG_TEETHPATCHING::VSSegmentedJaw& segmentedJaw,
	FuncGetNbrBndryLine getNbrBndryLine)
{
	const VSPerfectMesh& pm = segmentedJaw.mshWhole;
	
	
	//std::vector<unsigned> vertStatus(segmentedJaw.mshWhole.GetVertexCount(),VD_INVALID_INDEX);
	
	

	for (unsigned i = 0; i < cbNbrKeyPoints.len; i++)
	{
		std::vector<unsigned> otherToothLabel(segmentedJaw.nToothCount + 1, 0);
		
		unsigned mStart = cbNbrKeyPoints.pData[i];
		unsigned mEnd;
		if (cbNbrKeyPoints.len - 1 == i)
		{
			mEnd = cbNbrKeyPoints.pData[0];
		}
		else
		{
			mEnd = cbNbrKeyPoints.pData[i + 1];
		}

		auto AddVert = [&segmentedJaw, &otherToothLabel](const unsigned nVertIdxOnJaw, const unsigned nVertIdxOnTooth) {
			unsigned nVertLabel = segmentedJaw.cbVertexLable.pData[nVertIdxOnJaw];
			unsigned nToothIdx = nVertLabel;
			if (segmentedJaw.nToothCount < nToothIdx)
			{
				nToothIdx = segmentedJaw.nToothCount;
			}
			otherToothLabel[nToothIdx]++;
		};

		if (mStart < mEnd)
		{
			//CheckVertAttr(nToothLabel, cbBndryLineOnJawMesh, segmentedJaw, otherToothLabel, mStart, mEnd);// , vertStatus);
			CheckVertAttrEx(cbBndryLineOnJawMesh, segmentedJaw.mshWhole, mStart, mEnd, AddVert);
		}
		else
		{
			//CheckVertAttr(nToothLabel, cbBndryLineOnJawMesh, segmentedJaw, otherToothLabel, mStart, cbBndryLineOnJawMesh.len - 1);// , vertStatus);
			//CheckVertAttr(nToothLabel, cbBndryLineOnJawMesh, segmentedJaw, otherToothLabel, 0, mEnd);// , vertStatus);
			CheckVertAttrEx(cbBndryLineOnJawMesh, segmentedJaw.mshWhole, mStart, cbBndryLineOnJawMesh.len - 1, AddVert);
			CheckVertAttrEx(cbBndryLineOnJawMesh, segmentedJaw.mshWhole, 0, mEnd, AddVert);
		}

		std::vector<unsigned> vSize(3, 0);
		unsigned nMaxSize = otherToothLabel[0];
		unsigned nMaxIdx = 0;
		for (unsigned j = 1; j < otherToothLabel.size(); j++)
		{
			if (otherToothLabel[j] > nMaxSize)
			{
				nMaxSize = otherToothLabel[j];
				nMaxIdx = j;
			}
		}
		if (nMaxIdx == nToothLabel)
		{
			nMaxSize = 0;
			unsigned nNbrIdx = 0;
			for (unsigned j = 0; j < otherToothLabel.size(); j++)
			{
				if (nMaxIdx != j)
				{
					if (otherToothLabel[j] > nMaxSize)
					{
						nMaxSize = otherToothLabel[j];
						nNbrIdx = j;
					}
				}
			}
			if ((nNbrIdx != nToothLabel) && (nNbrIdx < segmentedJaw.nToothCount))
			{
				VSToothNbrEdgeLine& s = getNbrBndryLine();
				s.nNbrTooth = nNbrIdx;
				if (mStart < mEnd)
				{				
					for (size_t j = mStart; j <= mEnd; j++)
					{
						s.vBndryLine.push_back(j);
					}					
				}
				else
				{
					for (size_t j = mStart; j <cbBndryLineOnJawMesh.len; j++)
					{
						s.vBndryLine.push_back(j);
					}
					for (size_t j = 0; j <= mEnd; j++)
					{
						s.vBndryLine.push_back(j);
					}
				}

				
			}
		}
	}
}

void PatchToothSide(unsigned& nVertCount, std::vector<VNVECTOR3UI>& vSurfaces, unsigned& nCenterIndex,
	const VSConstBuffer<unsigned>& cbBndry, const unsigned addVertCount);
void CalcToothSideVertices(std::vector<VFVECTOR3>& vVertices, const VSConstBuffer<VFVECTOR3>& cbBndryVerts,
	const VSConstBuffer<VNVECTOR3UI>& cbSurfaces, const unsigned nVertCount);