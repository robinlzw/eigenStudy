#pragma once

#include<vmath/vfmesh.h>
#include "VBMeshToOVTable.h"
#include "VBDeltaProcess.h"

#include <sys/alg/isysmesh.h>
//#include <../obj/alg/ialgwzk.h>

//#include<iostream>
//#include<fstream>

//网格压缩算法

struct CLERS
{
	VSConstBuffer<char>         clers;
	VSConstBuffer<unsigned>     VertMapID;
	VSConstBuffer<unsigned>     TriMapID;
	VSConstBuffer<VFVECTOR3>    VertDelta;
	VSConstBuffer<VFVECTOR3>    VertReal;
	VSConstBuffer<VFVECTOR3>    VertPred;
	VSConstBuffer<VNVECTOR3UI>  PredRelation;    //pred = x + y - z
	VSConstBuffer<DLE3>         VertDeltaD;   //double
};

class VBEDCompress
{
public:
	VBEDCompress() :sCount(0), vCount(0), count(1), clersNum(0){}
	~VBEDCompress() {}

	void Build(CLERS & cmp, const OVTable & ov, const unsigned & startCorner)
	{
		MemberInit();
		CompressProc(ov, startCorner);

		m_vRealCoord.resize(ov.VertCoord.len);
		float t = 0.f;
		VFVECTOR3  temp(t, t, t);
		for (unsigned i = 0; i < ov.VertCoord.len; i++)
		{
			m_vPredCoord[i] += temp;
			m_vRealCoord[i] = ov.VertCoord.pData[i] + temp;
		}

		cmp.clers        = VD_V2CB(m_vClers);
		cmp.VertMapID    = VD_V2CB(m_vVertMapID);
		cmp.TriMapID     = VD_V2CB(m_vSurfMapID);
		cmp.VertDelta    = VD_V2CB(m_vDelta);
		cmp.VertPred     = VD_V2CB(m_vPredCoord);
		//cmp.VertReal   = ov.VertCoord;
		cmp.VertReal     = VD_V2CB(m_vRealCoord);
		cmp.PredRelation = VD_V2CB(m_vPredRelation);
	}

	unsigned* GetSurfMapID() { return &m_vSurfMapID.front(); }
private:
	void  MemberInit();
	void  CompressProc(const OVTable & ov, const unsigned startCorner);
	void  InitCompress(unsigned startCorner, const OVTable & ov);
	void  Compress(unsigned cid, const OVTable & ov);
	void  EncodeWithPrediction(int c, const OVTable & ov);
	void  EncodeDelta(int c, const OVTable & ov);

	//EB Helper Functions
	inline unsigned NextCorner(unsigned c);
	inline unsigned PrevCorner(unsigned c);
	inline unsigned RightTri(unsigned c, const OVTable & ov);
	inline unsigned LeftTri(unsigned c, const OVTable & ov);
	inline unsigned CornerToTri(unsigned c);

private:
	std::vector<char>          m_vClers;
	std::vector<VFVECTOR3>     m_vDelta;
	std::vector<unsigned>      m_vVertMapID;      //压缩后的点对应原始点的ID

	std::vector<VFVECTOR3>     m_vVertEstimate;	
	std::vector<bool>          m_vVertFlag;
	std::vector<unsigned>      m_vTriFlag;

	std::vector<VFVECTOR3>     m_vPredCoord;
	std::vector<VFVECTOR3>     m_vRealCoord;
	std::vector<VNVECTOR3UI>   m_vPredRelation;     //pred = x + y - z
	std::vector<unsigned>      m_vSurfMapID;       //压缩后的三角片索引对应的原始三角片的索引，角标为新三角片索引
private:
	unsigned  sCount;
	unsigned  vCount;
	unsigned  count, clersNum;
};