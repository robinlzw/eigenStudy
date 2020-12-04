#pragma once
#include<vmath/vfmesh.h>

#include <sys/alg/isysmesh.h>

//建立bittree，range-encode编码时用

class VBBitTree
{
public:
	VBBitTree() {}
	~VBBitTree() {}

	void Build( VSConstBuffer<unsigned> & feq, const unsigned & size)
	{
		MemberInit();
		Init(size);

		feq = VD_V2CB(m_vCumulativeFreq);
	}

public:	
	unsigned            GetCumulativeFrequency(unsigned idx);
	inline void         UpdateFreq(unsigned idx);
	unsigned            GetTotalFreq();

private:
	void                MemberInit();
	void                Init(unsigned size);
	inline unsigned     LowBitPos(unsigned val);

private:
	std::vector<unsigned>    m_vCumulativeFreq;
};