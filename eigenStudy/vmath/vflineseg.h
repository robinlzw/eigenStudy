#pragma once

#include <utility>
#include "vmathpredef.h" 
#include "vfvector3.h"
 
struct VFLineSeg
{
public :
    VFVector3 mOrigin ;
    VFVector3 mTarget ;

public:
    VFLineSeg():mOrigin( VFVector3::ZERO ), mTarget( VFVector3::AXIS_Z ) {}
    VFLineSeg(const VFVector3& origin, const VFVector3& targ)
        :mOrigin(origin), mTarget(targ) {}

    float GetLength() const
    {
        return ( mTarget - mOrigin ).Magnitude() ;
    }
} ;
 