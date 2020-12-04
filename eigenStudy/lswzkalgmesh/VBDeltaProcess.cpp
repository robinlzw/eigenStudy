#include "stdafx.h"
#include "VBDeltaProcess.h"

void VBDeltaProcess::MemberInit()
{
	m_vDeltaInt.clear();
	m_vPredRealCoord.clear();

	m_vPredRealCoordX.clear();
	m_vPredRealCoordY.clear();
	m_vPredRealCoordZ.clear();
}

void VBDeltaProcess::Init(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const float & deltaCoeff)
{
	m_vDeltaInt.resize(realCoord.len);
	m_vPredRealCoord.resize(realCoord.len);

	m_vPredRealCoord[0] = realCoord.pData[vMapID.pData[0]];
	m_vDeltaInt[0].x = m_vDeltaInt[0].y = m_vDeltaInt[0].z = 0;

	VNVECTOR3I deltaInt;
	VFVECTOR3 vPred = m_vPredRealCoord[deltaMapID.pData[1].x];
	VFVECTOR3 delta = realCoord.pData[vMapID.pData[1]] - vPred;
	deltaInt.x = static_cast<int>(delta.x / deltaCoeff);
	deltaInt.y = static_cast<int>(delta.y / deltaCoeff);
	deltaInt.z = static_cast<int>(delta.z / deltaCoeff);
	m_vDeltaInt[1] = deltaInt;
	delta.x = static_cast<float>(deltaInt.x * deltaCoeff);
	delta.y = static_cast<float>(deltaInt.y * deltaCoeff);
	delta.z = static_cast<float>(deltaInt.z * deltaCoeff);
	m_vPredRealCoord[1] = vPred + delta;

	vPred = (m_vPredRealCoord[deltaMapID.pData[2].x] + m_vPredRealCoord[deltaMapID.pData[2].y]) * .5f;
	delta = realCoord.pData[vMapID.pData[2]] - vPred;
	deltaInt.x = static_cast<int>(delta.x / deltaCoeff);
	deltaInt.y = static_cast<int>(delta.y / deltaCoeff);
	deltaInt.z = static_cast<int>(delta.z / deltaCoeff);
	m_vDeltaInt[2] = deltaInt;
	delta.x = static_cast<float>(deltaInt.x * deltaCoeff);
	delta.y = static_cast<float>(deltaInt.y * deltaCoeff);
	delta.z = static_cast<float>(deltaInt.z * deltaCoeff);
	m_vPredRealCoord[2] = vPred + delta;	
}

void VBDeltaProcess::Prediction(const VSConstBuffer<VFVECTOR3> & realCoord, const VSConstBuffer<unsigned> & vMapID, const VSConstBuffer<VNVECTOR3UI> & deltaMapID, const float & deltaCoeff)
{
	Init(realCoord, vMapID, deltaMapID, deltaCoeff);

	VNVECTOR3I  deltaInt;
	VFVECTOR3   delta, vPred;
	for (unsigned i = 3; i < realCoord.len; i++)
	{
		vPred = m_vPredRealCoord[deltaMapID.pData[i].x] + m_vPredRealCoord[deltaMapID.pData[i].y] - m_vPredRealCoord[deltaMapID.pData[i].z];
		delta = realCoord.pData[vMapID.pData[i]] - vPred;
		deltaInt.x = static_cast<int>(delta.x / deltaCoeff);
		deltaInt.y = static_cast<int>(delta.y / deltaCoeff);
		deltaInt.z = static_cast<int>(delta.z / deltaCoeff);
		m_vDeltaInt[i] = deltaInt;
		delta.x = static_cast<float>(deltaInt.x * deltaCoeff);
		delta.y = static_cast<float>(deltaInt.y * deltaCoeff);
		delta.z = static_cast<float>(deltaInt.z * deltaCoeff);
		m_vPredRealCoord[i] = vPred + delta;
	}

	m_vPredRealCoord.clear();
}

void VBDeltaProcess::InitD(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& deltaMapID, const double & deltaCoeff)
{
	m_vDeltaInt.resize(realCoord.len);
	m_vPredRealCoordX.resize(realCoord.len);
	m_vPredRealCoordY.resize(realCoord.len);
	m_vPredRealCoordZ.resize(realCoord.len);

	m_vPredRealCoordX[0] = realCoord.pData[vMapID.pData[0]].x;
	m_vPredRealCoordY[0] = realCoord.pData[vMapID.pData[0]].y;
	m_vPredRealCoordZ[0] = realCoord.pData[vMapID.pData[0]].z;
	m_vDeltaInt[0].x = m_vDeltaInt[0].y = m_vDeltaInt[0].z = 0;

	DLE3   vPredD, deltaD;
	vPredD.x = m_vPredRealCoordX[deltaMapID.pData[1].x];
	vPredD.y = m_vPredRealCoordY[deltaMapID.pData[1].x];
	vPredD.z = m_vPredRealCoordZ[deltaMapID.pData[1].x];
	ProcD(vPredD, deltaD, 1, realCoord, vMapID, deltaMapID, deltaCoeff);

	vPredD.x = (m_vPredRealCoordX[deltaMapID.pData[2].x] + m_vPredRealCoordX[deltaMapID.pData[2].y]) * .5;
	vPredD.y = (m_vPredRealCoordY[deltaMapID.pData[2].x] + m_vPredRealCoordY[deltaMapID.pData[2].y]) * .5;
	vPredD.z = (m_vPredRealCoordZ[deltaMapID.pData[2].x] + m_vPredRealCoordZ[deltaMapID.pData[2].y]) * .5;
	ProcD(vPredD, deltaD, 2, realCoord, vMapID, deltaMapID, deltaCoeff);
}

void VBDeltaProcess::PredictionD(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& deltaMapID, const double & deltaCoeff)
{
	InitD(realCoord, vMapID, deltaMapID, deltaCoeff);

	DLE3  deltaD, vPredD;
	for (unsigned i = 3; i < realCoord.len; i++)
	{
		vPredD.x = m_vPredRealCoordX[deltaMapID.pData[i].x] + m_vPredRealCoordX[deltaMapID.pData[i].y] - m_vPredRealCoordX[deltaMapID.pData[i].z];
		vPredD.y = m_vPredRealCoordY[deltaMapID.pData[i].x] + m_vPredRealCoordY[deltaMapID.pData[i].y] - m_vPredRealCoordY[deltaMapID.pData[i].z];
		vPredD.z = m_vPredRealCoordZ[deltaMapID.pData[i].x] + m_vPredRealCoordZ[deltaMapID.pData[i].y] - m_vPredRealCoordZ[deltaMapID.pData[i].z];
		ProcD(vPredD, deltaD, i, realCoord, vMapID, deltaMapID, deltaCoeff);
	}		
}

void VBDeltaProcess::ProcD(DLE3 & vPredD, DLE3 & deltaD, unsigned ID, const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& deltaMapID, const double & deltaCoeff)
{	
	deltaD.x = realCoord.pData[vMapID.pData[ID]].x - vPredD.x;
	deltaD.y = realCoord.pData[vMapID.pData[ID]].y - vPredD.y;
	deltaD.z = realCoord.pData[vMapID.pData[ID]].z - vPredD.z;
	m_vDeltaInt[ID].x = static_cast<int>(deltaD.x / deltaCoeff);
	m_vDeltaInt[ID].y = static_cast<int>(deltaD.y / deltaCoeff);
	m_vDeltaInt[ID].z = static_cast<int>(deltaD.z / deltaCoeff);
	deltaD.x = static_cast<double>(m_vDeltaInt[ID].x * deltaCoeff);
	deltaD.y = static_cast<double>(m_vDeltaInt[ID].y * deltaCoeff);
	deltaD.z = static_cast<double>(m_vDeltaInt[ID].z * deltaCoeff);
	m_vPredRealCoordX[ID] = vPredD.x + deltaD.x;
	m_vPredRealCoordY[ID] = vPredD.y + deltaD.y;
	m_vPredRealCoordZ[ID] = vPredD.z + deltaD.z;
}

//VD_EXPORT_SYSTEM_SIMPLE(VBDeltaProcess, VNALGMESH::VRDeltaProcess);