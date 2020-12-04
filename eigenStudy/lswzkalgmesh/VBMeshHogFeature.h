#pragma once
#include "VBMeshCurvature.h"

//���������hog����(��һ��)����������С����ƽ���͵Ŀ�����ΪȨ��ֵ
class VBMeshHogFeature
{
public:
	VBMeshHogFeature() {}
	~VBMeshHogFeature() {}

	void Build(VSConstBuffer<float> & hog, const VSConstBuffer< VNWZKALG::VSVertEigenInfo > & cv, const unsigned & blockNum)
	{
		HogFeature(cv, blockNum);

		hog = VD_V2CB(m_vHogFeature);
	}

private:
	void  HogFeature(const VSConstBuffer< VNWZKALG::VSVertEigenInfo > & cv, const unsigned & blockNum);

	VBMeshCurvature       m_oMC;

private:
	std::vector<float>       m_vHogFeature;

	std::vector<double>      m_vGradientLength;    //x: gradient length, y: angle
	std::vector<double>      m_vGradientAngle;
};
