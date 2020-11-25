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
	��ά������;
	��Ա���ݣ�
			vector<VFVECTOR3> vVertice;����������ά�����vector��ʾ�Ķ��㼯
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
		// ��Ա����
		std::vector<VFVECTOR3> vVertice;
		std::vector<VNVECTOR3UI> vSurface;


		// ���캯��
		VSMesh() { Clear(); }

		//			�������캯�� 
		VSMesh(const VSMesh& in)	
		{
			vVertice.assign(in.vVertice.begin(), in.vVertice.end());
			vSurface.assign(in.vSurface.begin(), in.vSurface.end());
		}

		//			����VSSimpleMesh��ʾ����ά�������������
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
// PMMeshCenterFixSmooth�� PMMeshCenterFixSmoothWithBndry��ֻ�޸Ķ���
// λ�ã����ı����˹�ϵ��ά��ԭ���Ķ���˳��
extern void PMMeshCenterFixSmooth(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh);
extern void PMMeshCenterFixSmoothWithBndry(NM_PMMESH::VSMesh& out, const VSPerfectMesh& perfMesh);
//////////////////////////////////////////////////////////////////////////

extern void PMGetCircle2Dim(std::vector<VFVECTOR2>&out, const unsigned nPointCnt,const float flRadius = 1.0f);
extern void PMRemoveRepeatedPoint(NM_PMMESH::VSMesh& out, const NM_PMMESH::VSMesh& in);
/*
������
���������
VFVECTOR3& positivePoint	- ��������Ľ���
VFVECTOR3& negativePoint	- ���߷���Ľ���
���������
const VSSimpleMeshF& sm		- ����ģ������
const VFRay& ray			- �������ڲ����ⷢ�������
const bool blIsFirstTouch	- true�� ��һ�νӴ��� false�� ���һ�νӴ�
����������ray����ʼ���Ǵ�������ģ�͵��ڲ���������ģ���ⲿ�������ߣ�
���ҵ�һ�νӴ�����ģ�͵ĵ�������һ�νӴ�����ģ�͵ĵ㡣
*/
extern void PMGetTouchedPoint(VFVECTOR3& positivePoint, VFVECTOR3& negativePoint,
	const VSSimpleMeshF& sm, const VFRay& ray, const bool blIsFirstTouch = true);
// ���ߺ�ƽ���ཻ
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
* @brief. ����㼯��projPlaneƽ���ϵ���С���ԲԲ�ĺͰ뾶�����ȶԵ㼯��͹��
* @param[in]. verts ������㼯
* @param[out]. center ��С���ԲԲ��
* @param[out]. radius ��С���Բ�뾶
* @param[in][opl]. projPlane ͶӰƽ�棬Ĭ��xOyƽ��
* @return. true:�ɹ�; false:ʧ�ܣ�����������
*/
extern bool PMCalcMinBoundCircle(const VSConstBuffer<VFVECTOR3>& verts, VFVECTOR3* center, float* radius, const VFPlane* projPlane = nullptr);
/*
* @brief. ����㼯��projPlaneƽ���ϵ��������ԲԲ�ĺͰ뾶����Ҫ����Բ��
* @param[in]. verts ������㼯
* @param[in]. center �������ԲԲ��
* @param[out]. radius �������Բ�뾶
* @param[in][opl]. projPlane ͶӰƽ�棬Ĭ��xOyƽ��
* @return. true:�ɹ�; false:ʧ�ܣ�����������
*/
extern bool PMCalcMaxInscribedCircle(const VSConstBuffer<VFVECTOR3>& verts, const VFVECTOR3& center, float* radius, const VFPlane* projPlane = nullptr);
/*
* @brief. �жϵ㼯��projPlaneƽ�����Ƿ���ָ���뾶��Բ���ڣ������Բ��Բ��
* @param[in]. verts ������㼯
* @param[in]. outterRadius Բ���⾶
* @param[in]. innerRadius Բ���ھ�
* @param[in][out]. center ����Ϊ�㼯ԭ�㣬���Ϊ������ϵ�·���������Բ��Բ�ģ�Ϊnullptr��Ĭ��Ϊ����ԭ��
* @param[in][opl]. projPlane ͶӰƽ�棬Ĭ��xOyƽ��
* @return. true:�ɹ�; false:ʧ�ܣ�û�з���������Բ��
*/
extern bool PMCalcMinMaxCircle(const VSConstBuffer<VFVECTOR3>& verts, const float& outterRadius, const float& innerRadius,
	VFVECTOR3* center = nullptr, const VFPlane* projPlane = nullptr);
/*
* @brief. �����������񽻵�
* @param[in]. mesh. ��������
* @param[in]. ray.  ��������
* @param[out]. posiPoints.  ����������������Ľ��㣬posiPoints[0]Ϊ�����һ�����㣬posiPoints[posiPoints.size() - 1]Ϊ�������һ������
* @param[out]. posiSurfIdx. ���������������񽻵����ڵ�����Ƭ��������posiPointsһһ��Ӧ
* @param[out]. negaPoints.  ���߷�����������Ľ��㣬negaPoints[0]Ϊ�����һ�����㣬negaPoints[negaPoints.size() - 1]Ϊ�������һ������
* @param[out]. negaSurfIdx. ���߷����������񽻵����ڵ�����Ƭ��������negaPointsһһ��Ӧ
* @return. ����������������Ľ����������(count % 2 == 0)�����������*���*�����⣬��(count % 2 == 1)�����������*���*������
*/
extern unsigned PMGetRayIntersect(const VSSimpleMeshF& mesh, const VFRay& ray,
	std::vector<VFVECTOR3>* posiPoints = nullptr, std::vector<unsigned>* posiSurfIdx = nullptr,
	std::vector<VFVECTOR3>* negaPoints = nullptr, std::vector<unsigned>* negaSurfIdx = nullptr);

//extern void PMBuildEllipse(std::vector<VFVECTOR3>& out, const unsigned nVertCount,
//	const float flLongAxis, const float flShortAxis, const float flAngle,
//	const VFRay& plane, const VFVECTOR3& start);