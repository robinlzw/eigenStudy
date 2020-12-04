#pragma once

/************************************/
//输入为一个完整的网格和一条曲线，输出为一个新的完整网格和
//一个顶点列表，顶点列表前面为原始点，后面为新增点。

// 网格上一条曲线将网格部分三角片分割成多个三角片，最后形成一个新的网格
// 在原始网格中曲线是由位于边上的点顺序连接成的一个环
// 在新网格中，这些点变为顶点
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
    std::vector<VFVECTOR3>      m_vVert;         //新增点直接添加在原始点后面
    std::vector<VNVECTOR3UI>    m_vSurfVert;
    std::vector<unsigned>       m_vTemp;         //三角片的标记状态 0：添加  1：删除
} ;
