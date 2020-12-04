#pragma once
#include "VBKDTree.h"
#include "VBKNearestNeighbor.h"
#include "Quaternion.h"
#include "../lswzkalgeq/VBDnsMatrixInverse.h"
#include <obj/lsalgtools/ilsalgtools.h>

class VBWaxVerticesMap
{
public:
	VBWaxVerticesMap()
	{
		num = INIGetInt(L"WaxBasePointNb", L"./config.ini");
		if (0 == num)
		{
			num = 40;
		}
	}
	~VBWaxVerticesMap() {}

	//theta为弧度值
	void Build(VSConstBuffer<VSConstBuffer<unsigned>> & out, const VSConstBuffer<VSSimpleMeshF> & gm, const float & theta)
	{
		WaxMap(gm.pData[0], gm.pData[1], theta);

		out = VD_V2CB(m_vOut);
	}
	/////////////////只是为填蜡算法新增加接口////////////////
	//theta为弧度值
	void BuildFillWax(VSConstBuffer<VSConstBuffer<unsigned>> & out, const VSConstBuffer<VSSimpleMeshF> & gm,
		const VSConstBuffer<VFRay> & toothAxis, const VSConstBuffer<VFPlane> & toothSurf, const float & theta)
	{
		WaxMapFillWax(gm.pData[0], gm.pData[1], toothAxis.pData[0], toothAxis.pData[1], toothSurf.pData[0], toothSurf.pData[1], theta);

		out = VD_V2CB(m_vOut);
	}

private:
	void       WaxMap(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2, const float & theta);
	VFVECTOR3  MeshBarycenter(const VSSimpleMeshF & gm);
	void       InitPoint(const float & theta, const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2);
	void       CreateKDTree(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2);
	void       FindNNPoint(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2);
	void       SmoothNNPoint();
	void       CalPoint(const float & theta);
	void       CreateMatrix();
	////////////////
	void       WaxMapFillWax(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2,
		const VFRay & tAxis1, const VFRay & tAxis2, const VFPlane &tSurf1, const VFPlane &tSurf2, const float & theta);
	void       InitPointFillWax(const float & theta, const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2, const VFVECTOR3 & tAxisAveDir);
	void       OptimalPointFillWax1(const VSSimpleMeshF & gm1, const VFPlane &tSurf1, const VFRay & tAxis1);
	void       OptimalPointFillWax2(const VSSimpleMeshF & gm2, const VFPlane &tSurf2, const VFRay & tAxis2);

private:
	unsigned num;
	std::vector<VSConstBuffer<unsigned>>  m_vOut;
	std::vector<VFVECTOR3>            m_vCirclePoint;
	std::vector<unsigned>             m_vMeshNNVertices1;
	std::vector<unsigned>             m_vMeshNNVertices2;
	
	std::vector<float>                m_vSmoothMatrix;
	std::vector<VFVECTOR3>            m_vMeshVert1;
	std::vector<VFVECTOR3>            m_vMeshVert2;
	std::vector<VFVECTOR3>            m_vMeshSmoothVert1;
	std::vector<VFVECTOR3>            m_vMeshSmoothVert2;
	VBKDTree                  tree1, tree2;
	VNALGMESH::VSKDTree       out1, out2;
	VBKNearestNeighbor        knn1, knn2;
	VSConstBuffer<VFVECTOR3>  cbf1, cbf2;
};
