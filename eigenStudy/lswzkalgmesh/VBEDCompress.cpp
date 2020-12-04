#include "stdafx.h"
#include "VBEDCompress.h"

void VBEDCompress::MemberInit()
{
	sCount = 0;
	vCount = 0;
	count = 1;
	clersNum = 0;

	m_vClers.clear();
	m_vDelta.clear();
	m_vVertMapID.clear();
	m_vVertEstimate.clear();
	m_vVertFlag.clear();
	m_vTriFlag.clear();
	m_vPredCoord.clear();
	m_vRealCoord.clear();
	m_vPredRelation.clear();
}

void VBEDCompress::CompressProc(const OVTable & ov, const unsigned startCorner)
{
	unsigned sNum = ov.OTable.len / 3;
	m_vSurfMapID.resize(sNum);
	m_vTriFlag.resize(sNum);
	m_vClers.resize(sNum - 1);             ////////
	m_vVertFlag.resize(ov.vNum);
	m_vVertEstimate.resize(ov.vNum);	
	m_vDelta.reserve(ov.vNum);
	m_vPredCoord.reserve(ov.vNum);
	m_vVertMapID.resize(ov.vNum);
	m_vPredRelation.reserve(ov.vNum);

	InitCompress(startCorner, ov);

	m_vVertEstimate.clear();
	m_vVertFlag.clear();
	m_vTriFlag.clear();
}

void VBEDCompress::InitCompress(unsigned startCorner, const OVTable & ov)
{
	//estimate 1st  vertex
	EncodeDelta(startCorner, ov);
	m_vVertFlag[ov.VTable.pData[startCorner]] = true;
	unsigned c = PrevCorner(startCorner);
	m_vVertMapID[0] = ov.VTable.pData[startCorner];

	//estimate third vertex
	EncodeDelta(c, ov);
	m_vVertFlag[ov.VTable.pData[c]] = true;
	m_vVertMapID[1] = ov.VTable.pData[c];
	//m_vDelta[1] = ov.VertCoord.pData[ov.VTable.pData[c]];

	//estimate second vertex
	EncodeDelta(PrevCorner(c), ov);
	m_vVertFlag[ov.VTable.pData[PrevCorner(c)]] = true;
	m_vVertMapID[2] = ov.VTable.pData[PrevCorner(c)];
	//m_vDelta[2] = ov.VertCoord.pData[ov.VTable.pData[PrevCorner(c)]];

	// mark the triangle as visited
	m_vTriFlag[CornerToTri(c)] = 1;
	vCount = 2;
	m_vSurfMapID[sCount] = CornerToTri(c);               ////////

	unsigned temp =ov.OTable.pData[c];
	while (temp != PrevCorner(ov.OTable.pData[PrevCorner(c)]))
	{
		m_vClers[count - 1] = 'C';                 
		count ++;
		m_vTriFlag[CornerToTri(temp)] = 1;
		sCount ++;	
		m_vSurfMapID[sCount] = CornerToTri(temp);        ////////

		EncodeDelta(temp, ov);
		m_vVertFlag[ov.VTable.pData[temp]] = true;

		vCount++;
		m_vVertMapID[vCount] = ov.VTable.pData[temp];

		temp = ov.OTable.pData[NextCorner(temp)];
	}
	m_vClers[count - 1] = 'R';                     
	m_vTriFlag[CornerToTri(temp)] = 1;
	sCount++;
	count++;
	m_vSurfMapID[sCount] = CornerToTri(temp);                 ////////
	
	clersNum = count - 1;
	Compress(ov.OTable.pData[PrevCorner(temp)], ov);
}

void VBEDCompress::Compress(unsigned c, const OVTable & ov)
{	
	do
	{
		m_vTriFlag[CornerToTri(c)] = 1;
		sCount ++;
		m_vSurfMapID[sCount] = CornerToTri(c);

		if (m_vVertFlag[ov.VTable.pData[c]] == 0)
		{
			m_vClers[clersNum] = 'C';
			clersNum ++;
			EncodeDelta(c, ov);
			if (!m_vVertFlag[ov.VTable.pData[c]])
			{
				vCount++;
				m_vVertMapID[vCount] = ov.VTable.pData[c];
			}
			m_vVertFlag[ov.VTable.pData[c]] = true;			
			c = RightTri(c, ov);			
		}
		else
		{
			if (m_vTriFlag[CornerToTri(RightTri(c, ov))] > 0)
			{
				if (m_vTriFlag[CornerToTri(LeftTri(c, ov))] > 0)
				{
					m_vClers[clersNum] = 'E';
					clersNum ++;
					return;
				}
				else
				{
					m_vClers[clersNum] = 'R';
					clersNum ++;
					c = LeftTri(c, ov);
				}
			}
			else
			{
				if (m_vTriFlag[CornerToTri(LeftTri(c, ov))] > 0)
				{
					m_vClers[clersNum] = 'L';
					clersNum ++;
					c = RightTri(c, ov);
				}
				else
				{
					m_vTriFlag[CornerToTri(c)] = sCount * 3 + 2;
					m_vClers[clersNum] = 'S';
					clersNum ++;
					Compress(RightTri(c, ov), ov);
					c = LeftTri(c, ov);
					if (m_vTriFlag[CornerToTri(c)] > 0)
						return;
				}
			}
		}
	} while (true);
}

void VBEDCompress::EncodeWithPrediction(int c, const OVTable & ov)
{
	VNVECTOR3UI    predRelation(VD_INVALID_INDEX, VD_INVALID_INDEX, VD_INVALID_INDEX);
 	VFVECTOR3  vPred, delta;
	if (m_vVertFlag[ov.VTable.pData[ov.OTable.pData[c]]] && m_vVertFlag[ov.VTable.pData[PrevCorner(c)]])
	{
		vPred = m_vVertEstimate[ov.VTable.pData[NextCorner(c)]] + m_vVertEstimate[ov.VTable.pData[PrevCorner(c)]];
		vPred -= m_vVertEstimate[ov.VTable.pData[ov.OTable.pData[c]]];
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(vPred);    /////////////
		predRelation.x = ov.VTable.pData[NextCorner(c)];
		predRelation.y = ov.VTable.pData[PrevCorner(c)];
		predRelation.z = ov.VTable.pData[ov.OTable.pData[c]];
		m_vPredRelation.push_back(predRelation);
	}
	else if (m_vVertFlag[ov.VTable.pData[ov.OTable.pData[c]]])
	{
		vPred = m_vVertEstimate[ov.VTable.pData[NextCorner(c)]] * 2;
		vPred -= m_vVertEstimate[ov.VTable.pData[ov.OTable.pData[c]]];
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(vPred);    /////////////
	}
	else if (m_vVertFlag[ov.VTable.pData[NextCorner(c)]] && m_vVertFlag[ov.VTable.pData[PrevCorner(c)]])
	{
		vPred = m_vVertEstimate[ov.VTable.pData[NextCorner(c)]] + m_vVertEstimate[ov.VTable.pData[PrevCorner(c)]];
		vPred *= .5f;
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(vPred);    /////////////
		predRelation.x = ov.VTable.pData[NextCorner(c)];
		predRelation.y = ov.VTable.pData[PrevCorner(c)];
		m_vPredRelation.push_back(predRelation);
	}
	else if (m_vVertFlag[ov.VTable.pData[NextCorner(c)]])
	{
		vPred = m_vVertEstimate[ov.VTable.pData[NextCorner(c)]];
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(vPred);    /////////////
		predRelation.x = ov.VTable.pData[NextCorner(c)];
		m_vPredRelation.push_back(predRelation);
	}
	else if (m_vVertFlag[ov.VTable.pData[PrevCorner(c)]])
	{
		vPred = m_vVertEstimate[ov.VTable.pData[PrevCorner(c)]];
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(vPred);    /////////////
		predRelation.x = ov.VTable.pData[PrevCorner(c)];
		m_vPredRelation.push_back(predRelation);
	}
	else
	{
		vPred = VFVECTOR3::ZERO;
		delta = ov.VertCoord.pData[ov.VTable.pData[c]] - vPred;

		m_vPredCoord.push_back(ov.VertCoord.pData[ov.VTable.pData[c]]);     ////////////
		m_vPredRelation.push_back(predRelation);
	}
	m_vVertEstimate[ov.VTable.pData[c]] = delta + vPred;    

	m_vDelta.push_back(delta);
}

void VBEDCompress::EncodeDelta(int c, const OVTable & ov)
{
	EncodeWithPrediction(c, ov);
}

inline unsigned VBEDCompress::NextCorner(unsigned c)
{
	return (3 * (c / 3) + (c + 1) % 3);
}

inline unsigned VBEDCompress::PrevCorner(unsigned c)
{
	return NextCorner(NextCorner(c));
}

inline unsigned VBEDCompress::RightTri(unsigned c, const OVTable & ov)
{
	//c.r = c.n.r 
	return ov.OTable.pData[NextCorner(c)];
}

inline unsigned VBEDCompress::LeftTri(unsigned c, const OVTable & ov)
{
	//c.l = c.n.n.r 
	return ov.OTable.pData[NextCorner(NextCorner(c))];
}

inline unsigned VBEDCompress::CornerToTri(unsigned c)
{
	return c / 3;
}

//VD_EXPORT_SYSTEM_SIMPLE(VBEDCompress, VNALGMESH::VREDCompress);