#pragma once
#include <obj/alg/ialgwzk.h>
#include <sys/alg/isysmesh.h>
#include "VBGenCoord.h"
#include "VBKDTree.h"
#include "VBKNearestNeighbor.h"

//世界坐标映射为球面坐标

class VBVtMap2SphereCoord
{
public:
	VBVtMap2SphereCoord() {}
	~VBVtMap2SphereCoord() {}

	void Build(VNWZKALG::VSSphereCoordMap<float> & out, const VSConstBuffer<VFRay> & curveVt, const VSConstBuffer<VFVECTOR3> & mapVt)
	{
		SphereCoordMap(curveVt, mapVt);

		out.idx   = VD_V2CB(m_vIdx);
		out.norm2 = VD_V2CB(m_vNorm2);
		out.theta = VD_V2CB(m_vTheta);
		out.phi   = VD_V2CB(m_vPhi);
	}

private:
	void  BuildTree(const VSConstBuffer<VFRay> & curveVt);
	void  SphereCoordMap(const VSConstBuffer<VFRay> & curveVt, const VSConstBuffer<VFVECTOR3> & mapVt);

private:
	std::vector<float>         m_vIdx;
	std::vector<float>         m_vNorm2;
	std::vector<float>         m_vTheta;
	std::vector<float>         m_vPhi;

	std::vector<VFVECTOR3>     m_vCurveVt;
	VSConstBuffer<VFVECTOR3>   cbf;

	VBKDTree               m_KdTree;
	VBKNearestNeighbor     m_KNN;
	VNALGMESH::VSKDTree    vst;
	VBGenCoord             m_GenCoord;
};
