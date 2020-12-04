#pragma once

#include <vmath/vtopoutil.h>
#include <vmath/vfmatrix3.h>
//#include "innerinterf.h"
#include <sys/alg/isysmesh.h>
#include <obj\alg\ialgwzk.h>

/*****************************/
//输入为一个网格，输出为网格上的点的特征值和特征向量，
//存储在m_vEigVert数组中，每个点的最小曲率为第二个
//特征值，最大曲率为第三个特征值值，特征向量与特征值一一对应
/*****************************/

class VBMeshCurvature
{
public:
    VBMeshCurvature(){} 
    ~VBMeshCurvature(void) {} 

public :
    void Build( VSConstBuffer< VNWZKALG::VSVertEigenInfo > & ei , const VSGraphMesh & mesh )
    {
        CalEigen( mesh ) ;
        ei = VD_V2CB( m_vEigVert ) ;
    }

private:   
	float     CalAverEdgeLen(const VSGraphMesh & mesh);
    float     SurfNormalAngle(const VSGraphMesh &mesh, const unsigned &eidx);
    VFMatrix3 CommonEdgeMat(const VSGraphMesh &mesh, unsigned &eidx);
    VFMatrix3 SingleVertMat(const VSGraphMesh &mesh, const unsigned &vidx);
    void      SmoothMat(const VSGraphMesh & mesh);
    void      CalEigen(const VSGraphMesh &mesh);
     
private:
    std::vector<VNWZKALG::VSVertEigenInfo>     m_vEigVert   ;   //特征值按照从小到大的顺序排列，特征向量与特征值对应
    std::vector<  VFMatrix3  >                 m_vSmoothMat ;
    std::vector<  VFMatrix3  >                 m_vTempMat   ;
};
