#pragma once
#include "lsalgteethprocess.h"
//////////////////////////////////////////////////////////////////////////
#define BTM_CTRL_RADIUS 0.5f

enum E_CTRLPOINT_T
{
	E_CTRLPOINT = 1,
	E_CTRLPOINT_CENTER,
};
void PatchToothBottom(unsigned& nVertCount, std::vector<VNVECTOR3UI>& vAddrSurfs, std::vector<unsigned>& vCtrlPoints,
	const VSConstBuffer<VFVECTOR3>& cbBndry);

void ExtendToothBottomCtrlPoints(std::vector<VFVECTOR3>& outVerts,
	NMALG_TEETHPATCHING::VSToothAxis& toothAxis,
	const VSConstBuffer<unsigned>& cbCtrlPoints, 
	const VSConstBuffer<unsigned>& cbGumLine, const VSPerfectMesh& pm);
