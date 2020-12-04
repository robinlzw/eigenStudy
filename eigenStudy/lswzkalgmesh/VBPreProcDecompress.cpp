#include "stdafx.h"
#include "VBPreProcDecompress.h"

void VBPreProcDecompress::Process(const VNMesh::VSCompressedMesh & cmp)
{
	Data(cmp);
	float delta = static_cast<float>(1e-5);	
	Dec.Build(res, info, delta);
}

void VBPreProcDecompress::Data(const VNMesh::VSCompressedMesh & cmp)
{
	info.vertLen = cmp.vertCount;
	info.triLen = cmp.triCount;
	info.delVertID = cmp.delVertID;
	info.TopoBin = cmp.clers;
	info.firstVert = cmp.firstVert;
	info.bndryLen = cmp.bndryV.len;
	info.bndryV = cmp.bndryV;
	info.handles = cmp.handles;

	m_vTemp.resize(3);
	m_vTemp[0].min = cmp.xVert.minValue;
	m_vTemp[0].bitLen.x = cmp.xVert.highBitLen;
	m_vTemp[0].bitLen.y = cmp.xVert.lowBitLen;
	m_vTemp[0].highBitCompVal = cmp.xVert.highBitData;
	m_vTemp[0].lowBitVal = cmp.xVert.lowBitData;
	m_vTemp[1].min = cmp.yVert.minValue;
	m_vTemp[1].bitLen.x = cmp.yVert.highBitLen;
	m_vTemp[1].bitLen.y = cmp.yVert.lowBitLen;
	m_vTemp[1].highBitCompVal = cmp.yVert.highBitData;
	m_vTemp[1].lowBitVal = cmp.yVert.lowBitData;
	m_vTemp[2].min = cmp.zVert.minValue;
	m_vTemp[2].bitLen.x = cmp.zVert.highBitLen;
	m_vTemp[2].bitLen.y = cmp.zVert.lowBitLen;
	m_vTemp[2].highBitCompVal = cmp.zVert.highBitData;
	m_vTemp[2].lowBitVal = cmp.zVert.lowBitData;
	info.compRes = VD_V2CB(m_vTemp);
}

VD_EXPORT_SYSTEM_SIMPLE(VBPreProcDecompress, VNALGMESH::VRPreProcDecompress);