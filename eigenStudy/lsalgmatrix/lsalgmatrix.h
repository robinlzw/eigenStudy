#pragma once
#include "obj/alg/ialgmatrix.h"
#include "eigen/Eigen/Dense"
#include "eigen/Eigen/Sparse"
#include "eigen/Eigen/IterativeLinearSolvers"
#include "eigen/Eigen/SparseQR"
#include "eigen/Eigen/OrderingMethods"
#include "eigen/Eigen/SuperLUSupport"
#include "eigen/Eigen/SPQRSupport"


namespace NMALG_MATRIX
{

	struct VSSprsMatThird
	{
		Eigen::SparseMatrix<double> sprsMat;
		std::vector<Eigen::Triplet<double> > tripList;
	};
}
