#pragma once
#include <obj/alg/ialgwzk.h>
#include <sys/alg/isysmesh.h>
#include "VBGenCoord.h"

//球面座标映射为世界坐标

class VBSphereCoordMap2Vt
{
public:
	VBSphereCoordMap2Vt() {}
	~VBSphereCoordMap2Vt() {}

	void Build(VSConstBuffer<VFVECTOR3> & out, const VNWZKALG::VSSphereCoordMap<float> & sc, const VSConstBuffer<VFRay> & curveVt)
	{
		ReverseMap(sc, curveVt);

		out = VD_V2CB(m_vMapVt);
	}

private:
	void     ReverseMap(const VNWZKALG::VSSphereCoordMap<float> & sc, const VSConstBuffer<VFRay> & curveVt);
	unsigned IDproc(float idx, unsigned vNum);

private:
	std::vector<VFVECTOR3>     m_vMapVt;

	VBGenCoord                 m_GenCoord;
};
