#include "stdafx.h"
#include<iomanip>
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/profile.h>
#include <obj/alg/ialgmatrix.h>
//#include <slu_ddefs.h>
#include "lsalgmatrix.h"

//#define SPEED_DEBUG
#ifdef SPEED_DEBUG
#define VD_PROFILE_START( profile ) VSFProfile::StartProfile(#profile,"E:/testdata/profile.txt");
#define VD_PROFILE_FINISH( profile ) VSFProfile::FinishProfile(#profile,"E:/testdata/profile.txt");
#else
#define VD_PROFILE_START( profile )
#define VD_PROFILE_FINISH( profile )
#endif
namespace NMALG_MATRIX
{
	static void TransEigenMatFromUser(VSSprsMatThird& sprsMat, const VSSprsMatrixD& userSprsMat)
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
	
	//////////////////////////////////////////////////////////////////////////
	// X以列主序排列
	void MAT_SolveAnyMat(VSSparseMatrix& X, const VSSprsMatrixD& A,
		const VSSprsMatrixD& B, const double dbTolerance)
	{
		VSSprsMatThird thidMatA;
		TransEigenMatFromUser(thidMatA, A);
		Eigen::MatrixXd b1_sparse(B.row, B.col);
		Eigen::MatrixXd x1_sparse;

		b1_sparse.setZero();
		for (unsigned i = 0; i < B.val.len; i++)
		{
			b1_sparse(B.val.pData[i].row, B.val.pData[i].col) = B.val.pData[i].val;
		}

#if 1
		// 压缩优化矩阵
		thidMatA.sprsMat.makeCompressed();

		Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double> > Solver_sparse;

		// 设置迭代精度
		Solver_sparse.setTolerance(dbTolerance);
		Solver_sparse.compute(thidMatA.sprsMat);

		//x1_sparse 即为解
		x1_sparse = Solver_sparse.solve(b1_sparse);
#else
		x1_sparse = Eigen::SparseQR<Eigen::SparseMatrix< double >, Eigen::COLAMDOrdering<int> >(A1_sparse).solve(b1_sparse);
#endif
		X.nRow = x1_sparse.rows();
		X.nCol = x1_sparse.cols();
		X.vValue.resize(x1_sparse.rows() * x1_sparse.cols());
		unsigned nIdx = 0;
		for (unsigned i = 0; i < x1_sparse.rows(); i++)
		{
			for (size_t j = 0; j < x1_sparse.cols(); j++)
			{
				X.vValue[nIdx].row = i;
				X.vValue[nIdx].col = j;
				X.vValue[nIdx++].val = x1_sparse(i, j);
			}
		}
	}

	void MAT_SolveSquareMat(std::vector<double> & X, const VSSprsMatrixD & A, const VSSprsMatrixD& B)
	{
		static int nSolveCount = 0;
		char szFileName[256] = { 0 };
		nSolveCount++;
		VSSprsMatThird thidMatA;
		TransEigenMatFromUser(thidMatA, A);

		Eigen::MatrixXd rhs(B.row, B.col);
		Eigen::MatrixXd x1_sparse;
		rhs.setZero();

		for (unsigned i = 0; i < B.val.len; i++)
		{
			rhs(B.val.pData[i].row, B.val.pData[i].col) = B.val.pData[i].val;
		}
		thidMatA.sprsMat.makeCompressed();

		{
			VD_PROFILE_START(Eigen::SparseLU);

			Eigen::SuperLU < Eigen::SparseMatrix < double > > Solver_sparse;

			Solver_sparse.compute(thidMatA.sprsMat);
			x1_sparse = Solver_sparse.solve(rhs);
			VD_PROFILE_FINISH(Eigen::SparseLU);
		}


	
		X.resize(B.row* B.col);
		for (size_t i = 0; i < B.row; i++)
		{
			for (size_t j = 0; j < B.col; j++)
			{
				X[j * B.row + i] = x1_sparse(i, j);
			}
		}

	}

	void MAT_AMultiDiagMat(VSSparseMatrix& outMat, const VSSprsMatrixD& A, const VSSprsMatrixD& diagMat)
	{
		outMat.nRow = A.row;
		outMat.nCol = diagMat.col;
		outMat.vValue.resize(A.val.len);
		for (size_t i = 0; i < A.val.len; i++)
		{
			outMat.vValue[i].row = A.val.pData[i].row;
			outMat.vValue[i].col = A.val.pData[i].col;
			outMat.vValue[i].val = A.val.pData[i].val * diagMat.val.pData[A.val.pData[i].col].val;
		}

	}

	void MAT_DiagMatMultiA(VSSparseMatrix& outMat, const VSSprsMatrixD& diagMat, const VSSprsMatrixD& A)
	{
		outMat.nRow = diagMat.row;
		outMat.nCol = A.col;
		outMat.vValue.resize(A.val.len);
		for (size_t i = 0; i < A.val.len; i++)
		{
			outMat.vValue[i].row = A.val.pData[i].row;
			outMat.vValue[i].col = A.val.pData[i].col;
			outMat.vValue[i].val = A.val.pData[i].val * diagMat.val.pData[A.val.pData[i].row].val;
		}
	}

	void MAT_AMultiB(VSSparseMatrix& outMat, const VSSprsMatrixD& A, const VSSprsMatrixD& B)
	{
		outMat.vValue.clear();

		VSSprsMatThird thidMatA;
		VSSprsMatThird thidMatB;
		TransEigenMatFromUser(thidMatA, A);
		TransEigenMatFromUser(thidMatB, B);
		
		Eigen::SparseMatrix<double> AOut = thidMatA.sprsMat*thidMatB.sprsMat;
		outMat.nRow = AOut.rows();
		outMat.nCol = AOut.cols();
		outMat.vValue.resize(AOut.nonZeros());
		unsigned nIdx = 0;
		for (int k = 0; k < AOut.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(AOut, k); it; ++it)
			{
				outMat.vValue[nIdx].row = it.row();
				outMat.vValue[nIdx].col = it.col();
				outMat.vValue[nIdx++].val = it.value();

 
			}
		}

	}

	void MAT_AAddB(VSSparseMatrix& outMat, const VSSprsMatrixD& A, const VSSprsMatrixD& B)
	{
		double dbPrecision = 1e-18;
		if ((A.row != B.row) || (A.col != B.col))
		{
			return;
		}

		{
			VSSprsMatThird thidMat1;
			VSSprsMatThird thidMat2;
			TransEigenMatFromUser(thidMat1, A);
			TransEigenMatFromUser(thidMat2, B);
			Eigen::SparseMatrix<double> smOut = thidMat1.sprsMat + thidMat2.sprsMat;
			outMat.nRow = smOut.rows();
			outMat.nCol = smOut.cols();
			outMat.vValue.resize(smOut.nonZeros());
			unsigned nIdx = 0;
			for (int k = 0; k < smOut.outerSize(); ++k)
			{
				for (Eigen::SparseMatrix<double>::InnerIterator it(smOut, k); it; ++it)
				{
					outMat.vValue[nIdx].row = it.row();
					outMat.vValue[nIdx].col = it.col();
					outMat.vValue[nIdx++].val = it.value();
				}
			}
		}
	}

	void MAT_AMinusB(VSSparseMatrix& outMat, const VSSprsMatrixD& A, const VSSprsMatrixD& B)
	{
		double dbPrecision = 1e-18;
		if ((A.row != B.row) || (A.col != B.col))
		{
			return;
		}

		{
			VSSprsMatThird thidMat1;
			VSSprsMatThird thidMat2;
			TransEigenMatFromUser(thidMat1, A);
			TransEigenMatFromUser(thidMat2, B);
			Eigen::SparseMatrix<double> smOut = thidMat1.sprsMat - thidMat2.sprsMat;
			outMat.nRow = A.row;
			outMat.nCol = B.col;
			outMat.vValue.resize(smOut.nonZeros());
			unsigned nIdx = 0;
			for (int k = 0; k < smOut.outerSize(); ++k)
			{
				for (Eigen::SparseMatrix<double>::InnerIterator it(smOut, k); it; ++it)
				{
					outMat.vValue[nIdx].row = it.row();
					outMat.vValue[nIdx].col = it.col();
					outMat.vValue[nIdx++].val = it.value();
				}
			}
		}

	}

	void MAT_QRFactorizate(VSSparseMatrix& vQ, VSSparseMatrix& vR,
		VSSparseMatrix& vE, const VSSprsMatrixD& A)
	{		
		static int nQRCount = 0;
		nQRCount++;
		unsigned nRunTime = GetTickCount();

		char szFileName[256] = { 0 };
		VSSprsMatThird sprsMat;
		Eigen::SparseMatrix<double> R;
		//Eigen::SparseMatrix<double> Q;
		VD_PROFILE_START(MAT_QRFactorizate1);
		TransEigenMatFromUser(sprsMat, A);
		VD_PROFILE_FINISH(MAT_QRFactorizate1);
		VD_PROFILE_START(MAT_QRFactorizate2);

	Eigen::SPQR < Eigen::SparseMatrix < double > > sprsQR;
	VSTripletD trip;
	sprsMat.sprsMat.makeCompressed();

	

	//VD_F_PROFILE_START(MAT_QRFactorizate);
	sprsQR.compute(sprsMat.sprsMat);
	

	R = sprsQR.matrixR();// .topLeftCorner(sprsQR.rank(), sprsQR.rank());
	
	vR.nRow = R.rows();
	vR.nCol = R.cols();

	for (int k = 0; k < R.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(R, k); it; ++it)
		{
			trip.row = it.row();
			trip.col = it.col();
			trip.val = it.value();
			vR.vValue.push_back(trip);
		}
	}

	const Eigen::SPQR < Eigen::SparseMatrix < double > >::PermutationType& E = sprsQR.colsPermutation();
	

	vE.nRow = E.rows();
	vE.nCol = E.cols();
	vE.vValue.clear();
	for (int i = 0; i < E.rows(); ++i)
	{
		VSTripletD trip;
		trip.col = i;
		trip.row = E.indices().coeff(i);
		trip.val = 1.0;
		vE.vValue.push_back(trip);
	}

	}

	void MAT_Transpose(VSSparseMatrix& outMat, const VSSprsMatrixD& A)
	{
		outMat.nRow = A.col;
		outMat.nCol = A.row;
		outMat.vValue.resize(A.val.len);
		for (unsigned i = 0; i < A.val.len; i++)
		{
			outMat.vValue[i].row = A.val.pData[i].col;
			outMat.vValue[i].col = A.val.pData[i].row;
			outMat.vValue[i].val = A.val.pData[i].val;
		}
	}

	void MAT_GetEigenValVec(std::vector<VSEigenValueVector>& vEigenValVec, const VSSprsMatrixD& mat)
	{
		Eigen::MatrixXd A(mat.row, mat.col);
		A.setZero();
		for (unsigned i = 0; i < mat.val.len; i++)
		{
			A(mat.val.pData[i].row, mat.val.pData[i].col) = mat.val.pData[i].val;
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


	// 行列式非零矩阵的逆
	void MAT_Inverse(VSSparseMatrix & outMat, const VSSprsMatrixD & A)
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

}
