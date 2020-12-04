#include "stdafx.h"
#include "VBEDEncodeComb.h"

void VBEDEncodeComb::MemberInit()
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

void VBEDEncodeComb::CombFunc(const VSGraphMesh & gm, const float & deltaCoeff)
{
	//mesh to ovtable
	OVTable  ov;
	EDovt.Build(ov, gm);

	//edgebreaker compress
	CLERS   clers;
	unsigned startCorner = 0;
	EDcmp.Build(clers, ov, startCorner);
	m_vVertMapID.resize(clers.VertMapID.len);    //PCA
	memcpy(&m_vVertMapID.front(), clers.VertMapID.pData, sizeof(unsigned) * clers.VertMapID.len);

	m_vTriMapID.resize(clers.TriMapID.len);
	memcpy_s(&m_vTriMapID.front(), sizeof(unsigned) * clers.TriMapID.len, clers.TriMapID.pData, sizeof(unsigned) * clers.TriMapID.len);
		
//#define DEBUG
//#ifdef DEBUG	
//	//新旧点对应关系映射表互换
//	std::vector<unsigned>   m_vVertOrgToNew;        //角标为原始点ID，存储的为对应的新点的ID
//	m_vVertOrgToNew.resize(clers.VertMapID.len);
//	for (unsigned i = 0; i < clers.VertMapID.len; i++)
//		m_vVertOrgToNew[clers.VertMapID.pData[i]] = i;
//
//	std::vector<VNVECTOR3UI>   m_vNewSurf;
//	unsigned sNum = gm.GetSurfCount();
//	m_vNewSurf.resize(sNum);
//	unsigned* pSurfMap = EDcmp.GetSurfMapID();
//	for (unsigned i = 0; i < sNum; i++)
//	{
//		unsigned oldSurfID = pSurfMap[i];
//		m_vNewSurf[i].x = m_vVertOrgToNew[gm.pSurfaces[oldSurfID].x];
//		m_vNewSurf[i].y = m_vVertOrgToNew[gm.pSurfaces[oldSurfID].y];
//		m_vNewSurf[i].z = m_vVertOrgToNew[gm.pSurfaces[oldSurfID].z];
//	}
//#endif // DEBUG

	//clers  compress
	VSConstBuffer<char>   codeBin;
	cencode.Build(codeBin, clers.clers);
	m_vCodeBin.resize(codeBin.len);
	memcpy(&m_vCodeBin.front(), codeBin.pData, sizeof(char) * codeBin.len);

	//edgebreaker decompress
	VSEdgeBreakerDecRes  msh;
	edDec.Build(msh, clers);

	vert = clers.VertDelta.pData[0];

	//delta error compensate
	VSConstBuffer<VNVECTOR3I>   deltaInt;
	deltaProc.Build(deltaInt, clers.VertReal, clers.VertMapID, msh.vMapID, deltaCoeff);
	
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
	VSLossQuantization  lx;
	VSLossQuantization  ly;
	VSLossQuantization  lz;
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

void VBEDEncodeComb::LowBitProcess(std::vector<char>& vec, const VSLossQuantization & LowBit)
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

VD_EXPORT_SYSTEM_SIMPLE(VBEDEncodeComb, VNALGMESH::VREDEncodeComb);