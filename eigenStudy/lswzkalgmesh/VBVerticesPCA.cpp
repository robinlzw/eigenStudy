#include "stdafx.h"
#include "VBVerticesPCA.h"

void VBVerticesPCA::CreateVertMat(const VSConstBuffer<VFVECTOR3> & vert)
{
	m_vVertMat.resize(3, vert.len);

	for (unsigned i = 0; i < vert.len; i++)
	{
		m_vVertMat.coeffRef(0, i) = vert.pData[i].x;
		m_vVertMat.coeffRef(1, i) = vert.pData[i].y;
		m_vVertMat.coeffRef(2, i) = vert.pData[i].z;
	}
}

void VBVerticesPCA::SVDProcess(const VSConstBuffer<VFVECTOR3> & vert)
{
	CreateVertMat(vert);
	Eigen::JacobiSVD<Eigen::MatrixXf>  svd(m_vVertMat, Eigen::ComputeThinU | Eigen::ComputeThinV);

	m_vEigenVal = svd.singularValues();
	m_vSingularVal.resize(svd.singularValues().rows());
	for (unsigned i = 0; i < m_vSingularVal.size(); i++)
		m_vSingularVal[i] = m_vEigenVal(i);

	m_vUMat.resize(svd.matrixU().rows() * svd.matrixU().cols());
	for (int i = 0; i < svd.matrixU().rows(); i++)
	{
		for (int j = 0; j < svd.matrixU().cols(); j++)
		{
			m_vUMat[i * svd.matrixU().cols() + j] = svd.matrixU()(i, j);
		}
	}

	m_vVMat.resize(svd.matrixV().rows() * svd.matrixV().cols());
	for (int i = 0; i < svd.matrixV().rows(); i++)
	{
		for (int j = 0; j < svd.matrixV().cols(); j++)
		{
			m_vVMat[i * svd.matrixV().cols() + j] = svd.matrixV()(i, j);
		}
	}
	//m_vUEigenVector = svd.matrixU();
	//m_vVEigenVector = svd.matrixV();
	//m_vDValues = svd.singularValues();
}
