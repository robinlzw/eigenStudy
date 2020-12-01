#include "stdafx.h"
#include<iomanip>
#include <obj/lsalgtools/ilsalgtoolsperfmesh.h>
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/profile.h>
#include <obj/alg/ialgmatrix.h>
#include "lsalgteethprocess.h"
#include "vcfindkeypoints.h"
#include "vctoothmakematrixs.h"
#include "vcteethseparation.h"
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/profile.h>
#include "vctoothobj.h"
#include "vcgapobj.h"



//#define  DUMP_ENABLE
namespace NMALG_TEETHPATCHING
{	
	VBTeethPatching::VBTeethPatching(const unsigned guid)
		: m_guid(guid), m_pCollisionSenceAA(NULL), m_pCollisionSence(NULL)
	{
		m_nOutput = INIGetInt(L"autocut_output", L"./config.ini");
		m_ptrVRPerfMesh.reset(new TVR2B< VNALGMESH::VRPerfMesh >(*m_extVRPerfMesh));

		if(1 == m_nOutput)
		{
			std::ofstream txtF("D:/testdata/ld/profile.txt");
			txtF.close();
		}

	}



	VBTeethPatching::~VBTeethPatching()
	{
		m_ptrVRPerfMesh.reset();
		if (NULL != m_pCollisionSence)
			delete m_pCollisionSence;
		if (NULL != m_pCollisionSenceAA)
			delete m_pCollisionSenceAA;
	}


	void VBTeethPatching::Reset()
	{
		m_vKeyPoints.clear();
		m_vObjTooth.clear();
		m_vVerts.clear();
		m_vSurfs.clear();
		m_vToothVertInfo.clear();
		m_vvToothVOnJawMesh.clear();
		m_vSideCtrlPointPair.clear();
		m_vX0.clear();
		m_vVarVertOnJawMesh.clear();
		m_vFixVertOnJawMesh.clear();
		m_vVertMapKid.clear();
		m_vVertMapUid.clear();		
		m_vVertFlag.clear();
		m_vVertTypes.clear();
		m_nKidCount = 0;
		m_nUidCount = 0;
		m_Aieq.clear();
		m_AieqUid.clear();
		m_AieqKid.vValue.clear();
		m_matHKidUidTrans.vValue.clear();
		m_Bieq.clear();
		m_Aeq.clear();
		m_AeqUid.clear();
		m_AeqKid.vValue.clear();
		m_Beq.clear();
		m_teeth.clear();
		m_patchPairs.clear();
		m_teethInfo.clear();
		m_pairGapInfo.clear();
		m_teethData.clear();
		m_teethSurfs.clear();
#ifdef SPEED_DEBUG
 
#endif
	}



	void VBTeethPatching::Init(const VSSegmentedJaw& segmentedJaw)
	{
		Reset();	// 复位
		m_jawMesh = segmentedJaw.mshWhole;
		m_vObjTooth.resize(segmentedJaw.nToothCount);
		double dbAvgLen = PMGetAvgEdgeLen(segmentedJaw.mshWhole);
		for (size_t nToothIdx = 0; nToothIdx < segmentedJaw.nToothCount; nToothIdx++)
		{
			m_vObjTooth[nToothIdx].reset(new VCToothObj(nToothIdx, dbAvgLen, *this));
		}		
	}




	void VBTeethPatching::InitCollisionSence(const VSESS< VNCollisionTest::VRCollisionExt > & essCollisionExt)
	{
		m_pCollisionSence = new VBCollisionSence(essCollisionExt);
	}




	void VBTeethPatching::InitCollisionSenceAA(const VSESS< VNCollisionTest::VRMergeCollisionTest > & sysCollisionTest)
	{
		m_pCollisionSenceAA = new VBAACollisionSence(sysCollisionTest);
	}




	void VBTeethPatching::BuildTopoTeeth(VSTeethPatchingTopoResult & topoRslt, const VSTeethPatchingInput & topoInput)
	{
		VD_PROFILE_START(lsBuildTopoTeeth);
		//OBJWriteSimpleMesh("E:/testdata/topoInputMesh.obj", topoInput.jawMesh);
		m_segmentedJaw.mshWhole = topoInput.jawMesh;
		//m_ptrVRPerfMesh->Build(m_segmentedJaw.mshWhole, topoInput.jawMesh);
		m_segmentedJaw.nToothCount = topoInput.toothCount;
		m_segmentedJaw.cbVertexLable = topoInput.vertexLable;
		Init(m_segmentedJaw);

		// 1. Find key triangle(三个顶点不同的属性)
		auto AppendKeyPoint = [this](const unsigned nToothLabel, const VFVECTOR3& keyPoint) {
			m_vObjTooth[nToothLabel]->AppendKeyPoint(keyPoint);
		};
		//OBJWritePerfectMesh("D:/testdata/jawmesh.obj", segmentedJaw.mshWhole);
		::FindKeyPoints(m_vKeyPoints, m_segmentedJaw, AppendKeyPoint);

		// 2. Refine triangles(扩散每颗牙齿的三角片，使之成为单联通区域)	
		auto GetVertRef = [this](const unsigned nToothLabel) -> std::vector<VFVECTOR3>& {
			return m_vObjTooth[nToothLabel]->GetVertRef();
		};
		auto GetSurfRef = [this](const unsigned nToothLabel) -> std::vector<VNVECTOR3UI>& {
			return m_vObjTooth[nToothLabel]->GetSurfRef();
		};

		auto GetVertOnJawMeshRef = [this](const unsigned nToothLabel) -> std::vector<unsigned>& {
			return m_vObjTooth[nToothLabel]->GetVertOnJawMeshRef();
		};
		std::vector< unsigned > vTteethStartSurfIndx;
		_ConfirmStartSurface(vTteethStartSurfIndx, m_segmentedJaw.mshWhole, m_segmentedJaw.cbVertexLable, m_segmentedJaw.nToothCount);
		::SplitTooth(m_segmentedJaw, VD_V2CB(vTteethStartSurfIndx), GetVertRef, GetSurfRef, GetVertOnJawMeshRef);

		for (size_t nToothIdx = 0; nToothIdx < m_segmentedJaw.nToothCount; nToothIdx++)
		{
			if (false == m_vObjTooth[nToothIdx]->Init())
			{
				return;
			}
			//	3. 查找草帽边顶点并标记，以测地距离最大值的0.1位界限。
			m_vObjTooth[nToothIdx]->FindStrawHatSideVerts();

			// 4. Find the edge to be patch(查找侧面需要修补的边界段)。
			//	 1) 在边界上查找关键点，使该点到关键三角片中心欧氏距离最短。
			//	 2) 统计相邻关键点的顶点属性，即每个边界点领域顶点属性和，如果第二大属性和的属性是牙齿属性，
			//		则该牙齿与此属性牙齿相邻，并且顶点个数大于5，则该侧面需要修补，并记下相邻的牙齿属性。		

			m_vObjTooth[nToothIdx]->FindNbrBndryToPatch(m_segmentedJaw);
			// 5. 补侧面
			//	 1). 把侧面需要修补的边界段，拓扑位半圆；
			//   2). 使用triangle库对牙齿侧面进行补洞，并把修补部分的中心点作为侧面的控制点，并产生牙龈线，即牙齿修补侧面后的新的边界。
			//	 3). 把半圆的点映射到牙齿网格上(通过解下面方程组)；并标记距离侧面中心点最近的顶点索引。
			//		M*v=V; M = |1   0    0  ...| 边界点 V = |v| 边界点坐标
			//				   |1 -1/n -1/n ...| 內?      |0| 內?
			m_vObjTooth[nToothIdx]->PatchToothNbrSide();

			// 6. 补牙齿底面
			//	 1). 根据牙龈线把牙齿底面拓扑为一个圆。
			//	 2). 使用triangle库对牙齿底面进行补洞，并选择底部中心点的一阶领域点作为牙齿控制点。
			//	 3). 把圆上的点映射到牙齿网格上(通过解下面方程组)，
			//		M*v=V; M = |1   0    0  ...| 边界点 V = |v| 边界点坐标
			//				   |1 -1/n -1/n ...| 內?      |0| 內?		
			m_vObjTooth[nToothIdx]->PatchBottom();

			// 7. 延伸牙齿底部控制点
			//	 1). 计算底部控制点的法向量均值dir，此值为牙轴方向。
			//	 2). 把牙龈线上的点投影到牙轴dir上，取最小值为V0。
			//	 3). 把V0向牙轴反方向-dir延伸len毫米，得到V1，这里len = 1毫米，此参量为可修改参量。
			//	 4). 由V1，dir构成一个平面，把其他控制点投影到此平面，得到新的控制点。

			m_vObjTooth[nToothIdx]->ExtendBottomCtrlPoints();
			// 8. 构建每颗牙齿的拉普拉斯矩阵，L0为牙齿网格laplace矩阵与牙龈线laplace矩阵权重之和，
			//	  L1为牙齿上固定点toothfix和bottomfix, L2为牙齿草帽边内的点。L1和L2都为0，1矩阵。
			//	  权重w0=1, w1=1000, w2=10.
			//		L0 = |1 -1/n -1/n   0    0  ...|  +  10 * |1  -1/2 -1/2 0 ...|
			//			 |0   1   0   -1/n -1/n ...|		  |0    0    0    ...|
			//		L1 = |1 0 0 ...| 固定点位1，其余为0
			//			 |0 1 0 ...|
			//		L2 = |1 0 0 ...| 草帽边内点位1，其余为0
			//			 |0 1 0 ...|
			//		V1 = 固定点坐标，其余为0，
			//		V2 = 草帽边内点坐标，其余为0

			m_vObjTooth[nToothIdx]->MakeMatrixL();
			// 9. V1为牙齿上固定点toothfix和bottomfix的坐标值，V2位草帽边內点的位置。解方?
			//		Mv=V; M = |w0*L0|			|w0* 0|
			//				  |w1*L1|	* v =	|w1*V1|
			//				  |w2*L2|			|w2*V2|
			//		解出来的v位牙齿顶点位置。
			m_vObjTooth[nToothIdx]->MakeMatrixR();
			m_vObjTooth[nToothIdx]->SolveEquation();
			//{
			//	char szFileName[256] = { 0 };
			//	sprintf_s(szFileName, "D:/testdata/patchedtooth_%d.obj", nToothIdx);
			//	OBJWritePerfectMesh(szFileName, m_vObjTooth[nToothIdx]->m_perfMesh);
			//}
			m_vObjTooth[nToothIdx]->SmoothGumLine(0.1);			
			//{
			//	char szFileName[256] = { 0 };
			//	sprintf_s(szFileName, "D:/testdata/smoothtooth_%d.obj", nToothIdx);
			//	OBJWritePerfectMesh(szFileName, m_vObjTooth[nToothIdx]->m_perfMesh);
			//}
		}
		m_teeth.resize(m_vObjTooth.size());
		m_patchPairs.clear();
		for (size_t i = 0; i < m_teeth.size(); i++)
		{
			m_teeth[i].gumLineVerts = VD_V2CB(m_vObjTooth[i]->m_vGumLine);
			m_teeth[i].rootVerts = VD_V2CB(m_vObjTooth[i]->m_vBtmCtrlPoints);
			m_teeth[i].vertTypes = VD_V2CB(m_vObjTooth[i]->m_vVertType);
			m_teeth[i].crownVerts = VD_V2CB(m_vObjTooth[i]->m_vCrownVerts);
			m_teeth[i].topoToothMesh.nVertCount = m_vObjTooth[i]->m_perfMesh.GetVertexCount();
			m_teeth[i].topoToothMesh.nTriangleCount = m_vObjTooth[i]->m_perfMesh.GetSurfCount();
			m_teeth[i].topoToothMesh.pVertices = m_vObjTooth[i]->m_perfMesh.pVertices;
			m_teeth[i].topoToothMesh.pTriangles = m_vObjTooth[i]->m_perfMesh.pSurfaces;
			bool blUsed = false;
			for (size_t j = 0; j < m_vObjTooth[i]->m_vNbrPatchedVert.size(); j++)
			{
				unsigned nNbrTooth = m_vObjTooth[i]->m_vNbrPatchedVert[j].nNbrTooth;
				if (VD_INVALID_INDEX==nNbrTooth)
					continue;

				if (i < nNbrTooth)
				{
					VSPatchPair pair;
					pair.toothAIdx = i;
					pair.toothAConnectVertIdx = m_vObjTooth[i]->m_vNbrPatchedVert[j].ctrlVert;
					pair.toothBIdx = nNbrTooth;
					pair.toothBConnectVertIdx = VD_INVALID_INDEX;
					for (size_t k = 0; k < m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert.size(); k++)
					{
						if (i == m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert[k].nNbrTooth)
						{
							pair.toothBConnectVertIdx = m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert[k].ctrlVert;
						}
					}
					if ((VD_INVALID_INDEX != pair.toothAConnectVertIdx) && (VD_INVALID_INDEX != pair.toothBConnectVertIdx))
					{
						if (nNbrTooth = i + 1)
						{
							blUsed = true;
						}
						m_patchPairs.push_back(pair);
					}
				}
			}
			if ((false == blUsed) && (i < m_teeth.size() -1))
			{
				VSPatchPair pair;
				pair.toothAIdx = i;
				pair.toothBIdx = i+1;
				pair.toothAConnectVertIdx = VD_INVALID_INDEX;
				pair.toothBConnectVertIdx = VD_INVALID_INDEX;
				m_patchPairs.push_back(pair);
			}
		}
		topoRslt.teeth = VD_V2CB(m_teeth);
		topoRslt.patchPairs = VD_V2CB(m_patchPairs);
		//GetHardPalateInfo(topoRslt);
		topoRslt.meshHardPalate.nVertCount = 0;
		topoRslt.meshHardPalate.pTriangles = NULL;
		topoRslt.meshHardPalate.pVertices = NULL;
		topoRslt.meshHardPalate.nTriangleCount = 0;
		topoRslt.meshHPVertType.len = 0;
		topoRslt.meshHPVertType.pData = NULL;		
		VD_PROFILE_FINISH(lsBuildTopoTeeth);
	}
	



	void VBTeethPatching::BuildInitInfo(VSTeethPatchInfo& info, 
		const VSTeethPatchingTopoResult& topo, CRITICAL_SECTION* pCsOpenGl)
	{
		VD_PROFILE_START(lsBuildInitInfo);
		m_teethInfo.resize(topo.teeth.len);
		m_pairGapInfo.resize(topo.patchPairs.len);
 
		for (size_t nPairIdx = 0; nPairIdx < topo.patchPairs.len; nPairIdx++)
		{
			VBCollisionSence::VSCollisionSenceResult ret;
			unsigned nTooth1 = topo.patchPairs.pData[nPairIdx].toothAIdx;
			unsigned nTooth2 = topo.patchPairs.pData[nPairIdx].toothBIdx;
			if (NULL != pCsOpenGl)
				EnterCriticalSection(pCsOpenGl);
			m_pCollisionSence->Build(ret, topo.teeth.pData[nTooth1].topoToothMesh, topo.teeth.pData[nTooth2].topoToothMesh);
			if (NULL != pCsOpenGl)
				LeaveCriticalSection(pCsOpenGl);
			if (true == ret.blCollision)
			{
				m_pairGapInfo[nPairIdx] = -ret.flDeepestDis;
			}
			else
				m_pairGapInfo[nPairIdx] = ret.flDeepestDis;
		}
//#endif
		for (size_t nToothIdx = 0; nToothIdx < topo.teeth.len; nToothIdx++)
		{
			VFVECTOR3 dir = VFVECTOR3::ZERO;
			VFVECTOR3 orgVert = VFVECTOR3::ZERO;
			unsigned nVertCount = topo.teeth.pData[nToothIdx].topoToothMesh.nVertCount;
			std::vector<unsigned> vVertMap(nVertCount, 0);
			// 把底部控制点加入固定的数列，后面构造矩阵时用到。
			for (size_t i = 0; i < nVertCount; i++)
			{
				orgVert += topo.teeth.pData[nToothIdx].topoToothMesh.pVertices[i];
			}
			orgVert /= (float)(topo.teeth.pData[nToothIdx].topoToothMesh.nVertCount);
			VSPerfectMesh perfMesh;
			m_ptrVRPerfMesh->Build(perfMesh, topo.teeth.pData[nToothIdx].topoToothMesh);
#if 1
			for (size_t i = 0; i < topo.teeth.pData[nToothIdx].rootVerts.len; i++)
			{
				vVertMap[topo.teeth.pData[nToothIdx].rootVerts.pData[i]] = 1;
			}
#if 0
			for (size_t i = 0; i < topo.teeth.pData[nToothIdx].rootVerts.len; i++)
			{
				std::vector<unsigned> dom;
				unsigned nVIdx = topo.teeth.pData[nToothIdx].rootVerts.pData[i];
				PMGet1OrderDomain(dom, nVIdx, perfMesh);
				bool blVal = true;
				for (size_t j = 0; j < dom.size(); j++)
				{
					if (0 == vVertMap[dom[i]])
					{
						blVal = false;
						break;
					}
				}
				if (true == blVal)
				{
					dir = perfMesh.pVertNormals[nVIdx];
					float flVal = (perfMesh.pVertices[nVIdx] - orgVert).Dot(-dir);
					m_teethInfo[nToothIdx].rootBackDis = std::abs(flVal);
					break;
				}
			}
#else
			bool blDirValid = false;
			for (size_t i = 0; i < perfMesh.GetSurfCount(); i++)
			{
				const VNVECTOR3UI& f = perfMesh.pSurfaces[i];
				if ((1 == vVertMap[f.x]) && (1 == vVertMap[f.y]) && (1 == vVertMap[f.z]))
				{
					dir = perfMesh.pSurfNormals[i];
					float flVal = (perfMesh.pVertices[f.x] - orgVert).Dot(-dir);
					m_teethInfo[nToothIdx].rootBackDis = std::abs(flVal);
					blDirValid = true;
					break;
				}
			}
			if (false == blDirValid)
			{
				if (topo.teeth.pData[nToothIdx].rootVerts.len < 1)
				{
					dir = VFVECTOR3::AXIS_Z;
				}
				else if(topo.teeth.pData[nToothIdx].rootVerts.len < 3)
				{
					for (size_t i = 0; i < topo.teeth.pData[nToothIdx].rootVerts.len; i++)
					{
						dir += perfMesh.pVertNormals[topo.teeth.pData[nToothIdx].rootVerts.pData[i]];
					}
					dir /= topo.teeth.pData[nToothIdx].rootVerts.len;
					dir.Normalize();
				}				
				else
				{
					VFVECTOR3 v1 = perfMesh.pVertices[topo.teeth.pData[nToothIdx].rootVerts.pData[1]] - perfMesh.pVertices[topo.teeth.pData[nToothIdx].rootVerts.pData[0]];
					VFVECTOR3 v2 = perfMesh.pVertices[topo.teeth.pData[nToothIdx].rootVerts.pData[2]] - perfMesh.pVertices[topo.teeth.pData[nToothIdx].rootVerts.pData[0]];
					dir = v1.Cross(v2);
					dir.Normalize();
					if (dir.Dot(perfMesh.pVertNormals[topo.teeth.pData[nToothIdx].rootVerts.pData[0]]) < 0.0f)
						dir = -dir;
				}					
			}
#endif
#else		

			float flDiff = FLT_MAX;
			for (size_t i = 0; i < topo.teeth.pData[nToothIdx].rootVerts.len; i++)
			{
				dir += perfMesh.pVertNormals[topo.teeth.pData[nToothIdx].rootVerts.pData[i]];
			}
			dir /= (float)(topo.teeth.pData[nToothIdx].rootVerts.len);
			dir.Normalize();
			for (size_t i = 0; i < topo.teeth.pData[nToothIdx].rootVerts.len; i++)
			{
				float flVal = (perfMesh.pVertices[topo.teeth.pData[nToothIdx].rootVerts.pData[i]] - orgVert).Dot(-dir);
				if (flVal < flDiff)
				{
					m_teethInfo[nToothIdx].rootBackDis = std::abs(flVal);
				}
			}
#endif
			m_teethInfo[nToothIdx].toothCuspDir.SetOrigin(orgVert);
			m_teethInfo[nToothIdx].toothCuspDir.SetDirection(-dir);
		}
		info.teethInfo = VD_V2CB(m_teethInfo);
		info.pairGapInfo = VD_V2CB(m_pairGapInfo);
		VD_PROFILE_FINISH(lsBuildInitInfo);
	}

	
	
	
	void VBTeethPatching::BuildResult(VSTeethPatchingResult& rslt, const VSTeethPatchingTopoResult& topo,
		const VSTeethPatchInfo& info, CRITICAL_SECTION* pCsOpenGl)
	{		

		VD_PROFILE_START(BuildResult);

		Reset();				
		m_vObjTooth.resize(topo.teeth.len);
		for (unsigned nToothIdx = 0; nToothIdx < topo.teeth.len; nToothIdx++)
		{
			char szFileName[256] = { 0 };
#ifdef DUMP_ENABLE
			sprintf_s(szFileName, "E:/testdata/jawsimpmesh1_%d.obj", nToothIdx);
			OBJWriteSimpleMesh(szFileName, topo.teeth.pData[nToothIdx].topoToothMesh);
#endif
			m_vObjTooth[nToothIdx].reset(new VCToothObj(nToothIdx, 0.2, *this, topo.teeth.pData[nToothIdx]));
			m_vObjTooth[nToothIdx]->SetToothAxis(topo.teeth.pData[nToothIdx], info.teethInfo.pData[nToothIdx]);
			m_vObjTooth[nToothIdx]->MakeMatrixL();
			// 9. V1为牙齿上固定点toothfix和bottomfix的坐标值，V2位草帽边內点的位置。解方?
			//		Mv=V; M = |w0*L0|			|w0* 0|
			//				  |w1*L1|	* v =	|w1*V1|
			//				  |w2*L2|			|w2*V2|
			//		解出来的v位牙齿顶点位置。
			m_vObjTooth[nToothIdx]->MakeMatrixR();
			m_vObjTooth[nToothIdx]->SolveEquation();
			m_vObjTooth[nToothIdx]->SmoothGumLine(0.1);
			//sprintf_s(szFileName, "D:/testdata/ld/jawsimpmesh2_%d.obj", nToothIdx);
			//OBJWritePerfectMesh(szFileName, m_vObjTooth[nToothIdx]->m_perfMesh);
		}
		//MergePatchedTooth(topo);	
		MergePatchedToothEx(topo);
		
		//UpdateBtmCtrlPoints(topo, info);
        //VSSimpleMeshF simpMesh;
        //simpMesh.nTriangleCount = m_vSurfs.size();
        //simpMesh.nVertCount = m_vVerts.size();
        //simpMesh.pTriangles = &m_vSurfs[0];
        //simpMesh.pVertices = &m_vVerts[0];
        //OBJWriteSimpleMesh("D:/testdata/btmChanged.obj", simpMesh);

		MakeQuadProgramMatrixs();
		std::vector<VSGapPointPairs> vGapPointPairs;
		CalcCollisionPointPair(vGapPointPairs, topo, pCsOpenGl);
		unsigned nIterCount = 5;
		m_teethData.resize(m_vToothVertInfo.size());
		for (size_t i = 0; i < m_vToothVertInfo.size(); i++)
		{
			m_teethData[i].mesh = topo.teeth.pData[i].topoToothMesh;
			m_teethData[i].mesh.pVertices = &m_vVerts[m_vToothVertInfo[i].nIdxBase];
			m_teethData[i].gumLineVerts = topo.teeth.pData[i].gumLineVerts;
		}
#ifdef DUMP_ENABLE
		char szFileName[256] = { 0 };
		std::ofstream txtF("E:/testdata/log.txt", std::ios::app);
		txtF << "Entry loop:" << std::endl;
#endif
//#ifdef SPEED_DEBUG
		if (1 == m_nOutput)
		{
			std::ofstream txtF("E:/testdata/loopinfo.txt", std::ios::app);
			txtF << "Entry loop: " << std::endl;
			txtF.close();
		}
//#endif
		std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > > vCollisionOut;
		for (size_t iter = 0; iter < nIterCount; iter++)
		{				
			VD_PROFILE_START(OutLoop);
			MakeQuadProgramMatrixsAieq(VD_V2CB(vGapPointPairs));
			std::vector<VSTripletD> X;
			
			SolveQuadProgram(X, m_sprsMatHUidUid, m_sprsMatHKidUidTrans, m_sprsMatAeqUid,
				m_sprsMatAeqKid, m_sprsMatAieqUid, m_sprsMatAieqKid);
			
			//OBJWritePerfectMesh("D:/testdata/lastjawmesh0.obj", m_jawMeshPatched);

			UpdateJawMesh(VD_V2CB(X));
#ifdef DUMP_ENABLE
			sprintf_s(szFileName, "E:/testdata/jawMeshPatched_%d.obj", iter);
			OBJWritePerfectMesh(szFileName, m_jawMeshPatched);
#endif
			VD_PROFILE_START(CalcCollisionPointPair);
			CalcCollisionPointPair(vGapPointPairs, VD_V2CB(m_teethData), pCsOpenGl);
			VD_PROFILE_FINISH(CalcCollisionPointPair);
			double depth = 0.05;
			for (size_t i = 0; i < vGapPointPairs.size(); i++)
			{
				if (depth > vGapPointPairs[i].flDeepestDis)
				{					
					depth = vGapPointPairs[i].flDeepestDis;
					if (1 == m_nOutput)
					{
						std::ofstream txtF("E:/testdata/loopinfo.txt", std::ios::app);
						txtF << "i: " << i << " depth: " << depth << std::endl;
						txtF.close();
					}

				}
			}

#ifdef DUMP_ENABLE
			txtF << depth << std::endl;
#endif
//#ifdef SPEED_DEBUG
			if (1 == m_nOutput)
			{
				std::ofstream txtF("E:/testdata/loopinfo.txt", std::ios::app);
				txtF << "iter: " << iter << " min depth: " << depth << std::endl;
				txtF.close();
			}
//#endif
			
			VD_PROFILE_FINISH(OutLoop);
			
			if (depth > -0.05)
			{
#ifdef COLLISIONSENCEAA_ENABLE
				bool blBreak = true;
				if (NULL != m_pCollisionSenceAA)
				{
					m_pCollisionSenceAA->CollisionDetection(vCollisionOut, VD_V2CB(m_teethData));

					for (size_t i = 0; i < vCollisionOut.size(); i++)
					{
						if (vCollisionOut[i].first < -0.05f)
						{
							blBreak = false;
							vGapPointPairs[i].flDeepestDis = vCollisionOut[i].first;
							VSCollisionPointPair pointPair;							
							const VFVECTOR3& vA = vCollisionOut[i].second.first;
							const VFVECTOR3& vB = vCollisionOut[i].second.second;
							auto GetMinIdx = [](const VSSimpleMeshF& mesh, const VFVECTOR3& v)->unsigned {
								float flMin = 1000.0f;
								unsigned nMinIdx = VD_INVALID_INDEX;
								for (size_t nVIdx = 0; nVIdx < mesh.nVertCount; nVIdx++)
								{
									float flVal = (v -mesh.pVertices[nVIdx]).SqrMagnitude();
									if (flVal < flMin)
									{
										flMin = flVal;
										nMinIdx = nVIdx;
									}
								}
								return nMinIdx;
							};

							pointPair.pointPair.first = GetMinIdx(m_teethData[i].mesh, vA);
							pointPair.pointPair.second= GetMinIdx(m_teethData[i+1].mesh, vB);
							vGapPointPairs[i].gapPointPairs.second.push_back(pointPair);
						}
					}
				}
#endif
				if (true == blBreak)
				{
					break;
				}
			}
		}
#ifdef DUMP_ENABLE		
		txtF.close();
#endif
//#ifdef SPEED_DEBUG
		if (1 == m_nOutput)
		{
			std::ofstream txtF("E:/testdata/loopinfo.txt", std::ios::app);
			txtF << "Leaving loop: \n" << std::endl;
			txtF.close();
		}
//#endif
		rslt.teethData = VD_V2CB(m_teethData);		
		VD_PROFILE_FINISH(BuildResult);
	}




	void VBTeethPatching::Build(VSTeethPatchingResult& result, const VSSegmentedJaw& segmentedJaw)
	{
		Init(segmentedJaw);

		// 1. Find key triangle(三个顶点不同的属性)
		auto AppendKeyPoint = [this](const unsigned nToothLabel, const VFVECTOR3& keyPoint) {
			m_vObjTooth[nToothLabel]->AppendKeyPoint(keyPoint);
		};
		//OBJWritePerfectMesh("D:/testdata/jawmesh.obj", segmentedJaw.mshWhole);
		::FindKeyPoints(m_vKeyPoints, segmentedJaw, AppendKeyPoint);
#if 0
		{
			std::ofstream keyF("D:/testdata/keypoints.obj");
			for (unsigned i = 0; i < m_vKeyPoints.size(); i++)
			{
				keyF << "v " << m_vKeyPoints[i].x << " " << m_vKeyPoints[i].y << " " << m_vKeyPoints[i].z << std::endl;
			}
			keyF.close();
		}
#endif
		// 2. Refine triangles(扩散每颗牙齿的三角片，使之成为单联通区域)	
		auto GetVertRef = [this](const unsigned nToothLabel) -> std::vector<VFVECTOR3>& {
			return m_vObjTooth[nToothLabel]->GetVertRef();
		};
		auto GetSurfRef = [this](const unsigned nToothLabel) -> std::vector<VNVECTOR3UI>& {
			return m_vObjTooth[nToothLabel]->GetSurfRef();
		};

		auto GetVertOnJawMeshRef = [this](const unsigned nToothLabel) -> std::vector<unsigned>& {
			return m_vObjTooth[nToothLabel]->GetVertOnJawMeshRef();
		};
		std::vector< unsigned > vTteethStartSurfIndx;
		_ConfirmStartSurface(vTteethStartSurfIndx, segmentedJaw.mshWhole, segmentedJaw.cbVertexLable, segmentedJaw.nToothCount);
		::SplitTooth(segmentedJaw, VD_V2CB(vTteethStartSurfIndx), GetVertRef, GetSurfRef, GetVertOnJawMeshRef);

		for (size_t nToothIdx = 0; nToothIdx < segmentedJaw.nToothCount; nToothIdx++)
		{
			if (false == m_vObjTooth[nToothIdx]->Init())
			{
				return;
			}
			//	3. 查找草帽边顶点并标记，以测地距离最大值的0.1位界限。
			m_vObjTooth[nToothIdx]->FindStrawHatSideVerts();

			// 4. Find the edge to be patch(查找侧面需要修补的边界段)。
			//	 1) 在边界上查找关键点，使该点到关键三角片中心欧氏距离最短。
			//	 2) 统计相邻关键点的顶点属性，即每个边界点领域顶点属性和，如果第二大属性和的属性是牙齿属性，
			//		则该牙齿与此属性牙齿相邻，并且顶点个数大于5，则该侧面需要修补，并记下相邻的牙齿属性。		

			m_vObjTooth[nToothIdx]->FindNbrBndryToPatch(segmentedJaw);
			// 5. 补侧面
			//	 1). 把侧面需要修补的边界段，拓扑位半圆；
			//   2). 使用triangle库对牙齿侧面进行补洞，并把修补部分的中心点作为侧面的控制点，并产生牙龈线，即牙齿修补侧面后的新的边界。
			//	 3). 把半圆的点映射到牙齿网格上(通过解下面方程组)；并标记距离侧面中心点最近的顶点索引。
			//		M*v=V; M = |1   0    0  ...| 边界点 V = |v| 边界点坐标
			//				   |1 -1/n -1/n ...| 內?      |0| 內?
			m_vObjTooth[nToothIdx]->PatchToothNbrSide();

			// 6. 补牙齿底面
			//	 1). 根据牙龈线把牙齿底面拓扑为一个圆。
			//	 2). 使用triangle库对牙齿底面进行补洞，并选择底部中心点的一阶领域点作为牙齿控制点。
			//	 3). 把圆上的点映射到牙齿网格上(通过解下面方程组)，
			//		M*v=V; M = |1   0    0  ...| 边界点 V = |v| 边界点坐标
			//				   |1 -1/n -1/n ...| 內?      |0| 內?		
			m_vObjTooth[nToothIdx]->PatchBottom();

			// 7. 延伸牙齿底部控制点
			//	 1). 计算底部控制点的法向量均值dir，此值为牙轴方向。
			//	 2). 把牙龈线上的点投影到牙轴dir上，取最小值为V0。
			//	 3). 把V0向牙轴反方向-dir延伸len毫米，得到V1，这里len = 1毫米，此参量为可修改参量。
			//	 4). 由V1，dir构成一个平面，把其他控制点投影到此平面，得到新的控制点。

			m_vObjTooth[nToothIdx]->ExtendBottomCtrlPoints();
			// 8. 构建每颗牙齿的拉普拉斯矩阵，L0为牙齿网格laplace矩阵与牙龈线laplace矩阵权重之和，
			//	  L1为牙齿上固定点toothfix和bottomfix, L2为牙齿草帽边内的点。L1和L2都为0，1矩阵。
			//	  权重w0=1, w1=1000, w2=10.
			//		L0 = |1 -1/n -1/n   0    0  ...|  +  10 * |1  -1/2 -1/2 0 ...|
			//			 |0   1   0   -1/n -1/n ...|		  |0    0    0    ...|
			//		L1 = |1 0 0 ...| 固定点位1，其余为0
			//			 |0 1 0 ...|
			//		L2 = |1 0 0 ...| 草帽边内点位1，其余为0
			//			 |0 1 0 ...|
			//		V1 = 固定点坐标，其余为0，
			//		V2 = 草帽边内点坐标，其余为0

			m_vObjTooth[nToothIdx]->MakeMatrixL();
			// 9. V1为牙齿上固定点toothfix和bottomfix的坐标值，V2位草帽边內点的位置。解方?
			//		Mv=V; M = |w0*L0|			|w0* 0|
			//				  |w1*L1|	* v =	|w1*V1|
			//				  |w2*L2|			|w2*V2|
			//		解出来的v位牙齿顶点位置。
			m_vObjTooth[nToothIdx]->MakeMatrixR();
			m_vObjTooth[nToothIdx]->SolveEquation();
			m_vObjTooth[nToothIdx]->SmoothGumLine(0.1);
			//{
			//	char szFileName[256] = { 0 };
			//	sprintf_s(szFileName, "D:/testdata/patchedtooth_%d.obj", nToothIdx);
			//	OBJWritePerfectMesh(szFileName, m_vObjTooth[nToothIdx]->m_perfMesh);
			//}
		}

		MergePatchedTooth();

		m_teethData.resize(m_vObjTooth.size());
		for (size_t i = 0; i < m_vObjTooth.size(); i++)
		{
			//g_logFile << i << " vert count: " << m_vToothVertInfo[i].nVertCount << std::endl;
			//g_logFile << i << " base index: " << m_vToothVertInfo[i].nIdxBase << std::endl;
			m_teethData[i].mesh.nVertCount = m_vToothVertInfo[i].nVertCount;
			m_teethData[i].mesh.pVertices = &m_vVerts[m_vToothVertInfo[i].nIdxBase];
			m_teethData[i].mesh.nTriangleCount = m_vObjTooth[i]->m_perfMesh.GetSurfCount();
			m_teethData[i].mesh.pTriangles = m_vObjTooth[i]->m_perfMesh.pSurfaces;
			m_teethData[i].gumLineVerts = VD_V2CB(m_vObjTooth[i]->m_vGumLine);
		}

		std::vector<VSGapPointPairs> vGapPointPairs;
		CalcCollisionPointPair(vGapPointPairs);
#if 0
		unsigned nRayPairCount = 0;
		OBJWritePerfectMesh("D:/testdata/ld/jaw.obj", m_jawMeshPatched);
		std::ofstream txtF("D:/testdata/ld/kidvertices.txt");
		for (size_t i = 0; i < m_nKidCount; i++)
		{
			txtF << m_vFixVertOnJawMesh[i] + 1 << std::endl;
		}
		txtF.close();

		txtF.open("D:/testdata/ld/uidvertices.txt");
		for (size_t i = 0; i < m_nUidCount; i++)
		{
			txtF << m_vVarVertOnJawMesh[i] + 1<< std::endl;
		}
		txtF.close();

		txtF.open("D:/testdata/ld/raypair.txt");
		for (size_t i = 0; i < vGapPointPairs.size(); i++)
		{
			nRayPairCount = vGapPointPairs[i].gapPointPairs.second.size();		
			
			for (size_t j = 0; j < nRayPairCount; j++)
			{
				const std::pair<unsigned, unsigned>& pair = vGapPointPairs[i].gapPointPairs.second[j].pointPair;
				unsigned nCol1 = m_vvToothVOnJawMesh[vGapPointPairs[i].pairToothLabel.first][pair.first] + 1;
				unsigned nCol2 = m_vvToothVOnJawMesh[vGapPointPairs[i].pairToothLabel.second][pair.second] + 1;
				txtF << nCol1 << " " << nCol2 << std::endl;
			}
		}		
		txtF.close();

		txtF.open("D:/testdata/ld/ctrlpointpair.txt");
		for (size_t i = 0; i < m_vSideCtrlPointPair.size(); i++)
		{
			txtF << m_vSideCtrlPointPair[i].first + 1 << std::endl;
			txtF << m_vSideCtrlPointPair[i].second + 1 << std::endl;
		}
		txtF.close();
#endif
		unsigned nIterCount = 5;
		
		MakeQuadProgramMatrixs();
		for (size_t iter = 0; iter < nIterCount; iter++)
		{			
			std::vector<VSTripletD> X;
			VD_PROFILE_START(OutLoop);
			VD_PROFILE_START(MakeQuadProgramMatrixsAieq);
			MakeQuadProgramMatrixsAieq(VD_V2CB(vGapPointPairs));
			VD_PROFILE_FINISH(MakeQuadProgramMatrixsAieq);
			VD_PROFILE_START(SolveQuadProgram);
#if 0
			char szFileName[256] = { 0 };
			{
				sprintf_s(szFileName, 256, "D:/testdata/matrixA_%d.txt", iter + 1);
				std::ofstream txtA(szFileName);
				sprintf_s(szFileName, 256, "D:/testdata/matrixB_%d.txt", iter + 1);
				std::ofstream txtB(szFileName);
				sprintf_s(szFileName, 256, "D:/testdata/matrixAeq_%d.txt", iter + 1);
				std::ofstream txtAeq(szFileName);
				sprintf_s(szFileName, 256, "D:/testdata/matrixBeq_%d.txt", iter + 1);
				std::ofstream txtBeq(szFileName);
				sprintf_s(szFileName, 256, "D:/testdata/matrixAieq_%d.txt", iter + 1);
				std::ofstream txtAieq(szFileName);
				sprintf_s(szFileName, 256, "D:/testdata/matrixBieq_%d.txt", iter + 1);
				std::ofstream txtBieq(szFileName);
				txtA << "A:" << std::endl;
				PMDumpSparsMatrix(txtA, m_sprsMatHUidUid);
				txtA.close();
				txtB << "B:" << std::endl;
				PMDumpSparsMatrix(txtB, m_sprsMatHKidUidTrans);
				txtB.close();
				txtAeq << "Aeq:" << std::endl;
				PMDumpSparsMatrix(txtAeq, m_sprsMatAeqUid);
				txtAeq.close();
				txtBeq << "Beq:" << std::endl;
				PMDumpSparsMatrix(txtBeq, m_sprsMatAeqKid);
				txtBeq.close();
				txtAieq << "Aieq:" << std::endl;
				PMDumpSparsMatrix(txtAieq, m_sprsMatAieqUid);
				txtAieq.close();
				txtBieq << "Bieq:" << std::endl;
				PMDumpSparsMatrix(txtBieq, m_sprsMatAieqKid);
				txtBieq.close();
			}
#endif
			SolveQuadProgram(X, m_sprsMatHUidUid, m_sprsMatHKidUidTrans, m_sprsMatAeqUid,
				m_sprsMatAeqKid, m_sprsMatAieqUid, m_sprsMatAieqKid);
			VD_PROFILE_FINISH(SolveQuadProgram);
			VD_PROFILE_START(UpdateJawMesh);
#if 0
			{
				sprintf_s(szFileName, 256, "D:/testdata/matrixX_%d.txt", iter + 1);
				std::ofstream txtX(szFileName);
				txtX << "X:" << std::endl;
				for (size_t i = 0; i < X.size(); i++)
				{
					txtX << i << " " << X[i].val << std::endl;
				}
				txtX.close();
			}
#endif
			//OBJWritePerfectMesh("D:/testdata/lastjawmesh0.obj", m_jawMeshPatched);
#if 0
			{
				std::vector<VSToothInfo> vToothInfo(m_vObjTooth.size());
				std::vector<VSPatchedTooth> vPatchedTooth(m_vObjTooth.size());
				for (size_t i = 0; i < m_vObjTooth.size(); i++)
				{
					vPatchedTooth[i].mesh.nVertCount = m_vObjTooth[i]->m_perfMesh.GetVertexCount();
					vPatchedTooth[i].mesh.nTriangleCount = m_vObjTooth[i]->m_perfMesh.GetSurfCount();
					vPatchedTooth[i].mesh.pVertices = m_vObjTooth[i]->m_perfMesh.pVertices;
					vPatchedTooth[i].mesh.pTriangles = m_vObjTooth[i]->m_perfMesh.pSurfaces;
					vPatchedTooth[i].gumLineVerts = VD_V2CB(m_vObjTooth[i]->m_vGumLine);
					vToothInfo[i].rootBackDis = m_vObjTooth[i]->m_toothAxis.m_RootBackDis;
					vToothInfo[i].toothCuspDir = m_vObjTooth[i]->m_toothAxis.m_TipRay;
				}
				VSConstBuffer<VSToothInfo> cbToothInfo = VD_V2CB(vToothInfo);
				VSConstBuffer<VSPatchedTooth> cbPatchedTooth = VD_V2CB(vPatchedTooth);
				TVPersist<VSConstBuffer<VSPatchedTooth> > perstCbPatchedTooth;
				TVPersist<VSConstBuffer<VSToothInfo> > perstCbToothInfo;
				perstCbPatchedTooth.Build(cbPatchedTooth);
				perstCbToothInfo.Build(cbToothInfo);
				std::ofstream datFile("E:/tempdata/gumgenerator.dat", std::ofstream::binary);
				datFile << perstCbPatchedTooth << perstCbToothInfo;
				datFile.close();
		}
#endif
			UpdateJawMesh(VD_V2CB(X));
			{
				static int nCollisionCount = 0;
				nCollisionCount++;
				char szFileName[256] = { 0 };
				sprintf_s(szFileName, 256, "D:/testdata/collisionmesh%d.obj", nCollisionCount);
				OBJWritePerfectMesh(szFileName, m_jawMeshPatched);
			}
			VD_PROFILE_FINISH(UpdateJawMesh);
			VD_PROFILE_START(CalcCollisionPointPair);
			//for (size_t i = 0; i < m_teethData.size(); i++)
			//{
			//	char szFileName[256] = { 0 };
			//	sprintf_s(szFileName, 256, "D:/testdata/tooth%d.obj", i);
			//	OBJWriteSimpleMesh(szFileName, m_teethData[i].mesh);

			//}
			//CalcCollisionPointPair(vGapPointPairs, VD_V2CB(m_teethData));
			CalcCollisionPointPair(vGapPointPairs);
#if 0
			{
				nRayPairCount = 0;
				for (size_t i = 0; i < vGapPointPairs.size(); i++)
				{
					nRayPairCount += vGapPointPairs[i].gapPointPairs.second.size();
					g_logFile << "tooth1: " << vGapPointPairs[i].pairToothLabel.first << " tooth2: " << vGapPointPairs[i].pairToothLabel.second
						<< " count: " << vGapPointPairs[i].gapPointPairs.second.size() << std::endl;
					for (unsigned j = 0; j < vGapPointPairs[i].gapPointPairs.second.size(); j++)
					{
						g_logFile << vGapPointPairs[i].gapPointPairs.second[j].pointPair.first << " " << vGapPointPairs[i].gapPointPairs.second[j].pointPair.second << std::endl;
					}
					g_logFile << std::endl;
				}
			}
#endif
			double depth = DBL_MIN;
			for (size_t i = 0; i < vGapPointPairs.size(); i++)
			{
				if (depth < vGapPointPairs[i].flDeepestDis)
				{
					depth = vGapPointPairs[i].flDeepestDis;
				}
			}
			VD_PROFILE_FINISH(CalcCollisionPointPair);
//#ifdef SPEED_DEBUG
			if (1 == m_nOutput)
			{
				std::ofstream txtF("D:/testdata/ld/loopinfo.txt", std::ios::app);
				txtF << "iter: " << iter << " min depth: " << depth << std::endl;
				txtF.close();
			}
//#endif
			VD_PROFILE_FINISH(OutLoop);
			if (depth <= 0.05)
				break;
		}
		result.teethData = VD_V2CB(m_teethData);
	}
	



	void VBTeethPatching::CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs)
	{
		// 10. 对有补齐侧面的牙齿对进行碰撞检测，并返回n对点对。
		vGapPointPairs.clear();
#ifdef DUMP_ENABLE
		std::ofstream txtF("E:/testdata/collision1.txt");
#endif
		for (size_t nToothIdx = 0; nToothIdx < m_vObjTooth.size(); nToothIdx++)
		{
			for (size_t i = 0; i < m_vObjTooth[nToothIdx]->m_vNbrPatchedVert.size(); i++)
			{
				VSGapPointPairs gapPointPair;
				if (nToothIdx < m_vObjTooth[nToothIdx]->m_vNbrPatchedVert[i].nNbrTooth)
				{
					unsigned nToothIdx1 = nToothIdx;
					unsigned nToothIdx2 = m_vObjTooth[nToothIdx]->m_vNbrEdgeLine[i].nNbrTooth;
					VBCollisionSence::VSCollisionSenceResult ret;
					//gapPointPair.flDeepestDis = CollisionCheck(gapPointPair, nToothIdx, m_vObjTooth[nToothIdx]->m_vNbrEdgeLine[i].nNbrTooth);
					gapPointPair.flDeepestDis = CollisionCheck(ret, gapPointPair.gapPointPairs, m_teethData[nToothIdx1].mesh,
						m_teethData[nToothIdx2].mesh);
					gapPointPair.pairToothLabel.first = nToothIdx1;
					gapPointPair.pairToothLabel.second = nToothIdx2;
					vGapPointPairs.push_back(gapPointPair);
#ifdef DUMP_ENABLE
					txtF << "tooth1: " << nToothIdx1 << " tooth2: " << nToothIdx2 << std::endl;
					txtF << "isCollision: " << ret.blCollision << std::endl;
					txtF << "dist: " << ret.flDeepestDis << std::endl << std::endl;
#endif
				}
			}
		}
#ifdef DUMP_ENABLE
		txtF.close();
#endif
	}
	
	
	
	void VBTeethPatching::CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs,
		const VSTeethPatchingTopoResult& topoJaw, CRITICAL_SECTION* pCsOpenGl)
	{
		// 10. 对有补齐侧面的牙齿对进行碰撞检测，并返回n对点对。
		vGapPointPairs.resize(topoJaw.patchPairs.len);
#ifdef DUMP_ENABLE
		std::ofstream txtF("E:/testdata/collision2.txt");
#endif
		for (size_t i = 0; i < topoJaw.patchPairs.len; i++)
		{
			unsigned nTooth1 = topoJaw.patchPairs.pData[i].toothAIdx;
			unsigned nTooth2 = topoJaw.patchPairs.pData[i].toothBIdx;
			vGapPointPairs[i].pairToothLabel.first = nTooth1;
			vGapPointPairs[i].pairToothLabel.second = nTooth2;
			VBCollisionSence::VSCollisionSenceResult ret;
			vGapPointPairs[i].flDeepestDis = CollisionCheck(ret, vGapPointPairs[i].gapPointPairs, topoJaw.teeth.pData[nTooth1].topoToothMesh,
				topoJaw.teeth.pData[nTooth2].topoToothMesh);
#ifdef DUMP_ENABLE
			txtF << "tooth1: " << nTooth1 << " tooth2: " << nTooth2 << std::endl;
			txtF << "isCollision: " << ret.blCollision << std::endl;
			txtF << "dist: " << ret.flDeepestDis << std::endl << std::endl;
#endif
		}	
#ifdef DUMP_ENABLE
		txtF.close();
#endif
	}
	
	
	
	void VBTeethPatching::CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs, 
		const VSConstBuffer<VSPatchedTooth>& cbPatchTooth, CRITICAL_SECTION* pCsOpenGl)
	{		
#ifdef DUMP_ENABLE
		std::ofstream txtF("E:/testdata/collision3.txt");
#endif
		for (size_t i = 0; i < vGapPointPairs.size(); i++)
		{
			unsigned nTooth1 = vGapPointPairs[i].pairToothLabel.first;
			unsigned nTooth2 = vGapPointPairs[i].pairToothLabel.second;		
			VBCollisionSence::VSCollisionSenceResult ret;
			vGapPointPairs[i].flDeepestDis = CollisionCheck(ret, vGapPointPairs[i].gapPointPairs, cbPatchTooth.pData[nTooth1].mesh,
				cbPatchTooth.pData[nTooth2].mesh);
#ifdef DUMP_ENABLE
			txtF << "tooth1: " << nTooth1 << " tooth2: " << nTooth2 << std::endl;
			txtF << "isCollision: " << ret.blCollision << std::endl;
			txtF << "dist: " << ret.flDeepestDis << std::endl << std::endl;
#endif
		}
#ifdef DUMP_ENABLE
		txtF.close();
#endif
	}




	void VBTeethPatching::_ConfirmStartSurface(std::vector< unsigned > & vsurf, 
		const VSPerfectMesh & mesh, const VSConstBuffer< unsigned > & orgLable, unsigned TC) const
	{
		unsigned SC = mesh.GetSurfCount();

		std::vector< unsigned > vSurfScore(SC);
		std::vector< unsigned > stkSearch, stkBuff;
		memset(&vSurfScore.front(), 0, SC * sizeof(unsigned));

		for (unsigned i = 0; i < SC; i++)
		{
			auto & s = mesh.pSurfaces[i];
			if (orgLable.pData[s[0]] == VD_INVALID_INDEX
				&& orgLable.pData[s[1]] == VD_INVALID_INDEX
				&& orgLable.pData[s[2]] == VD_INVALID_INDEX)
			{
				vSurfScore[i] = 1;
				stkSearch.push_back(i);
			}
		}

		unsigned nCurScore(1);
		while (!stkSearch.empty())
		{
			nCurScore++;

			for (auto sid : stkSearch)
			{
				auto eos = mesh.GetEdgeIndxOfSurf(sid);
				auto eis = mesh.GetEdgeNbrIndxOfSurf(sid);

				for (size_t j = 0; j < 3; j++)
				{
					auto soe = mesh.GetSurfIndxOfEdge(eos[j]);
					unsigned ni = ((eis[j] + 1) & 0x01);
					auto sn = soe[ni];
					if (sn != VD_INVALID_INDEX && vSurfScore[sn] == 0)
					{
						vSurfScore[sn] = nCurScore;
						stkBuff.push_back(sn);
					}
				}
			}

			stkSearch.swap(stkBuff);
			stkBuff.clear();
		}

		vsurf.resize(TC);
		std::vector< unsigned > vRsltScore(TC, 0);

		for (unsigned i = 0; i < SC; i++)
		{
			auto & s = mesh.pSurfaces[i];
			auto tid = orgLable.pData[s[0]];
			if (tid < TC && tid == orgLable.pData[s[1]] && tid == orgLable.pData[s[2]])
			{
				if (vRsltScore[tid] < vSurfScore[i])
				{
					vRsltScore[tid] = vSurfScore[i];
					vsurf[tid] = i;
				}
			}
		}
	}




	void VBTeethPatching::MakeQuadProgramMatrixs()
	{
		// 构造Aeq矩阵		
		std::vector<VSTripletD> vAeqKid;
		VSSprsMatrixD sprsMatAeqKid;
		
		MakeMatrixAeq();
		MakeMatrixAeqUidAndKid(vAeqKid);
		MakeMatrixBeq();
		
		sprsMatAeqKid.row = m_sprsMatAeqUid.row;
		sprsMatAeqKid.col = m_nKidCount * 3;
		sprsMatAeqKid.val = VD_V2CB(vAeqKid);

		// 构造H矩阵
		NMALG_MATRIX::VSSparseMatrix L2;
		std::vector<VSTripletD> matHKidUidTranspose;
		MakeMatrixL2(L2);

		MakeHUidUid(VD_V2CB(L2.vValue));
		MakeMatrixX0();

		// HKidUidTranspose * X0
		MakeHKidUidTranspose(matHKidUidTranspose, VD_V2CB(L2.vValue));
		{
			std::vector<unsigned> vFlag(m_nUidCount * 3, 0);
			unsigned nNonZeroRowCnt = 0;
			for (unsigned i = 0; i < matHKidUidTranspose.size(); i++)
			{
				if (vFlag[matHKidUidTranspose[i].row] == 0)
				{
					vFlag[matHKidUidTranspose[i].row] = 1;
					nNonZeroRowCnt++;
				}
			}
		}
		VSSprsMatrixD sprsMatKidUidTrans;
		sprsMatKidUidTrans.row = m_nUidCount * 3;
		sprsMatKidUidTrans.col = m_nKidCount * 3;
		sprsMatKidUidTrans.val = VD_V2CB(matHKidUidTranspose);
		
#if 1
		NMALG_MATRIX::MAT_AMultiB(m_matHKidUidTrans, sprsMatKidUidTrans, m_sprsMatX0);
		{
#else		
		{
			NMALG_MATRIX::VSSparseMatrix matHKidUidTrans;
			NMALG_MATRIX::MAT_AMultiB(matHKidUidTrans, sprsMatKidUidTrans, m_sprsMatX0);
			std::vector<VSTripletD> vMatUidNorms;			
			VSSprsMatrixD			sprsMatUidNorms;
			vMatUidNorms.resize(m_nUidCount * 3);
#ifdef DUMP_ENABLE
			std::ofstream objF("E:/testdata/jawmeshpatched.obj");
			for (size_t m = 0; m < m_jawMeshPatched.GetVertexCount(); m++)
			{
				const VFVECTOR3& v = m_jawMeshPatched.pVertices[m];
				objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
			}
#endif
			for (size_t i = 0; i < m_nUidCount; i++)
			{
				const VFVECTOR3& norm = m_jawMeshPatched.pVertNormals[m_vVarVertOnJawMesh[i]];
				VFRay ray(m_jawMeshPatched.pVertices[m_vVarVertOnJawMesh[i]], norm);
#ifdef DUMP_ENABLE
				for (float flLen = 0.2f; flLen < 1.6f; flLen += 0.2f)
				{
					VFVECTOR3 v = ray.GetPoint(flLen);
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
#endif
				for (size_t j = 0; j < 3; j++)
				{
					unsigned nIdx = m_nUidCount * j + i;
					vMatUidNorms[nIdx].row = m_nUidCount * j + i;
					vMatUidNorms[nIdx].col = 0;
					vMatUidNorms[nIdx].val = -0.01f * norm[j];
				}				
			}
#ifdef DUMP_ENABLE
			for (size_t m = 0; m < m_jawMeshPatched.GetSurfCount(); m++)
			{
				const VNVECTOR3UI& v = m_jawMeshPatched.pSurfaces[m];
				objF << "f " << v.x + 1 << " " << v.y + 1 << " " << v.z + 1 << std::endl;
			}

			objF.close();
#endif
			sprsMatUidNorms.row = vMatUidNorms.size();
			sprsMatUidNorms.col = 1;
			sprsMatUidNorms.val = VD_V2CB(vMatUidNorms);

			VSSprsMatrixD			sprsMatUidTrans;
			sprsMatKidUidTrans.row = matHKidUidTrans.nRow;
			sprsMatKidUidTrans.col = matHKidUidTrans.nCol;
			sprsMatKidUidTrans.val = VD_V2CB(matHKidUidTrans.vValue);
			NMALG_MATRIX::MAT_AAddB(m_matHKidUidTrans, sprsMatKidUidTrans, sprsMatUidNorms);
#endif
			std::vector<unsigned> vFlag(m_nUidCount * 3, 0);
			unsigned nNonZeroRowCnt = 0;
			for (unsigned i = 0; i < m_matHKidUidTrans.vValue.size(); i++)
			{
				if (vFlag[m_matHKidUidTrans.vValue[i].row] == 0)
				{
					vFlag[m_matHKidUidTrans.vValue[i].row] = 1;
					nNonZeroRowCnt++;
				}
			}
		}
		//H(kid,uid)'*Beq0
		m_sprsMatHKidUidTrans.row = m_nUidCount * 3;
		m_sprsMatHKidUidTrans.col = 1;
		m_sprsMatHKidUidTrans.val = VD_V2CB(m_matHKidUidTrans.vValue);

		NMALG_MATRIX::VSSparseMatrix vMatAeqKidTmp;
		NMALG_MATRIX::MAT_AMultiB(vMatAeqKidTmp, sprsMatAeqKid, m_sprsMatX0);

		VSSprsMatrixD sprsMatAeqKidTmp;
		sprsMatAeqKidTmp.row = sprsMatAeqKid.row;
		sprsMatAeqKidTmp.col = m_sprsMatX0.col;
		sprsMatAeqKidTmp.val = VD_V2CB(vMatAeqKidTmp.vValue);
						
		NMALG_MATRIX::MAT_AMinusB(m_AeqKid, m_sprsMatBeq, sprsMatAeqKidTmp);

		m_sprsMatAeqKid.row = m_sprsMatBeq.row;
		m_sprsMatAeqKid.col = m_sprsMatBeq.col;
		m_sprsMatAeqKid.val = VD_V2CB(m_AeqKid.vValue);
	}




	void VBTeethPatching::MakeQuadProgramMatrixsAieq(const VSConstBuffer<VSGapPointPairs>& cbGapPointPairs)
	{
		// 构造Aieq矩阵
		std::vector<VSTripletD> vAieqKid;
		VSSprsMatrixD sprsMatAieqKid;

		MakeMatrixAieq(cbGapPointPairs);				
		MakeMatrixAieqUidAndKid(vAieqKid);		
		MakeMatrixBieq();
		
		sprsMatAieqKid.row = m_sprsMatAieqUid.row;
		sprsMatAieqKid.col = m_nKidCount * 3;
		sprsMatAieqKid.val = VD_V2CB(vAieqKid);		
		
		NMALG_MATRIX::VSSparseMatrix vMatAieqKidTmp;		
		NMALG_MATRIX::MAT_AMultiB(vMatAieqKidTmp, sprsMatAieqKid, m_sprsMatX0);

		VSSprsMatrixD sprsMatAieqKidTmp;
		
		sprsMatAieqKidTmp.row = sprsMatAieqKid.row;
		sprsMatAieqKidTmp.col = m_sprsMatX0.col;
		sprsMatAieqKidTmp.val = VD_V2CB(vMatAieqKidTmp.vValue);
				
		NMALG_MATRIX::MAT_AMinusB(m_AieqKid, m_sprsMatBieq, sprsMatAieqKidTmp);
				
		m_sprsMatAieqKid.row = m_sprsMatBieq.row;
		m_sprsMatAieqKid.col = m_sprsMatBieq.col;
		m_sprsMatAieqKid.val = VD_V2CB(m_AieqKid.vValue);
	}
	
	
	
	
	void VBTeethPatching::MakeMatrixX0()
	{
		m_vX0.resize(m_nKidCount * 3);
		unsigned nIdx = 0;
		for (unsigned j = 0; j < m_nKidCount; j++)
		{
			const VFVECTOR3& v = m_jawMeshPatched.pVertices[m_vFixVertOnJawMesh[j]];
			for (size_t i = 0; i < 3; i++)
			{
				unsigned nShift = i * m_nKidCount;
				m_vX0[nIdx].row = m_vVertMapKid[m_vFixVertOnJawMesh[j]] + nShift;
				m_vX0[nIdx].col = 0;
				m_vX0[nIdx++].val = v[i];
			}
		}
		m_sprsMatX0.row = m_vX0.size();
		m_sprsMatX0.col = 1;
		m_sprsMatX0.val = VD_V2CB(m_vX0);
	}





	void VBTeethPatching::MakeMatrixL2(NMALG_MATRIX::VSSparseMatrix& L2)
	{
		unsigned nRow = m_jawMeshPatched.GetVertexCount();
		std::vector<VSTripletD> L;
		std::vector<VSTripletD> M;
		NMALG_MATRIX::VSSparseMatrix tmp;
		VSSprsMatrixD sprsMatL, sprsMatM, sprsMatTmp;
		MakeMatrixL(L);
		MakeMatrixM(M);
		sprsMatL.row = nRow;
		sprsMatL.col = nRow;
		sprsMatL.val = VD_V2CB(L);

		sprsMatM.row = nRow;
		sprsMatM.col = nRow;
		sprsMatM.val = VD_V2CB(M);

		//NMALG_MATRIX::VSSparseMatrix matMInv;
		//NMALG_MATRIX::MAT_Inverse(matMInv, sprsMatM);
		//VSSprsMatrixD sprsMatMInv;
		//sprsMatMInv.row = matMInv.nRow;
		//sprsMatMInv.col = matMInv.nCol;
		//sprsMatMInv.val = VD_V2CB(matMInv.vValue);
		//NMALG_MATRIX::MAT_AMultiDiagMat(tmp, sprsMatL, sprsMatM);
		NMALG_MATRIX::MAT_AMultiB(tmp, sprsMatL, sprsMatM);
		sprsMatTmp.row = nRow;
		sprsMatTmp.col = nRow;
		sprsMatTmp.val = VD_V2CB(tmp.vValue);
		//std::ofstream logF("D:/testdata/LXM.txt");
		//logF << nRow << " " << nRow << std::endl;
		//PMDumpSparsMatrix(logF, sprsMatTmp);
		NMALG_MATRIX::MAT_AMultiB(L2, sprsMatTmp, sprsMatL);
#if 0
		std::ofstream logF("D:/testdata/ld/L2Mat.txt");
		for (size_t i = 0; i < L2.vValue.size(); i++)
		{
			logF << L2.vValue[i].row << " " << L2.vValue[i].col << " " << L2.vValue[i].val << std::endl;
		}
		logF.close();
#endif
	}





	void VBTeethPatching::MakeMatrixL(std::vector<VSTripletD >& L)
	{
		// 12. 带碰撞约束的牙齿网格调整
		//		1) 构造矩阵L：
		//			l(i,j) = cot(angle1) + cot(angle2)
		//			l(i,i) = -(l(i,j) + l(i, k) + l(i, m) ...)		
		//			cot(angle1),cot(angle2)为边i,j的相邻三角片对面顶点的夹角的余切
		//			行数=列数=nv*3
		//OBJWritePerfectMesh("D:/testdata/ld/jawmesh.obj", m_jawMeshPatched);
		::MakeMatrixL(L, m_jawMeshPatched);
	}





	void VBTeethPatching::MakeMatrixM(std::vector<VSTripletD >& M)
	{
		// 12. 带碰撞约束的牙齿网格调整
		//		2) 构造矩阵M：
		//			m(i,i) = (s1+s2+...)/3; s1,s2...为顶点i的一阶领域三角片面积。
		//			行数=列数=nv*3
		::MakeMatrixM(M, m_jawMeshPatched);
	}





	void VBTeethPatching::MakeMatrixAeq()
	{
		unsigned nRShift = m_vSideCtrlPointPair.size();
		unsigned nCShift = m_jawMeshPatched.GetVertexCount();
		m_Aeq.resize(nRShift * 2 * 3);
		unsigned nIdx = 0;
		for (size_t k = 0; k < 3; k++)
		{
			unsigned nShift = nCShift * k;
			for (size_t i = 0; i < m_vSideCtrlPointPair.size(); i++)
			{						
				unsigned nRow = i + nRShift * k;				
				m_Aeq[nIdx].row = nRow;
				m_Aeq[nIdx].col = m_vSideCtrlPointPair[i].first + nShift;
				m_Aeq[nIdx++].val = 1.0;
				m_Aeq[nIdx].row = nRow;
				m_Aeq[nIdx].col = m_vSideCtrlPointPair[i].second + nShift;
				m_Aeq[nIdx++].val = -1.0;				
			}
		}
		m_sprsMatAeq.row = nRShift * 3;
		m_sprsMatAeq.col = nCShift * 3;
		m_sprsMatAeq.val = VD_V2CB(m_Aeq);
	}





	void VBTeethPatching::_MakeMatrixKidAndVid(std::vector<VSTripletD>& vMatKid,
		std::vector<VSTripletD>& vMatUid, VSConstBuffer<VSTripletD>& cbMat)
	{
		vMatKid.clear();
		vMatUid.clear();
		VSTripletD trip;
		int nVertCnt = m_jawMeshPatched.GetVertexCount();
		for (size_t i = 0; i < cbMat.len; i++)
		{
			trip.row = cbMat.pData[i].row;
			trip.val = cbMat.pData[i].val;
			
			unsigned nCol = cbMat.pData[i].col % nVertCnt;
			if (1 == m_vVertFlag[nCol])
			{	
				unsigned nShift = m_nKidCount * (cbMat.pData[i].col / nVertCnt);
				trip.col = m_vVertMapKid[nCol] + nShift;
				vMatKid.push_back(trip);
			}
			else
			{
				unsigned nShift = m_nUidCount * (cbMat.pData[i].col / nVertCnt);
				trip.col = m_vVertMapUid[nCol] + nShift;
				vMatUid.push_back(trip);
			}
		}
	}
	
	
	
	
	void VBTeethPatching::MakeMatrixAeqUidAndKid(std::vector<VSTripletD>& vAeqKid)
	{
		_MakeMatrixKidAndVid(vAeqKid, m_AeqUid, VD_V2CB(m_Aeq));
		
		m_sprsMatAeqUid.row = m_sprsMatAeq.row;		
		m_sprsMatAeqUid.col = m_nUidCount * 3;		
		m_sprsMatAeqUid.val = VD_V2CB(m_AeqUid);
	}

	void VBTeethPatching::MakeMatrixAieq(const VSConstBuffer<VSGapPointPairs>& cbGapPointPairs)
	{
		// 11. 根据n对点对，构造碰撞约束矩阵		
		/*
		碰撞点对 p1，p2；p1，p2为牙颌网格顶点索引，Aieq矩阵的行数等于拥有两个碰撞点的射线条数，
		列数等于牙颌网格顶点个数*3(x,y,z三个坐标轴，所以要乘以3)，dir(dx, dy, dz)为碰撞检测的方向
		矩阵的其中一行如下：
		列数	0 1 ... p1 ... p2 ...n-1 n n+1 ... n+p1 ... n+p2 ... 2n-1 2n ... 2n+p1 ... 2n+p2 ... 3n-1
			   |0 0 ... dx ...-dx ... 0  0  0  ...  dy  ... -dy  ... 0     0 ...  dz   ... -dz   ...  0   |
		*/
		m_Aieq.clear();
		unsigned nShift = m_jawMeshPatched.GetVertexCount();
		unsigned nShift1 = nShift * 2;
		unsigned nRowSize = 0;
#ifdef DUMP_ENABLE
		char szFileName[256] = { 0 };
		std::ofstream txtF("E:/testdata/aieq.txt");
		
		txtF << "point pair count: " << cbGapPointPairs.len << std::endl;
#endif
		unsigned nColSize = nShift * 3;
		
		for (size_t i = 0; i < cbGapPointPairs.len; i++)
		{
			const VFVECTOR3& dir = cbGapPointPairs.pData[i].gapPointPairs.first;		
#ifdef DUMP_ENABLE
			txtF << "No." << i << " dir: " << dir.x << " " << dir.y << " " << dir.z << std::endl;
			sprintf_s(szFileName, 256, "E:/testdata/aieq_%d.obj", i);
			std::ofstream objF(szFileName);
#endif
			for (size_t j = 0; j < cbGapPointPairs.pData[i].gapPointPairs.second.size(); j++)
			{
				const std::pair<unsigned, unsigned>& pair = cbGapPointPairs.pData[i].gapPointPairs.second[j].pointPair;				
				unsigned nCol1 = m_vvToothVOnJawMesh[cbGapPointPairs.pData[i].pairToothLabel.first][pair.first];
				unsigned nCol2 = m_vvToothVOnJawMesh[cbGapPointPairs.pData[i].pairToothLabel.second][pair.second];
#ifdef DUMP_ENABLE
				{
					VFVECTOR3 v = m_jawMeshPatched.pVertices[nCol1];
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
					v = m_jawMeshPatched.pVertices[nCol2];
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
#endif
				for (size_t k = 0; k < 3; k++)
				{
					VSTripletD trip;
					trip.row = nRowSize;
					trip.col = nCol1 + k * nShift;
					trip.val = dir[k];
					m_Aieq.push_back(trip);
#ifdef DUMP_ENABLE
					txtF << trip.row << " " << trip.col << " " << trip.val << std::endl;
#endif
					trip.col = nCol2 + k * nShift;
					trip.val = -dir[k];
					m_Aieq.push_back(trip);
#ifdef DUMP_ENABLE
					txtF << trip.row << " " << trip.col << " " << trip.val << std::endl;
#endif
				}
				nRowSize++;
			}
#ifdef DUMP_ENABLE
			objF.close();
#endif
		}
#ifdef DUMP_ENABLE
		txtF.close();
#endif
		m_sprsMatAieq.row = nRowSize;
		m_sprsMatAieq.col = nColSize;
		m_sprsMatAieq.val = VD_V2CB(m_Aieq);
	}

	void VBTeethPatching::MakeMatrixAieqUidAndKid(std::vector<VSTripletD>& vAieqKid)
	{
		_MakeMatrixKidAndVid(vAieqKid, m_AieqUid, VD_V2CB(m_Aieq));
		
		m_sprsMatAieqUid.row = m_sprsMatAieq.row;		
		m_sprsMatAieqUid.col = m_nUidCount * 3;		
		m_sprsMatAieqUid.val = VD_V2CB(m_AieqUid);
	}

	void VBTeethPatching::MakeMatrixBeq()
	{
		m_Beq.resize(m_sprsMatAeq.row);
		for (unsigned i = 0; i < m_sprsMatAeq.row; i++)
		{
			m_Beq[i].row = i;
			m_Beq[i].col = 0;
			m_Beq[i].val = 0.0;
		}
		m_sprsMatBeq.row = m_Beq.size();
		m_sprsMatBeq.col = 1;
		m_sprsMatBeq.val = VD_V2CB(m_Beq);
	}

	void VBTeethPatching::MakeMatrixBieq()
	{
		m_Bieq.resize(m_sprsMatAieq.row);
		for (unsigned i = 0; i < m_sprsMatAieq.row; i++)
		{
			m_Bieq[i].row = i;
			m_Bieq[i].col = 0;
			m_Bieq[i].val = 0.0;
		}
		m_sprsMatBieq.row = m_Bieq.size();
		m_sprsMatBieq.col = 1;
		m_sprsMatBieq.val = VD_V2CB(m_Bieq);
	}
	/*
	while true
		fprintf('#iteration %d.\n', iter);

		Aeq_tmp = [Aeq; Aieq(as_ieq,:)]; % Aieq的as_ieq行组成矩阵，放到Aeq下面
		Beq_tmp = [Beq; Bieq(as_ieq)];

		[X, lambda] = quad_prog_fixed(A, B, Aeq_tmp, Beq_tmp); % Aeq_tmp*X = Beq_tmp， lambda个数为Aeq_tmp的行数
		diff = max(abs(old_X - X));
		if diff < stop_threshold
			break;
		end
		old_X = X;

		lambda_ieq = lambda( size(Aeq,1) + (1:numel(as_ieq)) ); % lambda_ieq代表Aieq(as_ieq,:)对应的拉格朗日乘数
		as_ieq = as_ieq(lambda_ieq > lambda_threshold);

		new_as_ieq = find((Aieq*X - Bieq) > active_threshold);

		as_ieq = unique([as_ieq; new_as_ieq]);

		if iter == max_iter
			warning( ...
				sprintf('Max iterations %d reached without convergence!', max_iter));
			break;
		end
		iter = iter + 1;
	end
	*/
	void VBTeethPatching::SolveQuadProgram(std::vector<VSTripletD>& X, 
		const VSSprsMatrixD& A,const VSSprsMatrixD& B,
		const VSSprsMatrixD& Aeq, const VSSprsMatrixD& Beq,
		const VSSprsMatrixD& Aieq, const VSSprsMatrixD& Bieq)
	{	
		static unsigned nCallCount = 0;
		nCallCount++;
		//quad_prog(A, B, Aeq, Beq, Aieq, Bieq, opt)
		//		3) 求矩阵M的逆inv(M), 即m'(i, i) =  1 / m(i,i);
		//		4) 计算矩阵L2=L*inv(M)*L		
		unsigned nXCount = A.row;
		std::vector<double> vOldX(nXCount, DBL_MAX);
		unsigned nIterCount = 1;
		unsigned nIterMax = 15;
		double dbStopThreshold = 0.01;
		double dbActiveThreshold = 0.0001;
		double dbLambdaThreshold = 0.001;
		std::vector<unsigned> vAieq2Aeq;
		std::vector<VSTripletD> vLambda;
#ifdef DUMP_ENABLE
		
		char szFileName[256] = { 0 };
		{			
			sprintf_s(szFileName, 256, "D:/testdata/ld/AieqMatrix_%d.txt", nCallCount);
			std::ofstream objFile(szFileName);
			objFile << Aieq.row << " " << Aieq.col << " 0.0" << std::endl;
			for (size_t j = 0; j < Aieq.val.len; j++)
			{
				objFile << Aieq.val.pData[j].row << " " << Aieq.val.pData[j].col << " " << std::fixed << std::setprecision(18) << Aieq.val.pData[j].val << std::endl;
			}
			objFile.close();

			sprintf_s(szFileName, 256, "D:/testdata/ld/BieqMatrix_%d.txt", nCallCount);
			objFile.open(szFileName);
			objFile << Bieq.row << " " << Bieq.col << " 0.0" << std::endl;
			for (size_t j = 0; j < Bieq.val.len; j++)
			{
				objFile << Bieq.val.pData[j].row << " " << Bieq.val.pData[j].col << " " << std::fixed << std::setprecision(18) << Bieq.val.pData[j].val << std::endl;
			}
			objFile.close();

			sprintf_s(szFileName, 256, "D:/testdata/ld/AeqMatrix_%d.txt", nCallCount);
			objFile.open(szFileName);
			objFile << Aeq.row << " " << Aeq.col << " 0.0" << std::endl;
			for (size_t j = 0; j < Aeq.val.len; j++)
			{
				objFile << Aeq.val.pData[j].row << " " << Aeq.val.pData[j].col << " " << std::fixed << std::setprecision(18) << Aeq.val.pData[j].val << std::endl;
			}
			objFile.close();

			sprintf_s(szFileName, 256, "D:/testdata/ld/BeqMatrix_%d.txt", nCallCount);
			objFile.open(szFileName);
			objFile << Beq.row << " " << Beq.col << " 0.0" << std::endl;
			for (size_t j = 0; j < Beq.val.len; j++)
			{
				objFile << Beq.val.pData[j].row << " " << Beq.val.pData[j].col << " " << std::fixed << std::setprecision(18) << Beq.val.pData[j].val << std::endl;
			}
			objFile.close();

			sprintf_s(szFileName, 256, "D:/testdata/ld/AMatrix_%d.txt", nCallCount);
			objFile.open(szFileName);
			objFile << A.row << " " << A.col << " 0.0" << std::endl;
			for (size_t j = 0; j < A.val.len; j++)
			{
				objFile << A.val.pData[j].row << " " << A.val.pData[j].col << " " << std::fixed << std::setprecision(18) << A.val.pData[j].val << std::endl;
			}
			objFile.close();

			sprintf_s(szFileName, 256, "D:/testdata/ld/BMatrix_%d.txt", nCallCount);
			objFile.open(szFileName);
			objFile << B.row << " " << B.col << " 0.0" << std::endl;
			for (size_t j = 0; j < B.val.len; j++)
			{
				objFile << B.val.pData[j].row << " " << B.val.pData[j].col << " " << std::fixed << std::setprecision(18) << B.val.pData[j].val << std::endl;
			}
			objFile.close();
		}
#endif
		while (true)
		{
			VD_PROFILE_START(InLoop);
			std::vector<VSTripletD> vAeqTmp, vBeqTmp;
			if (Aeq.val.len > 0)
			{
				vAeqTmp.resize(Aeq.val.len);
				std::memcpy(&vAeqTmp[0], Aeq.val.pData, sizeof(VSTripletD) * Aeq.val.len);

				vBeqTmp.resize(Beq.val.len);
				std::memcpy(&vBeqTmp[0], Beq.val.pData, sizeof(VSTripletD) * Beq.val.len);
			}
			{
				// 添加等式约束行到Aeq矩阵下面		

				VSTripletD trip;
				for (size_t i = 0; i < vAieq2Aeq.size(); i++)
				{
					unsigned nRowShift = Aeq.row;
					for (size_t j = 0; j < Aieq.val.len; j++)
					{
						if (Aieq.val.pData[j].row == vAieq2Aeq[i])
						{
							trip.row = nRowShift + i;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                                                                                   + i;
							trip.col = Aieq.val.pData[j].col;
							trip.val = Aieq.val.pData[j].val;
							vAeqTmp.push_back(trip);
						}
					}
					nRowShift = Beq.row;
					for (size_t j = 0; j < Bieq.val.len; j++)
					{
						if (Bieq.val.pData[j].row == vAieq2Aeq[i])
						{
							trip.row = nRowShift + i;
							trip.col = Bieq.val.pData[j].col;
							trip.val = Bieq.val.pData[j].val;
							vBeqTmp.push_back(trip);
						}
					}
				}
			}
			VSSprsMatrixD sprsMatAeqTmp, sprsMatBeqTmp;
			sprsMatAeqTmp.row = Aeq.row + vAieq2Aeq.size();
			sprsMatAeqTmp.col = Aeq.col;
			sprsMatAeqTmp.val = VD_V2CB(vAeqTmp);
			
			sprsMatBeqTmp.row = Beq.row + vAieq2Aeq.size();
			sprsMatBeqTmp.col = Beq.col;
			sprsMatBeqTmp.val = VD_V2CB(vBeqTmp);

#ifdef DUMP_ENABLE
			{

				sprintf_s(szFileName, 256, "D:/testdata/ld/AeqTmpMatrix_%d_%d.txt", nCallCount, nIterCount);
				std::ofstream objFile(szFileName);
				objFile << sprsMatAeqTmp.row << " " << sprsMatAeqTmp.col << " 0.0" << std::endl;
				for (size_t j = 0; j < sprsMatAeqTmp.val.len; j++)
				{
					objFile << sprsMatAeqTmp.val.pData[j].row << " " << sprsMatAeqTmp.val.pData[j].col << " " << std::fixed << std::setprecision(18) << sprsMatAeqTmp.val.pData[j].val << std::endl;
				}
				objFile.close();

				sprintf_s(szFileName, 256, "D:/testdata/ld/BeqTmpMatrix_%d_%d.txt", nCallCount, nIterCount);
				objFile.open(szFileName);
				objFile << sprsMatBeqTmp.row << " " << sprsMatBeqTmp.col << " 0.0" << std::endl;
				for (size_t j = 0; j < sprsMatBeqTmp.val.len; j++)
				{
					objFile << sprsMatBeqTmp.val.pData[j].row << " " << sprsMatBeqTmp.val.pData[j].col << " " << std::fixed << std::setprecision(18) << sprsMatBeqTmp.val.pData[j].val << std::endl;
				}
				objFile.close();
			}

			std::ofstream logFile("D:/testdata/ld/AMatrixinfo_%d.txt", nCallCount);
			logFile << "A row: " << A.row << " col: " << A.col << " val count: " << A.val.len << std::endl;
			for (unsigned i = 0; i < A.val.len; i++)
			{
				logFile << A.val.pData[i].row << "\t" << A.val.pData[i].col << "\t" << A.val.pData[i].val << std::endl;
			}
			logFile.close();
			logFile.open("D:/testdata/ld/BMatrixinfo_%d.txt", nCallCount);
			logFile << "B row: " << B.row << " col: " << B.col << " val count: " << B.val.len << std::endl;
			for (unsigned i = 0; i < B.val.len; i++)
			{
				logFile << B.val.pData[i].row << "\t" << B.val.pData[i].col << "\t" << B.val.pData[i].val << std::endl;
			}
			logFile.close();
			logFile.open("D:/testdata/ld/AeqMatrixinfo_%d.txt", nCallCount);
			logFile << "Aeq row: " << sprsMatAeqTmp.row << " col: " << sprsMatAeqTmp.col << " val count: " << sprsMatAeqTmp.val.len << std::endl;
			for (unsigned i = 0; i < sprsMatAeqTmp.val.len; i++)
			{
				logFile << sprsMatAeqTmp.val.pData[i].row << "\t" << sprsMatAeqTmp.val.pData[i].col << "\t" << sprsMatAeqTmp.val.pData[i].val << std::endl;
			}
			logFile.close();
			logFile.open("D:/testdata/ld/BeqMatrixinfo_%d.txt", nCallCount);
			logFile << "Beq row: " << sprsMatBeqTmp.row << " col: " << sprsMatBeqTmp.col << " val count: " << sprsMatBeqTmp.val.len << std::endl;
			for (unsigned i = 0; i < sprsMatBeqTmp.val.len; i++)
			{
				logFile << sprsMatBeqTmp.val.pData[i].row << "\t" << sprsMatBeqTmp.val.pData[i].col << "\t" << sprsMatBeqTmp.val.pData[i].val << std::endl;
			}

			if (1 == m_nOutput)
			{
				if (vAieq2Aeq.size() > 0)
				{
					char szFileName[256] = { 0 };
					sprintf_s(szFileName, 256, "D:/testdata/ld/aieq2aeq_%d_%d.txt", nCallCount, nIterCount);
					std::ofstream txtF(szFileName);
					txtF << vAieq2Aeq.size() << " 1" << " 0" << std::endl;
					for (unsigned k = 0; k < vAieq2Aeq.size(); k++)
					{
						txtF << k << " 0" << " " << vAieq2Aeq[k] << std::endl;
					}
					txtF.close();
				}
			}	
#endif

			VD_PROFILE_START(ldQuadProgFixed);
			QuadProgFixed(X, vLambda, A, B, sprsMatAeqTmp, sprsMatBeqTmp);
			VD_PROFILE_FINISH(ldQuadProgFixed);

#ifdef DUMP_ENABLE
			if (1 == m_nOutput)
			{
				char szFileName[256] = { 0 };
				sprintf_s(szFileName, 256, "D:/testdata/ld/AeqXMatrix_%d_%d.txt", nCallCount, nIterCount);
				std::ofstream txtX(szFileName);
				txtX << X.size() << " " << 1 << " 0" << std::endl;
				for (size_t i = 0; i < X.size(); i++)
				{
					txtX << X[i].row << " " << X[i].col << " " << std::fixed << std::setprecision(18) << X[i].val << std::endl;
				}
				txtX.close();

				sprintf_s(szFileName, 256, "D:/testdata/ld/LamdMatrix_%d_%d.txt", nCallCount, nIterCount);
				txtX.open(szFileName);
				txtX << vLambda.size() << " " << 1 << " 0" << std::endl;
				for (size_t i = 0; i < vLambda.size(); i++)
				{
					txtX << vLambda[i].row << " " << vLambda[i].col << " " << std::fixed << std::setprecision(18) << vLambda[i].val << std::endl;
				}
				txtX.close();
			}

			//if (1 == m_nOutput)
			//{				
			//	char szFileName[256] = { 0 };
			//	sprintf_s(szFileName, 256, "D:/testdata/ld/XMatrixinfo_%d_%d.txt", nCallCount, nIterCount);
			//	std::ofstream logFile(szFileName);
			//	logFile << "X: " << X.size() << std::endl;
			//	for (unsigned i = 0; i < X.size(); i++)
			//	{
			//		logFile << X[i].row << "\t" << X[i].col << "\t" << X[i].val << std::endl;
			//	}
			//	logFile.close();
			//}
#endif
			double dbDiff = DBL_MIN;
			{
				//diff = max(abs(old_X - X));
				//if diff < stop_threshold
				//	break;
				//end
				//	old_X = X;
				
				
				for (size_t i = 0; i < X.size(); i++)
				{
					double dbVal = std::abs(vOldX[i] - X[i].val);					
					if (dbVal > dbDiff)
					{
						dbDiff = dbVal;
					}
					vOldX[i] = X[i].val;
				}
//#ifdef SPEED_DEBUG
//				if (1 == m_nOutput)
//				{
//					std::ofstream txtF("D:/testdata/ld/loopinfo.txt", std::ios::app);
//					txtF << "dbDiff: " << dbDiff << " iter count: " << nIterCount << std::endl;
//					txtF.close();
//				}
//#endif
				//if (dbDiff < dbStopThreshold)
				//{					
				//	VD_PROFILE_FINISH(InLoop);
				//	break;
				//}
			}

			if (vAieq2Aeq.size() > 0)
			{
				// lambda_ieq = lambda( size(Aeq,1) + (1:numel(as_ieq)) ); % lambda_ieq代表Aieq(as_ieq,:)对应的拉格朗日乘数
				// as_ieq = as_ieq(lambda_ieq > lambda_threshold);
				std::vector<unsigned> vAieq2AeqTmp(vAieq2Aeq);
				vAieq2Aeq.clear();
				unsigned nSize = vAieq2AeqTmp.size() + Aeq.row;
				for (size_t i = Aeq.row; i < nSize; i++)
				{
					if (vLambda[i].val > dbLambdaThreshold)
					{
						//g_logFile << "lambda val " << i <<" index: "<<vAieq2AeqTmp[i-Aeq.row]<< std::endl;
						vAieq2Aeq.push_back(vAieq2AeqTmp[i - Aeq.row]);
					}
				}				
			}

			{
				// new_as_ieq = find((Aieq*X - Bieq) > active_threshold);
				// 查找满足(Aieq*X - Bieq) > blActiveThreshold的行
				NMALG_MATRIX::VSSparseMatrix vAieqTmpTmp;
				VSSprsMatrixD sprsMatX;
				sprsMatX.row = Aieq.col;
				sprsMatX.col = 1;
				sprsMatX.val = VD_V2CB(X);
				NMALG_MATRIX::MAT_AMultiB(vAieqTmpTmp, Aieq, sprsMatX);
				VSSprsMatrixD sprsMatTmpTmp;
				sprsMatTmpTmp.row = Aieq.row;
				sprsMatTmpTmp.col = 1;
				sprsMatTmpTmp.val = VD_V2CB(vAieqTmpTmp.vValue);

				NMALG_MATRIX::VSSparseMatrix vAieqTmp;
				NMALG_MATRIX::MAT_AMinusB(vAieqTmp, sprsMatTmpTmp, Bieq);
				bool blRet = false;
				for (size_t i = 0; i < vAieqTmp.vValue.size(); i++)
				{
					if (vAieqTmp.vValue[i].val > dbActiveThreshold)
					{
						blRet = true;
						vAieq2Aeq.push_back(vAieqTmp.vValue[i].row);
					}
				}
				//as_ieq = unique([as_ieq; new_as_ieq]);
				if ((false == blRet) && (dbDiff < dbStopThreshold))
				{
					VD_PROFILE_FINISH(InLoop);
					break;
				}
				std::sort(vAieq2Aeq.begin(), vAieq2Aeq.end());
				vAieq2Aeq.erase(std::unique(vAieq2Aeq.begin(), vAieq2Aeq.end()), vAieq2Aeq.end());
				
			}

			//if iter == max_iter
			//	warning(...
			//		sprintf('Max iterations %d reached without convergence!', max_iter));
			//break;
			//end
			//	iter = iter + 1;
			if (nIterCount == nIterMax)
			{				
				VD_PROFILE_FINISH(InLoop);
				break;
			}
			VD_PROFILE_FINISH(InLoop);
			nIterCount++;
		}
		//if (1 == m_nOutput)
		//{
		//	std::ofstream txtF("D:/testdata/ld/loopinfo.txt", std::ios::app);
		//	txtF  << std::endl;
		//	txtF.close();
		//}
	}

	void VBTeethPatching::QuadProgFixed(std::vector<VSTripletD>& X, std::vector<VSTripletD>& vLambda,
		const VSSprsMatrixD& A, const VSSprsMatrixD& B,
		const VSSprsMatrixD& Aeq, const VSSprsMatrixD& Beq)
	{
		::QuadProgFixed(X, vLambda, A, B, Aeq, Beq);
		//std::vector<VSTripletD> Q, R, E;
		//std::vector<VSTripletD> vMatAeqTrans;
		//GetMatrixTranspose(vMatAeqTrans, sprsMatAeqUidTmp);
		//VSSprsMatrixD sprsMatAeqTrans;
		//sprsMatAeqTrans.row = sprsMatAeqUidTmp.col;
		//sprsMatAeqTrans.col = sprsMatAeqUidTmp.row;
		//sprsMatAeqTrans.val = VD_V2CB(vMatAeqTrans);
		//SprsMatrixQRFactorizate(Q, R, E, sprsMatAeqTrans);
	}

	void VBTeethPatching::UpdateToothAxis(VSTeethPatchingResult& result, const VSConstBuffer<VSToothAxisContext>& toothAxises)
	{

	}

	void VBTeethPatching::UpdateToothGap(VSTeethPatchingResult& result, const VSConstBuffer<VSGapContext>& gaps)
	{

	}

	void VBTeethPatching::OnChanged()
	{

	}

	double VBTeethPatching::CollisionCheck(VSGapPointPairs& gapPointPair,
		const unsigned nToothIdx1, const unsigned nToothIdx2, CRITICAL_SECTION* pCsOpenGl)
	{
		VSSimpleMeshF toothObj1;
		VSSimpleMeshF toothObj2;

		toothObj1.pTriangles		= m_vObjTooth[nToothIdx1]->m_perfMesh.pSurfaces;
		toothObj1.pVertices			= m_vObjTooth[nToothIdx1]->m_perfMesh.pVertices;
		toothObj1.nVertCount		= m_vObjTooth[nToothIdx1]->m_perfMesh.GetVertexCount();
		toothObj1.nTriangleCount	= m_vObjTooth[nToothIdx1]->m_perfMesh.GetSurfCount();

		toothObj2.pTriangles		= m_vObjTooth[nToothIdx2]->m_perfMesh.pSurfaces;
		toothObj2.pVertices			= m_vObjTooth[nToothIdx2]->m_perfMesh.pVertices;
		toothObj2.nVertCount		= m_vObjTooth[nToothIdx2]->m_perfMesh.GetVertexCount();
		toothObj2.nTriangleCount	= m_vObjTooth[nToothIdx2]->m_perfMesh.GetSurfCount();
		VBCollisionSence::VSCollisionSenceResult ret;
		if (NULL != pCsOpenGl)
			EnterCriticalSection(pCsOpenGl);
		
		m_pCollisionSence->Build(ret, toothObj1, toothObj2);
		gapPointPair.pairToothLabel.first = nToothIdx1;
		gapPointPair.pairToothLabel.second = nToothIdx2;
		gapPointPair.gapPointPairs.first = ret.dir;
		gapPointPair.gapPointPairs.second.resize(ret.gapPointPairs.len);
		for (size_t i = 0; i < ret.gapPointPairs.len; i++)
		{
			gapPointPair.gapPointPairs.second[i].pointPair.first = ret.gapPointPairs.pData[i].nMeshAPoint;
			gapPointPair.gapPointPairs.second[i].pointPair.second = ret.gapPointPairs.pData[i].nMeshBPoint;
		
		}
		
			//std::ofstream txtF("E:/testdata/log.txt", std::ios::app);
			//txtF << "collision: " << ret.blCollision << " dist: " << ret.flDeepestDis << std::endl;
			float flMin = 1000.0f;
			for (size_t i = 0; i < ret.gapPointPairs.len; i++)
			{
				const VFVECTOR3& v1 = toothObj1.pVertices[gapPointPair.gapPointPairs.second[i].pointPair.first];
				const VFVECTOR3& v2 = toothObj2.pVertices[gapPointPair.gapPointPairs.second[i].pointPair.second];
				VFRay ray(v1, ret.dir);
				float flDist = ray.GetUnitsLenth(v2);
				if (flDist < flMin)
				{
					flMin = flDist;
				}
				
			}
			//txtF << "min: " << flMin << std::endl;
			//txtF << std::endl;
			//txtF.close();

		if (NULL != pCsOpenGl)
			LeaveCriticalSection(pCsOpenGl);
		return flMin;
		//return ret.flDeepestDis;
	}
	double VBTeethPatching::CollisionCheck(VBCollisionSence::VSCollisionSenceResult& ret,
		std::pair<VFVECTOR3, std::vector<VSCollisionPointPair> >& gapPointPairs,
		const VSSimpleMeshF& mesh1, const VSSimpleMeshF& mesh2, CRITICAL_SECTION* pCsOpenGl)
	{
		
		if (NULL != pCsOpenGl)
			EnterCriticalSection(pCsOpenGl);
		
		m_pCollisionSence->Build(ret, mesh1, mesh2);
		gapPointPairs.first = ret.dir;
		gapPointPairs.second.resize(ret.gapPointPairs.len);
		for (size_t i = 0; i < ret.gapPointPairs.len; i++)
		{
			gapPointPairs.second[i].pointPair.first = ret.gapPointPairs.pData[i].nMeshAPoint;
			gapPointPairs.second[i].pointPair.second = ret.gapPointPairs.pData[i].nMeshBPoint;			
		}

		if (NULL != pCsOpenGl)
			LeaveCriticalSection(pCsOpenGl);
		
		//std::ofstream txtF("E:/testdata/log.txt", std::ios::app);
		//txtF << "collision: " << ret.blCollision << " dist: " << ret.flDeepestDis << std::endl;
		float flMin = 1000.0f;
		for (size_t i = 0; i < ret.gapPointPairs.len; i++)
		{
			const VFVECTOR3& v1 = mesh1.pVertices[gapPointPairs.second[i].pointPair.first];
			const VFVECTOR3& v2 = mesh2.pVertices[gapPointPairs.second[i].pointPair.second];
			VFRay ray(v1, ret.dir);
			float flDist = ray.GetUnitsLenth(v2);
			if (flDist < flMin)
			{
				flMin = flDist;
			}

		}
		//txtF << "min: " << flMin << std::endl;
		//txtF << std::endl;
		//txtF.close();
		return flMin;
		//if (true == ret.blCollision)
		//{
		//	return -ret.flDeepestDis;
		//}
		//
		//return ret.flDeepestDis;
	}
	void VBTeethPatching::MergePatchedTooth()
	{
		unsigned nVertCount = 0;
		unsigned nSurfCount = 0;
		unsigned nVertIdx = 0;
		unsigned nSurfIdx = 0;
		m_vvToothVOnJawMesh.resize(m_vObjTooth.size());
		for (size_t i = 0; i < m_vObjTooth.size(); i++)
		{
			unsigned nVCnt = m_vObjTooth[i]->m_perfMesh.GetVertexCount();
			m_vvToothVOnJawMesh[i].resize(nVCnt);
			nVertCount += nVCnt;
			nSurfCount += m_vObjTooth[i]->m_perfMesh.GetSurfCount();
			unsigned nLen = m_vObjTooth[i]->m_perfMesh.GetVertexCount();
			for (size_t nVIdx = 0; nVIdx < nLen; nVIdx++)
				m_vvToothVOnJawMesh[i][nVIdx] = nVertIdx++;
		}
		m_vVerts.resize(nVertCount);
		m_vSurfs.resize(nSurfCount);
		m_vToothVertInfo.resize(m_vObjTooth.size());
		m_vFixVertOnJawMesh.resize(nVertCount,VD_INVALID_INDEX);
		m_vVertFlag.resize(nVertCount, 0);
		m_nKidCount = 0;
		m_nUidCount = 0;
		m_vVertMapKid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertMapUid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVarVertOnJawMesh.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertTypes.resize(nVertCount, ConstCrown);
		nVertIdx = 0;
		m_vSideCtrlPointPair.clear();
		for (size_t nToothIdx = 0; nToothIdx < m_vObjTooth.size(); nToothIdx++)
		{
			unsigned nVCnt = m_vObjTooth[nToothIdx]->m_perfMesh.GetVertexCount();
			const VFVECTOR3* pVert = m_vObjTooth[nToothIdx]->m_perfMesh.pVertices;
			unsigned nBase = nVertIdx;				// 保存牙齿顶点索引偏移量
			m_vToothVertInfo[nToothIdx].nIdxBase = nBase;
			m_vToothVertInfo[nToothIdx].nVertCount = nVCnt;
			for (size_t nVIdx = 0; nVIdx < nVCnt; nVIdx++)
			{				
				m_vVertTypes[nVertIdx] = m_vObjTooth[nToothIdx]->m_vVertType[nVIdx];
				m_vVerts[nVertIdx] = pVert[nVIdx];
				if ((ConstCrown == m_vVertTypes[nVertIdx]) || (ConstRoot == m_vVertTypes[nVertIdx]))
				{
					m_vFixVertOnJawMesh[m_nKidCount] = nVertIdx;
					m_vVertFlag[nVertIdx] = 1;
					m_vVertMapKid[nVertIdx] = m_nKidCount++;
				}
				else
				{
					m_vVarVertOnJawMesh[m_nUidCount] = nVertIdx;
					m_vVertMapUid[nVertIdx] = m_nUidCount++;
				}

				//m_vvToothVOnJawMesh[nToothIdx][nVIdx] = nVertIdx++;
				nVertIdx++;
			}
			unsigned nSCnt = m_vObjTooth[nToothIdx]->m_perfMesh.GetSurfCount();
			const VNVECTOR3UI* pSurf = m_vObjTooth[nToothIdx]->m_perfMesh.pSurfaces;
			for (size_t nSIdx = 0; nSIdx < nSCnt; nSIdx++)
			{
				for (size_t i = 0; i < 3; i++)
				{
					m_vSurfs[nSurfIdx][i] = pSurf[nSIdx][i] + nBase;
				}	
				nSurfIdx++;
			}
			for (size_t i = 0; i < m_vObjTooth[nToothIdx]->m_vNbrPatchedVert.size(); i++)
			{
				unsigned nNbrTooth = m_vObjTooth[nToothIdx]->m_vNbrPatchedVert[i].nNbrTooth;
				if (nNbrTooth < nToothIdx)
					continue;				// 避免控制点重复
		
			for (size_t j = 0; j < m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert.size(); j++)
				{
					if (nToothIdx == m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert[j].nNbrTooth)
					{
						unsigned nCtrl1 = m_vvToothVOnJawMesh[nToothIdx][m_vObjTooth[nToothIdx]->m_vNbrPatchedVert[i].ctrlVert];
						unsigned nCtrl2 = m_vvToothVOnJawMesh[nNbrTooth][m_vObjTooth[nNbrTooth]->m_vNbrPatchedVert[j].ctrlVert];
						m_vSideCtrlPointPair.push_back(std::make_pair(nCtrl1, nCtrl2));
					}
				}						
			}
		}

		//m_vVarVertOnJawMesh.resize(m_nUidCount);
		//m_vFixVertOnJawMesh.resize(nFixVIdx);	// 重新分配内存大小，移除多余的内存，保留原有数据

		VSSimpleMeshF simpMesh;
		simpMesh.nTriangleCount = m_vSurfs.size();
		simpMesh.nVertCount = m_vVerts.size();
		simpMesh.pTriangles = &m_vSurfs[0];
		simpMesh.pVertices = &m_vVerts[0];
		//OBJWriteSimpleMesh("D:/testdata/jawsimpmesh.obj", simpMesh);
		m_ptrVRPerfMesh->Build(m_jawMeshPatched, simpMesh);		
	}

	void VBTeethPatching::MergePatchedTooth(const VSTeethPatchingTopoResult& topoMesh)
	{
		unsigned nVertCount = 0;
		unsigned nSurfCount = 0;
		unsigned nVertIdx = 0;
		unsigned nSurfIdx = 0;
		const VSConstBuffer< VSPatchPair >& patchPair = topoMesh.patchPairs;
		
		m_vvToothVOnJawMesh.resize(topoMesh.teeth.len);
		for (size_t i = 0; i < topoMesh.teeth.len; i++)
		{
			const VSSimpleMeshF& mesh = topoMesh.teeth.pData[i].topoToothMesh;
			unsigned nVCnt = mesh.nVertCount;
			m_vvToothVOnJawMesh[i].resize(nVCnt);
			nVertCount += nVCnt;
			nSurfCount += mesh.nTriangleCount;			
			for (size_t nVIdx = 0; nVIdx < nVCnt; nVIdx++)
				m_vvToothVOnJawMesh[i][nVIdx] = nVertIdx++;
		}
		m_vVerts.resize(nVertCount);
		m_vSurfs.resize(nSurfCount);
		m_vToothVertInfo.resize(topoMesh.teeth.len);
		
		m_vVertFlag.resize(nVertCount, 0);
		m_nKidCount = 0;
		m_nUidCount = 0;
		m_vVertMapKid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertMapUid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVarVertOnJawMesh.resize(nVertCount, VD_INVALID_INDEX);
		m_vFixVertOnJawMesh.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertTypes.resize(nVertCount, ConstCrown);
		nVertIdx = 0;
		m_vSideCtrlPointPair.clear();
		for (size_t nToothIdx = 0; nToothIdx < topoMesh.teeth.len; nToothIdx++)
		{
			const VSSimpleMeshF& mesh = topoMesh.teeth.pData[nToothIdx].topoToothMesh;
			const VSConstBuffer< VEToothVertType >& vertType = topoMesh.teeth.pData[nToothIdx].vertTypes;
			
			unsigned nVCnt = mesh.nVertCount;			
			const VFVECTOR3* pVert = mesh.pVertices;
			unsigned nBase = nVertIdx;				// 保存牙齿顶点索引偏移量
			m_vToothVertInfo[nToothIdx].nIdxBase = nBase;
			m_vToothVertInfo[nToothIdx].nVertCount = nVCnt;
			for (size_t nVIdx = 0; nVIdx < nVCnt; nVIdx++)
			{
				m_vVertTypes[nVertIdx] = vertType.pData[nVIdx];
				m_vVerts[nVertIdx] = pVert[nVIdx];
			
				if ((ConstCrown == m_vVertTypes[nVertIdx]) || (ConstRoot == m_vVertTypes[nVertIdx]))
				{
					m_vFixVertOnJawMesh[m_nKidCount] = nVertIdx;
					m_vVertFlag[nVertIdx] = 1;
					m_vVertMapKid[nVertIdx] = m_nKidCount++;
				}
				else
				{
					m_vVarVertOnJawMesh[m_nUidCount] = nVertIdx;
					m_vVertMapUid[nVertIdx] = m_nUidCount++;
				}

				nVertIdx++;
			}
			unsigned nSCnt = mesh.nTriangleCount;
			const VNVECTOR3UI* pSurf = mesh.pTriangles;
			for (size_t nSIdx = 0; nSIdx < nSCnt; nSIdx++)
			{
				for (size_t i = 0; i < 3; i++)
				{
					m_vSurfs[nSurfIdx][i] = pSurf[nSIdx][i] + nBase;
				}
				nSurfIdx++;
			}
			
		}

		//m_vVarVertOnJawMesh.resize(m_nUidCount);
		for (size_t i = 0; i < patchPair.len; i++)
		{
			unsigned nTooth1 = patchPair.pData[i].toothAIdx;
			unsigned nTooth2 = patchPair.pData[i].toothBIdx;

			unsigned nCtrl1 = m_vvToothVOnJawMesh[nTooth1][patchPair.pData[i].toothAConnectVertIdx];
			unsigned nCtrl2 = m_vvToothVOnJawMesh[nTooth2][patchPair.pData[i].toothBConnectVertIdx];
			m_vSideCtrlPointPair.push_back(std::make_pair(nCtrl1, nCtrl2));
		}
		VSSimpleMeshF simpMesh;
		simpMesh.nTriangleCount = m_vSurfs.size();
		simpMesh.nVertCount = m_vVerts.size();
		simpMesh.pTriangles = &m_vSurfs[0];
		simpMesh.pVertices = &m_vVerts[0];
		//OBJWriteSimpleMesh("D:/testdata/jawsimpmesh.obj", simpMesh);
		m_ptrVRPerfMesh->Build(m_jawMeshPatched, simpMesh);
		//OBJWritePerfectMesh("D:/testdata/jawmeshpatched.obj", m_jawMeshPatched);
	}
	
	void VBTeethPatching::MergePatchedToothEx(const VSTeethPatchingTopoResult& topoMesh)
	{
		unsigned nVertCount = 0;
		unsigned nSurfCount = 0;
		unsigned nVertIdx = 0;
		unsigned nSurfIdx = 0;
		const VSConstBuffer< VSPatchPair >& patchPair = topoMesh.patchPairs;

		m_vvToothVOnJawMesh.resize(topoMesh.teeth.len);
		for (size_t i = 0; i < topoMesh.teeth.len; i++)
		{
			const VSSimpleMeshF& mesh = topoMesh.teeth.pData[i].topoToothMesh;
			unsigned nVCnt = mesh.nVertCount;
			m_vvToothVOnJawMesh[i].resize(nVCnt);
			nVertCount += nVCnt;
			nSurfCount += mesh.nTriangleCount;
			for (size_t nVIdx = 0; nVIdx < nVCnt; nVIdx++)
				m_vvToothVOnJawMesh[i][nVIdx] = nVertIdx++;
		}
		m_vVerts.resize(nVertCount);
		m_vSurfs.resize(nSurfCount);
		m_vToothVertInfo.resize(topoMesh.teeth.len);

		m_vVertFlag.resize(nVertCount, 0);
		m_nKidCount = 0;
		m_nUidCount = 0;
		m_vVertMapKid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertMapUid.resize(nVertCount, VD_INVALID_INDEX);
		m_vVarVertOnJawMesh.resize(nVertCount, VD_INVALID_INDEX);
		m_vFixVertOnJawMesh.resize(nVertCount, VD_INVALID_INDEX);
		m_vVertTypes.resize(nVertCount, ConstCrown);
		nVertIdx = 0;
		m_vSideCtrlPointPair.clear();
		for (size_t nToothIdx = 0; nToothIdx < topoMesh.teeth.len; nToothIdx++)
		{
			const VSSimpleMeshF& mesh = topoMesh.teeth.pData[nToothIdx].topoToothMesh;
			const VSConstBuffer< VEToothVertType >& vertType = topoMesh.teeth.pData[nToothIdx].vertTypes;

			unsigned nVCnt = mesh.nVertCount;
			const VFVECTOR3* pVert = &m_vObjTooth[nToothIdx]->m_vVertices[0];//mesh.pVertices;
			unsigned nBase = nVertIdx;				// 保存牙齿顶点索引偏移量
			m_vToothVertInfo[nToothIdx].nIdxBase = nBase;
			m_vToothVertInfo[nToothIdx].nVertCount = nVCnt;
			for (size_t nVIdx = 0; nVIdx < nVCnt; nVIdx++)
			{
				m_vVertTypes[nVertIdx] = vertType.pData[nVIdx];
				m_vVerts[nVertIdx] = pVert[nVIdx];

				if ((ConstCrown == m_vVertTypes[nVertIdx]) || (ConstRoot == m_vVertTypes[nVertIdx]))
				{
					m_vFixVertOnJawMesh[m_nKidCount] = nVertIdx;
					m_vVertFlag[nVertIdx] = 1;
					m_vVertMapKid[nVertIdx] = m_nKidCount++;
				}
				else
				{
					m_vVarVertOnJawMesh[m_nUidCount] = nVertIdx;
					m_vVertMapUid[nVertIdx] = m_nUidCount++;
				}

				nVertIdx++;
			}
			unsigned nSCnt = mesh.nTriangleCount;
			const VNVECTOR3UI* pSurf = mesh.pTriangles;
			for (size_t nSIdx = 0; nSIdx < nSCnt; nSIdx++)
			{
				for (size_t i = 0; i < 3; i++)
				{
					m_vSurfs[nSurfIdx][i] = pSurf[nSIdx][i] + nBase;
				}
				nSurfIdx++;
			}

		}

		//m_vVarVertOnJawMesh.resize(m_nUidCount);
		for (size_t i = 0; i < patchPair.len; i++)
		{
			unsigned nTooth1 = patchPair.pData[i].toothAIdx;
			unsigned nTooth2 = patchPair.pData[i].toothBIdx;

			if ((VD_INVALID_INDEX == patchPair.pData[i].toothAConnectVertIdx) ||
				(VD_INVALID_INDEX == patchPair.pData[i].toothBConnectVertIdx))
				continue;
			unsigned nCtrl1 = m_vvToothVOnJawMesh[nTooth1][patchPair.pData[i].toothAConnectVertIdx];
			unsigned nCtrl2 = m_vvToothVOnJawMesh[nTooth2][patchPair.pData[i].toothBConnectVertIdx];
			
			m_vSideCtrlPointPair.push_back(std::make_pair(nCtrl1, nCtrl2));			
		}
		VSSimpleMeshF simpMesh;
		simpMesh.nTriangleCount = m_vSurfs.size();
		simpMesh.nVertCount = m_vVerts.size();
		simpMesh.pTriangles = &m_vSurfs[0];
		simpMesh.pVertices = &m_vVerts[0];
		//OBJWriteSimpleMesh("D:/testdata/jawsimpmesh.obj", simpMesh);
		m_ptrVRPerfMesh->Build(m_jawMeshPatched, simpMesh);
		//OBJWritePerfectMesh("D:/testdata/jawmeshpatched.obj", m_jawMeshPatched);
	}

	void VBTeethPatching::MakeHUidUid(const VSConstBuffer<VSTripletD>& L2)
	{
		unsigned nEleIdx = 0;
		m_matHUidUid.resize(L2.len);		
		
		for (size_t i = 0; i < L2.len; i++)
		{
			if ((0 == m_vVertFlag[L2.pData[i].row]) && (0 == m_vVertFlag[L2.pData[i].col]))
			{
				m_matHUidUid[nEleIdx].row = m_vVertMapUid[L2.pData[i].row];
				m_matHUidUid[nEleIdx].col = m_vVertMapUid[L2.pData[i].col];
				m_matHUidUid[nEleIdx++].val = L2.pData[i].val * 0.5;
			}
		}
		m_matHUidUid.resize(nEleIdx * 3);
		for (size_t i = 1; i <= 2; i++)
		{
			unsigned nShift = i * nEleIdx;
			unsigned nOffset = i * m_nUidCount;
			for (size_t j = 0; j < nEleIdx; j++)
			{
				unsigned nIdx = j + nShift;
				m_matHUidUid[nIdx].row = nOffset + m_matHUidUid[j].row;
				m_matHUidUid[nIdx].col = nOffset + m_matHUidUid[j].col;
				m_matHUidUid[nIdx].val = m_matHUidUid[j].val;
			}
		}
		m_sprsMatHUidUid.row = m_sprsMatHUidUid.col = m_nUidCount * 3;
		m_sprsMatHUidUid.val = VD_V2CB(m_matHUidUid);
	}

	void VBTeethPatching::MakeHKidUidTranspose(std::vector<VSTripletD>& matHKidUidTranspose, const VSConstBuffer<VSTripletD>& L2)
	{
		unsigned nEleIdx = 0;
		matHKidUidTranspose.resize(L2.len);
		
		for (size_t i = 0; i < L2.len; i++)
		{
			if ((1 == m_vVertFlag[L2.pData[i].row]) && (0 == m_vVertFlag[L2.pData[i].col]))
			{
				// Transpose
				matHKidUidTranspose[nEleIdx].col = m_vVertMapKid[L2.pData[i].row];
				matHKidUidTranspose[nEleIdx].row = m_vVertMapUid[L2.pData[i].col];
				matHKidUidTranspose[nEleIdx++].val = L2.pData[i].val;
			}
		}
		matHKidUidTranspose.resize(nEleIdx * 3);
		for (size_t i = 1; i <= 2; i++)
		{
			unsigned nShift = i * nEleIdx;
			unsigned nRowOffset = i * m_nUidCount;
			unsigned nColOffset = i * m_nKidCount;
			for (size_t j = 0; j < nEleIdx; j++)
			{
				unsigned nIdx = j + nShift;
				matHKidUidTranspose[nIdx].row = nRowOffset + matHKidUidTranspose[j].row;
				matHKidUidTranspose[nIdx].col = nColOffset + matHKidUidTranspose[j].col;
				matHKidUidTranspose[nIdx].val = matHKidUidTranspose[j].val;
			}
		}
	}

	void VBTeethPatching::UpdateJawMesh(const VSConstBuffer<VSTripletD>& cbX)
	{
		for (size_t i = 0; i < m_nUidCount; i++)
		{
			unsigned nVIdx = m_vVarVertOnJawMesh[i];
		
			m_vVerts[nVIdx].x = cbX.pData[i].val;
			m_vVerts[nVIdx].y = cbX.pData[i + m_nUidCount].val;
			m_vVerts[nVIdx].z = cbX.pData[i + m_nUidCount * 2].val;
		}
		
		VSSimpleMeshF simpMesh;
		simpMesh.pVertices = &m_vVerts[0];
		simpMesh.pTriangles = &m_vSurfs[0];
		simpMesh.nTriangleCount = m_vSurfs.size();
		simpMesh.nVertCount = m_vVerts.size();
		CheckZeroTriangle(m_vVerts, VD_V2CB(m_vSurfs));
#if 0
		{
			static unsigned nCount = 0;
			nCount++;
			char szFileName[256] = { 0 };
			sprintf_s(szFileName, 256, "D:/testdata/ld/jaw%d.obj", nCount);
			OBJWriteSimpleMesh(szFileName, simpMesh);
		}
#endif
		m_ptrVRPerfMesh->Build(m_jawMeshPatched, simpMesh);
	}

	void VBTeethPatching::CheckZeroTriangle(std::vector<VFVECTOR3> vVerts, const VSConstBuffer<VNVECTOR3UI>& cbSurfs)
	{
		float flThreshold = 1.0e-7;
		for (unsigned i = 0; i < cbSurfs.len; i++)
		{
			const VNVECTOR3UI& tri = cbSurfs.pData[i];
			VFVECTOR3 & v1 = vVerts[tri[0]];
			VFVECTOR3 & v2 = vVerts[tri[1]];
			VFVECTOR3 & v3 = vVerts[tri[2]];

			VFVECTOR3 u1(v1 - v2), u2(v2 - v3), u3(v3 - v1);

			if (u1.IsZero(0.0f))
			{
				v1.x += flThreshold;
				v1.y += flThreshold;
				v1.z += flThreshold;
				v2.x -= flThreshold;
				v2.y -= flThreshold;
				v2.z -= flThreshold;
			}
			else if(u2.IsZero(0.0f))
			{
				v3.x += flThreshold;
				v3.y += flThreshold;
				v3.z += flThreshold;
				v2.x -= flThreshold;
				v2.y -= flThreshold;
				v2.z -= flThreshold;
			}
			else if (u3.IsZero(0.0f))
			{
				v1.x += flThreshold;
				v1.y += flThreshold;
				v1.z += flThreshold;
				v3.x -= flThreshold;
				v3.y -= flThreshold;
				v3.z -= flThreshold;
			}
		}
	}

	void VBTeethPatching::UpdateBtmCtrlPoints(const VSTeethPatchingTopoResult& topo, const VSTeethPatchInfo& info)
	{

	}

	void VBTeethPatching::GetHardPalateInfo(VSTeethPatchingTopoResult & topoRslt)
	{
		topoRslt.meshHardPalate.pTriangles = NULL;
		topoRslt.meshHardPalate.pVertices = NULL;
		topoRslt.meshHardPalate.nTriangleCount = 0;
		topoRslt.meshHardPalate.nVertCount = 0;
		topoRslt.meshHPVertType.len = 0;
		topoRslt.meshHPVertType.pData = 0;
		//return;
		unsigned nVertCount = m_segmentedJaw.mshWhole.GetVertexCount();
		unsigned nSursCount = m_segmentedJaw.mshWhole.GetSurfCount();
		m_meshHardPalate.vVertice.resize(nVertCount);
		m_meshHardPalate.vSurface.resize(nSursCount);
		
		std::vector<unsigned> vVertFlag(nVertCount, VD_INVALID_INDEX);
		for (size_t nTIdx = 0; nTIdx < m_segmentedJaw.nToothCount; nTIdx++)
		{
			for (size_t nVIdx = 0; nVIdx < m_vObjTooth[nTIdx]->m_vVertOnJawMesh.size(); nVIdx++)
			{
				vVertFlag[m_vObjTooth[nTIdx]->m_vVertOnJawMesh[nVIdx]] = nTIdx;
			}
		}
#if 0
		{			
			char szFileName[256] = { 0 };			
			{
				sprintf_s(szFileName, 256, "E:/testdata/meshhardpalate_lower.dat");
				OBJWriteSimpleMesh("E:/testdata/meshHardPalateLower.obj", topoRslt.meshHardPalate);
			}
			std::ofstream datF(szFileName, std::ios::binary);
			TVPersist<NMALG_TEETHPATCHING::VSTeethPatchingTopoResult> pstRst;
			TVPersist<VSConstBuffer<unsigned> > pstVertFlag;
			std::vector<unsigned> vVertFlagTmp(vVertFlag.size() + 1);
			std::memcpy(&vVertFlagTmp[0], &vVertFlag[0], sizeof(unsigned) * vVertFlag.size());
			vVertFlagTmp[vVertFlag.size()] = m_segmentedJaw.nToothCount;
			pstVertFlag.Build(VD_V2CB(vVertFlagTmp));
			pstRst.Build(topoRslt);
			datF << pstRst << pstVertFlag;
			datF.close();
			
		}
#endif
		unsigned nVertIdx = 0;
		m_vvMeshHPVertType.resize(m_segmentedJaw.nToothCount);
#if 0
		std::vector< unsigned > vSurfScore(nSursCount, 0);
		const VSPerfectMesh& mesh = m_segmentedJaw.mshWhole;
		//VSSimpleMeshF simpMesh;
		//simpMesh.pTriangles = mesh.pSurfaces;
		//simpMesh.pVertices = mesh.pVertices;
		//simpMesh.nVertCount = mesh.GetVertexCount();
		//simpMesh.nTriangleCount = mesh.GetSurfCount();
		//OBJWriteSimpleMesh("E:/testdata/segmentjaw.obj", simpMesh);
		while(true)
		{
			unsigned nSurfIdx = 0;
			nVertIdx = 0;
			std::vector< unsigned > stkSearch, stkBuff;
			std::vector<unsigned> vVertMap(nVertCount, VD_INVALID_INDEX);
			std::vector<unsigned> nVertMark(nVertCount, 0);
			std::vector<unsigned> nVertTypeMark(nVertCount, 0);
			for (size_t i = 0; i < m_segmentedJaw.nToothCount; i++)
			{
				m_vvMeshHPVertType[i].clear();
			}
			for (unsigned i = 0; i < nSursCount; i++)
			{
				const VNVECTOR3UI& s = mesh.pSurfaces[i];
				if (vVertFlag[s[0]] == VD_INVALID_INDEX
					&& vVertFlag[s[1]] == VD_INVALID_INDEX
					&& vVertFlag[s[2]] == VD_INVALID_INDEX
					&& 0 == vSurfScore[i])
				{
					vSurfScore[i] = 1;
					stkSearch.push_back(i);
					for (size_t j = 0; j < 3; j++)
					{
						unsigned nVIdx = s[j];
						if (0 == nVertMark[nVIdx])
						{
							nVertMark[nVIdx] = 1;
							vVertMap[nVIdx] = nVertIdx;
							m_meshHardPalate.vVertice[nVertIdx++] = mesh.pVertices[nVIdx];
						}
					}
					m_meshHardPalate.vSurface[nSurfIdx++] = VNVECTOR3UI(vVertMap[s.x], vVertMap[s.y], vVertMap[s.z]);
					break;
				}
			}
					
			while (!stkSearch.empty())
			{	
				for (auto sid : stkSearch)
				{
					auto eos = mesh.GetEdgeIndxOfSurf(sid);
					auto eis = mesh.GetEdgeNbrIndxOfSurf(sid);

					for (size_t j = 0; j < 3; j++)
					{
						auto soe = mesh.GetSurfIndxOfEdge(eos[j]);
						unsigned ni = ((eis[j] + 1) & 0x01);
						auto sn = soe[ni];
						if (sn != VD_INVALID_INDEX && vSurfScore[sn] == 0)
						{
							vSurfScore[sn] = 1;
							const VNVECTOR3UI& s = mesh.pSurfaces[sn];
							unsigned nGumCount = 3;
							unsigned nIdx = VD_INVALID_INDEX;
							for (size_t i = 0; i < 3; i++)
							{
								if (VD_INVALID_INDEX != vVertFlag[s[i]])
								{
									nIdx = i;
									nGumCount--;
								}
							}
							if (3 == nGumCount)
							{
								for (size_t i = 0; i < 3; i++)
								{
									unsigned nVIdx = s[i];
									if (0 == nVertMark[nVIdx])
									{
										nVertMark[nVIdx] = 1;
										vVertMap[nVIdx] = nVertIdx;
										m_meshHardPalate.vVertice[nVertIdx++] = mesh.pVertices[nVIdx];
									}
								}
								m_meshHardPalate.vSurface[nSurfIdx++] = VNVECTOR3UI(vVertMap[s.x], vVertMap[s.y], vVertMap[s.z]);
								stkBuff.push_back(sn);
							}
						}
					}
				}
				stkSearch.swap(stkBuff);
				stkBuff.clear();
			}
			if (nVertIdx > nSursCount / 10)
			{
				m_meshHardPalate.vSurface.resize(nSurfIdx);
				m_meshHardPalate.vVertice.resize(nVertIdx);
				for (size_t i = 0; i < nSursCount; i++)
				{
					const VNVECTOR3UI& s = mesh.pSurfaces[i];
					unsigned nGumCount = 3;
					unsigned nIdx = VD_INVALID_INDEX;
					for (size_t j = 0; j < 3; j++)
					{
						if (VD_INVALID_INDEX != vVertFlag[s[j]])
						{
							nIdx = j;
							nGumCount--;
						}
					}
					if (2 == nGumCount)
					{
						unsigned nVIdx = s[nIdx];
						for (size_t k = 1; k <= 2; k++)
						{
							unsigned nNextIdx = s[(nIdx + k) % 3];
							if ((0 == nVertTypeMark[nNextIdx]) && (1 == nVertMark[nNextIdx]))
							{
								m_vvMeshHPVertType[vVertFlag[nVIdx]].push_back(vVertMap[nNextIdx]);
								nVertTypeMark[nNextIdx] = 1;
							}
						}						
					}
				}
				break;
			}
		}
		

#else		
		std::vector<unsigned> vVertMap(nVertCount, VD_INVALID_INDEX);
		for (size_t i = 0; i < nVertCount; i++)
		{
			if (VD_INVALID_INDEX == vVertFlag[i])
			{
				vVertMap[i] = nVertIdx;
				m_meshHardPalate.vVertice[nVertIdx++] = m_segmentedJaw.mshWhole.pVertices[i];
			}
		}
		m_meshHardPalate.vVertice.resize(nVertIdx);
		for (size_t nSIdx = 0; nSIdx < nSursCount; nSIdx++)
		{
			const VNVECTOR3UI& s = m_segmentedJaw.mshWhole.pSurfaces[nSIdx];
			VNVECTOR3UI newS;
			unsigned nGumCount = 3;
			unsigned nToothIdx = VD_INVALID_INDEX;
			unsigned nVMapIdx = VD_INVALID_INDEX;
			for (size_t i = 0; i < 3; i++)
			{
				newS[i] = vVertMap[s[i]];
				if (VD_INVALID_INDEX != vVertFlag[s[i]])
				{
					nToothIdx = vVertFlag[s[i]];
					nVMapIdx = vVertFlag[s[i]];
					nGumCount--;
				}
			}
			if (3 == nGumCount)
			{
				m_meshHardPalate.vSurface[nVertIdx++] = newS;
			}
			else if (2 == nGumCount)
			{
				m_vvMeshHPVertType[nToothIdx].push_back(nVMapIdx);
			}
		}
		m_meshHardPalate.vSurface.resize(nVertIdx);
#endif
		//RemoveSurfWith3Bndry(m_meshHardPalate);
		Remove1VertConn2Bndry(m_meshHardPalate);
		PMGetSimpMesh(topoRslt.meshHardPalate, m_meshHardPalate);
		m_vCbMeshHPVertType.resize(m_segmentedJaw.nToothCount);
		for (size_t nTIdx = 0; nTIdx < m_segmentedJaw.nToothCount; nTIdx++)
		{
			m_vCbMeshHPVertType[nTIdx] = VD_V2CB(m_vvMeshHPVertType[nTIdx]);
		}
		topoRslt.meshHPVertType = VD_V2CB(m_vCbMeshHPVertType);
	}

	/************************************************************************
		Function: VBTeethPatching::Remove1VertConn2Bndry
		Description:
	     移除一个顶点有4条边是边界的情况，通过增加两个新的三角片来解决；如下图所示:	 
							_________	
			|\     /|       |\ new /|
			|o\   /o|  		|o\   /o|
			|l \ / l|		|l \ / l|
			|d / \ d|  =>   |d / \ d|
			| /   \ |		| /   \ |
			|/     \|  		|/ new \|
							---------
	************************************************************************/
	void VBTeethPatching::Remove1VertConn2Bndry(NM_PMMESH::VSMesh& meshInOut)
	{
		VSPerfectMesh perfMesh;
		VSSimpleMeshF simpMesh;
		PMGetSimpMesh(simpMesh, meshInOut);
		GETPERFECTMESH(perfMesh, simpMesh, extPerfMesh, rbPerfMesh);		
		std::vector<unsigned> vSurfFlag(simpMesh.nTriangleCount, 0);
		for (size_t nVIdx = 0; nVIdx < simpMesh.nVertCount; nVIdx++)
		{
			unsigned eNum = perfMesh.GetEdgeCountOfVetex(nVIdx);
			unsigned sNum = perfMesh.GetSurfCountOfVetex(nVIdx);
			std::vector<std::pair<unsigned, unsigned> > vSurfs;
			for (size_t nIdx = 0; nIdx  < sNum; nIdx ++)
			{
				unsigned nSIdx = perfMesh.GetSurfIndxOfVertex(nVIdx, nIdx);
				const VNVECTOR3UI& s = simpMesh.pTriangles[nSIdx];
				unsigned nCurVIdx = 0;
				for (; nCurVIdx < 3; nCurVIdx++)
				{
					if (s[nCurVIdx] == nVIdx)
					{
						break;
					}
				}
				vSurfs.push_back(std::make_pair(s[(nCurVIdx+1)%3], s[(nCurVIdx +2) % 3]));
			}
			 
			unsigned nBndryCount = 0;
			std::vector<unsigned> vBndrySurf;		
			for (unsigned nEIdx = 0; nEIdx < eNum; nEIdx++)
			{
				unsigned eid = perfMesh.GetEdgeIndxOfVertex(nVIdx, nEIdx);
				VNVECTOR2UI  soe = perfMesh.GetSurfIndxOfEdge(eid);
				//VNVECTOR2UI  voe = perfMesh.GetVertIndxOfEdge(eid);
				for (size_t i= 0; i < 2; i++)
				{
					if (VD_INVALID_INDEX == soe[i])
					{
						nBndryCount++;
						unsigned nSIdx = soe[1 - i];
						if (0 == vSurfFlag[nSIdx])
						{
							vBndrySurf.push_back(nSIdx);
							vSurfFlag[nSIdx] = 1;
						}
					}
				}
			}
			if (4 == nBndryCount)
			{
				// 添加三角片
				const VNVECTOR3UI& surf0 = perfMesh.pSurfaces[vBndrySurf[0]];
				const VNVECTOR3UI& surf1 = perfMesh.pSurfaces[vBndrySurf[1]];
				std::vector<unsigned> vVIdx(2);
				for (size_t i = 0; i < 2; i++)
				{
					for (size_t j = 0; j < 3; j++)
					{
						if (nVIdx == perfMesh.pSurfaces[vBndrySurf[i]][j])
						{
							vVIdx[i] = j;
							break;
						}
					}
				}
				for (size_t i = 0; i < 2; i++)
				{
					meshInOut.vSurface.push_back(VNVECTOR3UI(nVIdx, perfMesh.pSurfaces[vBndrySurf[i]][(vVIdx[i] + 2) % 3], perfMesh.pSurfaces[vBndrySurf[1-i]][(vVIdx[1-i] + 1) % 3]));
				}
			}
			for ( unsigned i = 0; i < vBndrySurf.size(); i++)
			{
				vSurfFlag[vBndrySurf[i]] = 0;
			}
		}
	}

	void VBTeethPatching::RemoveSurfWith3Bndry(NM_PMMESH::VSMesh& meshInOut)
	{
		NM_PMMESH::VSMesh tmpMesh;
		PMCopyMesh(tmpMesh, meshInOut);
		VSPerfectMesh perfMesh;
		VSSimpleMeshF simpMesh;
		PMGetSimpMesh(simpMesh, tmpMesh);
		GETPERFECTMESH(perfMesh, simpMesh, extPerfMesh, rbPerfMesh);

		TVExtSource< VNALGMESH::VRSliceMeshBoundary > extMeshBndry;
		TVR2B<VNALGMESH::VRSliceMeshBoundary> rbMeshBndry(*extMeshBndry);
		VNALGMESH::VSMeshVertLine bndry;
		rbMeshBndry.Build(bndry, perfMesh);
		std::vector<unsigned> vVertFlag(simpMesh.nVertCount, 0);
		std::vector<unsigned> vSurfRemoveFlag(simpMesh.nTriangleCount, 0);
		for (size_t nVIdx = 0; nVIdx < bndry.len; nVIdx++)
		{
			vVertFlag[bndry.pData[nVIdx]] = 1;
		}

		for (size_t nSIdx = 0; nSIdx < simpMesh.nTriangleCount; nSIdx++)
		{
			const VNVECTOR3UI& s = simpMesh.pTriangles[nSIdx];
			if ((1 == vVertFlag[s.x]) && (1 == vVertFlag[s.y]) && (1 == vVertFlag[s.z]))
				vSurfRemoveFlag[nSIdx] = 1;
		}

		std::vector<unsigned> vVertMap(simpMesh.nVertCount, VD_INVALID_INDEX);
		unsigned nVertCount = 0;
		unsigned nSurfCount = 0;
		for (size_t nSIdx = 0; nSIdx < simpMesh.nTriangleCount; nSIdx++)
		{
			if (1 == vSurfRemoveFlag[nSIdx])
				continue;
			const VNVECTOR3UI& s = simpMesh.pTriangles[nSIdx];
			unsigned nVIdx[3];
			for (size_t i = 0; i < 3; i++)
			{
				if (VD_INVALID_INDEX == vVertMap[s[i]])
				{
					vVertMap[s[i]] = nVertCount;
					meshInOut.vVertice[nVertCount++] = simpMesh.pVertices[s[i]];
				}
				nVIdx[i] = vVertMap[s[i]];
			}
			meshInOut.vSurface[nSurfCount++] = VNVECTOR3UI(nVIdx[0], nVIdx[1], nVIdx[2]);
		}
		meshInOut.vVertice.resize(nVertCount);
		meshInOut.vSurface.resize(nSurfCount);
	}
}
