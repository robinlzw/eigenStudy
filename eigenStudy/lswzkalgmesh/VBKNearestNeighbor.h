#pragma once

#include <obj/ialgmesh/ialgmesh.h>
#include <sys/alg/isysmesh.h>

/****************/
//����Ϊ K-D tree��һ���㣬
//���ΪK-D tree�о���õ�����ĵ㼰����֮��ľ���
/***************/
class VBKNearestNeighbor
{
public:
    VBKNearestNeighbor() {}
    ~VBKNearestNeighbor() {}

	void Build(std::pair<unsigned, float> & vertOnMesh, const VNALGMESH::VSKDTree & KDtree, const VSConstBuffer<VFVector3> & cbf, const VFVector3 & target)
	{	
        VASSERT( search_path.empty() ) ;

		vertOnMesh = FindNearest(KDtree, cbf, target);
	}

public:	
    std::pair<unsigned, float> FindNearest(const VNALGMESH::VSKDTree & KDtree, const VSConstBuffer<VFVector3> & cbf, const VFVector3 & target);

private:
	void Path(const VNALGMESH::VSKDTree & KDtree, const VSConstBuffer<VFVector3> & cbf, const VFVector3 & target, unsigned curPos);

private:
	std::stack<const VNALGMESH::VSKDTreeNode *>  search_path;
};

