#pragma once

#include "VGraphAndHeap.h"
#include <vmath\vfmesh.h>
#include <algorithm>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//shortest path infomation

//struct PathInfo
//{
//	float   pathLen;
//	int     pathVert;
//	int     pathPreVert;
//};

class VBDijkstra
{
public:
	VBDijkstra() {}
	~VBDijkstra() {}    

	void Build(VSConstBuffer<VSPathInfo> & pathOut, const VSGraphMesh & gm
              , const VNWZKALG::VRDijkstraTerminal & term )
	{
        unsigned begVertIdx = term.bgnIndx ;
        unsigned endVertIdx = term.endIndx ;

        FindPath(gm, begVertIdx, endVertIdx);
		std::reverse(m_vSinglePath.begin(), m_vSinglePath.end());

		pathOut.len   = m_vSinglePath.size();
		pathOut.pData = &m_vSinglePath[0];
	}

private:
	static float maxDis;

	void FindNbrVert(const VSGraphMesh & gm, const unsigned & vidx);
	void DijkstraPath(const VSGraphMesh & gm, const unsigned & begVertIdx, const unsigned & endVertIdx);
	void FindPath(const VSGraphMesh & gm, const unsigned & begVertIdx, const unsigned & endVertIdx);

private:
	std::vector< VSPathInfo >     m_vSinglePath;
	std::vector< VSPathInfo >     m_vPathInfo;
	std::vector< DijkElem >       m_heap;
	std::vector< unsigned >       m_vNbrVert;
	std::vector< bool     >       m_vVertFlag;   //0: unvisited  1: visited
	std::vector< float    >       m_vVertWt;
};

