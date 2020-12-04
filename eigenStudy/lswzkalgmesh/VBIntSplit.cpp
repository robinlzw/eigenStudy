#include "stdafx.h"
#include "VBIntSplit.h"

void VBIntSplit::MemberInit()
{
	m_vFront.clear();
	m_vSplit.clear();
}

void VBIntSplit::SplitProcess(const VSConstBuffer<int> & val)
{
	m_vFront.resize(val.len);
	m_vSplit.resize(val.len);

	for (unsigned i = 0; i < val.len; i++)
	{
		unsigned  value = (unsigned&)(val.pData[i]);

		m_vFront[i].x = (value & 0xFFFFF000) >> 12;
		m_vFront[i].y = value & 0x00000FFF;

		m_vSplit[i].x = (value & 0xFFF00000) >> 20;
		m_vSplit[i].y = (value & 0x000FFF00) >> 8;
		m_vSplit[i].z = value & 0x000000FF;
	}
}

//VD_EXPORT_SYSTEM_SIMPLE(VBIntSplit, VNALGMESH::VRIntSplit);