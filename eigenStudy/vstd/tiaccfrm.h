
#pragma once

#include "tiaccsr.h"
#include "ticmn.h"
#include <map>
#include <algorithm>
#include "tilnkr.h"

template< class BaseType , class DerivedType >
class VTThrownObject : public BaseType
{
public :
    VTThrownObject() 
    {
    } ;

    ~VTThrownObject()
    {
        m_flwRelease.PutValue( this ) ;
    }

public :
    typedef TVAccCnstPtrTrait< BaseType > ACCTRAIT ;

public :
    IVFlowAccessor< BaseType * , ACCTRAIT > & GetFlwRelease()
    {
        return m_flwRelease ;
    }

private :
    TVFlowSnglObsvrAcc< BaseType * , ACCTRAIT > m_flwRelease ;
} ;


template< class Derived , class TIntf , class TAdaptor , class TImp >
class VTThrownFactory : public IVFlowAccessor< TIntf * , TVAccCnstPtrTrait< TIntf > >::Observer
{
protected :
    typedef TVAccCnstPtrTrait< TIntf > ACCTRAIT ;

    struct INFO
    {
        TImp                                 * pImp  ;
        IVFlowAccessor< TIntf * , ACCTRAIT > * pFlow ;
    } ;

public :
    VTThrownFactory(){}
    ~VTThrownFactory()
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() 
                     , [ this ]( std::map< const TIntf * , INFO >::reference ref )
        {
            INFO & info = ref.second ;
            info.pFlow->DetachChecker( this ) ;
            delete info.pFlow ;
            delete info.pImp  ;
        }
        ) ;
    }

protected :
    class DefaultPairFactory
    {
    public :
        DefaultPairFactory(){}
        ~DefaultPairFactory(){}
        
    public :
        TAdaptor * CreateAdaptor( )
        {
            return new TAdaptor( ) ;
        }

        TImp     * CreateImplement( )
        {
            return new TImp() ;
        }
    } ;

    template< class TLINKER >
    class LinkerablePairFactory 
    {
    public :
        LinkerablePairFactory( const std::shared_ptr< TLINKER > & linker )
            : m_linker( linker ) {}
        ~LinkerablePairFactory(){}

    public :
        TAdaptor * CreateAdaptor( )
        {
            return new TAdaptor( m_linker ) ;
        }

        TImp     * CreateImplement( )
        {
            return new TImp( m_linker ) ;
        }

    protected :
        std::shared_ptr< TLINKER > m_linker ;
    } ; 

public :
    template< class op >
    void TravelImplements( op & func )
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() , [ &func ]( std::map< const TIntf * , INFO >::reference ref ){
            func( * ref.second.pImp ) ;
        }) ;
    } ;

    template< class PairFactory >
    TAdaptor * CreateThrownObject( PairFactory & pf )
    {
        std::auto_ptr< TAdaptor > pObj( pf.CreateAdaptor() ) ;
        std::auto_ptr< TImp     > pImp( pf.CreateImplement() ) ;
        Derived  * pd   = static_cast< Derived * >( this ) ;

        pd->OnCreateThrown( * pObj , *pImp ) ;

        INFO info ;
        info.pImp  = pImp.release() ;
        info.pFlow = pObj->GetFlwRelease().Clone() ;
        info.pFlow->AttachChecker( this ) ;
        
        m_mapComponents[ pObj.get() ] = info ;
        
        return pObj.release() ;
    }

    template< class TLINKER >
    TAdaptor * CreateThrownObject( IVClassFactory< TLINKER > & linkerFactory = TVClssFactoryImp< TLINKER >() )
    {
        std::shared_ptr< TLINKER > pLinker( linkerFactory.CreateClass() ) ;
        LinkerablePairFactory< TLINKER > pf( pLinker ) ;
        return CreateThrownObject( pf ) ;
    }

    TAdaptor * CreateThrownObject()
    {
        DefaultPairFactory dpf ;

        return CreateThrownObject( dpf ) ;
    }
public :
    TImp * FindMate( const TIntf & p )
    { 
        auto it = m_mapComponents.find( &p ) ;

        if( it == m_mapComponents.end() )
            return NULL ;

        return it->second.pImp ;
    } ;

public :
    void OnCreateThrown( TAdaptor & a , TImp & b ) 
    { 
    }
    void OnDestoryThrown( TImp & b ) 
    { 
    }

private :
    virtual void OnFlowData( const TIntf * pObj ) 
    {
        auto it = m_mapComponents.find( pObj ) ;
        assert( it != m_mapComponents.end() ) ;
        it->second.pFlow->DetachChecker( this ) ;
              
        Derived  * pd   = static_cast< Derived * >( this ) ;
        pd->OnDestoryThrown( * it->second.pImp ) ;

        delete it->second.pFlow ;
        delete it->second.pImp  ;
        m_mapComponents.erase( it ) ;
    }

protected :
    std::map< const TIntf * , INFO >  m_mapComponents ;
} ;


template< class Derived , class TIntf , class TAdaptor , class TImp , class TLinker >
class VTThrownFactoryL : public IVFlowAccessor< TIntf * , TVAccCnstPtrTrait< TIntf > >::Observer
{
protected :
    typedef TVAccCnstPtrTrait< TIntf > ACCTRAIT ;

    struct INFO
    {
        TImp                                 * pImp  ;
        IVFlowAccessor< TIntf * , ACCTRAIT > * pFlow ;
    } ;

public :
    VTThrownFactoryL(){}
    ~VTThrownFactoryL()
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() 
                     , [ this ]( std::map< const TIntf * , INFO >::reference ref )
        {
            INFO & info = ref.second ;
            info.pFlow->DetachChecker( this ) ;
            delete info.pFlow ;
            delete info.pImp  ;
        }
        ) ;
    }

protected :  
    class LinkerablePairFactory 
    {
    public :
        LinkerablePairFactory( IVClassFactory< TLinker >  & lf )
            : m_linker( lf.CreateClass() ) {}
        ~LinkerablePairFactory(){}

    public :
        TAdaptor * CreateAdaptor( )
        {
            return new TAdaptor( m_linker ) ;
        }

        TImp     * CreateImplement( )
        {
            return new TImp( m_linker ) ;
        }

    protected :
        std::shared_ptr< TLinker > m_linker ;
    } ; 

public :
    template< class op >
    void TravelImplements( op & func )
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() , [ &func ]( std::map< const IVObject * , INFO >::reference ref ){
            func( * ref.second.pImp ) ;
        }) ;
    } ;

    template< class PairFactory >
    TAdaptor * CreateThrownObject( PairFactory & pf )
    {
        std::auto_ptr< TAdaptor > pObj( pf.CreateAdaptor() ) ;
        std::auto_ptr< TImp     > pImp( pf.CreateImplement() ) ;
        Derived  * pd   = static_cast< Derived * >( this ) ;

        pd->OnCreateThrown( * pObj , *pImp ) ;

        INFO info ;
        info.pImp  = pImp.release() ;
        info.pFlow = pObj->GetFlwRelease().Clone() ;
        info.pFlow->AttachChecker( this ) ;
        
        m_mapComponents[ pObj.get() ] = info ;
        
        return pObj.release() ;
    } 

    TAdaptor * CreateThrownObject( IVClassFactory< TLinker > & linkerFactory = TVClssFactoryImp< TLinker >() )
    { 
        LinkerablePairFactory pf( linkerFactory ) ;
        return CreateThrownObject( pf ) ;
    } 

public :
    TImp * FindMate( const TIntf & p )
    { 
        auto it = m_mapComponents.find( &p ) ;

        if( it == m_mapComponents.end() )
            return NULL ;

        return it->second.pImp ;
    } ;

public :
    void OnCreateThrown( TAdaptor & a , TImp & b ) 
    { 
    }
    void OnDestoryThrown( TImp & b ) 
    { 
    }

private :
    virtual void OnFlowData( const TIntf * pObj ) 
    {
        auto it = m_mapComponents.find( pObj ) ;
        assert( it != m_mapComponents.end() ) ;
        it->second.pFlow->DetachChecker( this ) ;
              
        Derived  * pd   = static_cast< Derived * >( this ) ;
        pd->OnDestoryThrown( * it->second.pImp ) ;

        delete it->second.pFlow ;
        delete it->second.pImp  ;
        m_mapComponents.erase( it ) ;
    }

protected :
    std::map< const TIntf * , INFO >  m_mapComponents ;
} ;
template< class Derived , class TIntf , class TAdaptor , class TImp , class TLinker >
class VTThrownFactoryLS : public VTThrownFactoryL< Derived , TIntf , TAdaptor , TImp , TLinker >
{ 
protected :
    typedef TVAccCnstPtrTrait< TIntf > ACCTRAIT ;
    typedef typename TImp::ISupport    ISUPPORT ;

    struct INFO
    {
        TImp                                 * pImp  ;
        IVFlowAccessor< TIntf * , ACCTRAIT > * pFlow ;
    } ;

public :
    VTThrownFactoryLS(){}
    ~VTThrownFactoryLS()
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() 
                     , [ this ]( std::map< const TIntf * , INFO >::reference ref )
        {
            INFO & info = ref.second ;
            info.pFlow->DetachChecker( this ) ;
            delete info.pFlow ;
            delete info.pImp  ;
        }
        ) ;
    }

protected :  
    class SprtFactory 
    {
    public :
        SprtFactory( IVClassFactory< TLinker >  & lf , typename ISUPPORT & sprt )
            : m_linker( lf.CreateClass() )
            , m_sprt( sprt ){}
        ~SprtFactory(){}

    public :
        TAdaptor * CreateAdaptor( )
        {
            return new TAdaptor( m_linker ) ;
        }

        TImp * CreateImplement( )
        {
            return new TImp( m_linker , m_sprt ) ;
        } 

    protected :
        std::shared_ptr< TLinker > m_linker ;
        ISUPPORT                 & m_sprt   ;
    } ;

public :
    template< class op >
    void TravelImplements( op & func )
    {
        std::for_each( m_mapComponents.begin() , m_mapComponents.end() , [ &func ]( std::map< const TIntf * , INFO >::reference ref ){
            func( * ref.second.pImp ) ;
        }) ;
    } ;

    template< class PairFactory >
    TAdaptor * CreateThrownObject( PairFactory & pf )
    {
        std::auto_ptr< TAdaptor > pObj( pf.CreateAdaptor() ) ;
        std::auto_ptr< TImp     > pImp( pf.CreateImplement() ) ;
        Derived  * pd   = static_cast< Derived * >( this ) ;

        pd->OnCreateThrown( * pObj , *pImp ) ;

        INFO info ;
        info.pImp  = pImp.release() ;
        info.pFlow = pObj->GetFlwRelease().Clone() ;
        info.pFlow->AttachChecker( this ) ;
        
        m_mapComponents[ pObj.get() ] = info ;
        
        return pObj.release() ;
    } 

    TAdaptor * CreateThrownObject( IVClassFactory< TLinker > & linkerFactory = TVClssFactoryImp< TLinker >() )
    { 
        SprtFactory pf( linkerFactory , * static_cast< Derived * >( this ) ) ;
        return CreateThrownObject( pf ) ;
    } 

    TAdaptor * CreateThrownObject( ISUPPORT & sp , IVClassFactory< TLinker > & linkerFactory = TVClssFactoryImp< TLinker >() )
    { 
        SprtFactory pf( linkerFactory , sp ) ;
        return CreateThrownObject( pf ) ;
    } 

public :
    TImp * FindMate( const TIntf & p )
    { 
        auto it = m_mapComponents.find( &p ) ;

        if( it == m_mapComponents.end() )
            return NULL ;

        return it->second.pImp ;
    } ;

public :
    void OnCreateThrown( TAdaptor & a , TImp & b ) 
    { 
    }
    void OnDestoryThrown( TImp & b ) 
    { 
    }

private :
    virtual void OnFlowData( const TIntf * pObj ) 
    {
        auto it = m_mapComponents.find( pObj ) ;
        assert( it != m_mapComponents.end() ) ;
        it->second.pFlow->DetachChecker( this ) ;
              
        Derived  * pd   = static_cast< Derived * >( this ) ;
        pd->OnDestoryThrown( * it->second.pImp ) ;

        delete it->second.pFlow ;
        delete it->second.pImp  ;
        m_mapComponents.erase( it ) ;
    }

protected :
    std::map< const TIntf * , INFO >  m_mapComponents ;
} ;

