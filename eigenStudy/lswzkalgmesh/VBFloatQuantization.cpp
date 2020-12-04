#include "stdafx.h"
#include "VBFloatQuantization.h"

void VBFloatQuantization::MemberInit()
{
	m_vQuanzation.clear();
	m_vTemp.clear();
}

VFVECTOR2 VBFloatQuantization::Quantization(const VSConstBuffer<float>& delta, const unsigned & quanLevel)
{
	m_vQuanzation.resize(delta.len);
	m_vTemp.resize(delta.len);
	memcpy(&m_vTemp.front(), delta.pData, sizeof(float) * delta.len);
	auto  minmax = std::minmax_element(m_vTemp.begin(), m_vTemp.end());
	float dist = (*minmax.second - *minmax.first) / static_cast<float>(quanLevel);
	assert(dist != 0);

	for (unsigned i = 0; i < delta.len; i++)
	{
		m_vQuanzation[i] = static_cast<int>((delta.pData[i] - *minmax.first) / dist);
	}

	VFVECTOR2 temp;
	temp.x = *minmax.first;
	temp.y = *minmax.second;
	return temp;
}

//VD_EXPORT_SYSTEM_SIMPLE(VBFloatQuantization, VNALGMESH::VRFloatQuantization);