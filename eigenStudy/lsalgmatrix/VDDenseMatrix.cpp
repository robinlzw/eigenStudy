#include "stdafx.h"
#include <obj/lsalgtools/VDDenseMatrix.h>

#if 0
// ���������洢����ת��Ϊ��������EIGEN�еľ���洢����
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

	// X������������,�Ƿ������
	void VDDenseMatrix::SolveUnsquare(VSDnsMatData< double, true > & X, const VDDenseMatrix& B, const double dbTolerance)
	{
		Eigen::MatrixXd Matb = B.m_mat;

		Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXd>Solver;

		// ���õ�������
		Solver.setTolerance(dbTolerance);
		Solver.compute(m_mat);

		//x1_sparse ��Ϊ��
		m_matSlove = Solver.solve(Matb);
		X.row = m_matSlove.rows();
		X.col = m_matSlove.cols();
		X.pData = m_matSlove.data();
	}

	// ��ⷽ��
	void VDDenseMatrix::SolveSquare(VSDnsMatData< double, true > & X, const VDDenseMatrix& B)
	{
		//Eigen::MatrixXd rhs = B.m_mat;

		////LU�ֽ���ⷽ��Ҫ�������棬������QR�ֽ���householderQr()
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
		// ȱ
	}
	void VDDenseMatrix::LUFactorizate(const VDDenseMatrix& B)
	{

	}

	// ����������������
	void VDDenseMatrix::EigenValVec(std::vector<VSEigenValueVector>& vEigenValVec)
	{
		Eigen::MatrixXd A = m_mat;

		//�������ֵ����������
		Eigen::EigenSolver<Eigen::MatrixXd> es(A);
		//�Ƚ�����ֵ�ô�С�����򣬻��x,y,z��
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

