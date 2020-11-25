#pragma once

#include "vfvector2.h"
#include "vfvector3.h"

struct VFArealCoord3
{
    union
    {
        struct 
        {
            float v1 ;
            float v2 ;
            float v3 ;
        } ;

        float v[3] ;
    } ;

    VFArealCoord3()
    {}
    VFArealCoord3( const VFArealCoord3 & src )
        :v1(src.v1),v2(src.v2),v3(src.v3)
    {}
    VFArealCoord3( float a1 , float a2 , float a3 )
        :v1(a1),v2(a2),v3(a3) 
    {}

    //²Î¿¼£º http://en.wikipedia.org/wiki/Barycentric_coordinate_system
    VFArealCoord3( const VFVECTOR2 & p1 , const VFVECTOR2 & p2 , const VFVECTOR2 & p3 , const VFVECTOR2 & p )
    {
        FromTriangle( p1 , p2 , p3 , p ) ;
    }

    VFArealCoord3( const VFVECTOR3 & p1 , const VFVECTOR3 & p2 , const VFVECTOR3 & p3 , const VFVECTOR3 & p )
    {
        FromTriangle( p1 , p2 , p3 , p ) ;
    }

    ~VFArealCoord3(){}
    
    bool IsOuterPoint( float threshould = 0 ) const
    {
        return v1 < -threshould || v2 < -threshould || v3 < -threshould ;
    }
    
    bool IsOuterByEdge( unsigned i , float threshould = 0 ) const 
    {
        return v[i] < -threshould ;
    }

    void MakeInner()
    {
        if( v1 < 0.f ) v1 = 0.f ;
        if( v2 < 0.f ) v2 = 0.f ;
        if( v3 < 0.f ) v3 = 0.f ;
        
        float S = v1 + v2 + v3 ;

        if( S == 0.f )
        {
            v1 = 1.f / 3.f ;
            v2 = 1.f / 3.f ;
            v3 = 1.f / 3.f ;
        }
        else
        {
            v1 /= S ;
            v2 /= S ;
            v3 /= S ;
        }
    }

    void LocateToEdge( unsigned i )
    {
        float & v1 = v[(i+1)%3] ;
        float & v2 = v[(i+2)%3] ;

        float   t = v1 + v2 ;
        
        v[i] = 0.f ;

        if( t == 0.f ) 
        {
            v1 = 0.5f ;
            v2 = 0.5f ;
        }
        else
        {
            v1 = v1 / t ;
            v2 = v2 / t ;
        }
    } ;

    template< class VT >
    static VFVECTOR3 calcArea( const VT & p1 , const VT & p2 , const VT & p3 , const VT & p ) ;
    //{
    //    static_assert( 0 ,"Do not compress here£¡" ) ;

    //    return VFVECTOR3( 1.f , 0.f , 0.f ) ;
    //}

    
   	template< class VT >
	static bool IsOut(const VT & p1, const VT & p2, const VT & p3, const VT & p);

    template< class VT >
    static VFArealCoord3 FromTriangle( const VT & p1 , const VT & p2 , const VT & p3 , const VT & p )
    {
        VFVECTOR3 vs( calcArea( p1 , p2 , p3 , p ) ) ;
        float     S = vs.x + vs.y + vs.z ;
        VFArealCoord3 ac ;

        if( S == 0.f )
        {
			bool blIsOut = IsOut(p1, p2, p3, p);
			if (true == blIsOut)
			{
				ac.v1 = -1.0f;
				ac.v2 = -1.0f;
				ac.v3 = -1.0f;
			}
			else
			{
				ac.v1 = 1.f / 3.f;
				ac.v2 = 1.f / 3.f;
				ac.v3 = 1.f / 3.f;
			}
        }
        else
        {
            ac.v1 = vs.x / S ;
            ac.v2 = vs.y / S ;
            ac.v3 = vs.z / S ;
        }

        return ac ;
    }

    static VFArealCoord3 FromPoint( unsigned pntIndx )
    {
        VFArealCoord3 ac ;

        ac.v[pntIndx] = 1.f ;
        ac.v[(pntIndx+1)%3] = 0.f ;
        ac.v[(pntIndx+2)%3] = 0.f ; 

        return ac ;
    }
      
    static VFArealCoord3 FromLine( unsigned EdgeIdx , float degreeInEdge )
    {
        VFArealCoord3 ac ;

        ac.v[EdgeIdx] = 0 ;
        ac.v[(EdgeIdx+1)%3] = ( 1.f - degreeInEdge ) ;
        ac.v[(EdgeIdx+2)%3] = degreeInEdge ;

        return ac ; 
    }

    template< class VT >
    VT TransToPos( const VT & p1 , const VT & p2 , const VT & p3 ) const
    {
        return p1 * v1 + p2 * v2 + p3 * v3 ;
    } 
} ;

inline VDataOutput & operator <<( VDataOutput & dOut , const VFArealCoord3 & v )
{
    dOut << v.v1 << v.v2 << v.v3 ;

    return dOut ;
}

inline VDataInput & operator >>( VDataInput & dIn , VFArealCoord3 & v )
{
    dIn >> v.v1 >> v.v2 >> v.v3 ; 

    return dIn ;
}
template<>
inline VFVECTOR3 VFArealCoord3::calcArea( const VFVECTOR2 & p1 , const VFVECTOR2 & p2 , const VFVECTOR2 & p3 , const VFVECTOR2 & p )
{
    VFVECTOR2  d1( p - p1 ) , d2( p - p2 ) , d3( p - p3 ) ;
    return VFVECTOR3( d3.Cross( d2 ) , d1.Cross( d3 ) , d2.Cross( d1 ) ) ;
}

template<>
inline VFVECTOR3 VFArealCoord3::calcArea( const VFVECTOR3 & p1 , const VFVECTOR3 & p2 , const VFVECTOR3 & p3 , const VFVECTOR3 & p )
{
    VFVECTOR3  v1( p3 - p2 ) , v2( p1 - p3 ) , v3( p2 - p1 ) ;
    VFVECTOR3  d1( p - p1 ) , d2( p - p2 ) , d3( p - p3 ) ;
    
    return VFVECTOR3( d3.Cross( d2 ).Dot( v2.Cross( v3 ) )
                     , d1.Cross( d3 ).Dot( v3.Cross( v1 ) )
                     , d2.Cross( d1 ).Dot( v1.Cross( v2 ) ) ) ;
}

template<>
inline bool VFArealCoord3::IsOut(const VFVECTOR2 & p1, const VFVECTOR2 & p2, const VFVECTOR2 & p3, const VFVECTOR2 & p)
{
	VFVECTOR2 vMax = VFVECTOR2(-100000.0f, -100000.0f);
	VFVECTOR2 vMin = VFVECTOR2(100000.0f, 100000.0f);

	for (size_t i = 0; i < 2; i++)
	{
		if (vMax[i] < p1[i])
			vMax[i] = p1[i];
		if (vMin[i] > p1[i])
			vMin[i] = p1[i];
		if (vMax[i] < p2[i])
			vMax[i] = p2[i];
		if (vMin[i] > p2[i])
			vMin[i] = p2[i];
		if (vMax[i] < p3[i])
			vMax[i] = p3[i];
		if (vMin[i] > p3[i])
			vMin[i] = p3[i];
	}
	for (size_t i = 0; i < 2; i++)
	{
		if ((p[i] > vMax[i]) || (p[i] < vMin[i]))
			return true;
	}
	return false;
}

template<>
inline bool VFArealCoord3::IsOut(const VFVECTOR3 & p1, const VFVECTOR3 & p2, const VFVECTOR3 & p3, const VFVECTOR3 & p)
{
	VFVECTOR3 vMax = VFVECTOR3(-100000.0f, -100000.0f, -100000.0f);
	VFVECTOR3 vMin = VFVECTOR3(100000.0f, 100000.0f, 100000.0f);

	for (size_t i = 0; i < 3; i++)
	{
		if (vMax[i] < p1[i])
			vMax[i] = p1[i];
		if (vMin[i] > p1[i])
			vMin[i] = p1[i];
		if (vMax[i] < p2[i])
			vMax[i] = p2[i];
		if (vMin[i] > p2[i])
			vMin[i] = p2[i];
		if (vMax[i] < p3[i])
			vMax[i] = p3[i];
		if (vMin[i] > p3[i])
			vMin[i] = p3[i];
	}

	for (size_t i = 0; i < 3; i++)
	{
		if ((p[i] > vMax[i]) || (p[i] < vMin[i]))
			return true;
	}
	return false;
}
