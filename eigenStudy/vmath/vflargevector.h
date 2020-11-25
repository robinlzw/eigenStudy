#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <functional>

class VFLargeVector
{
public: 
    VFLargeVector() ;
    VFLargeVector( unsigned len ) ;
    VFLargeVector( unsigned len , double initVal ) ;
    ~VFLargeVector(void);

public :
    unsigned GetLength() const                 ;
    double & operator[]( unsigned indx )       ;
    double   operator[]( unsigned indx ) const ;
    void operator *=( double r )               ;
    void operator +=( const VFLargeVector & v ) ;
    void operator -=( const VFLargeVector & v ) ;
    const VFLargeVector & operator = ( const VFLargeVector & src ) ;

    double Dot( const VFLargeVector & v ) ;
    double SqrtMag()                     ;

    void UsingBuffer( std::function< void ( const double * , unsigned ) > f ) const
    {
        f( & m_v[0] , m_v.size() ) ;
    }
    void ModifyBuffer( std::function< void ( double * , unsigned ) > f ) 
    {
        f( & m_v[0] , m_v.size() ) ;
    }

    void ModifyBuffer( unsigned newSize , std::function< void ( double * , unsigned ) > f ) 
    {
        m_v.resize( newSize ) ;
        f( & m_v[0] , m_v.size() ) ;
    }

    void ModifyBuffer( unsigned newSize ) 
    {
        m_v.resize( newSize ) ;
    }

    // rslt = v1 * r1 + v2 * r2 ;
    static void Multi( const VFLargeVector & v , double r , VFLargeVector & rslt ) ;
    static double MultiWithSqlLen( const VFLargeVector & v , double r , VFLargeVector & rslt ) ;
    static void MultiAdd( const VFLargeVector & v1 , double r1 
                        , const VFLargeVector & v2 , double r2 
                        , VFLargeVector & rslt ) ;
    // rslt = v1 - v2 
    static void Subtract( const VFLargeVector & v1 , const VFLargeVector & v2 , VFLargeVector & rslt ) ;
    // rslt = v1 + v2 
    static void Add( const VFLargeVector & v1 , const VFLargeVector & v2 , VFLargeVector & rslt ) ;


protected :
    std::vector< double >  m_v ;
};

inline
VFLargeVector::VFLargeVector()
{
}

inline
VFLargeVector::VFLargeVector( unsigned len )
{
    m_v.resize( len ) ;
}

inline
VFLargeVector::VFLargeVector( unsigned len , double initVal )
{
    m_v.assign( len , initVal ) ;
}

inline
VFLargeVector::~VFLargeVector(void)
{
}

inline
unsigned VFLargeVector::GetLength() const 
{
    return m_v.size() ;
}

inline
double & VFLargeVector::operator[]( unsigned indx )       
{
    assert( indx < m_v.size() ) ;
    return m_v[indx] ;
}

inline
double   VFLargeVector::operator[]( unsigned indx ) const     
{
    assert( indx < m_v.size() ) ;
    return m_v[indx] ;
}

inline
void VFLargeVector::operator *=( double r )         
{
    std::for_each( m_v.begin() , m_v.end() , [r]( double & x ){
        x *= r ;
    } ) ;
}

inline      
void VFLargeVector::operator +=( const VFLargeVector & v ) 
{
    for( unsigned i = 0 ; i < m_v.size() ;i ++ )
    {
        m_v[i] += v[i] ;
    }
}

inline
void VFLargeVector::operator -=( const VFLargeVector & v ) 
{
    for( unsigned i = 0 ; i < m_v.size() ;i ++ )
    {
        m_v[i] -= v[i] ;
    }
}
  
inline  
const VFLargeVector & VFLargeVector::operator = ( const VFLargeVector & src ) 
{
    assert( src.m_v.size() == m_v.size() ) ;

    m_v.assign( src.m_v.begin() , src.m_v.end() ) ;

    return *this ;
}
  
inline 
double VFLargeVector::Dot( const VFLargeVector & v ) 
{ 
    double rslt(0) ;

    for( unsigned i = 0 ; i < m_v.size() ;i ++ )
    {
        rslt += m_v[i] * v[i] ;
    }

    return rslt ;
}
  
inline
double VFLargeVector::SqrtMag()                     
{
    double rslt(0) ;

    std::for_each( m_v.begin() , m_v.end() , [&rslt]( double x ){
        rslt += x * x ;
    } ) ;

    return rslt ;
}
    
inline
void VFLargeVector::Multi( const VFLargeVector & v , double r , VFLargeVector & rslt ) 
{
    assert( v.GetLength() == rslt.GetLength() ) ;

    for( unsigned i = 0 ; i < v.GetLength() ;i ++ )
    {
        rslt[i] = v[i] * r ;
    }
}
    
inline
double VFLargeVector::MultiWithSqlLen( const VFLargeVector & v , double r , VFLargeVector & rslt ) 
{
    double vRtn(0) ;

    assert( v.GetLength() == rslt.GetLength() ) ;

    for( unsigned i = 0 ; i < v.GetLength() ;i ++ )
    {
        rslt[i] = v[i] * r ;
        vRtn += rslt[i] * rslt[i] ;
    }

    return vRtn ;
}

inline
void VFLargeVector::Subtract( const VFLargeVector & v1 , const VFLargeVector & v2 , VFLargeVector & rslt ) 
{
    assert( v1.GetLength() == v2.GetLength() && v1.GetLength() == rslt.GetLength()  ) ;

    for( unsigned i = 0 ; i < v1.GetLength() ;i ++ )
    {
        rslt[i] = v1[i] - v2[i] ;
    }
}

inline
    void VFLargeVector::Add( const VFLargeVector & v1 , const VFLargeVector & v2 , VFLargeVector & rslt ) 
{
    assert( v1.GetLength() == v2.GetLength() && v1.GetLength() == rslt.GetLength()  ) ;

    for( unsigned i = 0 ; i < v1.GetLength() ;i ++ )
    {
        rslt[i] = v1[i] + v2[i] ;
    }
}

inline
void VFLargeVector::MultiAdd( const VFLargeVector & v1 , double r1 
                           , const VFLargeVector & v2 , double r2 
                           , VFLargeVector & rslt ) 
{
    assert( v1.GetLength() == v2.GetLength() && v1.GetLength() == rslt.GetLength() ) ;

    for( unsigned i = 0 ; i < v1.GetLength() ; i ++ )
    {
        rslt[i] = v1[i] * r1 + v2[i] * r2 ;
    }
}
