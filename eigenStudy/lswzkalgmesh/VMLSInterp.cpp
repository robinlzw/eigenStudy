#include"stdafx.h"

#include"VMLSInterp.h"

float VMLSInterp::alpha = 0.1f;

void VMLSInterp::CalWeight(const VSConstBuffer<VFVECTOR2>& vSet, const float & xInterpV)
{
	m_vDist.resize(vSet.len);
	m_vWeight.resize(vSet.len);

	float sum = 0.f;
	for (unsigned i = 0; i < vSet.len; i++)
	{		
		m_vDist[i] = fabsf(vSet.pData[i].x - xInterpV);
		sum += m_vDist[i];
	}

	float avgDis = sum / static_cast<float>(vSet.len);
	sum = 0.f;
	for (unsigned i = 0; i < vSet.len; i++)
	{
		m_vWeight[i] = expf(-(m_vDist[i] * m_vDist[i]) / (alpha * avgDis * avgDis));
		sum += m_vWeight[i];
	}

	//¹éÒ»»¯
	for (unsigned i = 0; i < vSet.len; i++)
	{
		m_vWeight[i] /= sum;
	}
}

void VMLSInterp::EqSolve(const VSConstBuffer<VFVECTOR2>& vSet, const VSConstBuffer<float> & xInterp)
{	
	m_vYInterpValue.resize(xInterp.len);

	for (unsigned j = 0; j < xInterp.len; j++)
	{
		float mLeftCoef = 0.f, nLeftCoef = 0.f, cLeftCoef = 0.f;
		float dRightCoef = 0.f, eRightCoef = 0.f;
		float aCoef = 0.f, bCoef = 0.f;
		CalWeight(vSet, xInterp.pData[j]);

		for (unsigned i = 0; i < vSet.len; i++)
		{
			mLeftCoef  += vSet.pData[i].x * vSet.pData[i].x * m_vWeight[i];
			nLeftCoef  += vSet.pData[i].x * m_vWeight[i];
			cLeftCoef  += m_vWeight[i];
			dRightCoef += vSet.pData[i].x * vSet.pData[i].y * m_vWeight[i];
			eRightCoef += vSet.pData[i].y * m_vWeight[i];
		}

		bCoef = (dRightCoef * nLeftCoef - eRightCoef * mLeftCoef) /
			    (nLeftCoef * nLeftCoef - mLeftCoef * cLeftCoef);
		aCoef = (dRightCoef - bCoef * nLeftCoef) / mLeftCoef;

		m_vYInterpValue[j] = aCoef * xInterp.pData[j] + bCoef;
	}	
}
VD_EXPORT_SYSTEM_SIMPLE(VMLSInterp, VNALGMESH::VRMLSInterp);