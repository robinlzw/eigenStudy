#pragma once
#include "VBMeshWithHole2OV.h"
#include "VBFindHole.h"
#include "VBEDCmpWithHandles.h"
#include "VBEDDecWithHandles.h"
#include "VBCLERSEncode.h"
#include "VBDeltaProcWithHandle.h"
#include "VBIntLossPorcess.h"
#include "VBRangeEncode.h"

#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>

//带环柄的网格压缩组合

class VBEDCmpCombHandle
{
public:
	VBEDCmpCombHandle(): Rmax(1 << 31), Rmin(1 << 23), vert(0.f, 0.f, 0.f), bndryLen(0) {}
	~VBEDCmpCombHandle() {}

	void Build(VNWZKALG::VSEdgeBreakerDecInfo & res, const VSGraphMesh & gm, const unsigned & startCorner, const double & deltaCoeff)
	{
		CombFunc(gm, startCorner, deltaCoeff);

		res.vertLen           = gm.GetVertexCount();
		res.triLen            = gm.GetSurfCount();
		res.firstVert         = vert;
		res.delVertID         = VD_INVALID_INDEX;
		res.TopoBin           = VD_V2CB(m_vCodeBin);
		res.compRes           = VD_V2CB(m_vCompress);
		res.vertMapID         = VD_V2CB(m_vVertMapID);
		res.triMapID          = VD_V2CB(m_vTriMapID);
		res.handles           = VD_V2CB(m_vHandle);
		res.bndryLen          = bndryLen;
		res.bndryV            = VD_V2CB(m_vBndryV);
		res.vertMapIDwithHole = VD_V2CB(m_vVtMapIDwithBndry);
		res.vtMapIdUC2C       = VD_V2CB(m_vVtMapOld2New);
		res.triMapIdUC2C      = VD_V2CB(m_vTriMapOld2New);
	}

private:
	void  MemberInit();
	void  CombFunc(const VSGraphMesh & gm, const unsigned & startCorner, const double & deltaCoeff);
	void  LowBitProcess(std::vector<char> & vec, const VNWZKALG::VSLossQuantization & LowBit);

private:
	VBMeshWithHole2OV         m_objMesh2OV;
	VBFindHole                m_objFindBndry;
	VBEDCmpWithHandles        m_objCmpHandle;
	VBCLERSEncode             m_objCLERSProc;

	VBEDDecWithHandles        m_objDecHandle;
	VBDeltaProcWithHandle     m_objDeltaProc;
	VBDeltaProcess            m_objDelta;

private:
	std::vector<VNWZKALG::VSCompressRes>    m_vCompress;
	std::vector<char>                       m_vCodeBin;
	std::vector<char>                       m_vLowBitX;
	std::vector<char>                       m_vLowBitY;
	std::vector<char>                       m_vLowBitZ;
	std::vector<char>                       m_vHighBitX;
	std::vector<char>                       m_vHighBitY;
	std::vector<char>                       m_vHighBitZ;
	std::vector<unsigned>                   m_vVertMapID;
	std::vector<unsigned>                   m_vVtMapIDwithBndry;
	std::vector<unsigned>                   m_vTriMapID;
	std::vector<VNVECTOR2UI>                m_vHandle;
	std::vector<VFVECTOR3>                  m_vBndryV;
	std::vector<unsigned>                   m_vVtMapOld2New;
	std::vector<unsigned>                   m_vTriMapOld2New;
								            
	std::vector<int>                        m_vDeltaIntX;
	std::vector<int>                        m_vDeltaIntY;
	std::vector<int>                        m_vDeltaIntZ;
	std::vector<bool>                       m_vBitSetTemp;
private:
	unsigned    Rmin;
	unsigned    Rmax;
	VFVECTOR3   vert;
	unsigned    bndryLen;
};
