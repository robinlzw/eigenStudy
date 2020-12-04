#pragma once
#include <queue>
#include <vmath/vfmesh.h>
#include "HEMesh.h"
#include "HEMeshConstruct.h"
#include <sys/alg/isysmesh.h>

//·­×ªÈý½ÇÆ¬¼ì²â

class VBReversSurfCheck
{
public:
	VBReversSurfCheck() {}
	~VBReversSurfCheck() {}

	void Build(VSConstBuffer<unsigned> & rt, const VSSimpleMeshF & smesh)
	{
		Check(smesh);

		if (m_vNormalTri.size() > m_vReverseTri.size())
			rt = VD_V2CB(m_vReverseTri);
		else
			rt = VD_V2CB(m_vNormalTri);		
	}

private:
	void Init(const VSSimpleMeshF & smesh);
	void Check(const VSSimpleMeshF & smesh);

private:
	HEMesh    mesh;
	std::vector<unsigned>      m_vReverseTri;
	std::vector<unsigned>      m_vNormalTri;

	std::vector<int>           m_vTriClassify;    //1: normal, 0: reverse
	std::vector<bool>          m_vTriFlag;
	std::queue<FaceIter>       m_hQueue;
	std::vector<bool>          m_vPushFlag;
};
