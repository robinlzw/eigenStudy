#pragma once
#include <vmath\vfmesh.h>
#include "HEMesh.h"
#include "HEMeshConstruct.h"
#include <sys/alg/isysmesh.h>

//非流行网格检测

class VBNonManifoldCheck
{
public:
	VBNonManifoldCheck() {}
	~VBNonManifoldCheck() {}

	void Build(VNWZKALG::VSNonmanifoldCheckInfo & info, const VSSimpleMeshF & smesh)
	{
		Check(smesh);

		info = checkRes;
	}

private:
	void  Check(const VSSimpleMeshF & smesh);
	void  VertexFaceClassify(const VSSimpleMeshF & smesh);
	void  SearchFace(const VSSimpleMeshF & smesh);
	void  FindNonManifoldSurf(const VSSimpleMeshF & smesh, const std::pair<unsigned, unsigned> & ve);

private:
	HEMesh    mesh;
	VNWZKALG::VSNonmanifoldCheckInfo         checkRes;

	std::vector<unsigned>                   m_vEdgeFace;
	std::vector<VSConstBuffer<unsigned>>    m_vVertFace1;
	std::vector<VSConstBuffer<unsigned>>    m_vVertFace2;

	std::vector<std::vector<unsigned>>      m_vNonManifoldFace1;
	std::vector<std::vector<unsigned>>      m_vNonManifoldFace2;
	std::vector<unsigned>                   m_vNonManifoldVert;
	std::map<int, std::vector<unsigned>>    m_VertAllFace;
	std::vector<bool>                       m_vVFlag;
	std::pair<unsigned, unsigned>           m_nonManifoldEdge;
};


class VBNonManifoldCheck2
{
public:
	VBNonManifoldCheck2() {}
	~VBNonManifoldCheck2() {}

	void Build(VNWZKALG::VSNonmanifoldCheckInfo & info, const VSSimpleGraph & smesh)
	{
		VSSimpleMeshF   mesh;
		CreateSimpleMesh(mesh, smesh);
		m_nonCheck.Build(info, mesh);
	}

private:
	void CreateSimpleMesh(VSSimpleMeshF & mesh, const VSSimpleGraph & smesh)
	{
		VFVECTOR3 val = VFVECTOR3::ZERO;
		m_vVert.resize(smesh.nVertCount, val);

		mesh.nTriangleCount = smesh.nTriangleCount;
		mesh.nVertCount     = smesh.nVertCount;
		mesh.pTriangles     = smesh.pTriangles;
		mesh.pVertices      = &m_vVert.front();
	}

private:
	VBNonManifoldCheck         m_nonCheck;
	std::vector<VFVECTOR3>     m_vVert;
};