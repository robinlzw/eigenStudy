#pragma once

#include "../vstd/dstrm.h"

// VNVector<>类模板——顶点索引序列。


//  
template< class T >
struct VNVector2
{
    T  x ;
    T  y ;

    VNVector2(){}
    VNVector2( T ix , T iy ): x(ix),y(iy){}
    const T &operator[](unsigned i) const { return  ((T*)(&x))[i]  ; }
    T &operator[](unsigned i)  { return  ((T*)(&x))[i]  ; }  
    const T * Ptr() const { return &x ; }
    T * Ptr() { return &x ; }
        
    VNVector2< T > operator +( const VNVector2< T > & v ) const { return VNVector2< T >( x + v.x , y + v.y ) ; }
    VNVector2< T > operator -( const VNVector2< T > & v ) const { return VNVector2< T >( x - v.x , y - v.y ) ; }
    VNVector2< T > operator +() const { return *this; } ;
    VNVector2< T > operator -() const { return VNVector2< T >( -x , -y ) ; }
    VNVector2< T > operator *( T v ) const { return VNVector2< T >( x * v , y * v ) ; }
    VNVector2< T > operator /( T v ) const { return VNVector2< T >( x / v , y / v ) ; }
    bool operator==( const VNVector2<T> & src ) const { return x == src.x && y == src.y ; } 
    bool operator!=( const VNVector2<T> & src ) const { return x != src.x || y != src.y ; } 
} ;



// 
template< class T >
struct VNVector3
{
    T  x ;
    T  y ;
    T  z ;


    VNVector3(){}
    VNVector3( T ix , T iy , T iz ): x(ix),y(iy),z(iz){}


    const T &operator[](unsigned i) const { return  ((T*)(&x))[i]  ; }
    T &operator[](unsigned i)  { return  ((T*)(&x))[i]  ; }  


    const T * Ptr() const { return &x ; }
    T * Ptr() { return &x ; }

    VNVector3< T > operator +( const VNVector3< T > & v ) const { return VNVector3< T >( x + v.x , y + v.y , z + v.z ) ; }
    VNVector3< T > operator -( const VNVector3< T > & v ) const { return VNVector3< T >( x - v.x , y - v.y , z - v.z ) ; }
    VNVector3< T > operator +() const { return *this; } ;
    VNVector3< T > operator -() const { return VNVector3< T >( -x , -y , -z ) ; }
    VNVector3< T > operator *( T v ) const { return VNVector3< T >( x * v , y * v , z * v ) ; }
    VNVector3< T > operator /( T v ) const { return VNVector3< T >( x / v , y / v , z / v ) ; }

    bool operator==( const VNVector3<T> & src ) const { return x == src.x && y == src.y && z == src.z ; } 
    bool operator!=( const VNVector3<T> & src ) const { return x != src.x || y != src.y || z != src.z ; } 
} ;



template< class T >
struct VNVector4
{
    T  x ;
    T  y ;
    T  z ;
    T  w ;
    VNVector4(){}
    VNVector4( T ix , T iy , T iz , T iw ): x(ix),y(iy),z(iz),w(iw){}
    const T &operator[](unsigned i) const { return  ((T*)(&x))[i]  ; }
    T &operator[](unsigned i)  { return  ((T*)(&x))[i]  ; } 
    const T * Ptr() const { return &x ; }
    T * Ptr() { return &x ; }
    bool operator==( const VNVector4<T> & src ) { return x == src.x && y == src.y && z == src.z && w == src.w ; } 
    bool operator!=( const VNVector4<T> & src ) { return x != src.x || y != src.y || z != src.z || w != src.w ; } 
} ;

typedef VNVector2< int      > VNVECTOR2I ;
typedef VNVector2< char     > VNVECTOR2B ;
typedef VNVector2< short    > VNVECTOR2S ;
typedef VNVector2< long     > VNVECTOR2L ;

typedef VNVector2< unsigned int      > VNVECTOR2UI ;
typedef VNVector2< unsigned char     > VNVECTOR2UB ;
typedef VNVector2< unsigned short    > VNVECTOR2US ;
typedef VNVector2< unsigned long     > VNVECTOR2UL ;

typedef VNVector3< int      > VNVECTOR3I ;
typedef VNVector3< char     > VNVECTOR3B ;
typedef VNVector3< short    > VNVECTOR3S ;
typedef VNVector3< long     > VNVECTOR3L ;

typedef VNVector3< unsigned int      > VNVECTOR3UI ;
typedef VNVector3< unsigned char     > VNVECTOR3UB ;
typedef VNVector3< unsigned short    > VNVECTOR3US ;
typedef VNVector3< unsigned long     > VNVECTOR3UL ;

typedef VNVector4< int      > VNVECTOR4I ;
typedef VNVector4< char     > VNVECTOR4B ;
typedef VNVector4< short    > VNVECTOR4S ;
typedef VNVector4< long     > VNVECTOR4L ;

typedef VNVector4< unsigned int      > VNVECTOR4UI ;
typedef VNVector4< unsigned char     > VNVECTOR4UB ;
typedef VNVector4< unsigned short    > VNVECTOR4US ;
typedef VNVector4< unsigned long     > VNVECTOR4UL ;

template< class T >
inline
VDataOutput & operator <<( VDataOutput & stm , const VNVector2< T > & data )
{ 
    stm << data.x << data.y ;
    return stm ;
}



template< class T >
inline
VDataInput & operator >>( VDataInput & stm , VNVector2< T > & data )
{
    stm >> data.x >> data.y ;
    return stm ;
}



template< class T >
inline
VDataOutput & operator <<( VDataOutput & stm , const VNVector3< T > & data )
{ 
    stm << data.x << data.y << data.z ;
    return stm ;
}

template< class T >
inline
VDataInput & operator >>( VDataInput & stm , VNVector3< T > & data )
{
    stm >> data.x >> data.y >> data.z ;
    return stm ;
}

template< class T >
inline
VDataOutput & operator <<( VDataOutput & stm , const VNVector4< T > & data )
{ 
    stm << data.x << data.y << data.z << data.w ;
    return stm ;
}



template< class T >
inline
VDataInput & operator >>( VDataInput & stm , VNVector4< T > & data )
{
    stm >> data.x >> data.y >> data.z >> data.w ;
    return stm ;
}

