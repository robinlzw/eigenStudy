#pragma once

#include<vmath/vfmesh.h>
#include"VBEDCompress.h"
#include"VBDeltaProcess.h"

#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//struct EDDec
//{
//	VSSimpleMeshF    smesh;
//	VSConstBuffer<VNVECTOR3UI>   vMapID;    // Vpred = Vx + Vy - Vz
//};

//Õ¯∏Ò—πÀıΩ‚—πÀ„∑®

class VBEDDecompression
{
public:
	VBEDDecompression() : sCount(0), vid(0), EBVcount(0), EBVcountTemp(0){}
	~VBEDDecompression() {}

	void Build(VSEdgeBreakerDecRes & mesh, const CLERS & cmp)
	{
		MemberInit();
		Decompress(cmp);
		
		mesh.smesh.nTriangleCount = m_vTriangle.size();
		mesh.smesh.nVertCount = m_vVert.size();
		mesh.smesh.pTriangles = &m_vTriangle.front();
		mesh.smesh.pVertices = &m_vVert.front();
		mesh.vMapID = VD_V2CB(m_vVertMapID);
	}

private:
	void        MemberInit();
	void        Decompress(const CLERS & cmp);
	void        TranVTableToTri();
	void        InitConnectivity(const CLERS & cmp);
	void        DecompressConnectivity(const CLERS & cmp, unsigned c);
	void        InitDecompressVertices(const CLERS & cmp);
	void        DecompressVertices(unsigned c, const CLERS & cmp);
	void        Zip(unsigned c);
	VFVECTOR3   DecodeDelta(unsigned c, const CLERS & cmp);
	VFVECTOR3   DecodeWithPrediction(unsigned c, const CLERS & cmp);

	DLE3        DecodeDeltaD(unsigned c, const CLERS & cmp);
	DLE3        DecodeWithPredictionD(unsigned c, const CLERS & cmp);
	//EB helper function
	inline unsigned NextCorner(unsigned c);
	inline unsigned PrevCorner(unsigned c);
	inline unsigned RightTri(unsigned c);
	inline unsigned LeftTri(unsigned c);
	inline unsigned CornerToTri(unsigned c);

private:
	std::vector<VNVECTOR3UI>     m_vTriangle;
	std::vector<VFVECTOR3>       m_vVert;
	std::vector<VNVECTOR3UI>     m_vVertMapID;    // Vpred = Vx + Vy - Vz

	std::vector<int>             m_vVTable;
	std::vector<int>             m_vOTable;
	std::vector<bool>            m_vTriFlag;
	std::vector<bool>            m_vVertFlag;

	std::vector<DLE3>            m_vVertD;
private:
	int EBVcount, EBVcountTemp;
	unsigned sCount;
	unsigned vid;
};

