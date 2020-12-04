#include "stdafx.h"
#include "VBEDCmpCombHandle.h"

void VBEDCmpCombHandle::MemberInit()
{
	Rmax = 1 << 31;
	Rmin = 1 << 23;
	vert.x = vert.y = vert.z = 0.f;

	m_vCompress.clear();
	m_vCodeBin.clear();
	m_vLowBitX.clear();
	m_vLowBitY.clear();
	m_vLowBitZ.clear();
	m_vHighBitX.clear();
	m_vHighBitY.clear();
	m_vHighBitZ.clear();
	m_vVertMapID.clear();
	m_vDeltaIntX.clear();
	m_vDeltaIntY.clear();
	m_vDeltaIntZ.clear();
	m_vBitSetTemp.clear();
}

void VBEDCmpCombHandle::CombFunc(const VSGraphMesh & gm, const unsigned & startCorner, const double & deltaCoeff)
{
	//mesh to ovtable
	OVTable  ov;
	m_objMesh2OV.Build(ov, gm);

	//find boundary
	VNWZKALG::VSHoleBoundary  Bndry;
	m_objFindBndry.Build(Bndry, gm);

	//edgebreaker compress
	HandleCmpRes   cmpRes;
	m_objCmpHandle.Build(cmpRes, ov, Bndry, startCorner);

	vert = cmpRes.delta.pData[0];

	//boundary copy
	if (cmpRes.bndryLen > 0)
	{
		m_vBndryV.resize(cmpRes.bndryLen);
		memcpy_s(&m_vBndryV.front(), sizeof(VFVECTOR3) * cmpRes.bndryLen, cmpRes.bndryVt.pData, sizeof(VFVECTOR3) * cmpRes.bndryLen);
		m_vVtMapIDwithBndry.resize(cmpRes.vDecMapID.len);
		memcpy_s(&m_vVtMapIDwithBndry.front(), sizeof(unsigned) * cmpRes.vDecMapID.len, cmpRes.vDecMapID.pData, sizeof(unsigned) * cmpRes.vDecMapID.len);
	}	

	//map relation
	m_vVertMapID.resize(cmpRes.vMapID.len);
	memcpy_s(&m_vVertMapID.front(), sizeof(unsigned) * cmpRes.vMapID.len, cmpRes.vMapID.pData, sizeof(unsigned) * cmpRes.vMapID.len);
	m_vTriMapID.resize(cmpRes.triMapID.len);
	memcpy_s(&m_vTriMapID.front(), sizeof(unsigned) * cmpRes.triMapID.len, cmpRes.triMapID.pData, sizeof(unsigned) * cmpRes.triMapID.len);
	m_vVtMapOld2New.resize(cmpRes.vOld2New.len);
	memcpy_s(&m_vVtMapOld2New.front(), sizeof(unsigned) * cmpRes.vOld2New.len, cmpRes.vOld2New.pData, sizeof(unsigned) * cmpRes.vOld2New.len);
	m_vTriMapOld2New.resize(cmpRes.triOld2New.len);
	memcpy_s(&m_vTriMapOld2New.front(), sizeof(unsigned) * cmpRes.triOld2New.len, cmpRes.triOld2New.pData, sizeof(unsigned) * cmpRes.triOld2New.len);

	//handle copy
	if (cmpRes.handles.len > 0)
	{
		m_vHandle.resize(cmpRes.handles.len);
		memcpy_s(&m_vHandle.front(), sizeof(VNVECTOR2UI) * cmpRes.handles.len, cmpRes.handles.pData, sizeof(VNVECTOR2UI) * cmpRes.handles.len);
	}
	bndryLen = cmpRes.bndryLen;

	//clers  compress
	VSConstBuffer<char>   codeBin;
	m_objCLERSProc.Build(codeBin, cmpRes.cmpRes);
	m_vCodeBin.resize(codeBin.len);
	memcpy(&m_vCodeBin.front(), codeBin.pData, sizeof(char) * codeBin.len);

	//edgebreaker decompress
	//DecResHole  decRes;
	//m_objDecHandle.Build(decRes, cmpRes);

	//delta compensation
	VSConstBuffer<VNVECTOR3I>   deltaInt;
    double deltaCoeffD = 1e-5;//static_cast<double>(deltaCoeff);
	if (startCorner == VD_INVALID_INDEX)
		m_objDeltaProc.Build(deltaInt, cmpRes.realCoord, cmpRes.vMapID, cmpRes.decRelation, cmpRes.decBndry, deltaCoeffD);
		//m_objDeltaProc.Build(deltaInt, cmpRes.realCoord, cmpRes.vMapID, decRes.relation, decRes.decBndry, deltaCoeffD);
	else
		m_objDelta.Build(deltaInt, cmpRes.realCoord, cmpRes.vMapID, cmpRes.decRelation, deltaCoeff);
		//m_objDelta.Build(deltaInt, cmpRes.realCoord, cmpRes.vMapID, decRes.relation, deltaCoeff);

	m_vDeltaIntX.resize(deltaInt.len);
	m_vDeltaIntY.resize(deltaInt.len);
	m_vDeltaIntZ.resize(deltaInt.len);
	for (unsigned i = 0; i < deltaInt.len; i++)
	{
		m_vDeltaIntX[i] = deltaInt.pData[i].x;
		m_vDeltaIntY[i] = deltaInt.pData[i].y;
		m_vDeltaIntZ[i] = deltaInt.pData[i].z;
	}
	VSConstBuffer<int>   vsx;
	VSConstBuffer<int>   vsy;
	VSConstBuffer<int>   vsz;
	vsx = VD_V2CB(m_vDeltaIntX);
	vsy = VD_V2CB(m_vDeltaIntY);
	vsz = VD_V2CB(m_vDeltaIntZ);

	// int loss process
	VNWZKALG::VSLossQuantization  lx;
	VNWZKALG::VSLossQuantization  ly;
	VNWZKALG::VSLossQuantization  lz;
	VBIntLossPorcess    objX;
	VBIntLossPorcess    objY;
	VBIntLossPorcess    objZ;
	objX.Build(lx, vsx);
	objY.Build(ly, vsy);
	objZ.Build(lz, vsz);

	//range encode
	VBRangeEncode    lxh(Rmax, Rmin);
	VBRangeEncode    lyh(Rmax, Rmin);
	VBRangeEncode    lzh(Rmax, Rmin);
	VSConstBuffer<unsigned>   xh;
	VSConstBuffer<unsigned>   yh;
	VSConstBuffer<unsigned>   zh;
	lxh.Build(xh, lx.highBitVal, 1 << lx.bitLen.x);
	lyh.Build(yh, ly.highBitVal, 1 << ly.bitLen.x);
	lzh.Build(zh, lz.highBitVal, 1 << lz.bitLen.x);

	//unsigned to char
	LowBitProcess(m_vLowBitX, lx);
	LowBitProcess(m_vLowBitY, ly);
	LowBitProcess(m_vLowBitZ, lz);

	m_vHighBitX.resize(xh.len);
	for (unsigned i = 0; i < xh.len; i++)
		m_vHighBitX[i] = static_cast<char>(xh.pData[i] & 0x000000FF);
	m_vHighBitY.resize(yh.len);
	for (unsigned i = 0; i < yh.len; i++)
		m_vHighBitY[i] = static_cast<char>(yh.pData[i] & 0x000000FF);
	m_vHighBitZ.resize(zh.len);
	for (unsigned i = 0; i < zh.len; i++)
		m_vHighBitZ[i] = static_cast<char>(zh.pData[i] & 0x000000FF);

	m_vCompress.resize(3);
	m_vCompress[0].bitLen = lx.bitLen;
	m_vCompress[0].min = lx.min;
	m_vCompress[0].lowBitVal = VD_V2CB(m_vLowBitX);
	m_vCompress[0].highBitCompVal = VD_V2CB(m_vHighBitX);
	m_vCompress[1].bitLen = ly.bitLen;
	m_vCompress[1].min = ly.min;
	m_vCompress[1].lowBitVal = VD_V2CB(m_vLowBitY);
	m_vCompress[1].highBitCompVal = VD_V2CB(m_vHighBitY);
	m_vCompress[2].bitLen = lz.bitLen;
	m_vCompress[2].min = lz.min;
	m_vCompress[2].lowBitVal = VD_V2CB(m_vLowBitZ);
	m_vCompress[2].highBitCompVal = VD_V2CB(m_vHighBitZ);

	m_vDeltaIntX.clear();
	m_vDeltaIntY.clear();
	m_vDeltaIntZ.clear();
	m_vBitSetTemp.clear();
}

void VBEDCmpCombHandle::LowBitProcess(std::vector<char>& vec, const VNWZKALG::VSLossQuantization & LowBit)
{
	unsigned totalLowBitLen = LowBit.bitLen.y * LowBit.lowBitVal.len;
	m_vBitSetTemp.resize(totalLowBitLen);

	unsigned count = 0;
	for (unsigned i = 0; i < LowBit.lowBitVal.len; i++)
	{
		for (int j = LowBit.bitLen.y - 1; j >= 0; j--)
		{
			m_vBitSetTemp[count] = LowBit.lowBitVal.pData[i] >> j & 0x1;
			count++;
		}
	}

	count = 0;
	vec.resize(static_cast<unsigned>(ceilf(static_cast<float>(totalLowBitLen) / 8.f)));
	unsigned residueLen = m_vBitSetTemp.size() - totalLowBitLen / 8 * 8;
	for (unsigned i = 0; i < m_vBitSetTemp.size() - residueLen; i += 8)
	{
		char temp = 0;
		for (unsigned j = 0; j < 8; j++)
		{
			temp |= (static_cast<char>(m_vBitSetTemp[i + j])) << (7 - j);
		}
		vec[count] = temp;
		count++;
	}

	//the residue bites
	if (residueLen != 0)
	{
		char temp = 0;
		for (unsigned i = 0; i < residueLen; i++)
		{
			temp |= (static_cast<char>(m_vBitSetTemp[m_vBitSetTemp.size() - residueLen + i])) << (7 - i);
		}
		vec[count] = temp;
	}
}
