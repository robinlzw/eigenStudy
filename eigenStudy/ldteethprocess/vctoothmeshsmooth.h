#pragma once
#include "lsalgteethprocess.h"
//////////////////////////////////////////////////////////////////////////
void MakeMatrixGumLine(std::vector<VSTripletD>& matGumLine,
	const VSPerfectMesh& pm, const VSConstBuffer<unsigned>& cbGumLine);
void LaplaceMatAddGumLineMat(std::vector<VSTripletD>& laplaceMat,
	const VSConstBuffer<unsigned>& cbGumLine, const VSPerfectMesh& pm);

void AppendVertsTo01Matrix(std::vector<VSTripletD>& mat,const unsigned nRowShift,
	const VSConstBuffer<unsigned>& cbVertices, const double dbW = 1.0);
