#include "stdafx.h"
#include "VBCLERSDecode.h"

char VBCLERSDecode::bufLen = 8;

void VBCLERSDecode::MemberInit()
{
	buf = 0;
	bufID = 0;
	count = 0;

	m_vClers.clear();
}

void VBCLERSDecode::Decode(const VSConstBuffer<char>& code, const unsigned clersLen)
{
	m_vClers.reserve(3 * code.len);

	buf = code.pData[count];
	while (m_vClers.size() < clersLen)
	{
		ExamBufID(count, code);
		//if (bufID <= 7 && (buf & 1 << (bufLen - bufID - 1)) == 0)
		if ((buf & 1 << (bufLen - bufID - 1)) == 0)
		{
			OutputChar('C', m_vClers);
			bufID++;
		}
		else
		{
			bufID++;
			ExamBufID(count, code);
			//if (bufID <= 7 && (buf & 1 << (bufLen - bufID - 1)) == 0)
			if ((buf & 1 << (bufLen - bufID - 1)) == 0)
			{
				OutputChar('R', m_vClers);
				bufID++;
			}
			else
			{
				bufID++;
				ExamBufID(count, code);
				//if (bufID <= 7 && (buf & 1 << (bufLen - bufID - 1)) == 0)
				if ((buf & 1 << (bufLen - bufID - 1)) == 0)
				{
					OutputChar('L', m_vClers);
					bufID++;
				}
				else
				{
					bufID++;
					ExamBufID(count, code);
					//if (bufID <= 7 && (buf & 1 << (bufLen - bufID - 1)) == 0)
					if ((buf & 1 << (bufLen - bufID - 1)) == 0)
					{
						OutputChar('S', m_vClers);
						bufID++;
					}
					else
					{
						OutputChar('E', m_vClers);
						bufID++;
					}
				}
			}
		}
	}
}

inline void VBCLERSDecode::OutputChar(char c, std::vector<char>& vec)
{
	vec.push_back(c);
}

inline char VBCLERSDecode::NextChar(const unsigned & counter, const VSConstBuffer<char>& code)
{
	return code.pData[counter];
}

inline void VBCLERSDecode::ExamBufID(const unsigned & counter, const VSConstBuffer<char>& code)
{
	if (bufID > 7 && count < (code.len - 1))
	{
		count++;
		buf = NextChar(count, code);
		bufID = 0;
	}
}

//VD_EXPORT_SYSTEM_SIMPLE(VBCLERSDecode, VNALGMESH::VRCLERSDecode);