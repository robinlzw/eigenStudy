#pragma once

#include <cassert>

class VFSegDeg
{
public :
    VFSegDeg()
    {}
    VFSegDeg( float denominator , float numerator )
        : mDen( denominator ) , mNum( numerator ) 
    {} 

public :
    bool BeforStart() const
    {
        return mNum < 0 ;
    }

    bool InSegment() const
    {
        return mNum >= 0.f && mNum <= 1.f ;
    }
    bool AfterEnd() const
    {
        return mNum > 1.f ;
    }

    bool IsExist() const
    {
        return mDen != 0.f ;
    }

    template< class T > 
    T ToPoint( const T & stPnt , const T & endPnt ) const
    {
        assert( IsExist() ) ;
        return ( stPnt * ( mDen - mNum ) + endPnt * mNum ) / mDen ;
    } 

public :
    float mDen ;
    float mNum ;
};

