#pragma once

#include "VBMeshToOVTable.h"
#include "VBEDCompress.h"
#include "VBCLERSEncode.h"
#include "VBFloatSplit.h"
#include "VBIntSplit.h"
#include "VBRangeEncode.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//无损网格压缩组合

//struct lossnessDecInfo
//{
//	unsigned                vertLen;
//	unsigned                triLen;
//	VSConstBuffer<char>     TopoBin;
//	FloatSplit<char>        compRes;
//};

class VBEDLossnessEncComb
{
public:
	VBEDLossnessEncComb():Rmax(1 << 31), Rmin(1 << 23) {}
	~VBEDLossnessEncComb() {}

	void Build(VSLossnessDecInfo & res, const VSGraphMesh & gm)
	{
		MemberInit();
		CombFunc(Enc, gm);

		res.vertLen = gm.GetVertexCount();
		res.triLen = gm.GetSurfCount();
		res.TopoBin = codeBin;
		res.compRes = Enc;
	}

private:
	void MemberInit();
	void CombFunc(VSFloatSplit<char> & enc, const VSGraphMesh & gm);
	void TranType(VSFloatSplit<int> & FS);
	void RangeEncode();

private:
	VBMeshToOVTable         EDovt;
	VBEDCompress            EDcmp;
	VBCLERSEncode           Cencode;
	VBFloatSplit            Fs;

	VSConstBuffer<char>     codeBin;

private:
	std::vector<char>       m_vSignExpoEncode;
	std::vector<char>       m_vMiddleEncode;
	std::vector<char>       m_vEndMantissaEncode;

	std::vector<unsigned>   m_vSignExpo;
	std::vector<unsigned>   m_vMiddle;
	std::vector<unsigned>   m_vEndMantissa;

private:
	unsigned             Rmax;
	unsigned             Rmin;
	VSFloatSplit<char>   Enc;
};