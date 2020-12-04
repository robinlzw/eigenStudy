#include "stdafx.h"
#include "VCFindPointPairPath.h"
#include <obj/ialgmesh/ialgmesh.h>
#include <obj/tisysbak.h>
#include <pkg/ldck4cce.h>
#include <pkg/ldwzkupdater.h>


namespace NMALG_CUTPATHGEN
{
	VCFindPointPairPath::VCFindPointPairPath()
	{

	}

	VCFindPointPairPath::~VCFindPointPairPath()
	{

	}

	void GetShortestPoint(std::vector<unsigned> &out, const VFVECTOR3* pVertices,
		const VSConstBuffer<unsigned> &toothGum1, const VSConstBuffer<unsigned> &toothGum2)
	{
		double mDisMin = (pVertices[toothGum1.pData[0]] - pVertices[toothGum2.pData[0]]).Magnitude();
		unsigned tagi = 0;
		unsigned tagj = 0;
		for (unsigned i = 0; i < toothGum1.len; i++)
		{
			for (unsigned j = 0; j < toothGum2.len; j++)
			{
				double dis = (pVertices[toothGum1.pData[i]] - pVertices[toothGum2.pData[j]]).Magnitude();
				if (dis < mDisMin)
				{
					mDisMin = dis;
					tagi = i;
					tagj = j;

				}
			}
		}
		out[0] = tagi;
		out[1] = tagj;
	}

	void ExtendPoint(std::vector<std::vector<unsigned>> &out,
		int idxminusA, int idxplusA, int idxminusB, int idxplusB,
		const VFVECTOR3* pVertices, 
		const VSConstBuffer<unsigned> &toothGum1, const VSConstBuffer<unsigned> &toothGum2)
	{
		float threshold = 0.2;     //0.2可调
		int itercount = 0;
		while (1)
		{
			if (itercount > 5)         //5可调,控制点不能离最近点太远
			{
				out[0].resize(2);
				out[1].resize(2);

				out[0][0] = toothGum1.pData[idxminusA];
				out[0][1] = toothGum2.pData[idxplusB];
				out[1][0] = toothGum1.pData[idxplusA];
				out[1][1] = toothGum2.pData[idxminusB];
				break;
			}
#if 0
			if (toothGum1.len <= idxplusA)
			{
				idxplusA = idxplusA - toothGum1.len;
			}
			if (0 > idxminusA)
			{
				idxminusA = toothGum1.len + idxminusA;
			}

			if (toothGum2.len <= idxplusB)
			{
				idxplusB = idxplusB - toothGum2.len;
			}
			if (0 > idxminusB)
			{
				idxminusB = toothGum2.len + idxminusB;
			}
#endif
			float len1 = (pVertices[toothGum1.pData[idxplusA]] - pVertices[toothGum2.pData[idxminusB]]).Magnitude();
			float len2 = (pVertices[toothGum1.pData[idxminusA]] - pVertices[toothGum2.pData[idxplusB]]).Magnitude();


			if (((len2 - len1) / len2 > threshold))
			{
				idxplusA = (idxplusA + 1) % toothGum1.len;
				idxminusB = (idxminusB-1 + toothGum2.len) % toothGum2.len;
			}
			else if (((len1 - len2) / len1 > threshold))
			{
				idxplusB = (idxplusB + 1)% toothGum2.len;
				idxminusA = (idxminusA - 1 + toothGum1.len) % toothGum1.len;
			}
			else
			{
				out[0].resize(2);
				out[1].resize(2);

				out[0][0] = toothGum1.pData[idxminusA];
				out[0][1] = toothGum2.pData[idxplusB];
				out[1][0] = toothGum1.pData[idxplusA];  
				out[1][1] = toothGum2.pData[idxminusB];
				break;
			}
			itercount++;

		}

	}

	void GetExtendPoint(std::vector<std::vector<unsigned>> &out,
		const VFVECTOR3* pVertices, const std::vector<unsigned> &mPoint,
		const VSConstBuffer<unsigned> &toothGum1, const VSConstBuffer<unsigned> &toothGum2,
		int indextag)
	{

		int idxminusA = (mPoint[0] - indextag + toothGum1.len) % toothGum1.len;
		int idxplusA = (mPoint[0] + indextag) % toothGum1.len;
		int idxminusB = (mPoint[1] - indextag + toothGum2.len)% toothGum2.len;
		int idxplusB = (mPoint[1] + indextag)% toothGum2.len;
		ExtendPoint(out, idxminusA, idxplusA, idxminusB, idxplusB, pVertices, toothGum1, toothGum2);
	}

	void VCFindPointPairPath::Gen(VSConstBuffer<unsigned>& pointSet, const VSCutPathGenInput& input)
	{
		/*unsigned offset = 0;
		unsigned totalSize = 0;*/
		m_vPointSet.clear();
		for (unsigned i = 0; i < input.cbGumGumLine.len; ++i)   //牙齿上牙龈线
		{
			/*totalSize += input.cbGumGumLine.pData[i].len;
			m_vPointSet.resize(totalSize);
			std::memcpy(&m_vPointSet[offset], input.cbGumGumLine.pData[i].pData, sizeof(unsigned)*input.cbGumGumLine.pData[i].len);
			offset += input.cbGumGumLine.pData[i].len;*/
			for (unsigned j = 0; j < input.cbGumGumLine.pData[i].len; ++j)
			{
				m_vPointSet.push_back(input.cbGumGumLine.pData[i].pData[j]);
			}

		}
		
		//
		for (unsigned k = 0; k < input.cbGumGumLine.len - 1; ++k) 
		{
			//两颗牙齿的牙龈线上距离最短的两个点在cbGumGumLine中的idx
			std::vector<unsigned> mPoint(2);
			GetShortestPoint(mPoint, input.gumMesh.pVertices, input.cbGumGumLine.pData[k], input.cbGumGumLine.pData[k + 1]);

			//控制点对,mExtendPoint存的是点在mesh顶点中的idx                                      
			std::vector<std::vector<unsigned>> mExtendPoint(2);
			GetExtendPoint(mExtendPoint, input.gumMesh.pVertices, mPoint, input.cbGumGumLine.pData[k], input.cbGumGumLine.pData[k + 1], 4);
			//使用dijk
			VSConstBuffer< VNWZKALG::VSPathInfo >  onePathOut;
			VSConstBuffer< VNWZKALG::VSPathInfo >  otherPathOut;

			VSGraphMesh gm;  //
			TVExtSource<VNALGMESH::VRPerfGraph> esPerfGraphl;
			TVR2B< VNALGMESH::VRPerfGraph > bdrGraph(*esPerfGraphl);
			bdrGraph.Build(gm, input.gumMesh);
			gm.pVertices = input.gumMesh.pVertices;

			TVExtSource< VNALGMESH::VRDijkstra   > mExtSysDijkstra;
			TVR2B< VNALGMESH::VRDijkstra   >  mbdrDijkstr(*mExtSysDijkstra);
			VNWZKALG::VRDijkstraTerminal term;  //
			term.bgnIndx = mExtendPoint[0][0];
			term.endIndx = mExtendPoint[0][1];
			mbdrDijkstr.Build(onePathOut, gm, term);

			TVExtSource< VNALGMESH::VRDijkstra   > mExtSysDijkstra1;
			TVR2B< VNALGMESH::VRDijkstra   >  mbdrDijkstr1(*mExtSysDijkstra1);
			VNWZKALG::VRDijkstraTerminal term1;  //
			term1.bgnIndx = mExtendPoint[1][1];
			term1.endIndx = mExtendPoint[1][0];
			mbdrDijkstr1.Build(otherPathOut, gm, term1);
			//
			for (unsigned i = 1; i < onePathOut.len - 1; ++i)
			{
				m_vPointSet.push_back(onePathOut.pData[i].pathVert);
			}
			for (unsigned i = 1; i < otherPathOut.len - 1; ++i)
			{
				m_vPointSet.push_back(otherPathOut.pData[i].pathVert);
			}
		}
		pointSet = VD_V2CB(m_vPointSet);
#if 0
		std::string name = "E:/outtest.obj";
		std::ofstream objFilex(name);
		for (unsigned s = 0; s < m_vPointSet.size(); s++)
		{
			objFilex << "v " << input.gumMesh.pVertices[m_vPointSet[s]].x << " " << input.gumMesh.pVertices[m_vPointSet[s]].y << " " << input.gumMesh.pVertices[m_vPointSet[s]].z << std::endl;
		}

		objFilex.close();
#endif		
	}
}
