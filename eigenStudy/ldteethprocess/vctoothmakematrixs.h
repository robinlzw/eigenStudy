#pragma once
#include "lsalgteethprocess.h"

//////////////////////////////////////////////////////////////////////////
void MakeMatrixL(std::vector<VSTripletD >& L, const VSPerfectMesh& perfMesh);
void MakeMatrixM(std::vector<VSTripletD >& M, const VSPerfectMesh& perfMesh);
void MakeMatrixBndry(std::vector<VSTripletD >& bndryMat, const VSConstBuffer<VFVECTOR3>& cbBndryVerts,
	const VSConstBuffer<VNVECTOR3UI>& vSurfaces, const unsigned nVertCount);


void QuadProgFixed(std::vector<VSTripletD>& X, std::vector<VSTripletD>& vLambda,
	const VSSprsMatrixD& sprsMatHUidUid, const VSSprsMatrixD& sprsMatHKidUid,
	const VSSprsMatrixD& sprsMatAeqUidTmp, const VSSprsMatrixD& sprsMatAeqKidTmp);