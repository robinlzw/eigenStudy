#pragma once
#include <bitset>
#include "VBMeshToOVTable.h"
#include "VBEDCompress.h"
#include "VBCLERSEncode.h"
#include "VBFloatQuantization.h"
#include "VBIntSplit.h"
#include "VBRangeEncode.h"
#include "VBIntLossPorcess.h"
#include "VBFloatLossMerge.h"
#include "VBEDDecompression.h"
#include "VBDeltaProcess.h"
#include "VBQuantizationToFloat.h"
#include "VBFloatSplit.h"

#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//Íø¸ñÑ¹Ëõ×éºÏ

//struct resCompress
//{
//	int                      min;
//	VNVECTOR2UI              bitLen;      //x: high bit len, y: low bit len
//	VSConstBuffer<char>      highBitCompVal;
//	VSConstBuffer<char>      lowBitVal;
//};

//struct EDDecInfo
//{
//	unsigned                      vertLen;
//	unsigned                      triLen;
//	VFVECTOR3                     firstVert;	
//	VSConstBuffer<char>           TopoBin;
//	VSConstBuffer<resCompress>    compRes;
//};

class VBEDEncodeComb
{
public:
	VBEDEncodeComb() : Rmax(1 << 31), Rmin(1 << 23), vert(0.f, 0.f, 0.f){}
	~VBEDEncodeComb() {}

	void Build(VSEdgeBreakerDecInfo & res, const VSGraphMesh & gm, const float & deltaCoeff)
	{
		MemberInit();
		CombFunc(gm, deltaCoeff);		

		res.vertLen    = gm.GetVertexCount();
		res.triLen     = gm.GetSurfCount();
		res.firstVert  = vert;
		res.TopoBin    = VD_V2CB(m_vCodeBin);
		res.compRes    = VD_V2CB(m_vCompress);
		res.vertMapID  = VD_V2CB(m_vVertMapID);	
		res.triMapID   = VD_V2CB(m_vTriMapID);
	}

private:
	void  MemberInit();
	void  CombFunc(const VSGraphMesh & gm, const float & deltaCoeff);
	void  LowBitProcess(std::vector<char> & vec, const VSLossQuantization & LowBit);

private:
	VBMeshToOVTable         EDovt;
	VBEDCompress            EDcmp;
	VBCLERSEncode           cencode;

	VBEDDecompression       edDec;
	VBDeltaProcess          deltaProc;

private:
	std::vector<VSCompressRes>    m_vCompress;
	std::vector<char>             m_vCodeBin;
	std::vector<char>             m_vLowBitX;
	std::vector<char>             m_vLowBitY;
	std::vector<char>             m_vLowBitZ;
	std::vector<char>             m_vHighBitX;
	std::vector<char>             m_vHighBitY;
	std::vector<char>             m_vHighBitZ;
	std::vector<unsigned>         m_vVertMapID;
	std::vector<unsigned>         m_vTriMapID;
							     
	std::vector<int>              m_vDeltaIntX;
	std::vector<int>              m_vDeltaIntY;
	std::vector<int>              m_vDeltaIntZ;
	std::vector<bool>             m_vBitSetTemp;

private:
	unsigned    Rmin;
	unsigned    Rmax;
	VFVECTOR3   vert;	
};
