#pragma once

#include <ialgmesh/ialgmesh.h>


// 网格表面曲线上的节点
//{
//    unsigned   verNum ;     
//    unsigned  *pEdge  ;      //曲线点所在的边列表
//    float     *pAlpha ; 
//};

// 顶点曲率信息
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
//    const float         * lpVertDist   ; // 距原始边缘的距离
//    const VertEigenInfo * lpVertEigen  ; // 顶点曲率
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
