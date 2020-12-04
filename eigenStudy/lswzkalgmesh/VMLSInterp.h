#pragma once

#include <vstd/mem.h>
#include <vmath/vfvector2.h>
#include <sys/alg/isysmesh.h>

class VMLSInterp
{
public:
	VMLSInterp() {}
	~VMLSInterp() {}

	void Build(VSConstBuffer<float> & yInterpValue, const VSConstBuffer<VFVECTOR2> & vSet, const VSConstBuffer<float> & xInterp)
	{
		EqSolve(vSet, xInterp);

		yInterpValue.len = m_vYInterpValue.size();
		yInterpValue.pData = &m_vYInterpValue[0];
	}

private:
	void CalWeight(const VSConstBuffer<VFVECTOR2> & vSet, const float & xInterpV);
	void EqSolve(const VSConstBuffer<VFVECTOR2> & vSet, const VSConstBuffer<float> & xInterp);
	
private:
	float static            alpha;
	std::vector<float>      m_vYInterpValue;
	
	std::vector<float>      m_vDist;
	std::vector<float>      m_vWeight;
};

 