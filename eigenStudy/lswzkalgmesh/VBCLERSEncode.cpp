#include "stdafx.h"
#include "VBCLERSEncode.h"

void VBCLERSEncode::MemberInit()
{
	buf = 0;
	value = 0;
	filled_Len = 0;
	residue_Len = 0;
	subres_Len = 0;
	count = 0;

	m_vCodeStream.clear();
	m_vCodeNum.clear();
}

void VBCLERSEncode::Init(const VSConstBuffer<char>& clers)
{
	//char C = 0, R = 2, L = 6, S = 14, E = 15;
	unsigned num = 0;
	m_vCodeNum.resize(clers.len);
	m_vCodeStream.reserve(clers.len);
	for (unsigned i = 0; i < clers.len; i++)
	{
		switch (clers.pData[num])
		{
		case 'C':
			m_vCodeNum[num] = 0;
			num++;
			break;
		case 'R':
			m_vCodeNum[num] = 2;
			num++;
			break;
		case 'L':
			m_vCodeNum[num] = 6;
			num++;
			break;
		case 'S':
			m_vCodeNum[num] = 14;
			num++;
			break;
		case 'E':
			m_vCodeNum[num] = 15;
			num++;
			break;
		default:
			break;
		}
	}
}

void VBCLERSEncode::Encode(const VSConstBuffer<char> & clers)
{
	unsigned len = clers.len;
	Init(clers);
	value = m_vCodeNum[count];

	while (count < clers.len)
	{
		switch (clers.pData[count])
		{
		case 'C':         
			CharProcess(1, len);     //0
			break;
		case 'R':      
			CharProcess(2, len);    //10
			break;
		case 'L':        
			CharProcess(3, len);    //110
			break;
		case 'S':         
			CharProcess(4, len);    //1110
			break;
		case 'E':         
			CharProcess(4, len);    //1111
			break;

		default:
			break;
		}
	}
	m_vCodeStream.push_back(buf);
}

inline void VBCLERSEncode::CharProcess(char bitLen, const unsigned & len)
{
	residue_Len = 8 - filled_Len;
	if (residue_Len >= bitLen)
	{
		char temp = value << (residue_Len - bitLen);
		buf |= value << (residue_Len - bitLen);
		filled_Len += bitLen;
	}
	else
	{
		subres_Len = bitLen - residue_Len;
		buf |= value >> (subres_Len);
		OutputChar(buf, m_vCodeStream);
		buf = 0;
		filled_Len = 0;

		residue_Len = 8 - filled_Len;
		buf |= value << (residue_Len - subres_Len);
		filled_Len += subres_Len;
	}

	count++;
	if (count < len)
		value = NextChar(count);
}

inline void VBCLERSEncode::OutputChar(char c, std::vector<char>& vec)
{
	vec.push_back(c);
}

inline char VBCLERSEncode::NextChar(const unsigned & counter)
{
	return m_vCodeNum[counter];
}

//VD_EXPORT_SYSTEM_SIMPLE(VBCLERSEncode, VNALGMESH::VRCLERSEncode);