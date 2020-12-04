#pragma once

//拉普拉斯网格编辑

#include<vmath/vfmesh.h>
#include<vmath/vtopoutil.h>
//#include"../lswzkalgeq/VJLaplaceDeform.h"
#include"../lswzkalgeq/VBEquationResolver.h"
#include"VBLaplace2DeformNewRight.h"
#include <sys/alg/isysmesh.h>
#include <sys\alg\isysequation.h>

class VBLaplaceMeshEdit
{
public:
	VBLaplaceMeshEdit(const VSESS< VNALGEQ::VRUniformLaplaceMatOfMeshF > & ulaplace): m_objLaplace(ulaplace) {}
	~VBLaplaceMeshEdit() {}

	void Build(VSConstBuffer<VFVECTOR3> & verts, const VSGraphMesh & gm, const VSConstBuffer< unsigned, VFVECTOR3 > & destVert)
	{		
		EditVertPos(gm, destVert);

		verts.len = VD_V2CB(m_vSolve).len;
		verts.pData = VD_V2CB(m_vSolve).pData;		
	}

private:
	void      CalSurfNormal(const VSGraphMesh & gm);
	void      CalVertNormal(const VSGraphMesh & gm);
	void      FindOrthoEdge(const VSGraphMesh & gm);
	void      OrthoEdgeProject(const VSGraphMesh & gm);
	void      CalRotateParam(const VSGraphMesh & gm);	
	void      CreatSprsmat(const VSGraphMesh & gm, const VSConstBuffer< unsigned, VFVECTOR3 > & destVert);
	void      CoordTransform(const VSGraphMesh & gm);
	void      CalDifferCord(const VSGraphMesh & gm);
	void      EditVertPos( const VSGraphMesh & gm, const VSConstBuffer< unsigned, VFVECTOR3 > & destVert);
	
private:
	void _copy(std::vector< VFVECTOR3 > & arr, VSDnsMatData< float, true > & solv)
	{
		unsigned vc = solv.row;
		unsigned dvc = 2 * vc;

		arr.resize(vc);

		for (unsigned i = 0; i < vc; i++)
		{
			arr[i].x = solv.pData[i];
			arr[i].y = solv.pData[i + vc];
			arr[i].z = solv.pData[i + dvc];
		}
	}

private:
	//VBUniformLaplaceMatOfMesh<float>   Laplace;
	TVR2B< VNALGEQ::VRUniformLaplaceMatOfMeshF >    m_objLaplace;

	VSSprsMatrixColMajor< float >      matOut;
	VSDnsMatData< float, false >       mB;
	VBEquationResolver<float, false>   solve;
	VSDnsMatData< float, true >        X;
	VTopoGraphUtil                     tgl;
	VSConstBuffer<char>                flag;

private:
	std::vector<unsigned>    m_vOrthoEdge;     //每一个点最接近正交的边,实际存储的为点的索引
	std::vector<VFVECTOR3>   m_vOrtho;
	std::vector<VFVECTOR3>   m_vRotateParam;   //每一个点的旋转参数
	std::vector<VFVECTOR3>   m_vLaplaceVerts;  //变换后的局部坐标
	std::vector<VFVECTOR3>   m_vVertNormal;
	std::vector<VFVECTOR3>   m_vSurfNormal;
	std::vector<VFVECTOR3>   m_vSolve;         //变形后的位置
	std::vector<char>        m_vFlag;
};