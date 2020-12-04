#pragma once
#include <vmath\vfmesh.h>
#include <sys\alg\isysmesh.h>

//���ҿ׶������㷨ÿ���߽��ֻ������һ��

class VBFindHoleEdge
{
public:
	VBFindHoleEdge() : BndryEdgeNum(0) {}
	~VBFindHoleEdge() {}

	void Build(VNWZKALG::VSHoleBoundary & Bndry, const VSGraphMesh & gm)
	{
		FindAll(gm);
		Bndry.bndry = VD_V2CB(m_vBndryTotal);
	}

private:
	void       FindAll(const VSGraphMesh & gm);
	void       SingleBndryProc(const VSGraphMesh & gm, unsigned eid);
	void       EdgeVertFlag(const VSGraphMesh & gm);
	unsigned   FindBndryEdge(const VSGraphMesh & gm);

private:
	std::vector<VNWZKALG::VSSingleBoundary>    m_vBndryTotal;
	std::vector<std::vector<unsigned>>         m_vHoleBndry;

	std::vector<unsigned>                      m_vVertDegree;    //�������ڵı߽����
	std::vector<bool>                          m_vEdgeFlag;      //false���Ǳ߽�ߣ�true���߽��
	std::vector<unsigned>                      m_vDegID;         //�������ڱ߽�������1�Ķ���ID
	std::vector<bool>                          m_vDegIDFlag;     //��Ƕ������ڱ߽�������1�Ķ���ID
	std::vector<unsigned>                      m_vTemp;
	unsigned                                   BndryEdgeNum;
	bool                                       DegFlag;
};

//��������߽繲��һ�����������������ܱ�֤��ʱ�߽��Ψһ��