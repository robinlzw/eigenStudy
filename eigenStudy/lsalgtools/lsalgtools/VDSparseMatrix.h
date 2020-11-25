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
	struct VSSparseMatrixColMajor: VSSprsMatrixColMajorD
	{		
		unsigned Size() { return cbOuterIndice.pData[cbOuterIndice.len - 1]; }

		template<typename F>
		void TraverseValue(F f)
		{
			for (size_t i = 0; i < cbOuterIndice.len-1; i++)
			{
				unsigned nRowS = cbOuterIndice.pData[i];
				unsigned nRowE = cbOuterIndice.pData[i + 1];
				for (size_t j = nRowS; j < nRowE; j++)
				{
					f(cbIndice.pData[j], i, pValues[j]);
				}
			}
		}
	};

	class VDSparseMatrix
	{
	public:
		VDSparseMatrix(const VSSprsMatrixD& sprsMat);	
		VDSparseMatrix::VDSparseMatrix(const VDSparseMatrix& src);
		~VDSparseMatrix();
	
	public:		
		unsigned Rows() { return m_mat.rows(); }
		unsigned Cols() { return m_mat.cols(); }
		VDSparseMatrix operator* (const VDSparseMatrix& A) const;
		VDSparseMatrix operator+ (const VDSparseMatrix& A) const;
		VDSparseMatrix operator- (const VDSparseMatrix& A) const;		
		VDSparseMatrix& operator= (const VDSparseMatrix& mat);

		void GetData(VSSprsMatrixD& sprsMat) const;
		
		VDSparseMatrix Transpose() const;
		void Transpose();
		void SolveUnsquare(VSDnsMatData< double, true > & X, const VDSparseMatrix& B,
			const double dbTolerance = MAT_SOLVER_TOLERANCE);
		void SolveSquare(VSDnsMatData< double, true >& X, const VDSparseMatrix& B);
		void QRFactorizate(VSSparseMatrixColMajor& Q, VSSparseMatrixColMajor& R, VSSprsMatrixD& E);
		void LUFactorizate(VSSparseMatrixColMajor& L, VSSparseMatrixColMajor& U);
		void EigenValVec(std::vector<VSEigenValueVector>& vEigenValVec);
#if 0
		VDSparseMatrix Inverse();
		VDSparseMatrix Adjoint();
		double Determinant();

		// X以列主序排列
#endif
	private:	
		Eigen::SparseMatrix<double>			m_mat;
		std::vector<Eigen::Triplet<double> > m_tripList;
		Eigen::MatrixXd		m_matSlove;
		Eigen::SparseMatrix<double>		m_matQ;
		Eigen::SparseMatrix<double>		m_matR;
		Eigen::SparseQR<Eigen::SparseMatrix< double >, Eigen::COLAMDOrdering<int> >::PermutationType m_matE;
		std::vector<VSTripletD> m_vTripE;
		Eigen::SparseLU<Eigen::SparseMatrix< double >, Eigen::COLAMDOrdering<int> > m_sprsLU;
		Eigen::MatrixXd		m_matL;
		Eigen::MatrixXd		m_matU;
		Eigen::MatrixXd		m_matInv;
	private:		
		void Init(const VSSprsMatrixD& sprsMat);
		void Third2User(VSSprsMatrixD& outMat, std::vector<VSTripletD>& vTrip, const Eigen::SparseMatrix<double>& sprsMat) const;
		void Third2User(VSSprsMatrixColMajorD& outMat, Eigen::SparseMatrix<double>& sprsMat) const;
		void MakeMatrixR(Eigen::MatrixXd& b, const VDSparseMatrix& B);
	};
}