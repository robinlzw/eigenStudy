#pragma once
#include "VBEDEncodeComb.h"
#include "VBPCACompressNED.h"
#include <obj/alg/ialgwzk.h>

//PCAÑ¹Ëõ×éºÏ

class VBPCAEncodeComb
{
public:
	VBPCAEncodeComb() {}
	~VBPCAEncodeComb() {}

	void Build(VNWZKALG::VSFrameCompressResNoED & res, const VSConstBuffer< VSConstBuffer< VFVECTOR3 > > & FrameVert)
	{
		MemberInit();
		CombFunc(FrameVert);

		res = m_FrameRes;
	}

private:
	void  MemberInit();
	void  CombFunc(const VSConstBuffer< VSConstBuffer< VFVECTOR3 > > & vert);
	void  VCompress(const float & deltaCoeff);

	void  FloatProc(int & minI, VNVECTOR2UI & bitlen, std::vector<char> & lowBit, std::vector<char> & highBit, const std::vector<float> & data, const double & deltaCoeff);
	void  LowBitProcess(std::vector<char> & vec, const VSLossQuantization & LowBit);

private:
	std::vector<char>                      m_vULowBit;
	std::vector<char>                      m_vUHihtBit;
	std::vector<VNWZKALG::VSCompressRes >  m_vVCompres;
	std::vector<std::vector<char>>         m_vVLowBit;
	std::vector<std::vector<char>>         m_vVHighBit;
	std::vector<float>                     m_vMean;

	std::vector<unsigned>                  m_vVertMapID;
	std::vector<bool>                      m_vBitSetTemp;
	std::vector<float>                     m_vTemp;
	std::vector<std::vector<VFVECTOR3>>    m_vFrame;
	std::vector<VSConstBuffer<VFVECTOR3>>  m_vRemainFrame;

	//VNWZKALG::VSEdgeBreakerDecInfo         m_OrgMesh;
	UDVmat                                 m_UDVdata;
	VNWZKALG::VSFrameCompressResNoED        m_FrameRes;

	//VBEDEncodeComb    m_objEncode;
	VBPCACompressNED     m_objPCAComp;
	VBIntLossPorcess  m_objIntProc;
	unsigned          Rmax;
	unsigned          Rmin;

};
