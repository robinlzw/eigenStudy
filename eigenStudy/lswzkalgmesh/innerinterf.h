#pragma once

#include <ialgmesh/ialgmesh.h>


// ������������ϵĽڵ�
//{
//    unsigned   verNum ;     
//    unsigned  *pEdge  ;      //���ߵ����ڵı��б�
//    float     *pAlpha ; 
//};

// ����������Ϣ
//struct VertEigenInfo
//{
//    float         eigValue[3];
//    VFVECTOR3     eigVec[3]; 
//};

struct VSMinCurvPathFinderCntrl
{

} ;

//struct VSMinCurvPathFinderResult 
//{
//    float score ;
//} ;


//struct VSMinCurvPathFinderSource : VSGraphMesh
//{ 
//    const float         * lpVertDist   ; // ��ԭʼ��Ե�ľ���
//    const VertEigenInfo * lpVertEigen  ; // ��������
//} ;

//struct VSMeshVertexScore : VSConstBuffer< float > 
//{
//    VSMeshVertexScore(){}
//    VSMeshVertexScore( const VSConstBuffer< float > & cb ): VSConstBuffer< float >( cb ){}
//} ;

//typedef VSConstBuffer< float > VSMeshVertexScore ;
//struct VSMeshVertexScore
//{ 
//    VSConstBuffer< float > data ; 
//    VSMeshVertexScore(){}
//    VSMeshVertexScore( const VSConstBuffer< float > & cb ): data( cb ){}
//} ;
