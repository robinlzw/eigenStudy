#pragma once

#include "vmath/vfmesh.h"

class VSTLFReader
{
public:
	VSTLFReader(const std::string & fileName, bool bAscii);
	~VSTLFReader(void);

	bool Process(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs);     //参数是输出

private:
	void GetVertsAndSurfs(std::vector<VFVECTOR3>& vVerts, std::vector<VNVECTOR3UI>& vSurfs);
	int ReadNextValidData(char * &tBuf, unsigned &nCount, char * validData, const unsigned &nMaxSize);

protected:
	std::string    m_strFileName;
	bool            m_bAsciiMode;

	std::vector<VFVECTOR3> mVerts;

private:
	std::vector< VFVECTOR3 > m_listNormal;
};