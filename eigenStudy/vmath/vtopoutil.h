#pragma once

#include "vtopomesh.h" 
#include "vfvector3.h"

class VTopoGraphUtil
{
public :
    template< class VAT , typename F >
    static void BatchGetSurfNormals( const VTopoGraph & graph , VAT & pVertBuffer , F f )
    { 
        for( unsigned i = 0 ; i < graph.GetTriangleCount() ; i ++ )
        {
            f( i , CalcSurfaceNormal( graph , pVertBuffer , i ) ) ;
        }
    }  

    template< class VAT >
    static VFVECTOR3 CalcSurfaceNormal( const VNVECTOR3UI & tri , VAT &pVertBuffer )
    {
        //const VNVECTOR3UI & tri = pSurfBuffer[ srfIndx ] ;  // graph.GetVertIndxOfSurf( surfIndx ) ;

        //VASSERT( tri[0] < pVertBuffer.GetCount() ) ;
        //VASSERT( tri[1] < pVertBuffer.GetCount() ) ;
        //VASSERT( tri[2] < pVertBuffer.GetCount() ) ;

        const VFVECTOR3 & vs = pVertBuffer[tri[0]] ;
        const VFVECTOR3 & v  = pVertBuffer[tri[1]] ;
        const VFVECTOR3 & ve = pVertBuffer[tri[2]] ;

        VFVECTOR3 u1( vs - v ) , u2( ve - v ) ;

        if( u1.IsZero(0) || u2.IsZero(0) )
        {
            //ATLTRACE( "有畸变三胶片！\r\n" ) ;
            //VASSERT(0);
            return VFVECTOR3::ZERO ;
        } 

        u1.Normalize() ;
        u2.Normalize() ;

        VFVECTOR3 d = u2.Cross(u1)  ;

        if( d.IsZero(0) ) 
        {
            //ATLTRACE( "有畸变三胶片！\r\n" ) ;
            //VASSERT(0);
            return VFVECTOR3::ZERO ;
        }

        return d.Direction() ;
    } 
    
    template< class VAT , class SAT >
    static VFVECTOR3 CalcVertexNormal( const VTopoGraph & graph , VAT &pVertBuffer  , SAT &pIndxBuff , unsigned indx )
    {
        VFVECTOR3 norm( VFVECTOR3::ZERO ) ;
        unsigned triCount = graph.GetSurfCountOfVetex( indx ) ;

        for( unsigned i = 0 ; i < triCount ; i ++ )
        {
            auto ip = graph.GetSurfOfVertex( indx , i ) ; 
            const VNVECTOR3UI & ta = pIndxBuff[ ip.surfIndx ] ;
            const VFVECTOR3 & viSt = pVertBuffer[ ta[ ( ip.vertInSurf + 2 ) % 3 ] ] ;
            const VFVECTOR3 & viEd = pVertBuffer[ ta[ ( ip.vertInSurf + 1 ) % 3 ] ] ;

            norm += calcWeightedSurfNorm( viSt , pVertBuffer[indx] , viEd ) ;
        }

        if( norm.IsZero() ) 
            return VFVECTOR3::AXIS_Z ;

        norm.Normalize() ;

        return norm ;
    }

    //static VFVECTOR3 CalcVertexNormal( const VTopoGraph & graph , const VFVECTOR3 * pVertBuffer  , const VNVECTOR3UI * pIndxBuff , unsigned indx )
    //{
    //    VFVECTOR3 norm( VFVECTOR3::ZERO ) ;
    //    unsigned triCount = graph.GetSurfCountOfVetex( indx ) ;

    //    for( unsigned i = 0 ; i < triCount ; i ++ )
    //    {
    //        auto ip = graph.GetSurfOfVertex( indx , i ) ; 
    //        const VNVECTOR3UI & ta = pIndxBuff[ ip.surfIndx ] ;
    //        const VFVECTOR3 & viSt = pVertBuffer[ ta[ ( ip.vertInSurf + 2 ) % 3 ] ] ;
    //        const VFVECTOR3 & viEd = pVertBuffer[ ta[ ( ip.vertInSurf + 1 ) % 3 ] ] ;

    //        norm += calcWeightedSurfNorm( viSt , pVertBuffer[indx] , viEd ) ;
    //    }

    //    if( norm.IsZero() ) 
    //        return VFVECTOR3::AXIS_Z ;

    //    norm.Normalize() ;

    //    return norm ;
    //}

    //static bool IntersectRayNearPoint( const VTopoGraph & graph , const VFVECTOR3 * pVerts , const VSMeshSelResult_NearPoint & np  , /*out*/ VSMeshPoint & mp ) ;

private :
    static VFVECTOR3 calcWeightedSurfNorm( const VFVECTOR3 & vs , const VFVECTOR3 & v , const VFVECTOR3 & ve ) 
    {
        VFVECTOR3 u1( vs - v ) , u2( ve - v ) ;

        if( u1.IsZero(0) || u2.IsZero(0) )
        {
            //ATLTRACE( "有畸变三胶片！\r\n" ) ;
            return VFVECTOR3::ZERO ;
        } 

        u1.Normalize() ;
        u2.Normalize() ;

        VFVECTOR3 d = u2.Cross(u1)  ;

        if( d.IsZero(0) ) 
        {
            //ATLTRACE( "有畸变三胶片！\r\n" ) ;
            return VFVECTOR3::ZERO ;
        }

        d.Normalize() ;

        assert( u2.Dot( d.Cross( u1 ) ) < 0.f ) ;

        return d * VFMath::ACos( u1.Dot( u2 ) ).valueRadians() ;
        //return d * acosf( u1.Dot( u2 ) ) ;
    }  
} ;

