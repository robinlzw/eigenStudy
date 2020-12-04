#pragma once
#include "VBEDEncodeComb.h"
#include "VBPCACompress.h"
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

#include "VBEDDecodeComb.h"
#include "VBPCARestore.h"

//ÑÀö¸Ñ¹ËõËã·¨×éºÏ

//struct FrameCompressRes
//{
//	VNWZKALG::VSEdgeBreakerDecInfo             orgMesh;
//	VSConstBuffer<float>                       mean;
//	VNWZKALG::VSCompressRes                     U;
//	VSConstBuffer<float>                        D;
//	VSConstBuffer<VNWZKALG::VSCompressRes>      V;
//};

class VBEDPCAEncodeComb
{
public:
	VBEDPCAEncodeComb() {}
	~VBEDPCAEncodeComb() {}

	void Build(VSFrameCompressRes & res, const VSGraphMesh & gm, const VSConstBuffer< VSConstBuffer< VFVECTOR3 > > & vert, const float & deltaCoeff)
	{		
		MemberInit();
		CombFunc(gm, vert, deltaCoeff);

		res = m_FrameRes;
	}

private:
	void  MemberInit();
	void  CombFunc(const VSGraphMesh & gm, const VSConstBuffer< VSConstBuffer< VFVECTOR3 > > & vert, const float & deltaCoeff);
	void  VCompress(const float & deltaCoeff);
	
	void  FirstFrameDel(const VSConstBuffer< VSConstBuffer< VFVECTOR3 > > & vert);
	void  FloatProc(int & minI, VNVECTOR2UI & bitlen, std::vector<char> & lowBit, std::vector<char> & highBit, const std::vector<float> & data, const float & deltaCoeff);
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
									      
	VNWZKALG::VSEdgeBreakerDecInfo         m_OrgMesh;
	UDVmat                                 m_UDVdata;
	VSFrameCompressRes                     m_FrameRes;
									      
	VBEDEncodeComb    m_objEncode;
	VBPCACompress     m_objPCAComp;
	VBIntLossPorcess  m_objIntProc;
	unsigned          Rmax;
	unsigned          Rmin;
};