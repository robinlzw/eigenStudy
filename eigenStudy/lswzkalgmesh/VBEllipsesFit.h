#pragma once
#include <vmath/vfmesh.h>
#include <alg/ialgwzk.h>
#include "../lswzkalgeq/VBDnsMatrixInverse.h"
#include "../lswzkalgeq/VBMatrixEigen.h"

//Õ÷‘≤ƒ‚∫œ

//Direct Least Squares Fitting of Ellipses
class VBEllipsesFit
{
public:
	VBEllipsesFit() {}
	~VBEllipsesFit() {}

	void Build(VSConstBuffer<float> & coeff, const VSConstBuffer<VFVECTOR2> & fitPoint)
	{
		CoefficientFit(fitPoint);

		coeff = VD_V2CB(m_vCoefficient);
	}

private:
	void  CreateMatrix(const VSConstBuffer<VFVECTOR2> & fitPoint);
	void  CoefficientFit(const VSConstBuffer<VFVECTOR2> & fitPoint);

private:
	std::vector<float>    m_vCoefficient;

	std::vector<float>    m_vDesignMatrix;
	std::vector<float>    m_vScatterMat;
	std::vector<float>    m_vEigenMat;
	std::vector<float>    m_vConstraintMat;
};
