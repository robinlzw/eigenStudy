#pragma once
#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//浮点数量化

struct quantizationInfo
{
	VFVECTOR2           minmax;
	VSConstBuffer<int>  quanRes;
};

//(v - min) / (max - min) = x / quanlevel.
class VBFloatQuantization
{
public:
	VBFloatQuantization() {}
	~VBFloatQuantization() {}

	void Build(quantizationInfo & quan, const VSConstBuffer<float> & delta, const unsigned & quanLevel)
	{
		MemberInit();
		VFVECTOR2 val = Quantization(delta, quanLevel);

		quan.minmax  = val;
		quan.quanRes = VD_V2CB(m_vQuanzation);
	}

private:
	void        MemberInit();
	VFVECTOR2   Quantization(const VSConstBuffer<float> & delta, const unsigned & quanLevel);

private:
	std::vector<int>      m_vQuanzation;

	std::vector<float>    m_vTemp;
};