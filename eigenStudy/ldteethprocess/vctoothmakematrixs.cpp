#include "stdafx.h"
#include<iomanip>
#include <pkg/ldck4cce.h>
#include <pkg/ldwzkupdater.h>
#include <obj/alg/ialgmatrix.h>
#include <obj/lsalgtools/ilsalgtoolsperfmesh.h>
#include <obj/lsalgtools/ilsalgtools.h>
#include <obj/lsalgtools/VDSparseMatrix.h>
#include <obj/lsalgtools/profile.h>
#include "vctoothmakematrixs.h"

#define SOLVER_TOLERANCE	1.0e-10

void MakeMatrixL(std::vector<VSTriplet<double> >& L, const VSPerfectMesh& perfMesh)
{
	// 12. 带碰撞约束的牙齿网格调整
	//		1) 构造矩阵L：
	//			l(i,j) = cot(angle1) + cot(angle2)
	//			l(i,i) = -(l(i,j) + l(i, k) + l(i, m) ...)		
	//			cot(angle1),cot(angle2)为边i,j的相邻三角片对面顶点的夹角的余切
	//			行数=列数=nv*3	

	unsigned nSurfCount = perfMesh.GetSurfCount();
	unsigned nEdgeCount = perfMesh.GetEdgeCount();
	unsigned nVertCount = perfMesh.GetVertexCount();
	std::vector<unsigned> vEdgeFlag(nEdgeCount, 0);
	VSTripletD trip;
	memset(&trip, 0, sizeof(VSTripletD));
	L.resize(nVertCount + nEdgeCount * 2, trip);

	auto CalcCotValue=[&perfMesh](const unsigned nIdx, const VNVECTOR3UI& surf) -> double{
		const VFVECTOR3* pV[3];
		for (size_t i = 0; i < 3; i++)
		{
			pV[i] = &perfMesh.pVertices[surf[i]];
		}
		VFVECTOR3 v1 = *pV[(nIdx + 1) % 3] - *pV[nIdx];
		VFVECTOR3 v2 = *pV[(nIdx + 2) % 3] - *pV[nIdx];// *pV[(nIdx + 1) % 3];
		//v1.Normalize();
		//v2.Normalize();
		double dbCosVal = v1.Dot(v2);
		double dbSinVal = (v1.Cross(v2)).Magnitude();
		return dbCosVal / dbSinVal /2.0;
	};

	for (size_t nSurfIdx = 0; nSurfIdx < nSurfCount; nSurfIdx++)
	{
		VNVECTOR3UI eos = perfMesh.GetEdgeIndxOfSurf(nSurfIdx);
		VNVECTOR3UI f = perfMesh.pSurfaces[nSurfIdx];
		for (size_t i = 0; i < 3; i++)
		{
			double dbCotVal = CalcCotValue(i, f);			
			VNVECTOR2UI voe = perfMesh.GetVertIndxOfEdge(eos[i]);
			unsigned nV1 = voe[0];
			unsigned nV2 = voe[1];
			if (nV1 > nV2)
			{
				nV1 = voe[1];
				nV2 = voe[0];
			}
			L[nV1].row = L[nV1].col = nV1;
			L[nV1].val -= dbCotVal;
			L[nV2].row = L[nV2].col = nV2;			
			L[nV2].val -= dbCotVal;
			
			L[nVertCount + eos[i]].row = nV1;
			L[nVertCount + eos[i]].col = nV2;
			L[nVertCount + nEdgeCount + eos[i]].row = nV2;
			L[nVertCount + nEdgeCount + eos[i]].col = nV1;
			L[nVertCount + eos[i]].val += dbCotVal;
			L[nVertCount + nEdgeCount + eos[i]].val += dbCotVal;
		}
	}
#if 0
	std::ofstream logF("D:/testdata/ld/LMat.txt");
	for (size_t i = 0; i < L.size(); i++)
	{
		logF << L[i].row << " " << L[i].col << " " << L[i].val << std::endl;
	}
	logF.close();
#endif
}

void MakeMatrixM(std::vector<VSTriplet<double> >& M, const VSPerfectMesh& perfMesh)
{
	// 12. 带碰撞约束的牙齿网格调整
	//		2) 构造矩阵M：
	//			m(i,i) = (s1+s2+...)/3; s1,s2...为顶点i的一阶领域三角片面积。
	//	
	unsigned nVertCount = perfMesh.GetVertexCount();
	unsigned nSurfCount = perfMesh.GetSurfCount();
	M.resize(nVertCount);
	std::vector<double> vSurfArea(nSurfCount);
	for (size_t i = 0; i < nSurfCount; i++)
	{
		const VNVECTOR3UI& surf = perfMesh.pSurfaces[i];
		VFVECTOR3 v[3];
		for (size_t j = 0; j < 3; j++)
		{
			v[j] = perfMesh.pVertices[surf[j]];
		}
		VFVECTOR3 p1 = v[1] - v[0];
		VFVECTOR3 p2 = v[2] - v[0];
		vSurfArea[i] = p1.Cross(p2).Magnitude() / 2.0f;
	}
	for (unsigned i = 0; i < nVertCount; i++)
	{
		unsigned nCount = perfMesh.GetSurfCountOfVetex(i);
		double dbAreaVal = 0.0;
		for (unsigned j = 0; j < nCount; j++)
		{
			unsigned nSurfIdx = perfMesh.GetSurfIndxOfVertex(i, j);
			dbAreaVal += vSurfArea[nSurfIdx];
		}
		
		M[i].row = M[i].col = i;
		M[i].val = (3.0/dbAreaVal );
	}
#if 0
	std::ofstream logF("D:/testdata/ld/MMat.txt");
	for (size_t i = 0; i < M.size(); i++)
	{
		logF << M[i].row << " " << M[i].col << " " << M[i].val << std::endl;
	}
	logF.close();
#endif
}

void MakeMatrixBndry(std::vector<VSTripletD >& bndryMat, const VSConstBuffer<VFVECTOR3>& cbBndryVerts,
	const VSConstBuffer<VNVECTOR3UI>& vSurfaces, const unsigned nVertCount)
{
	TVExtSource< VNALGMESH::VRPerfGraph >	extVRPerfGraph;
	TVR2B< VNALGMESH::VRPerfGraph> vrPerfGraph(*extVRPerfGraph);
	VSSimpleGraph simpGraph;
	VSPerfTopoGraph topoGraph;

	simpGraph.nVertCount = nVertCount;
	simpGraph.nTriangleCount = vSurfaces.len;
	simpGraph.pTriangles = vSurfaces.pData;

	vrPerfGraph.Build(topoGraph, simpGraph);
	bndryMat.resize(nVertCount);
	

	for (size_t nVertIdx = 0; nVertIdx < nVertCount; nVertIdx++)
	{
		bndryMat[nVertIdx].row = bndryMat[nVertIdx].col = nVertIdx;
		bndryMat[nVertIdx].val = 1.0;		
	}
	for (size_t nVertIdx = cbBndryVerts.len; nVertIdx < nVertCount; nVertIdx++)
	{
		std::vector<unsigned> vDomain;
		VSTripletD trip;
		::PMGet1OrderDomain(vDomain, nVertIdx, topoGraph);
		trip.row = nVertIdx;
		trip.val = -1.0 / (double)(vDomain.size());
		for (size_t i = 0; i < vDomain.size(); i++)
		{
			trip.col = vDomain[i];
			bndryMat.push_back(trip);
		}
	}
}

#define  DEBUG_MATRIX_1
void QuadProgFixed(std::vector<VSTripletD>& X, std::vector<VSTripletD>& vLambda,
	const VSSprsMatrixD& A, const VSSprsMatrixD& B,
	const VSSprsMatrixD& Aeq, const VSSprsMatrixD& Beq)
{	
	//[X, lambda] = quad_prog_fixed(A, B, Aeq, Beq)
	std::vector<VSTripletD> lhs;
	std::vector<VSTripletD> rhs;

	if (Aeq.row > 0)
	{
		NMALG_MATRIX::VSSparseMatrix vMatAeqTrans;
		NMALG_MATRIX::MAT_Transpose(vMatAeqTrans, Aeq);
		VSSprsMatrixD sprsMatAeqTrans;
		sprsMatAeqTrans.row = Aeq.col;
		sprsMatAeqTrans.col = Aeq.row;
		sprsMatAeqTrans.val = VD_V2CB(vMatAeqTrans.vValue);
		VSSprsMatrixD sprsMatE;
		VSSprsMatrixD sprsMatETrans;
		int nLastNotZero = 0;
		//	Aeq_n = E' * Aeq;
		//	Beq_n = E' * Beq;
		NMALG_MATRIX::VSSparseMatrix Aeq_n;
		NMALG_MATRIX::VSSparseMatrix Beq_n;

		NMALG_MATRIX::VSSparseMatrix vQ;
		NMALG_MATRIX::VSSparseMatrix vR;
		NMALG_MATRIX::VSSparseMatrix vE;
		NMALG_MATRIX::VSSparseMatrix vETrans;
#if 0
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/QRMatrix%d.txt", nCountCall);
			std::ofstream objFile(szFileName);
			for (int k = 0; k < vMatAeqTrans.vValue.size(); ++k)
			{
				objFile << vMatAeqTrans.vValue[k].row << " " << vMatAeqTrans.vValue[k].col << " " << vMatAeqTrans.vValue[k].val << std::endl;
			}
			objFile.close();
			sprintf_s(szFileName, 256, "D:/testdata/ld/BMatrix%d.txt", nCountCall);
			objFile.open(szFileName);
			for (int k = 0; k < B.val.len; ++k)
			{
				objFile << B.val.pData[k].row << " " << B.val.pData[k].col << " " << B.val.pData[k].val << std::endl;
			}
			objFile.close();
		}
#endif
		//[Q, R, E] = qr(Aeq');	

		VD_PROFILE_START(MAT_QRFactorizate);
		NMALG_MATRIX::MAT_QRFactorizate(vQ, vR, vE, sprsMatAeqTrans);
		VD_PROFILE_FINISH(MAT_QRFactorizate);
		sprsMatE.row = vE.nRow;
		sprsMatE.col = vE.nCol;
		sprsMatE.val = VD_V2CB(vE.vValue);
		for (unsigned i = 0; i < vR.vValue.size(); i++)
		{
			if (vR.vValue[i].row > nLastNotZero)
			{
				nLastNotZero = vR.vValue[i].row;
			}
		}
#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/RMatrix%d.txt", nCountCall);
			std::ofstream objFile(szFileName);
			for (int k = 0; k < vR.vValue.size(); ++k)
			{
				objFile << vR.vValue[k].row << " " << vR.vValue[k].col << " " << vR.vValue[k].val << std::endl;
			}
			objFile.close();
			sprintf_s(szFileName, 256, "D:/testdata/ld/EMatrix%d.txt", nCountCall);
			objFile.open(szFileName);
			for (int k = 0; k < vE.vValue.size(); ++k)
			{
				objFile << vE.vValue[k].row << " " << vE.vValue[k].col << " " << vE.vValue[k].val << std::endl;
			}
			objFile.close();
		}
#endif
		NMALG_MATRIX::MAT_Transpose(vETrans, sprsMatE);
		sprsMatETrans.row = vETrans.nRow;
		sprsMatETrans.col = vETrans.nCol;
		sprsMatETrans.val = VD_V2CB(vETrans.vValue);
#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/sparseqr%d.txt", nCountCall);
			std::ofstream txtF(szFileName);
			for (size_t i = 0; i < vETrans.vValue.size(); i++)
			{
				txtF << vETrans.vValue[i].row << " " << vETrans.vValue[i].col << " " << vETrans.vValue[i].val << std::endl;
			}

			txtF.close();
		}
#endif

		//	Aeq_n = E' * Aeq;
		//	Beq_n = E' * Beq;
		NMALG_MATRIX::MAT_AMultiB(Aeq_n, sprsMatETrans, Aeq);
		NMALG_MATRIX::MAT_AMultiB(Beq_n, sprsMatETrans, Beq);
#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/Aeq_n%d.txt", nCountCall);
			std::ofstream txtF(szFileName);
			for (size_t i = 0; i < Aeq_n.vValue.size(); i++)
			{
				txtF << Aeq_n.vValue[i].row << " " << Aeq_n.vValue[i].col << " " << Aeq_n.vValue[i].val << std::endl;
			}

			txtF.close();
			sprintf_s(szFileName, 256, "D:/testdata/ld/Beq_n%d.txt", nCountCall);
			txtF.open(szFileName);
			for (size_t i = 0; i < Beq_n.vValue.size(); i++)
			{
				txtF << Beq_n.vValue[i].row << " " << Beq_n.vValue[i].col << " " << Beq_n.vValue[i].val << std::endl;
			}

			txtF.close();
		}
#endif
	
		//	lhs = [2 * A, Aeq_n(1:n, : )'; Aeq_n(1:n,:), sparse(n, n)];
		//	rhs = [-B; Beq_n(1:n, : )];
	
		lhs.resize(A.val.len + Aeq_n.vValue.size() * 2);

		unsigned nIdx = 0;
		for (unsigned i = 0; i < A.val.len; i++)
		{
			//按Triplet方式填充，速度快
			lhs[nIdx].row = A.val.pData[i].row;
			lhs[nIdx].col = A.val.pData[i].col;
			lhs[nIdx++].val = A.val.pData[i].val * 2.0;
		}
		unsigned nColBase = A.col;

		//Eigen::SparseMatrix<double> Aeq_nTrans = Aeq_n.transpose();
		// Aeq_n(1:n,:)'
		for (int k = 0; k < Aeq_n.vValue.size(); ++k)
		{
			if (Aeq_n.vValue[k].row <= nLastNotZero)
			{
				lhs[nIdx].row = Aeq_n.vValue[k].col;
				lhs[nIdx].col = Aeq_n.vValue[k].row + nColBase;
				lhs[nIdx++].val = Aeq_n.vValue[k].val;
			}
		}
		// Aeq_n(1:n,:)
		unsigned nRowBase = A.row;
		for (int k = 0; k < Aeq_n.vValue.size(); ++k)
		{
			if (Aeq_n.vValue[k].row <= nLastNotZero)
			{
				lhs[nIdx].row = Aeq_n.vValue[k].row + nRowBase;
				lhs[nIdx].col = Aeq_n.vValue[k].col;
				lhs[nIdx++].val = Aeq_n.vValue[k].val;
			}
		}
		lhs.resize(nIdx);
		rhs.resize(B.val.len + Beq_n.vValue.size());
		nIdx = 0;
		for (unsigned i = 0; i < B.val.len; i++)
		{
			rhs[nIdx].row = B.val.pData[i].row;
			rhs[nIdx].col = B.val.pData[i].col;
			rhs[nIdx++].val = B.val.pData[i].val * (-1.0);
		}
		nRowBase = B.row;
		for (int k = 0; k < Beq_n.vValue.size(); ++k)
		{
			if (Beq_n.vValue[k].row <= nLastNotZero)
			{
				rhs[nIdx].row = Beq_n.vValue[k].row + nRowBase;
				rhs[nIdx].col = Beq_n.vValue[k].col;
				rhs[nIdx++].val = Beq_n.vValue[k].val;
			}
		}
		rhs.resize(nIdx);
		VSSprsMatrixD sprsMatLHS;
		VSSprsMatrixD sprsMatRHS;
		std::vector<double> x1_sparse;

		sprsMatLHS.row = A.row + nLastNotZero + 1;
		sprsMatLHS.col = A.col + nLastNotZero + 1;
		sprsMatLHS.val = VD_V2CB(lhs);
		sprsMatRHS.row = B.row + nLastNotZero + 1;
		sprsMatRHS.col = B.col;
		sprsMatRHS.val = VD_V2CB(rhs);
	#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/matlhs%d.txt", nCountCall);
			std::ofstream txtL(szFileName);
			sprintf_s(szFileName, 256, "D:/testdata/ld/matrhs%d.txt", nCountCall);
			std::ofstream txtR(szFileName);
			for (size_t i = 0; i <lhs.size(); i++)
			{
				txtL << lhs[i].row << " " << lhs[i].col << " " << lhs[i].val << std::endl;
			}
			txtL.close();
			for (size_t i = 0; i < rhs.size(); i++)
			{
				txtR << rhs[i].row << " " << rhs[i].col << " " << rhs[i].val << std::endl;
			}
			txtR.close();
		}
	#endif
	
	
			VD_PROFILE_START(MAT_SolveSquareMat);
	#if 0
			std::ofstream txtF("d:/testdata/ld/profile.txt", std::ios::app);
			txtF << "matrix row: " << sprsMatLHS.row << " " << sprsMatLHS.col << std::endl;
			txtF.close();
	#endif

		NMALG_MATRIX::MAT_SolveSquareMat(x1_sparse, sprsMatLHS, sprsMatRHS);
	
		VD_PROFILE_FINISH(MAT_SolveSquareMat);
		//X = temp(1:size(A, 1));
	#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/XMatrix_%d.txt", nCountCall);
			std::ofstream txtX(szFileName);
			txtX << x1_sparse.size() << " " << 1 << " 0" << std::endl;
			for (size_t i = 0; i < x1_sparse.size(); i++)
			{
				txtX << i << " " << 0 << " " << std::fixed << std::setprecision(18) << x1_sparse[i] << std::endl;
			}
			txtX.close();
		}
	#endif
		X.resize(A.row);
		for (size_t i = 0; i < A.row; i++)
		{
			X[i].row = i;
			X[i].col = 0;
			X[i].val = x1_sparse[i + 0 * A.row];
		}

		//lambda = zeros(size(Aeq, 1), 1);
		//[r, c] = find(E(:, 1 : n));
		//lambda(r) = temp(size(A, 1) + 1:end);
		vLambda.resize(Aeq.row);
		for (size_t i = 0; i < Aeq.row; i++)
		{
			vLambda[i].row = i;
			vLambda[i].col = 0;
			vLambda[i].val = 0.0;
		}

		for (size_t i = 0; i < vE.vValue.size(); i++)
		{
			if (vE.vValue[i].col <= nLastNotZero)
			{
				vLambda[vE.vValue[i].row].val = x1_sparse[vE.vValue[i].col + A.row + 0 * sprsMatLHS.row];
			}
		}
	}
	else
	{
		lhs.resize(A.val.len);

		for (unsigned i = 0; i < A.val.len; i++)
		{
			//按Triplet方式填充，速度快
			lhs[i].row = A.val.pData[i].row;
			lhs[i].col = A.val.pData[i].col;
			lhs[i].val = A.val.pData[i].val * 2.0;
		}
		
		
		rhs.resize(B.val.len);
		
		for (unsigned i = 0; i < B.val.len; i++)
		{
			rhs[i].row = B.val.pData[i].row;
			rhs[i].col = B.val.pData[i].col;
			rhs[i].val = B.val.pData[i].val * (-1.0);
		}
		VSSprsMatrixD sprsMatLHS;
		VSSprsMatrixD sprsMatRHS;
		std::vector<double> x1_sparse;

		sprsMatLHS.row = A.row;
		sprsMatLHS.col = A.col;
		sprsMatLHS.val = VD_V2CB(lhs);
		sprsMatRHS.row = B.row;
		sprsMatRHS.col = B.col;
		sprsMatRHS.val = VD_V2CB(rhs);
#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/matlhs%d.txt", nCountCall);
			std::ofstream txtL(szFileName);
			sprintf_s(szFileName, 256, "D:/testdata/ld/matrhs%d.txt", nCountCall);
			std::ofstream txtR(szFileName);
			for (size_t i = 0; i <lhs.size(); i++)
			{
				txtL << lhs[i].row << " " << lhs[i].col << " " << lhs[i].val << std::endl;
			}
			txtL.close();
			for (size_t i = 0; i < rhs.size(); i++)
			{
				txtR << rhs[i].row << " " << rhs[i].col << " " << rhs[i].val << std::endl;
			}
			txtR.close();
		}
#endif


		VD_PROFILE_START(MAT_SolveSquareMat);

		NMALG_MATRIX::MAT_SolveSquareMat(x1_sparse, sprsMatLHS, sprsMatRHS);

		VD_PROFILE_FINISH(MAT_SolveSquareMat);
		//X = temp(1:size(A, 1));
#ifdef DEBUG_MATRIX
		{
			sprintf_s(szFileName, 256, "D:/testdata/ld/XMatrix_%d.txt", nCountCall);
			std::ofstream txtX(szFileName);
			txtX << x1_sparse.size() << " " << 1 << " 0" << std::endl;
			for (size_t i = 0; i < x1_sparse.size(); i++)
			{
				txtX << i << " " << 0 << " " << std::fixed << std::setprecision(18) << x1_sparse[i] << std::endl;
			}
			txtX.close();
		}
#endif
		X.resize(A.row);
		for (size_t i = 0; i < A.row; i++)
		{
			X[i].row = i;
			X[i].col = 0;
			X[i].val = x1_sparse[i + 0 * A.row];
		}

		//lambda = zeros(size(Aeq, 1), 1);
		//[r, c] = find(E(:, 1 : n));
		//lambda(r) = temp(size(A, 1) + 1:end);
		vLambda.clear();
	}
}
