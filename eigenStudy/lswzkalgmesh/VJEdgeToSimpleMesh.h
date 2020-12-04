#pragma once

#include<vmath/vfmesh.h>
#include "innerinterf.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>

class VJEdgeToSimpleMesh
{
public:
	VJEdgeToSimpleMesh() {}
	~VJEdgeToSimpleMesh() {}

public:
	void Build(VNWZKALG::VSCurveSplitedGraph & sg , const VSPerfTopoGraph & mesh, const VSConstBuffer<bool> & eflag)
	{
		m_vNewVertOnEdge.clear();
		m_vSurfTriangle.clear();

		unsigned oldVertCount = mesh.GetVertexCount();
		SplitTriToSimpMesh(mesh, eflag);

		sg.freshVertMap.len     = m_vNewVertOnEdge.size()                   ;
		sg.freshVertMap.pData   = &m_vNewVertOnEdge[0]                      ;
		sg.graph.nTriangleCount = m_vSurfTriangle.size()                    ;
		sg.graph.nVertCount     = oldVertCount + m_vNewVertOnEdge.size()    ;
		sg.graph.pTriangles     = &m_vSurfTriangle[0]                       ;
	}

private:
	void AddOrgTri(const VSPerfTopoGraph & mesh, const VSConstBuffer<bool> & eflag);            //eflag 0£∫≤ª∑÷¡— 1£∫∑÷¡—
	void SplitTriToSimpMesh(const VSPerfTopoGraph & mesh, const VSConstBuffer<bool> & eflag);

private:
	std::vector<VNVECTOR3UI>    m_vSurfTriangle;
	std::vector<unsigned>       m_vNewVertOnEdge;	
};

 