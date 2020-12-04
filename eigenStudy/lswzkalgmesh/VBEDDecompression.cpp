#include "stdafx.h"
#include "VBEDDecompression.h"

void VBEDDecompression::MemberInit()
{
	sCount = 0;
	vid = 0;
	EBVcount = 0;

	m_vTriangle.clear();
	m_vVert.clear();
	m_vVertMapID.clear();
	m_vVTable.clear();
	m_vOTable.clear();
	m_vTriFlag.clear();
	m_vVertFlag.clear();
}

void VBEDDecompression::Decompress(const CLERS & cmp)
{
	InitConnectivity(cmp);
	DecompressConnectivity(cmp, 2);
	InitDecompressVertices(cmp);
	DecompressVertices(m_vOTable[2], cmp);
	TranVTableToTri();

	m_vVTable.clear();
	m_vOTable.clear();
	m_vTriFlag.clear();
	m_vVertFlag.clear();
}

void VBEDDecompression::TranVTableToTri()
{
	for (unsigned i = 0; i < m_vTriangle.size(); i ++)
	{
		m_vTriangle[i].x = m_vVTable[i * 3];
		m_vTriangle[i].y = m_vVTable[i * 3 + 1];
		m_vTriangle[i].z = m_vVTable[i * 3 + 2];
	}
}

void VBEDDecompression::InitConnectivity(const CLERS & cmp)
{
	unsigned sNum = cmp.clers.len + 1;
	m_vOTable.resize(3 * sNum, -3);
	m_vVTable.resize(3 * sNum);
	m_vTriFlag.resize(sNum);
	m_vTriangle.resize(sNum);
	m_vVert.resize(cmp.VertDelta.len);
	m_vVertD.resize(cmp.VertDelta.len);      ///////////////////
	m_vVertFlag.resize(cmp.VertDelta.len);
	m_vVertMapID.reserve(cmp.VertDelta.len);

	//Initialize the first triangle 
	m_vVTable[0] = 0;
	m_vVTable[1] = 2;
	m_vVTable[2] = 1;
	m_vOTable[0] = -1;
	m_vOTable[1] = -1;

	vid = 2;
}

void VBEDDecompression::DecompressConnectivity(const CLERS & cmp, unsigned c)
{	
	do
	{
		sCount ++;
		m_vOTable[c] = 3 * sCount;  //attach new triangle, link opposite corners
		m_vOTable[3 * sCount] = c;

		m_vVTable[3 * sCount + 1] = m_vVTable[PrevCorner(c)];    //enter vertex Ids for shared vertices
		m_vVTable[3 * sCount + 2] = m_vVTable[NextCorner(c)];

		c = NextCorner(m_vOTable[c]);   //move corner to new triangle

		switch (cmp.clers.pData[sCount - 1])
		{
		case 'C' :
			m_vOTable[NextCorner(c)] = -1;    //C: left edge is free, store ref to new vertex
			m_vVTable[3 * sCount] = ++vid;
			break;
		case 'L' :
			m_vOTable[NextCorner(c)] = -2;     //L: orient free edge
			Zip(NextCorner(c));
			break;
		case 'R':
			m_vOTable[c] = -2;                 //R: orient free edge, go left 
			c = NextCorner(c);
			break;
		case 'S':
			//S: recursion going right, then go left
			DecompressConnectivity(cmp, c);
			c = NextCorner(c);
			//if the triangle to the left was visited, then return
			if (m_vOTable[c] >= 0)
				return;
			break;
		case 'E':
			//E: left and right edges are  free
			m_vOTable[c] = -2;
			m_vOTable[NextCorner(c)] = -2;
			Zip(NextCorner(c));
			//pop
			return;

		/*default:
			break;*/
		}

	} while (true);
}

void VBEDDecompression::InitDecompressVertices(const CLERS & cmp)
{
	//estimate 1st  vertex
	m_vVert[0]  = DecodeDelta(0, cmp);
	m_vVertD[0] = DecodeDeltaD(0, cmp);                  ////////////
	m_vVertFlag[0] = true;

	//estimate third vertex and mark it as visited
	m_vVert[1]  = DecodeDelta(2, cmp);
	m_vVertD[1] = DecodeDeltaD(2, cmp);                  ////////////
	m_vVertFlag[1] = true;

	//estimate second vertex and mark it as visited
	m_vVert[2]  = DecodeDelta(1, cmp);
	m_vVertD[2] = DecodeDeltaD(1, cmp);                  ////////////
	m_vVertFlag[2] = true;

	vid = 2;
	m_vTriFlag[0] = true;
}

void VBEDDecompression::DecompressVertices(unsigned c, const CLERS & cmp)
{
	do
	{
		m_vTriFlag[CornerToTri(c)] = true;

		//test whether tip vertex was visited
		if (!m_vVertFlag[m_vVTable[c]]) 
		{
			vid ++;
			m_vVert[vid]  = DecodeDelta(c, cmp);
			m_vVertD[vid] = DecodeDeltaD(c, cmp);                  ////////////
			m_vVertFlag[m_vVTable[c]] = true;
			c = RightTri(c);
		}
		else
		{
			//test whether right triangle was visited
			if (m_vTriFlag[CornerToTri(RightTri(c))])
			{
				if (m_vTriFlag[CornerToTri(LeftTri(c))])
					return;    //E, pop
				else
					c = LeftTri(c);    //R,move to left triangle
			}
			else
			{
				//test whether left triangle was visited
				if (m_vTriFlag[CornerToTri(LeftTri(c))])
					c = RightTri(c);        //L, move to right triangle
				else
				{
					//S, recursive call to visit right branch first
					DecompressVertices(RightTri(c), cmp);
					c = LeftTri(c);   //move to left triangle
				    //if the triangle to the left was visited, then return
					if (m_vTriFlag[CornerToTri(c)])
						return;
				}
			}
		}
	} while (true);
}

void VBEDDecompression::Zip(unsigned c)
{
	/*unsigned b = NextCorner(c);
	//search clockwise for free edge
	while (m_vOTable[b] >= 0 && m_vOTable[b] != c)
	{
		b = NextCorner(m_vOTable[b]);
	}

	if (m_vOTable[b] != -1)
		return;
	
	m_vOTable[c] = b; 
	m_vOTable[b] = c;

	//assign co-incident corners
	unsigned a = NextCorner(c);
	m_vVTable[NextCorner(a)] = m_vVTable[NextCorner(b)];
	while (m_vOTable[a] >= 0 && a != b)
	{
		a = NextCorner(m_vOTable[a]);
		m_vVTable[NextCorner(a)] = m_vVTable[NextCorner(b)];
	}

	c = PrevCorner(c);
	while (m_vOTable[c] >= 0 && c != b)
	{
		c = PrevCorner(m_vOTable[c]);
	}
	if (m_vOTable[c] == -2)
		Zip(c);*/

	do
	{
		unsigned b = NextCorner(c);
		//search clockwise for free edge
		while (m_vOTable[b] >= 0 && m_vOTable[b] != c)
		{
			b = NextCorner(m_vOTable[b]);
		}

		if (m_vOTable[b] != -1)
			return;

		/*m_vOTable[b] = c;
		m_vOTable[c] = b;*/

		m_vOTable[c] = b;
		m_vOTable[b] = c;

		//assign co-incident corners
		unsigned a = NextCorner(c);
		m_vVTable[NextCorner(a)] = m_vVTable[NextCorner(b)];
		while (m_vOTable[a] >= 0 && a != b)
		{
			a = NextCorner(m_vOTable[a]);
			m_vVTable[NextCorner(a)] = m_vVTable[NextCorner(b)];
		}

		c = PrevCorner(c);
		while (m_vOTable[c] >= 0 && c != b)
		{
			c = PrevCorner(m_vOTable[c]);
		}
	} while (m_vOTable[c] == -2);
}

VFVECTOR3 VBEDDecompression::DecodeDelta(unsigned c, const CLERS & cmp)
{
	return  DecodeWithPrediction(c, cmp);
}

VFVECTOR3 VBEDDecompression::DecodeWithPrediction(unsigned c, const CLERS & cmp)
{
	VNVECTOR3UI  vid(VD_INVALID_INDEX, VD_INVALID_INDEX, VD_INVALID_INDEX);
	VFVECTOR3 vPred, delta = cmp.VertDelta.pData[EBVcount];
	EBVcount ++;	

	if (m_vVertFlag[m_vVTable[m_vOTable[c]]] && m_vVertFlag[m_vVTable[PrevCorner(c)]])
	{
		vPred = m_vVert[m_vVTable[NextCorner(c)]] + m_vVert[m_vVTable[PrevCorner(c)]];
		vPred -= m_vVert[m_vVTable[m_vOTable[c]]];
		vPred += delta;

		///////////////
		vid.x = m_vVTable[NextCorner(c)];
		vid.y = m_vVTable[PrevCorner(c)];
		vid.z = m_vVTable[m_vOTable[c]];
		m_vVertMapID.push_back(vid);

		return vPred;
	}
	if (m_vVertFlag[m_vVTable[m_vOTable[c]]])
	{
		vPred = m_vVert[m_vVTable[NextCorner(c)]] * 2.f;
		vPred -= m_vVert[m_vVTable[m_vOTable[c]]];
		vPred += delta;

		//////////////
		//impossible

		return vPred;
	}
	if (m_vVertFlag[m_vVTable[NextCorner(c)]] && m_vVertFlag[m_vVTable[PrevCorner(c)]])
	{
		vPred = m_vVert[m_vVTable[NextCorner(c)]] + m_vVert[m_vVTable[PrevCorner(c)]];
		vPred *= .5f;
		vPred += delta;

		///////////////
		vid.x = m_vVTable[NextCorner(c)];
		vid.y = m_vVTable[PrevCorner(c)];
		m_vVertMapID.push_back(vid);

		return vPred;
	}
	if (m_vVertFlag[m_vVTable[NextCorner(c)]])
	{
		vPred = m_vVert[m_vVTable[NextCorner(c)]];
		vPred += delta;

		/////////////////
		vid.x = m_vVTable[NextCorner(c)];
		m_vVertMapID.push_back(vid);

		return vPred;
	}
	else if (m_vVertFlag[m_vVTable[PrevCorner(c)]])
	{
		vPred = m_vVert[m_vVTable[PrevCorner(c)]];
		vPred += delta;

		/////////////////
		vid.x = m_vVTable[PrevCorner(c)];
		m_vVertMapID.push_back(vid);

		return vPred;
	}
	else
	{
		vPred = delta;

		//////////////////
		m_vVertMapID.push_back(vid);

		return vPred;
	}
}

DLE3 VBEDDecompression::DecodeDeltaD(unsigned c, const CLERS & cmp)
{
	return DecodeWithPredictionD(c, cmp);;
}

DLE3 VBEDDecompression::DecodeWithPredictionD(unsigned c, const CLERS & cmp)
{
	VNVECTOR3UI  vid(VD_INVALID_INDEX, VD_INVALID_INDEX, VD_INVALID_INDEX);
	DLE3   vPredD, deltaD;
	if (cmp.VertDeltaD.len == cmp.VertDelta.len)
	{
		deltaD.x = cmp.VertDeltaD.pData[EBVcountTemp].x;
		deltaD.y = cmp.VertDeltaD.pData[EBVcountTemp].y;
		deltaD.z = cmp.VertDeltaD.pData[EBVcountTemp].z;
		EBVcountTemp++;

		if (m_vVertFlag[m_vVTable[m_vOTable[c]]] && m_vVertFlag[m_vVTable[PrevCorner(c)]])
		{
			//vPred = m_vVert[m_vVTable[NextCorner(c)]] + m_vVert[m_vVTable[PrevCorner(c)]];
			//vPred -= m_vVert[m_vVTable[m_vOTable[c]]];
			//vPred += delta;

			///////////////
			vPredD.x = m_vVertD[m_vVTable[NextCorner(c)]].x + m_vVertD[m_vVTable[PrevCorner(c)]].x - m_vVertD[m_vVTable[m_vOTable[c]]].x;
			vPredD.y = m_vVertD[m_vVTable[NextCorner(c)]].y + m_vVertD[m_vVTable[PrevCorner(c)]].y - m_vVertD[m_vVTable[m_vOTable[c]]].y;
			vPredD.z = m_vVertD[m_vVTable[NextCorner(c)]].z + m_vVertD[m_vVTable[PrevCorner(c)]].z - m_vVertD[m_vVTable[m_vOTable[c]]].z;
			vPredD.x = vPredD.x + deltaD.x;
			vPredD.y = vPredD.y + deltaD.y;
			vPredD.z = vPredD.z + deltaD.z;

			return vPredD;
		}
		if (m_vVertFlag[m_vVTable[m_vOTable[c]]])
		{
			//vPred = m_vVert[m_vVTable[NextCorner(c)]] * 2.f;
			//vPred -= m_vVert[m_vVTable[m_vOTable[c]]];
			//vPred += delta;
			//////////////
			vPredD.x = m_vVertD[m_vVTable[NextCorner(c)]].x * 2. - m_vVertD[m_vVTable[m_vOTable[c]]].x;
			vPredD.y = m_vVertD[m_vVTable[NextCorner(c)]].y * 2. - m_vVertD[m_vVTable[m_vOTable[c]]].y;
			vPredD.z = m_vVertD[m_vVTable[NextCorner(c)]].z * 2. - m_vVertD[m_vVTable[m_vOTable[c]]].z;
			vPredD.x = vPredD.x + deltaD.x;
			vPredD.y = vPredD.y + deltaD.y;
			vPredD.z = vPredD.z + deltaD.z;
			//////////////
			//impossible

			return vPredD;
		}
		if (m_vVertFlag[m_vVTable[NextCorner(c)]] && m_vVertFlag[m_vVTable[PrevCorner(c)]])
		{
			//vPred = m_vVert[m_vVTable[NextCorner(c)]] + m_vVert[m_vVTable[PrevCorner(c)]];
			//vPred *= .5f;
			//vPred += delta;
			///////////////
			vPredD.x = (m_vVertD[m_vVTable[NextCorner(c)]].x + m_vVertD[m_vVTable[PrevCorner(c)]].x) * .5;
			vPredD.y = (m_vVertD[m_vVTable[NextCorner(c)]].y + m_vVertD[m_vVTable[PrevCorner(c)]].y) * .5;
			vPredD.z = (m_vVertD[m_vVTable[NextCorner(c)]].z + m_vVertD[m_vVTable[PrevCorner(c)]].z) * .5;
			vPredD.x = vPredD.x + deltaD.x;
			vPredD.y = vPredD.y + deltaD.y;
			vPredD.z = vPredD.z + deltaD.z;

			return vPredD;
		}
		if (m_vVertFlag[m_vVTable[NextCorner(c)]])
		{
			//vPred = m_vVert[m_vVTable[NextCorner(c)]];
			//vPred += delta;
			/////////////////
			vPredD.x = m_vVertD[m_vVTable[NextCorner(c)]].x;
			vPredD.y = m_vVertD[m_vVTable[NextCorner(c)]].y;
			vPredD.z = m_vVertD[m_vVTable[NextCorner(c)]].z;
			vPredD.x = vPredD.x + deltaD.x;
			vPredD.y = vPredD.y + deltaD.y;
			vPredD.z = vPredD.z + deltaD.z;

			return vPredD;
		}
		else if (m_vVertFlag[m_vVTable[PrevCorner(c)]])
		{
			//vPred = m_vVert[m_vVTable[PrevCorner(c)]];
			//vPred += delta;
			//////////////////
			vPredD.x = m_vVertD[m_vVTable[PrevCorner(c)]].x;
			vPredD.y = m_vVertD[m_vVTable[PrevCorner(c)]].y;
			vPredD.z = m_vVertD[m_vVTable[PrevCorner(c)]].z;
			vPredD.x = vPredD.x + deltaD.x;
			vPredD.y = vPredD.y + deltaD.y;
			vPredD.z = vPredD.z + deltaD.z;

			return vPredD;
		}
		else
		{
			//vPred = delta;
			vPredD = deltaD;

			return vPredD;
		}
	}
	else
		return vPredD;
}

inline unsigned VBEDDecompression::NextCorner(unsigned c)
{
	return (3 * (c / 3) + (c + 1) % 3);;
}

inline unsigned VBEDDecompression::PrevCorner(unsigned c)
{
	return NextCorner(NextCorner(c));
}

inline unsigned VBEDDecompression::RightTri(unsigned c)
{
	return m_vOTable[NextCorner(c)];
}

inline unsigned VBEDDecompression::LeftTri(unsigned c)
{
	return m_vOTable[NextCorner(NextCorner(c))];
}

inline unsigned VBEDDecompression::CornerToTri(unsigned c)
{
	return (c / 3);
}

//VD_EXPORT_SYSTEM_SIMPLE(VBEDDecompression, VNALGMESH::VREDDecompression);