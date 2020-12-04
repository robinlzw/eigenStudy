#pragma once
#include "VBEDDecWithHandles.h"
#include "VBCLERSDecode.h"
#include "VBRangeDecode.h"
#include "VBFloatLossMerge.h"

#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>

//带环柄的网格压缩解码

class VBEDDecCombHandle
{
public:
	VBEDDecCombHandle() {}
	~VBEDDecCombHandle() {}

	void Build(VSSimpleMeshF & smsh, const VNWZKALG::VSEdgeBreakerDecInfo & dec, const float & deltaCoeff)
	{
		MemberInit();
		CombFunc(dec, deltaCoeff);

		smsh = msh;
	}

private:
	void MemberInit();
	void CombFunc(const VNWZKALG::VSEdgeBreakerDecInfo & dec, const float & deltaCoeff);
	void MergeToFloat(const VNWZKALG::VSEdgeBreakerDecInfo & dec, const float & deltaCoeff);
	void Decode(const VNWZKALG::VSEdgeBreakerDecInfo & dec);
	void TypeTran(const VNWZKALG::VSEdgeBreakerDecInfo & dec);
	void LowBitPorcess(std::vector<unsigned> & lowBitValue, const VSConstBuffer<char> & lowBitChar, const unsigned & totalLowBitLen, const unsigned & lowBitLen);

	void MergeToDouble(const VNWZKALG::VSEdgeBreakerDecInfo & dec, const double deltaCoeff);
	void MergeD(std::vector<double> & merg, const VSConstBuffer<unsigned> & highBit, const VSConstBuffer<unsigned> & lowBit, const double delta, const unsigned & lowBitLen, const int & min);
private:
	VBCLERSDecode          m_objCLERSDec;
	VBEDDecWithHandles     m_objDec;

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
	VSSimpleMeshF    msh;
};
