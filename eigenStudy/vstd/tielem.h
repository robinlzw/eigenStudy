#pragma once

#include "tisys.h"

/*
template< typename TElem > class TVElement2 ; 

template< typename TIDID ,  typename TItem >
class TVElement2< IVElement2< IVClassifiable< TIDID > , IVSysArray< TItem > > > 
    : public IVProvider< IVSlot< TVRelation< TIDID > > >
{
public :
    typedef IVSlot< TVRelation< TIDID > >  DynElemType ;

public :
    template< class TFact >
    void Reset( TFact & f )
    {
    } ;

    void PushBack( VPDD< TItem > & item )
    {
    }

    void Apply( VO< DynElemType > & usr )
    {
    } 
} ;
 
template< typename TIDID ,   typename ... TInput >
class TVElement2< IVElement2< IVClassifiable< TIDID > , IVSysMerge< TInput ... > > >
    : public IVProvider< IVSlot< TVRelation< TIDID > > >
{
public :
    typedef IVSlot< TVRelation< TIDID > >  DynElemType ;

public :
    template< class TFact > 
    void Reset( TFact & f , VPDD< TInput > & ... provs )
    {
    } 

    void Apply( VO< DynElemType > & usr )
    {
    } 
} ; 

//
//template< typename TID , typename TItem  >
//class TVElemArray
//{
//} ;
//
//template< typename TID , typename TOutput, typename ...TInput  >
//class TVElemMerge 
//{
//} ;


template< typename TID , typename TE > class TVElement2 ; 

template< typename TID, typename TOutput, typename ...TInput  >
class TVElementMergeClnt : TVServer< VNSYS::MERGE::IPipe<TID , TOutput > >
                         , TVServer< IVTracer >
{
public :
    typedef IVElement2< TID , IVSysMerge< TInput ...  > > interf_type ; 
    typedef VNSYS::MERGE::IPipe<TID , TOutput > pipe_type ;

public :
    template< typename F >
    TVElementMergeClnt( interf_type & interf , VI< IVSlot< TInput > > & ... spcInputs , VI< IVTracer > & spcTracer , F f )
        : m_Elem( interf )
    {
        m_listener.Create( spcTracer ) ;
        m_Elem.Import( spcInputs ... ) ;  

        _reset_data() ;

        TVServer< VNSYS::MERGE::IPipe<TID , TOutput > >::Run( f ) ;
    }
    ~TVElementMergeClnt()
    {
        TVServer< IVTracer >::Close() ;
        TVServer< VNSYS::MERGE::IPipe<TID , TOutput > >::Close() ;
    }
    
private :
    virtual void OnChanged()
    {
        m_listener.UseServer( []( auto & t ){ t.OnChanged() ; } ) ;
    }

    virtual bool UpdateData()
    {
        return false ; // _check_data() ;
    }
    
    virtual void GetElement( VO<  TVConstRef< TID >  >& usr )
    {
        m_Elem.GetData( usr ) ; 
    }
    
    virtual void CreateData( VO<TVConstRef< TOutput >> & usr )
    {
        _check_data() ;
        m_proxyData.UseServer( [ & usr ]( auto & svr ){
            svr.GetData( usr ) ;
        } ) ;
    }
    
private :
    void _check_data() 
    {
        if( m_proxyData.IsClosed() )
        {
            _reset_data() ;
        }
    } 

    void _reset_data()
    {
        m_Elem.Export( VD_P2U( m_proxyData , [this]( IVSlot< TOutput > & dd ){
            TVServer< IVTracer >::Run( [ & dd ]( auto & spcTracer ){
                dd.Trace( spcTracer ) ;
            } ) ;
        } ) ) ;
    }

private :
    interf_type & m_Elem ;
    TVServiceProxy< IVTracer  > m_listener ;
    TVServiceProxy< IVSlot< TOutput > > m_proxyData ; 
} ;

template< typename TID, typename TOutput, typename ...TInput  >
class TVElement2< TID, IVSysMerge< TInput ... > > : public IVProvider< IVSlot< TID > >
                                                           , TVServer< IVTracer >
{
public :
    typedef VNSYS::MERGE::IPipe<TID , TOutput > pipe_type ;
    typedef IVElement2< TID , IVSysMerge< TInput ...  > > interf_type ;
    typedef TVElement2< TID , IVSysMerge< TInput ... > > my_type ;
    typedef TVElementMergeClnt< TID , TOutput , TInput ... >      clnt_type ;
    typedef VNSYS::MERGE::TBatchProvider< TInput ... > batch_prov ;

private :
    class TExp : public IVSlot< TOutput >
    {
    public:
        TExp(my_type & m) : me(m) {}
        ~TExp() {}
    private:
        virtual void Trace( VI< IVTracer > & spc) { me._trace(spc); }
        virtual void GetData( VO< TVConstRef< TOutput > > & usr ) { me._getData(usr); }

    private:
        my_type & me;
    };

public :
    template< class TProv >
    void Reset( TProv & prov, IVProvider< IVSlot< TInput > > & ... provs )
    {
        auto F = [ & prov , this ]( VI< IVSlot< TInput > > & ... spcs ){
            prov.Create( VD_L2U( [this , &spcs ... ]( VI< interf_type > & spcElem ) {
                _reset( spcElem , spcs ... ) ;
            } ) ) ;
        }  ;
        
        batch_prov::Execute( F , provs ... ) ;
    } 

    void Export( VODD< TOutput > & usr )
    {
        m_svcDynData.RunNew( usr , *this ) ;
    }
 
    void Apply( VO< IVSlot< TID > > & usr )
    {
        assert(0);
    }

private :
    void OnChanged()
    {
        m_listener.TidyTravel([](auto & t) { t.OnChanged(); });
    }
    
private :
    void _trace( VI< IVTracer > & spc ) { m_listener.Add( spc ) ; }
    void _getData( VO< TVConstRef< TOutput > > & usr )
    {
        m_proxyPipe.UseServer( [ &usr ]( pipe_type & pipe ){
            pipe.CreateData( usr ) ;
        } ) ;
    }
    void _reset( VI< interf_type > & spcElem , VI< IVSlot< TInput > > & ... spcInputs)
    {
        TVServer< IVTracer >::Run([this, & spcElem , &spcInputs ...](VI< IVTracer > & spcTracer) {
            m_clnt.Create( spcElem , spcInputs ... , spcTracer, [ this ](VI< pipe_type > & spc) {
                m_proxyPipe.Create(spc);
            }) ;
        });

        OnChanged();
    }

private :
    TVServiceProxyArray< IVTracer >                   m_listener   ;
    TVClient< clnt_type , interf_type >              m_clnt       ;
    TVServiceArray< IVSlot< TOutput > , TExp > m_svcDynData ;
    TVServiceProxy< pipe_type >                       m_proxyPipe  ;  
} ;

template< typename TID , typename TItem >
class TVElement2< TID , IVSysArray< TItem > > : public IVProvider< IVSlot< TID > >
{
public :
    template< class TProv >
    void Reset( TProv & prov )
    { 
        assert(0);
    } 
     
    void PushBack( VPDD< TItem > & dd )
    { 
        //virtual void Create( VO< IVSysMerge< unsigned , T > > & usr ) = 0 ;
    } 

    void Apply( VO< IVSlot< TID > > & usr )
    {
    }
} ;

template< typename TID , typename TOutput, typename ...TInput  >
using TVElemMerge = TVElement2< TID , IVSysMerge< TInput ... > >  ;

 template< typename TID , typename TItem  >
using TVElemArray = TVElement2< TID , IVSysArray< TItem > >  ;

namespace VNDATA
{

    template< class T, class TC >
    class TVClone : public IVProvider< IVSlot< T > >
        , TVServer< IVTracer >
    {
    public:
        typedef TVClone< T , TC > my_type;
    public:
        TVClone()
        {
        }
        ~TVClone()
        {
            m_svc.Close();
            TVServer< IVTracer >::Close();
        }

    private:
        struct Dyn : IVSlot< T >
        {
            my_type & me;
            Dyn(my_type & m) : me(m) {}

            virtual void Trace(VI< IVTracer        > & spc) { me._trace(spc); }
            void GetData(VO< TVConstRef< T > > & usr) { me._getData(usr); }
        };

    public:
        template< typename TFact >
        void Reset(TFact & f)
        {
            f.Export(VD_P2U(m_proxy, [this](IVSlot< T > & dd) {
                TVServer< IVTracer >::Run([&dd](auto & t) {
                    dd.Trace(t);
                });
            }));
        }

        void Apply(VO< IVSlot< T > > & spc)
        {
            m_svc.RunNew(spc, *this);
        }

    private:
        virtual void _trace(VI< IVTracer > & spc)
        {
            m_listener.Add(spc);
        }

        void _getData(VO< TVConstRef< T > > & usr)
        {
            m_proxy.UseServer([&usr](auto & svr) {
                svr.GetData(usr);
            });
        }

        void OnChanged()
        {
            m_listener.TidyTravel([](auto & t) { t.OnChanged(); });
        }

    private:
        TVServiceProxy< IVSlot< T > >        m_proxy;
        TVServiceProxyArray< IVTracer >             m_listener;
        TVServiceArray< IVSlot< T >, Dyn >  m_svc;
    };

    template< class T, class TImp = T >
    class TVSource : public IVProvider< IVSlot< T > >
    {
    public :
        TVSource( const T & src )
            : m_imp( src ) 
        {
        }
        ~TVSource()
        {
            m_svcDyn.Close() ;
            m_svcData.Close() ;
        }
    private :
        typedef TVSource< T , TImp > my_type ;
        struct Dyn : IVSlot< T >
        {
            my_type & me;
            Dyn(my_type & m) : me(m) {}

            virtual void Trace(VI< IVTracer        > & spc) {}
            void GetData(VO< TVConstRef< T > > & usr) { me._getData(usr); }
        };

    public :
        void Apply( VO< IVSlot< T > > & usr )
        {
            m_svcDyn.RunNew( usr , *this ) ;
        }

    private :
        void _getData(VO< TVConstRef< T > > & usr)
        {
            m_svcData.RunNew( usr , m_imp ) ;
        }

    private :
        TImp m_imp ;
        TVServiceArray< IVSlot< T > , Dyn > m_svcDyn ;
        TVServiceArray< TVConstRef< T > > m_svcData ;
    };
};

template< class T >
using TVData   = VNDATA::TVData< T > ;
*/