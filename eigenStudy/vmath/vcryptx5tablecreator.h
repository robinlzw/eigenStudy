#pragma once

#define CryptComputeSeedFromPreSeed( preSeed ) (preSeed * 125 + 3) % 0x2AAAAB
#define CryptComputeValue( Seed1 , Seed2 ) (Seed1 & 0xFFFF) << 0x10 | (Seed2 & 0xFFFF)

template< unsigned long INDEX >
class Cryptx5
{
public:
    Cryptx5( unsigned long * cryptTable )
    {
        Cryptx5<INDEX-1> subcrypt(cryptTable);
        cryptTable[INDEX + 0x100 * 0] = value1;
        cryptTable[INDEX + 0x100 * 1] = value2;
        cryptTable[INDEX + 0x100 * 2] = value3;
        cryptTable[INDEX + 0x100 * 3] = value4;
        cryptTable[INDEX + 0x100 * 4] = value5;
    }
    ~Cryptx5(){}

    static unsigned const seed0   = Cryptx5<INDEX-1>::seed5_2;
    static unsigned const seed1_1 = CryptComputeSeedFromPreSeed( seed0 );
    static unsigned const seed1_2 = CryptComputeSeedFromPreSeed( seed1_1 );
    static unsigned const value1  = CryptComputeValue( seed1_1 , seed1_2 );
    static unsigned const seed2_1 = CryptComputeSeedFromPreSeed( seed1_2 );
    static unsigned const seed2_2 = CryptComputeSeedFromPreSeed( seed2_1 );
    static unsigned const value2  = CryptComputeValue( seed2_1 , seed2_2 );
    static unsigned const seed3_1 = CryptComputeSeedFromPreSeed( seed2_2 );
    static unsigned const seed3_2 = CryptComputeSeedFromPreSeed( seed3_1 );
    static unsigned const value3  = CryptComputeValue( seed3_1 , seed3_2 );
    static unsigned const seed4_1 = CryptComputeSeedFromPreSeed( seed3_2 );
    static unsigned const seed4_2 = CryptComputeSeedFromPreSeed( seed4_1 );
    static unsigned const value4  = CryptComputeValue( seed4_1 , seed4_2 );
    static unsigned const seed5_1 = CryptComputeSeedFromPreSeed( seed4_2 );
    static unsigned const seed5_2 = CryptComputeSeedFromPreSeed( seed5_1 );
    static unsigned const value5  = CryptComputeValue( seed5_1 , seed5_2 );
};

template<>
class Cryptx5<0>
{
public:
    Cryptx5( unsigned long * cryptTable )
    {
        cryptTable[0x100 * 0] = value1;
        cryptTable[0x100 * 1] = value2;
        cryptTable[0x100 * 2] = value3;
        cryptTable[0x100 * 3] = value4;
        cryptTable[0x100 * 4] = value5;
    }
    ~Cryptx5(){}

    static unsigned const seed0 = 0x00100001 ;
    static unsigned const seed1_1 = CryptComputeSeedFromPreSeed( seed0 );
    static unsigned const seed1_2 = CryptComputeSeedFromPreSeed( seed1_1 );
    static unsigned const value1  = CryptComputeValue( seed1_1 , seed1_2 );
    static unsigned const seed2_1 = CryptComputeSeedFromPreSeed( seed1_2 );
    static unsigned const seed2_2 = CryptComputeSeedFromPreSeed( seed2_1 );
    static unsigned const value2  = CryptComputeValue( seed2_1 , seed2_2 );
    static unsigned const seed3_1 = CryptComputeSeedFromPreSeed( seed2_2 );
    static unsigned const seed3_2 = CryptComputeSeedFromPreSeed( seed3_1 );
    static unsigned const value3  = CryptComputeValue( seed3_1 , seed3_2 );
    static unsigned const seed4_1 = CryptComputeSeedFromPreSeed( seed3_2 );
    static unsigned const seed4_2 = CryptComputeSeedFromPreSeed( seed4_1 );
    static unsigned const value4  = CryptComputeValue( seed4_1 , seed4_2 );
    static unsigned const seed5_1 = CryptComputeSeedFromPreSeed( seed4_2 );
    static unsigned const seed5_2 = CryptComputeSeedFromPreSeed( seed5_1 );
    static unsigned const value5  = CryptComputeValue( seed5_1 , seed5_2 );
};