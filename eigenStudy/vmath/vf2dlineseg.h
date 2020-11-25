#pragma once

#include "vfvector2.h"
#include "vfsegdeg.h"

class VF2DLineSeg
{
public :
    VFVector2 mOrigin ;
    VFVector2 mTarget ;
 
public:
    VF2DLineSeg()
    {}
    VF2DLineSeg(const VFVector2& origin, const VFVector2& targ)
        :mOrigin(origin), mTarget(targ) {}

    ~VF2DLineSeg(void)
    {
    } 

    float GetLength() const
    {
        return ( mTarget - mOrigin ).Magnitude() ;
    }

    VFVector2 GetStart() const
    {
        return mOrigin; 
    }

    VFVector2 GetEnd() const
    {
        return mTarget; 
    }

    VFVector2 GetDirection(void) const 
    {
        VFVector2 v = mTarget - mOrigin ;
        float len = v.Magnitude() ;

        if( len == 0 )
            return VFVector2::ORIGIN ;

        return v / len ;
    } 

    VFVector2 GetPosition( const VFSegDeg & deg ) const
    { 
        return deg.ToPoint( mOrigin , mTarget ) ;
    } 

    std::pair< VFSegDeg , VFSegDeg > Intersection( const VF2DLineSeg & other ) const
    {
        VFVECTOR2 u = mTarget - mOrigin ;
        VFVECTOR2 v = other.mTarget - other.mOrigin ;
        VFVECTOR2 w = other.mOrigin - mOrigin ;

        float den = u.Cross( v ) ;
        float a   = w.Cross( v ) ;
        float b   = w.Cross( u ) ;

        return std::pair< VFSegDeg , VFSegDeg >( VFSegDeg( den , a ) , VFSegDeg( den , b ) ) ;
    }
};

