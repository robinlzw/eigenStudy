#include "stdafx.h"
#include "VBDeltaProcWithHandle.h"

void VBDeltaProcWithHandle::Init(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& decPredMapID, const VSConstBuffer<unsigned> & decBndry, const float & deltaCoeff)
{
	vCount = 0;
	m_vDeltaInt.resize(realCoord.len);
	m_vPredRealCoord.resize(realCoord.len);
	m_vVertFlag.resize(realCoord.len);

	VFVECTOR3  delta, vPred;
	VNVECTOR3I deltaInt(0, 0, 0);
	for (unsigned i = 0; i < decBndry.len; i++)
	{
		delta = realCoord.pData[vMapID.pData[i]];
		m_vDeltaInt[i] = deltaInt;
		m_vPredRealCoord[ decBndry.pData[i] ] = delta;
		vCount ++;

		m_vVertFlag[ decBndry.pData[i] ] = true;
	}

	//the first vertex
	vPred = (m_vPredRealCoord[decPredMapID.pData[vCount].x] + m_vPredRealCoord[decPredMapID.pData[vCount].y]) * .5f;
	delta = realCoord.pData[vMapID.pData[vCount]] - vPred;
	deltaInt.x = static_cast<int>(delta.x / deltaCoeff);
	deltaInt.y = static_cast<int>(delta.y / deltaCoeff);
	deltaInt.z = static_cast<int>(delta.z / deltaCoeff);
	m_vDeltaInt[vCount] = deltaInt;
	delta.x = static_cast<float>(deltaInt.x * deltaCoeff);
	delta.y = static_cast<float>(deltaInt.y * deltaCoeff);
	delta.z = static_cast<float>(deltaInt.z * deltaCoeff);
	m_vPredRealCoord[0] = vPred + delta;
	m_vVertFlag[0] = true;
	vCount ++;
}

void VBDeltaProcWithHandle::Prediction(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& decPredMapID, const VSConstBuffer<unsigned>& decBndry, const float & deltaCoeff)
{
	Init(realCoord, vMapID, decPredMapID, decBndry, deltaCoeff);

	VFVECTOR3  delta, vPred;
	VNVECTOR3I deltaInt;
	for (unsigned i = 3; i < realCoord.len; i++)
	{
		if (!m_vVertFlag[i])
		{
			vPred = m_vPredRealCoord[decPredMapID.pData[vCount].x] + m_vPredRealCoord[decPredMapID.pData[vCount].y] - m_vPredRealCoord[decPredMapID.pData[vCount].z];
			delta = realCoord.pData[vMapID.pData[vCount]] - vPred;
			deltaInt.x = static_cast<int>(delta.x / deltaCoeff);
			deltaInt.y = static_cast<int>(delta.y / deltaCoeff);
			deltaInt.z = static_cast<int>(delta.z / deltaCoeff);
			m_vDeltaInt[vCount] = deltaInt;
			delta.x = static_cast<float>(deltaInt.x * deltaCoeff);
			delta.y = static_cast<float>(deltaInt.y * deltaCoeff);
			delta.z = static_cast<float>(deltaInt.z * deltaCoeff);
			m_vPredRealCoord[i] = vPred + delta;
			m_vVertFlag[i] = true;
			vCount ++;
		}
	}
}

void VBDeltaProcWithHandle::InitDouble(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& decPredMapID, const VSConstBuffer<unsigned> & decBndry, const double & deltaCoeff)
{
	vCount = 0;
	m_vDeltaInt.resize(realCoord.len);
	m_vPredRealCoordD.resize(realCoord.len);
	m_vVertFlag.resize(realCoord.len);

	DLE3   vPredD, deltaD;
	VNVECTOR3I temp(0, 0, 0);
	//hole bndry process
	for (unsigned i = 0; i < decBndry.len; i++)
	{
		vPredD.x = realCoord.pData[vMapID.pData[i]].x;
		vPredD.y = realCoord.pData[vMapID.pData[i]].y;
		vPredD.z = realCoord.pData[vMapID.pData[i]].z;
		m_vDeltaInt[i] = temp;
		m_vPredRealCoordD[decBndry.pData[i]] = vPredD;
		vCount++;

		m_vVertFlag[decBndry.pData[i]] = true;
	}

	//the first vertex
	vPredD.x = (m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].x + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].x) * .5;
	vPredD.y = (m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].y + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].y) * .5;
	vPredD.z = (m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].z + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].z) * .5;
	ProcDouble(vPredD, deltaD, vCount, realCoord, vMapID, deltaCoeff);
	OutPut(vPredD, deltaD, 0);
	m_vVertFlag[0] = true;
	vCount ++;
}

void VBDeltaProcWithHandle::PredictionDouble(const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const VSConstBuffer<VNVECTOR3UI>& decPredMapID, const VSConstBuffer<unsigned> & decBndry, const double & deltaCoeff)
{
	InitDouble(realCoord, vMapID, decPredMapID, decBndry, deltaCoeff);

	DLE3  deltaD, vPredD;
	for (unsigned i = 3; i < realCoord.len; i++)
	{
		if (!m_vVertFlag[i])
		{
			vPredD.x = m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].x + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].x - m_vPredRealCoordD[decPredMapID.pData[ vCount ].z].x;
			vPredD.y = m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].y + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].y - m_vPredRealCoordD[decPredMapID.pData[ vCount ].z].y;
			vPredD.z = m_vPredRealCoordD[decPredMapID.pData[ vCount ].x].z + m_vPredRealCoordD[decPredMapID.pData[ vCount ].y].z - m_vPredRealCoordD[decPredMapID.pData[ vCount ].z].z;
			ProcDouble(vPredD, deltaD, vCount, realCoord, vMapID, deltaCoeff);
			OutPut(vPredD, deltaD, i);
			m_vVertFlag[i] = true;
			vCount ++;
		}
	}
}

void VBDeltaProcWithHandle::ProcDouble(DLE3 & vPredD, DLE3 & deltaD, unsigned ID, const VSConstBuffer<VFVECTOR3>& realCoord, const VSConstBuffer<unsigned>& vMapID, const double & deltaCoeff)
{
	deltaD.x = realCoord.pData[vMapID.pData[ID]].x - vPredD.x;
	deltaD.y = realCoord.pData[vMapID.pData[ID]].y - vPredD.y;
	deltaD.z = realCoord.pData[vMapID.pData[ID]].z - vPredD.z;
	m_vDeltaInt[ vCount ].x = static_cast<int>(deltaD.x / deltaCoeff);      ////
	m_vDeltaInt[ vCount ].y = static_cast<int>(deltaD.y / deltaCoeff);      ////
	m_vDeltaInt[ vCount ].z = static_cast<int>(deltaD.z / deltaCoeff);      ////
	deltaD.x = static_cast<double>(m_vDeltaInt[ vCount ].x * deltaCoeff);
	deltaD.y = static_cast<double>(m_vDeltaInt[ vCount ].y * deltaCoeff);
	deltaD.z = static_cast<double>(m_vDeltaInt[ vCount ].z * deltaCoeff);
	//m_vPredRealCoordX[ID] = vPredD.x + deltaD.x;
	//m_vPredRealCoordY[ID] = vPredD.y + deltaD.y;
	//m_vPredRealCoordZ[ID] = vPredD.z + deltaD.z;
}

void VBDeltaProcWithHandle::OutPut(const DLE3 & vPredD, const DLE3 & deltaD, unsigned id)
{
	m_vPredRealCoordD[id].x = vPredD.x + deltaD.x;
	m_vPredRealCoordD[id].y = vPredD.y + deltaD.y;
	m_vPredRealCoordD[id].z = vPredD.z + deltaD.z;
}
