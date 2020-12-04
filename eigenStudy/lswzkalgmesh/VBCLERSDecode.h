#pragma once
#include<vmath/vfmesh.h>

#include <sys/alg/isysmesh.h>

//网格压缩三角片的CLERS解码

class VBCLERSDecode
{
public:
	VBCLERSDecode() :buf(0), bufID(0), count(0) {}
	~VBCLERSDecode() {}

	void Build(VSConstBuffer<char> & clers, const VSConstBuffer<char> & code, const unsigned & clersLen)
	{
		MemberInit();
		Decode(code, clersLen);

		clers = VD_V2CB( m_vClers );
	}

private:
	void        MemberInit();
	void        Decode(const VSConstBuffer<char> & code, const unsigned clersLen);
	inline void OutputChar(char c, std::vector<char> & vec);
	inline char NextChar(const unsigned & counter, const VSConstBuffer<char> & code);
	inline void ExamBufID(const unsigned & counter, const VSConstBuffer<char> & code);

private:
	std::vector<char>     m_vClers;
	
private:
	char buf;
	char bufID;
	unsigned count;

	static char bufLen;
};
