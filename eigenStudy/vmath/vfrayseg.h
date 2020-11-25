#pragma once

#include <utility>
#include "vmathpredef.h" 
#include "vfvector3.h"

struct VFRaySeg 
{
public :
    VFVector3 mOrigin ;
    VFVector3 mTarget ;

public:
    VFRaySeg():mOrigin( VFVector3::ZERO ), mTarget( VFVector3::AXIS_Z ) {}
    VFRaySeg(const VFVector3& origin, const VFVector3& targ)
        :mOrigin(origin), mTarget(targ) {}

    float GetLength() const
    {
        return ( mTarget - mOrigin ).Magnitude() ;
    }

    float GetSqrLength() const
    {
        return ( mTarget - mOrigin ).SqrMagnitude() ;
    }

    VFVector3 GetStart() const
    {
        return mOrigin; 
    }

    VFVector3 GetEnd() const
    {
        return mTarget; 
    }

    VFVector3 GetDirection(void) const 
    {
        VFVector3 v = mTarget - mOrigin ;
        float len = v.Magnitude() ;

        if( len == 0 )
            return VFVECTOR3::ORIGIN ;

        return v / len ;
    } 

    VFVector3 GetPosition( float deg ) const
    {
        return mOrigin + ( mTarget - mOrigin ) * deg ;  //() * deg + mOrigin ;
    }

    float Project( const VFVECTOR3 & p ) const
    {
        VFVector3 v = mTarget - mOrigin ;
        float sl = v.SqrMagnitude() ;

		if(vf_appro_zero(sl))
        //if( sl == 0 )
            return 0.f ;
        float dt = v.Dot( p - mOrigin );

        return dt / sl ;
    }
} ;
