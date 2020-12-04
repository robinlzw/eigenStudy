#pragma once

#include<vmath/vfmesh.h>

#include <sys/alg/isysmesh.h>

//Íø¸ñÑ¹ËõÖÐÎó²îµÄ²¹³¥

struct DLE3
{
	double x;
	double y;
	double z;

	DLE3():x(0.), y(0.),z(0.) {}
	~DLE3() {}
};

class VBDeltaProcess
{
public:
	VBDeltaProcess() {}
	~VBDeltaProcess() {}

	void Build(VSConstBuffer<VNVECTOR3I> & delta, const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const double & deltaCoeff)
	{
		MemberInit();
		PredictionD(realCoord, vMapID, deltaMapID, deltaCoeff);

		m_vDeltaIntDelFirst.resize(m_vDeltaInt.size() - 1);
		memcpy(&m_vDeltaIntDelFirst.front(), &m_vDeltaInt[1].x, sizeof(VNVECTOR3I) * (m_vDeltaInt.size() - 1));
		delta = VD_V2CB(m_vDeltaIntDelFirst);
	}

private:
	void  MemberInit();
	void  Init(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const float & deltaCoeff);
	void  Prediction(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const float & deltaCoeff);

	void InitD(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const double & deltaCoeff);
	void PredictionD(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const double & deltaCoeff);
	void ProcD(DLE3 & vPredD, DLE3 & deltaD, unsigned ID,
		const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, 
		const VSConstBuffer<VNVECTOR3UI>& deltaMapID, const double & deltaCoeff);
private:
	std::vector<VNVECTOR3I>     m_vDeltaIntDelFirst;
	std::vector<VNVECTOR3I>     m_vDeltaInt;
	std::vector<VFVECTOR3>      m_vPredRealCoord;

	std::vector<double>			m_vPredRealCoordX;
	std::vector<double>			m_vPredRealCoordY;
	std::vector<double>			m_vPredRealCoordZ;
};