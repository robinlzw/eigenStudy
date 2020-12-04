#include"stdafx.h"

#include"VBMLSInterpCircle.h"

//float VBMLSInterpCircle::alpha = 0.005f;

void VBMLSInterpCircle::CalWeight(const VSConstBuffer<VFVECTOR2>& vSet, const float & xInterpV , float lineLen , float alpha )
{
	m_vDist.resize(vSet.len);
	m_vWeight.resize(vSet.len);

	float sum = 0.f;
	float L = lineLen / 2.f;
	for (unsigned i = 0; i < vSet.len; i++)
	{
        float fd = fabsf(vSet.pData[i].x - xInterpV) ;
        m_vDist[i] = ( fd <= L ) ? fd : lineLen - fd ;
  //      if (fabsf(vSet.pData[i].x - xInterpV) <= L)
		//{
		//	m_vDist[i] = fabsf(vSet.pData[i].x - xInterpV);
		//}
		//else
		//{
		//	m_vDist[i] = lineLen - fabsf(vSet.pData[i].x - xInterpV);
		//}
		
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

void VBMLSInterpCircle::EqSolve(const VSConstBuffer<VFVECTOR2>& vSet, const VSConstBuffer<float> & xInterp, float lineLen , float alpha  )
{
	m_vYInterpValue.resize(xInterp.len);
    float L = lineLen / 2.f;

	for (unsigned j = 0; j < xInterp.len; j++)
	{
		float mLeftCoef = 0.f, nLeftCoef = 0.f, cLeftCoef = 0.f;
		float dRightCoef = 0.f, eRightCoef = 0.f;
		float aCoef = 0.f, bCoef = 0.f;

        float xi = xInterp.pData[j] ;
		CalWeight(vSet, xi , lineLen , alpha );

		for (unsigned i = 0; i < vSet.len; i++)
		{
            float x = vSet.pData[i].x ;

            if( x - xi > L )
                x -= lineLen ;
            else if( x - xi < -L )
                x += lineLen ;

			mLeftCoef += x * x * m_vWeight[i];
			nLeftCoef += x * m_vWeight[i];
			cLeftCoef += m_vWeight[i];
			dRightCoef += x * vSet.pData[i].y * m_vWeight[i];
			eRightCoef += vSet.pData[i].y * m_vWeight[i];
		}

		bCoef = (dRightCoef * nLeftCoef - eRightCoef * mLeftCoef) /
			(nLeftCoef * nLeftCoef - mLeftCoef * cLeftCoef);
		aCoef = (dRightCoef - bCoef * nLeftCoef) / mLeftCoef;

		m_vYInterpValue[j] = aCoef * xi + bCoef;
	}
}
VD_EXPORT_SYSTEM_SIMPLE(VBMLSInterpCircle, VNALGMESH::VRMLSInterpCircle);