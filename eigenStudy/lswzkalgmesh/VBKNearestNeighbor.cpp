#include "stdafx.h"
#include "VBKNearestNeighbor.h"

void VBKNearestNeighbor::Path(const VNALGMESH::VSKDTree & KDtree, const VSConstBuffer<VFVector3>& cbf, const VFVector3 & target, unsigned curPos)
{
	while (curPos != VD_INVALID_INDEX)
	{
		const VNALGMESH::VSKDTreeNode & node = KDtree.pData[curPos];
		unsigned s = node.split;

		search_path.push(&(KDtree.pData[curPos]));

		if (target[s] <= cbf.pData[node.idx][s])
			curPos = node._left_child;
		else
			curPos = node._right_child;
	}
}

std::pair<unsigned, float> VBKNearestNeighbor::FindNearest(const VNALGMESH::VSKDTree & KDtree, const VSConstBuffer<VFVector3> & cbf, const VFVector3 & target)
{
	float  min_dist;
	unsigned  nearest;

    VASSERT( KDtree.len > 0 ) ;
    unsigned nCur = 0 ; 
	Path(KDtree, cbf, target, nCur);

	nearest = search_path.top()->idx;
	min_dist = (cbf.pData[nearest] - target).SqrMagnitude();
	search_path.pop();

	while (!search_path.empty())
    {
        const VNALGMESH::VSKDTreeNode * pBack = search_path.top();
        unsigned s = pBack->split;		
		search_path.pop();
		if (min_dist > (target - cbf.pData[pBack->idx]).SqrMagnitude())
		{
			nearest = pBack->idx;
			min_dist = (target - cbf.pData[pBack->idx]).SqrMagnitude();
		}

		float dtemp = cbf.pData[pBack->idx][s] - target[s];
		if (dtemp * dtemp < min_dist)
		{			
			if (target[s] <= cbf.pData[pBack->idx][s])
				Path(KDtree, cbf, target, pBack->_right_child);
			else
				Path(KDtree, cbf, target, pBack->_left_child);	
		}		
    }

	std::pair<unsigned, float>  res(nearest, sqrtf(min_dist));

	return res;
}

VD_EXPORT_SYSTEM_SIMPLE(VBKNearestNeighbor, VNALGMESH::VRKNearestNeighbor);