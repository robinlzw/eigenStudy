#include "stdafx.h"
#include "VCCalcLevelSet.h"
#include <obj/alg/ialgmatrix.h>

namespace NMALG_CUTPATHGEN
{	
	VCCalcLevelSet::VCCalcLevelSet()
	{

	}

	VCCalcLevelSet::~VCCalcLevelSet()
	{

	}

	void VCCalcLevelSet::Calc(VSConstBuffer<double>& cbLevelSet, const VSPerfectMesh& perfMesh,
		const VSConstBuffer<unsigned>& cbFrontPointSet, const VSConstBuffer<unsigned>& cbBgkPointSet)
	{
		double dbLanda = 2.0;
		std::vector<VSTripletD> matLaplace;
		VSSprsMatrixD sprsMatL;
		VSSprsMatrixD sprsMatMInv;
		VSSprsMatrixD sprsMatVert;
		unsigned nSurfCount = perfMesh.GetSurfCount();
		unsigned nEdgeCount = perfMesh.GetEdgeCount();
		unsigned nVertCount = perfMesh.GetVertexCount();
		std::vector<unsigned> vEdgeFlag(nEdgeCount, 0);
		std::vector<unsigned> vVertFlag(nVertCount, 0);
		VSTripletD trip;
		memset(&trip, 0, sizeof(VSTripletD));
		matLaplace.resize(nVertCount + nEdgeCount * 2, trip);

		for (size_t i = 0; i < cbFrontPointSet.len; i++)
		{
			vVertFlag[cbFrontPointSet.pData[i]] = 1;
		}
		for (size_t i = 0; i < cbBgkPointSet.len; i++)
		{
			vVertFlag[cbBgkPointSet.pData[i]] = 1;
		}

		auto CalcCotValue = [&perfMesh](const unsigned nIdx, const VNVECTOR3UI& surf) -> double {
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
			return dbCosVal / dbSinVal / 2.0;
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
				matLaplace[nV1].row = matLaplace[nV1].col = nV1;
				matLaplace[nV1].val -= dbCotVal;
				matLaplace[nV2].row = matLaplace[nV2].col = nV2;
				matLaplace[nV2].val -= dbCotVal;

				matLaplace[nVertCount + eos[i]].row = nV1;
				matLaplace[nVertCount + eos[i]].col = nV2;
				matLaplace[nVertCount + nEdgeCount + eos[i]].row = nV2;
				matLaplace[nVertCount + nEdgeCount + eos[i]].col = nV1;
				matLaplace[nVertCount + eos[i]].val += dbCotVal;
				matLaplace[nVertCount + nEdgeCount + eos[i]].val += dbCotVal;				
			}
		}

		sprsMatL.row = nVertCount;
		sprsMatL.col = nVertCount;
		sprsMatL.val = VD_V2CB(matLaplace);
	
		std::vector<double> vSurfArea(nSurfCount);
		std::vector<VSTriplet<double> > matMInv(nVertCount);		
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

			matMInv[i].row = matMInv[i].col = i;
			matMInv[i].val = (3.0/dbAreaVal );
		}

		sprsMatMInv.row = nVertCount;
		sprsMatMInv.col = nVertCount;
		sprsMatMInv.val = VD_V2CB(matMInv);
		
		NMALG_MATRIX::VSSparseMatrix sprsMInvXL;
		NMALG_MATRIX::MAT_AMultiB(sprsMInvXL, sprsMatMInv, sprsMatL);
		for (size_t i = 0; i < sprsMInvXL.vValue.size(); i++)
		{
			if (1 == vVertFlag[sprsMInvXL.vValue[i].row]) 
			{
				if (sprsMInvXL.vValue[i].row == sprsMInvXL.vValue[i].col)
					sprsMInvXL.vValue[i].val = 1.0;
				else
					sprsMInvXL.vValue[i].val = 0.0;
			}
		}
		VSSprsMatrixD sprsMatMInvXL;
		sprsMatMInvXL.row = sprsMatMInvXL.col = nVertCount;
		sprsMatMInvXL.val = VD_V2CB(sprsMInvXL.vValue);
		std::vector<VSTripletD> vertMatV0(nVertCount);

		for (size_t i = 0; i < nVertCount; i++)
		{
			for (size_t j = 0; j < 1; j++)
			{
				VSTripletD& trid = vertMatV0[i + j];
				trid.row = i;
				trid.col = j;
				trid.val = 0.0f;
			}
		}
		for (size_t i = 0; i < cbFrontPointSet.len; i++)
		{
			vertMatV0[cbFrontPointSet.pData[i]].val = 1.0f;
		}
		for (size_t i = 0; i < cbBgkPointSet.len; i++)
		{
			vertMatV0[cbBgkPointSet.pData[i]].val = -1.0f;
		}
		
		VSSprsMatrixD sprsMatV0;
		sprsMatV0.row = nVertCount;
		sprsMatV0.col = 1;
		sprsMatV0.val = VD_V2CB(vertMatV0);
		NMALG_MATRIX::VSSparseMatrix matX;

		NMALG_MATRIX::MAT_SolveSquareMat(m_vLevelSet, sprsMatMInvXL, sprsMatV0);
		cbLevelSet = VD_V2CB(m_vLevelSet);
	}
}
