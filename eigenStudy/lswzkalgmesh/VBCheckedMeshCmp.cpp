#include "stdafx.h"
#include "VBCheckedMeshCmp.h"

void VBCheckedMeshCmp::Process(const VSSimpleMeshF & smesh, const unsigned & startCorner, const float & coeff)
{
	TVExtSource< VNALGMESH::VRPerfMesh > extsysEC;
	TVR2B< VNALGMESH::VRPerfMesh >  m_PerfMesh(*extsysEC);
	VSPerfectMesh gm = m_PerfMesh.Run(smesh).Get<VSPerfectMesh>();
	
	m_objCmpCombHandle.Build(res, gm, startCorner, coeff);

	checkedRes.vertCount = res.vertLen;
	checkedRes.triCount = res.triLen;
	checkedRes.firstVert = res.firstVert;

	checkedRes.xVert.minValue = res.compRes.pData[0].min;
	checkedRes.xVert.highBitLen = res.compRes.pData[0].bitLen.x;
	checkedRes.xVert.highBitData = res.compRes.pData[0].highBitCompVal;
	checkedRes.xVert.lowBitLen = res.compRes.pData[0].bitLen.y;
	checkedRes.xVert.lowBitData = res.compRes.pData[0].lowBitVal;

	checkedRes.yVert.minValue = res.compRes.pData[1].min;
	checkedRes.yVert.highBitLen = res.compRes.pData[1].bitLen.x;
	checkedRes.yVert.highBitData = res.compRes.pData[1].highBitCompVal;
	checkedRes.yVert.lowBitLen = res.compRes.pData[1].bitLen.y;
	checkedRes.yVert.lowBitData = res.compRes.pData[1].lowBitVal;

	checkedRes.zVert.minValue = res.compRes.pData[2].min;
	checkedRes.zVert.highBitLen = res.compRes.pData[2].bitLen.x;
	checkedRes.zVert.highBitData = res.compRes.pData[2].highBitCompVal;
	checkedRes.zVert.lowBitLen = res.compRes.pData[2].bitLen.y;
	checkedRes.zVert.lowBitData = res.compRes.pData[2].lowBitVal;

	checkedRes.clers = res.TopoBin;	
	checkedRes.handles = res.handles;
	checkedRes.bndryV = res.bndryV;
}