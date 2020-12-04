#include "stdafx.h"
#include "lsalgcutpathgen.h"
#include <obj/lsalgtools/ilsalgtoolsperfmesh.h>
#include <obj/lsalgtools/profile.h>
#include <pkg/ldck4cce.h>
#include "VCFindPointPairPath.h"
#include "VCCalcLevelSet.h"

//#define DUMP_INFO
//#define RUNTIME_TEST
#ifdef RUNTIME_TEST
#ifdef VD_F_PROFILE_START
#undef  VD_F_PROFILE_START
#endif
#ifdef VD_F_PROFILE_FINISH
#undef  VD_F_PROFILE_FINISH
#endif
#define VD_F_PROFILE_START( profile ) VSFProfile::StartProfile(#profile,"E:/testdata/profile.txt");
#define VD_F_PROFILE_FINISH( profile ) VSFProfile::FinishProfile(#profile,"E:/testdata/profile.txt");
#endif


namespace NMALG_CUTPATHGEN
{
	IVObject* GetGenerator(const unsigned nGuid)
	{
		return static_cast<IVCutPathGenerator*>(new VSCutPathGenerator(nGuid));
	}

	VSCutPathGenerator::VSCutPathGenerator(const unsigned nGuid): NM_COMMON::VObjRef(nGuid)
	{
		m_flMaxRatio = INIGetFloat(L"CutPathMaxRatio", L"./config.ini");//6.0f;
		m_flMinRatio = INIGetFloat(L"CutPathMinRatio", L"./config.ini");//6.0f;
	}

	VSCutPathGenerator::~VSCutPathGenerator()
	{

	}

	void VSCutPathGenerator::Gen(VSCutPathInfo& pathInfo, const VSCutPathGenInput& input)
	{
		VCFindPointPairPath findPointPairPath;
		VCCalcLevelSet calcLevelSet;
		VD_F_PROFILE_START(VSCutPathGenerator::Gen);
		VSConstBuffer<unsigned> cbFrontVertices;
		VSConstBuffer<double> cbLevelSet;
		std::vector<unsigned> vCutPathVIndex;
		std::memset(&pathInfo, 0, sizeof(VSCutPathInfo));
		VSPerfectMesh perfMesh;
		GETPERFECTMESH(perfMesh, input.gumMesh, extSys, rb);
		pathInfo.blValid = ErrorCode::CUTPATH_GEN_OK;

		for (size_t ti = 0; ti < input.teethMesh.len - 1; ti++)
		{
			const auto& curTeethVerts = input.teethMesh.pData[ti].pVertices;
			const auto& curTeethVertCnt = input.teethMesh.pData[ti].nVertCount;
			const auto& nxtTeethVerts = input.teethMesh.pData[ti + 1].pVertices;
			const auto& nxtTeethVertCnt = input.teethMesh.pData[ti + 1].nVertCount;

			VFVECTOR3 curTeethCen = VFVECTOR3::ZERO;
			VFVECTOR3 nxtTeethCen = VFVECTOR3::ZERO;
			for (size_t i = 0; i < curTeethVertCnt; i++)
				curTeethCen += curTeethVerts[i];
			curTeethCen /= (float)curTeethVertCnt;
			for (size_t i = 0; i < nxtTeethVertCnt; i++)
				nxtTeethCen += nxtTeethVerts[i];
			nxtTeethCen /= (float)nxtTeethVertCnt;

			VFVECTOR3 cur2nxt = nxtTeethCen - curTeethCen;
			cur2nxt.Normalize();
			float curMaxDist = VF_MIN;
			for (size_t i = 0; i < curTeethVertCnt; i++)
			{
				const float dist = cur2nxt.Dot(curTeethVerts[i] - curTeethCen);
				if (curMaxDist < dist)
					curMaxDist = dist;
			}
			VFVECTOR3 nxt2cur = curTeethCen - nxtTeethCen;
			nxt2cur.Normalize();
			float nxtMaxDist = VF_MIN;
			for (size_t i = 0; i < nxtTeethVertCnt; i++)
			{
				const float dist = nxt2cur.Dot(nxtTeethVerts[i] - nxtTeethCen);
				if (nxtMaxDist < dist)
					nxtMaxDist = dist;
			}

			if ((nxtTeethCen - curTeethCen).Magnitude() - curMaxDist - nxtMaxDist > 3.5f)
			{
				pathInfo.blValid = ErrorCode::CUTPATH_GEN_FAIL_EXTRACTED;
				return;
			}
		}

#ifdef DUMP_INFO
		OBJWriteSimpleMesh("E:/testdata/meshTeeth.obj", input.teethMesh);
		OBJWriteSimpleMesh("E:/testdata/meshJawGum.obj", input.gumMesh);
#endif
		VD_F_PROFILE_START(VSCutPathGenerator::findPointPairPath.Gen);
		findPointPairPath.Gen(cbFrontVertices, input);		
		VD_F_PROFILE_FINISH(VSCutPathGenerator::findPointPairPath.Gen);
		VD_F_PROFILE_START(VSCutPathGenerator::calcLevelSet.Calc);
		calcLevelSet.Calc(cbLevelSet, perfMesh, cbFrontVertices, input.cbBottomLine);
		VD_F_PROFILE_FINISH(VSCutPathGenerator::calcLevelSet.Calc);
		std::vector<float> vLevelSet(cbLevelSet.len);
		for (size_t i = 0; i < cbLevelSet.len; i++)
		{
			vLevelSet[i] = cbLevelSet.pData[i];
		}
		VD_F_PROFILE_START(VSCutPathGenerator::m_findCutPath.Gen);
		m_findCutPath.Gen(pathInfo, vCutPathVIndex, perfMesh, VD_V2CB(vLevelSet));
		VD_F_PROFILE_FINISH(VSCutPathGenerator::m_findCutPath.Gen);
		if (pathInfo.blValid != CUTPATH_GEN_OK)
			return;

		std::vector<unsigned> vOptimizeVertIndex;
		std::vector<VFVECTOR3> vCutVertices;
		std::vector<VFVECTOR3> vCutNormals;
		VD_F_PROFILE_START(VSCutPathGenerator::OptimizeCutPath);
		OptimizeCutPath(vCutVertices, vOptimizeVertIndex, VD_V2CB(vCutPathVIndex), pathInfo.cbCutVertices);
		VD_F_PROFILE_FINISH(VSCutPathGenerator::OptimizeCutPath);
		VD_F_PROFILE_START(VSCutPathGenerator::CalcVertNormals);				
		if (!CalcVertNormals(vCutNormals, VD_V2CB(vCutVertices), VD_V2CB(vOptimizeVertIndex), perfMesh, input))
			pathInfo.blValid = ErrorCode::CUTPATH_GEN_FAIL_CUTNORM;
		VD_F_PROFILE_FINISH(VSCutPathGenerator::CalcVertNormals);
		// 添加入射点和结束点
		VASSERT(vCutVertices.size() == vCutNormals.size());
		unsigned nSize = vCutVertices.size();
		m_vCutVertices.resize(nSize + 3);
		m_vCutNormals.resize(nSize + 3);
		unsigned nStartIdx = VD_INVALID_INDEX;
		float flMin = 1000.0f;
		for (size_t i = 0; i < nSize; i++)
		{
			if (vCutVertices[i].y < flMin)
			{
				flMin = vCutVertices[i].y;
				nStartIdx = i;
			}
		}
		unsigned nZMinStartIdx = nStartIdx;
		for (size_t i = 0; i <= 10; i++)
		{
			unsigned idx = (nStartIdx - 5 + nSize + i) % nSize;
			if (vCutVertices[idx].z < vCutVertices[nZMinStartIdx].z)
				nZMinStartIdx = idx;
		}
		nStartIdx = nZMinStartIdx;

		VFVECTOR3 inDir = vCutVertices[nStartIdx] - vCutVertices[(nStartIdx + 1) % nSize];
		inDir.Normalize();
		//inDir = (inDir * 0.8f + (-input.btmRay.GetDirection()) * 0.2f);
		inDir.Normalize();
		//inDir = -input.btmRay.GetDirection();
		m_vCutVertices[0] = vCutVertices[nStartIdx] + inDir * 2.0f;
		m_vCutNormals[0] = vCutNormals[nStartIdx];
		for (unsigned i = nStartIdx; i < nSize + nStartIdx; i++)
		{
			m_vCutVertices[i - nStartIdx + 1] = vCutVertices[i % nSize];
			m_vCutNormals[i - nStartIdx + 1] = vCutNormals[i%nSize];
		}
		
		m_vCutVertices[nSize + 1] = vCutVertices[nStartIdx];
		m_vCutNormals[nSize + 1] = vCutNormals[nStartIdx];

		inDir = vCutVertices[nStartIdx] - vCutVertices[(nStartIdx + nSize - 1) % nSize];
		inDir.Normalize();
		//inDir = (inDir  * 0.8f + (-input.btmRay.GetDirection()) * 0.2f);
		inDir.Normalize();
		//inDir = -input.btmRay.GetDirection();
		m_vCutVertices[nSize + 2] = vCutVertices[nStartIdx] + inDir * 2.0f;
		m_vCutNormals[nSize + 2] = vCutNormals[nStartIdx];

		pathInfo.cbCutVertices = VD_V2CB(m_vCutVertices);
		pathInfo.cbCutNormals = VD_V2CB(m_vCutNormals);
#if 0
		{
			OBJWriteSimpleMesh("D:/testdata/gummesh.obj", input.gumMesh);
			std::ofstream objF("D:/testdata/frontvertices.obj");
			for (size_t i = 0; i < cbFrontVertices.len; i++)
			{
				objF << cbFrontVertices.pData[i] << std::endl;
			}
			objF.close();
			objF.open("D:/testdata/bkgvertices.obj");
			for (size_t i = 0; i < input.cbBottomLine.len; i++)
			{
				objF << input.cbBottomLine.pData[i] + 1 << std::endl;
			}
			objF.close();
			objF.open("D:/testdata/gumgumline.obj");
			for (size_t i = 0; i < input.cbGumGumLine.len; i++)
			{
				for (size_t j = 0; j < input.cbGumGumLine.pData[i].len; j++)
				{
					objF << input.cbGumGumLine.pData[i].pData[j] << std::endl;
				}
			}
			objF.close();
			objF.open("D:/testdata/bottomline.obj");
			for (size_t i = 0; i < input.cbBottomLine.len; i++)
			{
				objF << input.cbBottomLine.pData[i] << std::endl;
			}
			objF.close();
			OBJWriteVertices(pathInfo.cbCutVertices, "D:/testdata/cutvertices.obj");
		}
#endif
		VD_F_PROFILE_FINISH(VSCutPathGenerator::Gen);
	}

	void VSCutPathGenerator::GenAttCutPath(VSCutPathInfo& pathInfo, const VFRay& ray, const VSSimpleMeshF& simpMesh,
		NMALG_MESHCONVEXHULL::IVMeshConvexHull* pMeshConvexHull)
	{
		m_vCutVertices.resize(0);
		m_vCutNormals.resize(0);
		pathInfo.blValid = CUTPATH_GEN_FAIL_FIND;
		if (NULL == pMeshConvexHull)
			return;

		// 1. Create plane
		VFPlane plane(ray.GetDirection(), ray.GetOrigin());

		// 2. Project mesh simpMesh vertices to the plane
		std::vector<VFVECTOR3> vVerts(simpMesh.nVertCount);
		for (size_t i = 0; i < simpMesh.nVertCount; i++)
		{
			vVerts[i] = plane.projectPoint(simpMesh.pVertices[i]);
		}

		// 3. Calculate the convex hull of the project vertices 
		NMALG_MESHCONVEXHULL::VSConvexHullOutput output;
		NMALG_MESHCONVEXHULL::VSConvexHullInput input;
		input.cbVertices = VD_V2CB(vVerts);
		input.projPlane = NULL;
		input.uiDimension = NMALG_MESHCONVEXHULL::Dimension2;
		pMeshConvexHull->Build(output, input);
		if (0 == output.simpMesh.nVertCount)
			return;

		VFVECTOR3 cutNorm = ray.GetDirection();
		{
			float flMaxRatio = INIGetFloat(L"CutPathMaxRatio", L"./config.ini");//6.0f;
			float flMinRatio = INIGetFloat(L"CutPathMinRatio", L"./config.ini");//6.0f;
			VFRay ray_tmp(VFVECTOR3::ZERO, VFVECTOR3::AXIS_Z);

			float flNzLen = ray_tmp.GetUnitsLenth(cutNorm);
			VFVECTOR3 v1 = cutNorm + ray_tmp.GetDirection() * (-flNzLen);
			float flNxyLen = v1.Magnitude();
			if (flNxyLen * flMinRatio > flNzLen)
			{
				cutNorm = v1 + ray_tmp.GetDirection() * flNxyLen * flMinRatio;
				cutNorm.Normalize();
			}
		}

		m_vCutVertices.resize(output.simpMesh.nVertCount);
		m_vCutNormals.resize(output.simpMesh.nVertCount, cutNorm);
		VFVECTOR3 cent(VFVECTOR3::ZERO);
		for (size_t i = 0; i < output.simpMesh.nVertCount; i++)
		{
			cent += output.simpMesh.pVertices[i];
		}
		cent /= (float)output.simpMesh.nVertCount;
		for (size_t i = 0; i < output.simpMesh.nVertCount; i++)
		{
			VFVECTOR3 dir = output.simpMesh.pVertices[i] - cent;
			dir.Normalize();
			m_vCutVertices[i] = output.simpMesh.pVertices[i] + dir * 0.5f;
		}

		pathInfo.cbCutVertices = VD_V2CB(m_vCutVertices);
		pathInfo.cbCutNormals = VD_V2CB(m_vCutNormals);
		pathInfo.blValid = CUTPATH_GEN_OK;
		return;
	}

	void VSCutPathGenerator::OptimizeCutPath(std::vector<VFVECTOR3>& outPath, std::vector<unsigned>& vOptimizeVertIndex,
		const VSConstBuffer<unsigned>& cbVertIndex, const VSConstBuffer<VFVECTOR3>& cbCutPath)
	{
		if (0 == cbCutPath.len)
			return;

		const unsigned cnVertCount = 200;
		int nVertCount = cbCutPath.len;
		if (nVertCount > cnVertCount)
		{
			std::vector<float> vEdgeLen(nVertCount + 1);

			vEdgeLen[0] = 0.0f;

			for (int nVIdx = 1; nVIdx <= nVertCount; nVIdx++)
			{
				float flVal = (cbCutPath.pData[nVIdx - 1] - cbCutPath.pData[nVIdx % nVertCount]).SqrMagnitude();
				vEdgeLen[nVIdx] = vEdgeLen[nVIdx - 1] + flVal;
			}
			float flStep = vEdgeLen[nVertCount] / (float)cnVertCount;

			outPath.resize(cnVertCount);
			vOptimizeVertIndex.resize(cnVertCount);
			outPath[0] = cbCutPath.pData[0];
			unsigned nOrgVIdx = 1;
			for (int nCutVCount = 1; nCutVCount < cnVertCount; nCutVCount++)
			{
				float flVal = flStep * nCutVCount;
				for (; nOrgVIdx < nVertCount; nOrgVIdx++)
				{
					if (flVal - vEdgeLen[nOrgVIdx] < 0.0f)
					{
						outPath[nCutVCount] = cbCutPath.pData[nOrgVIdx];
						vOptimizeVertIndex[nCutVCount] = cbVertIndex.pData[nOrgVIdx];
						if (std::abs(vEdgeLen[nOrgVIdx] - flVal) > std::abs(vEdgeLen[nOrgVIdx - 1] - flVal))
						{
							outPath[nCutVCount] = cbCutPath.pData[nOrgVIdx - 1];
							vOptimizeVertIndex[nCutVCount] = cbVertIndex.pData[nOrgVIdx - 1];
						}
						break;
					}
				}
			}
		}
		else
		{			
			outPath.resize(nVertCount);
			vOptimizeVertIndex.resize(nVertCount);
			std::memcpy(&outPath[0], cbCutPath.pData, sizeof(VFVECTOR3) * nVertCount);
			std::memcpy(&vOptimizeVertIndex[0], cbVertIndex.pData, sizeof(unsigned) * nVertCount);
		}
	}
//#define DUMP_DEBUG
	bool VSCutPathGenerator::CalcVertNormals(std::vector<VFVECTOR3>& vCutNormals,
		const VSConstBuffer<VFVECTOR3>& cbCutVertices,
		const VSConstBuffer<unsigned>& cbVertIndex, const VSPerfectMesh& perfMesh,
		const VSCutPathGenInput& input)
	{
		auto MergeMesh = [](NM_PMMESH::VSMesh& mesh, const VSSimpleMeshF& sm) -> void
		{
			unsigned surfCnt = mesh.vSurface.size();
			unsigned vertCnt = mesh.vVertice.size();
			mesh.vSurface.resize(surfCnt + sm.nTriangleCount);
			mesh.vVertice.resize(vertCnt + sm.nVertCount);
			for (size_t i = 0; i < sm.nTriangleCount; i++)
			{
				for (size_t j = 0; j < 3; j++)
					mesh.vSurface[surfCnt + i][j] = sm.pTriangles[i][j] + vertCnt;
			}
			for (size_t i = 0; i < sm.nVertCount; i++)
				mesh.vVertice[vertCnt + i] = sm.pVertices[i];
		};

		TVExtSource< VNALGMESH::VRKNearestNeighbor > extNearestNbr;
		TVExtSource< VNALGMESH::VRKDTree > extKDTree;
		TVR2B< VNALGMESH::VRKNearestNeighbor > r2bNearestNbr(*extNearestNbr);
		TVR2B< VNALGMESH::VRKDTree > r2bKDTree(*extKDTree);
		VNALGMESH::VSKDTree m_kdTree;

		NM_PMMESH::VSMesh toothMesh;
		std::vector<unsigned> vertToTeethMap;
		size_t vaildVertCnt = 0;
		for (size_t i = 0; i < input.teethMesh.len; i++)
		{
			MergeMesh(toothMesh, input.teethMesh.pData[i]);
			vertToTeethMap.insert(vertToTeethMap.end(), input.teethMesh.pData[i].nVertCount, i);
			vaildVertCnt += input.teethMesh.pData[i].nVertCount;
		}
		for (size_t i = 0; i < input.attMesh.len; i++)
		{
			MergeMesh(toothMesh, input.attMesh.pData[i]);
			vertToTeethMap.insert(vertToTeethMap.end(), input.attMesh.pData[i].nVertCount, input.attMeshInTeeth.pData[i]);
			vaildVertCnt += input.attMesh.pData[i].nVertCount;
		}
		for (size_t i = 0; i < input.waxMesh.len; i++)
		{
			MergeMesh(toothMesh, input.waxMesh.pData[i]);
			vertToTeethMap.insert(vertToTeethMap.end(), input.waxMesh.pData[i].nVertCount, input.waxMeshInTeeth.pData[i]);
		}

		VSSimpleMeshF toothSimpMesh;
		PMGetSimpMesh(toothSimpMesh, toothMesh);

		VSConstBuffer<VFVECTOR3> cbVertices;
		cbVertices.len = toothMesh.vVertice.size();
		cbVertices.pData = &toothMesh.vVertice[0];
		VD_F_PROFILE_START(VSCutPathGenerator::r2bKDTree.Build);
		r2bKDTree.Build(m_kdTree, cbVertices);
		VD_F_PROFILE_FINISH(VSCutPathGenerator::r2bKDTree.Build);
		VSPerfectMesh teethPerfMesh;
		VD_F_PROFILE_START(VSCutPathGenerator::GETPERFECTMESH);
		GETPERFECTMESH(teethPerfMesh, toothSimpMesh, extPerfSys, rbPerf);
		VD_F_PROFILE_FINISH(VSCutPathGenerator::GETPERFECTMESH);
		unsigned nSize = teethPerfMesh.GetVertexCount();
		std::vector<VFVECTOR3> vTmpNormals(nSize, VFVECTOR3::ZERO);
		std::vector<VFVECTOR3> vRsltNormals(nSize, VFVECTOR3::ZERO);
		std::memcpy(&vTmpNormals[0], teethPerfMesh.pVertNormals, sizeof(VFVECTOR3) * nSize);
		const VFVECTOR3& dir = input.btmRay.GetDirection();
		VFRay ray;
		ray.SetOrigin(VFVECTOR3::ZERO);
		ray.SetDirection(dir);
		VD_F_PROFILE_START(VSCutPathGenerator::SmoothNormals1);
		unsigned nFlag = 0;
		VFVECTOR3* pNormal[2];
		pNormal[0] = &vTmpNormals[0];
		pNormal[1] = &vRsltNormals[0];
		std::vector < std::vector<unsigned> > vvDom(nSize);
		// 1. 单个法向量光滑，及牙龈法线光滑		
		unsigned nFlagRslt = 1 - nFlag;
		for (size_t nVIdx = 0; nVIdx < nSize; nVIdx++)
		{			
			PMGet1OrderDomain(vvDom[nVIdx], nVIdx, teethPerfMesh);
			pNormal[nFlagRslt][nVIdx] = pNormal[nFlag][nVIdx];

			for (size_t i = 0; i < vvDom[nVIdx].size(); i++)
			{
				pNormal[1 - nFlag][nVIdx] += pNormal[nFlag][vvDom[nVIdx][i]];
			}

			pNormal[nFlagRslt][nVIdx] /= (float)(vvDom[nVIdx].size() + 1);
			pNormal[nFlagRslt][nVIdx].Normalize();			
		}
		nFlag = 1 - nFlag;
		for (size_t nLoop = 0; nLoop < 5; nLoop++)
		{
			nFlagRslt = 1 - nFlag;
			for (size_t nVIdx = 0; nVIdx < nSize; nVIdx++)
			{				
				unsigned nFlagRslt = 1 - nFlag;				
				pNormal[nFlagRslt][nVIdx] = pNormal[nFlag][nVIdx];
				for (size_t i = 0; i < vvDom[nVIdx].size(); i++)
				{
					pNormal[1 - nFlag][nVIdx] += pNormal[nFlag][vvDom[nVIdx][i]];
				}

				pNormal[nFlagRslt][nVIdx] /= (float)(vvDom[nVIdx].size() + 1);
				pNormal[nFlagRslt][nVIdx].Normalize();
				nFlag = 1 - nFlag;
			}
			nFlag = 1 - nFlag;
		}
		if (0 == nFlag)
		{
			vRsltNormals.swap(vTmpNormals);
		}
		VD_F_PROFILE_FINISH(VSCutPathGenerator::SmoothNormals1);
		// 2. 路径法向量光滑
		VD_F_PROFILE_START(VSCutPathGenerator::SmoothNormals2);
		nSize = cbCutVertices.len;
		vTmpNormals.resize(nSize);
		std::vector<std::pair<unsigned, float> > vNearestPointPair(nSize);
		for (size_t i = 0; i < nSize; i++)
		{
			const VFVECTOR3& v0 = cbCutVertices.pData[i];
			
			r2bNearestNbr.Build(vNearestPointPair[i], m_kdTree, cbVertices, v0);
			vTmpNormals[i] = vRsltNormals[vNearestPointPair[i].first];
		}

		vCutNormals.resize(nSize);
#ifdef DUMP_DEBUG
		{
			std::ofstream objF("E:/testdata/cutnormal0.obj");
			for (size_t i = 0; i < vTmpNormals.size(); i++)
			{
				VFRay ray(cbCutVertices.pData[i], vTmpNormals[i]);
				for (size_t j = 0; j < 10; j++)
				{
					VFVECTOR3 v = ray.GetPoint(((float)j) * 0.5f);
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
			}
			objF.close();
			objF.open("E:/testdata/teethaxis.obj");
			for (size_t i = 0; i < input.cbTeethCuspDir.len; i++)
			{	
				for (size_t j = 0; j < 10; j++)
				{
					VFVECTOR3 v = input.cbTeethCuspDir.pData[i].GetPoint(((float)j) * 0.5f);
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
			}
			objF.close();
		}
#endif
		for (size_t cutIdx = 0; cutIdx < vNearestPointPair.size(); cutIdx++) {
			auto& cutNorm = vTmpNormals[cutIdx];
			const auto& cutVert = cbCutVertices.pData[cutIdx];
			const auto& teethIdx = vertToTeethMap[vNearestPointPair[cutIdx].first];
			const auto& teethOri = input.cbTeethCuspDir.pData[teethIdx].GetOrigin();
			const auto& teethDir = input.cbTeethCuspDir.pData[teethIdx].GetDirection();

			if (vNearestPointPair[cutIdx].first >= vaildVertCnt && teethIdx < input.cbTeethCuspDir.len) {
				const auto& nextOri = input.cbTeethCuspDir.pData[teethIdx + 1].GetOrigin();
				const auto& nextDir = input.cbTeethCuspDir.pData[teethIdx + 1].GetDirection();
				const auto ori = (teethOri + nextOri) / 2.0f;
				auto dir = teethDir + nextDir;
				dir.Normalize();
				if ((cutVert - ori).Cross(dir).Dot(cutNorm.Cross(dir)) < 0)
					cutNorm = teethDir;
				continue;
			}
			if ((cutVert - teethOri).Cross(teethDir).Dot(cutNorm.Cross(teethDir)) < 0)
				cutNorm = teethDir;
		}
				
		pNormal[0] = &vTmpNormals[0];
		pNormal[1] = &vCutNormals[0];
		nFlag = 0;
#ifdef DUMP_DEBUG
		{
			std::ofstream objF("E:/testdata/cutnormal1.obj");
			for (size_t i = 0; i < vTmpNormals.size(); i++)
			{
				VFRay ray(cbCutVertices.pData[i], vTmpNormals[i]);
				for (size_t j = 0; j < 10; j++)
				{
					VFVECTOR3 v = ray.GetPoint(((float)j) * 0.5f);
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
			}
			objF.close();
		}
#endif
		for (size_t nLoop = 0; nLoop < 10; nLoop++)
		{
			nFlagRslt = 1 - nFlag;
			for (size_t i = 0; i < nSize; i++)
			{	
				float flNzLen = ray.GetUnitsLenth(vTmpNormals[i]);
				VFVECTOR3 v1 = pNormal[nFlag][i] + dir * (-flNzLen);
				float flNxyLen = v1.Magnitude();
				if (flNxyLen * m_flMinRatio > flNzLen)
				{
					pNormal[nFlag][i] = v1 + dir * flNxyLen * m_flMinRatio;
					pNormal[nFlag][i].Normalize();
				}
				//if(flNzLen / flNxyLen >  flMaxRatio)
				//{
				//	vTmpNormals[i] = v1 + dir * flNxyLen * flMaxRatio;
				//	vTmpNormals[i].Normalize();
				//}
			}
			
			for (size_t i = 0; i < nSize; i++)
			{
				pNormal[nFlagRslt][(i + 2) % nSize] = pNormal[nFlag][i] * 0.1f + pNormal[nFlag][(i + 1) % nSize] * 0.2f +
					pNormal[nFlag][(i + 2) % nSize] * 0.4f + pNormal[nFlag][(i + 3) % nSize] * 0.2f + pNormal[nFlag][(i + 4) % nSize] * 0.1f;
				pNormal[nFlagRslt][(i + 2) % nSize].Normalize();
			}
			
			nFlag = 1 - nFlag;
		}		
		if (0 == nFlag)
		{
			vCutNormals.swap(vTmpNormals);
		}
#ifdef DUMP_DEBUG
		{
			std::ofstream objF("E:/testdata/cutnormal2.obj");
			for (size_t i = 0; i < vCutNormals.size(); i++)
			{
				VFRay ray(cbCutVertices.pData[i], vCutNormals[i]);
				for (size_t j = 0; j < 10; j++)
				{
					VFVECTOR3 v = ray.GetPoint(((float)j) * 0.5f);
					objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
				}
			}
			objF.close();
		}
#endif
		VD_F_PROFILE_FINISH(VSCutPathGenerator::SmoothNormals2);

		VFVECTOR3 toothCenter = VFVECTOR3::ZERO;
		size_t toothVertCnt = 0;
		for (size_t ti = 0; ti < input.teethMesh.len; ti++)
		{
			toothVertCnt += input.teethMesh.pData[ti].nVertCount;
			for (size_t tvi = 0; tvi < input.teethMesh.pData[ti].nVertCount; tvi++)
				toothCenter += input.teethMesh.pData[ti].pVertices[tvi];
		}
		toothCenter /= toothVertCnt;
		std::unordered_map<size_t, std::pair<size_t, float>> teethLowestVertMap;

		for (size_t cutIdx = 0; cutIdx < vNearestPointPair.size(); cutIdx++) {
			const auto& cutNorm = vCutNormals[cutIdx];
			const auto& cutVert = cbCutVertices.pData[cutIdx];
			const auto& teethIdx = vertToTeethMap[vNearestPointPair[cutIdx].first];

			for (size_t ti = 0; ti < vertToTeethMap.size(); ti++) {
				if (vertToTeethMap[ti] != teethIdx)
					continue;
				auto v0 = toothSimpMesh.pVertices[ti] - cutVert;
				if (v0.Magnitude() < 1.5f)
					continue;
				v0.Normalize();
				if (cutNorm.Dot(v0) > cos(5.0f * (VF_PI / 180.0f)))
				{
#ifdef DUMP_DEBUG
					std::ofstream objF("E:/testdata/cutnormal.obj");
					for (size_t i = 0; i < vCutNormals.size(); i++)
					{
						VFRay ray(cbCutVertices.pData[i], vCutNormals[i]);
						for (size_t j = 0; j < 10; j++)
						{
							VFVECTOR3 v = ray.GetPoint(((float)j) * 0.5f);
							objF << "v " << v.x << " " << v.y << " " << v.z << std::endl;
						}
					}
					objF.close();
#endif
					return false;
				}
					
			}

			VFRay ray(cutVert, cutNorm);
			for (size_t attidx = 0; attidx < input.attMeshInTeeth.len; attidx++) {
				if (input.attMeshInTeeth.pData[attidx] != teethIdx)
					continue;
				if (::PMGetRayIntersect(input.attMesh.pData[attidx], ray) > 0)
					return false;
			}

			if (vCutNormals[cutIdx].Dot(cbCutVertices.pData[cutIdx] - toothCenter) > 0) {
				const float dist = input.cbTeethCuspDir.pData[teethIdx].GetUnitsLenth(cbCutVertices.pData[cutIdx]);
				if (teethLowestVertMap.count(teethIdx) == 0)
					teethLowestVertMap[teethIdx] = std::make_pair(cutIdx, dist);
				else if (teethLowestVertMap[teethIdx].second > dist)
					teethLowestVertMap[teethIdx] = std::make_pair(cutIdx, dist);
			}
		}
		
		for (const auto& tv : teethLowestVertMap) {
			VFPlane plane(input.cbTeethCuspDir.pData[tv.first].GetDirection(), cbCutVertices.pData[tv.second.first]);
			for (size_t ai = 0; ai < input.attMeshInTeeth.len; ai++) {
				if (input.attMeshInTeeth.pData[ai] != tv.first)
					continue;
				for (size_t i = 0; i < input.attMesh.pData[ai].nVertCount; i++) {
					if (plane.getDistance(input.attMesh.pData[ai].pVertices[i]) <= 0.0f)
						return false;
				}
			}
		}
		
		return true;
	}
}
