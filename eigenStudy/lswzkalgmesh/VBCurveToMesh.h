#pragma once

/************************************/
//����Ϊһ�������������һ�����ߣ����Ϊһ���µ����������
//һ�������б������б�ǰ��Ϊԭʼ�㣬����Ϊ�����㡣

// ������һ�����߽����񲿷�����Ƭ�ָ�ɶ������Ƭ������γ�һ���µ�����
// ��ԭʼ��������������λ�ڱ��ϵĵ�˳�����ӳɵ�һ����
// ���������У���Щ���Ϊ����
/************************************/
#include <vmath/vtopomesh.h>
//#include "VTopoMesh.h"
#include "innerinterf.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>

//class VCurveToMesh : public VTopoMesh
//{
//};

struct VBCurveToMesh
{
public:
	VBCurveToMesh() {}
    ~VBCurveToMesh() {}

public :
    void Build(VNWZKALG::VSCurveSplitedMesh & csm , const VSGraphMesh & mesh , const VSConstBuffer< VNALGMESH::VSPointOnEdge > & curve )
    {
        m_vVert.clear()     ;
        m_vSurfVert.clear() ;
        m_vTemp.clear()     ;

        UpdateTopo(mesh, curve ) ; 

		csm.mesh.nVertCount = m_vVert.size();
        csm.mesh.nTriangleCount = m_vSurfVert.size() ;
		csm.mesh.pTriangles = &m_vSurfVert[0];
		csm.mesh.pVertices = &m_vVert[0];
		csm.nGrow = curve.len;
		//csm.pCurveVerts = &m_vCurveVert[0];
    }
public :
    typedef VSConstBuffer< VNALGMESH::VSPointOnEdge > Curve ; 

public:
    void UpdateTopo(const VSGraphMesh & mesh, const Curve & cv);

private:
    void AddVertInfo(const VSGraphMesh & mesh, const Curve & cv);
    void DelSurf(const VSGraphMesh & mesh, const Curve & cv);
    void TopoSurf(const VSGraphMesh & mesh, const Curve & cv);
   
private:
    std::vector<VFVECTOR3>      m_vVert;         //������ֱ�������ԭʼ�����
    std::vector<VNVECTOR3UI>    m_vSurfVert;
    std::vector<unsigned>       m_vTemp;         //����Ƭ�ı��״̬ 0�����  1��ɾ��
} ;
