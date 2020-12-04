#include "stdafx.h"
#include "VBHyperbola.h"

void VBHyperbola::MemberInit()
{
	m_vHyperCoef.clear();
}

void VBHyperbola::CalCoef(const VSConstBuffer<VFVECTOR2>& vSet)
{
	m_vCoefMat.resize(vSet.len, 5);
	m_vRight.setZero(vSet.len);

	for (unsigned i = 0; i < vSet.len; i++)
	{
		m_vCoefMat.coeffRef(i, 0) = vSet.pData[i].x * vSet.pData[i].y;
		m_vCoefMat.coeffRef(i, 1) = vSet.pData[i].y * vSet.pData[i].y;
		m_vCoefMat.coeffRef(i, 2) = vSet.pData[i].x;
		m_vCoefMat.coeffRef(i, 3) = vSet.pData[i].y;
		m_vCoefMat.coeffRef(i, 4) = 1.f;
		m_vRight.coeffRef(i) = -vSet.pData[i].x * vSet.pData[i].x;
	}

	m_vSolve = m_vCoefMat.ldlt().solve(m_vRight);

/*	std::pair<float, unsigned>  temp(10000.f, 0);
	Eigen::EigenSolver<Eigen::MatrixXf>  ev(m_vCoefMat);
	Eigen::VectorXf  eigenValue = ev.eigenvalues();
	for (unsigned i = 0; i < eigenValue.size(); i++)
	{
		if (temp.first > fabs(eigenValue[i]))
		{
			temp.first = eigenValue[i];
			temp.second = i;
		}
	}
	m_vSolve = ev.eigenvectors().col(temp.second);    //该列特征向量对应六个系数
	*/
}
VD_EXPORT_SYSTEM_SIMPLE(VBHyperbola, VNALGMESH::VRHyperbola);