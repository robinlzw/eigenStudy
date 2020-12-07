#include "stdafx.h"
#include <obj/lsalgtools/VDDenseMatrix.h>

#if 0
// 将本类矩阵存储类型转化为第三方库EIGEN中的矩阵存储类型
static void TransEigenMatFromUser(VSSprsMatThird & sprsMat, const VSSprsMatrixD & userSprsMat)
{
	sprsMat.sprsMat.resize(userSprsMat.row, userSprsMat.col);
	sprsMat.tripList.reserve(userSprsMat.val.len);
	for (size_t i = 0; i < userSprsMat.val.len; i++)
	{
		sprsMat.tripList.push_back(Eigen::Triplet<double>(userSprsMat.val.pData[i].row,
			userSprsMat.val.pData[i].col, userSprsMat.val.pData[i].val));
	}
	sprsMat.sprsMat.setFromTriplets(sprsMat.tripList.begin(), sprsMat.tripList.end());

}
#endif
namespace NMALG_MATRIX
{
	VDDenseMatrix::VDDenseMatrix(const VSDnsMatData<double, true>& dnsMat)
	{
		m_mat.resize(dnsMat.row, dnsMat.col);
		for (size_t i = 0; i < dnsMat.row; i++)
		{
			for (size_t j = 0; j < dnsMat.col; j++)
				m_mat(i, j) = dnsMat.pData[i + j * dnsMat.row];
		}
	}

	VDDenseMatrix::~VDDenseMatrix()
	{

	}

	inline VDDenseMatrix& VDDenseMatrix::operator= (const VDDenseMatrix& A)
	{
		Eigen::MatrixXd matTmp = A.m_mat;
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}

	VDDenseMatrix VDDenseMatrix::operator* (const VDDenseMatrix& A) const
	{		
		Eigen::MatrixXd matTmp = m_mat * A.m_mat;
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}

	VDDenseMatrix VDDenseMatrix::operator+ (const VDDenseMatrix& A) const
	{	
		Eigen::MatrixXd matTmp = m_mat + A.m_mat;
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}

	VDDenseMatrix VDDenseMatrix::operator- (const VDDenseMatrix& A) const
	{
		Eigen::MatrixXd matTmp = m_mat - A.m_mat;
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}

	VDDenseMatrix VDDenseMatrix::Transpose() const
	{
		Eigen::MatrixXd matTmp = m_mat.transpose();
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}

	// X以列主序排列,非方阵求解
	void VDDenseMatrix::SolveUnsquare(VSDnsMatData< double, true > & X, const VDDenseMatrix& B, const double dbTolerance)
	{
		Eigen::MatrixXd Matb = B.m_mat;

		Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXd>Solver;

		// 设置迭代精度
		Solver.setTolerance(dbTolerance);
		Solver.compute(m_mat);

		//x1_sparse 即为解
		m_matSlove = Solver.solve(Matb);
		X.row = m_matSlove.rows();
		X.col = m_matSlove.cols();
		X.pData = m_matSlove.data();
	}

	// 求解方阵
	void VDDenseMatrix::SolveSquare(VSDnsMatData< double, true > & X, const VDDenseMatrix& B)
	{
		//Eigen::MatrixXd rhs = B.m_mat;

		////LU分解求解方法要求矩阵可逆，这里用QR分解中householderQr()
		//m_matSlove = m_mat.householderQr().solve(rhs);
		//X.row = m_matSlove.rows();
		//X.col = m_matSlove.cols();
		//X.pData = m_matSlove.data();
	}

	void VDDenseMatrix::QRFactorizate(const VDDenseMatrix& B)
	{
		Eigen::MatrixXd rhs = B.m_mat;

		Eigen::HouseholderQR<Eigen::MatrixXd> qr;
		qr.compute(rhs);
		m_matQ = qr.householderQ();
		// 缺
	}
	void VDDenseMatrix::LUFactorizate(const VDDenseMatrix& B)
	{

	}

	// 计算矩阵的特征向量
	void VDDenseMatrix::EigenValVec(std::vector<VSEigenValueVector>& vEigenValVec)
	{
		Eigen::MatrixXd A = m_mat;

		//获得特征值和特征向量
		Eigen::EigenSolver<Eigen::MatrixXd> es(A);
		//比较特征值得大小，排序，获得x,y,z轴
		vEigenValVec.resize(es.eigenvalues().size());
		for (unsigned i = 0; i < vEigenValVec.size(); i++)
		{
			vEigenValVec[i].dbEigenValue = es.eigenvalues()[i].real();
			Eigen::VectorXcd v = es.eigenvectors().col(i);
			for (size_t j = 0; j < 3; j++)
			{
				vEigenValVec[i].eigenVector[j] = v(j).real();
			}
		}
	}

#if 0
	VDDenseMatrix & operator*(const VDDenseMatrix & A, const VDDenseMatrix & B)
	{
		Eigen::MatrixXd matTmp = A.m_mat * B.m_mat;
		VSDnsMatData<double, true> dnsMatTmp;
		dnsMatTmp.row = matTmp.rows();
		dnsMatTmp.col = matTmp.cols();
		dnsMatTmp.pData = matTmp.data();
		return VDDenseMatrix(dnsMatTmp);
	}
#endif
}

