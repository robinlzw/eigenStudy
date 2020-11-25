#pragma once

#include <vstd/obj.h"
#include "vfsparsematrix.h"

//参考资料： http://zh.wikipedia.org/zh-cn/稳定双共轭梯度法

template< class TMatrix >
class VAlgBiCGSTAB
{
public :
    VINTERFACE IDataProvider
    {
        virtual void FillData( VFLargeVector  & vb
                             , VFLargeVector  & vx )         = 0 ;
    } ;

public:
    VAlgBiCGSTAB( const TMatrix & cm , const VFLargeVector & vb , VFLargeVector & vx ) ;
    VAlgBiCGSTAB( const TMatrix & cm , const VFLargeVector & vb , VFLargeVector & vx , std::function< void (const VFLargeVector & ,VFLargeVector&)> fnInitR0 ) ;
    ~VAlgBiCGSTAB(void);

public :
    const VFLargeVector & Resolve( unsigned maxStep , double threshold ) ;
    const VFLargeVector & GetCurrentResult() const ;
    bool  NextStep( double threshold ) ;

public :
    // 方程：m_matA * x = m_vecB
    const TMatrix & mA ;
    const VFLargeVector  & vB ;
    VFLargeVector        & vX ;
    VFLargeVector   vR0   ;
    VFLargeVector   vR    ;
    VFLargeVector   vV    ;
    VFLargeVector   vP    ;
    VFLargeVector   vS    ;
    VFLargeVector   vT    ;
    double         ro    ;
    double         alpha ;
    double         omiga ;
};

template< class TMatrix >
inline VAlgBiCGSTAB< TMatrix >::VAlgBiCGSTAB( const TMatrix & cm , const VFLargeVector & vb , VFLargeVector & vx ) 
    : mA( cm ) , vB( vb ) , vX( vx ) 
    , vV( vx.GetLength() , 0 ) 
    , vP( vx.GetLength() , 0 )
    , vR( vx.GetLength() )
    , vR0( vx.GetLength() )
    , ro( 1 )
    , alpha( 1 )
    , omiga( 1 )
    , vS( vx.GetLength() ) 
    , vT( vx.GetLength() )
{  
    cm.Multi( vx , vR ) ;
    VFLargeVector::MultiAdd( vb , 1 , vR , -1 , vR0 ) ; 
    vR = vR0 ;
}

template< class TMatrix >
inline VAlgBiCGSTAB< TMatrix >::VAlgBiCGSTAB( const TMatrix & cm , const VFLargeVector & vb , VFLargeVector & vx , std::function< void (const VFLargeVector & , VFLargeVector&)> fnInitR0 ) 
    : mA( cm ) , vB( vb ) , vX( vx ) 
    , vV( vx.GetLength() , 0 ) 
    , vP( vx.GetLength() , 0 )
    , vR( vx.GetLength() )
    , vR0( vx.GetLength() )
    , ro( 1 )
    , alpha( 1 )
    , omiga( 1 )
    , vS( vx.GetLength() ) 
    , vT( vx.GetLength() )
{  
    cm.Multi( vx , vR ) ;
    VFLargeVector::MultiAdd( vb , 1 , vR , -1 , vR0 ) ; 
    vR = vR0 ;
    fnInitR0( vR0 , vR ) ;
}

template< class TMatrix >
inline VAlgBiCGSTAB< TMatrix >::~VAlgBiCGSTAB()
{
}
    
template< class TMatrix >
inline const VFLargeVector & VAlgBiCGSTAB< TMatrix >::Resolve( unsigned maxStep , double threshold ) 
{
    for( unsigned i = 0 ; i < maxStep ; i ++ )
    { 
        if( NextStep( threshold ) )
            break ;
    }

    return vX ;
}

template< class TMatrix >
inline const VFLargeVector & VAlgBiCGSTAB< TMatrix >::GetCurrentResult() const 
{
    return vX ;
}

template< class TMatrix >
inline bool  VAlgBiCGSTAB< TMatrix >::NextStep( double threshold ) 
{
    double roN = vR0.Dot( vR ) ;

    if( roN <= threshold && roN >= -threshold ) 
        return true ;

    double beta = ( roN / ro ) * ( alpha / omiga ) ;

    ro = roN    ;
    vV *= omiga ;
    vP -= vV    ;
    vP *= beta  ;
    vP += vR    ;

    mA.Multi( vP , vV ) ;

    alpha = ro / vR0.Dot( vV ) ;
    
    VFLargeVector::Multi( vV , -alpha , vS ) ;
    vS += vR ; 

    mA.Multi( vS , vT ) ;

    omiga = vT.Dot( vS ) / vT.SqrtMag() ;

    VFLargeVector::Multi( vT , -omiga , vR ) ;
    vR += vS ;

    VFLargeVector::MultiAdd( vP , alpha , vS , omiga , vT ) ;

    vX += vT ;

    double fLen = vT.SqrtMag() ;

    return fLen <= threshold ;
}
