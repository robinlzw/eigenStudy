#include "StdAfx.h"
#include "VSTLFReader.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <unordered_map>
#include <set>
#include <algorithm>  
#include "obj/lsalgtools/ilsalgtools.h"

#if 0
template<>
struct std::hash<VFVECTOR3>
{
	size_t operator() (const VFVECTOR3& v) const noexcept
	{
		return std::hash<decltype(v.x)>()(v.x)
			+ std::hash<decltype(v.y)>()(v.y)
			+ std::hash<decltype(v.z)>()(v.z);
	}
};
template<>
struct std::equal_to<VFVECTOR3>
{
	bool operator() (const VFVECTOR3& v0, const VFVECTOR3& v1) const noexcept
	{
		return (fabs(v0.x - v1.x) < VF_EPS_2)
			&& (fabs(v0.y - v1.y) < VF_EPS_2)
			&& (fabs(v0.z - v1.z) < VF_EPS_2);
	}
};
#endif
VSTLFReader::VSTLFReader(const std::string & fileName, bool bAscii)
	: m_strFileName(fileName)
	, m_bAsciiMode(bAscii)
{
}


VSTLFReader::~VSTLFReader(void)
{
}

bool VSTLFReader::Process(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs)
{
	if (m_bAsciiMode)
	{
		std::ifstream fin(m_strFileName, std::ios::in);

		fin.seekg(0, std::ios::end);   //seek to the end
		unsigned fileLen = (unsigned)fin.tellg();
		if (0 == fileLen)      // file is empty
		{
			return false;
		}
		fin.seekg(0, std::ios::beg); //seek to the beg

		char * pFileBuf = new char[fileLen + 1];
		std::memset(pFileBuf, 0, fileLen + 1);
		fin.read(pFileBuf, fileLen);

		char *pTemp = NULL;
		pTemp = pFileBuf;
		char tempBuffer[1024];
		unsigned nMaxSize = 1024;
		unsigned nReadLen = 0;
		unsigned nRet = 0;
		while (nReadLen < fileLen)
		{
			nRet = ReadNextValidData(pTemp, nReadLen, tempBuffer, nMaxSize);
			if (0 == nRet)
			{
				break;
			}
			if (std::strcmp(tempBuffer, "vertex") == 0)    //顶点信息
			{
				VFVECTOR3 vert;
				nRet = ReadNextValidData(pTemp, nReadLen, tempBuffer, nMaxSize);
				if (0 == nRet)
				{
					break;
				}
				vert.x = (float)atof(tempBuffer);
				nRet = ReadNextValidData(pTemp, nReadLen, tempBuffer, nMaxSize);
				if (0 == nRet)
				{
					break;
				}
				vert.y = (float)atof(tempBuffer);
				nRet = ReadNextValidData(pTemp, nReadLen, tempBuffer, nMaxSize);
				if (0 == nRet)
				{
					break;
				}
				vert.z = (float)atof(tempBuffer);
				mVerts.push_back(vert);
			}
		}
		delete(pFileBuf);

		GetVertsAndSurfs(vVerts, vSurfs);

		return true;
	}
	else
	{
		std::ifstream fin(m_strFileName, std::ios::in | std::ios::binary);

		fin.seekg(0, std::ios::end);   //seek to the end
		unsigned fileLen = (unsigned)fin.tellg();
		if (0 == fileLen)      // file is empty
		{
			return false;
		}

		fin.seekg(0, std::ios::beg);
		unsigned len = fin.tellg();
		char * buffer = new char[fileLen + 1];
		std::memset(buffer, 0, fileLen + 1);
		fin.read(buffer, fileLen);

		unsigned offset = 80;
		unsigned nVertDataCount = *(unsigned*)(buffer + offset);   //获取nVertDataCount
		offset += sizeof(int32_t);

		//从二进制文件读取顶点信息
		VFVECTOR3 pt = VFVECTOR3::ZERO;
		mVerts.resize(nVertDataCount * 3);

		for (unsigned k = 0; k < nVertDataCount; k++)
		{
			offset += 4 * 3; //normal

			for (unsigned i = 0; i < 3; i++)
			{
				pt.x = *(float*)(buffer + offset);
				offset += 4;
				pt.y = *(float*)(buffer + offset);
				offset += 4;
				pt.z = *(float*)(buffer + offset);
				offset += 4;

				mVerts[3 * k + i] = pt;
			}

			offset += 2;
		}
		delete(buffer);

		GetVertsAndSurfs(vVerts, vSurfs);

		return true;
	}
}


int VSTLFReader::ReadNextValidData(char * &tBuf, unsigned &nCount, char * validData, const unsigned &nMaxSize)
{
	unsigned nIndex = 0;

	while ((tBuf[0] == ' ') ||
		(tBuf[0] == '\n') ||
		(tBuf[0] == '\t') ||
		(tBuf[0] == '\r'))
	{
		tBuf++;
		nCount++;
	}

	while ((tBuf[0] != ' ') &&
		(tBuf[0] != '\n') &&
		(tBuf[0] != '\t') &&
		(tBuf[0] != '\r') &&
		(tBuf[0] != '\null') &&
		(tBuf[0] != 0) &&
		(nIndex < nMaxSize))
	{
		validData[nIndex++] = *(tBuf++);
		nCount++;
	}
	validData[nIndex] = 0;
	return nIndex;
}

void VSTLFReader::GetVertsAndSurfs(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs)
{
	// 修改说明：原先去除重复点的方法时间复杂度过高，改用hashmap
	// 修改人：wbc 2020-02-17
	unsigned nOldVertCnt = mVerts.size();
	vSurfs.resize(nOldVertCnt / 3);
	vVerts.reserve(nOldVertCnt);
	std::unordered_map<VFVECTOR3, unsigned> mapVerts;

	for (unsigned i = 0; i < nOldVertCnt / 3; i++)
	{
		unsigned nVCnt = 0;
		for (unsigned k = 0; k < 3; k++)
		{
			unsigned nOldIdx = i * 3 + k;
			const VFVECTOR3& v = mVerts[nOldIdx];

			if (mapVerts.count(v) == 0)
			{
				mapVerts.insert(std::make_pair(v, vVerts.size()));
				vVerts.push_back(v);
			}

			auto vert = mapVerts.find(v);
			vSurfs[i][k] = vert->second;
		}
	}

#if 0
	vSurfs.resize(mVerts.size() / 3);
	vVerts.resize(mVerts.size());
	std::vector<unsigned> vVertsIndex(mVerts.size(), VD_INVALID_INDEX);

	unsigned nNewVCnt = 0;
	for (unsigned i = 0; i < vSurfs.size(); i++)
	{
		unsigned nVCnt = 0;
		for (unsigned k = 0; k < 3; k++)
		{
			unsigned nOldIdx = i * 3 + k;
			const VFVECTOR3& v = mVerts[nOldIdx];
			unsigned j = 0;
			for (; j < nNewVCnt; j++)
			{
				//float flDist = (v - vVerts[j]).SqrMagnitude();
				//if (flDist < 0.001f)
				if (v == vVerts[j])
				{
					break;
				}
			}
			if (j < nNewVCnt)
			{
				vSurfs[i][k] = j;
			}
			else
			{
				vSurfs[i][k] = nNewVCnt + nVCnt;
				vVerts[nNewVCnt + nVCnt] = v;
				nVCnt++;
			}
		}
		nNewVCnt += nVCnt;
	}
	vVerts.resize(nNewVCnt);
#endif

#if 0
	//根据获得的顶点信息，获取verts和surfs的信息
	std::vector<VFVECTOR3> mTrans(mVerts);
	std::vector<VFVECTOR3>::iterator iter = unique(mTrans.begin(), mTrans.end());
	mTrans.erase(iter, mTrans.end());
	vVerts.assign(mTrans.begin(), mTrans.end());

	for (unsigned i = 0; i < mVerts.size() / 3; i++)
	{
		VNVECTOR3UI indx;
		for (unsigned j = 0; j < vVerts.size(); j++)
		{
			unsigned flag = 0;
			if (mVerts[3 * i] == vVerts[j])
			{
				indx.x = j;
				flag++;
			}
			if (mVerts[3 * i + 1] == vVerts[j])
			{
				indx.y = j;
				flag++;
			}
			if (mVerts[3 * i + 2] == vVerts[j])
			{
				indx.z = j;
				flag++;
			}
			if (3 == flag)
			{
				break;
			}
		}
		vSurfs.push_back(indx);
	}
#endif
}

