#include "vfasthashcompare.h"

unsigned long vfasthashcompare::m_CryptTable[0x500];
Cryptx5<0xFF> vfasthashcompare::m_CryptOx500(vfasthashcompare::m_CryptTable);