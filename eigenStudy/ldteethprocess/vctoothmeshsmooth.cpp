#include "stdafx.h"
#include <pkg/ldck4cce.h>
#include <pkg/ldwzkupdater.h>
#include <obj/lsalgtools/ilsalgtools.h>
#include "vctoothmeshsmooth.h"

void MakeMatrixGumLine(std::vector<VSTripletD>& matGumLine,
	const VSPerfectMesh& pm, const VSConstBuffer<unsigned>& cbGumLine)
{
	matGumLine.resize(cbGumLine.len * 3);
	
	unsigned nIdx = 0;
	unsigned nVIdx = cbGumLine.pData[0];
	unsigned nPrev = cbGumLine.pData[cbGumLine.len - 1];
	unsigned nNext = cbGumLine.pData[1];
	
	matGumLine[nIdx].row = matGumLine[nIdx].col = nVIdx;
	matGumLine[nIdx++].val = 1.0;
	matGumLine[nIdx].row = nVIdx;
	matGumLine[nIdx].col = nPrev;
	matGumLine[nIdx++].val = -0.5;
	matGumLine[nIdx].row = nVIdx;
	matGumLine[nIdx].col = nNext;
	matGumLine[nIdx].val = -0.5;


	for (unsigned i = 1; i < cbGumLine.len - 1; i++)
	{
		nVIdx = cbGumLine.pData[i];
		nPrev = cbGumLine.pData[i - 1];
		nNext = cbGumLine.pData[i + 1];
		nIdx = i * 3;
		matGumLine[nIdx].row = matGumLine[nIdx].col = nVIdx;
		matGumLine[nIdx++].val = 1.0;
		matGumLine[nIdx].row = nVIdx;
		matGumLine[nIdx].col = nPrev;
		matGumLine[nIdx++].val = -0.5;
		matGumLine[nIdx].row = nVIdx;
		matGumLine[nIdx].col = nNext;
		matGumLine[nIdx].val = -0.5;
	}
	nVIdx = cbGumLine.pData[cbGumLine.len - 1];
	nPrev = cbGumLine.pData[cbGumLine.len - 2];
	nNext = cbGumLine.pData[0];
	nIdx = (cbGumLine.len - 1) * 3;
	matGumLine[nIdx].row = matGumLine[nIdx].col = nVIdx;
	matGumLine[nIdx++].val = 1.0;
	matGumLine[nIdx].row = nVIdx;
	matGumLine[nIdx].col = nPrev;
	matGumLine[nIdx++].val = -0.5;
	matGumLine[nIdx].row = nVIdx;
	matGumLine[nIdx].col = nNext;
	matGumLine[nIdx].val = -0.5;
}

void LaplaceMatAddGumLineMat(std::vector<VSTripletD>& laplaceMat,
	const VSConstBuffer<unsigned>& cbGumLine, const VSPerfectMesh& pm)
{
	unsigned nVertCount = pm.GetVertexCount();
	std::vector<unsigned> vVertBndryFlag(nVertCount, 0);
	for (size_t i = 0; i < cbGumLine.len; i++)
	{
		vVertBndryFlag[cbGumLine.pData[i]] = 1;
	}
	unsigned nIdx = 0;
	while (nIdx < nVertCount)
	{
		if (laplaceMat[nIdx].row == laplaceMat[nIdx].col)
		{
			if (1 == vVertBndryFlag[laplaceMat[nIdx].row])
			{
				unsigned nRow = laplaceMat[nIdx].row;
				laplaceMat[nIdx].val += 10.0;
				nIdx++;
				while (laplaceMat[nIdx].row == nRow)
				{
					if (1 == vVertBndryFlag[laplaceMat[nIdx].col])
					{
						laplaceMat[nIdx].val += -5.0;
					}
					nIdx++;
				}
			}
			else
				nIdx++;
		}
		else
			nIdx++;
	}
}


void AppendVertsTo01Matrix(std::vector<VSTripletD>& mat, const unsigned nRowShift,
	const VSConstBuffer<unsigned>& cbVertices, const double dbW)
{
	for (unsigned i = 0; i < cbVertices.len; i++)
	{
		unsigned nRow = nRowShift + i;
		VSTripletD trip;
		trip.row = nRow;
		trip.col = cbVertices.pData[i];
		trip.val = dbW;
		mat.push_back(trip);
	}
}