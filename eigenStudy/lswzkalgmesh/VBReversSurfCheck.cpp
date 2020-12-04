#include "stdafx.h"
#include "VBReversSurfCheck.h"

void VBReversSurfCheck::Init(const VSSimpleMeshF & smesh)
{
	unsigned sNum = smesh.nTriangleCount;
	m_vNormalTri.reserve(sNum);
	m_vReverseTri.reserve(sNum);
	m_vTriFlag.resize(sNum);
	m_vPushFlag.resize(sNum);
	m_vTriClassify.resize(sNum, -1);
}

void VBReversSurfCheck::Check(const VSSimpleMeshF & smesh)
{
	Init(smesh);

	//改进方法
	unsigned count = 0;
	if (mesh.read(smesh)) 
	{
		while (count < mesh.faces.size())
		{
			FaceIter seed;    //seed triangle
			for (seed = mesh.faces.begin(); seed < mesh.faces.end(); seed ++)
			{
				if (!m_vTriFlag[seed->index])
				{
					m_vTriClassify[seed->index] = 1;
					m_hQueue.push(seed);
					m_vPushFlag[seed->index] = true;
					break;
				}
			}
			
			while (!m_hQueue.empty())
			{
				seed = m_hQueue.front();
				HalfEdgeIter h = seed->he;
				do
				{
					if (h->flip->face < mesh.faces.end() && h->flip->face >= mesh.faces.begin())
					{
						if (!m_vTriFlag[h->flip->face->index])
						{
							if ((h->flip->vertex->index != h->vertex->index))
								m_vTriClassify[h->flip->face->index] = m_vTriClassify[h->face->index];
							else if ((h->flip->vertex->index == h->vertex->index))
								m_vTriClassify[h->flip->face->index] = (m_vTriClassify[h->face->index] + 1) % 2;

							if (!m_vPushFlag[h->flip->face->index])
							{
								m_hQueue.push(h->flip->face);
								m_vPushFlag[h->flip->face->index] = true;
							}

							count ++;
						}
					}
					h = h->next;
				} while (h != seed->he);
				m_vTriFlag[seed->index] = true;
				m_hQueue.pop();
			}
		}
	}

	for (unsigned i = 0; i < smesh.nTriangleCount; i++)
	{
		if (m_vTriClassify[i] == 1)
			m_vNormalTri.push_back(i);
		else
			m_vReverseTri.push_back(i);
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBReversSurfCheck, VNALGMESH::VRReversSurfCheck);