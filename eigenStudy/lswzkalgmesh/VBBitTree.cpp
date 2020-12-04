#include "stdafx.h"
#include "VBBitTree.h"

void VBBitTree::MemberInit()
{
	m_vCumulativeFreq.clear();
}

void VBBitTree::Init(unsigned size)
{
	m_vCumulativeFreq.resize(size + 1);
	//m_vCumulativeFreq[size] = size;

	for (unsigned i = 0; i < size; i++)
	{
		UpdateFreq(i);
	}
	//m_vCumulativeFreq[size] = GetCumulativeFrequency(size - 1) + 1;
}

unsigned VBBitTree::GetCumulativeFrequency(unsigned idx)
{
	unsigned sum = 0;

	while (idx > 0)
	{
		sum += m_vCumulativeFreq[idx];

		idx -= LowBitPos(idx);        //µÍÎ»1¼õ1
	}
	return sum;
}

inline void VBBitTree::UpdateFreq(unsigned idx)
{
	idx += 1;
	while (idx <= m_vCumulativeFreq.size() && idx > 0)
	{
		m_vCumulativeFreq[idx] += 1;

		idx += LowBitPos(idx);         //µÍÎ»1¼Ó1
	}
}

unsigned VBBitTree::GetTotalFreq()
{
	return m_vCumulativeFreq[m_vCumulativeFreq.size() - 1];
}

inline unsigned VBBitTree::LowBitPos(unsigned val)
{
	return (val & (~val + 1));
}

//VD_EXPORT_SYSTEM_SIMPLE(VBBitTree, VNALGMESH::VRBitTree);