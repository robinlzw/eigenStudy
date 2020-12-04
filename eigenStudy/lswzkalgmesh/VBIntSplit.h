#pragma once
#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>

//整形数的分割

class VBIntSplit
{
public:
	VBIntSplit() {}
	~VBIntSplit() {}

	void Build(VSConstBuffer<VNVECTOR3UI> & split, const VSConstBuffer<int> & val)
	{
		SplitProcess(val);

		split = VD_V2CB(m_vSplit);
	}

private:
	void  MemberInit();
	void  SplitProcess(const VSConstBuffer<int> & val);

private:
	std::vector<VNVECTOR2UI>      m_vFront;    //x: 20   y: 12
	std::vector<VNVECTOR3UI>      m_vSplit;
};

