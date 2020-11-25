#pragma once
#include "vcryptx5tablecreator.h"
#include <ctype.h>
#ifdef WIN32
//#include <xfunctional>
// 后来的VS版本中<xfunctional>不在对外提供，其被包含在<functional>中
#include <functional>
#endif
class vfasthashcompare
{	
public:
    vfasthashcompare(){}
    ~vfasthashcompare(){}

    enum
    {	// parameters for hash table
        bucket_size = 1		// 0 < bucket_size
    };

    size_t operator()(const char* str) const
    {	
        return HashString(str , 0);
    }

    bool operator()(const char* str1, const char* str2) const
    {	
        return HashString(str1 , 1) < HashString(str2 , 1);//(m_comp(str1 , str2));//
    }

private:
    unsigned long HashString(const char *lpszFileName, unsigned long dwHashType ) const
    { 
        unsigned char *key  = (unsigned char *)lpszFileName;
        unsigned long seed1 = 0x7FED7FED;
        unsigned long seed2 = 0xEEEEEEEE;
        int ch;

        while( *key != 0 )
        { 
            ch = toupper(*key++);

            seed1 = m_CryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
            seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
        }
        return seed1; 
    }

public:
    //std::less<const char*>    m_comp;

    static unsigned long m_CryptTable[0x500];
    static Cryptx5<0xFF> m_CryptOx500;
};
