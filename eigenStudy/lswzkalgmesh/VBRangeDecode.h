#pragma once

#include "VBBitTree.h"

class VBRangeDecode
{
public:	
	VBRangeDecode(unsigned rMax, unsigned rMin) : Rmax(rMax), Rmin(rMin), L(0), R(rMax), codeCount(0), V(0) {}
	~VBRangeDecode() {}

	void Build(VSConstBuffer<unsigned> & dec, const VSConstBuffer<unsigned> & code, const unsigned & RangeSize, const unsigned & codeSize)
	{
		Decode(code, RangeSize, codeSize);

		dec = VD_V2CB(m_vDecode);
	}

private:	
	void              Decode(const VSConstBuffer<unsigned> & code, const unsigned RangeSize, const unsigned codeSize);

	inline void       DecodeProcess(const VSConstBuffer<unsigned> & code);
	unsigned          FindFreqID(unsigned estFreq);
	unsigned          EstimateFreq();
	void              RangeAdjust(const VSConstBuffer<unsigned> & code, unsigned decodeID);
	inline unsigned   InputCode(const VSConstBuffer<unsigned> & code);
	unsigned          FindHighBitPos(unsigned val);
	
private:
	std::vector<unsigned>    m_vDecode;

private:
	VBBitTree   btree;
	VSConstBuffer<unsigned>  range;
	unsigned Rmax;
	unsigned Rmin;
	unsigned L;
	unsigned R;
	unsigned V;
	unsigned codeCount;
};
