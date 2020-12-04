#pragma once
#include "VBMeshWithHole2OV.h"
#include "VBEDCmpWithHandles.h"

//带环柄的网格解码

struct DecResHole
{
	VSSimpleMeshF                 smesh;
	//VSConstBuffer<VNVECTOR3UI>    relation;
	//VSConstBuffer<unsigned>       decBndry;
};

class VBEDDecWithHandles
{
public:
	VBEDDecWithHandles() : triNum(0), hCount(0), vCount(0) {}
	~VBEDDecWithHandles() {}

	void Build(VSSimpleMeshF & smesh, const HandleCmpRes & hcr)
	{
		InitDecompress(hcr);

        m_vVertD2F.resize(m_vVertOut.size());
        for (unsigned i = 0; i < m_vVertOut.size(); i++)
        {
            m_vVertD2F[ i ].x = (float)m_vVertOut[ i ].x;
            m_vVertD2F[ i ].y = (float)m_vVertOut[ i ].y;
            m_vVertD2F[ i ].z = (float)m_vVertOut[ i ].z;
        }

		//std::ofstream fp("D:\\vd.txt");
		//for (unsigned i = 0; i < m_vVert.size(); i++)
		//{
		//	fp << m_vVert[i].x << " " << m_vVert[i].y << " " << m_vVert[i].z << std::endl;
		//}
		//fp.close();
		//fp.open("D:\\vdouble.txt");
		//for (unsigned i = 0; i < m_vVertD.size(); i++)
		//{
		//	fp << "V " << m_vVertD[i].x << " " << m_vVertD[i].y << " " << m_vVertD[i].z << std::endl;
		//}
		//fp.close();
		//std::ofstream fp("D:\\test.obj");
		//fp.precision(8);
		//for (unsigned i = 0; i < m_vVertD.size(); i++)
		//{
		//	fp << "v " << m_vVertD[i].x << " " << m_vVertD[i].y << " " << m_vVertD[i].z << std::endl;
		//}
		//fp << std::endl;
		//for (unsigned i = 0; i < m_vTriangle.size(); i++)
		//{
		//	fp << "f " << m_vTriangle[i].x + 1 << "  " << m_vTriangle[i].y + 1 << "  " << m_vTriangle[i].z + 1 << std::endl;
		//}
		//fp.close();
		
		smesh.nTriangleCount = (unsigned)m_vTriangleOut.size();
		smesh.nVertCount     = (unsigned)m_vVertOut.size();
		smesh.pTriangles     = &m_vTriangleOut.front();
		smesh.pVertices      = &m_vVertD2F.front();
	}

private:
	void        InitDecompress(const HandleCmpRes & res);
	void        DecompressConnectivity(const VSConstBuffer<char> & clers, const VSConstBuffer<VNVECTOR2UI> & handles, unsigned blen, unsigned c);
	unsigned    SpecialCase(const VSConstBuffer<char> & clers, const VSConstBuffer<VNVECTOR2UI> & handles, unsigned blen, unsigned c);
	bool        CheckHandle(const VSConstBuffer<VNVECTOR2UI> & handles, unsigned c);
	void        Zip(unsigned c);
	void        InitDecompressVertices(const HandleCmpRes & res);
	void        DecompressVertices(const HandleCmpRes & res, unsigned c);
	VFVECTOR3   DecodeDelta(const VSConstBuffer<VFVECTOR3> & res, unsigned c);
	void        TranVTableToTri();

	DLE3        DecodeDeltaD(const VSConstBuffer<DLE3> & res, unsigned c);
	DLE3        AddD(const DLE3 & delta, const DLE3 & pred);

	//help function
	inline unsigned NextCorner(unsigned c);
	inline unsigned PrevCorner(unsigned c);
	inline unsigned RightTriCorner(unsigned c);
	inline unsigned LeftTriCorner(unsigned c);
	inline unsigned CornerToTri(unsigned c);

private:
	void     DelVertAndSurf(const unsigned & delID);

private:
	std::vector<int>         m_vOTable;
	std::vector<int>         m_vVTable;
	std::vector<VFVECTOR3>   m_vVert;
	std::vector<VNVECTOR3UI> m_vTriangle;
	//std::vector<VNVECTOR3UI> m_vRelation;   //vp = x + y - z
	//std::vector<unsigned>    m_vDecBndry;
	std::vector<DLE3>        m_vVertD;
    std::vector<VFVECTOR3>   m_vVertD2F;

	std::stack<unsigned>     m_sBranch;
	std::vector<bool>        m_vVertFlag;
	std::vector<bool>        m_vTriFlag;
	std::vector<DLE3>        m_vVertOut;
	std::vector<VNVECTOR3UI> m_vTriangleOut;

	unsigned  triNum;
	unsigned  hCount;
	unsigned  vCount;
	unsigned  EBcount;
	unsigned  bndryNum;
	//VNVECTOR3UI   relation;
};
