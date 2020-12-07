#include "stdafx.h"
#include <obj/lsalgtools/VDSparseMatrix.h>

namespace NMALG_MATRIX
{
	struct VSTripletAdp : public VSTripletD
	{
		int col() const { return VSTriplet::col; }
		int row() const { return VSTriplet::row; }
		double value() const { return VSTriplet::val; }
	};
	VDSparseMatrix::VDSparseMatrix(const VSSprsMatrixD& sprsMat)
	{
		Init(sprsMat);
	}
	VDSparseMatrix::VDSparseMatrix(const VDSparseMatrix& src)
	{
		VSSprsMatrixD sprsMat;
		src.GetData(sprsMat);
		Init(sprsMat);
	}
	VDSparseMatrix::~VDSparseMatrix()
	{

	}
	
	void VDSparseMatrix::GetData(VSSprsMatrixD& sprsMat) const
	{
		sprsMat.row = m_mat.rows();
		sprsMat.col = m_mat.cols();
		sprsMat.val.len = m_tripList.size();
		sprsMat.val.pData = (VSTripletD*)&m_tripList[0];
	}

	inline VDSparseMatrix& VDSparseMatrix::operator= (const VDSparseMatrix& mat)
	{
		VSSprsMatrixD sprsMat;
		mat.GetData(sprsMat);
		Init(sprsMat);		
		return *this;
	}

	VDSparseMatrix VDSparseMatrix::operator* (const VDSparseMatrix& A) const
	{
		Eigen::SparseMatrix<double> sprsMat = m_mat * A.m_mat;
		VSSprsMatrixD tmpMat;
		VSSprsMatrixD outMat;
		std::vector<VSTripletD> vTrip;
		Third2User(outMat, vTrip, sprsMat);
		return VDSparseMatrix(outMat);
	}

	VDSparseMatrix VDSparseMatrix::operator+ (const VDSparseMatrix& A) const
	{
		Eigen::SparseMatrix<double> sprsMat = m_mat + A.m_mat;
		VSSprsMatrixD tmpMat;
		VSSprsMatrixD outMat;
		std::vector<VSTripletD> vTrip;
		Third2User(outMat, vTrip, sprsMat);
		return VDSparseMatrix(outMat);
	}

	VDSparseMatrix VDSparseMatrix::operator- (const VDSparseMatrix& A) const
	{
		Eigen::SparseMatrix<double> sprsMat = m_mat - A.m_mat;
		VSSprsMatrixD outMat;
		std::vector<VSTripletD> vTrip;
		Third2User(outMat, vTrip, sprsMat);
		return VDSparseMatrix(outMat);
	}
		
	// 矩阵的转置
	VDSparseMatrix VDSparseMatrix::Transpose() const
	{
		Eigen::SparseMatrix<double> sprsMat = m_mat.transpose();
		VSSprsMatrixD outMat;
		std::vector<VSTripletD> vTrip;
		Third2User(outMat, vTrip, sprsMat);
		return VDSparseMatrix(outMat);
	}
	// 矩阵的转置
	void VDSparseMatrix::Transpose()
	{
		m_mat = m_mat.transpose();

		unsigned nIdx = 0;
		for (int k = 0; k < m_mat.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(m_mat, k); it; ++it)
			{
				m_tripList[nIdx++] = Eigen::Triplet<double>(it.row(), it.col(), it.value());
			}
		}
	}
#if 0
	// 行列式非零矩阵的逆
	VDSparseMatrix VDSparseMatrix::Inverse()
	{
		Eigen::SparseMatrix<double> sprsMat = m_mat.cwiseInverse();
		VSSprsMatrixD outMat;
		std::vector<VSTripletD> vTrip;
		Third2User(outMat, vTrip, sprsMat);
		return VDSparseMatrix(outMat);
		Eigen::MatrixXd mat(A.row, A.col);
		mat.setZero();
		for (unsigned i = 0; i < A.val.len; i++)
		{
			mat(A.val.pData[i].row, A.val.pData[i].col) = A.val.pData[i].val;
		}
		Eigen::MatrixXd matinverse = mat.inverse();
		outMat.nRow = matinverse.rows();
		outMat.nCol = matinverse.cols();
		outMat.vValue.resize(outMat.nRow * outMat.nCol);
		unsigned nIdx = 0;
		for (unsigned i = 0; i < outMat.nRow; ++i)
		{
			for (unsigned j = 0; j < outMat.nCol; ++j)
			{
				outMat.vValue[nIdx].row = i;
				outMat.vValue[nIdx].col = j;
				outMat.vValue[nIdx++].val = matinverse(i, j);
			}
		}
	}

	// 矩阵的伴随矩阵
	void NMALG_MATRIX::VCMatrix::MAT_Adjoint(VSSparseMatrix & outMat, const VSSprsMatrixD & A)
	{
		outMat.vValue.clear();
		if (A.row != A.col)
		{
			return;
		}

		Eigen::MatrixXd mat(A.row, A.col);
		mat.setZero();
		for (unsigned i = 0; i < A.val.len; i++)
		{
			mat(A.val.pData[i].row, A.val.pData[i].col) = A.val.pData[i].val;
		}
		Eigen::MatrixXd matinverse = mat.adjoint();
		outMat.nRow = matinverse.rows();
		outMat.nCol = matinverse.cols();
		outMat.vValue.resize(outMat.nRow * outMat.nCol);
		unsigned nIdx = 0;
		for (unsigned i = 0; i < outMat.nRow; ++i)
		{
			for (unsigned j = 0; j < outMat.nCol; ++j)
			{
				outMat.vValue[nIdx].row = i;
				outMat.vValue[nIdx].col = j;
				outMat.vValue[nIdx++].val = matinverse(i, j);
			}
		}
	}

	// 矩阵的行列式
	double NMALG_MATRIX::VCMatrix::MAT_Determinant(const VSSprsMatrixD & A)
	{
		if (A.row != A.col)
		{
			return -1;		// 行列式为负，表示矩阵不能求行列式
		}

		Eigen::MatrixXd mat(A.row, A.col);
		mat.setZero();
		for (unsigned i = 0; i < A.val.len; i++)
		{
			mat(A.val.pData[i].row, A.val.pData[i].col) = A.val.pData[i].val;
		}
		return mat.determinant();
	}
#endif

	
	// X以列主序排列,非方阵求解
	void VDSparseMatrix::SolveUnsquare(VSDnsMatData< double, true > & X, const VDSparseMatrix & B, const double dbTolerance)
	{
		Eigen::MatrixXd b1_sparse;

		MakeMatrixR(b1_sparse, B);
#if 1
		// 压缩优化矩阵
		m_mat.makeCompressed();

		Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double> > Solver_sparse;

		// 设置迭代精度
		Solver_sparse.setTolerance(dbTolerance);
		Solver_sparse.compute(m_mat);

		//x1_sparse 即为解
		m_matSlove = Solver_sparse.solve(b1_sparse);
#else
		x1_sparse = Eigen::SparseQR<Eigen::SparseMatrix< double >, Eigen::COLAMDOrdering<int> >(A1_sparse).solve(b1_sparse);
#endif
		X.row = m_matSlove.rows();
		X.col = m_matSlove.cols();
		X.pData = m_matSlove.data();
	}

	// 求解方阵
	void VDSparseMatrix::SolveSquare(VSDnsMatData< double, true > & X, const VDSparseMatrix& B)
	{
		Eigen::MatrixXd rhs;
		MakeMatrixR(rhs, B);

		Eigen::SparseLU<Eigen::SparseMatrix<double> > Solver_sparse;
		Solver_sparse.compute(m_mat);
		m_matSlove = Solver_sparse.solve(rhs);
		X.row = m_matSlove.rows();
		X.col = m_matSlove.cols();
		X.pData = m_matSlove.data();
	}

	// 矩阵的QR分解
	void VDSparseMatrix::QRFactorizate(VSSparseMatrixColMajor & Q, VSSparseMatrixColMajor & R, VSSprsMatrixD & E)
	{
		Eigen::SparseQR<Eigen::SparseMatrix< double >, Eigen::COLAMDOrdering<int> > sprsQR;
//		VD_F_PROFILE_START(QRFactorizateCompute);
		sprsQR.compute(m_mat);
//		VD_F_PROFILE_FINISH(QRFactorizateCompute);
		//VD_F_PROFILE_START(QRFactorizateQQ);
		//m_matQ = sprsQR.matrixQ();
		//VD_F_PROFILE_FINISH(QRFactorizateQQ);
		//VD_F_PROFILE_START(QRFactorizateQQQ);
		//m_matQ.makeCompressed();
		//Third2User(Q, m_matQ);
		//VD_F_PROFILE_FINISH(QRFactorizateQQQ);
//		VD_F_PROFILE_START(QRFactorizateRR);
		m_matR = sprsQR.matrixR();
		m_matR.makeCompressed();
		Third2User(R, m_matR);
		//VD_F_PROFILE_FINISH(QRFactorizateRR);
		//VD_F_PROFILE_START(QRFactorizateEE);
		m_matE = sprsQR.colsPermutation();
		E.row = m_matE.rows();
		E.col = m_matE.cols();
		m_vTripE.resize(E.row);
		E.val = VD_V2CB(m_vTripE);

		for (int i = 0; i < m_matE.rows(); ++i)
		{
			m_vTripE[i].row = m_matE.indices().coeff(i);
			m_vTripE[i].col = i;
			m_vTripE[i].val = 1.0;
		}
//		VD_F_PROFILE_FINISH(QRFactorizateEE);
	}

	// LU分解
	void VDSparseMatrix::LUFactorizate(VSSparseMatrixColMajor & L, VSSparseMatrixColMajor & U)
	{
		if (m_mat.rows() != m_mat.cols())
			return;

		m_sprsLU.compute(m_mat);
		Eigen::SparseLUMatrixLReturnType<Eigen::internal::MappedSuperNodalMatrix<double, int> > matL = m_sprsLU.matrixL();
		Eigen::SparseLUMatrixUReturnType<Eigen::internal::MappedSuperNodalMatrix<double, int>, Eigen::MappedSparseMatrix<double, 0, int> > matU = m_sprsLU.matrixU();
		m_matE = m_sprsLU.colsPermutation();
	}

	// 计算矩阵的特征向量
	void VDSparseMatrix::EigenValVec(std::vector<VSEigenValueVector>& vEigenValVec)
	{
		Eigen::MatrixXd A(m_mat.rows(), m_mat.cols());
		A.setZero();
		for (int k = 0; k < m_mat.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(m_mat, k); it; ++it)
			{
				A(it.row(), it.col()) = it.value();
			}
		}
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
	//////////////////////////////////////////////////////////////////////////
	// private:
	inline void VDSparseMatrix::Init(const VSSprsMatrixD& sprsMat)
	{
		m_mat.resize(sprsMat.row, sprsMat.col);
		m_tripList.reserve(sprsMat.val.len);
		for (size_t i = 0; i < sprsMat.val.len; i++)
		{
			m_tripList.push_back(Eigen::Triplet<double>(sprsMat.val.pData[i].row,
				sprsMat.val.pData[i].col, sprsMat.val.pData[i].val));
		}
		//const VSTripletAdp * padp = reinterpret_cast<const VSTripletAdp *>(sprsMat.val.pData);
		//m_mat.setFromTriplets(padp, padp + sprsMat.val.len);
		m_mat.setFromTriplets(m_tripList.begin(), m_tripList.end());
	}
	inline void VDSparseMatrix::Third2User(VSSprsMatrixD& outMat, std::vector<VSTripletD>& vTrip,
		const Eigen::SparseMatrix<double>& sprsMat) const
	{
		vTrip.resize(sprsMat.nonZeros());
		unsigned nIdx = 0;
		for (int k = 0; k < sprsMat.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(sprsMat, k); it; ++it)
			{
				vTrip[nIdx].row = it.row();
				vTrip[nIdx].col = it.col();
				vTrip[nIdx++].val = it.value();
			}
		}
		outMat.row = sprsMat.rows();
		outMat.col = sprsMat.cols();
		outMat.val = VD_V2CB(vTrip);
	}
	inline void VDSparseMatrix::Third2User(VSSprsMatrixColMajorD& outMat, Eigen::SparseMatrix<double>& sprsMat) const
	{
		outMat.row = sprsMat.rows();
		outMat.col = sprsMat.cols();
		outMat.cbOuterIndice.len = sprsMat.outerSize() + 1;
		outMat.cbOuterIndice.pData = reinterpret_cast<unsigned*>(sprsMat.outerIndexPtr());
		outMat.cbIndice.len = sprsMat.data().size();
		outMat.cbIndice.pData = reinterpret_cast<unsigned*>(sprsMat.data().indexPtr());
		outMat.pValues = sprsMat.data().valuePtr();
	}
	inline void VDSparseMatrix::MakeMatrixR(Eigen::MatrixXd& b, const VDSparseMatrix& B)
	{
		Eigen::MatrixXd b1_sparse(B.m_mat.rows(), B.m_mat.cols());

		b1_sparse.setZero();
		for (int k = 0; k < B.m_mat.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(B.m_mat, k); it; ++it)
			{
				b1_sparse(it.row(), it.col()) = it.value();
			}
		}

	}
}

