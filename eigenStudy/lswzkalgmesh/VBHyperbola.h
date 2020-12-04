#pragma once
#include <vstd/mem.h>
#include <vmath/vfvector2.h>
#include <sys/alg/isysmesh.h>

#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif

#include <eigen/Eigen/Sparse>
#include <eigen/Eigen/Dense> 

#ifdef _DEBUG
#pragma pop_macro("new") 
#endif

//曲线方程为Ax^2+Bxy+Cy^2+Dx+Ey+F=0，输出为方程的系数，其中A=1已知

class VBHyperbola
{
public:
	VBHyperbola() {}
	~VBHyperbola() {}

	void Build(VSConstBuffer<float> & hyperCoef, const VSConstBuffer<VFVECTOR2> & vSet)
	{
		MemberInit();
		CalCoef(vSet);

		m_vHyperCoef.resize(m_vSolve.size() + 1);
		m_vHyperCoef[0] = 1.f;
		for (unsigned i = 1; i < m_vHyperCoef.size(); i++)
		{
			m_vHyperCoef[i] = m_vSolve[i - 1];
		}
		hyperCoef.len = m_vHyperCoef.size();
		hyperCoef.pData = &m_vHyperCoef[0];
	}

private:
	void MemberInit();
	void CalCoef(const VSConstBuffer<VFVECTOR2> & vSet);

private:
	std::vector<float>    m_vHyperCoef;
	Eigen::VectorXf       m_vSolve;
	Eigen::MatrixXf       m_vCoefMat;
	Eigen::VectorXf       m_vRight;
};