#pragma once
#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>

//量化值转为浮点数

class VBQuantizationToFloat
{
public:
	VBQuantizationToFloat();
	~VBQuantizationToFloat();

	void Build(VSConstBuffer<float> & fval, const VSConstBuffer<int> & quan, const VFVECTOR2 & minmax, const unsigned & quanLevel)
	{
		Process(quan, minmax, quanLevel);

		fval = VD_V2CB(m_vValue);
	}

private:
	void  Process(const VSConstBuffer<int> & quan, const VFVECTOR2 & minmax, const unsigned quanLevel)
	{
		float dist = minmax.y - minmax.x;

		m_vValue.resize(quan.len);
		for (unsigned i = 0; i < quan.len; i++)
		{
			m_vValue[i] = static_cast<float>(quan.pData[i]) * dist / static_cast<float>(quanLevel) + minmax.x;
		}
	}

private:
	std::vector<float>      m_vValue;
};
