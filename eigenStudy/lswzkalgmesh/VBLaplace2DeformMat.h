#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj\ialgeq\ialgeq.h>

//二阶拉普拉斯矩阵

//#include<iostream>
//#include<fstream>

class VBLaplace2DeformMat
{
public:
	VBLaplace2DeformMat() {}
	~VBLaplace2DeformMat() {}

	void Build(VSSprsMatrixColMajor< double > & modifiedSecLap, const VSSprsMatrixColMajor< double > & secondLap, const VSConstBuffer< char > & destVert)
	{
	    m_vOuterStarts.clear() ;
	    m_vInnerIndices.clear() ;
	    m_vValues.clear() ; 
	    m_vOrgDiagElem.clear() ;

		Modify(secondLap, destVert);

		modifiedSecLap.row = secondLap.row;
		modifiedSecLap.col = secondLap.col;
		modifiedSecLap.cbOuterIndice.len = m_vOuterStarts.size();
		modifiedSecLap.cbOuterIndice.pData = reinterpret_cast<unsigned*>(&m_vOuterStarts[0]);
		modifiedSecLap.cbIndice.len = m_vInnerIndices.size();
		modifiedSecLap.cbIndice.pData = reinterpret_cast<unsigned*>(&m_vInnerIndices[0]);
		modifiedSecLap.pValues = &m_vValues[0];
	}

private:
	void Modify(const VSSprsMatrixColMajor< double > & secondLap, const VSConstBuffer< char > & destVert);

private:
	std::vector<unsigned>    m_vOuterStarts;
	std::vector<unsigned>    m_vInnerIndices;
	std::vector<double>       m_vValues;

	std::vector<double>       m_vOrgDiagElem;
};
