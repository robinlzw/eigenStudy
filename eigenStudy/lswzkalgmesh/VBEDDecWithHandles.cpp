#include "stdafx.h"
#include "VBEDDecWithHandles.h"

void VBEDDecWithHandles::InitDecompress(const HandleCmpRes & res)
{
	unsigned sNum = res.cmpRes.len + 1, vNum = res.delta.len;
	m_vOTable.resize(sNum * 3, -3);
	m_vVTable.resize(sNum * 3);
	m_vTriFlag.resize(sNum);
	m_vTriangle.resize(sNum);
	m_vVertFlag.resize(vNum);
	m_vVert.resize(vNum);
	m_vVertD.resize(vNum);
	//m_vRelation.resize(vNum);

	//Initialize the first triangle
	
	if (res.bndryLen == 0)
	{
		m_vVTable[0] = 0, m_vVTable[1] = 2, m_vVTable[2] = 1;
		m_vOTable[0] = -1;
		m_vOTable[1] = -1;
	}
	else
	{
		m_vVTable[0] = 0, m_vVTable[1] = 1, m_vVTable[2] = 2;
		m_vOTable[2] = -1;
	}		

	m_vTriFlag[0] = true;

	triNum = 0;
	vCount = 2;
	hCount = 0;
	EBcount = 0;
	//relation.x = relation.y = relation.z = VD_INVALID_INDEX;

	if (res.bndryLen == 0)
	{
		DecompressConnectivity(res.cmpRes, res.handles, res.bndryLen, 2);
		InitDecompressVertices(res);
		DecompressVertices(res, m_vOTable[2]);
	}
	else
	{
		DecompressConnectivity(res.cmpRes, res.handles, res.bndryLen, 1);
		InitDecompressVertices(res);
		DecompressVertices(res, m_vOTable[1]);
	}
	
	TranVTableToTri();
	if (res.delVertID != VD_INVALID_INDEX)
		DelVertAndSurf(res.delVertID);
	else
	{
		m_vVertOut.resize(m_vVertD.size());
		m_vTriangleOut.resize(m_vTriangle.size());
		memcpy_s(&m_vVertOut.front(), sizeof(DLE3) * m_vVertD.size(), &m_vVertD.front(), sizeof(DLE3) * m_vVertD.size());
		memcpy_s(&m_vTriangleOut.front(), sizeof(VNVECTOR3UI) * m_vTriangle.size(), &m_vTriangle.front(), sizeof(VNVECTOR3UI) * m_vTriangle.size());
	}
}

void VBEDDecWithHandles::DecompressConnectivity(const VSConstBuffer<char>& clers, const VSConstBuffer<VNVECTOR2UI> & handles, unsigned blen, unsigned c)
{
	bndryNum = 0;
	unsigned curCorner = c;
	do
	{
		triNum ++;            //new triangle
		if (triNum > clers.len)      //stop
			return;
        
		m_vOTable[curCorner] = 3 * triNum;                                  //attach new triangle, link opposite corners
		m_vOTable[3 * triNum] = curCorner;
		
		m_vVTable[3 * triNum + 1] = m_vVTable[PrevCorner(curCorner)];       //enter vertex Ids for shared vertices
		m_vVTable[3 * triNum + 2] = m_vVTable[NextCorner(curCorner)];
		
		curCorner = NextCorner(m_vOTable[curCorner]);		                //move corner to new triangle
		if (bndryNum != blen - 2)
		{
			switch (clers.pData[triNum - 1])                                    //select operation based on next symbol
			{
			case 'C':
				m_vOTable[NextCorner(curCorner)] = -1;                          //C: left edge is free, store ref to new vertex	
				m_vVTable[3 * triNum] = ++vCount;
				break;
			case 'L':
				m_vOTable[NextCorner(curCorner)] = -2;                //L: orient free edge

				if (!CheckHandle(handles, NextCorner(curCorner)))     //check for handles, if non, try zip
					Zip(NextCorner(curCorner));
				break;
			case 'R':
				if (blen != 0 && bndryNum < blen - 2)
				{
					m_vOTable[curCorner] = -3;
					m_vVTable[PrevCorner(curCorner)] = ++vCount;
					bndryNum ++;
				}
				else
					m_vOTable[curCorner] = -2;                        //R: orient free edge, check for handles, go left 

				CheckHandle(handles, curCorner);
				curCorner = NextCorner(curCorner);
				break;
			case 'S':
				//S: recursion going right, then go left
				m_sBranch.push(NextCorner(curCorner));
				//if (m_vOTable[NextCorner(curCorner)] > 0)              //if the triangle to the left was visited, then return	
				//{
				//	if (!m_sBranch.empty())
				//	{
				//		curCorner = m_sBranch.top();
				//		m_sBranch.pop();
				//	}
				//	else
				//		return;
				//}
				break;
			case 'E':
				//E: left and right edges are  free
				m_vOTable[curCorner] = -2;
				m_vOTable[NextCorner(curCorner)] = -2;
				//check for handles on the right
				CheckHandle(handles, curCorner);
				//check for handles on the left, if non, try to zip
				if (!CheckHandle(handles, NextCorner(curCorner)))
					Zip(NextCorner(curCorner));
				//pop
				if (!m_sBranch.empty())
				{
					do
					{
						curCorner = m_sBranch.top();
						m_sBranch.pop();
					} while (m_vOTable[curCorner] > 0 && !m_sBranch.empty());
					/*curCorner = m_sBranch.top();
					m_sBranch.pop();*/
				}
				break;
			default:
				break;
			}
		}
		else
			curCorner = SpecialCase(clers, handles, blen, curCorner);
		
	} while (true);
}

unsigned VBEDDecWithHandles::SpecialCase(const VSConstBuffer<char>& clers, const VSConstBuffer<VNVECTOR2UI>& handles, unsigned blen, unsigned c)
{
	switch (clers.pData[triNum - 1])
	{
	case 'C':
		m_vOTable[NextCorner(c)] = -1;
		m_vVTable[3 * triNum] = ++vCount;
		break;
	case 'L':
		m_vOTable[NextCorner(c)] = -2;                //L: orient free edge
		m_vVTable[NextCorner(c)] = vCount;

		if (!CheckHandle(handles, NextCorner(c)))     //check for handles, if non, try zip
			Zip(NextCorner(c));
		break;
	case 'R':
		m_vOTable[c] = -3;
		m_vVTable[c] = vCount;
		m_vVTable[PrevCorner(c)] = 1;
		bndryNum++;

		CheckHandle(handles, c);
		c = NextCorner(c);
		break;
	case 'S':
		m_vVTable[PrevCorner(c)] = 0;

		m_sBranch.push(NextCorner(c));
		break;
	case 'E':
		m_vOTable[c] = -3;
		m_vOTable[NextCorner(c)] = 2;
		m_vVTable[PrevCorner(c)] = 1;
		bndryNum++;
		//check for handles on the right
		CheckHandle(handles, c);
		//check for handles on the left, if non, try to zip
		if (!CheckHandle(handles, NextCorner(c)))
			Zip(NextCorner(c));
		//pop
		if (!m_sBranch.empty())
		{
			c = m_sBranch.top();
			m_sBranch.pop();
		}
		break;
	default:
		break;
	}
		
	return c;
}

bool VBEDDecWithHandles::CheckHandle(const VSConstBuffer<VNVECTOR2UI> & handles, unsigned c)
{
	//check if this is a handle
	if (hCount >= handles.len || c != handles.pData[hCount].y)
		return false;
	else
	{
		//link opposite corners
		m_vOTable[c] = handles.pData[hCount].x;
		m_vOTable[handles.pData[hCount].x] = c;

		//find corner of next free edge if any 
		int temp = PrevCorner(c);
		while ((m_vOTable[temp] >= 0) && (temp != handles.pData[hCount].x))
			temp = PrevCorner(m_vOTable[temp]);

		//zip if found cw edge
		if (m_vOTable[temp] == -2)
			Zip(temp);

		//find corner of next free edge if any
		temp = PrevCorner(m_vOTable[c]);
		while ((m_vOTable[temp] >= 0) && (temp != c))
			temp = PrevCorner(m_vOTable[temp]);

		//zip if found cw edge
		if (m_vOTable[temp] == -2)
			Zip(temp);

		//next handle
		hCount ++;
		return true;
	}
}

void VBEDDecWithHandles::Zip(unsigned c)
{
	do
	{
		//tries to zip free edges opposite c
		int temp = NextCorner(c);

		//search clockwise for free edge
		while (m_vOTable[temp] >= 0 && m_vOTable[temp] != c)
			temp = NextCorner(m_vOTable[temp]);

		//pop if no zip possible
		if (m_vOTable[temp] != -1)
			return;

		//link opposite corners
		m_vOTable[temp] = c;
		m_vOTable[c] = temp;

		//assign co-incident corners
		int tempV = NextCorner(c);
		m_vVTable[NextCorner(tempV)] = m_vVTable[NextCorner(temp)];
		while (m_vOTable[tempV] >= 0 && tempV != temp)
		{
			tempV = NextCorner(m_vOTable[tempV]);
			m_vVTable[NextCorner(tempV)] = m_vVTable[NextCorner(temp)];
		}

		//find corner of next free edge on right
		c = PrevCorner(c);
		while (m_vOTable[c] >= 0 && c != temp)
			c = PrevCorner(m_vOTable[c]);
		//try to zip again
	} while (m_vOTable[c] == -2);
}

void VBEDDecWithHandles::InitDecompressVertices(const HandleCmpRes & res)
{
	if (res.bndryLen == 0)
	{
		m_vVert[0] = DecodeDelta(res.delta, 0);
		m_vVertD[0] = DecodeDeltaD(res.deltaD, 0);   ////////
		m_vVertFlag[0] = true;
		//m_vRelation[0] = relation;
		EBcount++;

		m_vVert[1] = DecodeDelta(res.delta, 2);
		m_vVertD[1] = DecodeDeltaD(res.deltaD, 2);    ///////////
		m_vVertFlag[1] = true;		
		//m_vRelation[1] = relation;           
		EBcount++;

		m_vVert[2] = DecodeDelta(res.delta, 1);
		m_vVertD[2] = DecodeDeltaD(res.deltaD, 1);
		m_vVertFlag[2] = true;
		//m_vRelation[2] = relation;        
		EBcount++;
	}
	else
	{
		//delta compensation
		//m_vDecBndry.resize(res.bndryLen);
		unsigned curCorner = 0;

		//the second vertex
		EBcount++;
		m_vVert[EBcount] = res.bndryVt.pData[EBcount - 1];
		//////
		m_vVertD[EBcount].x = res.bndryVt.pData[EBcount - 1].x;
		m_vVertD[EBcount].y = res.bndryVt.pData[EBcount - 1].y;
		m_vVertD[EBcount].z = res.bndryVt.pData[EBcount - 1].z;
		//////
		m_vVertFlag[NextCorner(curCorner)] = true;
		//m_vRelation[EBcount - 1] = relation;
		//m_vDecBndry[EBcount - 1] = 1;		

		//the third vertex
		EBcount++;
		m_vVert[NextCorner(NextCorner(curCorner))] = res.bndryVt.pData[EBcount - 1]; 
		//////
		m_vVertD[NextCorner(NextCorner(curCorner))].x = res.bndryVt.pData[EBcount - 1].x;
		m_vVertD[NextCorner(NextCorner(curCorner))].y = res.bndryVt.pData[EBcount - 1].y;
		m_vVertD[NextCorner(NextCorner(curCorner))].z = res.bndryVt.pData[EBcount - 1].z;
		//////
		m_vVertFlag[NextCorner(NextCorner(curCorner))] = true;		
		//m_vRelation[EBcount - 1] = relation;
		//m_vDecBndry[EBcount - 1] = 2;                        		

		curCorner = m_vOTable[NextCorner(curCorner)];
		while (EBcount < res.bndryLen)
		{
			while (m_vOTable[NextCorner(curCorner)] > 0)
				curCorner = m_vOTable[NextCorner(curCorner)];

			EBcount++;
			m_vVert[m_vVTable[curCorner]] = res.bndryVt.pData[EBcount - 1]; 
			//////
			m_vVertD[m_vVTable[curCorner]].x = res.bndryVt.pData[EBcount - 1].x;
			m_vVertD[m_vVTable[curCorner]].y = res.bndryVt.pData[EBcount - 1].y;
			m_vVertD[m_vVTable[curCorner]].z = res.bndryVt.pData[EBcount - 1].z;
			//////
			m_vVertFlag[m_vVTable[curCorner]] = true;
			//m_vRelation[EBcount - 1] = relation;
			//m_vDecBndry[EBcount - 1] = m_vVTable[curCorner];			

			curCorner = NextCorner(curCorner);
		} 
		EBcount = res.bndryLen;

		//the first vertex		
		m_vVert[0] = DecodeDelta(res.delta, 0);
		//////
		m_vVertD[0] = DecodeDeltaD(res.deltaD, 0);
		//////
		m_vVertFlag[0] = true;
		//m_vRelation[EBcount] = relation;                  //////////	
		EBcount++;
	}	
}

void VBEDDecWithHandles::DecompressVertices(const HandleCmpRes & res, unsigned c)
{
	do
	{
		m_vTriFlag[CornerToTri(c)] = true;

		//test whether tip vertex was visited
		if (!m_vVertFlag[m_vVTable[c]])
		{
			m_vVert[m_vVTable[c]] = DecodeDelta(res.delta, c);
			m_vVertD[m_vVTable[c]] = DecodeDeltaD(res.deltaD, c);
			m_vVertFlag[m_vVTable[c]] = true;
			//m_vRelation[EBcount] = relation;
			EBcount++;
			c = RightTriCorner(c);
		}
		else
		{
			//test whether right triangle was visited
			if (RightTriCorner(c) == -3 || m_vTriFlag[CornerToTri(RightTriCorner(c))])
			{
				if (m_vTriFlag[CornerToTri(LeftTriCorner(c))])
				{
					if (m_sBranch.empty())                                      //terminate
						return;

					do
					{
						c = m_sBranch.top();                            //left branch of 'S'
						m_sBranch.pop();                                //pop stack
					} while (m_vTriFlag[CornerToTri(c)] && !m_sBranch.empty());           //left triangle is visited
				}
				else
					c = LeftTriCorner(c);  //R,move to left triangle
			}
			else
			{
				//test whether left triangle was visited
				if (m_vTriFlag[CornerToTri(LeftTriCorner(c))])
					c = RightTriCorner(c);       //L, move to right triangle
				else
				{
					//S, recursive call to visit right branch first
					m_sBranch.push(LeftTriCorner(c));    //push left triangle to stack

					c = RightTriCorner(c);
				}
			}
		}

	} while (true);
}

VFVECTOR3 VBEDDecWithHandles::DecodeDelta(const VSConstBuffer<VFVECTOR3> & res, unsigned curCorner)
{
	VFVECTOR3  vPred(0.f, 0.f, 0.f), delta = res.pData[EBcount];
	//relation.x = relation.y = relation.z = VD_INVALID_INDEX;

	if (m_vOTable[curCorner] != -3 && m_vVertFlag[m_vVTable[m_vOTable[curCorner]]] && m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])  //case 1
	{
		vPred = m_vVert[m_vVTable[NextCorner(curCorner)]] + 
			    m_vVert[m_vVTable[PrevCorner(curCorner)]] - 
			    m_vVert[m_vVTable[m_vOTable[curCorner]]];
		vPred += delta;

		////////
		//relation.x = m_vVTable[NextCorner(curCorner)];
		//relation.y = m_vVTable[PrevCorner(curCorner)];
		//relation.z = m_vVTable[m_vOTable[curCorner]];

		return vPred;
	}	
	else if (m_vOTable[curCorner] > 0 && m_vVertFlag[m_vVTable[m_vOTable[curCorner]]])   //case 2
	{
		vPred = m_vVert[m_vVTable[NextCorner(curCorner)]] * 2.f  - m_vVert[m_vVTable[m_vOTable[curCorner]]];
		vPred += delta;

		//////
		//relation.x = m_vVTable[NextCorner(curCorner)];
		//relation.y = m_vVTable[m_vOTable[curCorner]];

		return vPred;
	}
	else if (m_vVertFlag[m_vVTable[NextCorner(curCorner)]] && m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])   //case 3
	{
		vPred = (m_vVert[m_vVTable[NextCorner(curCorner)]] + m_vVert[m_vVTable[PrevCorner(curCorner)]]) * .5f;
		vPred += delta;

		//////
		//relation.x = m_vVTable[NextCorner(curCorner)];
		//relation.y = m_vVTable[PrevCorner(curCorner)];

		return vPred;
	}
	else if (m_vVertFlag[m_vVTable[NextCorner(curCorner)]])      //case 4
	{
		vPred = m_vVert[m_vVTable[NextCorner(curCorner)]];
		vPred += delta;

		//////
		//relation.x = m_vVTable[NextCorner(curCorner)];

		return vPred;
	}
	else if (m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])      //case 5
	{
		vPred = m_vVert[m_vVTable[PrevCorner(curCorner)]];
		vPred += delta;

		//////
		//relation.x = m_vVTable[PrevCorner(curCorner)];

		return vPred;
	}
	else                                                         //case 6
	{
		vPred = delta;

		return vPred;
	}
}

void VBEDDecWithHandles::TranVTableToTri()
{
	for (unsigned i = 0; i < m_vTriangle.size(); i++)
	{
		m_vTriangle[i].x = m_vVTable[i * 3];
		m_vTriangle[i].y = m_vVTable[i * 3 + 1];
		m_vTriangle[i].z = m_vVTable[i * 3 + 2];
	}
}

DLE3 VBEDDecWithHandles::DecodeDeltaD(const VSConstBuffer<DLE3>& res, unsigned curCorner)
{
	DLE3 deltaD = res.pData[EBcount];
	DLE3 vPredD;

	if (m_vOTable[curCorner] != -3 && m_vVertFlag[m_vVTable[m_vOTable[curCorner]]] && m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])  //case 1
	{
		vPredD.x = m_vVertD[m_vVTable[NextCorner(curCorner)]].x + m_vVertD[m_vVTable[PrevCorner(curCorner)]].x - m_vVertD[m_vVTable[m_vOTable[curCorner]]].x;
		vPredD.y = m_vVertD[m_vVTable[NextCorner(curCorner)]].y + m_vVertD[m_vVTable[PrevCorner(curCorner)]].y - m_vVertD[m_vVTable[m_vOTable[curCorner]]].y;
		vPredD.z = m_vVertD[m_vVTable[NextCorner(curCorner)]].z + m_vVertD[m_vVTable[PrevCorner(curCorner)]].z - m_vVertD[m_vVTable[m_vOTable[curCorner]]].z;
		vPredD = AddD(deltaD, vPredD);
		return vPredD;
	}
	else if (m_vOTable[curCorner] > 0 && m_vVertFlag[m_vVTable[m_vOTable[curCorner]]])   //case 2
	{
		vPredD.x = m_vVertD[m_vVTable[NextCorner(curCorner)]].x * 2. - m_vVertD[m_vVTable[m_vOTable[curCorner]]].x;
		vPredD.y = m_vVertD[m_vVTable[NextCorner(curCorner)]].y * 2. - m_vVertD[m_vVTable[m_vOTable[curCorner]]].y;
		vPredD.z = m_vVertD[m_vVTable[NextCorner(curCorner)]].z * 2. - m_vVertD[m_vVTable[m_vOTable[curCorner]]].z;

		vPredD = AddD(deltaD, vPredD);
		return vPredD;
	}
	else if (m_vVertFlag[m_vVTable[NextCorner(curCorner)]] && m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])   //case 3
	{
		vPredD.x = (m_vVertD[m_vVTable[NextCorner(curCorner)]].x + m_vVertD[m_vVTable[PrevCorner(curCorner)]].x) * .5;
		vPredD.y = (m_vVertD[m_vVTable[NextCorner(curCorner)]].y + m_vVertD[m_vVTable[PrevCorner(curCorner)]].y) * .5;
		vPredD.z = (m_vVertD[m_vVTable[NextCorner(curCorner)]].z + m_vVertD[m_vVTable[PrevCorner(curCorner)]].z) * .5;

		vPredD = AddD(deltaD, vPredD);
		return vPredD;
	}
	else if (m_vVertFlag[m_vVTable[NextCorner(curCorner)]])      //case 4
	{
		vPredD = m_vVertD[m_vVTable[NextCorner(curCorner)]];
		vPredD = AddD(deltaD, vPredD);
		return vPredD;
	}
	else if (m_vVertFlag[m_vVTable[PrevCorner(curCorner)]])      //case 5
	{
		vPredD = m_vVertD[m_vVTable[PrevCorner(curCorner)]];
		vPredD = AddD(deltaD, vPredD);
		return vPredD;
	}
	else                                                         //case 6
	{
		vPredD = deltaD;

		return vPredD;
	}
}

DLE3 VBEDDecWithHandles::AddD(const DLE3 & delta, const DLE3 & pred)
{
	DLE3 temp;
	temp.x = delta.x + pred.x;
	temp.y = delta.y + pred.y;
	temp.z = delta.z + pred.z;
	return temp;
}

inline unsigned VBEDDecWithHandles::NextCorner(unsigned c)
{
	return (3 * (c / 3) + (c + 1) % 3);
}

inline unsigned VBEDDecWithHandles::PrevCorner(unsigned c)
{
	return NextCorner(NextCorner(c));
}

inline unsigned VBEDDecWithHandles::RightTriCorner(unsigned c)
{
	//c.r = c.n.r 
	return m_vOTable[NextCorner(c)];
}

inline unsigned VBEDDecWithHandles::LeftTriCorner(unsigned c)
{
	//c.l = c.n.n.r 
	return m_vOTable[NextCorner(NextCorner(c))];
}

inline unsigned VBEDDecWithHandles::CornerToTri(unsigned c)
{
	return c / 3;
}

void VBEDDecWithHandles::DelVertAndSurf(const unsigned & delID)
{
	if (delID == VD_INVALID_INDEX)
		return;

	//m_vVertTemp.resize(m_vVertD.size());
	//m_vTriangleTemp.resize(m_vTriangle.size());
	//memcpy_s(&m_vVertTemp.front(), sizeof(DLE3) * m_vVertD.size(), &m_vVertD.front(), sizeof(DLE3) * m_vVertD.size());
	//memcpy_s(&m_vTriangleTemp.front(), sizeof(VNVECTOR3UI) * m_vTriangle.size(), &m_vTriangle.front(), sizeof(VNVECTOR3UI) * m_vTriangle.size());

	std::vector<bool>   m_vFlag;
	m_vVertOut.resize(m_vVertD.size() - 1);
	if (delID == 0)
	{
		memcpy_s(&m_vVertOut.front(), sizeof(DLE3) * m_vVertOut.size(), &m_vVertD[1], sizeof(DLE3) * m_vVertOut.size());
	}
	else
	{
		memcpy_s(&m_vVertOut.front(), sizeof(DLE3) * delID, &m_vVertD.front(), sizeof(DLE3) * delID);
		memcpy_s(&m_vVertOut[delID], sizeof(DLE3) * (m_vVertOut.size() - delID), &m_vVertD[delID + 1], sizeof(DLE3) * (m_vVertOut.size() - delID));
	}

	unsigned count = 0;
	m_vFlag.resize(m_vTriangle.size());
	for (unsigned i = 0; i < m_vTriangle.size(); i++)
	{
		if ((m_vTriangle[i].x == delID) || (m_vTriangle[i].y == delID) || (m_vTriangle[i].z == delID))
		{
			m_vFlag[i] = true;
			count++;
		}
	}
	m_vTriangleOut.resize(m_vTriangle.size() - count);
	count = 0;
	for (unsigned i = 0; i < m_vTriangle.size(); i++)
	{
		if (!m_vFlag[i])
		{
			for (unsigned j = 0; j < 3; j++)
			{
				if (m_vTriangle[i][j] > delID)
					m_vTriangleOut[count][j] = m_vTriangle[i][j] - 1;
				else
					m_vTriangleOut[count][j] = m_vTriangle[i][j];
			}
			count++;
		}
	}
}
