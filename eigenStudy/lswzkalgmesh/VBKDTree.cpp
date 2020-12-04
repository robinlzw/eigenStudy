#include "stdafx.h"

#include "VBKDTree.h"

void VBKDTree::Init(const VSConstBuffer<VFVector3> & cbf)
{
	//kdcount = 0;
    unsigned num = cbf.len;   
    m_Root.resize(num);

    for (unsigned i = 0; i < num; ++ i)
    {       
		m_Root[i].idx          = VD_INVALID_INDEX;
		m_Root[i].split        = VD_INVALID_INDEX;
        m_Root[i]._left_child  = VD_INVALID_INDEX;
        m_Root[i]._right_child = VD_INVALID_INDEX;
        m_Root[i]._parent      = VD_INVALID_INDEX;
    }   
}

unsigned VBKDTree::CreatKDTree(std::vector<unsigned> & ds, const VSConstBuffer<VFVector3> & cbf, unsigned & kdcount)
{
	unsigned _size = ds.size();

	if (_size == 0)
		return VD_INVALID_INDEX;
    else
    {
		// 计算每个维的方差，选出方差值最大的维
		float variance_max = -0.1f;
		int _split = -1;                    //无效值		
		for (unsigned i = 0; i < 3; ++i)
		{
			float mean = 0.f, sqr_mean = 0.f, variance = 0.f;
			for (unsigned j = 0; j < _size; ++j)
			{
				mean += cbf.pData[ds[j]][i];
				sqr_mean += cbf.pData[ds[j]][i] * cbf.pData[ds[j]][i];
			}
			mean = 1.f / static_cast<float>(_size) * mean;
			sqr_mean = 1.f / static_cast<float>(_size) * sqr_mean;

			variance = sqr_mean - mean * mean;
			if (variance > variance_max)
			{
				variance_max = variance;
				_split = i;
			}
		}

        std::sort(ds.begin(), ds.end(), [&_split, &cbf](unsigned a, unsigned b) {return cbf.pData[a][ _split ] < cbf.pData[b][ _split ];});
		unsigned SplitChoice = ds[_size / 2];

        std::vector<unsigned>  vLeft;
        std::vector<unsigned>  vRight;

        for (unsigned i = 0; i < _size; ++ i)
        {
            if (ds[i] != SplitChoice && cbf.pData[ds[i]][ _split ] <= cbf.pData[SplitChoice][ _split ])
                vLeft.push_back(ds[i]);
            else if (ds[i] != SplitChoice && cbf.pData[ds[i]][ _split ] > cbf.pData[SplitChoice][ _split ])
                vRight.push_back(ds[i]);
        }   

		unsigned curIndx = kdcount;		
		m_Root[kdcount].idx = SplitChoice;
		m_Root[kdcount].split = _split;		
		kdcount++;
		if (kdcount == cbf.len)
			return VD_INVALID_INDEX;
		m_Root[kdcount]._parent = curIndx;
		m_Root[curIndx]._left_child = CreatKDTree(vLeft, cbf, kdcount);
		if (kdcount == cbf.len)
			return VD_INVALID_INDEX;
		m_Root[kdcount]._parent = curIndx;
		m_Root[curIndx]._right_child = CreatKDTree(vRight, cbf, kdcount);
		return curIndx;
    }    
}
VD_EXPORT_SYSTEM_SIMPLE(VBKDTree, VNALGMESH::VRKDTree);