#include "stdafx.h"
#include "VBEDLossnessEncComb.h"

void VBEDLossnessEncComb::MemberInit()
{
	Rmax = 1 << 31;
	Rmin = 1 << 23;

	m_vSignExpoEncode.clear();
	m_vMiddleEncode.clear();
	m_vEndMantissaEncode.clear();
	m_vSignExpo.clear();
	m_vMiddle.clear();
	m_vEndMantissa.clear();
}

void VBEDLossnessEncComb::CombFunc(VSFloatSplit<char> & enc, const VSGraphMesh & gm)
{
	//mesh to ovtable
	OVTable  ov;
	EDovt.Build(ov, gm);

	//edgebreaker compress
	CLERS   clers;
	unsigned startCorner = 0;
	EDcmp.Build(clers, ov, startCorner);

	//clers  compress
	Cencode.Build(codeBin, clers.clers);

	//float split
	VSFloatSplit<int>     FS;
	Fs.Build(FS, clers.VertPred, clers.VertReal);

	TranType(FS);

	//Range Encode
	RangeEncode();

	enc.flag           = FS.flag;
	enc.SignExpo       = VD_V2CB(m_vSignExpoEncode);
	enc.MiddleMantissa = VD_V2CB(m_vMiddleEncode);
	enc.EndMantissa    = VD_V2CB(m_vEndMantissaEncode);
}

void VBEDLossnessEncComb::TranType(VSFloatSplit<int>& FS)
{
	m_vSignExpo.resize(FS.SignExpo.len);
	m_vMiddle.resize(FS.MiddleMantissa.len);
	m_vEndMantissa.resize(FS.EndMantissa.len);

	for (unsigned i = 0; i < FS.flag.len; i++)
	{
		m_vSignExpo[i]    = (unsigned&)FS.SignExpo.pData[i];
		m_vMiddle[i]      = (unsigned&)FS.MiddleMantissa.pData[i];
		m_vEndMantissa[i] = (unsigned&)FS.EndMantissa.pData[i];
	}
}

void VBEDLossnessEncComb::RangeEncode()
{
	VSConstBuffer<unsigned>   signExpo;
	VSConstBuffer<unsigned>   middle;
	VSConstBuffer<unsigned>   endMantissa;
	signExpo = VD_V2CB(m_vSignExpo);
	middle = VD_V2CB(m_vMiddle);
	endMantissa = VD_V2CB(m_vEndMantissa);

	unsigned len = m_vMiddle.size() / 3;
	/*std::vector<unsigned>  tt;
	tt.resize(len);
	memcpy(&tt.front(), &m_vSignExpo.front(), sizeof(unsigned) * len);
	signExpo = VD_V2CB(tt);*/

	std::vector<unsigned>  tt1;
	tt1.resize(len);
	memcpy(&tt1.front(), &m_vMiddle.front(), sizeof(unsigned) * len);
	middle = VD_V2CB(tt1);

	std::vector<unsigned>  tt2;
	tt2.resize(len);
	memcpy(&tt2.front(), &m_vEndMantissa.front(), sizeof(unsigned) * len);
	endMantissa = VD_V2CB(tt2);

	VBRangeEncode           rgEncode(Rmax, Rmin);
	
	VSConstBuffer<unsigned>  res;

	//SignExponent
	unsigned RangeSize = 1 << 9;
	rgEncode.Build(res, signExpo, RangeSize);
	m_vSignExpoEncode.resize(res.len);
	memcpy(&m_vSignExpoEncode.front(), res.pData, sizeof(char) * res.len);

	//Middle Mantissa
	RangeSize = 1 << 11;
	rgEncode.Build(res, middle, RangeSize);
	m_vMiddleEncode.resize(res.len);
	memcpy(&m_vMiddleEncode.front(), res.pData, sizeof(char) * res.len);

	//End Mantissa
	RangeSize = 1 << 12;
	rgEncode.Build(res, endMantissa, RangeSize);
	m_vEndMantissaEncode.resize(res.len);
	memcpy(&m_vEndMantissaEncode.front(), res.pData, sizeof(char) * res.len);
}

VD_EXPORT_SYSTEM_SIMPLE(VBEDLossnessEncComb, VNALGMESH::VREDLossnessEncComb);