#pragma once

#include "vfvector3.h"
#include "vnvector.h"
#include "vtopomesh.h"
#include "vfarealcoord.h"
#include "../vstd/strm.h"

// vfmshe.h——提供了网格数据结构


// TVSimpleGraph类
/*
	成员数据
			unsigned                nVertCount      ;
			unsigned                nTriangleCount  ;
			const VNVector3<IT>*	pTriangles      ;——法线向量
*/




// TVSimpleMesh类
/*
	继承于TVSimpleGraph
*/




// TVNormaledMesh
/*
*/


// TVMeshPoint
/*
*/



// TVSimpleGraph类——simpleMesh的基类，没有顶点数据。
template< class IT = unsigned >
struct TVSimpleGraph
{
    unsigned                nVertCount      ;
    unsigned                nTriangleCount  ;
    const VNVector3<IT>*	pTriangles      ;
    


	TVSimpleGraph()
	{
		nVertCount = 0;
		nTriangleCount = 0;
		pTriangles = NULL;
	}



    template< typename TP > static void MapTo( TP & mapper , const TVSimpleGraph< IT > & ti ){
        mapper.Append( VSConstBuffer< char >( ti.nTriangleCount * sizeof( VNVector3< IT > ) , reinterpret_cast< const char * >( ti.pTriangles ) ) ) ;
    }


    template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        const TVSimpleGraph< IT > * pgraph = reinterpret_cast< const TVSimpleGraph< IT > * >( pbuff.pData + objPos ) ;
        const VNVector3< IT > * pv = reinterpret_cast< const VNVector3< IT > * >( pbuff.pData + curPos ) ;
        r.Modify( objPos + ((size_t)&reinterpret_cast<char const volatile&>((((TVSimpleGraph<IT>*)0)->pTriangles)))
                , VSConstBuffer< char >( sizeof( const VNVector3< IT > * ) , reinterpret_cast< const char * >( &pv ) ) ) ;
        posEnd = curPos + pgraph->nTriangleCount * sizeof( VNVector3< IT > ) ;
    };
} ;




// 三维网格类，数据是只读的。而VSMesh中数据是可读写的，所以传递网格对象的时候用此类对象，如果要修改数据则用VSMesh对象。
template< class VT , class IT = unsigned >
struct TVSimpleMesh : TVSimpleGraph< IT >
{ 
    const VT              * pVertices ;			// 顶点坐标



	TVSimpleMesh()
	{
		TVSimpleGraph::TVSimpleGraph();
		pVertices = NULL;
	}
   


	// ？？？貌似mapper必须是TVPersist那一系列的类。
	template< typename TP > 
	static void MapTo( TP & mapper , const TVSimpleMesh< VT , IT > & ti )
	{
        TVSimpleGraph< IT >::MapTo( mapper , ti ) ;
        mapper.Append( VSConstBuffer< char >( ti.nVertCount * sizeof( VT ) , reinterpret_cast< const char * >( ti.pVertices ) ) ) ;
    }
   
	
	template< typename CP >
	static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        static_assert( ( reinterpret_cast< const char * >( ( TVSimpleMesh< VT , IT > * )(0) ) 
                     == reinterpret_cast< const char * >( (TVSimpleGraph< IT >     * )(0) ) ) , "Stucture Offset Error！") ;
        TVSimpleGraph< IT >::CorrectPtr( r , posEnd , pbuff , objPos , curPos ) ;

        const TVSimpleMesh< VT , IT > * pmesh = reinterpret_cast< const TVSimpleMesh< VT , IT > * >( pbuff.pData + objPos ) ;
        const VT * pv = reinterpret_cast< const VT * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + ((size_t)&reinterpret_cast<char const volatile&>((((TVSimpleMesh<VT,IT>*)0)->pVertices)))
                , VSConstBuffer< char >( sizeof( const VT * ) , reinterpret_cast< const char * >( &pv ) ) ) ;
        posEnd += pmesh->nVertCount * sizeof( VT ) ;
    };

} ;




template< class VT , class NT = VT , class IT = unsigned >
struct TVNormaledMesh : TVSimpleMesh< VT , IT >
{
    const NT              * pNormals       ;

    template< typename TP > static void MapTo( TP & mapper , const TVNormaledMesh< VT , NT , IT > & ti ){
        TVSimpleMesh< VT , IT >::MapTo( mapper , ti ) ;
        mapper.Append( VSConstBuffer< char >( ti.nVertCount * sizeof( NT ) , reinterpret_cast< const char * >( ti.pNormals ) ) ) ;
    }
    template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        static_assert( ( reinterpret_cast< const char * >( ( TVNormaledMesh< VT , NT , IT > * )(0) ) 
                     == reinterpret_cast< const char * >( (TVSimpleMesh< VT , IT >     * )(0) ) ) , "Stucture Offset Error！") ;
        TVSimpleMesh< VT , IT >::CorrectPtr( r , posEnd , pbuff , objPos , curPos ) ;
      
        const TVNormaledMesh< VT , NT , IT > * pmesh = reinterpret_cast< const TVNormaledMesh< VT , NT , IT > * >( pbuff.pData + objPos ) ;
        const NT * pv = reinterpret_cast< const NT * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + ((size_t)&reinterpret_cast<char const volatile&>((((TVNormaledMesh<VT,IT>*)0)->pNormals)))
                , VSConstBuffer< char >( sizeof( const NT * ) , reinterpret_cast< const char * >( &pv ) ) ) ;
        posEnd += pmesh->nVertCount * sizeof( NT ) ;
    };
} ; 

typedef TVSimpleGraph< unsigned >    VSSimpleGraph   ;
typedef TVSimpleMesh< VFVECTOR3 >    VSSimpleMeshF   ;
typedef TVNormaledMesh< VFVECTOR3 >  VSNormaledMeshF ;




struct VSMeshPoint
{
    static const unsigned INVALIDINDX = VD_INVALID_INDEX ;
    unsigned        triangleIndx  ;
    VFArealCoord3   posInTriangle ;
} ; 




struct VSPerfTopoGraph : VTopoGraph
{  
    const VNVECTOR3UI * pSurfaces ; 

    template< typename TP > static void MapTo( TP & mapper , const VSPerfTopoGraph & ti ){
        VTopoGraph::MapTo( mapper , ti ) ;
        mapper.Append( VSConstBuffer< char >( ti.GetSurfCount() * sizeof( VNVECTOR3UI ) , reinterpret_cast< const char * >( ti.pSurfaces ) ) ) ; 
    }
    template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        static_assert( ( reinterpret_cast< const char * >( ( VSPerfTopoGraph * )(0) ) 
                     == reinterpret_cast< const char * >( ( VTopoGraph    * )(0) ) ) , "Stucture Offset Error！") ;
        VTopoGraph::CorrectPtr( r , posEnd , pbuff , objPos , curPos ) ;
      
        const VSPerfTopoGraph * pmesh = reinterpret_cast< const VSPerfTopoGraph * >( pbuff.pData + objPos ) ;
        const VNVECTOR3UI * pvn = reinterpret_cast< const VNVECTOR3UI * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + offsetof( VSPerfTopoGraph , pSurfaces )
                , VSConstBuffer< char >( sizeof( const VNVECTOR3UI * ) , reinterpret_cast< const char * >( &pvn ) ) ) ;
        posEnd += pmesh->GetSurfCount() * sizeof( VNVECTOR3UI ) ;
    };
} ;




struct VSGraphMesh : VSPerfTopoGraph
{  
    VSGraphMesh(){ pVertices = 0 ; } 
    const VFVECTOR3     * pVertices ;   

    template< typename TP > static void MapTo( TP & mapper , const VSGraphMesh & ti ){
        VSPerfTopoGraph::MapTo( mapper , ti ) ;
        mapper.Append( VSConstBuffer< char >( ti.GetVertexCount() * sizeof( VFVECTOR3 ) , reinterpret_cast< const char * >( ti.pVertices ) ) ) ; 
    }
    template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        static_assert( ( reinterpret_cast< const char * >( ( VSGraphMesh * )(0) ) 
                     == reinterpret_cast< const char * >( ( VSPerfTopoGraph    * )(0) ) ) , "Stucture Offset Error！") ;
        VSPerfTopoGraph::CorrectPtr( r , posEnd , pbuff , objPos , curPos ) ;
      
        const VSGraphMesh * pmesh = reinterpret_cast< const VSGraphMesh * >( pbuff.pData + objPos ) ;
        const VFVECTOR3 * pvn = reinterpret_cast< const VFVECTOR3 * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + offsetof( VSGraphMesh , pVertices )
                , VSConstBuffer< char >( sizeof( const VFVECTOR3 * ) , reinterpret_cast< const char * >( &pvn ) ) ) ;
        posEnd += pmesh->GetVertexCount() * sizeof( VFVECTOR3 ) ;
    };
} ; 



// VSPerfectMesh类————带拓扑信息的网格
struct VSPerfectMesh : VSGraphMesh
{  
	const VFVECTOR3     * pVertNormals;
	const VFVECTOR3     * pSurfNormals;


    VSPerfectMesh(){ pVertNormals = 0 , pSurfNormals = 0 ; }

    template< typename TP > 
	static void MapTo( TP & mapper , const VSPerfectMesh & ti ){
        VSGraphMesh::MapTo( mapper , ti ) ;
        mapper.Append( VSConstBuffer< char >( ti.GetVertexCount() * sizeof( VFVECTOR3 ) , reinterpret_cast< const char * >( ti.pVertNormals ) ) ) ;
        mapper.Append( VSConstBuffer< char >( ti.GetSurfCount  () * sizeof( VFVECTOR3 ) , reinterpret_cast< const char * >( ti.pSurfNormals ) ) ) ;
    }
    
	
	template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        static_assert( ( reinterpret_cast< const char * >( ( VSPerfectMesh * )(0) ) 
                     == reinterpret_cast< const char * >( ( VSGraphMesh    * )(0) ) ) , "Stucture Offset Error！") ;
        VSGraphMesh::CorrectPtr( r , posEnd , pbuff , objPos , curPos ) ;
      
        const VSPerfectMesh * pmesh = reinterpret_cast< const VSPerfectMesh * >( pbuff.pData + objPos ) ;
        const VFVECTOR3 * pvn = reinterpret_cast< const VFVECTOR3 * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + offsetof( VSPerfectMesh , pVertNormals )
                , VSConstBuffer< char >( sizeof( const VFVECTOR3 * ) , reinterpret_cast< const char * >( &pvn ) ) ) ;
        posEnd += pmesh->GetVertexCount() * sizeof( VFVECTOR3 ) ;
        
        pvn = reinterpret_cast< const VFVECTOR3 * >( pbuff.pData + posEnd ) ;
        r.Modify( objPos + offsetof( VSPerfectMesh , pSurfNormals )
                , VSConstBuffer< char >( sizeof( const VFVECTOR3 * ) , reinterpret_cast< const char * >( &pvn ) ) ) ;
        posEnd += pmesh->GetSurfCount() * sizeof( VFVECTOR3 ) ;
    };
} ;




template< class TBase >
class TVPerfTopoGraphAdaptor : public TVGraphAdaptor< TBase >
{
public :
    TVPerfTopoGraphAdaptor(){}


    TVPerfTopoGraphAdaptor( const VSPerfTopoGraph & gm )
        : TVGraphAdaptor< TBase >( gm )
    {
        static_assert( std::is_base_of< VSPerfTopoGraph , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfTopoGraph * >( this )->pSurfaces = gm.pSurfaces ; 
    }


    TVPerfTopoGraphAdaptor( const VTopoGraph & gm , const VNVECTOR3UI * pSurf )
        : TVGraphAdaptor< TBase >( gm )
    {
        static_cast< VSPerfTopoGraph * >( this )->pSurfaces = pSurf ; 
    }



    void Redirect( const VSPerfTopoGraph & gm ) 
    {
        static_assert( std::is_base_of< VSPerfTopoGraph , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 

        TVGraphAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSPerfTopoGraph * >( this )->pSurfaces = gm.pSurfaces ; 
    }


    void Redirect( const VTopoGraph & gm , const VNVECTOR3UI * pSurf ) 
    {
        TVGraphAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSPerfTopoGraph * >( this )->pSurfaces = pSurf ; 
    }
} ;




template< class TBase >
class TVGraphMeshAdaptor : public TVPerfTopoGraphAdaptor< TBase >
{
public :
    TVGraphMeshAdaptor() {}

    TVGraphMeshAdaptor( const VSGraphMesh & gm )
        : TVPerfTopoGraphAdaptor< TBase >( gm )
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        static_cast< VSGraphMesh * >( this )->pVertices = gm.pVertices ;
    }
    TVGraphMeshAdaptor( const VSPerfTopoGraph & gm , const VFVECTOR3 * lpVert )
        : TVPerfTopoGraphAdaptor< TBase >( gm )
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        static_cast< VSGraphMesh * >( this )->pVertices = lpVert ;
    }
    TVGraphMeshAdaptor( const VSGraphMesh & gm , const VNVECTOR3UI * pSurf )
        : TVPerfTopoGraphAdaptor< TBase >( gm , pSurf )
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        static_cast< VSGraphMesh * >( this )->pVertices = gm.pVertices ;
    }
    TVGraphMeshAdaptor( const VTopoGraph & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * lpVert )
        : TVPerfTopoGraphAdaptor< TBase >( gm , pSurf )
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        static_cast< VSGraphMesh * >( this )->pVertices = lpVert ;
    }

    void Redirect( const VSGraphMesh & gm ) 
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        TVPerfTopoGraphAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSGraphMesh * >( this )->pVertices = gm.pVertices ;
    }
    void Redirect( const VSPerfTopoGraph & gm , const VFVECTOR3 * lpVert ) 
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        TVPerfTopoGraphAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSGraphMesh * >( this )->pVertices = lpVert ;
    }
    void Redirect( const VSGraphMesh & gm , const VNVECTOR3UI * pSurf ) 
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        TVPerfTopoGraphAdaptor< TBase >::Redirect( gm , pSurf ) ;
        static_cast< VSGraphMesh * >( this )->pVertices = gm.pVertices ;
    }
    void Redirect( const VTopoGraph & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * lpVert )
    {
        static_assert( std::is_base_of< VSGraphMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ;  
        TVPerfTopoGraphAdaptor< TBase >::Redirect( gm , pSurf ) ;
        static_cast< VSGraphMesh * >( this )->pVertices = lpVert ;
    }
} ;




template< class TBase >
class TVPerfectMeshAdaptor : public TVGraphMeshAdaptor< TBase >
{
public :
    TVPerfectMeshAdaptor(){}

    TVPerfectMeshAdaptor( const VSPerfectMesh & gm )
        : TVGraphMeshAdaptor< TBase >( gm )
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfectMesh * >( this )->pVertNormals = gm.pVertNormals ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = gm.pSurfNormals ;
    }

    TVPerfectMeshAdaptor( const VSGraphMesh & gm , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN )
        : TVGraphMeshAdaptor< TBase >( gm ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }

    TVPerfectMeshAdaptor( const VSPerfTopoGraph & gm , const VFVECTOR3 * lpVert , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN )
        : TVGraphMeshAdaptor< TBase >( gm , lpVert ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }

    TVPerfectMeshAdaptor( const VSGraphMesh & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN )
        : TVGraphMeshAdaptor< TBase >( gm , pSurf ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }
    TVPerfectMeshAdaptor( const VTopoGraph & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * lpVert , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN )
        : TVGraphMeshAdaptor< TBase >( gm , pSurf , lpVert ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }

    void Redirect( const VSPerfectMesh & gm )
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        TVGraphMeshAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSPerfectMesh * >( this )->pVertNormals = gm.pVertNormals ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = gm.pSurfNormals ;
    }
    void Redirect( const VSGraphMesh & gm , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        TVGraphMeshAdaptor< TBase >::Redirect( gm ) ;
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }
    void Redirect( const VSPerfTopoGraph & gm , const VFVECTOR3 * lpVert , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        TVGraphMeshAdaptor< TBase >::Redirect( gm , lpVert ) ;
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }
    void Redirect( const VSGraphMesh & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        TVGraphMeshAdaptor< TBase >::Redirect( gm , pSurf ) ;
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }
    void Redirect( const VTopoGraph & gm , const VNVECTOR3UI * pSurf , const VFVECTOR3 * lpVert , const VFVECTOR3 * pVN , const VFVECTOR3 * pSN ) 
    {
        static_assert( std::is_base_of< VSPerfectMesh , TBase >::value , "TBase mast derived from VTopoGraph " ) ; 
        TVGraphMeshAdaptor< TBase >::Redirect( gm , pSurf , lpVert ) ;
        static_cast< VSPerfectMesh * >( this )->pVertNormals = pVN ;
        static_cast< VSPerfectMesh * >( this )->pSurfNormals = pSN ;
    }
} ;
