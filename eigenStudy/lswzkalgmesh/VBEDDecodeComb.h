#pragma once

#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>

#include "VBCLERSDecode.h"
#include "VBEDDecompression.h"
#include "VBQuantizationToFloat.h"
#include "VBRangeDecode.h"
#include "VBEDEncodeComb.h"

//带环柄的网格压缩解码组合

using namespace VNWZKALG;

class VBEDDecodeComb
{
public:
	VBEDDecodeComb() : Rmax(1 << 31), Rmin(1 << 23){}
	~VBEDDecodeComb() {}

	void Build(VSEdgeBreakerDecRes & smsh, const VSEdgeBreakerDecInfo & dec, const float & deltaCoeff)
	{
		CombFunc(dec, deltaCoeff);

		smsh = msh;

		/*std::ofstream f("D:\\quanDelta.txt");
		for (unsigned i = 0; i < m_vDelta.size(); i++)
		{
			f << m_vDelta[i].x << " " << m_vDelta[i].y << " " << m_vDelta[i].z << std::endl;
		}
		f.close();*/

		/*std::ofstream f("D:\\mesh.obj");
		for (unsigned i = 0; i < smsh.smesh.nVertCount; i++)
		{
			f << "v " << smsh.smesh.pVertices[i].x << " " << smsh.smesh.pVertices[i].y << " " << smsh.smesh.pVertices[i].z << std::endl;
		}
		f << std::endl;
		for (unsigned i = 0; i < smsh.smesh.nTriangleCount; i++)
		{
			f << "f " << smsh.smesh.pTriangles[i].x + 1 << " " << smsh.smesh.pTriangles[i].y + 1 << " " << smsh.smesh.pTriangles[i].z + 1 << std::endl;
		}
		f.close();*/
	}

private:
	void MemberInit();
	void CombFunc(const VSEdgeBreakerDecInfo & dec, const float & deltaCoeff);
	void MergeToFloat(const VSEdgeBreakerDecInfo & dec, const float & deltaCoeff);
	void Decode(const VSEdgeBreakerDecInfo & dec);
	void TypeTran(const VSEdgeBreakerDecInfo & dec);
	void LowBitPorcess(std::vector<unsigned> & lowBitValue, const VSConstBuffer<char> & lowBitChar, const unsigned & totalLowBitLen, const unsigned & lowBitLen);

	void MergeToDouble(const VSEdgeBreakerDecInfo & dec, const double deltaCoeff);
	void MergeD(std::vector<double> & merg, const VSConstBuffer<unsigned> & highBit, const VSConstBuffer<unsigned> & lowBit, const double delta, const unsigned & lowBitLen, const int & min);
private:
	VBCLERSDecode          cdecode;		
	VBEDDecompression      edDec;

private:	
	std::vector<VFVECTOR3>      m_vDelta;
	std::vector<unsigned>       m_vHighBitValX;
	std::vector<unsigned>       m_vHighBitValY;
	std::vector<unsigned>       m_vHighBitValZ;
	std::vector<unsigned>       m_vLowBitValX;
	std::vector<unsigned>       m_vLowBitValY;
	std::vector<unsigned>       m_vLowBitValZ;
	std::vector<bool>           m_vBitSetTemp;

	std::vector<VFVECTOR3>      m_vDeltaTemp;
	std::vector<DLE3>           m_vDeltaD;
	std::vector<DLE3>           m_vDeltaTempD;
	std::vector<double>         m_vMergeX;
	std::vector<double>         m_vMergeY;
	std::vector<double>         m_vMergeZ;

private:
	unsigned Rmax;
	unsigned Rmin;
	VSEdgeBreakerDecRes    msh;
};