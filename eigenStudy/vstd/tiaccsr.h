 
#pragma once 

#include "tisvr.h"
#include "accsr.h"
#include <vector>
#include <memory>

class TVTrigger : public TVServiceProxy< IVTriggerable >
{ 
public :
    void Fire()
    {
        UseServer( []( IVTriggerable & trg ){ trg.Fire() ; } ) ;
    }
    template< class F >
    void FileWithCheck( F procOnFail )
    {
        UseServer( []( IVTriggerable & trg ){ trg.Fire() ; } , procOnFail ) ;
    }
} ; 

template< class DerivedT , unsigned ObserverCount = 1 >
class TVTriggerUser
{
public : 
    TVTriggerUser()
    {
    } ;

    ~TVTriggerUser()
    {
#if _ITERATOR_DEBUG_LEVEL == 2
        for( unsigned i = 0 ; i < ObserverCount ; i ++ )
        {
            VASSERT_MSG( m_triggers[i].IsClosed() , "请显示调用TVTriggerUser::Clear来析构一些Space对象，否则在某些特殊情况下可能会造成错误！\r\n" ) ;
        }
#endif /* _ITERATOR_DEBUG_LEVEL */  

        Clear() ;
    }

public : 
    template< unsigned indx >
    void AttachTrigger( TVTrigger & trg , void ( DerivedT::* ft )() ) 
    { 
        static_assert( indx < ObserverCount , "Error indx of trigger observer !" ) ;
        
        m_triggers[indx].Run( [ & trg ]( IVSpace< IVTriggerable > & spc ){
            trg.Create( spc ) ;
        } , *static_cast< DerivedT * >( this ) , ft ) ;
    }

    template< unsigned indx >
    void Dettach() 
    {
        static_assert( indx < ObserverCount , "Error indx of trigger observer !" ) ;
        m_triggers[indx].Close() ;
    }
    
    void Clear()
    {
        for( unsigned i = 0 ; i < ObserverCount ; i ++ )
        {
            m_triggers[i].Close() ;
        }
    }

private : 
    class TrgImp : public IVTriggerable 
    {
    public :
        TrgImp( DerivedT & pObj , void ( DerivedT::* ft )() )
            : m_function( ft )
            , m_pObject( &pObj )
        {}
        ~TrgImp(){}

    private :
        virtual void Fire() 
        {
            (m_pObject->*m_function)() ;
        }
    private :
        DerivedT   *  m_pObject  ;
        void ( DerivedT::* m_function )() ;
    } ;
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS2( TVTriggerUser , DerivedT , ObserverCount ) ;

private :
    TVService< IVTriggerable , TrgImp > m_triggers[ ObserverCount ] ;
} ;

template< class DerivedT >
class TVTriggerUser< DerivedT , 0 >
{
public : 
    TVTriggerUser()
    {
    } ;

    ~TVTriggerUser()
    {
#if _ITERATOR_DEBUG_LEVEL == 2
        for( unsigned i = 0 ; i < m_triggers.size() ; i ++ )
        {
            VASSERT_MSG( m_triggers[i]->IsClosed() , "请显示调用TVTriggerUser::Clear来析构一些Space对象，否则在某些特殊情况下可能会造成错误！\r\n" ) ;
        }
#endif /* _ITERATOR_DEBUG_LEVEL */  

        Clear() ;
    }

public :  
    void AttachTrigger( TVTrigger & trg , void ( DerivedT::* ft )() ) 
    { 
        for( unsigned i = 0 ; i < m_triggers.size() ; i ++ )
        {
            std::shared_ptr< TVService< IVTriggerable , TrgImp > > pItem( m_triggers[i] ) ;
            if( pItem->IsClosed() ) 
            {
                pItem->Run( [ & trg ]( IVSpace< IVTriggerable > & spc ){
                    trg.Create( spc ) ;
                } , *static_cast< DerivedT * >( this ) , ft ) ;

                return ;
            } 
        }

        std::shared_ptr< TVService< IVTriggerable , TrgImp > > pItemNew( new TVService< IVTriggerable , TrgImp >() ) ;
        m_triggers.push_back( pItemNew ) ;

        pItemNew->Run( [ & trg ]( IVSpace< IVTriggerable > & spc ){
            trg.Create( spc ) ;
        } , *static_cast< DerivedT * >( this ) , ft ) ; 
    }
     
    void Dettach( unsigned indx ) 
    {
        if( indx < m_triggers.size() )
        {
            m_triggers[indx]->Close() ;
        }
    }
    
    void Clear()
    {
        for( unsigned i = 0 ; i < m_triggers.size() ; i ++ )
        {
            m_triggers[i]->Close() ;
        }
    }

private : 
    class TrgImp : public IVTriggerable 
    {
    public :
        TrgImp( DerivedT & pObj , void ( DerivedT::* ft )() )
            : m_function( ft )
            , m_pObject( &pObj )
        {}
        ~TrgImp(){}

    private :
        virtual void Fire() 
        {
            (m_pObject->*m_function)() ;
        }
    private :
        DerivedT   *  m_pObject  ;
        void ( DerivedT::* m_function )() ;
    } ;

    VD_DECLARE_NO_COPY_TEMPLATE_CLASS2( TVTriggerUser , DerivedT , 0 ) ;

private :
    //TVService< IVTriggerable , TrgImp > m_triggers[ ObserverCount ] ;
    std::vector< std::shared_ptr< TVService< IVTriggerable , TrgImp > > > m_triggers ;
} ;

template< class T >
class TVPipe : public TVServiceProxy< IVPipe< T > >
{
public :
    TVPipe(){}
    ~TVPipe(){}

public :
    void Pump( T t )
    {
        UseServer( [ &t ]( IVPipe< T > & pipe ){ pipe.Flow( t ) ; } ) ;
    }

    template< class F >
    void PumpWithCheck( T t , F procOnFail )
    {
        UseServer( [ &t ]( IVPipe< T > & pipe ){ pipe.Flow( t ) ; } , procOnFail ) ;
    } 

    VD_DECLARE_NO_COPY_TEMPLATE_CLASS( TVPipe , T ) ;
} ; 

template< class T >
struct TVDataTrait< TVPipe<T> >
{  
    typedef TVPipe<T> & reference ; 
} ;
 
template<>
struct TVDataTrait< TVTrigger >
{  
    typedef TVTrigger & reference ; 
} ;

template< class TDerived , class TData , unsigned ObserverCount = 1 >
class TVPipeUser
{
public : 
    TVPipeUser(){ } ;

    ~TVPipeUser()
    {
#if _ITERATOR_DEBUG_LEVEL == 2
        for( unsigned i = 0 ; i < ObserverCount ; i ++ )
        {
            VASSERT_MSG( m_pipes[i].IsClosed() , "请显示调用TVTriggerUser::Clear来析构一些Space对象，否则在某些特殊情况下可能会造成错误！\r\n");
        }
#endif /* _DEBUG */  

        Clear() ;
    } 

public : 
    template< unsigned indx >
    void AttachPipe( TVPipe< TData > & trg , void ( TDerived::* ft )( TData ) ) 
    { 
        static_assert( indx < ObserverCount , "Error indx of trigger observer !" ) ; 

        m_pipes[indx].Run( [ & trg ]( IVSpace< IVPipe< TData > > & spc ){
            trg.Create( spc ) ;
        } , *static_cast< TDerived * >( this ) , ft ) ;
    }
    
    template< unsigned indx >
    void Dettach() 
    {
        static_assert( indx < ObserverCount , "Error indx of trigger observer !" ) ; 
        m_pipes[indx].Close() ;
    }

    void Clear()
    {
        for( unsigned i = 0 ; i < ObserverCount ; i ++ )
        {
            m_pipes[i].Close() ;
        }
    }

private : 
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS3( TVPipeUser , TDerived , TData , ObserverCount ) ;

    class PipeImp : public IVPipe< TData >
    {
    public :
        PipeImp( TDerived & pObj , void ( TDerived::* ft )( TData ) )
            : m_function( ft )
            , m_pObject( &pObj )
        {}
        ~PipeImp(){}

    private :
        virtual void Flow( TData v )  
        {
            (m_pObject->*m_function)( v ) ;
        }
    private :
        TDerived   *  m_pObject  ;
        void ( TDerived::* m_function )( TData ) ;
    } ;

private :
    TVService< IVPipe< TData > , PipeImp > m_pipes[ ObserverCount ] ;
} ;

template< class TDerived , class TData >
class TVPipeUser< TDerived , TData , 0 >
{
public : 
    TVPipeUser(){ } ;

    ~TVPipeUser()
    {
#if _ITERATOR_DEBUG_LEVEL == 2
        for( unsigned i = 0 ; i < m_vPipes.size() ; i ++ )
        {
            VASSERT_MSG( m_vPipes[i]->IsClosed() , "请显示调用TVTriggerUser::Clear来析构一些Space对象，否则在某些特殊情况下可能会造成错误！\r\n");
        }
#endif /* _DEBUG */  

        Clear() ;
    } 

public : 
    unsigned AttachPipe( TVPipe< TData > & trg , void ( TDerived::* ft )( TData ) ) 
    { 
        unsigned nID ;
        TVService< IVPipe< TData > , PipeImp > * pPipe( NULL ) ;

        for( unsigned i = 0 ; i < m_vPipes.size() ; i ++ )
        {
            if( m_vPipes[i]->IsClosed() )
            {
                pPipe = m_vPipes[i] ;
                nID = i ;
                break ;
            }
        }

        if( ! pPipe )
        {
            nID = m_vPipes.size() ;
            pPipe = new TVService< IVPipe< TData > , PipeImp > ;
            m_vPipes.push_back( pPipe ) ;
        } 

        pPipe->Run( [ & trg ]( IVSpace< IVPipe< TData > > & spc ){
            trg.Create( spc ) ;
        } , *static_cast< TDerived * >( this ) , ft ) ;

        return nID ;
    }
    
    void Dettach( unsigned id ) 
    { 
        assert( id < m_vPipes.size() ) ;
        m_vPipes[id]->Close() ;
    }

    void Clear()
    {
        for( unsigned i = 0 ; i < m_vPipes.size() ; i ++ )
        {
            m_vPipes[i]->Close() ;
            delete m_vPipes[i] ;
        }
        m_vPipes.clear() ;
    }

private : 
    class PipeImp : public IVPipe< TData >
    {
    public :
        PipeImp( TDerived & pObj , void ( TDerived::* ft )( TData ) )
            : m_function( ft )
            , m_pObject( &pObj )
        {}
        ~PipeImp(){}

    private :
        virtual void Flow( TData v )  
        {
            (m_pObject->*m_function)( v ) ;
        }
    private :
        TDerived   *  m_pObject  ;
        void ( TDerived::* m_function )( TData ) ;
    } ;
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS3( TVPipeUser , TDerived , TData , 0 ) ;

private :
    std::vector< TVService< IVPipe< TData > , PipeImp > * > m_vPipes ;
} ;