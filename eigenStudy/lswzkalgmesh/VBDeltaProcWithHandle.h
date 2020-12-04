#pragma once
#include "VBDeltaProcess.h"

class VBDeltaProcWithHandle
{
public:
	VBDeltaProcWithHandle() : vCount(0) {}
	~VBDeltaProcWithHandle() {}

	//deltaMapID == decode relation id
	void Build(VSConstBuffer<VNVECTOR3I> & delta, const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, 
		       const VSConstBuffer<VNVECTOR3UI> & decPredMapID, const VSConstBuffer<unsigned> & decBndry, const double & deltaCoeff)
	{
		PredictionDouble(realCoord, vMapID, decPredMapID, decBndry, deltaCoeff);

		delta = VD_V2CB(m_vDeltaInt);
	}

private:
	void  Init(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & decPredMapID, const VSConstBuffer<unsigned> & decBndry, const float & deltaCoeff);
	void  Prediction(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & decPredMapID, const VSConstBuffer<unsigned> & decBndry, const float & deltaCoeff);


	void InitDouble(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, 
		            const VSConstBuffer<VNVECTOR3UI> & decPredMapID, const VSConstBuffer<unsigned> & decBndry, const double & deltaCoeff);
	void PredictionDouble(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, 
		                  const VSConstBuffer<VNVECTOR3UI> & decPredMapID, const VSConstBuffer<unsigned> & decBndry, const double & deltaCoeff);
	void ProcDouble(DLE3 & vPredD, DLE3 & deltaD, unsigned ID, const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const double & deltaCoeff);
	void OutPut(const DLE3 & vPredD, const DLE3 & deltaD, unsigned id);

private:
	std::vector<VNVECTOR3I>     m_vDeltaInt;
	std::vector<VFVECTOR3>      m_vPredRealCoord;
	std::vector<DLE3>           m_vPredRealCoordD;

	std::vector<bool>           m_vVertFlag;
	unsigned  vCount;
};
