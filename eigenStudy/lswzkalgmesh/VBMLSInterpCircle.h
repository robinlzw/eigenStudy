#pragma once

#include <vstd/mem.h>
#include <vmath/vfvector2.h>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//最小二乘插值拟合圆

//struct Coeff
//{
//	float len;
//	float alpha;
//};

class VBMLSInterpCircle
{
public:
	VBMLSInterpCircle() {}
	~VBMLSInterpCircle() {}

	void Build(VSConstBuffer<float> & yInterpValue , const VSConstBuffer<VFVECTOR2> & vSet
                                                   , const VSConstBuffer<float> & xInterp
                                                   , const VSMLSInterpCircleCoeff & coeff )
	{
		EqSolve(vSet, xInterp, coeff.len , coeff.alpha );

		yInterpValue.len = m_vYInterpValue.size();
		yInterpValue.pData = &m_vYInterpValue[0];
	}

private:
	void CalWeight(const VSConstBuffer<VFVECTOR2> & vSet, const float & xInterpV, float lineLen , float alpha );
	void EqSolve(const VSConstBuffer<VFVECTOR2> & vSet, const VSConstBuffer<float> & xInterp, float lineLen , float alpha );

private:
	float static            alpha;
	std::vector<float>      m_vYInterpValue;

	std::vector<float>      m_vDist;
	std::vector<float>      m_vWeight;
};

