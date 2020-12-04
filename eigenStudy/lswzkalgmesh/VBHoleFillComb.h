#pragma once
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
#include "VBFindHole.h"

//补洞算法组合，在每个孔洞中心添加一个点

class VBHoleFillComb
{
public:
	VBHoleFillComb(const VSESS< VNALGMESH::VRPerfMesh > & alg1,
		           const VSESS< VNALGMESH::VRHoleFill > & alg3)
		          :m_PerfMesh(alg1), m_HoleFill(alg3) {}
	~VBHoleFillComb() {}

	void Build(VSSimpleMeshF & smsh, const VSSimpleMeshF & msh)
	{		
		/*std::ofstream fp("D:\\gm.obj");
		for (unsigned i = 0; i < msh.nVertCount; i++)
		{
			fp << "v " << msh.pVertices[i].x << "  " << msh.pVertices[i].y << "  " << msh.pVertices[i].z << std::endl;
		}
		fp << std::endl;
		for (unsigned i = 0; i < msh.nTriangleCount; i++)
		{
			fp << "f " << msh.pTriangles[i].x + 1 << "  " << msh.pTriangles[i].y + 1 << "  " << msh.pTriangles[i].z + 1 << std::endl;
		}
		fp.close();*/
		CombFun(msh);

		smsh.nTriangleCount = m_vSurfaces.size();
		smsh.nVertCount = m_vVertices.size();
		smsh.pTriangles = &m_vSurfaces.front();
		smsh.pVertices = &m_vVertices.front();
	}
private:
	void  CombFun(const VSSimpleMeshF & msh);
	VFVECTOR3  CenterVert(const VSSimpleMeshF& sm, const VSConstBuffer<unsigned> & bndryV);
	
	std::vector<VFVECTOR3>                    m_vVertices;
	std::vector<VNVECTOR3UI>                  m_vSurfaces;
		
	std::vector<VFVECTOR3>                    m_vAddVert;
	std::vector<VNVECTOR3UI>                  m_vAddTri;
	TVR2B< VNALGMESH::VRPerfMesh >            m_PerfMesh;
	TVR2B< VNALGMESH::VRHoleFill >            m_HoleFill;
	VBFindHole                                m_FindBndry;
};
