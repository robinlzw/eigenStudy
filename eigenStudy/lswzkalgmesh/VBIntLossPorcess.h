#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//整形数有损处理

//struct lossQuantization
//{
//	int                          min;
//	VNVECTOR2UI                  bitLen;      //x: high bit len, y: low bit len
//	VSConstBuffer<unsigned>      highBitVal;
//	VSConstBuffer<unsigned>      lowBitVal;
//};

class VBIntLossPorcess
{
public:
	VBIntLossPorcess() {}
	~VBIntLossPorcess() {}

	void Build(VSLossQuantization & split, const VSConstBuffer<int> & val)
	{		
		split.min = IntToUnsigned(val);
		split.bitLen = bitlen;
		split.highBitVal = VD_V2CB(m_vHighBitVal);
		split.lowBitVal = VD_V2CB(m_vLowBitVal);
	}

public:
	void           MemberInit();
	int            IntToUnsigned(const VSConstBuffer<int> & val);
	unsigned       FindHighBitPos(unsigned value);
	void           SplitProcess();

	inline void    ValueCase(unsigned & lowBit, unsigned pos);

private:
	std::vector<int>             m_vValue;
	std::vector<unsigned>        m_vTranslate;
	std::vector<unsigned>        m_vHighBitVal;
	std::vector<unsigned>        m_vLowBitVal;

	VNVECTOR2UI                  bitlen;
};