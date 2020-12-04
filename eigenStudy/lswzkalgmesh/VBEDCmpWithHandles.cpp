#include "stdafx.h"
#include "VBEDCmpWithHandles.h"

unsigned VBEDCmpWithHandles::StartCorner(const OVTable & ov, unsigned startVid)
{
	for (size_t i = 0; i < ov.OTable.len; i++)
	{
		if (ov.OTable.pData[i] == VD_INVALID_INDEX && ov.VTable.pData[NextCorner(i)] == startVid)
			return i;
	}

	return VD_INVALID_INDEX;
}

void VBEDCmpWithHandles::InitCompress(const OVTable & ov, const VNWZKALG::VSHoleBoundary & Bndry, const unsigned & startC)
{
	triNum = 0, vCount = 0, mapVCount = 0;
	m_vTriFlag.resize(ov.OTable.len / 3);
	m_vVertFlag.resize(ov.vNum);
	//m_vPredictionVert.resize(ov.vNum - 1);
	m_vPredictionVert.resize(ov.vNum);
	m_vClers.resize(ov.OTable.len / 3 - 1);     //the first triangle is removed
	m_vVert.resize(ov.vNum);
	m_vDeltaD.resize(ov.vNum);
	m_vVtMap.resize(ov.vNum);
	m_vTriMap.resize(ov.OTable.len / 3);
	m_vRelation.resize(ov.vNum);
	m_vRelateTemp.resize(ov.vNum);
	m_vVtMapWithBndry.resize(ov.vNum);	
	m_vOldV2NewV.resize(ov.vNum);
	m_vOldTri2NewTri.resize(ov.OTable.len / 3);
	
	//startCorner
	unsigned startCorner = startC;
	unsigned t = 0;
	if (startC == VD_INVALID_INDEX)
	{
		bndryNum = Bndry.bndry.pData[0].orderedVert.len;
		m_vDecBndryVtID.resize(bndryNum);

		startCorner = StartCorner(ov, Bndry.bndry.pData[0].orderedVert.pData[0]);
		BndryVertPrediction(Bndry, ov.VertCoord, ov);

		vCount++;
		EncodeWithPrediction(startCorner, ov);				
		m_vVertFlag[ov.VTable.pData[startCorner]] = true;
		m_vVert[0] = ov.VertCoord.pData[ov.VTable.pData[startCorner]];
		m_vVtMapWithBndry[0] = ov.VTable.pData[startCorner];
		m_vVtMap[vCount] = ov.VTable.pData[startCorner];
		m_vRelateTemp[vCount] = relation;

		m_vTriFlag[CornerToTri(startCorner)] = true;            //flag the first triangle is visited
		m_vTriMap[triNum] = CornerToTri(startCorner);

		Compress(ov, ov.OTable.pData[NextCorner(startCorner)]);
		
		VFVECTOR3 temp(0.f, 0.f, 0.f);
		for (unsigned i = 0; i < bndryNum; i++)
			m_vPredictionVert[i] = temp;
	}
	else
	{		
		//vert = ov.VertCoord.pData[ov.VTable.pData[startCorner]];    //the first vertex
		EncodeWithPrediction(startCorner, ov);
		m_vVertFlag[ov.VTable.pData[startCorner]] = true;
		//m_vVert[0] = vert;
		m_vVtMap[0] = ov.VTable.pData[startCorner];
		m_vRelateTemp[0] = relation;
		
		unsigned c = PrevCorner(startCorner);
		//unsigned c = NextCorner(startCorner);

		vCount++;
		//estimate third vertex
		EncodeWithPrediction(c, ov);		
		m_vVertFlag[ov.VTable.pData[c]] = true;
		m_vVert[1] = ov.VertCoord.pData[ov.VTable.pData[c]];
		m_vVtMap[1] = ov.VTable.pData[c];
		m_vRelateTemp[1] = relation;

		vCount++;
		//estimate second vertex
		EncodeWithPrediction(PrevCorner(c), ov);		
		m_vVertFlag[ov.VTable.pData[PrevCorner(c)]] = true;
		m_vVert[2] = ov.VertCoord.pData[ov.VTable.pData[PrevCorner(c)]];
		m_vVtMap[2] = ov.VTable.pData[PrevCorner(c)];
		m_vRelateTemp[2] = relation;

		// mark the triangle as visited
		m_vTriFlag[CornerToTri(c)] = 1;
		m_vTriMap[triNum] = CornerToTri(startCorner);
		triNum++;

		vCount = 2;

		unsigned temp = ov.OTable.pData[c];
		while (temp != PrevCorner(ov.OTable.pData[PrevCorner(c)]))
		{
			m_vClers[triNum - 1] = 'C';
			triNum++;
			m_vTriFlag[CornerToTri(temp)] = 1;
			m_vTriMap[triNum - 1] = CornerToTri(temp);

			vCount++;
			EncodeWithPrediction(temp, ov);						
			m_vVertFlag[ov.VTable.pData[temp]] = true;						
			m_vVtMap[vCount] = ov.VTable.pData[temp];
			m_vRelateTemp[vCount] = relation;

			temp = ov.OTable.pData[NextCorner(temp)];
		}
		m_vClers[triNum - 1] = 'R';
		m_vTriFlag[CornerToTri(temp)] = 1;
		m_vTriMap[triNum] = CornerToTri(temp);

		bndryNum = 0;
		mapVCount = 2;

		Compress(ov, ov.OTable.pData[PrevCorner(temp)]);
	}

	RelationMap();
}

void VBEDCmpWithHandles::Compress(const OVTable & ov, unsigned corner)
{
	unsigned curCorner = corner, Rcount = 0;
	do
	{		
		m_vTriFlag[CornerToTri(curCorner)] = 1;          //mark the triangle as visited
		triNum ++;
		m_vTriMap[triNum] = CornerToTri(curCorner);

		CheckHandle(curCorner, ov);                      //check handle

		if (!m_vVertFlag[ov.VTable.pData[curCorner]])    //current vertex is unvisited
		{
			m_vClers[triNum - 1] = 'C';

			unsigned id = ov.VTable.pData[curCorner];

			vCount++;
			EncodeWithPrediction(curCorner, ov);						
			m_vVtMap[vCount] = ov.VTable.pData[curCorner];
			m_vRelateTemp[vCount] = relation;

			mapVCount++;
			m_vVert[mapVCount] = ov.VertCoord.pData[ov.VTable.pData[curCorner]];
			m_vVtMapWithBndry[mapVCount] = ov.VTable.pData[curCorner];
			
			m_vVertFlag[ov.VTable.pData[curCorner]] = true;			
			curCorner = RightTriCorner(curCorner, ov);			
		}
		else                                             //current vertex is visited
		{
			if (RightTriCorner(curCorner, ov) == VD_INVALID_INDEX || m_vTriFlag[CornerToTri(RightTriCorner(curCorner, ov))] > 0)     //right triangle is visited
			{
				if (RightTriCorner(curCorner, ov) == VD_INVALID_INDEX)
				{
					if (Rcount < bndryNum - 2)
					{
						mapVCount++;
						m_vVert[mapVCount] = ov.VertCoord.pData[ov.VTable.pData[curCorner]];
						m_vVtMapWithBndry[mapVCount] = ov.VTable.pData[curCorner];
						m_vDecBndryVtID[decBndryTemp] = mapVCount;    //解压时边界点
						decBndryTemp++;
					}					
				}
				if (m_vTriFlag[CornerToTri(LeftTriCorner(curCorner, ov))] > 0)   //left triangle is visited
				{
					m_vClers[triNum - 1] = 'E';

					if (m_sBranch.empty())                                      //terminate
						return;												    

					do														    
					{	
						if (m_sBranch.empty())                                      //terminate
							return;

						curCorner = m_sBranch.top();                            //left branch of 'S'
						m_sBranch.pop();                                        //pop stack
					} while (m_vTriFlag[CornerToTri(curCorner)] > 0);           //left triangle is visited
				}
				else
				{
					m_vClers[triNum - 1] = 'R';
					curCorner = LeftTriCorner(curCorner, ov);
					Rcount++; /////////////
				}				
			}
			else                                                                 //right triangle is unvisited
			{
				if (m_vTriFlag[CornerToTri(LeftTriCorner(curCorner, ov))] > 0)   //left triangle is visited
				{
					m_vClers[triNum - 1] = 'L';
					curCorner = RightTriCorner(curCorner, ov);
				}
				else
				{
					m_vClers[triNum - 1] = 'S';
					m_vTriFlag[CornerToTri(curCorner)] = 3 * triNum + 2;
					m_sBranch.push(ov.OTable.pData[PrevCorner(curCorner)]);

					curCorner = RightTriCorner(curCorner, ov);					
				}
			}
		}
	} while (true);
}

void VBEDCmpWithHandles::CheckHandle(unsigned curCorner, const OVTable & ov)
{
	//check for handles from the right
	if (ov.OTable.pData[NextCorner(curCorner)] != VD_INVALID_INDEX && m_vTriFlag[CornerToTri(ov.OTable.pData[NextCorner(curCorner)])] > 1)
	{
		//write opposite corners for handle triangles into file
		m_vHandle.push_back(VNVECTOR2UI(m_vTriFlag[CornerToTri(ov.OTable.pData[NextCorner(curCorner)])], 3 * triNum + 1));
	}

	//check for handles from the left
	if (ov.OTable.pData[PrevCorner(curCorner)] != VD_INVALID_INDEX && m_vTriFlag[CornerToTri(ov.OTable.pData[PrevCorner(curCorner)])] > 1)
	{
		//write opposite corners for handle triangles into file
		m_vHandle.push_back(VNVECTOR2UI(m_vTriFlag[CornerToTri(ov.OTable.pData[PrevCorner(curCorner)])], 3 * triNum + 2));
	}
}

void VBEDCmpWithHandles::BndryVertPrediction(const VNWZKALG::VSHoleBoundary & Bndry, const VSConstBuffer<VFVECTOR3>& vt, const OVTable & ov)
{	
	//start from the first boundary or only one boundary
	m_vBndryVt.resize(Bndry.bndry.pData[0].orderedVert.len);
	relation.x = relation.y = relation.z = VD_INVALID_INDEX;
	decBndryTemp = 0;

	//the first vertex
	unsigned vid = Bndry.bndry.pData[0].orderedVert.pData[0];
	m_vVertFlag[vid] = true;
	m_vVtMap[vCount] = vid;	
	m_vPredictionVert[vCount] = vt.pData[vid];
	m_vVert[1] = vt.pData[vid];
	m_vVtMapWithBndry[1] = vid;
	m_vBndryVt[vCount] = vt.pData[vid];
	m_vDecBndryVtID[decBndryTemp] = 1;         //解压时起始边界点
	m_vRelateTemp[vCount] = relation;
	decBndryTemp ++;

	for (unsigned i = Bndry.bndry.pData[0].orderedVert.len - 1; i > 0; i--)
	{
		vCount++;
		vid = Bndry.bndry.pData[0].orderedVert.pData[i];
		m_vPredictionVert[vCount] = vt.pData[vid];
		m_vBndryVt[vCount] = vt.pData[vid];
		m_vVertFlag[vid] = true;
		m_vVtMap[vCount] = vid;		
		m_vRelateTemp[vCount] = relation;
	}
	
	vid = Bndry.bndry.pData[0].orderedVert.pData[Bndry.bndry.pData[0].orderedVert.len - 1];
	m_vVert[2] = vt.pData[vid];
	m_vVtMapWithBndry[2] = vid;
	m_vDecBndryVtID[decBndryTemp] = 2;    //解压时第二个边界点
	decBndryTemp ++;
	mapVCount = 2;
}

void VBEDCmpWithHandles::EncodeWithPrediction(unsigned curCorner, const OVTable & ov)
{
	VFVECTOR3  vPred(0.f, 0.f, 0.f), delta(0.f, 0.f, 0.f);
	VFVECTOR3  tempA(0.f, 0.f, 0.f), tempB(0.f, 0.f, 0.f), tempC(0.f, 0.f, 0.f), tempOrg(0.f, 0.f, 0.f);
	relation.x = relation.y = relation.z = VD_INVALID_INDEX;

	if (ov.OTable.pData[curCorner] != VD_INVALID_INDEX && m_vVertFlag[ov.VTable.pData[ov.OTable.pData[curCorner]]] && m_vVertFlag[ov.VTable.pData[PrevCorner(curCorner)]])   //case 1
	{
		tempA = ov.VertCoord.pData[ov.VTable.pData[NextCorner(curCorner)]];
		tempB = ov.VertCoord.pData[ov.VTable.pData[PrevCorner(curCorner)]];
		tempC = ov.VertCoord.pData[ov.VTable.pData[ov.OTable.pData[curCorner]]];
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];

		relation.x = ov.VTable.pData[NextCorner(curCorner)];
		relation.y = ov.VTable.pData[PrevCorner(curCorner)];
		relation.z = ov.VTable.pData[ov.OTable.pData[curCorner]];

		vPred = tempA + tempB - tempC;
		delta = tempOrg - vPred;
	}
	else if (ov.OTable.pData[curCorner] != VD_INVALID_INDEX && m_vVertFlag[ov.VTable.pData[ov.OTable.pData[curCorner]]])    //case 2
	{
		tempA = ov.VertCoord.pData[ov.VTable.pData[NextCorner(curCorner)]];
		tempB = ov.VertCoord.pData[ov.VTable.pData[ov.OTable.pData[curCorner]]];
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];

		relation.x = ov.VTable.pData[NextCorner(curCorner)];
		relation.y = ov.VTable.pData[ov.OTable.pData[curCorner]];

		vPred = tempA * 2.f - tempB;
		delta = tempOrg - vPred;
	}
	else if (m_vVertFlag[ov.VTable.pData[NextCorner(curCorner)]] && m_vVertFlag[ov.VTable.pData[PrevCorner(curCorner)]])   //case 3
	{
		tempA = ov.VertCoord.pData[ov.VTable.pData[NextCorner(curCorner)]];
		tempB = ov.VertCoord.pData[ov.VTable.pData[PrevCorner(curCorner)]];
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];

		relation.x = ov.VTable.pData[NextCorner(curCorner)];
		relation.y = ov.VTable.pData[PrevCorner(curCorner)];

		vPred = (tempA + tempB) * .5f;
		delta = tempOrg - vPred;
	}
	else if (m_vVertFlag[ov.VTable.pData[NextCorner(curCorner)]])        //case 4
	{
		tempA = ov.VertCoord.pData[ov.VTable.pData[NextCorner(curCorner)]];
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];

		relation.x = ov.VTable.pData[NextCorner(curCorner)];

		vPred = tempA;
		delta = tempOrg - vPred;
	}
	else if (m_vVertFlag[ov.VTable.pData[PrevCorner(curCorner)]])      //case 5
	{
		tempA = ov.VertCoord.pData[ov.VTable.pData[PrevCorner(curCorner)]];
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];

		relation.x = ov.VTable.pData[PrevCorner(curCorner)];

		vPred = tempA;
		delta = tempOrg - vPred;
	}
	else     //case 6
	{
		tempOrg = ov.VertCoord.pData[ov.VTable.pData[curCorner]];
		delta = tempOrg;		
	}

	m_vPredictionVert[vCount] = delta;
}

void VBEDCmpWithHandles::RelationMap()
{
	if (bndryNum > 0)
	{
		for (unsigned i = 0; i < m_vVtMapWithBndry.size(); i++)
		{
			m_vOldMapNewVt[m_vVtMapWithBndry[i]] = i;
			m_vOldV2NewV[m_vVtMapWithBndry[i]] = i;    //UC2C
		}

	}
	else
	{
		for (unsigned i = 0; i < m_vVtMap.size(); i++)
		{
			m_vOldMapNewVt[m_vVtMap[i]] = i;
			m_vOldV2NewV[m_vVtMap[i]] = i;      //UC2C
		}
	}

	for (unsigned i = 0; i < m_vTriMap.size(); i++)
	{
		m_vOldTri2NewTri[m_vTriMap[i]] = i;    //UC2C
	}
	

	for (unsigned i = 0; i < m_vRelateTemp.size(); i++)
	{
		if (m_vRelateTemp[i].x == VD_INVALID_INDEX)
			m_vRelation[i] = m_vRelateTemp[i];
		else if (m_vRelateTemp[i].y == VD_INVALID_INDEX)
		{
			m_vRelation[i].x = m_vOldMapNewVt[m_vRelateTemp[i].x];
			m_vRelation[i].y = m_vRelateTemp[i].y;
			m_vRelation[i].z = m_vRelateTemp[i].z;
		}
		else if (m_vRelateTemp[i].z == VD_INVALID_INDEX)
		{
			m_vRelation[i].x = m_vOldMapNewVt[m_vRelateTemp[i].x];
			m_vRelation[i].y = m_vOldMapNewVt[m_vRelateTemp[i].y];
			m_vRelation[i].z = m_vRelateTemp[i].z;
		}
		else
		{
			m_vRelation[i].x = m_vOldMapNewVt[m_vRelateTemp[i].x];
			m_vRelation[i].y = m_vOldMapNewVt[m_vRelateTemp[i].y];
			m_vRelation[i].z = m_vOldMapNewVt[m_vRelateTemp[i].z];
		}
	}
}

inline unsigned VBEDCmpWithHandles::NextCorner(unsigned c)
{
	return (3 * (c / 3) + (c + 1) % 3);
}

inline unsigned VBEDCmpWithHandles::PrevCorner(unsigned c)
{
	return NextCorner(NextCorner(c));
}

inline unsigned VBEDCmpWithHandles::RightTriCorner(unsigned c, const OVTable & ov)
{
	//c.r = c.n.r 
	return ov.OTable.pData[NextCorner(c)];
}

inline unsigned VBEDCmpWithHandles::LeftTriCorner(unsigned c, const OVTable & ov)
{
	//c.l = c.n.n.r 
	return ov.OTable.pData[NextCorner(NextCorner(c))];
}

inline unsigned VBEDCmpWithHandles::CornerToTri(unsigned c)
{
	return c / 3;
}