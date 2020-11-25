#pragma once

#include "vfvector2.h"

struct VF2DLocation
{
public:
    // construction
    VF2DLocation(){}
    VF2DLocation( const VFVECTOR2 & pos , const float xClockRadius )
        :position(pos),xclock(xClockRadius){} // 3点为0 逆时针为正 
    ~VF2DLocation(){}

public:
    VF2DLocation TransLocalPosToGlobal( const VF2DLocation & v ) const
    {
        float newxclock = v.xclock + xclock;
        VFVECTOR2 newpos = TransLocalVertexToGlobal( v.position );

        return VF2DLocation( newpos , newxclock );
    }

    VF2DLocation TransGlobalPosToLocal( const VF2DLocation & v ) const
    {
        float newxclock = v.xclock - xclock;
        VFVECTOR2 newpos = TransGlobalVertexToLocal( v.position );

        return VF2DLocation( newpos , newxclock );
    }

    VFVECTOR2 TransLocalVertexToGlobal( const VFVECTOR2 & v ) const
    {
        VFVECTOR2 rtValue;
        rtValue.x =   v.x * cos( -xclock ) + v.y * sin( -xclock );
        rtValue.y = - v.x * sin( -xclock ) + v.y * cos( -xclock );
        return rtValue + position;
    }

    VFVECTOR2 TransGlobalVertexToLocal( const VFVECTOR2 & v ) const
    {
        VFVECTOR2 rtValue;
        rtValue.x =   ( v - position ).x * cos( xclock ) + ( v - position ).y * sin( xclock );
        rtValue.y = - ( v - position ).x * sin( xclock ) + ( v - position ).y * cos( xclock );
        return rtValue;
    }

public:
    VFVECTOR2   position;
    float       xclock;

    // static variable 
    static const VF2DLocation ORIGIN ;
};