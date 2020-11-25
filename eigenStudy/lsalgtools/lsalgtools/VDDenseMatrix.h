#pragma once
#include <eigen/Eigen/Dense>
#include <eigen/Eigen/Sparse>
#include <eigen/Eigen/IterativeLinearSolvers>
#include <eigen/Eigen/SparseQR>
#include <eigen/Eigen/SparseLU>
#include <eigen/Eigen/OrderingMethods>
#include <obj/alg/stdnotdef.h>
#include <vmath/vflocation.h>
#include <vmath/vfmesh.h>
#include <vstd/tisys.h>
#include <obj/ialgeq/ialgeq.h>
#include <obj/alg/ialgmatrix.h>
#include "stdafx.h"

namespace NMALG_MATRIX
{
	class VDDenseMatrix
	{
	public:
		VDDenseMatrix(const VSDnsMatData<double, true>& dnsMat);
		~VDDenseMatrix();
	
	public:
		//friend VDDenseMatrix & operator*(const VDDenseMatrix & A, const VDDenseMatrix & B);
		VDDenseMatrix operator* (const VDDenseMatrix& A) const;
		VDDenseMatrix operator+ (const VDDenseMatrix& A) const;
		VDDenseMatrix operator- (const VDDenseMatrix& A) const;
		VDDenseMatrix& operator= (const VDDenseMatrix& A);

		VDDenseMatrix Transpose() const;
		void SolveUnsquare(VSDnsMatData< double, true > & X, const VDDenseMatrix& B,
			const double dbTolerance = MAT_SOLVER_TOLERANCE);
		void SolveSquare(VSDnsMatData< double, true >& X, const VDDenseMatrix& B);
		void QRFactorizate(const VDDenseMatrix& B);
		void LUFactorizate(const VDDenseMatrix& B);
		void EigenValVec(std::vector<VSEigenValueVector>& vEigenValVec);

#if 0
		friend VSSparseMatrix& operator * (const VSSprsMatrixD& A, const VSSprsMatrixD& B);
		friend VSSparseMatrix& operator + (const VSSprsMatrixD& A, const VSSprsMatrixD& B);
		friend VSSparseMatrix& operator - (const VSSprsMatrixD& A, const VSSprsMatrixD& B);

		void MAT_AMultiDiagMat(VSSparseMatrix& outMat, const VSSprsMatrixD& A, const VSSprsMatrixD& diagMat);
		void MAT_DiagMatMultiA(VSSparseMatrix& outMat, const VSSprsMatrixD& diagMat, const VSSprsMatrixD& A);
		void MAT_Transpose(VSSparseMatrix& outMat, const VSSprsMatrixD& A);
		void MAT_Inverse(VSSparseMatrix& outMat, const VSSprsMatrixD& A);
		void MAT_Adjoint(VSSparseMatrix& outMat, const VSSprsMatrixD& A);
		double MAT_Determinant(const VSSprsMatrixD& A);

		// X以列主序排列
		void MAT_SolveAnyMat(VSSparseMatrix& X, const VSSprsMatrixD& A,
			const VSSprsMatrixD& B, const double dbTolerance = MAT_SOLVER_TOLERANCE);

		void MAT_SolveSquareMat(std::vector<double>& X, const VSSprsMatrixD & A, const VSSprsMatrixD& B);
		void MAT_QRFactorizate(VSSparseMatrix& vQ, VSSparseMatrix& vR,
			VSSparseMatrix& vE, const VSSprsMatrixD& A);
		void MAT_LUFactorizate(VSSparseMatrix& vL, VSSparseMatrix& vU, const VSSprsMatrixD& A);
		void MAT_GetEigenValVec(std::vector<VSEigenValueVector>& vEigenValVec, const VSSprsMatrixD& mat);
#endif
	
	private:
		Eigen::MatrixXd		m_mat;
		Eigen::MatrixXd		m_matSlove;
		Eigen::MatrixXd		m_matQ;
		Eigen::MatrixXd		m_matR;
		Eigen::MatrixXd		m_matE;
		Eigen::MatrixXd		m_matL;
		Eigen::MatrixXd		m_matU;
		Eigen::MatrixXd		m_matInv;
	};
}