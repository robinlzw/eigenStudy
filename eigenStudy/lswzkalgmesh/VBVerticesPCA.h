#pragma once
#include<vmath/vfmesh.h>
#include <obj\ialgeq\ialgeq.h>

#ifdef _DEBUG
#pragma push_macro("new")
#undef new
#endif

#include <eigen/Eigen/Eigenvalues>

#ifdef _DEBUG
#pragma pop_macro("new") 
#endif

struct VSPCAUDVmat
{
	VSDnsMatrixF               U;
	VSDnsMatrixF               D;
	VSDnsMatrixF               V;
};

//three dimension points PCA
class VBVerticesPCA
{
public:
	VBVerticesPCA() {}
	~VBVerticesPCA() {}

	void Build(VSPCAUDVmat & pca, const VSConstBuffer<VFVECTOR3> & vert)
	{
		SVDProcess(vert);

		pca.D.row = m_vEigenVal.rows();
		pca.D.col = m_vEigenVal.cols();
		pca.D.pData = &m_vSingularVal.front();
		pca.U.row = 3;
		pca.U.col = 3;
		pca.U.pData = &m_vUMat.front();
		pca.V.row = vert.len;
		pca.V.col = vert.len;
		pca.V.pData = &m_vVMat.front();
	}

private:
	void CreateVertMat(const VSConstBuffer<VFVECTOR3> & vert);
	void SVDProcess(const VSConstBuffer<VFVECTOR3> & vert);

private:
	std::vector<float>                       m_vUMat;
	std::vector<float>                       m_vVMat;
	std::vector<float>                       m_vSingularVal;

	Eigen::MatrixXf                          m_vVertMat;
	Eigen::MatrixXf                          m_vEigenVal;
	//Eigen::MatrixXf                          m_vUEigenVector;
	//Eigen::MatrixXf                          m_vVEigenVector;
	//Eigen::MatrixXf                          m_vDValues;
};
