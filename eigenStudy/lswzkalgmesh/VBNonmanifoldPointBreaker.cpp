#include "stdafx.h"
#include "VBNonmanifoldPointBreaker.h"

void VBNonmanifoldPointBreaker::Build( VSSimpleMeshF & smsh , const VSSimpleMeshF & msh )
{
    m_vVertices.resize( msh.nVertCount );
    memcpy( &m_vVertices[ 0 ] , msh.pVertices , sizeof( VFVECTOR3 ) * msh.nVertCount );
    m_vSurfaces.resize( msh.nTriangleCount );
    memcpy( &m_vSurfaces[ 0 ] , msh.pTriangles , sizeof( VNVECTOR3UI ) * msh.nTriangleCount );

    VSPerfectMesh pm = m_PerfMesh.Run( msh ).Get<VSPerfectMesh>();

    unsigned vc = pm.GetVertexCount();
    for ( unsigned i = 0; i < vc; i++ )
    {
        unsigned aroundTriCount = pm.GetSurfCountOfVetex( i );
        std::vector< std::pair< unsigned , unsigned > > triIdxs; triIdxs.resize( aroundTriCount );
        std::vector< VNVECTOR3UI > triEdgeIdxs; triEdgeIdxs.resize( aroundTriCount );
        for ( unsigned j = 0; j < aroundTriCount; j++ )
        {
            unsigned triidx = pm.GetSurfIndxOfVertex( i , j );
            triIdxs[ j ] = std::make_pair( triidx , j );
            triEdgeIdxs[ j ] = pm.GetEdgeIndxOfSurf( triidx );
        }

        for ( unsigned j = 0; j < aroundTriCount; j++ )
        {
            VNVECTOR2UI twotri = pm.GetSurfIndxOfEdge( triEdgeIdxs[ j ].x );
            unsigned firstFind = 4294967295U;
            for ( unsigned k = 0; k < triIdxs.size(); k++ )
            {
                if ( triIdxs[ k ].first == twotri.x || triIdxs[ k ].first == twotri.y )
                {
                    if ( firstFind == 4294967295U )
                    {
                        firstFind = triIdxs[ k ].second;
                    }
                    else
                    {
                        unsigned oldgroup = triIdxs[ k ].second;
                        unsigned newgroup = firstFind;
                        for ( unsigned p = 0; p < triIdxs.size(); p++ )
                        {
                            if ( triIdxs[ p ].second == oldgroup )
                            {
                                triIdxs[ p ].second = newgroup;
                            }
                        }
                    }
                }
            }

            twotri = pm.GetSurfIndxOfEdge( triEdgeIdxs[ j ].y );
            firstFind = 4294967295U;
            for ( unsigned k = 0; k < triIdxs.size(); k++ )
            {
                if ( triIdxs[ k ].first == twotri.x || triIdxs[ k ].first == twotri.y )
                {
                    if ( firstFind == 4294967295U )
                    {
                        firstFind = triIdxs[ k ].second;
                    }
                    else
                    {
                        unsigned oldgroup = triIdxs[ k ].second;
                        unsigned newgroup = firstFind;
                        for ( unsigned p = 0; p < triIdxs.size(); p++ )
                        {
                            if ( triIdxs[ p ].second == oldgroup )
                            {
                                triIdxs[ p ].second = newgroup;
                            }
                        }
                    }
                }
            }

            twotri = pm.GetSurfIndxOfEdge( triEdgeIdxs[ j ].z );
            firstFind = 4294967295U;
            for ( unsigned k = 0; k < triIdxs.size(); k++ )
            {
                if ( triIdxs[ k ].first == twotri.x || triIdxs[ k ].first == twotri.y )
                {
                    if ( firstFind == 4294967295U )
                    {
                        firstFind = triIdxs[ k ].second;
                    }
                    else
                    {
                        unsigned oldgroup = triIdxs[ k ].second;
                        unsigned newgroup = firstFind;
                        for ( unsigned p = 0; p < triIdxs.size(); p++ )
                        {
                            if ( triIdxs[ p ].second == oldgroup )
                            {
                                triIdxs[ p ].second = newgroup;
                            }
                        }
                    }
                }
            }
        }

        std::vector< unsigned > groups;
        for ( unsigned j = 0; j < aroundTriCount; j++ )
        {
            bool exist = false;
            for ( unsigned k = 0; k < groups.size(); k++ )
            {
                if ( triIdxs[ j ].second == groups[ k ] )
                {
                    exist = true;
                    break;
                }
            }
            if ( !exist )
            {
                groups.push_back( triIdxs[ j ].second );
            }
        }

        for ( unsigned j = 1; j < groups.size(); j++ )
        {
            unsigned oldIndex = i;
            unsigned newIndex = m_vVertices.size();
            for ( unsigned k = 0; k < triIdxs.size(); k++ )
            {
                if ( triIdxs[ k ].second == groups[ j ] )
                {
                    VNVECTOR3UI tri = m_vSurfaces[ triIdxs[ k ].first ];
                    if ( tri.x == oldIndex )
                    {
                        m_vSurfaces[ triIdxs[ k ].first ].x = newIndex;
                    }
                    else if ( tri.y == oldIndex )
                    {
                        m_vSurfaces[ triIdxs[ k ].first ].y = newIndex;
                    }
                    else if ( tri.z == oldIndex )
                    {
                        m_vSurfaces[ triIdxs[ k ].first ].z = newIndex;
                    }
                    else
                    {
                        VASSERT( 0 && "error" );
                    }
                }
            }
            m_vVertices.push_back( m_vVertices[ i ] );
        }
    }

    smsh.nVertCount = m_vVertices.size();
    smsh.pVertices = &m_vVertices[ 0 ];
    smsh.nTriangleCount = m_vSurfaces.size();
    smsh.pTriangles = &m_vSurfaces[ 0 ];
}

VD_BEGIN_PIPELINE( VPLNonmanifoldPointBreakerCreator , VSESS< VNALGMESH::VRPerfMesh > );
    VDDP_NOD2( NonmanifoldPointBreaker , VBNonmanifoldPointBreaker );
VD_END_PIPELINE();

class VMNonmanifoldPointBreaker
{
public:
    VMNonmanifoldPointBreaker( VPLNonmanifoldPointBreakerCreator & pl
                               , IVSlot< VSSimpleMeshF > & sltHoleMesh
                               , IVInputPort< VSSimpleMeshF > & ipFullMesh )
    {
        pl.CreateNonmanifoldPointBreaker( m_NonmanifoldPointBreaker , pl.GetEnv().Get<VSESS< VNALGMESH::VRPerfMesh >>() , sltHoleMesh );
        VLNK( ipFullMesh , m_NonmanifoldPointBreaker );
    }

    ~VMNonmanifoldPointBreaker(){}

private:
    VPLNonmanifoldPointBreakerCreator::NonmanifoldPointBreaker m_NonmanifoldPointBreaker;
};

struct VTraitSysNonmanifoldPointBreakerCreator
{
    typedef VPLNonmanifoldPointBreakerCreator PIPELINE;
    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGMESH::VRNonmanifoldPointBreaker >
    {
        typedef VMNonmanifoldPointBreaker mngr_type;
    };
};

VD_EXPORT_SYSTEM_SYSTEM_SIMPLE( VTraitSysNonmanifoldPointBreakerCreator , VNALGMESH::VRNonmanifoldPointBreakerCreator );