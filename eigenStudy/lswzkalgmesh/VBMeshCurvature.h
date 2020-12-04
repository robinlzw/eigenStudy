#pragma once

#include <vmath/vtopoutil.h>
#include <vmath/vfmatrix3.h>
//#include "innerinterf.h"
#include <sys/alg/isysmesh.h>
#include <obj\alg\ialgwzk.h>

/*****************************/
//����Ϊһ���������Ϊ�����ϵĵ������ֵ������������
//�洢��m_vEigVert�����У�ÿ�������С����Ϊ�ڶ���
//����ֵ���������Ϊ����������ֵֵ����������������ֵһһ��Ӧ
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
    std::vector<VNWZKALG::VSVertEigenInfo>     m_vEigVert   ;   //����ֵ���մ�С�����˳�����У���������������ֵ��Ӧ
    std::vector<  VFMatrix3  >                 m_vSmoothMat ;
    std::vector<  VFMatrix3  >                 m_vTempMat   ;
};
