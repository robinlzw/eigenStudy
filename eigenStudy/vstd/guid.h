#pragma once

#include "dstrm.h"

struct VGUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];

    bool VGUID::operator == ( const VGUID& target ) const
    {
        int* p1 = (int*)&Data1;
        int* p2 = (int*)&Data2;
        int* p3 = (int*)&Data4[0];
        int* p4 = (int*)&Data4[4];

        int* p1t = (int*)&target.Data1;
        int* p2t = (int*)&target.Data2;
        int* p3t = (int*)&target.Data4[0];
        int* p4t = (int*)&target.Data4[4];

        return *p1 == *p1t 
            && *p2 == *p2t 
            && *p3 == *p3t 
            && *p4 == *p4t;
    }
};

inline VDataOutput & operator <<( VDataOutput & dOut , const VGUID & v )
{
    dOut << v.Data1 << v.Data2 << v.Data3 ;
    dOut.Write8( v.Data4 , 8 );

    return dOut ;
}

inline VDataInput & operator >>( VDataInput & dIn , VGUID & v )
{
    dIn >> v.Data1 >> v.Data2 >> v.Data3 ;
    dIn.Read8( v.Data4 , 8 );

    return dIn ;
}