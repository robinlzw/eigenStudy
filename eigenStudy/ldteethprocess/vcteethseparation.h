#pragma once
#include "lsalgteethprocess.h"
#include <set>
enum
{
	STATUS_FLAG_E_UNINIT = 0,
	STATUS_FLAG_E_INITED,
};

unsigned GetNext(unsigned &CurrEdgeIdx, const VSPerfectMesh& mesh, std::set<unsigned>& mEdge, std::vector<unsigned>& SurfStatus)
{

	if (mEdge.begin() != mEdge.end())
	{
		CurrEdgeIdx = *mEdge.begin();
	}
	else
	{
		return VD_INVALID_INDEX;
	}
	mEdge.erase(CurrEdgeIdx);
	VNVECTOR2UI soe = mesh.GetSurfIndxOfEdge(CurrEdgeIdx);
	if (STATUS_FLAG_E_INITED == SurfStatus[soe.x] && STATUS_FLAG_E_INITED == SurfStatus[soe.y])
	{
		return GetNext(CurrEdgeIdx, mesh, mEdge, SurfStatus);
	}
	else if (STATUS_FLAG_E_UNINIT == SurfStatus[soe.x])
	{
		return soe.x;
	}
	else
	{
		return soe.y;

	}
}


//////////////////////////////////////////////////////////////////////////
// 函数: void FuncGetVertRef(const unsigned nToothlabel)
// 函数: void FuncGetSurfRef(const unsigned nToothlabel )
// 函数: void FuncGetVertOnJawMeshRef(const unsigned nToothlabel )
//
//函数FuncGetVertRef FuncGetSurfRef 使用例子
//for (size_t i = 0; i < segmentedJaw.nToothCount; i++)
//{
//std::vector<VFVECTOR3>& vertices = getVertRef(i);
//std::vector<VNVECTOR3UI>& surfaces = getSurfRef(i);
//std::vector<unsigned>& vertOnJawMesh = getVertOnJawMeshRef(i);
//}

template < typename FuncGetVertRef, typename FuncGetSurfRef, typename FuncGetVertOnJawMeshRef >
void SplitTooth(const VNTrnCut::VSRawSegmentedJaw& segmentedJaw,
	const VSConstBuffer<unsigned>& cbLabelSurface,
	FuncGetVertRef getVertRef, FuncGetSurfRef getSurfRef,
	FuncGetVertOnJawMeshRef getVertOnJawMeshRef)
{
#if 1
	std::vector<unsigned> VertStatus;
	VertStatus.resize(segmentedJaw.mshWhole.GetVertexCount(), VD_INVALID_INDEX);
	std::vector<unsigned> SurfStatus;
	const VSPerfectMesh mesh = segmentedJaw.mshWhole;
	SurfStatus.resize(mesh.GetSurfCount(), STATUS_FLAG_E_UNINIT);
	for (unsigned NumOfTeeth = 0; NumOfTeeth < cbLabelSurface.len; NumOfTeeth++)
	{
		std::vector<VFVECTOR3>& vertices = getVertRef(NumOfTeeth);
		std::vector<VNVECTOR3UI>& surfaces = getSurfRef(NumOfTeeth);
		std::vector<unsigned>& vertOnJawMesh = getVertOnJawMeshRef(NumOfTeeth);
		
		unsigned nextSIdx = cbLabelSurface.pData[NumOfTeeth];
		std::set<unsigned> mEdge;
		unsigned CurrEdgeIdx;
		const VNVECTOR3UI & f = mesh.pSurfaces[nextSIdx];
		if ((segmentedJaw.cbVertexLable.pData[f.x] == segmentedJaw.cbVertexLable.pData[f.y]) &&
			(segmentedJaw.cbVertexLable.pData[f.x] == segmentedJaw.cbVertexLable.pData[f.z]) &&
			(segmentedJaw.cbVertexLable.pData[f.x] == NumOfTeeth))
		{
			SurfStatus[nextSIdx] = STATUS_FLAG_E_INITED;

			//surfaces.push_back(f);
			for (unsigned i = 0; i < 3; i++)
			{
				if (VD_INVALID_INDEX == VertStatus[f[i]])
				{
					VertStatus[f[i]] = vertOnJawMesh.size();
					vertOnJawMesh.push_back(f[i]);
					vertices.push_back(segmentedJaw.mshWhole.pVertices[f[i]]);
				}
			}

			surfaces.push_back(VNVECTOR3UI(VertStatus[f.x], VertStatus[f.y], VertStatus[f.z]));

			VNVECTOR3UI  eos = mesh.GetEdgeIndxOfSurf(nextSIdx);
			CurrEdgeIdx = eos.x;
			mEdge.insert(eos.y);
			mEdge.insert(eos.z);
			VNVECTOR2UI soe = mesh.GetSurfIndxOfEdge(CurrEdgeIdx);
			nextSIdx = (soe.x == nextSIdx) ? soe.y : soe.x;
			while (1)
			{
				//if (mEdge.begin() == mEdge.end())
				//{
				//	break;
				//}

				const VNVECTOR3UI & f = mesh.pSurfaces[nextSIdx];
				if ((segmentedJaw.cbVertexLable.pData[f.x] == segmentedJaw.cbVertexLable.pData[f.y]) &&
					(segmentedJaw.cbVertexLable.pData[f.x] == segmentedJaw.cbVertexLable.pData[f.z]) &&
					(segmentedJaw.cbVertexLable.pData[f.x] == NumOfTeeth))
				{

					SurfStatus[nextSIdx] = STATUS_FLAG_E_INITED;

					//surfaces.push_back(f);
					for (unsigned i = 0; i < 3; i++)
					{
						if (VD_INVALID_INDEX == VertStatus[f[i]])
						{
							VertStatus[f[i]] = vertOnJawMesh.size();
							vertOnJawMesh.push_back(f[i]);
							vertices.push_back(segmentedJaw.mshWhole.pVertices[f[i]]);
						}
					}

					surfaces.push_back(VNVECTOR3UI(VertStatus[f.x], VertStatus[f.y], VertStatus[f.z]));

					VNVECTOR3UI  eos = mesh.GetEdgeIndxOfSurf(nextSIdx);
					if (CurrEdgeIdx != eos.x)
					{
						if (CurrEdgeIdx != eos.y)
						{
							mEdge.insert(eos.y);
						}
						else
						{
							mEdge.insert(eos.z);
						}
						CurrEdgeIdx = eos.x;
					}
					else if (CurrEdgeIdx != eos.y && CurrEdgeIdx != eos.z)
					{
						CurrEdgeIdx = eos.y;
						mEdge.insert(eos.z);
					}
					VNVECTOR2UI soe = mesh.GetSurfIndxOfEdge(CurrEdgeIdx);
					nextSIdx = (soe.x == nextSIdx) ? soe.y : soe.x;
					if (STATUS_FLAG_E_INITED == SurfStatus[nextSIdx])
					{
						mEdge.erase(CurrEdgeIdx);
						nextSIdx = GetNext(CurrEdgeIdx, mesh, mEdge, SurfStatus);
						if (VD_INVALID_INDEX == nextSIdx)
							break;
					}
				}
				else
				{
					nextSIdx = GetNext(CurrEdgeIdx, mesh, mEdge, SurfStatus);
					if (VD_INVALID_INDEX == nextSIdx)
						break;
				}
			}
		}
	}
#endif
}
