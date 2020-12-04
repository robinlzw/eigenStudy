#include "stdafx.h"
#include "VBSphereCoordMap2Vt.h"

void VBSphereCoordMap2Vt::ReverseMap(const VNWZKALG::VSSphereCoordMap<float> & sc, const VSConstBuffer<VFRay>& curveVt)
{
	LocalCoord    localC;
	m_GenCoord.Build(localC, curveVt);

	unsigned mapNum = sc.idx.len;
	m_vMapVt.resize(mapNum);
	VFVECTOR3 temp;
	for (unsigned i = 0; i < mapNum; i++)
	{
		unsigned idPos = IDproc(sc.idx.pData[i], curveVt.len);		
		temp.x = sc.norm2.pData[ i ] * cosf(sc.phi.pData[ i ]) * cosf(sc.theta.pData[ i ]);
		temp.y = sc.norm2.pData[ i ] * cosf(sc.phi.pData[ i ]) * sinf(sc.theta.pData[ i ]);
		temp.z = sc.norm2.pData[ i ] * sinf(sc.phi.pData[ i ]);

		m_vMapVt[i] = temp.x * localC.coordX.pData[idPos] + temp.y * localC.coordY.pData[idPos] + temp.z * localC.coordZ.pData[idPos] + curveVt.pData[idPos].GetOrigin();
	}
}

unsigned VBSphereCoordMap2Vt::IDproc(float idx, unsigned vNum)
{
	if (idx < 0.f)
		return 0;
	else if (idx > static_cast<float>(vNum))
		return vNum;
	else
		return static_cast<unsigned>(roundf(idx));
}

VD_EXPORT_SYSTEM_SIMPLE(VBSphereCoordMap2Vt, VNALGMESH::VRSphereCoordMap2Vt);