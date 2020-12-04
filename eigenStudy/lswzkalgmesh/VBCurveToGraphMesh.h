#pragma once

#include "innerinterf.h"
#include "VBCurveToMesh.h"
//#include "VJPerfGraph.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//struct VSCurveSplitedGraphMesh
//{
//    VSGraphMesh               graphMesh   ;
//    unsigned                  nGrow       ; // 相对原始网格，增加的顶点的个数，这些顶点都位于顶点数组的后端
//    VSConstBuffer< unsigned > curveVerts  ; // 原分割线中位于边上的点在新网格中变成一个顶点，这个数组保存这些点的索引，数组长度与原曲线相同
//} ;

struct VBCurveToGraphMesh
{
	VBCurveToGraphMesh(const VSESS< VNALGMESH::VRPerfGraph > & pm): m_m2g(pm) {}
	~VBCurveToGraphMesh() {}

    void Build( VSCurveSplitedGraphMesh & csm , const VSGraphMesh & mesh , const VSConstBuffer< VNALGMESH::VSPointOnEdge > & curve )
    { 
        VSCurveSplitedMesh  orgMesh  ;
        VSPerfTopoGraph     orgGraph ;

        m_c2m.Build( orgMesh , mesh , curve ) ;
        m_m2g.Build( orgGraph , orgMesh.mesh ) ;

        static_cast< VSPerfTopoGraph & >( csm.graphMesh ) = orgGraph ;
        csm.graphMesh.pVertices = orgMesh.mesh.pVertices ;
        csm.nGrow = orgMesh.nGrow ;

        unsigned nBase = mesh.GetVertexCount() ;
        m_vDividLine.resize( csm.nGrow ) ;
        for( unsigned i = 0 ; i < csm.nGrow ; i ++ )
        {
            m_vDividLine[i] = nBase + i ;
        }
        csm.curveVerts = VD_V2CB( m_vDividLine ) ;
    }

    VBCurveToMesh       m_c2m ;
    //VJPerfGraph         m_m2g ;
	TVR2B< VNALGMESH::VRPerfGraph >     m_m2g;
    std::vector< unsigned > m_vDividLine ;
} ;

