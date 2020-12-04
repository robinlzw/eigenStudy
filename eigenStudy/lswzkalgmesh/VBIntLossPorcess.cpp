#include "stdafx.h"
#include "VBIntLossPorcess.h"

void VBIntLossPorcess::MemberInit()
{
	m_vValue.clear();
	m_vTranslate.clear();
	m_vHighBitVal.clear();
	m_vLowBitVal.clear();
}

int VBIntLossPorcess::IntToUnsigned(const VSConstBuffer<int>& val)
{
	m_vValue.resize(val.len);
	memcpy(&m_vValue.front(), val.pData, sizeof(int) * val.len);

	std::vector<int>::iterator min = std::min_element(m_vValue.begin(), m_vValue.end());
	//int temp = abs(*min);

	m_vTranslate.resize(val.len);
	for (unsigned i = 0; i < val.len; i++)
	{
		//m_vTranslate[i] = static_cast<unsigned>(m_vValue[i] + temp);
		m_vTranslate[i] = static_cast<unsigned>(m_vValue[i] - *min);
	}

	int temp = *min;
	SplitProcess();
	m_vValue.clear();
	m_vTranslate.clear();
	
	//return -temp;	
	return temp;
}

unsigned VBIntLossPorcess::FindHighBitPos(unsigned value)
{
	if (!value)
		return 0;

	unsigned bit = 0;
	while (value)
	{
		value = value >> 1;
		bit++;
	}
	return bit;
}

void VBIntLossPorcess::SplitProcess()
{
	m_vHighBitVal.resize(m_vTranslate.size());
	m_vLowBitVal.resize(m_vTranslate.size());

	unsigned highBitPos = 0;
	for (unsigned i = 0; i < m_vTranslate.size(); i++)
	{
		unsigned temp = FindHighBitPos(m_vTranslate[i]);

		if (temp > highBitPos)
			highBitPos = temp;
	}

	unsigned lowBit = 0;
	unsigned pos = highBitPos / 2;
	ValueCase(lowBit, pos);

	for (unsigned i = 0; i < m_vTranslate.size(); i++)
	{
		m_vHighBitVal[i] = m_vTranslate[i] >> pos;
		m_vLowBitVal[i] = m_vTranslate[i] & lowBit;
	}

	bitlen.x = highBitPos - pos;
	bitlen.y = pos;
}

inline void VBIntLossPorcess::ValueCase(unsigned & lowBit, unsigned pos)
{
	switch (pos)
	{
	case 1:
		lowBit = 0x00000001;
		break;
	case 2:
		lowBit = 0x00000003;
		break;
	case 3:
		lowBit = 0x00000007;
		break;
	case 4:
		lowBit = 0x0000000F;
		break;
	case 5:
		lowBit = 0x0000001F;
		break;
	case 6:
		lowBit = 0x0000003F;
		break;
	case 7:
		lowBit = 0x0000007F;
		break;
	case 8:
		lowBit = 0x000000FF;
		break;
	case 9:
		lowBit = 0x000001FF;
		break;
	case 10:
		lowBit = 0X000003FF;
		break;
	case 11:
		lowBit = 0x000007FF;
		break;
	case 12:
		lowBit = 0x00000FFF;
		break;
	case 13:
		lowBit = 0x00001FFF;
		break;
	case 14:
		lowBit = 0x00003FFF;
		break;
	case 15:
		lowBit = 0x00007FFF;
		break;
	case 16:
		lowBit = 0x0000FFFF;
		break;
	default:
		VASSERT(lowBit != 0);
		break;
	}
}

//VD_EXPORT_SYSTEM_SIMPLE(VBIntLossPorcess, VNALGMESH::VRIntLossPorcess);