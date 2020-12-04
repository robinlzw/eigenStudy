#pragma once
#include<vmath/vfmesh.h>
#include "VBBitTree.h"

class VBRangeEncode
{
public:
	VBRangeEncode(unsigned rMAX, unsigned rMIN):Rmax(rMAX), Rmin(rMIN), R(rMAX), L(rMAX), d(0), n(0) {}
	~VBRangeEncode() {}

	void Build(VSConstBuffer<unsigned> & code, const VSConstBuffer<unsigned> & value, const unsigned & RangeSize)
	{
		RangeEncode(value, RangeSize);

		code = VD_V2CB(m_vCompress);
	}

private:
	void   RangeEncode(const VSConstBuffer<unsigned> & value, unsigned RangeSize);
	void   Encode(unsigned symbol);
	void   FinishEncode(unsigned d, unsigned n, unsigned L);

private:
	std::vector<unsigned>     m_vCompress;

	VBBitTree   btree;
	VSConstBuffer<unsigned>    Range;
	unsigned Rmax;
	unsigned Rmin;
	unsigned L;
	unsigned H;
	unsigned R;
	unsigned d;
	unsigned n;
};
