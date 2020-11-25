#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <obj/ialgeq/ialgeq.h>
#include "lsalgobjfile.h"
#include "ilsalgtools.h"



// struct VSMesh
/*
	三维网格类;
	成员数据：
			vector<VFVECTOR3> vVertice;————三维点对象vector表示的顶点集
			vector<VNVECTOR3UI> vSurface;

			

*/


namespace NM_PMMESH
{
	extern const float g_flReciprocalN[21];	
	extern const VFMATRIX3 g_matA[21];
	extern const VFMATRIX3 g_matAInv[21];
	extern const VFVECTOR3 g_vecBAInv[21];
	extern const float g_flBAInvAtBt[21];

	struct VSMesh
	{
		// 成员数据
		std::vector<VFVECTOR3> vVertice;
		std::vector<VNVECTOR3UI> vSurface;


		// 构造函数
		VSMesh() { Clear(); }

		//			拷贝构造函数 
		VSMesh(const VSMesh& in)	
		{
			vVertice.assign(in.vVertice.begin(), in.vVertice.end());
			vSurface.assign(in.vSurface.begin(), in.vSurface.end());
		}

		//			输入VSSimpleMesh表示的三维网格对象来构造
		VSMesh(const VSSimpleMeshF& in)
		{
			if (0 == in.nVertCount)
			{
				Clear();
				return;
			}
			vVertice.resize(in.nVertCount);
			std::memcpy(&vVertice[0], in.pVertices, sizeof(VFVECTOR3) * in.nVertCount);
			if (0 == in.nTriangleCount)
			{
				Clear();
				return;
			}
			vSurface.resize(in.nTriangleCount);
			std::memcpy(&vSurface[0], in.pTriangles, sizeof(VNVECTOR3UI) * in.nTriangleCount);
		}



		// 
		void GetSimpMesh(VSSimpleMeshF& out)
		{
			if (vVertice.size() > 0)
			{
				out.pVertices = &vVertice[0];
				out.nVertCount = vVertice.size();
			}
			else
			{
				out.pVertices = NULL;
				out.nVertCount = 0;
			}
			if (vSurface.size() > 0)
			{
				out.pTriangles = &vSurface[0];
				out.nTriangleCount = vSurface.size();
			}
			else
			{
				out.pTriangles = NULL;
				out.nTriangleCount = 0;
			}
		}
		
		
		void Clear()
		{
			vVertice.clear();
			vSurface.clear();
		}
	};
}
//////////////////////////////////////////////////////////////////////////
// PerfMesh operator
extern void PMGetSimpMesh(VSSimpleMeshF& out, const NM_PMMESH::VSMesh& mesh);
extern void PMCopyMesh(NM_PMMESH::VSMesh& out, const NM_PMMESH::VSMesh& in);
extern void PMCopyMesh(NM_PMMESH::VSMesh& out, const VSSimpleMeshF& in);
extern void PMMeshRingPatching(NM_PMMESH::VSMesh& out, const VSConstBuffer<VFVECTOR3>& cbBndry1, 
	const VSConstBuffer<VFVECTOR3>& cbBndry2, const float flPrecision);
extern double PMGetAvgEdgeLen(const VSPerfectMesh& pm);
extern void PMMakeLaplaceMatrix(std::vector<VSTripletD>& matLaplace, const VSPerfectMesh& pm, const double dbW = 1.0);
extern void PMGet1OrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfectMesh& pm);
extern void PMGet1OrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfTopoGraph& pg);
extern void PMGetnOrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfectMesh& pm, const unsigned n);
extern void PMGetnOrderDomain(std::vector<unsigned>& domainOut, const unsigned nVIdx, const VSPerfTopoGraph& pg, const unsigned n);
extern void PMDumpSparsMatrix(std::ofstream& dstFile, const VSSprsMatrixD& mat);
extern void PMIncreasedVerticesDensity(std::vector<VFVECTOR3>& vNewVerts, const VSSimpleMeshF& sm);
extern void PMMeshRefining(NM_PMMESH::VSMesh& out, const VSSimpleMeshF& sm, const float flPrecision);
extern void PMGetLocationToRay(VFLocation& loc, const VFRay& ray);
extern void PMGetFistTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSSimpleMeshF& sm, const VFRay& ray, const float flPrecision);
extern void PMGetFistTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSConstBuffer<VFVECTOR3>& cbVerts, const VFRay& ray, const float flPrecision);
extern void PMGetLastTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSConstBuffer<VFVECTOR3>& cbVerts, const VFRay& ray, const float flPrecision);

//////////////////////////////////////////////////////////////////////////
// PMMeshCenterFixSmooth， PMMeshCenterFixSmoothWithBndry，只修改顶点
// 位置，不改变拓扑关系，维持原来的顶点顺序。
extern void PMMeshCenterFixSmooth(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh);
extern void PMMeshCenterFixSmoothWithBndry(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh);
//////////////////////////////////////////////////////////////////////////

extern void PMGetCircle2Dim(std::vector<VFVECTOR2>&out, const unsigned nPointCnt,const float flRadius = 1.0f);
extern void PMRemoveRepeatedPoint(NM_PMMESH::VSMesh& out, const NM_PMMESH::VSMesh& in);
/*
参数：
输出参数：
VFVECTOR3& positivePoint	- 射线正向的交点
VFVECTOR3& negativePoint	- 射线反向的交点
输入参数：
const VSSimpleMeshF& sm		- 牙齿模型网格
const VFRay& ray			- 从牙齿内侧向外发射的射线
const bool blIsFirstTouch	- true： 第一次接触， false： 最后一次接触
描述：射线ray的起始点是处于牙齿模型的内部，向牙齿模型外部发射射线，
查找第一次接触牙齿模型的点或者最后一次接触牙齿模型的点。
*/
extern void PMGetTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSSimpleMeshF& sm, const VFRay& ray, const bool blIsFirstTouch = true);
// 射线和平面相交
extern NM_TOOLS::RAYPLANE_CROSS_E PMGetCrossPointRayAndPlane(VFVECTOR3& crossPoint,
	const VFRay& ray, const VFPlane& plane);

extern void PMGetMeshCenter(VFVECTOR3& center, const VSSimpleMeshF& simpMesh);
extern void PMMeshTransMove(std::vector<VFVECTOR3>& out,const VSSimpleMeshF& simpMesh, const VFVECTOR3& shift);
extern void PMMeshSimplifyConnection(NM_PMMESH::VSMesh& outMesh, const VSPerfectMesh& mesh);
extern void PMMeshGetSimpleMesh(VSSimpleMeshF& simpMesh, const NM_PMMESH::VSMesh& mesh);
template<typename T>
void PMMeshScale(std::vector<VFVECTOR3>& out, const VSSimpleMeshF& simpMesh, const T scale, const bool blMoved = true)
{
	out.resize(simpMesh.nVertCount);
	if (true == blMoved)
	{
		std::vector<VFVECTOR3> vertTmp;
		std::vector<VFVECTOR3> vertTmp1(simpMesh.nVertCount);
		VFVECTOR3 center;
		PMGetMeshCenter(center, simpMesh);
		PMMeshTransMove(vertTmp, simpMesh, center);
		for (size_t i = 0; i < simpMesh.nVertCount; i++)
		{
			vertTmp1[i] = vertTmp[i] * scale;
		}
		VSSimpleMeshF meshTmp;
		meshTmp.nVertCount = simpMesh.nVertCount;
		meshTmp.pVertices = &vertTmp1[0];
		PMMeshTransMove(out, meshTmp, -center);
	}
	else
	{
		for (size_t i = 0; i < simpMesh.nVertCount; i++)
		{
			out[i] = simpMesh.pVertices[i] * scale;
		}
	}
}

/*
* @brief. 计算点集在projPlane平面上的最小外包圆圆心和半径，可先对点集求凸包
* @param[in]. verts 待计算点集
* @param[out]. center 最小外包圆圆心
* @param[out]. radius 最小外包圆半径
* @param[in][opl]. projPlane 投影平面，默认xOy平面
* @return. true:成功; false:失败，检查输入参数
*/
extern bool PMCalcMinBoundCircle(const VSConstBuffer<VFVECTOR3>& verts, VFVECTOR3* center, float* radius, const VFPlane* projPlane = nullptr);
/*
* @brief. 计算点集在projPlane平面上的最大内切圆圆心和半径，需要输入圆心
* @param[in]. verts 待计算点集
* @param[in]. center 最大内切圆圆心
* @param[out]. radius 最大内切圆半径
* @param[in][opl]. projPlane 投影平面，默认xOy平面
* @return. true:成功; false:失败，检查输入参数
*/
extern bool PMCalcMaxInscribedCircle(const VSConstBuffer<VFVECTOR3>& verts, const VFVECTOR3& center, float* radius, const VFPlane* projPlane = nullptr);
/*
* @brief. 判断点集在projPlane平面内是否在指定半径的圆环内，并输出圆环圆心
* @param[in]. verts 待计算点集
* @param[in]. outterRadius 圆环外径
* @param[in]. innerRadius 圆环内径
* @param[in][out]. center 输入为点集原点，输出为该坐标系下符合条件的圆环圆心，为nullptr则默认为坐标原点
* @param[in][opl]. projPlane 投影平面，默认xOy平面
* @return. true:成功; false:失败，没有符合条件的圆环
*/
extern bool PMCalcMinMaxCircle(const VSConstBuffer<VFVECTOR3>& verts, const float& outterRadius, const float& innerRadius,
	VFVECTOR3* center = nullptr, const VFPlane* projPlane = nullptr);
/*
* @brief. 求射线与网格交点
* @param[in]. mesh. 输入网格
* @param[in]. ray.  输入射线
* @param[out]. posiPoints.  射线正方向与网格的交点，posiPoints[0]为正向第一个交点，posiPoints[posiPoints.size() - 1]为正向最后一个交点
* @param[out]. posiSurfIdx. 射线正方向与网格交点所在的三角片索引，与posiPoints一一对应
* @param[out]. negaPoints.  射线反方向与网格的交点，negaPoints[0]为反向第一个交点，negaPoints[negaPoints.size() - 1]为反向最后一个交点
* @param[out]. negaSurfIdx. 射线反方向与网格交点所在的三角片索引，与negaPoints一一对应
* @return. 射线正方向与网格的交点个数。若(count % 2 == 0)则射线起点在*封闭*网格外，若(count % 2 == 1)则射线起点在*封闭*网格内
*/
extern unsigned PMGetRayIntersect(const VSSimpleMeshF& mesh, const VFRay& ray,
	std::vector<VFVECTOR3>* posiPoints = nullptr, std::vector<unsigned>* posiSurfIdx = nullptr,
	std::vector<VFVECTOR3>* negaPoints = nullptr, std::vector<unsigned>* negaSurfIdx = nullptr);

//extern void PMBuildEllipse(std::vector<VFVECTOR3>& out, const unsigned nVertCount,
//	const float flLongAxis, const float flShortAxis, const float flAngle,
//	const VFRay& plane, const VFVECTOR3& start);