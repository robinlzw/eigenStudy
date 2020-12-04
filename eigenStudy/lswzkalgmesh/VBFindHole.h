#pragma once
#include <vmath\vfmesh.h>
#include <sys\alg\isysmesh.h>

//查找孔洞，该算法每个边界点只允许用一次

class VBFindHole2
{
public:
	VBFindHole2():count(0) {}
	~VBFindHole2() {}

	void Build(VNWZKALG::VSHoleBoundary & Bndry, const VSPerfTopoGraph & gm)
	{
		FindAll(gm);

		Bndry.bndry = VD_V2CB(m_vBndryTotal);
	}

private:
	void      FindAll(const VSPerfTopoGraph & gm);
	void      FlagBndryVert(const VSPerfTopoGraph & gm);
	void      SingleBndryProc(const VSPerfTopoGraph & gm, unsigned vid);
	unsigned  FindBndryVert();

private:
	std::vector<VNWZKALG::VSSingleBoundary>    m_vBndryTotal;
	std::vector<std::vector<unsigned>>         m_vHoleBndry;
										     
	std::vector<unsigned>                      m_vVertBndry;
	std::vector<unsigned>                      m_vTemp;
	std::vector<bool>                          m_vVertFlag;
	int                                        count;
};

// Modified by chakai, 2019/2/10 , 用VBFindHole2替代VBFindHole，保留旧接口
class VBFindHole
{
public:
	VBFindHole(){}
	~VBFindHole() {}

	void Build(VNWZKALG::VSHoleBoundary & Bndry, const VSGraphMesh & gm)
	{
        m_fhProxy.Build( Bndry , gm ) ;
		//FindAll(gm);

		//Bndry.bndry = VD_V2CB(m_vBndryTotal);
	}

private:
    VBFindHole2 m_fhProxy ;
//	void      FindAll(const VSGraphMesh & gm);
//	void      FlagBndryVert(const VSGraphMesh & gm);
//	void      SingleBndryProc(const VSGraphMesh & gm, unsigned vid);
//	unsigned  FindBndryVert();
//
//private:
//	std::vector<VNWZKALG::VSSingleBoundary>    m_vBndryTotal;
//	std::vector<std::vector<unsigned>>         m_vHoleBndry;
//										     
//	std::vector<unsigned>                      m_vVertBndry;
//	std::vector<unsigned>                      m_vTemp;
//	std::vector<bool>                          m_vVertFlag;
//	int                                        count;
};

//当出现两条边界共用一个顶点的时候会出现死循环的情况