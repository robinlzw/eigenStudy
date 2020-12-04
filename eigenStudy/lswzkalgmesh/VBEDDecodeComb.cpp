#include "stdafx.h"
#include "VBEDDecodeComb.h"

void VBEDDecodeComb::MemberInit()
{
	Rmax = 1 << 31;
	Rmin = 1 << 23;

	m_vDelta.clear();
	m_vHighBitValX.clear();
	m_vHighBitValY.clear();
	m_vHighBitValZ.clear();
	m_vLowBitValX.clear();
	m_vLowBitValY.clear();
	m_vLowBitValZ.clear();
	m_vBitSetTemp.clear();
}

void VBEDDecodeComb::CombFunc(const VSEdgeBreakerDecInfo & dec, const float & deltaCoeff)
{
	//clers decompress
	VSConstBuffer<char>   decClers;
	cdecode.Build(decClers, dec.TopoBin, dec.triLen - 1);

	//range decode
	Decode(dec);

	//int merge to float	
	MergeToFloat(dec, deltaCoeff);
	double deltaCoeffD = deltaCoeff;
	MergeToDouble(dec, deltaCoeffD);              //////////double

	//m_vDelta[0] = dec.firstVert;
	m_vDeltaTemp.resize(m_vDelta.size() + 1);													  ///////////
	m_vDeltaTemp[0] = dec.firstVert;															  ///////////
	memcpy(&m_vDeltaTemp[1].x, &m_vDelta.front().x, sizeof(VFVECTOR3) * m_vDelta.size());		  ///////////

	//double
	m_vDeltaTempD.resize(m_vDelta.size() + 1);														 ////////
	m_vDeltaTempD[0].x = dec.firstVert.x;															 ////////
	m_vDeltaTempD[0].y = dec.firstVert.y;															 ////////
	m_vDeltaTempD[0].z = dec.firstVert.z;															 ////////
	memcpy(&m_vDeltaTempD[1].x, &m_vDeltaD.front().x, sizeof(DLE3) * m_vDeltaD.size());		 ////////

	//edgebreaker decompress
	CLERS  clers1;
	clers1.clers = decClers;
	//clers1.VertDelta = VD_V2CB(m_vDelta);
	clers1.VertDelta = VD_V2CB(m_vDeltaTemp);    //////////////
	clers1.VertDeltaD = VD_V2CB(m_vDeltaTempD);    //////////////double
	edDec.Build(msh, clers1);
}

void VBEDDecodeComb::MergeToFloat(const VSEdgeBreakerDecInfo & dec, const float & deltaCoeff)
{
	VSConstBuffer<unsigned>   highX = VD_V2CB(m_vHighBitValX);
	VSConstBuffer<unsigned>   highY = VD_V2CB(m_vHighBitValY);
	VSConstBuffer<unsigned>   highZ = VD_V2CB(m_vHighBitValZ);
	VSConstBuffer<unsigned>   lowX = VD_V2CB(m_vLowBitValX);
	VSConstBuffer<unsigned>   lowY = VD_V2CB(m_vLowBitValY);
	VSConstBuffer<unsigned>   lowZ = VD_V2CB(m_vLowBitValZ);

	VSConstBuffer<float>     vx;
	VSConstBuffer<float>     vy;
	VSConstBuffer<float>     vz;
	VBFloatLossMerge    mx;
	VBFloatLossMerge    my;
	VBFloatLossMerge    mz;
	mx.Build(vx, highX, lowX, deltaCoeff, dec.compRes.pData[0].bitLen.y, dec.compRes.pData[0].min);
	my.Build(vy, highY, lowY, deltaCoeff, dec.compRes.pData[1].bitLen.y, dec.compRes.pData[1].min);
	mz.Build(vz, highZ, lowZ, deltaCoeff, dec.compRes.pData[2].bitLen.y, dec.compRes.pData[2].min);

	m_vDelta.resize(vx.len);
	for (unsigned i = 0; i < vx.len; i++)
	{
		m_vDelta[i].x = vx.pData[i];
		m_vDelta[i].y = vy.pData[i];
		m_vDelta[i].z = vz.pData[i];
	}
}

void VBEDDecodeComb::Decode(const VSEdgeBreakerDecInfo & dec)
{
	TypeTran(dec);

	VBRangeDecode   objDecX(Rmax, Rmin);
	VBRangeDecode   objDecY(Rmax, Rmin);
	VBRangeDecode   objDecZ(Rmax, Rmin);

	VSConstBuffer<unsigned>   xHighBit;
	VSConstBuffer<unsigned>   yHighBit;
	VSConstBuffer<unsigned>   zHighBit;	

	VSConstBuffer<unsigned>   xh = VD_V2CB(m_vHighBitValX);
	VSConstBuffer<unsigned>   yh = VD_V2CB(m_vHighBitValY);
	VSConstBuffer<unsigned>   zh = VD_V2CB(m_vHighBitValZ);

	objDecX.Build(xHighBit, xh, 1 << dec.compRes.pData[0].bitLen.x, dec.vertLen - 1);		 /////////////
	objDecY.Build(yHighBit, yh, 1 << dec.compRes.pData[1].bitLen.x, dec.vertLen - 1);		 /////////////
	objDecZ.Build(zHighBit, zh, 1 << dec.compRes.pData[2].bitLen.x, dec.vertLen - 1);		 /////////////

	m_vHighBitValX.resize(xHighBit.len);
	m_vHighBitValY.resize(yHighBit.len);
	m_vHighBitValZ.resize(zHighBit.len);

	memcpy(&m_vHighBitValX.front(), xHighBit.pData, sizeof(unsigned) * xHighBit.len);
	memcpy(&m_vHighBitValY.front(), yHighBit.pData, sizeof(unsigned) * yHighBit.len);
	memcpy(&m_vHighBitValZ.front(), zHighBit.pData, sizeof(unsigned) * zHighBit.len);
}

void VBEDDecodeComb::TypeTran(const VSEdgeBreakerDecInfo & dec)
{	
	m_vHighBitValX.resize(dec.compRes.pData[0].highBitCompVal.len);
	for (unsigned i = 0; i < m_vHighBitValX.size(); i++)
	{
		m_vHighBitValX[i] = static_cast<unsigned>((dec.compRes.pData[0].highBitCompVal.pData[i] << 24 & 0xFF000000) >> 24);
	}
	
	m_vHighBitValY.resize(dec.compRes.pData[1].highBitCompVal.len);
	for (unsigned i = 0; i < m_vHighBitValY.size(); i++)
	{
		m_vHighBitValY[i] = static_cast<unsigned>((dec.compRes.pData[1].highBitCompVal.pData[i] << 24 & 0xFF000000) >> 24);
	}
	
	m_vHighBitValZ.resize(dec.compRes.pData[2].highBitCompVal.len);
	for (unsigned i = 0; i < m_vHighBitValZ.size(); i++)
	{
		m_vHighBitValZ[i] = static_cast<unsigned>((dec.compRes.pData[2].highBitCompVal.pData[i] << 24 & 0xFF000000) >> 24);
	}
	
	LowBitPorcess(m_vLowBitValX, dec.compRes.pData[0].lowBitVal, (dec.vertLen - 1) * dec.compRes.pData[0].bitLen.y, dec.compRes.pData[0].bitLen.y);	 /////////////
	LowBitPorcess(m_vLowBitValY, dec.compRes.pData[1].lowBitVal, (dec.vertLen - 1) * dec.compRes.pData[1].bitLen.y, dec.compRes.pData[1].bitLen.y);	 /////////////
	LowBitPorcess(m_vLowBitValZ, dec.compRes.pData[2].lowBitVal, (dec.vertLen - 1) * dec.compRes.pData[2].bitLen.y, dec.compRes.pData[2].bitLen.y);	 /////////////
}

void VBEDDecodeComb::LowBitPorcess(std::vector<unsigned>& lowBitValue, const VSConstBuffer<char>& lowBitChar, const unsigned & totalLowBitLen, const unsigned & lowBitLen)
{
	VASSERT(lowBitChar.len * 8 >= totalLowBitLen);
	m_vBitSetTemp.resize(totalLowBitLen);

	unsigned count = 0;
	for (unsigned i = 0; i < lowBitChar.len - 1; i++)
	{
		for (unsigned j = 0; j < 8; j++)
		{
			m_vBitSetTemp[count] = lowBitChar.pData[i] >> (7 - j) & 1;
			count++;
		}
	}
	for (unsigned i = 0; i < 8; i++)
	{
		m_vBitSetTemp[count] = lowBitChar.pData[lowBitChar.len - 1] >> (7 - i) & 1;
		count++;
		if (count >= totalLowBitLen)
			break;
	}

	count = 0;
	lowBitValue.resize(totalLowBitLen / lowBitLen);	
	for (unsigned i = 0; i < m_vBitSetTemp.size(); i += lowBitLen)
	{
		unsigned temp = 0;
		for (unsigned j = 0; j < lowBitLen; j++)
		{
			temp |= m_vBitSetTemp[i + j] << (lowBitLen - j - 1);
		}
		lowBitValue[count] = temp;
		count++;
	}
}

void VBEDDecodeComb::MergeToDouble(const VSEdgeBreakerDecInfo & dec, const double deltaCoeff)
{
	VSConstBuffer<unsigned>   highX = VD_V2CB(m_vHighBitValX);
	VSConstBuffer<unsigned>   highY = VD_V2CB(m_vHighBitValY);
	VSConstBuffer<unsigned>   highZ = VD_V2CB(m_vHighBitValZ);
	VSConstBuffer<unsigned>   lowX = VD_V2CB(m_vLowBitValX);
	VSConstBuffer<unsigned>   lowY = VD_V2CB(m_vLowBitValY);
	VSConstBuffer<unsigned>   lowZ = VD_V2CB(m_vLowBitValZ);

	MergeD(m_vMergeX, highX, lowX, deltaCoeff, dec.compRes.pData[0].bitLen.y, dec.compRes.pData[0].min);
	MergeD(m_vMergeY, highY, lowY, deltaCoeff, dec.compRes.pData[1].bitLen.y, dec.compRes.pData[1].min);
	MergeD(m_vMergeZ, highZ, lowZ, deltaCoeff, dec.compRes.pData[2].bitLen.y, dec.compRes.pData[2].min);

	m_vDeltaD.resize(m_vMergeX.size());
	for (unsigned i = 0; i < m_vMergeX.size(); i++)
	{
		m_vDeltaD[i].x = m_vMergeX[i];
		m_vDeltaD[i].y = m_vMergeY[i];
		m_vDeltaD[i].z = m_vMergeZ[i];
	}
}

void VBEDDecodeComb::MergeD(std::vector<double> & merg, const VSConstBuffer<unsigned> & highBit, const VSConstBuffer<unsigned> & lowBit, const double delta, const unsigned & lowBitLen, const int & min)
{
	merg.resize(highBit.len);

	for (unsigned i = 0; i < merg.size(); i++)
	{
		int a = ((highBit.pData[i] << lowBitLen) | lowBit.pData[i]) + min;
		double b = a * delta;
		merg[i] = (static_cast<int>((highBit.pData[i] << lowBitLen) | lowBit.pData[i]) + min) * delta;
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBEDDecodeComb, VNALGMESH::VREDDecodeComb);