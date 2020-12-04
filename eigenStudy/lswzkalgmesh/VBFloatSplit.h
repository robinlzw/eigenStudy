#pragma once

#include<vmath/vfmesh.h>
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//32位浮点数的分割

//template<class T>
//struct FloatSplit
//{
//	VSConstBuffer<char>     flag;         //true: vReal, false: vPred
//	VSConstBuffer<T>      SignExpo;
//	VSConstBuffer<T>      MiddleMantissa;
//	VSConstBuffer<T>      EndMantissa;
//};

class VBFloatSplit
{
public:
	VBFloatSplit():dCount(0), rCount(0) {}
	~VBFloatSplit() {}

public:
	/*void Build(FloatSplit & fs , const VSConstBuffer<VFVECTOR3> & vf)
	{
		SplitProcess(vf);

		fs.SignExpo = VD_V2CB(m_vSignExponent);
		fs.MiddleMantissa = VD_V2CB(m_vEndMantissa);
		fs.EndMantissa = VD_V2CB(m_vEndMantissa);
	}*/
	void Build(VSFloatSplit<int> & fs, const VSConstBuffer<VFVECTOR3> & vPred, const VSConstBuffer<VFVECTOR3> & vReal)
	{
		SplitProcess1(vPred, vReal);

		fs.flag           = VD_V2CB(m_vVertFlag);
		fs.SignExpo       = VD_V2CB(m_vSignExponent);
		fs.MiddleMantissa = VD_V2CB(m_vMiddleMantissa);
		fs.EndMantissa    = VD_V2CB(m_vEndMantissa);
	}

private:
	void SplitProcess(const VSConstBuffer<VFVECTOR3> & vf);

	void SplitProcess1(const VSConstBuffer<VFVECTOR3> & vPred, const VSConstBuffer<VFVECTOR3> & vReal);
	void Process(float pred, float real, unsigned pos);

private:	
	std::vector<int>       m_vSignExponent;      //9
	std::vector<int>       m_vMiddleMantissa;	 //12
	std::vector<int>       m_vEndMantissa;		 //11	
	std::vector<char>      m_vVertFlag;          //true: vReal, false: vPred

	unsigned dCount;
	unsigned rCount;
	//std::vector<int>       m_vFloatSplit;   //9 - 12 - 11
};