#pragma once

#include <vstd/tidata.h"
#include <vstd/ticmn.h"
#include "vmath.h"

class VMarkup_FVector3 : public VMarkupElementImp< VMarkup_FVector3 >
{
public :
    VMarkup_FVector3( const VFVECTOR3 & v )
        : m_value( v ){}

public :
    static bool Parse( VNMarkup::IElement & element , VFVECTOR3 & outV )
    {
        return element.GetAttibute( "X" , outV.x )
            && element.GetAttibute( "Y" , outV.y )
            && element.GetAttibute( "Z" , outV.z ) ;
    } 

public :
    void Imp_TravelAttributes( std::function< void ( const char * , VNMarkup::AttrType ) > clltr )
    {
        clltr( "X" , AT_FLOAT ) ;
        clltr( "Y" , AT_FLOAT ) ;
        clltr( "Z" , AT_FLOAT ) ;
    }

    virtual bool Imp_GetAttibuteF( const char * lpAttrName , float & outValue )
    {
        if( lpAttrName[0] >= 'X' && lpAttrName[0] <= 'Z' && lpAttrName[1] == '\0' )
        {
            unsigned at = lpAttrName[0] - 'X' ;

            outValue = m_value[at] ;
            return true ;
        }

        return false ;
    }
      

protected :
    VFVECTOR3 m_value ;
} ;

class VMarkup_NUIVector3 : public VMarkupElementImp< VMarkup_NUIVector3 >
{
public :
    VMarkup_NUIVector3( const VNVECTOR3UI & v )
        : m_value( v ){}

public :
    static bool Parse( VNMarkup::IElement & element , VNVECTOR3UI & outV )
    {
        return element.GetAttibute( "X" , outV.x )
            && element.GetAttibute( "Y" , outV.y )
            && element.GetAttibute( "Z" , outV.z ) ;
    } 

public :
    void Imp_TravelAttributes( std::function< void ( const char * , VNMarkup::AttrType ) > clltr )
    {
        clltr( "X" , AT_UNSIGNED ) ;
        clltr( "Y" , AT_UNSIGNED ) ;
        clltr( "Z" , AT_UNSIGNED ) ;
    }

    virtual bool Imp_GetAttibuteU( const char * lpAttrName , unsigned & outValue )
    {
        if( lpAttrName[0] >= 'X' && lpAttrName[0] <= 'Z' && lpAttrName[1] == '\0' )
        {
            unsigned at = lpAttrName[0] - 'X' ;

            outValue = m_value[at] ;
            return true ;
        }

        return false ;
    }


protected :
    VNVECTOR3UI m_value ;
};

class VMarkup_VFQuaternion : public VMarkupElementImp< VMarkup_VFQuaternion >
{
public :
    VMarkup_VFQuaternion( const VFQuaternion & v )
        : m_value( v ){}
    
public :
    static bool Parse( VNMarkup::IElement & element , VFQuaternion & outV )
    {
        return element.GetAttibute( "X" , outV.x )
            && element.GetAttibute( "Y" , outV.y )
            && element.GetAttibute( "Z" , outV.z )
            && element.GetAttibute( "W" , outV.w ) ;
    } 

protected :
    class VXEnumMarkAttributes : public IVXEnumerator< std::pair< const char * , VNMarkup::AttrType > & >
    {
    };

public :
    void Imp_TravelAttributes( std::function< void ( const char * , VNMarkup::AttrType ) > clltr )
    {
        clltr( "X" , AT_FLOAT ) ;
        clltr( "Y" , AT_FLOAT ) ;
        clltr( "Z" , AT_FLOAT ) ;
        clltr( "W" , AT_FLOAT ) ;
    }

    virtual bool Imp_GetAttibuteF( const char * lpAttrName , float & outValue )
    {
        if( lpAttrName[0] != '\0' && lpAttrName[1] == '\0' )
        {
            switch( lpAttrName[0] )
            {
            case 'X' : outValue = m_value.x ; break ;
            case 'Y' : outValue = m_value.y ; break ;
            case 'Z' : outValue = m_value.z ; break ;
            case 'W' : outValue = m_value.w ; break ;
            default  : return false ;
            }
            return true ;
        }
        return false ; 
    }
      

protected :
    VFQuaternion m_value ;
} ;

class VMarkup_FLocation : public VMarkupElementImp< VMarkup_FLocation > 
{
public :
    VMarkup_FLocation( const VFLocation & v ) 
        : mv( v.position ) , mq( v.orientation ) {}

private :
    static const char * GetPosLable()       { static const char * LP = "POS" ; return LP ; }
    static const char * GetOrientLable()    { static const char * LO = "ORI" ; return LO ; }

public :
    static bool Parse( VNMarkup::IElement & element , VFLocation & locOut )
    {
        unsigned posCount(0) , oriCount(0) ;
        auto AccPos = [ &locOut , & posCount ]( VNMarkup::IElement &child ){
            if( VMarkup_FVector3::Parse( child , locOut.position ) )
                posCount ++ ;
        } ;
        auto AccOri = [ &locOut , & oriCount ]( VNMarkup::IElement &child ){
            if( VMarkup_VFQuaternion::Parse( child , locOut.orientation ) )
                oriCount ++ ;
        } ;

        element.FindChild( GetPosLable()    , VCMN::TVUserAdaptor< VNMarkup::IElement & >( AccPos ) ) ;
        element.FindChild( GetOrientLable() , VCMN::TVUserAdaptor< VNMarkup::IElement & >( AccOri ) ) ;

        return ( posCount == 1 && oriCount == 1 ) ;
    }

public :
    virtual void Imp_FindChild      ( const char * lpChildName , IVUser< VNMarkup::IElement & > & usr )
    {
        if( strcmp( lpChildName , GetPosLable() ) == 0 )
            usr.Visit( mv ) ;
        else if( strcmp( lpChildName , GetOrientLable() ) == 0 )
            usr.Visit( mq ) ;
    }

    virtual void Imp_TravelChildren ( IVUser< const VNMarkup::ChildInfo & > & usr )
    {
        VNMarkup::ChildInfo ciPos , ciOri ;

        ciPos.name   = GetPosLable() ;
        ciPos.pChild = &mv ;

        ciOri.name   = GetOrientLable() ;
        ciOri.pChild = &mq ;

        usr.Visit( ciPos ) ;
        usr.Visit( ciOri ) ;
    }

protected :
    VMarkup_FVector3      mv ;  
    VMarkup_VFQuaternion  mq ;
} ;
 
class VMarkup_FAreaCoord : public VMarkupElementImp<VMarkup_FAreaCoord>
{
public:
    VMarkup_FAreaCoord( const VFArealCoord3 & v )
        : m_value(v){}
    ~VMarkup_FAreaCoord(){}

public :
    static bool Parse( VNMarkup::IElement & element , VFArealCoord3 & vOut )
    {
        return element.GetAttibute( "X" , vOut.v1 )
            && element.GetAttibute( "Y" , vOut.v2 )
            && element.GetAttibute( "Z" , vOut.v3 ) ;
    }

public :
    void Imp_TravelAttributes( std::function< void ( const char * , VNMarkup::AttrType ) > clltr )
    {
        clltr( "X" , AT_FLOAT ) ;
        clltr( "Y" , AT_FLOAT ) ;
        clltr( "Z" , AT_FLOAT ) ;
    }

    bool Imp_GetAttibuteF( const char * lpAttrName , float & outValue )
    {
        if( lpAttrName[0] >= 'X' && lpAttrName[0] <= 'Z' && lpAttrName[1] == '\0' )
        {
            unsigned at = lpAttrName[0] - 'X' ;

            outValue = m_value.v[at] ;
            return true ;
        }
        else
            return false ;
    }

private:
    VFArealCoord3 m_value ;
};
