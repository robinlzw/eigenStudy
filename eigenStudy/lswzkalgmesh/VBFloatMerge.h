#pragma once
#include<vmath/vfmesh.h>

//合并为浮点数

template <class T1, class T2>
class VBFloatMerge
{
public:
	VBFloatMerge() {}
	~VBFloatMerge() {}

	void Build(VSConstBuffer<T1> & mergeFloat, const VSConstBuffer<T2> & signExpo, const VSConstBuffer<T2> & middleMantissa, const VSConstBuffer<T2> & endMantissa, const unsigned midSize)
	{
		m_vMergeFloat.clear();
		MergeToFloat(signExpo, middleMantissa, endMantissa, midSize);

		mergeFloat = VD_V2CB(m_vMergeFloat);
	}

private:
	void MergeToFloat(const VSConstBuffer<T2> & signExpo, const VSConstBuffer<T2> & middleMantissa, const VSConstBuffer<T2> & endMantissa, const unsigned midSize);

private:
	std::vector<T1>     m_vMergeFloat;
};


template<class T1, class T2>
inline void VBFloatMerge<T1, T2>::MergeToFloat(const VSConstBuffer<T2>& signExpo, const VSConstBuffer<T2>& middleMantissa, const VSConstBuffer<T2>& endMantissa, const unsigned midSize)
{
	m_vMergeFloat.resize(signExpo.len);
	for (unsigned i = 0; i < signExpo.len; i++)
	{
		T2 signexp = signExpo.pData[i] << 23;
		T2 middleM = middleMantissa.pData[i] << midSize;
		T2 mergeNum = signexp & middleM & endMantissa.pData[i];

		m_vMergeFloat[i] = (T1&)mergeNum;
	}
}
