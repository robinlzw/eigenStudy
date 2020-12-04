#include "stdafx.h"
#include "VBPCACompressNED.h"

bool myComp(std::pair<float, unsigned> & a, std::pair<float, unsigned> & b)
{
	return a.first > b.first;
}

void VBPCACompressNED::MemberInit()
{
	m_vUMat.clear();
	m_vVMat.clear();
	m_vSingularVal.clear();
	m_vMeanValue.clear();
}

void VBPCACompressNED::DynamicCompress(const VSConstBuffer<VSConstBuffer<VFVECTOR3>>& framePos, float ratio)
{
	MeanVector(framePos);
	CreatFrameMat(framePos);
	SVDProcess();
	PCAProcess(ratio);
	MyRelease();
}

void VBPCACompressNED::MeanVector(const VSConstBuffer<VSConstBuffer<VFVECTOR3>>& framePos)
{
	m_vMeanValue.resize(framePos.len);
	float len = static_cast<float>(framePos.pData[0].len);     //every frame length is same

	for (unsigned i = 0; i < framePos.len; i++)
	{
		float sumX = 0.f, sumY = 0.f, sumZ = 0.f;
		for (unsigned j = 0; j < framePos.pData[i].len; j++)
		{
			sumX += framePos.pData[i].pData[j].x;
			sumY += framePos.pData[i].pData[j].y;
			sumZ += framePos.pData[i].pData[j].z;
		}

		m_vMeanValue[i].x = sumX / len;
		m_vMeanValue[i].y = sumY / len;
		m_vMeanValue[i].z = sumZ / len;
	}
}

void VBPCACompressNED::CreatFrameMat(const VSConstBuffer<VSConstBuffer<VFVECTOR3>>& framePos)
{
	m_vFrameMat.resize(framePos.len * 3, framePos.pData[0].len);

	for (unsigned i = 0; i < framePos.len; i++)
	{
		for (unsigned j = 0; j < framePos.pData[i].len; j++)
		{
			m_vFrameMat.coeffRef(3 * i + 0, j) = framePos.pData[i].pData[ j ].x - m_vMeanValue[i].x;
			m_vFrameMat.coeffRef(3 * i + 1, j) = framePos.pData[i].pData[ j ].y - m_vMeanValue[i].y;
			m_vFrameMat.coeffRef(3 * i + 2, j) = framePos.pData[i].pData[ j ].z - m_vMeanValue[i].z;
		}
	}
}

void VBPCACompressNED::SVDProcess()
{
	Eigen::JacobiSVD<Eigen::MatrixXf>  svd(m_vFrameMat, Eigen::ComputeThinU | Eigen::ComputeThinV);

	m_vUEigenVector = svd.matrixU();
	m_vVEigenVector = svd.matrixV();
	m_vDValues = svd.singularValues();

	/*std::ofstream fp("D:\\v.txt");
	fp << m_vVEigenVector;
	fp.close();*/
}

void VBPCACompressNED::PCAProcess(float ratio)
{
	unsigned numD = PersistDNum(ratio);

	m_vSingularVal.resize(numD);
	m_vUMat.resize(m_vUEigenVector.rows() * numD);
	m_vVMat.resize(m_vVEigenVector.rows() * numD);
	for (unsigned i = 0; i < numD; i++)
	{
		m_vSingularVal[i] = m_vDValues(i);
	}
	for (int i = 0; i < m_vUEigenVector.rows(); i++)
	{
		for (unsigned j = 0; j < numD; j++)
		{
			m_vUMat[i * numD + j] = m_vUEigenVector(i, j);
		}
	}
	/*for (int i = 0; i < m_vVEigenVector.rows(); i++)
	{
	for (unsigned j = 0; j < numD; j++)
	{
	m_vVMat[i * numD + j] = m_vVEigenVector(i, j);
	}
	}*/
	int vResistCol = m_vVEigenVector.rows();
	for (unsigned i = 0; i < numD; i++)
	{
		for (int j = 0; j < vResistCol; j++)
		{
			m_vVMat[i * vResistCol + j] = m_vVEigenVector(j, i);
		}
	}
}

unsigned VBPCACompressNED::PersistDNum(float ratio)
{
	float sum = 0.f, len = static_cast<float>(m_vDValues.size());

	unsigned count = 0;
	float sumD = m_vDValues.sum();
	for (int i = 0; i < m_vDValues.size(); i++)
	{
		sum += m_vDValues(i);
		if (sum / sumD > ratio)
		{
			count = i + 1;
			break;
		}
	}

	return count;
}

void VBPCACompressNED::MyRelease()
{
	m_vFrameMat.resize(0, 0);
	m_vUEigenVector.resize(0, 0);
	m_vVEigenVector.resize(0, 0);
	m_vDValues.resize(0, 0);
}
