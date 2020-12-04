#include "stdafx.h"
#include "VBEllipsesFit.h"

void VBEllipsesFit::CreateMatrix(const VSConstBuffer<VFVECTOR2>& fitPoint)
{
	m_vDesignMatrix.resize(fitPoint.len * 6);
	for (unsigned i = 0; i < fitPoint.len; i++)
	{
		m_vDesignMatrix[i * 6 + 0] = fitPoint.pData[i].x * fitPoint.pData[i].x;
		m_vDesignMatrix[i * 6 + 1] = fitPoint.pData[i].x * fitPoint.pData[i].y;
		m_vDesignMatrix[i * 6 + 2] = fitPoint.pData[i].y * fitPoint.pData[i].y;
		m_vDesignMatrix[i * 6 + 3] = fitPoint.pData[i].x;
		m_vDesignMatrix[i * 6 + 4] = fitPoint.pData[i].y;
		m_vDesignMatrix[i * 6 + 5] = 1.f;
	}

	m_vScatterMat.resize(36);
	for (unsigned i = 0; i < 6; i++)
	{
		for (unsigned j = 0; j < 6; j++)
		{
			float sum = 0.f;
			for (unsigned k = 0; k < fitPoint.len; k++)
			{
				sum += m_vDesignMatrix[k * 6 + i] * m_vDesignMatrix[k * 6 + j];
			}

			m_vScatterMat[i * 6 + j] = sum;
		}
	}

	VSDnsMatrixF   scaterMat, scaterMatInv;
	scaterMat.row = 6;
	scaterMat.col = 6;
	scaterMat.pData = &m_vScatterMat.front();

	VBDnsMatrixInverseF   m_objInv;
	m_objInv.Build(scaterMatInv, scaterMat);

	m_vConstraintMat.resize(36, 0.f);
	m_vConstraintMat[2] = 2.f;
	m_vConstraintMat[7] = -1.f;
	m_vConstraintMat[12] = 2.f;

	m_vEigenMat.resize(36, 0.f);
	for (unsigned i = 0; i < 6; i++)
	{
		m_vEigenMat[i * 6 + 0] = scaterMatInv.pData[i * 6 + 2] * 2.f;
		m_vEigenMat[i * 6 + 1] = -scaterMatInv.pData[i * 6 + 1];
		m_vEigenMat[i * 6 + 2] = scaterMatInv.pData[i + 0] * 2.f;
	}
}

void VBEllipsesFit::CoefficientFit(const VSConstBuffer<VFVECTOR2>& fitPoint)
{
	CreateMatrix(fitPoint);

	VNWZKALG::VSMatrixEigen<float>  eig;
	VSDnsMatrixF    eigMat;
	eigMat.row = eigMat.col = 6;
	eigMat.pData = &m_vEigenMat.front();
	VBMatrixEigenF  m_objEig;
	m_objEig.Build(eig, eigMat);

	int pos = -1;
	for (unsigned i = 0; i < eig.eigenValues.len; i++)
	{
		if (eig.eigenValues.pData[i] > 0.f)
		{
			pos = i;
			break;
		}
	}

	m_vCoefficient.resize(6);
	for (unsigned i = 0; i < 6; i++)
	{
		if (eig.eigenVectors.pData[pos].pData[0] < 0.f)
			m_vCoefficient[i] = - eig.eigenVectors.pData[pos].pData[i];
		else
			m_vCoefficient[i] = eig.eigenVectors.pData[pos].pData[i];
	}
}
