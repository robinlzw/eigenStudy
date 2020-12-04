#include "stdafx.h"
#include "VBRangeDecode.h"

void VBRangeDecode::Decode(const VSConstBuffer<unsigned> & code, const unsigned RangeSize, const unsigned codeSize)
{
	btree.Build(range, RangeSize);
	
	unsigned temp = 32;
	while (temp > 0)
	{
		temp -= 8;
		V = V | (InputCode(code) << temp);
	}
	
	while (m_vDecode.size() < codeSize)
	{
		DecodeProcess(code);		
	}
}

inline void VBRangeDecode::DecodeProcess(const VSConstBuffer<unsigned>& code)
{
	unsigned estFreq = EstimateFreq();
	unsigned ID = FindFreqID(estFreq);
	m_vDecode.push_back(ID);
	RangeAdjust(code, ID);
	btree.UpdateFreq(ID);
}

unsigned VBRangeDecode::FindFreqID(unsigned estFreq)
{
	unsigned idx = 0;
	unsigned maxval = range.len - 1;
	unsigned bitMask = FindHighBitPos(maxval);
	bitMask = 1 << (bitMask - 1);

	while (bitMask != 0 && (idx < maxval))
	{
		unsigned tempIdx = idx + bitMask;     //middle of interval
		unsigned currCumFreq = btree.GetCumulativeFrequency(tempIdx);
		unsigned nextCumFreq = btree.GetCumulativeFrequency(tempIdx + 1);
		if ( (estFreq >= currCumFreq && estFreq < nextCumFreq) )
		{
			return tempIdx;
		}
		else if (estFreq > currCumFreq)
		{
			idx = tempIdx;
			//estFreq -= cumulativeFreq;
		}
		bitMask >>= 1;
	}

	return idx;
}

unsigned VBRangeDecode::EstimateFreq()
{
	unsigned code = V >> 1;
	
	//estimate frequency
	R /= btree.GetTotalFreq();      //此时R不是区间大小，为临时值，调整区间时使用

	if (code < L)
		return (code + Rmax - L) / R;
	else
		return (code - L) / R;
}

void VBRangeDecode::RangeAdjust(const VSConstBuffer<unsigned> & code, unsigned decodeID)
{	
	unsigned Fs = btree.GetCumulativeFrequency(decodeID);
	unsigned fs = btree.GetCumulativeFrequency(decodeID + 1) - Fs;

	//此时的R为EstimateFreq()计算出的临时值
	L = L + Fs * R;
	R = R * fs;

	//调整区间
	while (R <= Rmin)
	{
		L = (L << 8) & (Rmax - 1);
		R = R << 8;
		V = (V << 8) | (InputCode(code));
	}
}

inline unsigned VBRangeDecode::InputCode(const VSConstBuffer<unsigned> & code)
{
	codeCount ++;
	VASSERT(codeCount - 1 < code.len);
	return code.pData[codeCount - 1];
}

unsigned VBRangeDecode::FindHighBitPos(unsigned val)
{
	if (!val)
		return 0;
	
	unsigned bit = 0;
	while (val)
	{
		val = val >> 1;
		bit ++;
	}
	return (bit - 1);
}
