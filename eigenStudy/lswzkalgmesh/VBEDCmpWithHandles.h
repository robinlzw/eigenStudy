#pragma once
#include "VBMeshWithHole2OV.h"
#include "VBDeltaProcess.h"
#include <map>

//������������ѹ��

struct HandleCmpRes
{
	unsigned                    bndryLen;
	unsigned                    delVertID;    //��ѹ����Ҫɾ���Ķ���ID(�������޸���
	VSConstBuffer<char>         cmpRes;
	VSConstBuffer<VNVECTOR2UI>  handles;
	VSConstBuffer<VFVECTOR3>    realCoord;
	VSConstBuffer<VFVECTOR3>    delta;
	VSConstBuffer<unsigned>     vMapID;      //�ޱ߽�ʱ��ѹ����ԭʼ���Ӧ��ϵ���б߽�ʱ�߽�ӳ���ϵ
	VSConstBuffer<unsigned>     triMapID;
	VSConstBuffer<VFVECTOR3>    bndryVt;
	VSConstBuffer<DLE3>         deltaD;
	VSConstBuffer<VNVECTOR3UI>  decRelation;
	VSConstBuffer<unsigned>     decBndry;     
	VSConstBuffer<unsigned>     vDecMapID;    //�б߽�ʱ��ѹ����ԭʼ���Ӧ��ϵ
	VSConstBuffer<unsigned>     vOld2New;     //ԭʼ�����ѹ���Ӧ��ϵ
	VSConstBuffer<unsigned>     triOld2New;   //ԭʼ���������ѹ�����ζ�Ӧ��ϵ
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
	std::vector<VNVECTOR2UI>       m_vHandle;   //x:S�����ε�Corner, y: ��ǰ�����ε�Corner
	std::vector<VFVECTOR3>         m_vPredictionVert;
	std::vector<VFVECTOR3>         m_vVert;     //�Ǳ��Ӧ����ʱ�Ķ���ID
	std::vector<unsigned>          m_vVtMap;    //���ȴ洢�߽�Ķ�Ӧ��ϵ����m_vVert˳����ȫһ�£���Ҫ����ڱ߽粿��. �ޱ߽�ʱ�洢��Ϊ��ѹ����ԭʼ��Ķ�Ӧ��ϵ
	std::vector<unsigned>          m_vTriMap;   //�Ǳ��Ӧ��ѹ����Ƭ��������Ϊ��Ӧ��ԭʼ����Ƭ����
	std::vector<VFVECTOR3>         m_vBndryVt;
	std::vector<DLE3>              m_vDeltaD;
	std::vector<VNVECTOR3UI>       m_vRelation;
	std::map<unsigned, unsigned>   m_vOldMapNewVt;   //key:��ѹ����ID��value:��Ӧԭʼ��ID
	std::vector<unsigned>          m_vVtMapWithBndry;  //���ڱ߽�ʱ����ѹ����ԭʼ���ʵ�ʶ�Ӧ��ϵ
	std::vector<unsigned>          m_vDecBndryVtID;      //��ѹ��ı߽��ID��˳ʱ�뷽��

	std::vector<unsigned>          m_vOldV2NewV;     //�Ǳ��Ӧԭʼ�㣬������Ϊ��Ӧ���µ�����
	std::vector<unsigned>          m_vOldTri2NewTri; //�Ǳ�Ϊԭʼ����Ƭ��������Ϊ��Ӧ���µ�����Ƭ����
	
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