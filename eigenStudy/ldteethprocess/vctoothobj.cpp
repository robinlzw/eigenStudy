#include "stdafx.h"
#include <obj/lsalgtools/ilsalgtoolsperfmesh.h>
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/lsalgobjfile.h>
#include <obj/alg/ialgmatrix.h>
#include "vctoothobj.h"
#include "vctoothpatchside.h"
#include "vctoothpatchbtm.h"
#include "vctoothmakematrixs.h"
#include "vctoothfindstrawhatside.h"
#include "vctoothmeshsmooth.h" 





VCToothObj::VCToothObj(const unsigned nToothLabel,const double dbAvgEdgeLen, IVTracer& tracer)
	:m_tracer(tracer), m_nToothLabel(nToothLabel), m_dbAvgEdgeLen(dbAvgEdgeLen)
{
	m_ptrVRPerfMesh.reset(new TVR2B< VNALGMESH::VRPerfMesh >(*m_extVRPerfMesh));
	m_ptrVRGeodicMesh.reset(new TVR2B< VNALGMESH::VRGeodicMesh >(*m_extVRGeodicMesh));
	m_ptrVRSliceMeshBndry.reset(new TVR2B< VNALGMESH::VRSliceMeshBoundary >(*m_extVRSliceMeshBndry));
}

VCToothObj::VCToothObj(const unsigned nToothLabel, const double dbAvgEdgeLen, IVTracer& tracer,
	const NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth& topoTooth)
	:m_tracer(tracer), m_nToothLabel(nToothLabel), m_dbAvgEdgeLen(dbAvgEdgeLen)
{
	m_ptrVRPerfMesh.reset(new TVR2B< VNALGMESH::VRPerfMesh >(*m_extVRPerfMesh));
	m_ptrVRGeodicMesh.reset(new TVR2B< VNALGMESH::VRGeodicMesh >(*m_extVRGeodicMesh));
	m_ptrVRSliceMeshBndry.reset(new TVR2B< VNALGMESH::VRSliceMeshBoundary >(*m_extVRSliceMeshBndry));
	m_vVertices.resize(topoTooth.topoToothMesh.nVertCount);
	m_vSurfaces.resize(topoTooth.topoToothMesh.nTriangleCount);
	std::memcpy(&m_vVertices[0], topoTooth.topoToothMesh.pVertices, sizeof(VFVECTOR3)*topoTooth.topoToothMesh.nVertCount);
	std::memcpy(&m_vSurfaces[0], topoTooth.topoToothMesh.pTriangles, sizeof(VNVECTOR3UI)*topoTooth.topoToothMesh.nTriangleCount);
	VSSimpleMeshF simpFile;
	simpFile.nTriangleCount = m_vSurfaces.size();
	simpFile.nVertCount = m_vVertices.size();
	simpFile.pVertices = &m_vVertices[0];
	simpFile.pTriangles = &m_vSurfaces[0];
	m_ptrVRPerfMesh->Build(m_perfMesh, simpFile);
	m_vStrawHatSideVerts.clear();
	m_vFixedVerts.clear();
	m_vCrownVerts.clear();
	for (size_t i = 0; i < topoTooth.vertTypes.len; i++)
	{
		switch (topoTooth.vertTypes.pData[i])
		{
		case NMALG_TEETHPATCHING::ConstCrown:
			m_vCrownVerts.push_back(i);
			m_vFixedVerts.push_back(i);
			break;
		case NMALG_TEETHPATCHING::ConstRoot:
			m_vFixedVerts.push_back(i);
			m_vBtmCtrlPoints.push_back(i);
			break;
		case NMALG_TEETHPATCHING::CrownEdge:
			m_vStrawHatSideVerts.push_back(i);
			break;
		case NMALG_TEETHPATCHING::SideAdded:
		case NMALG_TEETHPATCHING::BtmAdded:
			break;
		default:
			break;
		}
	}
	m_vGumLine.resize(topoTooth.gumLineVerts.len);
	std::memcpy(&m_vGumLine[0], topoTooth.gumLineVerts.pData, sizeof(unsigned)*topoTooth.gumLineVerts.len);
}
VCToothObj::~VCToothObj()
{
	m_ptrVRPerfMesh.reset();
	m_ptrVRGeodicMesh.reset();
	m_ptrVRSliceMeshBndry.reset();
}

void VCToothObj::Reset()
{
	m_vSurfaces.clear();
	m_vVertices.clear();
	m_vSideAddPoints.clear();
	m_vVertType.clear();
	m_vFixedVerts.clear();
	m_vCrownVerts.clear();
	m_vStrawHatSideVerts.clear();
	m_vStrawHatSideVertsOnJawMesh.clear();
	m_vNbrKeyPoints.clear();
	m_vNbrEdgeLine.clear();
	m_vVertOnJawMesh.clear();
	m_vNbrPatchedVert.clear();
	m_vMatrixL.clear();
}

bool VCToothObj::Init()
{
	// 在调用Init之前，顶点和三角面片已经都准备好了。
	if ((0 == m_vVertices.size()) || (0 == m_vSurfaces.size()))
	{
		LOG_DumpError("Tooth %d has no data!!!", m_nToothLabel);
		return false;
	}	
		 
	UpdatePerfMesh();
	m_ptrVRSliceMeshBndry->Build(m_edgeLine, m_perfMesh);
	m_vBndryLine.resize(m_edgeLine.len);
	memcpy(&m_vBndryLine[0], m_edgeLine.pData, m_edgeLine.len * sizeof(unsigned));
	m_ptrVRGeodicMesh->Build(m_meshGeodic, m_perfMesh, m_edgeLine);
#if 0
	{
		char szFileName[256] = { 0 };
		sprintf_s(szFileName, "D:/testdata/ld/patchtooth_%d.obj", m_nToothLabel);
		OBJWritePerfectMesh(szFileName, m_perfMesh);
	}
#endif
	return true;
}

void VCToothObj::AppendKeyPoint(const VFVECTOR3& keyPoint)
{
	m_vNbrKeyPoints.push_back(keyPoint);
}

std::vector<VFVECTOR3>& VCToothObj::GetVertRef()
{
	return m_vVertices;
}

std::vector<VNVECTOR3UI>& VCToothObj::GetSurfRef()
{
	return m_vSurfaces;
}

std::vector<unsigned>& VCToothObj::GetVertOnJawMeshRef()
{
	return m_vVertOnJawMesh;
}

void VCToothObj::FindStrawHatSideVerts()
{
	::FindStrawHatSideVerts(m_vStrawHatSideVerts, m_vFixedVerts, m_meshGeodic);
	if (m_vVertices.size() > 0)
	{
		m_vVertType.resize(m_vVertices.size(), NMALG_TEETHPATCHING::ConstCrown);
		for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
		{
			m_vVertType[m_vStrawHatSideVerts[i]] = NMALG_TEETHPATCHING::CrownEdge;
		}
	}
	m_vCrownVerts.resize(m_vFixedVerts.size());
	std::memcpy(&m_vCrownVerts[0], &m_vFixedVerts[0], sizeof(unsigned) * m_vFixedVerts.size());
}

void VCToothObj::FindNbrBndryToPatch(const NMALG_TEETHPATCHING::VSSegmentedJaw& segmentedJaw)
{
	auto GetNrbBndryLine = [this]() ->VSToothNbrEdgeLine& {
		m_vNbrEdgeLine.resize(m_vNbrEdgeLine.size() + 1);
		return m_vNbrEdgeLine[m_vNbrEdgeLine.size() - 1];
	};
	std::vector<unsigned> vBndryLineOnJawMesh(m_vBndryLine.size());
	std::vector<unsigned> vKeyPointsIdx(m_vNbrKeyPoints.size());
#if 0
	{		
		std::ofstream objFile("D:/testdata/bndry.obj");
		for (unsigned i = 0; i < m_vBndryLine.size(); i++)
		{
			const VFVECTOR3& v = m_perfMesh.pVertices[m_vBndryLine[i]];
			objFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
		}
		objFile.close();
	}
#endif
	for (size_t i = 0; i < m_vBndryLine.size(); i++)
	{
		vBndryLineOnJawMesh[i] = m_vVertOnJawMesh[m_vBndryLine[i]];
	}
	for (size_t i = 0; i < m_vNbrKeyPoints.size(); i++)
	{
		unsigned nIdx = 0;
		float flVal = (m_perfMesh.pVertices[m_vBndryLine[0]] - m_vNbrKeyPoints[i]).Magnitude();
		for (size_t j = 1; j < m_vBndryLine.size(); j++)
		{
			float flTmp = (m_perfMesh.pVertices[m_vBndryLine[j]] - m_vNbrKeyPoints[i]).Magnitude();
			if (flTmp < flVal)
			{
				flVal = flTmp;
				nIdx = j;
			}
			vKeyPointsIdx[i] = nIdx;
		}
	}
	std::sort(vKeyPointsIdx.begin(), vKeyPointsIdx.end());
	::FindBndryToPatch(m_nToothLabel,VD_V2CB(vKeyPointsIdx), VD_V2CB(vBndryLineOnJawMesh), segmentedJaw, GetNrbBndryLine);
#if 0
	for (size_t nTIdx = 0; nTIdx < m_vNbrEdgeLine.size(); nTIdx++)
	{
		unsigned nToothIdx = m_vNbrEdgeLine[nTIdx].nNbrTooth;
		if (VD_INVALID_INDEX == nToothIdx)
			continue;
		
		for (size_t nIdx = nTIdx+1; nIdx < m_vNbrEdgeLine.size(); nIdx++)
		{
			unsigned nToothIdx1 = m_vNbrEdgeLine[nIdx].nNbrTooth;
			if (VD_INVALID_INDEX == nToothIdx1)
				continue;

			if (nToothIdx == nToothIdx1) 
			{
				int nDiff = m_vNbrEdgeLine[nIdx].vBndryLine[0] - m_vNbrEdgeLine[nTIdx].vBndryLine[m_vNbrEdgeLine[nTIdx].vBndryLine.size() - 1];
				if (nDiff <= 1)
				{
					m_vNbrEdgeLine[nTIdx].vBndryLine.insert(m_vNbrEdgeLine[nTIdx].vBndryLine.end(), m_vNbrEdgeLine[nIdx].vBndryLine.begin() + (1 - nDiff), m_vNbrEdgeLine[nIdx].vBndryLine.end());
					m_vNbrEdgeLine[nIdx].vBndryLine.clear();
					m_vNbrEdgeLine[nIdx].nNbrTooth = VD_INVALID_INDEX;
				}
			}
		}
	}
#endif
#if 0
	{		
		for (unsigned i = 0; i < m_vNbrEdgeLine.size(); i++)
		{
			char szFileName[256] = { 0 };
			sprintf_s(szFileName, 256, "D:/testdata/bndrynbr_%d.obj", i);
			std::ofstream objFile(szFileName);
			for (unsigned j = 0; j < m_vNbrEdgeLine[i].vBndryLine.size(); j++)
			{
				const VFVECTOR3& v = m_perfMesh.pVertices[m_vBndryLine[m_vNbrEdgeLine[i].vBndryLine[j]]];
				objFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
			}
			objFile.close();
		}
	}
#endif
}

void VCToothObj::PatchToothNbrSide()
{		
	std::vector<std::vector<VNVECTOR3UI> > vvAddSurfs(m_vNbrEdgeLine.size());
	std::vector<std::vector<VFVECTOR3> > vvVertices(m_vNbrEdgeLine.size());
	std::vector<unsigned> vCtrlPoints(m_vNbrEdgeLine.size(), VD_INVALID_INDEX);
	for (unsigned j = 0; j < m_vNbrEdgeLine.size(); j++)
	{	
		std::vector<unsigned>& edgeLine = m_vNbrEdgeLine[j].vBndryLine;
		if (edgeLine.size() > 5)
		{
			std::vector<VFVECTOR3> vBndryVerts;

			for (size_t i = 0; i < edgeLine.size(); i++)
			{
				vBndryVerts.push_back(m_perfMesh.pVertices[m_vBndryLine[edgeLine[i]]]);
			}

			VFVECTOR3 offset = (vBndryVerts[0] - vBndryVerts[vBndryVerts.size() - 1]);
			double dbVal = offset.Magnitude();
			unsigned nCount = dbVal / m_dbAvgEdgeLen;
			unsigned nVertCount = 0;
			if (nCount < 1)
				continue;
			
			::PatchToothSide(nVertCount, vvAddSurfs[j], vCtrlPoints[j], VD_V2CB(m_vNbrEdgeLine[j].vBndryLine), nCount+1);

			if (vvAddSurfs[j].size() == 0)
				continue;

			offset = offset / ((float)nCount);
			for (size_t i = 1; i < nCount; i++)
			{
				vBndryVerts.push_back(m_perfMesh.pVertices[m_vBndryLine[edgeLine[edgeLine.size() - 1]]] + offset * (float)i);
			}
			//OBJWriteVertices(VD_V2CB(vBndryVerts), "D:/testdata/bndrycycle.obj");
			::CalcToothSideVertices(vvVertices[j], VD_V2CB(vBndryVerts), VD_V2CB(vvAddSurfs[j]), nVertCount);
		}
	}
	UpdateMesh(VD_V2CB(vvVertices), VD_V2CB(vvAddSurfs), VD_V2CB(vCtrlPoints));
		
#if 0
	{
		VSSimpleMeshF simpMesh;
		simpMesh.pTriangles = &vvAddSurfs[0][0];
		simpMesh.pVertices = &vvVertices[0][0];
		simpMesh.nVertCount = vvVertices[0].size();
		simpMesh.nTriangleCount = vvAddSurfs[0].size();
		OBJWriteSimpleMesh("D:/testdata/nbrpatchedtooth.obj", simpMesh);
	}
#endif
	UpdatePerfMesh();			// 修补侧面后，重新构造网格拓扑，并保留原始顶点和三角片顺序

	VNALGMESH::VSMeshVertLine gumLine;
	m_ptrVRSliceMeshBndry->Build(gumLine, m_perfMesh);	// 构造牙龈线
	//OBJWritePerfectMesh("D:/testdata/patchedperftooth.obj", m_perfMesh);
	m_vGumLine.resize(gumLine.len);
	memcpy(&m_vGumLine[0], gumLine.pData, gumLine.len * sizeof(unsigned));	
}

void VCToothObj::PatchBottom()
{	
	std::vector<VNVECTOR3UI> vAddSurfs;
	std::vector<VFVECTOR3> vBndryLine;
	std::vector<VFVECTOR3> vVertices;
	unsigned nVertCount = 0;	
	vBndryLine.resize(m_vGumLine.size());
	for (size_t i = 0; i < m_vGumLine.size(); i++)
	{
		vBndryLine[i] = m_perfMesh.pVertices[m_vGumLine[i]];
	}

	::PatchToothBottom(nVertCount, vAddSurfs, m_vBtmCtrlPoints, VD_V2CB(vBndryLine));

	::CalcToothSideVertices(vVertices, VD_V2CB(vBndryLine), VD_V2CB(vAddSurfs), nVertCount);
		
	UpdateMeshWithBtm(VD_V2CB(vVertices), VD_V2CB(vAddSurfs), VD_V2CB(m_vGumLine));
	UpdatePerfMesh();
	CalcToothAxis();
}

void VCToothObj::UpdateMesh(const VSConstBuffer<std::vector<VFVECTOR3> >& cbVertices, 
	const VSConstBuffer<std::vector<VNVECTOR3UI> >& cbSurfaces, const VSConstBuffer<unsigned>& cbCtrlPoints)
{
	m_vNbrPatchedVert.resize(cbSurfaces.len);
	for (size_t i = 0; i < cbVertices.len; i++)
	{
		m_vNbrPatchedVert[i].ctrlVert = VD_INVALID_INDEX;
		m_vNbrPatchedVert[i].nNbrTooth = m_vNbrEdgeLine[i].nNbrTooth;
		if (0 == cbVertices.pData[i].size())
			continue;

		unsigned nStart = m_vVertices.size();
		unsigned nBndryLen = m_vNbrEdgeLine[i].vBndryLine.size();
		m_vNbrPatchedVert[i].ctrlVert = cbCtrlPoints.pData[i] - nBndryLen + nStart;
		
		for (size_t j = nBndryLen; j < cbVertices.pData[i].size(); j++)
		{
			unsigned nVertIdx = m_vVertices.size();			
			m_vNbrPatchedVert[i].vVertices.push_back(nVertIdx);			
			m_vSideAddPoints.push_back(nVertIdx);
			m_vVertices.push_back(cbVertices.pData[i][j]);
			m_vVertType.push_back(NMALG_TEETHPATCHING::SideAdded);
		}
		for (size_t j = 0; j < cbSurfaces.pData[i].size(); j++)
		{
			const VNVECTOR3UI& s = cbSurfaces.pData[i][j];
			unsigned nVal[3] = { VD_INVALID_INDEX };
			for (size_t k = 0; k < 3; k++)
			{
				if (s[k] < nBndryLen)
					nVal[k] = m_vBndryLine[m_vNbrEdgeLine[i].vBndryLine[s[k]]];
				else
					nVal[k] = s[k] - nBndryLen + nStart;
			}
			m_vSurfaces.push_back(VNVECTOR3UI(nVal[0], nVal[1], nVal[2]));
		}
	}
}

void VCToothObj::UpdateMeshWithBtm(const VSConstBuffer<VFVECTOR3>& cbVertices,
	const VSConstBuffer<VNVECTOR3UI>& cbSurfaces, 
	const VSConstBuffer<unsigned>& cbBndryLine)
{
	unsigned nStart = m_vVertices.size();
	unsigned nBndryLen = cbBndryLine.len;
	std::vector<unsigned> vCtrlPoints;
	for (size_t j = nBndryLen; j < cbVertices.len; j++)
	{
		unsigned nVertIdx = m_vVertices.size();	
		if (VD_INVALID_INDEX != m_vBtmCtrlPoints[j])
		{
			if (E_CTRLPOINT_CENTER == m_vBtmCtrlPoints[j])
			{
				m_nBottomCenter = nVertIdx;
			}
			//m_vBtmCtrlPoints[j] = nVertIdx;			
			vCtrlPoints.push_back(nVertIdx);
			m_vVertType.push_back(NMALG_TEETHPATCHING::ConstRoot);
		}
		else
			m_vVertType.push_back(NMALG_TEETHPATCHING::BtmAdded);
		m_vVertices.push_back(cbVertices.pData[j]);
	}
	m_vBtmCtrlPoints.resize(vCtrlPoints.size());
	std::memcpy(&m_vBtmCtrlPoints[0], &vCtrlPoints[0], vCtrlPoints.size() * sizeof(unsigned));
	for (size_t j = 0; j < cbSurfaces.len; j++)
	{
		const VNVECTOR3UI& s = cbSurfaces.pData[j];
		unsigned nVal[3] = { VD_INVALID_INDEX };
		for (size_t k = 0; k < 3; k++)
		{
			if (s[k] < nBndryLen)
				nVal[k] = cbBndryLine.pData[s[k]];
			else
				nVal[k] = s[k] - nBndryLen + nStart;
		}
		m_vSurfaces.push_back(VNVECTOR3UI(nVal[0], nVal[1], nVal[2]));
	}
}

void VCToothObj::SetToothAxis(const NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth & topoTooth, const NMALG_TEETHPATCHING::VSToothInfo& toothInfo)
{
#if 1
	m_toothAxis.m_TipRay = toothInfo.toothCuspDir;	
	//build a plane
	float flDiff = FLT_MAX;
	const VFVECTOR3& dir = m_toothAxis.m_TipRay.GetDirection();
	const VFVECTOR3& root = m_toothAxis.m_TipRay.GetOrigin();

	//std::ofstream datF("D:/tempdata/toothcuspdir.dat", std::ios::binary);
	//TVPersist<NMALG_TEETHPATCHING::VSToothInfo> perstToothInfo;
	//TVPersist<VFPlane> perstPlane;
	//perstToothInfo.Build(toothInfo);
	for (size_t i = 0; i < m_vGumLine.size(); i++)
	{
		float flVal = (m_perfMesh.pVertices[m_vGumLine[i]] - root).Dot(dir);
		if (flVal < flDiff)
		{
			flDiff = flVal;
		}
	}
	VFVECTOR3 mPoint = m_toothAxis.m_TipRay.GetPoint(flDiff - 1.0f);// -toothAxis.m_RootBackDis);
	VFPlane mPlane(m_toothAxis.m_TipRay.GetDirection(), mPoint);
	float flDisMin = FLT_MAX;
	//perstPlane.Build(mPlane);
	//datF << perstPlane << perstToothInfo;
	//datF.close();
	//int nIdxMin = 0;
	//for (int i = -50;  i < 100; i++)
	//{
	//	VFVECTOR3 v = toothInfo.toothCuspDir.GetPoint((float)i * (0.1f));
	//	float flDis = std::abs(mPlane.getDistance(v));
	//	if (flDis < flDisMin)
	//	{
	//		flDisMin = flDis;
	//		nIdxMin = i;
	//	}
	//}
	//VFVECTOR3 vMin = toothInfo.toothCuspDir.GetPoint((float)(nIdxMin -1) * 0.1f);
	//VFVECTOR3 vShift = mPoint - 
	//projection and save
	VFVECTOR3 dotInPlane = -mPlane.d * mPlane.normal;
	std::vector<VFVECTOR3> vVarVerts(topoTooth.vertTypes.len);
	m_vBtmCtrlPoints.clear();
	VFVECTOR3 vCenter = VFVECTOR3::ZERO;
	for (unsigned i = 0; i < topoTooth.vertTypes.len; i++)
	{
		if (NMALG_TEETHPATCHING::ConstRoot == topoTooth.vertTypes.pData[i])
		{
			VFVECTOR3 dif = dotInPlane - m_perfMesh.pVertices[i];
			dif = dif.Dot(mPlane.normal) * mPlane.normal;
			vVarVerts[i] = m_perfMesh.pVertices[i] + dif;			
			vCenter += vVarVerts[i];
			m_vBtmCtrlPoints.push_back(i);	
		}
	}

	vCenter /= (float)m_vBtmCtrlPoints.size();
	VFVECTOR3 dif = mPoint - vCenter;
	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		m_vVertices[m_vBtmCtrlPoints[i]] = vVarVerts[m_vBtmCtrlPoints[i]] + dif;
	}

#if 1
#else
	m_vFixedVerts.clear();	
	std::vector<unsigned> vVarVertsMap(topoTooth.vertTypes.len, VD_INVALID_INDEX);
	std::vector<unsigned> vVarVToTooth(topoTooth.vertTypes.len, VD_INVALID_INDEX);
	std::vector<unsigned> vCtrlsVMap(topoTooth.vertTypes.len, 0);
	m_vBtmCtrlPoints.clear();
	float flMinDis = FLT_MAX;// BTM_CTRL_RADIUS;
	unsigned nVarVCount = 0;
	unsigned nMinIdx = VD_INVALID_INDEX;
	std::ofstream objFile("D:/testdata/vert.obj");
	for (unsigned i = 0; i < topoTooth.vertTypes.len; i++)
	{
		if ((NMALG_TEETHPATCHING::BtmAdded == topoTooth.vertTypes.pData[i]) || (NMALG_TEETHPATCHING::ConstRoot == topoTooth.vertTypes.pData[i]))
		{
			VFVECTOR3 dif = dotInPlane - m_perfMesh.pVertices[i];
			dif = dif.Dot(mPlane.normal) * mPlane.normal;
			vVarVerts[i] = m_perfMesh.pVertices[i] + dif;
			objFile << "v " << vVarVerts[i].x << " " << vVarVerts[i].y <<" " << vVarVerts[i].z << std::endl;
			vVarVToTooth[nVarVCount] = i;
			vVarVertsMap[i] = nVarVCount++;
			float flVal = (vVarVerts[i] - mPoint).Magnitude();
			if (flVal < flMinDis)
			{
				flMinDis = flVal;
				nMinIdx = i;
			}
			//	vCtrlsVMap[i] = 1;
			//	m_vBtmCtrlPoints.push_back(i);
			//}			
		}	
		else if(NMALG_TEETHPATCHING::ConstCrown == topoTooth.vertTypes.pData[i])
			m_vFixedVerts.push_back(i);
	}
	std::vector<unsigned> v2Domain;
	TVExtSource<VNALGMESH::VRPerfMesh> extPerfM;
	TVR2B< VNALGMESH::VRPerfMesh > rbPerfM(*extPerfM);
	VSPerfectMesh perfM;
	rbPerfM.Build(perfM, topoTooth.topoToothMesh);
	PMGet1OrderDomain(v2Domain, nMinIdx, perfM);
	for (size_t i = 0; i < v2Domain.size(); i++)
	{
		if ((NMALG_TEETHPATCHING::BtmAdded == topoTooth.vertTypes.pData[v2Domain[i]]) || 
			(NMALG_TEETHPATCHING::ConstRoot == topoTooth.vertTypes.pData[v2Domain[i]]))
		{
			m_vBtmCtrlPoints.push_back(v2Domain[i]);
			m_vFixedVerts.push_back(v2Domain[i]);
			m_vVertices[v2Domain[i]] = vVarVerts[v2Domain[i]];
		}
	}
	objFile << "v " << mPoint.x << " " << mPoint.y << " " << mPoint.z << std::endl;
	objFile << "flMinDis: " << flMinDis << std::endl;
	objFile.close();
#endif
#if 0
	if (m_vBtmCtrlPoints.size() < 3)
	{
		unsigned nVIdx = 0;
		float flMin = FLT_MAX;
		std::memset(&vCtrlsVMap[0], 0, sizeof(unsigned) * topoTooth.vertTypes.len);

		m_vBtmCtrlPoints.clear();
		for (size_t i = 0; i < nVarVCount; i++)
		{
			float flVal = (vVarVerts[vVarVToTooth[i]] - mPoint).Magnitude();
			if (flVal < flMin)
			{
				flMin = flVal;
				nVIdx = vVarVToTooth[i];
			}
		}
		std::vector<unsigned> vDom;
		PMGet1OrderDomain(vDom, nVIdx, m_perfMesh);
		for (size_t i = 0; i <vDom.size();  i++)
		{
			if (VD_INVALID_INDEX != vVarVertsMap[vDom[i]])
			{
				vCtrlsVMap[vDom[i]] = 1;
				m_vBtmCtrlPoints.push_back(vDom[i]);
			}
		}		
	}

	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		m_vFixedVerts.push_back(m_vBtmCtrlPoints[i]);
		m_vVertices[m_vBtmCtrlPoints[i]] = vVarVerts[m_vBtmCtrlPoints[i]];
	}
#endif
#else
    auto rootVertIdxs = topoTooth.rootVerts;
    auto randomFunc = [](unsigned start, unsigned end) -> unsigned
    {
        return unsigned(double(start) + (double(end) - double(start))*rand() / (RAND_MAX + 1.0));
    };

    VASSERT(m_vBtmCtrlPoints.size() == rootVertIdxs.len);

    // cal tooth bottom plane;
    VFPlane rootPlane;
    bool noPlane = true;
    while (noPlane)
    {
        //srand(unsigned(time(0)));
        //unsigned a = randomFunc(0, rootVertIdxs.len - 1);
        //unsigned b = randomFunc(0, rootVertIdxs.len - 1);
        //unsigned c = randomFunc(0, rootVertIdxs.len - 1);
		unsigned nStep = rootVertIdxs.len / 4;
		unsigned a = 0;
		unsigned b = nStep;
		unsigned c = 2 * nStep;
        VFVector3 va = topoTooth.topoToothMesh.pVertices[rootVertIdxs.pData[a]];
        VFVector3 vb = topoTooth.topoToothMesh.pVertices[rootVertIdxs.pData[b]];
        VFVector3 vc = topoTooth.topoToothMesh.pVertices[rootVertIdxs.pData[c]];

        VFVector3 dir1 = va - vb;
        VFVector3 dir2 = va - vc;

        if (!dir1.IsZero() && !dir2.IsZero() && !vf_equal_real(abs(dir1.Dot(dir2)), 1.f))
        {
            rootPlane.redefine(va, vb, vc);

            unsigned crownIdx = 0;
            for (unsigned i = 0; i < topoTooth.vertTypes.len; i++)
            {
                if (topoTooth.vertTypes.pData[i] == NMALG_TEETHPATCHING::VEToothVertType::ConstCrown)
                {
                    crownIdx = i;
                    break;
                }
            }
            if (rootPlane.getSide(topoTooth.topoToothMesh.pVertices[crownIdx]) == VFPlane::NEGATIVE_SIDE)
            {
                rootPlane.redefine(va, vc, vb);
            }
            noPlane = false;
        }
    }
#if 0
	{
		char szFileName[256] = { 0 };
		sprintf_s(szFileName, "D:/testdata/ld/rootPlane%d.obj", m_nToothLabel);
		std::ofstream axisF(szFileName);
		const VFVECTOR3& dir = toothInfo.toothCuspDir.GetDirection();
		axisF << "rootPlane.normal: " << rootPlane.normal.x << " " << rootPlane.normal.y << " " << rootPlane.normal.z << std::endl;
		axisF << "dir: " << dir.x << " " << dir.y << " " << dir.z << std::endl;
		axisF.close();
	}
#endif	
    VFLocation rot(VFVector3::ZERO, VFQuaternion::FromRotateAxis(rootPlane.normal, toothInfo.toothCuspDir.GetDirection()));

    VFVector3 rootCenter = VFVector3::ZERO;
    for (unsigned i = 0; i < rootVertIdxs.len; i++)
    {
        rootCenter += topoTooth.topoToothMesh.pVertices[rootVertIdxs.pData[i]];
    }
    rootCenter /= rootVertIdxs.len;

    VFVector3 rootCenterToBackCenter = toothInfo.toothCuspDir.GetOrigin() + toothInfo.toothCuspDir.GetDirection() *  (-toothInfo.rootBackDis) - rootCenter;
#if 0
	{
		char szFileName[256] = { 0 };
		sprintf_s(szFileName, "D:/testdata/ld/initvertices_%d.obj", m_nToothLabel);
		OBJWriteVertices(VD_V2CB(m_vVertices), szFileName);
		sprintf_s(szFileName, "D:/testdata/ld/toothaxisinfo_%d.obj", m_nToothLabel);
		std::ofstream axisF(szFileName);
		axisF << "rootVertIdxs.len " << rootVertIdxs.len << std::endl;
		axisF << "rootCenter: " << rootCenter.x << " " << rootCenter.y << " " << rootCenter.z << std::endl;
		for (unsigned i = 0; i < rootVertIdxs.len; i++)
		{
			unsigned vertIdx = rootVertIdxs.pData[i];
			axisF << i << " " << "vertIdx  " << vertIdx << std::endl;
			VFVector3 vertData = topoTooth.topoToothMesh.pVertices[vertIdx];
			axisF << "vertData: " << vertData.x << " " << vertData.y << " " << vertData.z << std::endl;
			axisF << "rot pos: " << rot.position.x << " " << vertData.y << " " << vertData.z << std::endl;
			axisF << "rot orint: " << rot.orientation.s << " " << rot.orientation.x << " " << rot.orientation.y << " " << rot.orientation.z << std::endl;
			VFVector3 transVertData = rot.TransLocalVertexToGlobal(vertData - rootCenter) + rootCenter;
			axisF << "transVertData: " << transVertData.x << " " << transVertData.y << " " << transVertData.z << std::endl;
			transVertData += rootCenterToBackCenter;
			axisF << "transVertData: " << transVertData.x << " " << transVertData.y << " " << transVertData.z << std::endl;
		}
		axisF.close();
	}
#endif	
	
    for (unsigned i = 0; i < rootVertIdxs.len; i++)
    {
        unsigned vertIdx = rootVertIdxs.pData[i];
        VFVector3 vertData = topoTooth.topoToothMesh.pVertices[vertIdx];
        VFVector3 transVertData = rot.TransLocalVertexToGlobal(vertData - rootCenter) + rootCenter;

        transVertData += rootCenterToBackCenter;
        //float needBackDis = -toothInfo.rootBackDis - (transVertData - toothInfo.toothCuspDir.GetOrigin()).Dot(toothInfo.toothCuspDir.GetDirection());
        //transVertData += (toothInfo.toothCuspDir.GetDirection() * needBackDis);

        m_vVertices[vertIdx] = transVertData;
    }
#if 0
	{
		char szFileName[256] = { 0 };
		sprintf_s(szFileName, "D:/testdata/ld/vertices_%d.obj", m_nToothLabel);
		OBJWriteVertices(VD_V2CB(m_vVertices), szFileName);
	}
#endif	
#endif
}

void VCToothObj::CalcToothAxis()
{
	
	VFVECTOR3 dir = VFVECTOR3::ZERO;
	// 把底部控制点加入固定的数列，后面构造矩阵时用到。
	for (size_t i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		dir += m_perfMesh.pVertNormals[m_vBtmCtrlPoints[i]];		
		m_vFixedVerts.push_back(m_vBtmCtrlPoints[i]);		// 把底部控制点加入固定的数列，后面构造矩阵时用到。
	}	
	dir /= (float)(m_vBtmCtrlPoints.size());
	dir.Normalize();
	m_toothAxis.m_TipRay.SetOrigin(m_perfMesh.pVertices[m_nBottomCenter] + dir);
	m_toothAxis.m_TipRay.SetDirection(-dir);
	m_toothAxis.m_RootBackDis = 1.0f;
}

void VCToothObj::ExtendBottomCtrlPoints()
{
	std::vector<VFVECTOR3> vCtrlPoints(m_vBtmCtrlPoints.size());	
	ExtendToothBottomCtrlPoints(vCtrlPoints, m_toothAxis, VD_V2CB(m_vBtmCtrlPoints), VD_V2CB(m_vGumLine), m_perfMesh);
	for (size_t i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		m_vVertices[m_vBtmCtrlPoints[i]] = vCtrlPoints[i];
	}
}

void VCToothObj::MakeMatrixL()
{
	m_dbW0 = 1.0;
	m_dbW1 = 1000.0;
	m_dbW2 = 10.0;
	unsigned nVertCount = m_perfMesh.GetVertexCount();
	FillVertType();
	::PMMakeLaplaceMatrix(m_vMatrixL, m_perfMesh);
	//std::vector<VSTripletD> matGumLine;
	//MakeMatrixGumLine(matGumLine, m_perfMesh, VD_V2CB(m_vGumLine));
	// L0 = LaplaceMat + GumLineMat
	//LaplaceMatAddGumLineMat(m_vMatrixL, VD_V2CB(m_vGumLine), m_perfMesh);	
	::AppendVertsTo01Matrix(m_vMatrixL, nVertCount, VD_V2CB(m_vFixedVerts), m_dbW1);
	::AppendVertsTo01Matrix(m_vMatrixL, nVertCount + m_vFixedVerts.size(), VD_V2CB(m_vStrawHatSideVerts), m_dbW2);
#if 0
	char szFileName[256] = { 0 };
	sprintf_s(szFileName, 256, "D:/testdata/ld/ToothLMat_%d.txt", m_nToothLabel);
	std::ofstream logF(szFileName);
	for (size_t i = 0; i < m_vMatrixL.size(); i++)
	{
		logF << m_vMatrixL[i].row << " " << m_vMatrixL[i].col << " " << m_vMatrixL[i].val << std::endl;
	}
	logF.close();
#endif
}

void VCToothObj::MakeMatrixR()
{
#if 0
	char szFileName[256] = { 0 };
	sprintf_s(szFileName, 256, "D:/testdata/ld/ToothVertices_%d.obj", m_nToothLabel);
	OBJWriteVertices(VD_V2CB(m_vVertices), szFileName);
	sprintf_s(szFileName, 256, "D:/testdata/ld/ToothFixedVerts_%d.obj", m_nToothLabel);
	std::ofstream txtFill(szFileName);
	txtFill << "vert count: " << m_perfMesh.GetVertexCount() << std::endl;
	txtFill << "fixed count: " << m_vFixedVerts.size() << std::endl;
	for (size_t i = 0; i < m_vFixedVerts.size(); i++)
	{
		txtFill << i << " " << m_vFixedVerts[i] << std::endl;
	}
	txtFill << "var count: " << m_vStrawHatSideVerts.size() << std::endl;
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		txtFill << i << " " << m_vStrawHatSideVerts[i] << std::endl;
	}
	txtFill.close();
#endif
	unsigned nVertCount = m_perfMesh.GetVertexCount() + m_vStrawHatSideVerts.size() + m_vFixedVerts.size();
	m_pMatrixR.reset(new double[nVertCount * 3]);
	double* pData = m_pMatrixR.get();
	memset(pData, 0, sizeof(double) * nVertCount * 3);
	unsigned nShift1 = nVertCount;
	unsigned nShift2 = nShift1 + nVertCount;
	unsigned nRowBase = m_perfMesh.GetVertexCount();
	for (size_t i = 0; i < m_vFixedVerts.size(); i++)
	{
		const VFVECTOR3& v = m_vVertices[m_vFixedVerts[i]];
		pData[nRowBase + i] = v.x * m_dbW1;
		pData[nRowBase + i + nShift1] = v.y * m_dbW1;
		pData[nRowBase + i + nShift2] = v.z * m_dbW1;
		VSTripletD trip;
		trip.row = nRowBase + i;
		trip.col = 0;
		trip.val = v.x * m_dbW1;
		m_vMatrixR.push_back(trip);
		trip.col = 1;
		trip.val = v.y * m_dbW1;
		m_vMatrixR.push_back(trip);
		trip.col = 2;
		trip.val = v.z * m_dbW1;
		m_vMatrixR.push_back(trip);
	}
	nRowBase += m_vFixedVerts.size();
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		const VFVECTOR3& v = m_vVertices[m_vStrawHatSideVerts[i]];
		pData[nRowBase + i] = v.x * m_dbW2;
		pData[nRowBase + i + nShift1] = v.y * m_dbW2;
		pData[nRowBase + i + nShift2] = v.z * m_dbW2;
		VSTripletD trip;
		trip.row = nRowBase + i;
		trip.col = 0;
		trip.val = v.x * m_dbW2;
		m_vMatrixR.push_back(trip);
		trip.col = 1;
		trip.val = v.y * m_dbW2;
		m_vMatrixR.push_back(trip);
		trip.col = 2;
		trip.val = v.z * m_dbW2;
		m_vMatrixR.push_back(trip);
	}
#if 0
	sprintf_s(szFileName, 256, "D:/testdata/ld/ToothRMat_%d.txt", m_nToothLabel);
	std::ofstream logF(szFileName);
	for (size_t i = 0; i < m_vMatrixR.size(); i++)
	{
		logF << m_vMatrixR[i].row << " " << m_vMatrixR[i].col << " " << m_vMatrixR[i].val << std::endl;
	}
	logF.close();
#endif
}

void VCToothObj::SolveEquation()
{
	//OBJWritePerfectMesh("D:/testdata/toothsolverbefore.obj", m_perfMesh);
#if 1
	unsigned nRow = m_perfMesh.GetVertexCount() + m_vFixedVerts.size() + m_vStrawHatSideVerts.size();
	unsigned nCol = m_perfMesh.GetVertexCount();
	NMALG_MATRIX::VSSparseMatrix vResult;
	VSSprsMatrixD sprsMatL, sprsMatR;
	sprsMatL.row = nRow;
	sprsMatL.col = nCol;
	sprsMatL.val = VD_V2CB(m_vMatrixL);
	sprsMatR.row = nRow;
	sprsMatR.col = 3;
	sprsMatR.val = VD_V2CB(m_vMatrixR);

	NMALG_MATRIX::MAT_SolveAnyMat(vResult, sprsMatL, sprsMatR);
	std::vector<VFVECTOR3> vCtrlPoints(m_vBtmCtrlPoints.size());
	unsigned nShift2 = m_vFixedVerts.size() + m_vStrawHatSideVerts.size() - m_vBtmCtrlPoints.size();
	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		vCtrlPoints[i] = m_vVertices[m_vBtmCtrlPoints[i]];
	}
	// 更新修补侧面和底面添加的顶点

	for (size_t i = 0; i < vResult.vValue.size(); i++)
	{
		unsigned nRow = vResult.vValue[i].row;
		unsigned nCol = vResult.vValue[i].col;
		if ((NMALG_TEETHPATCHING::CrownEdge == m_vVertType[nRow]) ||
			(NMALG_TEETHPATCHING::SideAdded == m_vVertType[nRow]) ||
			(NMALG_TEETHPATCHING::BtmAdded == m_vVertType[nRow]))
		{
			m_vVertices[nRow][nCol] = vResult.vValue[i].val;			
		}
	}
	
#else
#if 1
	unsigned nRow = m_perfMesh.GetVertexCount() + m_vFixedVerts.size() + m_vStrawHatSideVerts.size();
	unsigned nCol = m_perfMesh.GetVertexCount();
	Eigen::SparseMatrix<double> A1_sparse(nRow, nCol);
	Eigen::MatrixXd b1_sparse(nRow, 3);
	//Eigen::VectorXd b1_sparse(nRow, 3);
	//Eigen::VectorXd x1_sparse;
	Eigen::MatrixXd x1_sparse;
	std::vector<Eigen::Triplet<double>> tripletlist;
	for (unsigned i = 0; i < m_vMatrixL.size(); i++)
	{		
		//按Triplet方式填充，速度快
		tripletlist.push_back(Eigen::Triplet<double>(m_vMatrixL[i].row, m_vMatrixL[i].col, m_vMatrixL[i].val));

		// 直接插入速度慢
		//A1_sparse.insert(i, j) = A(i, j);
	}
	unsigned nShift1 = nRow;
	unsigned nShift2 = nShift1 + nRow;
	
	for (size_t i = 0; i < nRow; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			b1_sparse(i, j) = m_pMatrixR.get()[i + nRow * j];
		}
	}
	A1_sparse.setFromTriplets(tripletlist.begin(), tripletlist.end());

	// 压缩优化矩阵
	A1_sparse.makeCompressed();

	Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double> > Solver_sparse;

	// 设置迭代精度
	Solver_sparse.setTolerance(0.001);
	Solver_sparse.compute(A1_sparse);

	//x1_sparse 即为解
	x1_sparse = Solver_sparse.solve(b1_sparse);	
	nShift2 = m_vFixedVerts.size() + m_vStrawHatSideVerts.size() - m_vBtmCtrlPoints.size();
	// 保存底部控制点
	std::vector<VFVECTOR3> vCtrlPoints(m_vBtmCtrlPoints.size());
	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		vCtrlPoints[i] = m_vVertices[m_vBtmCtrlPoints[i]];
	}
	// 更新修补侧面和底面添加的顶点
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		unsigned nIdx = m_vStrawHatSideVerts[i];
		m_vVertices[nIdx].x = x1_sparse(nIdx, 0);
		m_vVertices[nIdx].y = x1_sparse(nIdx, 1);
		m_vVertices[nIdx].z = x1_sparse(nIdx, 2);
	}
	// 更新修补侧面和底面添加的顶点
	for (unsigned nIdx = nShift2; nIdx < nCol; nIdx++)
	{
		m_vVertices[nIdx].x = x1_sparse(nIdx, 0);
		m_vVertices[nIdx].y = x1_sparse(nIdx, 1);
		m_vVertices[nIdx].z = x1_sparse(nIdx, 2);
	}
#else
#if 1
	TVExtSource< VNALGEQ::VRDnsResolverD > extDnsResolver;
	TVR2B< VNALGEQ::VRDnsResolverD > dnsResolver(*extDnsResolver);
	VSEquaA<double>		mA;
	VSEquaB<double>		mB;
	VSDnsMatrixD		X;
	std::vector<double>					vLeft;
	mA.matx.row = m_perfMesh.GetVertexCount() + m_vFixedVerts.size() + m_vStrawHatSideVerts.size();
	mA.matx.col = m_perfMesh.GetVertexCount();
	vLeft.resize(mA.matx.row * mA.matx.col, 0.0);
	mA.matx.pData = &vLeft[0];

	mB.matx.row = mA.matx.row;
	mB.matx.col = 3;
	mB.matx.pData = m_pMatrixR.get();
	
	for (unsigned i = 0; i < m_vMatrixL.size(); i++)
	{
		vLeft[m_vMatrixL[i].row * mA.matx.col + m_vMatrixL[i].col] = m_vMatrixL[i].val;
	}
	dnsResolver.Build(X, mA, mB);
	unsigned nShift1 = m_perfMesh.GetVertexCount();
	unsigned nShift2 = nShift1 * 2;
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		unsigned nIdx = m_vStrawHatSideVerts[i];
		m_vVertices[nIdx].x = X.pData[nIdx];
		m_vVertices[nIdx].y = X.pData[nIdx + nShift1];
		m_vVertices[nIdx].z = X.pData[nIdx + nShift2];
	}
#else
	TVExtSource< VNALGEQ::VRSprsResolverD > extSprsResolver;
	TVExtSource< VNALGEQ::VRTriple2MatrixD > extTriple2Matrix;
	TVR2B< VNALGEQ::VRSprsResolverD > sprsResolver(*extSprsResolver);
	TVR2B< VNALGEQ::VRTriple2MatrixD > triple2Matrix(*extTriple2Matrix);
	VSSprsMatrixColMajor< double >	A;
	VSDnsMatData< double, true >	B;
	VSDnsMatData< double, true >	X;
	VSSprsMatData< double >	ATriple;
	
	
	ATriple.row = m_perfMesh.GetVertexCount() + m_vFixedVerts.size() + m_vStrawHatSideVerts.size();
	ATriple.col = m_perfMesh.GetVertexCount();
	ATriple.val = VD_V2CB(m_vMatrixL);

	B.row = ATriple.row;
	B.col = 3;
	B.pData = m_pMatrixR.get();

	A = triple2Matrix.Run(ATriple).Get<VSSprsMatrixColMajor< double > >();
	X = sprsResolver.Run(A, B).Get<VSDnsMatData< double, true > >();
	
	unsigned nShift1 = m_perfMesh.GetVertexCount();
	unsigned nShift2 = nShift1 * 2;
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		unsigned nIdx = m_vStrawHatSideVerts[i];
		m_vVertices[nIdx].x = X.pData[nIdx];
		m_vVertices[nIdx].y = X.pData[nIdx + nShift1];
		m_vVertices[nIdx].z = X.pData[nIdx + nShift2];
	}
#endif
	nShift2 = m_vFixedVerts.size() + m_vStrawHatSideVerts.size() - m_vBtmCtrlPoints.size();
	// 保存底部控制点
	std::vector<VFVECTOR3> vCtrlPoints(m_vBtmCtrlPoints.size());
	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		vCtrlPoints[i] = m_vVertices[m_vBtmCtrlPoints[i]];
	}
	// 更新修补侧面和底面添加的顶点
	for (unsigned nIdx = nShift2; nIdx < nShift1; nIdx++)
	{
		m_vVertices[nIdx].x = X.pData[nIdx];
		m_vVertices[nIdx].y = X.pData[nIdx + nShift1];
		m_vVertices[nIdx].z = X.pData[nIdx + nShift2];
	}
#endif
#endif
	// 恢复底部控制点	
	for (unsigned i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		m_vVertices[m_vBtmCtrlPoints[i]] = vCtrlPoints[i];
	}
	UpdatePerfMesh();	
}

void VCToothObj::SmoothGumLine(const double dbW)
{
	std::vector<VSTripletD> vMatrixL;
	MakeGumLineMatrix(vMatrixL, dbW);
#if 0
	unsigned nShift = m_vGumLine.size() * 2 * 3;
	double* pRight = new double[nShift];
	memset(pRight, 0, sizeof(double) * nShift);
	nShift = m_vGumLine.size() * 2;
	unsigned nShift1 = nShift * 2;
	for (size_t i = 0; i < m_vGumLine.size(); i++)
	{
		const VFVECTOR3& v = m_vVertices[m_vGumLine[i]];
		pRight[i] = v.x * dbW;
		pRight[i + nShift] = v.y * dbW;
		pRight[i + nShift1] = v.z * dbW;
	}

	TVExtSource< VNALGEQ::VRSprsResolverD > extSprsResolver;
	TVExtSource< VNALGEQ::VRTriple2MatrixD > extTriple2Matrix;
	TVR2B< VNALGEQ::VRSprsResolverD > sprsResolver(*extSprsResolver);
	TVR2B< VNALGEQ::VRTriple2MatrixD > triple2Matrix(*extTriple2Matrix);
	VSSprsMatrixColMajor< double >	A;
	VSDnsMatData< double, true >	B;
	VSDnsMatData< double, true >	X;
	VSSprsMatData< double >	ATriple;

	ATriple.row = nShift;
	ATriple.col = m_vGumLine.size();
	ATriple.val = VD_V2CB(vMatrixL);

	B.row = ATriple.row;
	B.col = 3;
	B.pData = pRight;

	A = triple2Matrix.Run(ATriple).Get<VSSprsMatrixColMajor< double > >();
	X = sprsResolver.Run(A, B).Get<VSDnsMatData< double, true > >();

	for (size_t i = 0; i < m_vGumLine.size(); i++)
	{
		unsigned nIdx = m_vGumLine[i];
		m_vVertices[nIdx].x = X.pData[nIdx];
		m_vVertices[nIdx].y = X.pData[nIdx + nShift];
		m_vVertices[nIdx].z = X.pData[nIdx + nShift1];
	}
#else
	unsigned nRow = m_vGumLine.size() * 2;
	unsigned nCol = m_vGumLine.size();
	NMALG_MATRIX::VSSparseMatrix vResult;
	std::vector<VSTripletD> vMatrixR;
	unsigned nShift = m_vGumLine.size() * 2;
	unsigned nShift1 = nShift * 2;
	for (size_t i = 0; i < m_vGumLine.size(); i++)
	{
		const VFVECTOR3& v = m_vVertices[m_vGumLine[i]];
		VSTripletD trip;
		trip.row = i;
		trip.col = 0;
		trip.val = v.x * dbW;
		vMatrixR.push_back(trip);
		trip.col = 1;
		trip.val = v.y * dbW;
		vMatrixR.push_back(trip);
		trip.col = 2;
		trip.val = v.z * dbW;
		vMatrixR.push_back(trip);
	}
	VSSprsMatrixD sprsMatL, sprsMatR;
	sprsMatL.row = nRow;
	sprsMatL.col = nCol;
	sprsMatL.val = VD_V2CB(vMatrixL);

	sprsMatR.row = nRow;
	sprsMatR.col = 3;
	sprsMatR.val = VD_V2CB(vMatrixR);
	NMALG_MATRIX::MAT_SolveAnyMat(vResult, sprsMatL, sprsMatR);
	
	for (size_t  i = 0; i <vResult.vValue.size(); i++)
	{
		unsigned nIdx = m_vGumLine[vResult.vValue[i].row];
		m_vVertices[nIdx][vResult.vValue[i].col] = vResult.vValue[i].val;
	}
	//for (size_t i = 0; i < m_vGumLine.size(); i++)
	//{
	//	unsigned nIdx = m_vGumLine[i];
	//	m_vVertices[nIdx].x = vResult[i * 3];
	//	m_vVertices[nIdx].y = vResult[i * 3 + 1];
	//	m_vVertices[nIdx].z = vResult[i * 3 + 2];
	//}
#endif
	
	UpdatePerfMesh();	
#if 0
	{
		char szFileName[256] = { 0 };
		static int nCount = 0;
		sprintf_s(szFileName, 256, "D:/testdata/smoothbtm_%d.obj", nCount++);		
		OBJWritePerfectMesh(szFileName, m_perfMesh);
	}
#endif
}

void VCToothObj::FillVertType()
{
	m_vVertType.resize(m_perfMesh.GetVertexCount(), NMALG_TEETHPATCHING::BtmAdded);
	for (size_t i = 0; i < m_vStrawHatSideVerts.size(); i++)
	{
		m_vVertType[m_vStrawHatSideVerts[i]] = NMALG_TEETHPATCHING::CrownEdge;
	}
	for (size_t i = 0; i < m_vCrownVerts.size(); i++)
	{
		m_vVertType[m_vCrownVerts[i]] = NMALG_TEETHPATCHING::ConstCrown;
	}
	for (size_t i = 0; i < m_vBtmCtrlPoints.size(); i++)
	{
		if (VD_INVALID_INDEX != m_vBtmCtrlPoints[i])
		{
			m_vVertType[m_vBtmCtrlPoints[i]] = NMALG_TEETHPATCHING::ConstRoot;
		}		
	}
	for (size_t i = 0; i < m_vSideAddPoints.size(); i++)
	{
		m_vVertType[m_vSideAddPoints[i]] = NMALG_TEETHPATCHING::SideAdded;
	}
}

void VCToothObj::MakeGumLineMatrix(std::vector<VSTripletD>& vMatrixL,const double dbW)
{
	unsigned nSize = m_vGumLine.size();	
	unsigned nShift = nSize;	
	unsigned nIdx = nShift;
	unsigned nRow = nShift;
	vMatrixL.resize(nSize * 4);
	vMatrixL[0].row = vMatrixL[0].col = 0;
	vMatrixL[0].val = dbW;
	vMatrixL[nIdx].row = nRow;
	vMatrixL[nIdx].col = 0;
	vMatrixL[nIdx].val = 1.0;
	vMatrixL[nIdx + 1].row = nRow;
	vMatrixL[nIdx + 1].col = nSize - 1;
	vMatrixL[nIdx + 1].val = -0.5;
	vMatrixL[nIdx + 2].row = nRow;
	vMatrixL[nIdx + 2].col = 1;
	vMatrixL[nIdx + 2].val = -0.5;
	for (size_t i = 1; i < nSize - 1; i++)
	{
		vMatrixL[i].row = vMatrixL[i].col = i;
		vMatrixL[i].val = dbW;
		nIdx = i * 3 + nShift;
		nRow = i + nShift;
		vMatrixL[nIdx].row = nRow;
		vMatrixL[nIdx].col = i;
		vMatrixL[nIdx].val = 1.0;
		vMatrixL[nIdx+1].row = nRow;
		vMatrixL[nIdx+1].col = i - 1;
		vMatrixL[nIdx+1].val = -0.5;
		vMatrixL[nIdx+2].row = nRow;
		vMatrixL[nIdx+2].col = i+1;
		vMatrixL[nIdx+2].val = -0.5;
	}
	nIdx = nSize - 1;
	vMatrixL[nIdx].row = vMatrixL[nIdx].col = nIdx;
	vMatrixL[nIdx].val = dbW;
	nIdx = nIdx * 3 + nShift;
	nRow = nSize - 1 + nShift;
	vMatrixL[nIdx].row = nRow;
	vMatrixL[nIdx].col = nSize-1;
	vMatrixL[nIdx].val = 1.0;
	vMatrixL[nIdx + 1].row = nRow;
	vMatrixL[nIdx + 1].col = nSize - 1;
	vMatrixL[nIdx + 1].val = -0.5;
	vMatrixL[nIdx + 2].row = nRow;
	vMatrixL[nIdx + 2].col = 0;
	vMatrixL[nIdx + 2].val = -0.5;
}

void VCToothObj::UpdatePerfMesh()
{
	VSSimpleMeshF simpMesh;
	simpMesh.pTriangles = &m_vSurfaces[0];
	simpMesh.pVertices = &m_vVertices[0];
	simpMesh.nVertCount = m_vVertices.size();
	simpMesh.nTriangleCount = m_vSurfaces.size();	
	m_ptrVRPerfMesh->Build(m_perfMesh, simpMesh);
	//OBJWriteSimpleMesh("D:/testdata/tooth.obj", simpMesh);
}