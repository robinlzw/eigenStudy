#pragma once
#include <obj/ialgeq/ialgeq.h>
#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif

#include <eigen/Eigen/Eigenvalues>

#ifdef _DEBUG
#pragma pop_macro("new") 
#endif

struct UDVmat
{
	VSDnsMatrixF               U;
	VSDnsMatrixF               D;
	VSDnsMatrixF               V;
	VSConstBuffer<VFVECTOR3>   mean;
};

//PCAÑ¹Ëõ£¬²»°üº¬Íø¸ñÑ¹ËõËã·¨

class VBPCACompressNED
{
public:
	VBPCACompressNED() {}
	~VBPCACompressNED() {}

	void Build(UDVmat & pca, const VSConstBuffer<VSConstBuffer<VFVECTOR3>> & framePos, const float & ratio)
	{
		MemberInit();
		DynamicCompress(framePos, ratio);

		pca.D.row = m_vSingularVal.size();
		pca.D.col = 1;
		pca.D.pData = &m_vSingularVal.front();
		pca.U.row = m_vUMat.size() / m_vSingularVal.size();
		pca.U.col = m_vSingularVal.size();
		pca.U.pData = &m_vUMat.front();
		pca.V.row = m_vSingularVal.size();
		pca.V.col = m_vVMat.size() / m_vSingularVal.size();
		pca.V.pData = &m_vVMat.front();
		pca.mean = VD_V2CB(m_vMeanValue);
	}

private:
	void      MemberInit();
	void      DynamicCompress(const VSConstBuffer<VSConstBuffer<VFVECTOR3>> & framePos, float ratio);
	void      MeanVector(const VSConstBuffer<VSConstBuffer<VFVECTOR3>> & framePos);
	void      CreatFrameMat(const VSConstBuffer<VSConstBuffer<VFVECTOR3>> & framePos);
	void      SVDProcess();
	void      PCAProcess(float ratio);
	unsigned  PersistDNum(float ratio);
	void      MyRelease();

private:
	std::vector<float>                       m_vUMat;
	std::vector<float>                       m_vVMat;
	std::vector<float>                       m_vSingularVal;

	std::vector<VFVECTOR3>                   m_vMeanValue;
	Eigen::MatrixXf                          m_vFrameMat;
	Eigen::MatrixXf                          m_vUEigenVector;
	Eigen::MatrixXf                          m_vVEigenVector;
	Eigen::MatrixXf                          m_vDValues;
};
