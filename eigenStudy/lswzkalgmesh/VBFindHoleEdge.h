#pragma once
#include <vmath\vfmesh.h>
#include <sys\alg\isysmesh.h>

//查找孔洞，该算法每个边界边只允许用一次

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

	std::vector<unsigned>                      m_vVertDegree;    //顶点所在的边界个数
	std::vector<bool>                          m_vEdgeFlag;      //false：非边界边，true：边界边
	std::vector<unsigned>                      m_vDegID;         //顶点所在边界数大于1的顶点ID
	std::vector<bool>                          m_vDegIDFlag;     //标记顶点所在边界数大于1的顶点ID
	std::vector<unsigned>                      m_vTemp;
	unsigned                                   BndryEdgeNum;
	bool                                       DegFlag;
};

//允许多条边界共用一个顶点的情况，但不能保证此时边界的唯一性