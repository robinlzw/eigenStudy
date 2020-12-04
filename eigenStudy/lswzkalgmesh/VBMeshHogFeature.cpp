#include "stdafx.h"
#include "VBMeshHogFeature.h"

void VBMeshHogFeature::HogFeature(const VSConstBuffer< VNWZKALG::VSVertEigenInfo > & cv, const unsigned & blockNum)
{
	m_vGradientLength.resize(cv.len);
	m_vGradientAngle.resize(cv.len);
	m_vHogFeature.resize(blockNum);

	//VSConstBuffer< VNWZKALG::VSVertEigenInfo >   cv;
	//m_oMC.Build(cv, gm);

	double sum = 0.;
	double bi = VD_PI / static_cast<double>(blockNum), rot = VD_PI / 4.;
	for (unsigned i = 0; i < cv.len; i++)
	{
		m_vGradientLength[i] = sqrt(cv.pData[i].eigValue[1] * cv.pData[i].eigValue[1] +
			                        cv.pData[i].eigValue[2] * cv.pData[i].eigValue[2]);
		m_vGradientAngle[i] = atan2(cv.pData[i].eigValue[2], cv.pData[i].eigValue[1]);

		if (m_vGradientAngle[i] < 0)
			m_vGradientAngle[i] += 2. * VD_PI;

		unsigned pos = static_cast<unsigned>(floor((m_vGradientAngle[i] - rot) / bi));    //减去rot，保证区间在[0, pi]
		VASSERT(pos <= blockNum);
		m_vHogFeature[pos] += (float)m_vGradientLength[i];
	}

	for (unsigned i = 0; i < m_vHogFeature.size(); i++)
	{
		sum += (m_vHogFeature[i] * m_vHogFeature[i]);
	}
	sum = sqrt(sum);
	for (unsigned i = 0; i < m_vHogFeature.size(); i++)
	{
		m_vHogFeature[i] = (float)(m_vHogFeature[i] / sum);
	}
}
