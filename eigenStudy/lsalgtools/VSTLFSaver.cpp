#include "StdAfx.h"
#include "VSTLFSaver.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>

VSTLFSaver::VSTLFSaver( const std::string & fileName , bool bAscii )
    : m_strFileName( fileName ) 
    , m_bAsciiMode( bAscii )
{
}


VSTLFSaver::~VSTLFSaver(void)
{
}
    
bool VSTLFSaver::Process( const VSSimpleMeshF & mesh ) 
{
    if ( m_bAsciiMode )
    {
        const VFVECTOR3 *   Vertex = mesh.pVertices ;
        const VNVECTOR3UI * Index  = mesh.pTriangles;

        std::ofstream fout( m_strFileName ) ;

        //std::string m = (LPSTR)(LPCTSTR)m_strFileName.c_str() ;
        const char name[] = "BRACKET" ;

        fout<< "solid "<< name << "\r" ;

        for ( unsigned i(0) ; i< mesh.nTriangleCount ; i++ )
        {
            FacetNormal( i , Vertex , Index , fout ) ;
        }

        fout<< "endsolid "<< name ;

        return true ;
    }
    else
    {
        std::ofstream fout( m_strFileName , std::ios::binary );

        const VFVECTOR3 *   Vertex = mesh.pVertices ;
        const VNVECTOR3UI * Index  = mesh.pTriangles;

        char title[80] = {0};
        int32_t triCount = mesh.nTriangleCount;

        fout.write( title , 80 );
        fout.write( (char*)&triCount , sizeof(int32_t) );

        for ( int i = 0 ; i < triCount ; i++ )
        {
            VFVECTOR3 pt1 = Vertex[ Index[i].x ];
            VFVECTOR3 pt2 = Vertex[ Index[i].y ];
            VFVECTOR3 pt3 = Vertex[ Index[i].z ];
            VFVECTOR3 normal = computeNormal( pt1 , pt2 , pt3 );

            fout.write( (char*)&normal.x , 4 );
            fout.write( (char*)&normal.y , 4 );
            fout.write( (char*)&normal.z , 4 );
            fout.write( (char*)&pt1.x , 4 );
            fout.write( (char*)&pt1.y , 4 );
            fout.write( (char*)&pt1.z , 4 );
            fout.write( (char*)&pt2.x , 4 );
            fout.write( (char*)&pt2.y , 4 );
            fout.write( (char*)&pt2.z , 4 );
            fout.write( (char*)&pt3.x , 4 );
            fout.write( (char*)&pt3.y , 4 );
            fout.write( (char*)&pt3.z , 4 );
            //fout << normal.x << normal.y << normal.z;
            //fout << pt1.x << pt1.y << pt1.z;
            //fout << pt2.x << pt2.y << pt2.z;
            //fout << pt3.x << pt3.y << pt3.z;

            char triAttr[2] = {0};
            fout.write( triAttr , 2 );
        }

        return true ;
    }
}

VFVECTOR3 VSTLFSaver::computeNormal( const VFVECTOR3& pt1 , const VFVECTOR3& pt2 , const VFVECTOR3& pt3 )
{
    VFVECTOR3 normal = ( pt2 - pt1 ).Cross( pt3 - pt1 );
    if ( normal.IsZero() )
        normal = VFVECTOR3::AXIS_Z;
    else
        normal.Normalize();

    return normal;
}


void VSTLFSaver::FacetNormal( unsigned idex , const VFVECTOR3 *  listVertex , const VNVECTOR3UI * listIndex , std::ofstream & ofs )
{
    const VNVECTOR3UI & tri   = listIndex[idex] ;

    const VFVECTOR3   & p0    = listVertex[ tri[0] ] ;
    const VFVECTOR3   & p1    = listVertex[ tri[1] ] ;
    const VFVECTOR3   & p2    = listVertex[ tri[2] ] ;

    VFVECTOR3 normal = ( p1 - p0 ).Cross( p2 - p0 ) ;

    if ( normal.IsZero( 0 ))
        normal = VFVECTOR3::AXIS_Z ;
    else
        normal.Normalize() ;

    m_listNormal.push_back( normal ) ;

    WriteFile( m_listNormal[idex] , p0 , p1 , p2 , ofs ) ;

}

void VSTLFSaver::WriteFile( const VFVECTOR3 & Normal , const VFVECTOR3 & Vertex1 , const VFVECTOR3 & Vertex2 
                         , const VFVECTOR3 & Vertex3 , std::ofstream & ofs )
{
    //std::stringstream ssn ;
    //ssn<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Normal.x <<" "<< Normal.y <<" "<<Normal.z ;
    //std::string normal = ssn.str();
    //ssn.str("") ;

    //ssn<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex1.x <<" "<< Vertex1.y <<" "<< Vertex1.z ;
    //std::string vertex1 = ssn.str();
    //ssn.str("") ;

    //ssn<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex2.x <<" "<< Vertex2.y <<" "<< Vertex2.z ;
    //std::string vertex2 = ssn.str();
    //ssn.str("") ;

    //ssn<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex3.x <<" "<< Vertex3.y <<" "<< Vertex3.z ;
    //std::string vertex3 = ssn.str();
    //ssn.str("") ;

    ofs<<"facet normal "<< std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Normal.x <<" "<< Normal.y <<" "<<Normal.z << "\r" ;
    ofs<<"outer loop"<<"\r" ;
    ofs<<"vertex "<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex1.x <<" "<< Vertex1.y <<" "<< Vertex1.z<< "\r" ;
    ofs<<"vertex "<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex2.x <<" "<< Vertex2.y <<" "<< Vertex2.z<< "\r" ;
    ofs<<"vertex "<<std::setprecision(10)<< std::setiosflags(std::ios::fixed)<< Vertex3.x <<" "<< Vertex3.y <<" "<< Vertex3.z<< "\r" ;
    ofs<<"endloop"<<"\r" ;
    ofs<<"endfacet"<<"\r";	
}