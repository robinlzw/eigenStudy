#include "stdafx.h"
#include "VBPCAEncodeComb.h"

void VBPCAEncodeComb::MemberInit()
{
	m_vULowBit.clear();
	m_vUHihtBit.clear();
	m_vVCompres.clear();
	m_vVLowBit.clear();
	m_vVHighBit.clear();
	m_vMean.clear();
	m_vVertMapID.clear();
	m_vBitSetTemp.clear();
	m_vTemp.clear();
	m_vFrame.clear();
	m_vRemainFrame.clear();
}

void VBPCAEncodeComb::CombFunc(const VSConstBuffer<VSConstBuffer<VFVECTOR3>>& vert)
{
	Rmax = 1 << 31;
	Rmin = 1 << 23;
	//初始网格压缩
	//m_objEncode.Build(m_OrgMesh, gm, deltaCoeff);
	//m_FrameRes.orgMesh = m_OrgMesh;
	//
	//////////////
	////初始网格解压
	////*VBEDDecodeComb  dec;
	//VSEdgeBreakerDecRes  smsh;
	//float deltaCoeff1 = 1e-7;
	//dec.Build(smsh, m_OrgMesh, deltaCoeff);
	//std::ofstream f("D:\\mesh.obj");
	//for (unsigned i = 0; i < smsh.smesh.nVertCount; i++)
	//{
	//f << "v " << smsh.smesh.pVertices[i].x << " " << smsh.smesh.pVertices[i].y << " " << smsh.smesh.pVertices[i].z << std::endl;
	//}
	//f << std::endl;
	//for (unsigned i = 0; i < smsh.smesh.nTriangleCount; i++)
	//{
	//f << "f " << smsh.smesh.pTriangles[i].x + 1 << " " << smsh.smesh.pTriangles[i].y + 1 << " " << smsh.smesh.pTriangles[i].z + 1 << std::endl;
	//}
	//f.close();*/
	//////////////
	//
	////删除第一帧数据
	//VSConstBuffer<VSConstBuffer<VFVECTOR3>>    vt;
	//FirstFrameDel(vert);
	//vt = VD_V2CB(m_vRemainFrame);

	//PCA数据调整
	float ratio = .9999f;
	m_objPCAComp.Build(m_UDVdata, vert, ratio);

	/////////
	//UDV数据还原
	/*VSDnsMatrixF framePos;
	VBPCARestore  resPCA;
	resPCA.Build(framePos, m_UDVdata);*/
	/////////

	double deltaCoeff = 1e-5;

	//U矩阵压缩
	unsigned  uLen = m_UDVdata.U.row * m_UDVdata.U.col;
	m_vTemp.clear();
	m_vTemp.resize(uLen);
	memcpy(&m_vTemp.front(), m_UDVdata.U.pData, sizeof(float) * uLen);
	FloatProc(m_FrameRes.U.min, m_FrameRes.U.bitLen, m_vULowBit, m_vUHihtBit, m_vTemp, deltaCoeff);
	m_FrameRes.U.lowBitVal = VD_V2CB(m_vULowBit);
	m_FrameRes.U.highBitCompVal = VD_V2CB(m_vUHihtBit);

	//V矩阵压缩
	VCompress((float)deltaCoeff);
	m_FrameRes.V = VD_V2CB(m_vVCompres);

	//D 矩阵
	m_FrameRes.D.len = m_UDVdata.D.row * m_UDVdata.D.col;
	m_FrameRes.D.pData = m_UDVdata.D.pData;

	//mean
	m_vMean.resize(m_UDVdata.mean.len * 3);
	for (unsigned i = 0; i < m_UDVdata.mean.len; i++)
	{
		m_vMean[i * 3 + 0] = m_UDVdata.mean.pData[i].x;
		m_vMean[i * 3 + 1] = m_UDVdata.mean.pData[i].y;
		m_vMean[i * 3 + 2] = m_UDVdata.mean.pData[i].z;
	}
	m_FrameRes.mean = VD_V2CB(m_vMean);
}

void VBPCAEncodeComb::VCompress(const float & deltaCoeff)
{
	m_vTemp.clear();
	m_vTemp.resize(m_UDVdata.V.col);
	m_vVLowBit.resize(m_UDVdata.V.row);
	m_vVHighBit.resize(m_UDVdata.V.row);
	m_vVCompres.resize(m_UDVdata.V.row);

	double deltaCoeffD = 1e-5;
	for (unsigned i = 0; i < m_UDVdata.V.row; i++)
	{
		memcpy(&m_vTemp.front(), &m_UDVdata.V.pData[i * m_UDVdata.V.col], sizeof(float) * m_UDVdata.V.col);

		FloatProc(m_vVCompres[i].min, m_vVCompres[i].bitLen, m_vVLowBit[i], m_vVHighBit[i], m_vTemp, deltaCoeffD);

		m_vVCompres[i].lowBitVal = VD_V2CB(m_vVLowBit[i]);
		m_vVCompres[i].highBitCompVal = VD_V2CB(m_vVHighBit[i]);
	}
}

void VBPCAEncodeComb::FloatProc(int & minI, VNVECTOR2UI & bitlen, std::vector<char>& lowBit, std::vector<char>& highBit, const std::vector<float>& data, const double & deltaCoeff)
{
	//float to int
	std::vector<int>  m_temp;
	m_temp.resize(data.size());
	for (unsigned i = 0; i < data.size(); i++)
		m_temp[i] = static_cast<int>(data[i] / deltaCoeff);

	//int to unsigned
	VNWZKALG::VSLossQuantization    iQuan;
	VSConstBuffer<int>      intData = VD_V2CB(m_temp);
	m_objIntProc.Build(iQuan, intData);

	//range  encode
	VBRangeEncode   hbit(Rmax, Rmin);
	VSConstBuffer<unsigned>    code;
	hbit.Build(code, iQuan.highBitVal, 1 << iQuan.bitLen.x);

	//unsigned to char
	highBit.resize(code.len);
	for (unsigned i = 0; i < code.len; i++)
		highBit[i] = static_cast<char>(code.pData[i] & 0x000000FF);
	LowBitProcess(lowBit, iQuan);

	minI = iQuan.min;
	bitlen = iQuan.bitLen;
}

void VBPCAEncodeComb::LowBitProcess(std::vector<char>& vec, const VSLossQuantization & LowBit)
{
	unsigned totalLowBitLen = LowBit.bitLen.y * LowBit.lowBitVal.len;
	m_vBitSetTemp.resize(totalLowBitLen);

	unsigned count = 0;
	for (unsigned i = 0; i < LowBit.lowBitVal.len; i++)
	{
		for (int j = LowBit.bitLen.y - 1; j >= 0; j--)
		{
			m_vBitSetTemp[count] = LowBit.lowBitVal.pData[i] >> j & 0x1;
			count++;
		}
	}

	count = 0;
	vec.resize(static_cast<unsigned>(ceilf(static_cast<float>(totalLowBitLen) / 8.f)));
	unsigned residueLen = m_vBitSetTemp.size() - totalLowBitLen / 8 * 8;
	for (unsigned i = 0; i < m_vBitSetTemp.size() - residueLen; i += 8)
	{
		char temp = 0;
		for (unsigned j = 0; j < 8; j++)
		{
			temp |= (static_cast<char>(m_vBitSetTemp[i + j])) << (7 - j);
		}
		vec[count] = temp;
		count++;
	}

	//the residue bites
	if (residueLen != 0)
	{
		char temp = 0;
		for (unsigned i = 0; i < residueLen; i++)
		{
			temp |= (static_cast<char>(m_vBitSetTemp[m_vBitSetTemp.size() - residueLen + i])) << (7 - i);
		}
		vec[count] = temp;
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBPCAEncodeComb, VNALGMESH::VRPCAEncodeCombNoED);