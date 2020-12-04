#pragma once

#include <obj/ialgmesh/ialgmesh.h>
#include <sys/alg/isysmesh.h>

/****************/
//输入为 K-D tree和一个点，
//输出为K-D tree中距离该点最近的点及两点之间的距离
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

