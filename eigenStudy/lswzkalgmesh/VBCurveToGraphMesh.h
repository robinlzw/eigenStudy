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
//    unsigned                  nGrow       ; // ���ԭʼ�������ӵĶ���ĸ�������Щ���㶼λ�ڶ�������ĺ��
//    VSConstBuffer< unsigned > curveVerts  ; // ԭ�ָ�����λ�ڱ��ϵĵ����������б��һ�����㣬������鱣����Щ������������鳤����ԭ������ͬ
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

