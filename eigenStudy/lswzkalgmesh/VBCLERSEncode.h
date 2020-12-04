#pragma once
#include<vmath/vfmesh.h>

#include <sys/alg/isysmesh.h>

//Íø¸ñÑ¹ËõÈý½ÇÆ¬µÄCLERS±àÂë
// C:0  R:10  L:110  S:1110   E:1111
class VBCLERSEncode
{
public:
	VBCLERSEncode() : buf(0), value(0), filled_Len(0), residue_Len(0), subres_Len(0), count(0) {}
	~VBCLERSEncode() {}

	void Build(VSConstBuffer<char> & code, const VSConstBuffer<char> & clers)
	{
		MemberInit();
		Encode(clers);

		code = VD_V2CB(m_vCodeStream);
	}

private:
	void MemberInit();
	void Init(const VSConstBuffer<char> & clers);
	void Encode(const VSConstBuffer<char> & clers);
	inline void CharProcess(char bitLen, const unsigned & len);
	inline void OutputChar(char c, std::vector<char> & vec);
	inline char NextChar(const unsigned & counter);

private:
	std::vector<char>    m_vCodeStream;
	std::vector<char>    m_vCodeNum;
private:
	char buf, value;
	char filled_Len;
	char residue_Len;
	char subres_Len;
	unsigned count;
};