#pragma once
#include "VBMeshWithHole2OV.h"
#include "VBDeltaProcess.h"
#include <map>

//带环柄的网格压缩

struct HandleCmpRes
{
	unsigned                    bndryLen;
	unsigned                    delVertID;    //解压后需要删除的顶点ID(当网格被修复后）
	VSConstBuffer<char>         cmpRes;
	VSConstBuffer<VNVECTOR2UI>  handles;
	VSConstBuffer<VFVECTOR3>    realCoord;
	VSConstBuffer<VFVECTOR3>    delta;
	VSConstBuffer<unsigned>     vMapID;      //无边界时解压点与原始点对应关系及有边界时边界映射关系
	VSConstBuffer<unsigned>     triMapID;
	VSConstBuffer<VFVECTOR3>    bndryVt;
	VSConstBuffer<DLE3>         deltaD;
	VSConstBuffer<VNVECTOR3UI>  decRelation;
	VSConstBuffer<unsigned>     decBndry;     
	VSConstBuffer<unsigned>     vDecMapID;    //有边界时解压点与原始点对应关系
	VSConstBuffer<unsigned>     vOld2New;     //原始点与解压点对应关系
	VSConstBuffer<unsigned>     triOld2New;   //原始三角形与解压三角形对应关系
};

class VBEDCmpWithHandles
{
public:
	VBEDCmpWithHandles(): triNum(0), vCount(0){}
	~VBEDCmpWithHandles() {}

	void Build(HandleCmpRes & clers, const OVTable & ov, const VNWZKALG::VSHoleBoundary & Bndry, const unsigned & startCorner)
	{
		InitCompress(ov, Bndry, startCorner);

		//std::ofstream fp("D:\\v.txt");		
		//for (unsigned i = 0; i < m_vVert.size(); i++)
		//{
		//	fp << "v " << m_vVert[i].x << " " << m_vVert[i].y << " " << m_vVert[i].z << std::endl;
		//}
		//fp.close();

		clers.bndryLen    = bndryNum;
		clers.delVertID   = VD_INVALID_INDEX;
		clers.cmpRes      = VD_V2CB(m_vClers);
		clers.handles     = VD_V2CB(m_vHandle);
		clers.realCoord   = ov.VertCoord;
		clers.delta       = VD_V2CB(m_vPredictionVert);		
		clers.triMapID    = VD_V2CB(m_vTriMap);
		clers.bndryVt     = VD_V2CB(m_vBndryVt);
		clers.deltaD      = VD_V2CB(m_vDeltaD);
		clers.vMapID      = VD_V2CB(m_vVtMap);
		clers.vDecMapID   = VD_V2CB(m_vVtMapWithBndry);
		clers.decBndry    = VD_V2CB(m_vDecBndryVtID);
		clers.decRelation = VD_V2CB(m_vRelation);
		clers.vOld2New    = VD_V2CB(m_vOldV2NewV);
		clers.triOld2New  = VD_V2CB(m_vOldTri2NewTri);
	}

private:
	unsigned    StartCorner(const OVTable & ov, unsigned startVid);
	void        InitCompress(const OVTable & ov, const VNWZKALG::VSHoleBoundary & Bndry, const unsigned & startCorner);
	void        Compress(const OVTable & ov, unsigned corner);
	void        CheckHandle(unsigned curCorner, const OVTable & ov);
	void        BndryVertPrediction(const VNWZKALG::VSHoleBoundary & Bndry, const VSConstBuffer<VFVECTOR3> & vt, const OVTable & ov);
	void        EncodeWithPrediction(unsigned curCorner, const OVTable & ov);
	void        RelationMap();

	//help function
	inline unsigned NextCorner(unsigned c);
	inline unsigned PrevCorner(unsigned c);
	inline unsigned RightTriCorner(unsigned c, const OVTable & ov);
	inline unsigned LeftTriCorner(unsigned c, const OVTable & ov);
	inline unsigned CornerToTri(unsigned c);
	
private:
	std::vector<char>              m_vClers;
	std::vector<VNVECTOR2UI>       m_vHandle;   //x:S三角形的Corner, y: 当前三角形的Corner
	std::vector<VFVECTOR3>         m_vPredictionVert;
	std::vector<VFVECTOR3>         m_vVert;     //角标对应解码时的顶点ID
	std::vector<unsigned>          m_vVtMap;    //优先存储边界的对应关系，与m_vVert顺序不完全一致，主要差别在边界部分. 无边界时存储的为解压点与原始点的对应关系
	std::vector<unsigned>          m_vTriMap;   //角标对应解压三角片，里面存的为对应的原始三角片索引
	std::vector<VFVECTOR3>         m_vBndryVt;
	std::vector<DLE3>              m_vDeltaD;
	std::vector<VNVECTOR3UI>       m_vRelation;
	std::map<unsigned, unsigned>   m_vOldMapNewVt;   //key:解压顶点ID，value:对应原始点ID
	std::vector<unsigned>          m_vVtMapWithBndry;  //存在边界时，解压点与原始点的实际对应关系
	std::vector<unsigned>          m_vDecBndryVtID;      //解压后的边界点ID，顺时针方向

	std::vector<unsigned>          m_vOldV2NewV;     //角标对应原始点，里面存的为对应的新点坐标
	std::vector<unsigned>          m_vOldTri2NewTri; //角标为原始三角片，里面存的为对应的新的三角片索引
	
	std::stack<unsigned>           m_sBranch; 
	std::vector<unsigned>          m_vTriFlag;
	std::vector<bool>              m_vVertFlag;
	std::vector<VNVECTOR3UI>       m_vRelateTemp;

	unsigned      triNum;
	unsigned      vCount;
	unsigned      bndryNum;
	unsigned      mapVCount;
	VNVECTOR3UI   relation;
	unsigned      decBndryTemp;
};