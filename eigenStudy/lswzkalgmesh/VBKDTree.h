#pragma once

#include <vstd/mem.h>
#include <vmath/vfvector3.h>
#include <obj/ialgmesh/ialgmesh.h>
#include <sys/alg/isysmesh.h>

class VBKDTree
{
public: 
	void Build(VNALGMESH::VSKDTree & KDtreeOut, const VSConstBuffer<VFVector3> & cbf)
	{
		std::vector<unsigned>    m_Dataset;
		m_Dataset.resize(cbf.len);
		for (unsigned i = 0; i < cbf.len; i++)
		{
			m_Dataset[i] = i;
		}
		unsigned p = 0;
		Init(cbf);
		CreatKDTree(m_Dataset, cbf, p);

		KDtreeOut = VD_V2CB(m_Root);
	}
    
private:
    void       Init(const VSConstBuffer<VFVector3> & cbf);
    unsigned   CreatKDTree(std::vector<unsigned> & ds , const VSConstBuffer<VFVector3> & cbf, unsigned & count);

private:
    std::vector<VNALGMESH::VSKDTreeNode>    m_Root;
};

