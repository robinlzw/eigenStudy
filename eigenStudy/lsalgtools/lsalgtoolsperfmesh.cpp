#include "stdafx.h"
#include "lsalgtools/ilsalgtools.h"
#include "lsalgtools/ilsalgtoolsperfmesh.h"
#include "pkg/ldwzkupdater.h"
#include "pkg/ldck4cce.h"

#include <map>

namespace NM_PMMESH
{
	

}


// PMGetSimpMesh()——获取输入VSMesh网格对应的VSSimpleMeshF，其中不发生数据的拷贝。
void PMGetSimpMesh(VSSimpleMeshF& out, const NM_PMMESH::VSMesh& mesh)
{
	if (mesh.vVertice.size() > 0)
	{
		out.pVertices = &mesh.vVertice[0];
		out.nVertCount = mesh.vVertice.size();
	}
	if (mesh.vSurface.size() > 0)
	{
		out.pTriangles = &mesh.vSurface[0];
		out.nTriangleCount = mesh.vSurface.size();
	}
}



// PMCopyMesh()——拷贝VSMesh网格对象
void PMCopyMesh(NM_PMMESH::VSMesh& out, const NM_PMMESH::VSMesh& in)
{
	if (in.vVertice.size() > 0)
	{
		out.vVertice.resize(in.vVertice.size());
		std::memcpy(&out.vVertice[0], &in.vVertice[0], sizeof(VFVECTOR3) * in.vVertice.size());
	}
	if (in.vSurface.size() > 0)
	{
		out.vSurface.resize(in.vSurface.size());
		std::memcpy(&out.vSurface[0], &in.vSurface[0], sizeof(VNVECTOR3UI) * in.vSurface.size());
	}
}


// PMCopyMesh()——从simpleMesh所指的网格对象中拷贝数据到VSMesh对象中。
void PMCopyMesh(NM_PMMESH::VSMesh& out, const VSSimpleMeshF& in)
{
	if (in.nVertCount > 0)
	{
		out.vVertice.resize(in.nVertCount);
		std::memcpy(&out.vVertice[0], in.pVertices, sizeof(VFVECTOR3) * in.nVertCount);
	}
	if (in.nTriangleCount > 0)
	{	
		out.vSurface.resize(in.nTriangleCount);
		std::memcpy(&out.vSurface[0], in.pTriangles, sizeof(VNVECTOR3UI) * in.nTriangleCount);
	}
}



// PMGetCircle2Dim——获得二维环形点集，输入点数，环半径。
void PMGetCircle2Dim(std::vector<VFVECTOR2>&out, const unsigned nPointCnt, const float flRadius)
{
	if (nPointCnt<3)
		return;

	out.resize(nPointCnt);
	float flStep = VF_DBL_PI / (float)nPointCnt;
	for (unsigned i=0; i<nPointCnt;i++)
	{
		float flAngle = (float)i * flStep;
		out[i].x = std::cos(flAngle)*flRadius;
		out[i].y = std::sin(flAngle)*flRadius;
	}
}


// PMMakeLaplaceMatrix()——求出网格对应的laplace矩阵，输入perfectMesh对象，
void PMMakeLaplaceMatrix(std::vector<VSTripletD>& matLaplace, const VSPerfectMesh& pm, const double dbW)
{
	for (unsigned nVIdx = 0; nVIdx < pm.GetVertexCount(); nVIdx++)
	{
		std::vector<unsigned> vDomain;
		PMGet1OrderDomain(vDomain, nVIdx, pm);
		
		double dbVal = -1.0 / (double)(vDomain.size()) * dbW;
		VSTripletD trip;
		trip.row = trip.col = nVIdx;		
		trip.val = 1.0 * dbW;
		matLaplace.push_back(trip);
		for (unsigned nEIdx = 0; nEIdx < vDomain.size(); nEIdx++)
		{
			trip.col = vDomain[nEIdx];
			trip.val = dbVal;
			matLaplace.push_back(trip);
		}
	}
}



void PMGet1OrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfectMesh& pm)
{
	unsigned nEdgeNum = pm.GetEdgeCountOfVetex(nVIdx);	
	domainOut.resize(nEdgeNum);
	for (unsigned nEIdx = 0; nEIdx < nEdgeNum; nEIdx++)
	{
		VTopoGraph::VOE voe = pm.GetEdgeOfVertex(nVIdx, nEIdx).Opposite();
		unsigned nOppoV = pm.GetVertIndxOfEdge(voe.edgeIndx)[voe.vertInEdge];
		domainOut[nEIdx] = nOppoV;
	}
}


void PMGet1OrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfTopoGraph& pg)
{
	unsigned nEdgeNum = pg.GetEdgeCountOfVetex(nVIdx);
	domainOut.resize(nEdgeNum);
	for (unsigned nEIdx = 0; nEIdx < nEdgeNum; nEIdx++)
	{
		VTopoGraph::VOE voe = pg.GetEdgeOfVertex(nVIdx, nEIdx).Opposite();
		unsigned nOppoV = pg.GetVertIndxOfEdge(voe.edgeIndx)[voe.vertInEdge];
		domainOut[nEIdx] = nOppoV;
	}
}

void PMGetnOrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfectMesh& pm, const unsigned n)
{
	PMGetnOrderDomain(domainOut, nVIdx, *((VSPerfTopoGraph*)(&pm)), n);
}


void PMGetnOrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfTopoGraph& pg, const unsigned n)
{
	std::vector<unsigned> vFlag(pg.GetVertexCount(), 0);
	int nLoop = n;
	std::vector<unsigned> vLoop[2];
	int nSwitch = 0;
	vLoop[nSwitch].resize(1, nVIdx);
	vFlag[nVIdx] = 1;
	while (nLoop-- > 0)
	{
		vLoop[1 - nSwitch].clear();
		for (unsigned i = 0; i < vLoop[nSwitch].size(); i++)
		{
			std::vector<unsigned> vDomain;
			PMGet1OrderDomain(vDomain, vLoop[nSwitch][i], pg);
			for (size_t j = 0; j < vDomain.size(); j++)
			{
				if (0 == vFlag[vDomain[j]])
				{
					vFlag[vDomain[j]] = 1;
					vLoop[1 - nSwitch].push_back(vDomain[j]);
				}
			}
		}
		domainOut.insert(domainOut.end(), vLoop[1 - nSwitch].begin(), vLoop[1 - nSwitch].end());
		nSwitch = 1 - nSwitch;
	}
}

double PMGetAvgEdgeLen(const VSPerfectMesh& pm)
{
	double dbAvgLen = 0;
	for (size_t nEdgeIdx = 0; nEdgeIdx < pm.GetEdgeCount(); nEdgeIdx++)
	{
		VNVECTOR2UI voe = pm.GetVertIndxOfEdge(nEdgeIdx);
		dbAvgLen += (pm.pVertices[voe.x] - pm.pVertices[voe.y]).Magnitude();
	}
	dbAvgLen /= ((double)pm.GetEdgeCount());
	return dbAvgLen;
}

void PMMeshGetSimpleMesh(VSSimpleMeshF& simpMesh, const NM_PMMESH::VSMesh& mesh)
{
	simpMesh.pVertices = &mesh.vVertice[0];
	simpMesh.pTriangles = &mesh.vSurface[0];
	simpMesh.nVertCount = mesh.vVertice.size();
	simpMesh.nTriangleCount = mesh.vSurface.size();
}
//#define ADD_BY_EDGE
#define ADD_BY_TRIANGLE_AVG			// 最大边长，平均等分, 连接增加点与对顶点的连线。
#define ADD_BY_TRIANGLE_HALF		// 最大边长，取中点，连接中点与对顶点。
#define ADD_BY_TRIANGLE_HALF_ANGLE	// 最大边长，取中点, 边的对角角度小于45度，则不添加此边上的点。
#define ADD_BY_TRIANGLE_SHAPE		// 根据三角片的形状来细分
#define MAX_LOOP_SIZE 50
// 给定三维网格数据，根据flPrecision的精度，新增顶点和三角片，使每个三角片的边长不超过flPrecision。
void PMMeshRefining(NM_PMMESH::VSMesh& out, const VSSimpleMeshF& simpMesh, const float flPrecision)
{	
#ifdef ADD_BY_EDGE
	float flThreshold = flPrecision * flPrecision;
	bool blExit = false;
	unsigned VC = simpMesh.nVertCount;
	unsigned SC = simpMesh.nTriangleCount;
	out.vSurface.resize(SC);
	out.vVertice.resize(VC);
	std::memcpy(&out.vSurface[0], simpMesh.pTriangles, sizeof(VNVECTOR3UI) * SC);
	std::memcpy(&out.vVertice[0], simpMesh.pVertices, sizeof(VFVECTOR3) * VC);
	VSSimpleMeshF simpMeshTmp;
	PMMeshGetSimpleMesh(simpMeshTmp, out);
	NM_PMMESH::VSMesh meshTmp;
	
	char szFileName[256] = { 0 };
	unsigned nLoopCount = 0;

	while (true)
	{
		blExit = true;
		VSPerfectMesh perfMesh;
		GETPERFECTMESH(perfMesh, simpMeshTmp, extSys1, rb1);

		unsigned nEdgeCount = perfMesh.GetEdgeCount();
		unsigned nSurfCount = perfMesh.GetSurfCount();
		unsigned nVertCount = perfMesh.GetVertexCount();
		std::vector<unsigned> vEdgeFlag(nEdgeCount, 0);
		std::vector<unsigned> vSurfFlag(nSurfCount, 0);
		std::vector<VNVECTOR3UI> vNewSurfs;
		std::vector<VFVECTOR3> vNewVerts;
		unsigned nNewVertCount = 0;
		for (size_t nEdgeIdx = 0; nEdgeIdx < nEdgeCount; nEdgeIdx++)
		{
			if (1 == vEdgeFlag[nEdgeIdx])
				continue;
			VNVECTOR2UI vertIdxOfEdge = perfMesh.GetVertIndxOfEdge(nEdgeIdx);
			VFVECTOR3 vDif = perfMesh.pVertices[vertIdxOfEdge.y] - perfMesh.pVertices[vertIdxOfEdge.x];
			float flDist = vDif.SqrMagnitude();
			if (flDist > flThreshold)
			{
				int nCount = sqrt(flDist) / flPrecision;
				if (nCount <= 0)
					continue;
				
				vDif = vDif / (nCount + 1);
				blExit = false;
				vNewVerts.resize(nNewVertCount + nCount);
				const VFVECTOR3& vStart = perfMesh.pVertices[vertIdxOfEdge.x];
				VNVECTOR2UI surfIdxOfEdge = perfMesh.GetSurfIndxOfEdge(nEdgeIdx);
				VNVECTOR2UI edgeIdxInSurf = perfMesh.GetSurfNbrIndxOfEdge(nEdgeIdx);
				for (size_t i = 1; i <= nCount; i++)
				{
					vNewVerts[nNewVertCount + i - 1] = vStart + vDif * i;
				}
				unsigned nShift = nVertCount + nNewVertCount;
				if (VD_INVALID_INDEX != surfIdxOfEdge.x)
				{
					vSurfFlag[surfIdxOfEdge.x] = 1;
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.x];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, nShift, s[edgeIdxInSurf.x]));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i, nShift + i + 1, s[edgeIdxInSurf.x]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, s[edgeIdxInSurf.x], nShift + nCount - 1));
					VNVECTOR3UI soe = perfMesh.GetEdgeIndxOfSurf(surfIdxOfEdge.x);
					for (size_t i = 0; i < 3; i++)
					{
						vEdgeFlag[soe[i]] = 1;
					}
				}
			
				if (VD_INVALID_INDEX != surfIdxOfEdge.y)
				{
					vSurfFlag[surfIdxOfEdge.y] = 1;
					unsigned nShift = nVertCount + nNewVertCount;
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.y];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, s[edgeIdxInSurf.y], nShift));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i + 1, nShift + i, s[edgeIdxInSurf.y]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, nShift + nCount - 1, s[edgeIdxInSurf.y]));
					VNVECTOR3UI soe = perfMesh.GetEdgeIndxOfSurf(surfIdxOfEdge.y);
					for (size_t i = 0; i < 3; i++)
					{
						vEdgeFlag[soe[i]] = 1;
					}
				}				
			}
			nNewVertCount = vNewVerts.size();
		}
		if ((true == blExit) || nLoopCount >= MAX_LOOP_SIZE)
			return;
		out.vVertice.resize(nVertCount + nNewVertCount);
		std::memcpy(&out.vVertice[nVertCount], &vNewVerts[0], sizeof(VFVECTOR3) * nNewVertCount);
		std::vector<VNVECTOR3UI> vSurfTmp(out.vSurface.begin(), out.vSurface.end());
		out.vSurface.resize(nSurfCount + vNewSurfs.size());
		nSurfCount = 0;
		for (size_t i = 0; i < vSurfTmp.size(); i++)
		{
			if (0 == vSurfFlag[i])
			{
				out.vSurface[nSurfCount++] = vSurfTmp[i];
			}
		}
		for (size_t i = 0; i < vNewSurfs.size(); i++)
		{
			out.vSurface[nSurfCount++] = vNewSurfs[i];
		}
		out.vSurface.resize(nSurfCount);
		PMMeshGetSimpleMesh(simpMeshTmp, out);
		nLoopCount++;
		sprintf_s(szFileName, 256, "D:/testdata/refinemeshbyedge_%d.obj", nLoopCount);
		OBJWriteSimpleMesh(szFileName, simpMeshTmp);
	};
#else
#ifdef ADD_BY_TRIANGLE_AVG
	float flThreshold = flPrecision * flPrecision;
	bool blExit = false;
	unsigned VC = simpMesh.nVertCount;
	unsigned SC = simpMesh.nTriangleCount;
	out.vSurface.resize(SC);
	out.vVertice.resize(VC);
	std::memcpy(&out.vSurface[0], simpMesh.pTriangles, sizeof(VNVECTOR3UI) * SC);
	std::memcpy(&out.vVertice[0], simpMesh.pVertices, sizeof(VFVECTOR3) * VC);
	VSSimpleMeshF simpMeshTmp;
	PMMeshGetSimpleMesh(simpMeshTmp, out);
	NM_PMMESH::VSMesh meshTmp;

	auto MarkSurfFlag = [](std::vector<unsigned>& vSurfFlag, const VSPerfectMesh& perfMesh, const unsigned nSIdx) {
		auto soe = perfMesh.GetEdgeIndxOfSurf(nSIdx);
		auto eis = perfMesh.GetEdgeNbrIndxOfSurf(nSIdx);
		vSurfFlag[nSIdx] = 1;
		for (size_t j = 0; j < 3; j++)
		{
			VNVECTOR2UI eos = perfMesh.GetSurfIndxOfEdge(soe[j]);
			unsigned ni = ((eis[j] + 1) & 0x01);
			unsigned sn = eos[ni];
			if (VD_INVALID_INDEX != sn)
				vSurfFlag[sn] = 1;			
		}
	};
		
	char szFileName[256] = { 0 };
	unsigned nLoopCount = 0;
	while (true)
	{
		blExit = true;
		VSPerfectMesh perfMesh;
		GETPERFECTMESH(perfMesh, simpMeshTmp, extSys1, rb1);

		unsigned nEdgeCount = perfMesh.GetEdgeCount();
		unsigned nSurfCount = perfMesh.GetSurfCount();
		unsigned nVertCount = perfMesh.GetVertexCount();
		std::vector<unsigned> vEdgeFlag(nEdgeCount, 0);
		std::vector<unsigned> vSurfFlag(nSurfCount, 0);
		std::vector<unsigned> vSurfRemoved(nSurfCount, 0);
		std::vector<VNVECTOR3UI> vNewSurfs;
		std::vector<VFVECTOR3> vNewVerts;
		std::vector<float> vEdgeLength(nEdgeCount, 0.0f);
		for (size_t nEdgeIdx = 0; nEdgeIdx < nEdgeCount; nEdgeIdx++)
		{
			VNVECTOR2UI eov = perfMesh.GetVertIndxOfEdge(nEdgeIdx);
			vEdgeLength[nEdgeIdx] = (perfMesh.pVertices[eov.y] - perfMesh.pVertices[eov.x]).SqrMagnitude();
		}
		unsigned nNewVertCount = 0;
		for (size_t nSurfIdx = 0; nSurfIdx < nSurfCount; nSurfIdx++)
		{
			if (1 == vSurfFlag[nSurfIdx])
				continue;
			VNVECTOR3UI edgeIdxOfSurf = perfMesh.GetEdgeIndxOfSurf(nSurfIdx);
			float flDist = vEdgeLength[edgeIdxOfSurf[0]];
			unsigned nEdgeIdx = edgeIdxOfSurf[0];
			for (size_t i = 1; i < 3; i++)
			{
				if (flDist < vEdgeLength[edgeIdxOfSurf[i]])
				{
					flDist = vEdgeLength[edgeIdxOfSurf[i]];
					nEdgeIdx = edgeIdxOfSurf[i];
				}
			}
			if (flDist > flThreshold)
			{
				int nCount = sqrt(flDist) / flPrecision;
				if (nCount <= 0)
					continue;

				VNVECTOR2UI vertIdxOfEdge = perfMesh.GetVertIndxOfEdge(nEdgeIdx);
				VFVECTOR3 vDif = perfMesh.pVertices[vertIdxOfEdge.y] - perfMesh.pVertices[vertIdxOfEdge.x];
#ifdef ADD_BY_TRIANGLE_HALF
				nCount = 1;
#endif
				vDif = vDif / (float)(nCount + 1);
				blExit = false;				
				const VFVECTOR3& vStart = perfMesh.pVertices[vertIdxOfEdge.x];
				VNVECTOR2UI surfIdxOfEdge = perfMesh.GetSurfIndxOfEdge(nEdgeIdx);
				VNVECTOR2UI edgeIdxInSurf = perfMesh.GetSurfNbrIndxOfEdge(nEdgeIdx);
#ifdef ADD_BY_TRIANGLE_HALF_ANGLE
				bool blDo = true;
				for (size_t i = 0; i < 2; i++)
				{
					if (VD_INVALID_INDEX != surfIdxOfEdge[i])
					{
						const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge[i]];
						const VFVECTOR3& v = perfMesh.pVertices[s[edgeIdxInSurf[i]]];
						VFVECTOR3 v1 = perfMesh.pVertices[vertIdxOfEdge.x] - v;
						VFVECTOR3 v2 = perfMesh.pVertices[vertIdxOfEdge.y] - v;
						v1.Normalize();
						v2.Normalize();
						float cosvalue = v1.Dot(v2);
						cosvalue = cosvalue < 1.0 ? cosvalue : 1.0f - FLT_EPSILON;
						cosvalue = cosvalue > -1.0 ? cosvalue : -1.0f + FLT_EPSILON;

						cosvalue = acos(cosvalue);
						if (cosvalue < VF_QUARTER_PI)
						{
							blDo = false;
						}
					}
				}
				if (false == blDo)
					continue;
#endif
				vNewVerts.resize(nNewVertCount + nCount);
				for (size_t i = 1; i <= nCount; i++)
				{
					vNewVerts[nNewVertCount + i - 1] = vStart + vDif * i;
				}
				unsigned nShift = nVertCount + nNewVertCount;
				if (VD_INVALID_INDEX != surfIdxOfEdge.x)
				{
					vSurfRemoved[surfIdxOfEdge.x] = 1;
					MarkSurfFlag(vSurfFlag, perfMesh, surfIdxOfEdge.x);
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.x];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, nShift, s[edgeIdxInSurf.x]));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i, nShift + i + 1, s[edgeIdxInSurf.x]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, s[edgeIdxInSurf.x], nShift + nCount - 1));
				}

				if (VD_INVALID_INDEX != surfIdxOfEdge.y)
				{
					vSurfRemoved[surfIdxOfEdge.y] = 1;
					MarkSurfFlag(vSurfFlag, perfMesh, surfIdxOfEdge.y);
					unsigned nShift = nVertCount + nNewVertCount;
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.y];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, s[edgeIdxInSurf.y], nShift));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i + 1, nShift + i, s[edgeIdxInSurf.y]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, nShift + nCount - 1, s[edgeIdxInSurf.y]));
					/*VNVECTOR3UI soe = perfMesh.GetEdgeIndxOfSurf(eos.y);
					for (size_t i = 0; i < 3; i++)
					{
						vEdgeFlag[soe[i]] = 1;
					}*/
				}
			}
			nNewVertCount = vNewVerts.size();
		}
		if ((true == blExit) || nLoopCount >= MAX_LOOP_SIZE)
			return;
		out.vVertice.resize(nVertCount + nNewVertCount);
		std::memcpy(&out.vVertice[nVertCount], &vNewVerts[0], sizeof(VFVECTOR3) * nNewVertCount);
		std::vector<VNVECTOR3UI> vSurfTmp(out.vSurface.begin(), out.vSurface.end());
		out.vSurface.resize(nSurfCount + vNewSurfs.size());
		nSurfCount = 0;
		for (size_t i = 0; i < vSurfTmp.size(); i++)
		{
			if (0 == vSurfRemoved[i])
			{
				out.vSurface[nSurfCount++] = vSurfTmp[i];
			}
		}
		for (size_t i = 0; i < vNewSurfs.size(); i++)
		{
			out.vSurface[nSurfCount++] = vNewSurfs[i];
		}
		out.vSurface.resize(nSurfCount);
		PMMeshGetSimpleMesh(simpMeshTmp, out);
		nLoopCount++;
		//sprintf_s(szFileName, 256, "D:/testdata/refinemeshbytriangle_%d.obj", nLoopCount);
		//OBJWriteSimpleMesh(szFileName, simpMeshTmp);
	};
#else
#ifdef ADD_BY_TRIANGLE_SHAPE
enum ETriShape
{
	ETriShapeOxygon = 1,
	ETriShapeObtuseSym = 2,
	ETriShapeObtuseAsym = 3,
};
struct VSTriShapeInfo
{
	ETriShape shape;
	unsigned nEdgeShortest;
	unsigned nEdgeLongest;
};
	float flCos45 = cos(VF_QUARTER_PI);
	float flThreshold = flPrecision * flPrecision;
	bool blExit = false;
	unsigned VC = simpMesh.nVertCount;
	unsigned SC = simpMesh.nTriangleCount;
	out.vSurface.resize(SC);
	out.vVertice.resize(VC);
	std::memcpy(&out.vSurface[0], simpMesh.pTriangles, sizeof(VNVECTOR3UI) * SC);
	std::memcpy(&out.vVertice[0], simpMesh.pVertices, sizeof(VFVECTOR3) * VC);
	VSSimpleMeshF simpMeshTmp;
	PMMeshGetSimpleMesh(simpMeshTmp, out);
	NM_PMMESH::VSMesh meshTmp;

	auto MarkSurfFlag = [](std::vector<unsigned>& vSurfFlag, const VSPerfectMesh& perfMesh, const unsigned nSIdx) {
		auto soe = perfMesh.GetEdgeIndxOfSurf(nSIdx);
		auto eis = perfMesh.GetEdgeNbrIndxOfSurf(nSIdx);
		vSurfFlag[nSIdx] = 1;
		for (size_t j = 0; j < 3; j++)
		{
			VNVECTOR2UI eos = perfMesh.GetSurfIndxOfEdge(soe[j]);
			unsigned ni = ((eis[j] + 1) & 0x01);
			unsigned sn = eos[ni];
			if (VD_INVALID_INDEX != sn)
				vSurfFlag[sn] = 1;
			else
				std::cout << "sn: " << sn << std::endl;
		}
	};

	auto GetTriangleShage = [](VSTriShapeInfo& triShapeInfo, const VSPerfectMesh& perfMesh, const VNVECTOR3UI& tri){
		const VFVECTOR3& v1 = perfMesh.pVertices[tri.x];
		const VFVECTOR3& v2 = perfMesh.pVertices[tri.y];
		const VFVECTOR3& v3 = perfMesh.pVertices[tri.z];
		VFVECTOR3 v12 = v2 - v1;
		VFVECTOR3 v23 = v3 - v2;
		VFVECTOR3 v31 = v1 - v3;
		v12.Normalize();
		v23.Normalize();
		v31.Normalize();
		float cosvalue1 = v12.Dot(-v31);
		float cosvalue2 = v23.Dot(-v12);
		float cosvalue3 = v31.Dot(-v23);
		cosvalue1 = cosvalue1 < 1.0 ? cosvalue1 : 1.0f - FLT_EPSILON;
		cosvalue1 = cosvalue1 > -1.0 ? cosvalue1 : -1.0f + FLT_EPSILON;
		cosvalue2 = cosvalue2 < 1.0 ? cosvalue2 : 1.0f - FLT_EPSILON;
		cosvalue2 = cosvalue2 > -1.0 ? cosvalue2 : -1.0f + FLT_EPSILON;
		cosvalue3 = cosvalue3 < 1.0 ? cosvalue3 : 1.0f - FLT_EPSILON;
		cosvalue3 = cosvalue3 > -1.0 ? cosvalue3 : -1.0f + FLT_EPSILON;


	};

	char szFileName[256] = { 0 };
	unsigned nLoopCount = 0;
	while (true)
	{
		blExit = true;
		VSPerfectMesh perfMesh;
		GETPERFECTMESH(perfMesh, simpMeshTmp, extSys1, rb1);

		unsigned nEdgeCount = perfMesh.GetEdgeCount();
		unsigned nSurfCount = perfMesh.GetSurfCount();
		unsigned nVertCount = perfMesh.GetVertexCount();
		std::vector<unsigned> vEdgeFlag(nEdgeCount, 0);
		std::vector<unsigned> vSurfFlag(nSurfCount, 0);
		std::vector<unsigned> vSurfRemoved(nSurfCount, 0);
		std::vector<VNVECTOR3UI> vNewSurfs;
		std::vector<VFVECTOR3> vNewVerts;
		std::vector<float> vEdgeLength(nEdgeCount, 0.0f);
		for (size_t nEdgeIdx = 0; nEdgeIdx < nEdgeCount; nEdgeIdx++)
		{
			VNVECTOR2UI eov = perfMesh.GetVertIndxOfEdge(nEdgeIdx);
			vEdgeLength[nEdgeIdx] = (perfMesh.pVertices[eov.y] - perfMesh.pVertices[eov.x]).SqrMagnitude();
		}
		unsigned nNewVertCount = 0;
		for (size_t nSurfIdx = 0; nSurfIdx < nSurfCount; nSurfIdx++)
		{
			if (1 == vSurfFlag[nSurfIdx])
				continue;
			VNVECTOR3UI edgeIdxOfSurf = perfMesh.GetEdgeIndxOfSurf(nSurfIdx);
			float flDist = vEdgeLength[edgeIdxOfSurf[0]];
			unsigned nEdgeIdx = edgeIdxOfSurf[0];
			for (size_t i = 1; i < 3; i++)
			{
				if (flDist < vEdgeLength[edgeIdxOfSurf[i]])
				{
					flDist = vEdgeLength[edgeIdxOfSurf[i]];
					nEdgeIdx = edgeIdxOfSurf[i];
				}
			}
			if (flDist > flThreshold)
			{
				int nCount = sqrt(flDist) / flPrecision;
				if (nCount <= 0)
					continue;

				VNVECTOR2UI vertIdxOfEdge = perfMesh.GetVertIndxOfEdge(nEdgeIdx);
				VFVECTOR3 vDif = perfMesh.pVertices[vertIdxOfEdge.y] - perfMesh.pVertices[vertIdxOfEdge.x];
	#ifdef ADD_BY_TRIANGLE_HALF
				nCount = 1;
	#endif
				vDif = vDif / (float)(nCount + 1);
				blExit = false;
				vNewVerts.resize(nNewVertCount + nCount);
				const VFVECTOR3& vStart = perfMesh.pVertices[vertIdxOfEdge.x];
				VNVECTOR2UI surfIdxOfEdge = perfMesh.GetSurfIndxOfEdge(nEdgeIdx);
				VNVECTOR2UI edgeIdxInSurf = perfMesh.GetSurfNbrIndxOfEdge(nEdgeIdx);
	#ifdef ADD_BY_TRIANGLE_HALF_ANGLE
				bool blDo = false;
				for (size_t i = 0; i < 2; i++)
				{
					if (VD_INVALID_INDEX != surfIdxOfEdge[i])
					{
						const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge[i]];
						const VFVECTOR3& v = perfMesh.pVertices[s[edgeIdxInSurf[i]]];
						VFVECTOR3 v1 = perfMesh.pVertices[vertIdxOfEdge.x] - v;
						VFVECTOR3 v2 = perfMesh.pVertices[vertIdxOfEdge.y] - v;
						v1.Normalize();
						v2.Normalize();
						float cosvalue = v1.Dot(v2);
						cosvalue = cosvalue < 1.0 ? cosvalue : 1.0f - FLT_EPSILON;
						cosvalue = cosvalue > -1.0 ? cosvalue : -1.0f + FLT_EPSILON;

						cosvalue = acos(cosvalue);
						if (cosvalue > VF_HALF_PI)
						{
							blDo = true;
						}
					}
				}
				if (false == blDo)
					continue;
	#endif
				for (size_t i = 1; i <= nCount; i++)
				{
					vNewVerts[nNewVertCount + i - 1] = vStart + vDif * i;
				}
				unsigned nShift = nVertCount + nNewVertCount;
				if (VD_INVALID_INDEX != surfIdxOfEdge.x)
				{
					vSurfRemoved[surfIdxOfEdge.x] = 1;
					MarkSurfFlag(vSurfFlag, perfMesh, surfIdxOfEdge.x);
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.x];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, nShift, s[edgeIdxInSurf.x]));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i, nShift + i + 1, s[edgeIdxInSurf.x]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, s[edgeIdxInSurf.x], nShift + nCount - 1));
				}

				if (VD_INVALID_INDEX != surfIdxOfEdge.y)
				{
					vSurfRemoved[surfIdxOfEdge.y] = 1;
					MarkSurfFlag(vSurfFlag, perfMesh, surfIdxOfEdge.y);
					unsigned nShift = nVertCount + nNewVertCount;
					const VNVECTOR3UI& s = perfMesh.pSurfaces[surfIdxOfEdge.y];
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.x, s[edgeIdxInSurf.y], nShift));
					for (size_t i = 0; i < nCount - 1; i++)
					{
						vNewSurfs.push_back(VNVECTOR3UI(nShift + i + 1, nShift + i, s[edgeIdxInSurf.y]));
					}
					vNewSurfs.push_back(VNVECTOR3UI(vertIdxOfEdge.y, nShift + nCount - 1, s[edgeIdxInSurf.y]));
					/*VNVECTOR3UI soe = perfMesh.GetEdgeIndxOfSurf(eos.y);
					for (size_t i = 0; i < 3; i++)
					{
					vEdgeFlag[soe[i]] = 1;
					}*/
				}
			}
			nNewVertCount = vNewVerts.size();
		}
		if ((true == blExit) || nLoopCount >= MAX_LOOP_SIZE)
			return;
		out.vVertice.resize(nVertCount + nNewVertCount);
		std::memcpy(&out.vVertice[nVertCount], &vNewVerts[0], sizeof(VFVECTOR3) * nNewVertCount);
		std::vector<VNVECTOR3UI> vSurfTmp(out.vSurface.begin(), out.vSurface.end());
		out.vSurface.resize(nSurfCount + vNewSurfs.size());
		nSurfCount = 0;
		for (size_t i = 0; i < vSurfTmp.size(); i++)
		{
			if (0 == vSurfRemoved[i])
			{
				out.vSurface[nSurfCount++] = vSurfTmp[i];
			}
		}
		for (size_t i = 0; i < vNewSurfs.size(); i++)
		{
			out.vSurface[nSurfCount++] = vNewSurfs[i];
		}
		out.vSurface.resize(nSurfCount);
		PMMeshGetSimpleMesh(simpMeshTmp, out);
		nLoopCount++;
		sprintf_s(szFileName, 256, "D:/testdata/refinemeshbytriangle_%d.obj", nLoopCount);
		OBJWriteSimpleMesh(szFileName, simpMeshTmp);
	};
#endif
#endif
#endif
}

void PMIncreasedVerticesDensity(std::vector<VFVECTOR3>& vNewVerts, const VSSimpleMeshF& sm)
{
	vNewVerts.resize(sm.nVertCount + sm.nTriangleCount);
	std::memcpy(&vNewVerts[0], sm.pVertices, sizeof(VFVECTOR3) * sm.nVertCount);
	// 计算三角片中心,以增加顶点密度。
	for (size_t i = 0; i < sm.nTriangleCount; i++)
	{
		const VNVECTOR3UI& s = sm.pTriangles[i];
		vNewVerts[sm.nVertCount + i] = (vNewVerts[s[0]] + vNewVerts[s[1]] + vNewVerts[s[2]]) / 3.0f;
		vNewVerts.push_back((vNewVerts[s[0]] + vNewVerts[s[1]] + vNewVerts[sm.nVertCount + i]) / 3.0f);
		vNewVerts.push_back((vNewVerts[s[0]] + vNewVerts[s[2]] + vNewVerts[sm.nVertCount + i]) / 3.0f);
		vNewVerts.push_back((vNewVerts[s[2]] + vNewVerts[s[1]] + vNewVerts[sm.nVertCount + i]) / 3.0f);
	}
}

void PMGetLocationToRay(VFLocation& loc, const VFRay& ray)
{
	// 1. 转换坐标系
	VFVECTOR3 vTmpZ = ray.GetDirection();
	vTmpZ.x += 2.0f;
	vTmpZ = ray.GetOrigin() + vTmpZ * 5.0f;
	VFVECTOR3 vTmpX = (vTmpZ - ray.GetOrigin()).Cross(ray.GetDirection());
	vTmpZ = ray.GetDirection();
	VFVECTOR3 vTmpY = vTmpZ.Cross(vTmpX);
	vTmpX.Normalize();
	vTmpY.Normalize();
	VFMatrix3 rot(vTmpX.x, vTmpY.x, vTmpZ.x,
		vTmpX.y, vTmpY.y, vTmpZ.y,
		vTmpX.z, vTmpY.z, vTmpZ.z);
	loc = VFLocation(ray.GetOrigin(), VFQuaternion::FromRotationMatrix(rot));
}

void PMChangeVertices(std::vector<VFVECTOR3>& vVertsOut, const VSConstBuffer<VFVECTOR3>& vVertsIn, const VFLocation& loc)
{
	vVertsOut.resize(vVertsIn.len);
	for (unsigned i = 0; i < vVertsIn.len; i++)
	{
		vVertsOut[i] = loc.TransGlobalVertexToLocal(vVertsIn.pData[i]);
	}
}

void PMGetFistTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint, 
	const VSSimpleMeshF& sm, const VFRay& ray, const float flPrecision)
{	
	std::vector<VFVECTOR3> vVerts;	
	// 1. 增加顶点密度
	PMIncreasedVerticesDensity(vVerts, sm);
		
	// 2. 计算给定范围的顶点的z值	
	PMGetFistTouchedPoint(positivePoint, negativePoint, VD_V2CB(vVerts), ray, flPrecision);
}

void PMGetFistTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSConstBuffer<VFVECTOR3>& cbVerts, const VFRay& ray, const float flPrecision)
{
	VFLocation loc;
	std::vector<VFVECTOR3> vVerts;
	// 1. 计算以射线为z轴的坐标系
	PMGetLocationToRay(loc, ray);	
	// 2. 转换顶点到新的坐标系
	PMChangeVertices(vVerts, cbVerts, loc);
	// 4. 计算给定范围的顶点的z值
	float flCheckMax = flPrecision / 2.0f;
	float flCheckMin = -flCheckMax;
	float flMax = VERTICE_COOR_MIN;
	float flMin = VERTICE_COOR_MAX;
	for (size_t i = 0; i < cbVerts.len; i++)
	{
		if ((vVerts[i].x > flCheckMax) || (vVerts[i].x < flCheckMin) ||
			(vVerts[i].y > flCheckMax) || (vVerts[i].y < flCheckMin))
			continue;
		if (vVerts[i].z < 0.0f)
		{
			if (flMax < vVerts[i].z)
				flMax = vVerts[i].z;
		}
		else
		{
			if (flMin > vVerts[i].z)
				flMin = vVerts[i].z;
		}
	}
	positivePoint = VFVECTOR3::ZERO;
	positivePoint.z = flMin;

	negativePoint = VFVECTOR3::ZERO;
	negativePoint.z = flMax;
	positivePoint = loc.TransLocalVertexToGlobal(positivePoint);
	negativePoint = loc.TransLocalVertexToGlobal(negativePoint);
}

void PMGetLastTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSConstBuffer<VFVECTOR3>& cbVerts, const VFRay& ray, const float flPrecision)
{
	VFLocation loc;
	std::vector<VFVECTOR3> vVerts;
	// 1. 计算以射线为z轴的坐标系
	PMGetLocationToRay(loc, ray);
	// 2. 转换顶点到新的坐标系
	PMChangeVertices(vVerts, cbVerts, loc);
	// 4. 计算给定范围的顶点的z值
	float flCheckMax = flPrecision / 2.0f;
	float flCheckMin = -flCheckMax;
	float flMax = VERTICE_COOR_MIN;
	float flMin = VERTICE_COOR_MAX;
	for (size_t i = 0; i < cbVerts.len; i++)
	{
		if ((vVerts[i].x > flCheckMax) || (vVerts[i].x < flCheckMin) ||
			(vVerts[i].y > flCheckMax) || (vVerts[i].y < flCheckMin))
			continue;
		if (vVerts[i].z < 0.0f)
		{
			if (flMin > vVerts[i].z)
				flMin = vVerts[i].z;
		}
		else
		{
			if (flMax < vVerts[i].z)
				flMax = vVerts[i].z;
		}
	}
	positivePoint = VFVECTOR3::ZERO;
	positivePoint.z = flMax;

	negativePoint = VFVECTOR3::ZERO;
	negativePoint.z = flMin;
	positivePoint = loc.TransLocalVertexToGlobal(positivePoint);
	negativePoint = loc.TransLocalVertexToGlobal(negativePoint);
}

void PMGetTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,	
	const VSSimpleMeshF& sm,  const VFRay& ray,  const bool blIsFirstTouch)
{
	std::vector<VFVECTOR3> vVerts;
	// 1. 转换坐标系
	VFLocation loc;
	VSConstBuffer<VFVECTOR3> cbVerts;
	cbVerts.len = sm.nVertCount;
	cbVerts.pData = sm.pVertices;
	PMGetLocationToRay(loc, ray);
	PMChangeVertices(vVerts, cbVerts, loc);
	float flMax = VERTICE_COOR_MIN;
	float flMin = VERTICE_COOR_MAX;
	VFRay tmpRay;
	tmpRay.SetOrigin(loc.TransGlobalVertexToLocal(ray.GetOrigin()));
	tmpRay.SetDirection(loc.TransGlobalNormalToLocal(ray.GetDirection()));
	// 2. 计算给定范围的顶点的z值	
	if (true == blIsFirstTouch)
	{		
		for (size_t i = 0; i < sm.nTriangleCount; i++)
		{
			const VNVECTOR3UI& s = sm.pTriangles[i];
			VFPlane plane(vVerts[s[0]], vVerts[s[1]], vVerts[s[2]]);
			VFVECTOR3 crossP;
			NM_TOOLS::RAYPLANE_CROSS_E crossT = PMGetCrossPointRayAndPlane(crossP, tmpRay, plane);
			if (NM_TOOLS::E_PARALLEL != crossT)
			{
				VFArealCoord3 arealCoord = VFArealCoord3::FromTriangle(
					vVerts[s[0]], vVerts[s[1]], vVerts[s[2]], crossP);
				if (false == arealCoord.IsOuterPoint(VF_EPS_2))
				{
					if (NM_TOOLS::E_CROSS_POSITIVE == crossT)
					{
						if (flMin > crossP.z)
							flMin = crossP.z;
					}
					else
					{
						if (flMax < crossP.z)
							flMax = crossP.z;
					}
				}
			}
		}
	}
	else
	{
		flMax = VERTICE_COOR_MAX;
		flMin = VERTICE_COOR_MIN;
		for (size_t i = 0; i < sm.nTriangleCount; i++)
		{
			const VNVECTOR3UI& s = sm.pTriangles[i];
			VFPlane plane(vVerts[s[0]], vVerts[s[1]], vVerts[s[2]]);
			VFVECTOR3 crossP;
			NM_TOOLS::RAYPLANE_CROSS_E crossT = PMGetCrossPointRayAndPlane(crossP, tmpRay, plane);
			if (NM_TOOLS::E_PARALLEL != crossT)
			{
				VFArealCoord3 arealCoord = VFArealCoord3::FromTriangle(
					vVerts[s[0]], vVerts[s[1]], vVerts[s[2]], crossP);
				if (false == arealCoord.IsOuterPoint(VF_EPS_2))
				{
					if (NM_TOOLS::E_CROSS_POSITIVE == crossT)
					{
						if (flMin < crossP.z)
							flMin = crossP.z;
					}
					else
					{
						if (flMax > crossP.z)
							flMax = crossP.z;
					}
				}
			}
		}
	}
	positivePoint = VFVECTOR3::ZERO;
	positivePoint.z = flMin;

	negativePoint = VFVECTOR3::ZERO;
	negativePoint.z = flMax;
	positivePoint = loc.TransLocalVertexToGlobal(positivePoint);
	negativePoint = loc.TransLocalVertexToGlobal(negativePoint);
}


NM_TOOLS::RAYPLANE_CROSS_E PMGetCrossPointRayAndPlane(VFVECTOR3& crossPoint,
	const VFRay& ray, const VFPlane& plane)
{
	VFVECTOR3 dir = ray.GetDirection();
	float flDot = plane.normal.Dot(dir);

	if (abs(flDot) < VF_EPS)
		return NM_TOOLS::E_PARALLEL;
	VFVECTOR3 org = ray.GetOrigin();
	float x = -plane.getDistance(org) / flDot;
	crossPoint = ray.GetPoint(x);
	if (x > 0.0f)
	{
		return NM_TOOLS::E_CROSS_POSITIVE;
	}
	return NM_TOOLS::E_CROSS_NEGATIVE;
}

void PMGetMeshCenter(VFVECTOR3& center, const VSSimpleMeshF& simpMesh)
{
	if (0 == simpMesh.nVertCount)
		return;

	center = VFVECTOR3::ZERO;
	for (size_t i = 0; i < simpMesh.nVertCount; i++)
	{
		center += simpMesh.pVertices[i];
	}
	center /= (float)simpMesh.nVertCount;
}


void PMMeshTransMove(std::vector<VFVECTOR3>& out,  const VSSimpleMeshF& simpMesh, const VFVECTOR3& shift)
{
	if (0 == simpMesh.nVertCount)
		return;

	out.resize(simpMesh.nVertCount);
	for (size_t i = 0; i < simpMesh.nVertCount; i++)
	{
		out[i] = simpMesh.pVertices[i] - shift;
	}
}



//////////////////////////////////////////////////////////////////////////
// 检查网格，移除单独的较小的独立网格，保留顶点最多的网格，保证网格是一个单连通的网格。
void PMMeshSimplifyConnection(NM_PMMESH::VSMesh& outMesh, const VSPerfectMesh& mesh)
{
	unsigned SC = mesh.GetSurfCount();
	unsigned VC = mesh.GetVertexCount();
	std::vector< unsigned > vSurfScore(SC, 0);
	std::vector< unsigned > stkSearch, stkBuff;
	memset(&vSurfScore.front(), 0, SC * sizeof(unsigned));

	for (unsigned nSIdx = 0; nSIdx < SC; nSIdx++)
	{
		if (1 == vSurfScore[nSIdx])
			continue;

		stkSearch.push_back(nSIdx);
		std::vector<unsigned> vVertMap(mesh.GetVertexCount(), VD_INVALID_INDEX);
		NM_PMMESH::VSMesh tmpMesh;
		tmpMesh.vVertice.resize(VC);
		tmpMesh.vSurface.resize(SC);
		unsigned nVertCount = 0;
		unsigned nSurfCount = 0;
		vSurfScore[nSIdx] = 1;
		while (!stkSearch.empty())
		{
			for (auto sid : stkSearch)
			{
				const VNVECTOR3UI& s = mesh.pSurfaces[sid];

				for (size_t i = 0; i < 3; i++)
				{
					if (VD_INVALID_INDEX == vVertMap[s[i]])
					{
						vVertMap[s[i]] = nVertCount;
						tmpMesh.vVertice[nVertCount++] = mesh.pVertices[s[i]];
					}
					tmpMesh.vSurface[nSurfCount][i] = vVertMap[s[i]];
				}
				nSurfCount++;

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
						stkBuff.push_back(sn);
					}
				}
			}
			stkSearch.swap(stkBuff);
			stkBuff.clear();
		}
		if (nSurfCount > (SC / 2 + 2))
		{
			outMesh.vVertice.resize(nVertCount);
			outMesh.vSurface.resize(nSurfCount);
			std::memcpy(&outMesh.vVertice[0], &tmpMesh.vVertice[0], sizeof(VFVECTOR3) * nVertCount);
			std::memcpy(&outMesh.vSurface[0], &tmpMesh.vSurface[0], sizeof(VFVECTOR3) * nSurfCount);
			return;
		}
	}
}

void PMMeshRingPatching(NM_PMMESH::VSMesh& out, const VSConstBuffer<VFVECTOR3>& cbBndry1,
	const VSConstBuffer<VFVECTOR3>& cbBndry2, const float flPrecision)
{
	NM_PMMESH::VSMesh midMesh;
	midMesh.vVertice.resize(cbBndry1.len + cbBndry2.len);
	std::memcpy(&midMesh.vVertice[0], cbBndry1.pData, sizeof(VFVECTOR3) * cbBndry1.len);
	std::memcpy(&midMesh.vVertice[cbBndry1.len], cbBndry2.pData, sizeof(VFVECTOR3) * cbBndry2.len);
	unsigned nBndry2CurIdx = 0;
	unsigned nBndry1CurIdx = 0;
	bool blExit = false;
	while(false == blExit)
	{
		unsigned nBndry2NextIdx = (nBndry2CurIdx + 1) % cbBndry2.len;
		unsigned nBndry1NextIdx = (nBndry1CurIdx + 1) % cbBndry1.len;
		VFVECTOR3 vTmp1 = cbBndry1.pData[nBndry1NextIdx] - cbBndry1.pData[nBndry1CurIdx];
		vTmp1.Normalize();
		VFVECTOR3 vTmp2 = cbBndry2.pData[nBndry2NextIdx] - cbBndry2.pData[nBndry2CurIdx];
		vTmp2.Normalize();
		VFVECTOR3 vTmp12 = cbBndry2.pData[nBndry2CurIdx] - cbBndry1.pData[nBndry1CurIdx];
		vTmp12.Normalize();
		
		float flVal1 = vTmp12.Dot(vTmp1);
		float flVal2 = (-vTmp12).Dot(vTmp2);		
		
		if (flVal1 > flVal2)
		{	
			if (nBndry1CurIdx < cbBndry1.len)
			{
				midMesh.vSurface.push_back(VNVECTOR3UI(nBndry1CurIdx, nBndry2CurIdx + cbBndry1.len, nBndry1NextIdx));
				nBndry1CurIdx++;
			}
			else if (nBndry2CurIdx < cbBndry2.len)
			{
				midMesh.vSurface.push_back(VNVECTOR3UI(0, nBndry2CurIdx + cbBndry1.len, nBndry2NextIdx+cbBndry1.len));
				nBndry2CurIdx++;
			}			
		}
		else
		{		
			if (nBndry2CurIdx < cbBndry2.len)
			{
				midMesh.vSurface.push_back(VNVECTOR3UI(nBndry1CurIdx % cbBndry1.len, nBndry2CurIdx + cbBndry1.len, nBndry2NextIdx + cbBndry1.len));
				nBndry2CurIdx++;
			}
			else if (nBndry1CurIdx < cbBndry1.len)
			{
				midMesh.vSurface.push_back(VNVECTOR3UI(nBndry1CurIdx, nBndry2CurIdx % cbBndry2.len + cbBndry1.len, nBndry1NextIdx));
				nBndry1CurIdx++;
			}
		}	
		if ((nBndry1CurIdx == cbBndry1.len) && (nBndry2CurIdx == cbBndry2.len))
		{
			blExit = true;
			break;
		}
	}
	VSSimpleMeshF simpMesh;
	PMGetSimpMesh(simpMesh, midMesh);
#if 0
	{
		OBJWriteSimpleMesh("D:/testdata/tmpmesh.obj", simpMesh);
	}
#endif
	PMMeshRefining(out, simpMesh, flPrecision);
}

void PMMeshCenterFixSmooth(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh)
{
	VFVECTOR3 vecB(1.0f, 1.0f, 1.0f);
	unsigned nSurfCount = perfMesh.GetSurfCount();
	unsigned nVertCount = perfMesh.GetVertexCount();
	std::vector<VFMATRIX3> vMatAInv(nSurfCount);
	std::vector<VFMATRIX3> vMatP(nSurfCount);
	std::vector<VFVECTOR3> vVecBAInv(nSurfCount);
	std::vector<VFVECTOR3> vVecAInvTransBTran(nSurfCount);
	std::vector<VFVECTOR3> vVecAInvAInvTranBTran(nSurfCount);
	std::vector<VFVECTOR3> vVecC(nSurfCount);
	std::vector<float>	vBAInvAInvTranBTran(nSurfCount);
	std::vector<VFVECTOR3> vVertices(nVertCount, VFVECTOR3::ZERO);
	
	out.vVertice.resize(nVertCount);
	std::memcpy(&out.vVertice[0], perfMesh.pVertices, sizeof(VFVECTOR3)*nVertCount);
	VFVECTOR3* pVertices[2] = {&out.vVertice[0], &vVertices[0]};
	bool blExit = false;
	int nFlag = 0;

	for (size_t nSIdx = 0; nSIdx < nSurfCount; nSIdx++)
	{
		const VNVECTOR3UI& s = perfMesh.pSurfaces[nSIdx];
		for (size_t i = 0; i < 3; i++)
		{
			std::vector<unsigned> vDom;
			::PMGet1OrderDomain(vDom, s[i], perfMesh);
			unsigned nDomSize = vDom.size();
			for (size_t j = 0; j < 3; j++)
			{
				vMatAInv[nSIdx][i][j] = (i == j) ? 1.0f : (-1.0f/nDomSize);// (-NM_PMMESH::g_flReciprocalN[nDomSize]);
				vMatP[nSIdx][i][j] = perfMesh.pVertices[s[i]][j];				
			}
		}
		vMatAInv[nSIdx] = vMatAInv[nSIdx].Inverse();
		vVecBAInv[nSIdx] = vecB * vMatAInv[nSIdx];
		vVecAInvTransBTran[nSIdx] = vMatAInv[nSIdx].Transpose() * vecB;
		vVecAInvAInvTranBTran[nSIdx] = vMatAInv[nSIdx] * vVecAInvTransBTran[nSIdx];
		vBAInvAInvTranBTran[nSIdx] = vVecBAInv[nSIdx].Dot(vVecAInvTransBTran[nSIdx]);
		vVecC[nSIdx] = vecB * vMatP[nSIdx];
	}
	while (false == blExit)
	{
		VFVECTOR3* pSrcVert = pVertices[nFlag];
		nFlag = 1 - nFlag;
		VFVECTOR3* pDstVert = pVertices[nFlag];
		std::vector<unsigned> vVertCount(nVertCount, 0);
		std::memset(pDstVert, 0, sizeof(VFVECTOR3) * nVertCount);
		for (size_t nSIdx = 0; nSIdx < nSurfCount; nSIdx++)
		{	
			const VNVECTOR3UI& s = perfMesh.pSurfaces[nSIdx];
			VFMATRIX3 matQ;
			for (size_t i = 0; i < 3; i++)
			{
				std::vector<unsigned> vDom;
				::PMGet1OrderDomain(vDom, s[i], perfMesh);
				unsigned nDomSize = vDom.size();
				for (size_t j = 0; j < 3; j++)
				{	
					matQ[i][j] = 0.0f;
					for (size_t k = 0; k < nDomSize; k++)
					{
						if ((vDom[k] != s[(i + 1) % 3]) && (vDom[k] != s[(i + 2) % 3]))
						{
							matQ[i][j] += pSrcVert[vDom[k]][j] * (1.0f / (float)nDomSize);// (-NM_PMMESH::g_flReciprocalN[nDomSize]);
						}
					}
				}
			}			
			
			VFVECTOR3 vecBAInvQ = vVecBAInv[nSIdx] * matQ;			
			VFVECTOR3 vecLanda = (vVecC[nSIdx] - vecBAInvQ) / vBAInvAInvTranBTran[nSIdx];
			
			VFMATRIX3 matLanda(
				vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[2],
				vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[2],
				vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[2]);
			VFMATRIX3 newP = vMatAInv[nSIdx] * matQ + matLanda;
			for (size_t i = 0; i < 3; i++)
			{
				pDstVert[s[i]] += VFVECTOR3(newP[i][0], newP[i][1], newP[i][2]);
				vVertCount[s[i]]++;
			}			
		}
		float flMax = 0.0f;
		for (size_t nVIdx = 0;  nVIdx < nVertCount;  nVIdx++)
		{
			pDstVert[nVIdx] /= ((float)(vVertCount[nVIdx]));
			float flVal = (pDstVert[nVIdx] - pSrcVert[nVIdx]).SqrMagnitude();
			if (flVal > flMax)
				flMax = flVal;
		}
		if (flMax < 0.00001f)
		{
			blExit = true;
		}
	}
	if (0 != nFlag)
	{
		std::memcpy(&out.vVertice[0], &vVertices[0], sizeof(VFVECTOR3) * nVertCount);
	}
	out.vSurface.resize(nSurfCount);
	std::memcpy(&out.vSurface[0], perfMesh.pSurfaces, sizeof(VNVECTOR3UI) * nSurfCount);
}

void PMMeshCenterFixSmoothWithBndry(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh)
{
	VFVECTOR3 vecB(1.0f, 1.0f, 1.0f);
	unsigned nSurfCount = perfMesh.GetSurfCount();
	unsigned nVertCount = perfMesh.GetVertexCount();
	std::vector<VFMATRIX3> vMatAInv(nSurfCount);
	std::vector<VFMATRIX3> vMatP(nSurfCount);
	std::vector<VFVECTOR3> vVecBAInv(nSurfCount);
	std::vector<VFVECTOR3> vVecAInvTransBTran(nSurfCount);
	std::vector<VFVECTOR3> vVecAInvAInvTranBTran(nSurfCount);
	std::vector<VFVECTOR3> vVecC(nSurfCount);
	std::vector<float>	vBAInvAInvTranBTran(nSurfCount);
	std::vector<VFVECTOR3> vVertices(nVertCount, VFVECTOR3::ZERO);
	
	out.vVertice.resize(nVertCount);
	std::memcpy(&out.vVertice[0], perfMesh.pVertices, sizeof(VFVECTOR3)*nVertCount);
	VFVECTOR3* pVertices[2] = { &out.vVertice[0], &vVertices[0] };
	bool blExit = false;
	int nFlag = 0;

	VFMATRIX3 Aedge = { 2.0f / 3 , 1.0f / 3 , 0.0f ,
						1.0f / 3 , 2.0f / 3 , 0.0f ,
						0.0f     , 0.0f     , 0.0f };
	TVExtSource< VNALGMESH::VRFindHole2 > esFindHole;
	TVR2B< VNALGMESH::VRFindHole2 > bdrFindHole(*esFindHole);
	VNWZKALG::VSHoleBoundary holebndry;
	bdrFindHole.Build(holebndry, perfMesh);

	for (size_t nSIdx = 0; nSIdx < nSurfCount; nSIdx++)
	{
		const VNVECTOR3UI& s = perfMesh.pSurfaces[nSIdx];
		for (size_t i = 0; i < 3; i++)
		{
			std::vector<unsigned> vDom;
			::PMGet1OrderDomain(vDom, s[i], perfMesh);
			unsigned nDomSize = vDom.size();
			for (size_t j = 0; j < 3; j++)
			{
				vMatAInv[nSIdx][i][j] = (i == j) ? 1.0f : (-1.0f / nDomSize);// (-NM_PMMESH::g_flReciprocalN[nDomSize]);
				vMatP[nSIdx][i][j] = perfMesh.pVertices[s[i]][j];
			}
		}
		vMatAInv[nSIdx] = vMatAInv[nSIdx].Inverse();
		vVecBAInv[nSIdx] = vecB * vMatAInv[nSIdx];
		vVecAInvTransBTran[nSIdx] = vMatAInv[nSIdx].Transpose() * vecB;
		vVecAInvAInvTranBTran[nSIdx] = vMatAInv[nSIdx] * vVecAInvTransBTran[nSIdx];
		vBAInvAInvTranBTran[nSIdx] = vVecBAInv[nSIdx].Dot(vVecAInvTransBTran[nSIdx]);
		vVecC[nSIdx] = vecB * vMatP[nSIdx];
	}
	while (false == blExit)
	{
		VFVECTOR3* pSrcVert = pVertices[nFlag];
		nFlag = 1 - nFlag;
		VFVECTOR3* pDstVert = pVertices[nFlag];
		std::vector<unsigned> vVertCount(nVertCount, 0);
		std::memset(pDstVert, 0, sizeof(VFVECTOR3) * nVertCount);
		
		// process holebndry
		for (size_t hbIdx = 0; hbIdx < holebndry.bndry.len; hbIdx++)
		{
			const auto& bvLen = holebndry.bndry.pData[hbIdx].orderedVert.len;
			const auto& bvData = holebndry.bndry.pData[hbIdx].orderedVert.pData;

			for (size_t i = 0; i < bvLen; i++)
			{
				const auto& ori = pSrcVert[bvData[i]];
				const auto& v0 = pSrcVert[bvData[((i - 1) + bvLen) % bvLen]];
				const auto& v1 = pSrcVert[bvData[((i - 2) + bvLen) % bvLen]];
				const auto& v2 = pSrcVert[bvData[((i + 1) + bvLen) % bvLen]];

				VFVECTOR3 d = (ori + v0 - v1 - v2) / 2;
				VFMATRIX3 D = { d.x , d.y , d.z ,
								d.x , d.y , d.z ,
								0   , 0   , 0 };
				VFMATRIX3 p = { v1.x , v1.y , v1.z ,
								v2.x , v2.y , v2.z ,
								0    , 0    , 0 };
				VFMATRIX3 P = Aedge * p + D;

				if (i < bvLen - 1)
				{
					pDstVert[bvData[i]] += VFVECTOR3(P[0][0], P[0][1], P[0][2]);
					pDstVert[bvData[i + 1]] += VFVECTOR3(P[1][0], P[1][1], P[1][2]);
					vVertCount[bvData[i]]++;
					vVertCount[bvData[i + 1]]++;
				}
			}
		}

		for (size_t nSIdx = 0; nSIdx < nSurfCount; nSIdx++)
		{
			const VNVECTOR3UI& s = perfMesh.pSurfaces[nSIdx];
			VFMATRIX3 matQ;
			for (size_t i = 0; i < 3; i++)
			{
				std::vector<unsigned> vDom;
				::PMGet1OrderDomain(vDom, s[i], perfMesh);
				unsigned nDomSize = vDom.size();
				for (size_t j = 0; j < 3; j++)
				{
					matQ[i][j] = 0.0f;
					for (size_t k = 0; k < nDomSize; k++)
					{
						if ((vDom[k] != s[(i + 1) % 3]) && (vDom[k] != s[(i + 2) % 3]))
						{
							matQ[i][j] += pSrcVert[vDom[k]][j] * (1.0f / (float)nDomSize);// (-NM_PMMESH::g_flReciprocalN[nDomSize]);
						}
					}
				}
			}

			VFVECTOR3 vecBAInvQ = vVecBAInv[nSIdx] * matQ;
			VFVECTOR3 vecLanda = (vVecC[nSIdx] - vecBAInvQ) / vBAInvAInvTranBTran[nSIdx];

			VFMATRIX3 matLanda(
				vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][0] * vecLanda[2],
				vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][1] * vecLanda[2],
				vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[0], vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[1], vVecAInvAInvTranBTran[nSIdx][2] * vecLanda[2]);
			VFMATRIX3 newP = vMatAInv[nSIdx] * matQ + matLanda;
			for (size_t i = 0; i < 3; i++)
			{
				pDstVert[s[i]] += VFVECTOR3(newP[i][0], newP[i][1], newP[i][2]);
				vVertCount[s[i]]++;
			}
		}
		float flMax = 0.0f;
		for (size_t nVIdx = 0; nVIdx < nVertCount; nVIdx++)
		{
			pDstVert[nVIdx] /= ((float)(vVertCount[nVIdx]));
			float flVal = (pDstVert[nVIdx] - pSrcVert[nVIdx]).SqrMagnitude();
			if (flVal > flMax)
				flMax = flVal;
		}
		if (flMax < 0.00001f)
		{
			blExit = true;
		}
	}
	if (0 != nFlag)
	{
		std::memcpy(&out.vVertice[0], &vVertices[0], sizeof(VFVECTOR3) * nVertCount);
	}
	out.vSurface.resize(nSurfCount);
	std::memcpy(&out.vSurface[0], perfMesh.pSurfaces, sizeof(VNVECTOR3UI) * nSurfCount);
}

void PMRemoveRepeatedPoint(NM_PMMESH::VSMesh& out, const NM_PMMESH::VSMesh& in)
{
	unsigned nOldVertCnt = in.vVertice.size();
	std::vector<unsigned> vVertMap2New(nOldVertCnt, VD_INVALID_INDEX);
	out.vVertice.clear();
	out.vVertice.reserve(nOldVertCnt);
	out.vSurface.resize(in.vSurface.size());
	std::unordered_map<VFVECTOR3, unsigned> mapVerts;
	unsigned nNewVertIdx = 0;
	for (unsigned nOldIdx = 0; nOldIdx < nOldVertCnt; nOldIdx++)
	{
		const VFVECTOR3& v = in.vVertice[nOldIdx];

		if (mapVerts.count(v) == 0)
		{
			mapVerts.insert(std::make_pair(v, nNewVertIdx));
			out.vVertice.push_back(v);
			vVertMap2New[nOldIdx] = nNewVertIdx++;
		}

		auto vert = mapVerts.find(v);
		vVertMap2New[nOldIdx] = vert->second;
	}
	for (size_t i = 0; i < in.vSurface.size(); i++)
	{
		const VNVECTOR3UI& f = in.vSurface[i];
		out.vSurface[i] = VNVECTOR3UI(vVertMap2New[f.x], vVertMap2New[f.y], vVertMap2New[f.z]);
	}
}

unsigned PMGetRayIntersect(const VSSimpleMeshF& mesh, const VFRay& ray,
	std::vector<VFVECTOR3>* posiPoints, std::vector<unsigned>* posiSurfIdx,
	std::vector<VFVECTOR3>* negaPoints, std::vector<unsigned>* negaSurfIdx)
{
	if (posiPoints)  posiPoints->resize(0);
	if (posiSurfIdx) posiSurfIdx->resize(0);
	if (negaPoints)  negaPoints->resize(0);
	if (negaSurfIdx) negaSurfIdx->resize(0);

	const auto& ori = ray.GetOrigin();
	auto dir = ray.GetDirection();
	dir.Normalize();

	auto Intersect = [&mesh, &ori, &dir](float& out, const unsigned idx) -> bool {
		const auto& v0 = mesh.pVertices[mesh.pTriangles[idx].x];
		const auto& v1 = mesh.pVertices[mesh.pTriangles[idx].y];
		const auto& v2 = mesh.pVertices[mesh.pTriangles[idx].z];
		const auto  e1 = v1 - v0;
		const auto  e2 = v2 - v0;
		const auto  p = dir.Cross(e2);
		auto det = e1.Dot(p);
		VFVECTOR3 t;
		if (det > 0) {
			t = ori - v0;
		}
		else {
			t = v0 - ori;
			det = -det;
		}
		if (det < VF_EPS_2) return false;
		const auto u = t.Dot(p);
		if (u < 0.0f || u > det) return false;
		const auto q = t.Cross(e1);
		const auto v = dir.Dot(q);
		if (v < 0.0f || u + v > det) return false;
		out = e2.Dot(q) * (1.0f / det);
		return true;
	};

	std::map<float, unsigned, std::less<float>>    po_pnts;
	std::map<float, unsigned, std::greater<float>> op_pnts;

	float out = 0.0f;
	for (size_t i = 0; i < mesh.nTriangleCount; i++) {
		if (!Intersect(out, i))   continue;
		if (out > 0)   po_pnts.insert(std::make_pair(out, i));
		else           op_pnts.insert(std::make_pair(out, i));
	}

	if (!po_pnts.empty()) {
		if (posiPoints != nullptr) {
			posiPoints->reserve(po_pnts.size());
			for (const auto& v : po_pnts)
				posiPoints->push_back(ori + dir * v.first);
		}
		if (posiSurfIdx != nullptr) {
			posiSurfIdx->reserve(po_pnts.size());
			for (const auto& v : po_pnts)
				posiSurfIdx->push_back(v.second);
		}
	}
	if (!op_pnts.empty()) {
		if (negaPoints != nullptr) {
			negaPoints->reserve(op_pnts.size());
			for (const auto& v : op_pnts)
				negaPoints->push_back(ori + dir * v.first);
		}
		if (negaSurfIdx != nullptr) {
			negaSurfIdx->reserve(op_pnts.size());
			for (const auto& v : op_pnts)
				negaSurfIdx->push_back(v.second);
		}
	}

	return po_pnts.size();
}