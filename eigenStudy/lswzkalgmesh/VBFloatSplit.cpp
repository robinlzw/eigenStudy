#include "stdafx.h"
#include "VBFloatSplit.h"

void VBFloatSplit::SplitProcess(const VSConstBuffer<VFVECTOR3>& vf)
{	
	unsigned num = vf.len * 3;
	m_vSignExponent.resize(num);
	m_vMiddleMantissa.resize(num);
	m_vEndMantissa.resize(num);
	//m_vFloatSplit.resize(vf.len * 9);

	for (unsigned i = 0; i < vf.len; i++)
	{
		int temp = (int&)vf.pData[i].x;
		m_vSignExponent[i]   = temp & 0xFF800000 >> 23;
		m_vMiddleMantissa[i] = temp & 0x7FF800 >> 11;
		m_vEndMantissa[i]    = temp & 0x7FF;

		temp = (int&)vf.pData[i].y;
		m_vSignExponent[i + vf.len]   = temp & 0xFF800000 >> 23;
		m_vMiddleMantissa[i + vf.len] = temp & 0x7FF800 >> 11;
		m_vEndMantissa[i + vf.len]    = temp & 0x7FF;

		temp = (int&)vf.pData[i].z;
		m_vSignExponent[i + vf.len * 2]   = temp & 0xFF800000 >> 23;
		m_vMiddleMantissa[i + vf.len * 2] = temp & 0x7FF800 >> 11;
		m_vEndMantissa[i + vf.len * 2]    = temp & 0x7FF;

		////x
		//int temp = (int&)vf.pData[i].x;
		//m_vFloatSplit[i] = temp & 0xFF800000 >> 23;             //9
		//m_vFloatSplit[i + vf.len] = temp & 0x7FF800 >> 11;		//12
		//m_vFloatSplit[i + vf.len * 2] = temp & 0x7FF;			//11
		//temp = (int&)vf.pData[i].y;
		//m_vFloatSplit[i + 3 * vf.len] = temp & 0xFF800000 >> 23;
		//m_vFloatSplit[i + 4 * vf.len] = temp & 0x7FF800 >> 11;
		//m_vFloatSplit[i + 5 * vf.len] = temp & 0x7FF;
		//temp = (int&)vf.pData[i].z;
		//m_vFloatSplit[i + 6 * vf.len] = temp & 0xFF800000 >> 23;
		//m_vFloatSplit[i + 7 * vf.len] = temp & 0x7FF800 >> 11;
		//m_vFloatSplit[i + 8 * vf.len] = temp & 0x7FF;
	}
}

void VBFloatSplit::SplitProcess1(const VSConstBuffer<VFVECTOR3>& vPred, const VSConstBuffer<VFVECTOR3>& vReal)
{
	unsigned num = vReal.len * 3;
	m_vSignExponent.resize(num);
	m_vMiddleMantissa.resize(num);
	m_vEndMantissa.resize(num);
	m_vVertFlag.resize(num);
	
	for (unsigned i = 0; i < vReal.len; i++)
	{
		//x
		Process(vPred.pData[i].x, vReal.pData[i].x, i);

		//y
		Process(vPred.pData[i].y, vReal.pData[i].y, i + vReal.len);		

		//z
		Process(vPred.pData[i].z, vReal.pData[i].z, i + vReal.len * 2);		
	}
}

void VBFloatSplit::Process(float pred, float real, unsigned pos)
{
	unsigned mantissaReal = (((unsigned&)real) & 0x007FFFFF);
	unsigned mantissaPred = (((unsigned&)pred) & 0x007FFFFF);
	unsigned delta = mantissaReal ^ mantissaPred;

	unsigned tempR = (unsigned&)real;
	unsigned tempP = (unsigned&)pred;
	unsigned signExponentReal = (tempR & 0xFF800000) >> 23;
	unsigned signExponentPred = (tempP & 0xFF800000) >> 23;

	if (signExponentPred == signExponentReal)
	{
		m_vSignExponent[pos] = signExponentPred;
		m_vMiddleMantissa[pos] = (delta & 0x7FF000) >> 12;
		m_vEndMantissa[pos] = delta & 0xFFF;
		/*m_vMiddleMantissa[pos] = ( delta & 0x7FF800 ) >> 11;
		m_vEndMantissa[pos] = delta & 0x7FF;*/
		m_vVertFlag[pos] = 0;    //false
		dCount++;
	}
	else
	{
		m_vSignExponent[pos] = signExponentReal;
		m_vMiddleMantissa[pos] = (tempR & 0x7FF000) >> 12;
		m_vEndMantissa[pos] = tempR & 0xFFF;
		/*m_vMiddleMantissa[pos] = ( tempR & 0x7FF800 ) >> 11;
		m_vEndMantissa[pos] = tempR & 0x7FF;*/
		m_vVertFlag[pos] = 1;    //true
		rCount++;
	}
}

//VD_EXPORT_SYSTEM_SIMPLE(VBFloatSplit, VNALGMESH::VRFloatSplit);