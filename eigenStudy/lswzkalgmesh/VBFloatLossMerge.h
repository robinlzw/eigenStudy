#pragma once

#include "VBIntLossPorcess.h"
#include <sys/alg/isysmesh.h>

//浮点数有损合并

class VBFloatLossMerge
{
public:
	VBFloatLossMerge();
	~VBFloatLossMerge();

	void Build(VSConstBuffer<float> & merg, const VSConstBuffer<unsigned> & highBit, const VSConstBuffer<unsigned> & lowBit, const float & delta, const unsigned & lowBitLen, const int & min)
	{
		MemberInit();
		Merge(highBit, lowBit, delta, lowBitLen, min);

		merg = VD_V2CB(m_vMerge);
	}

private:
	void  MemberInit()
	{
		m_vMerge.clear();
	}

	void  Merge(const VSConstBuffer<unsigned> & highBit, const VSConstBuffer<unsigned> & lowBit, const float & delta, const unsigned & lowBitLen, const int & min)
	{
		m_vMerge.resize(highBit.len);

		for (unsigned i = 0; i < m_vMerge.size(); i++)
		{
			m_vMerge[i] = (static_cast<int>((highBit.pData[i] << lowBitLen) | lowBit.pData[i]) + min) * delta;
		}
	}

private:
	std::vector<float>      m_vMerge;
};