#pragma once

#include "tisvr.h"
#include "tidyn.h" 

#include <thread>
#include <mutex>
//#include <chrono>
//#include <functional>
//#include <atomic>

#ifdef _DEBUG
#define VD_NOT_CARE_PERFORMANCE
#endif

/////////////////////////////////////////////////////////////////////////////////

// Usage  of hub

/////////////////////////////////////////////////////////////////////////////////
// Standard Pipeline Arguments 

template< typename ... TParams > class TVPipelineArgBase ;

template<> 
class TVPipelineArgBase<>
{
public :
    typedef void FstType;

    TVPipelineArgBase(){}
    ~TVPipelineArgBase(){}
} ;

template< typename TFST , typename ... TOTHERs >
class TVPipelineArgBase< TFST , TOTHERs ... > : public TVPipelineArgBase< TOTHERs ... >
{
public :
    typedef TVPipelineArgBase< TFST , TOTHERs ... > SelfType;
    typedef TFST FstType;

    TVPipelineArgBase( const TFST & v , const TOTHERs & ... vothers ) 
        : m_ref(v) , TVPipelineArgBase< TOTHERs ... >( vothers... ) 
    {
    } 
    ~TVPipelineArgBase(){}

public :
    const TFST & m_ref ;
} ;  

template< typename ... Ts > class TVPipelineArgBaseMatcher;

template< typename TMatch , typename TFstArg , typename ... TArgs >
class TVPipelineArgBaseMatcher< TMatch , TVPipelineArgBase< TFstArg , TArgs ... > >
    : public TVPipelineArgBaseMatcher< TMatch , TVPipelineArgBase< TArgs ... > >
{};

template< typename TMatch , typename ... TArgs >
class TVPipelineArgBaseMatcher< TMatch , TVPipelineArgBase< TMatch , TArgs ... > >
{
public:
    typedef TVPipelineArgBase< TMatch , TArgs ... > CvtType;
};

template< typename TMatch >
class TVPipelineArgBaseMatcher< TMatch , TVPipelineArgBase<> > {};

template< typename ... TParams >
class TVPipelineArg 
{
public :
    TVPipelineArg( const TParams & ... vothers ) 
        : m_imp( vothers... ) 
    {
    } 
    ~TVPipelineArg()
    {
    }
public :
    template< typename TGet > 
    const TGet & Get() const 
    {  
        return _getFrom( m_imp , static_cast< const TGet * >( 0 ) );
        //return (( TVPipelineArgBaseMatcher< TGet , TVPipelineArgBase< TParams ... > >::CvtType* )( &m_imp ))->m_ref;
    }

    template< typename FUNC >
    void GetSome( FUNC f ) const
    {
        VGetSomeHelper< FUNC > helper( *this );
        helper.Use( f );
    }

private : 
    template< typename TGet , typename ... TArg >
    const TGet & _getFrom( const TVPipelineArgBase< TGet , TArg ... > & arg , const TGet * ) const
    {
        return arg.m_ref;
    }

    template< typename TGet , typename TFST , typename ... TArg >
    const TGet & _getFrom( const TVPipelineArgBase< TFST , TArg ... > & arg , const TGet * p ) const
    {
        return _getFrom( static_cast< const TVPipelineArgBase< TArg ... > >( arg ) , p );
    }

    template< typename T , T > 
    struct TLambdaAnalyzer : public std::false_type
    {
        typedef std::tuple<> TTypesTuple;
    };

    template< typename TLambda , typename ... TRs , void( TLambda::*op )( const TRs & ... ) const >
    struct TLambdaAnalyzer< void( TLambda::* )( const TRs & ... ) const , op > : public std::true_type
    {
        typedef std::tuple< TRs ... > TTypesTuple;
    };

    template< typename TLambda >
    using TLambdaAnalyzerBuilder = TLambdaAnalyzer< decltype( &TLambda::operator() ) , &TLambda::operator() >;

    template< size_t I , typename ... Ts > class VGetSomeHelperHelper;

    template< size_t I , typename Tuple >
    class VGetSomeHelperHelper< I , Tuple > : public VGetSomeHelperHelper< I - 1 , Tuple >
    {
        typedef VGetSomeHelperHelper< I - 1 , Tuple > PARENT;
        typedef typename std::tuple_element< I - 1 , Tuple >::type ELETYPE;

    public:
        VGetSomeHelperHelper( const TVPipelineArg< TParams ... > & src ):m_Src(src),PARENT(src){}
        ~VGetSomeHelperHelper(){}

        template< typename FUNC , typename ... TRes >
        void Use( FUNC f , const TRes & ... ress )
        {
            PARENT::Use< FUNC , ELETYPE , TRes ... >( f , m_Src.Get< ELETYPE >() , ress ... );
        }

    private:
        const TVPipelineArg< TParams ... > & m_Src;
    };

    template< typename Tuple >
    class VGetSomeHelperHelper< 0 , Tuple >
    {
    public:
        VGetSomeHelperHelper( const TVPipelineArg< TParams ... > & src ){}
        ~VGetSomeHelperHelper(){}

        template< typename FUNC , typename ... TRes >
        void Use( FUNC f , const TRes & ... ress )
        {
            f( ress ... );
        }
    };

    template< typename FUNC >
    class VGetSomeHelper : public VGetSomeHelperHelper< std::tuple_size< typename TLambdaAnalyzerBuilder< FUNC >::TTypesTuple >::value , typename TLambdaAnalyzerBuilder< FUNC >::TTypesTuple >
    {
        typedef TLambdaAnalyzerBuilder< FUNC > TFuncLambdaAnalyzer;
        typedef VGetSomeHelperHelper< std::tuple_size< typename TLambdaAnalyzerBuilder< FUNC >::TTypesTuple >::value , typename TLambdaAnalyzerBuilder< FUNC >::TTypesTuple > PARENT;

    public:
        VGetSomeHelper( const TVPipelineArg< TParams ... > & src ):PARENT(src){}
        ~VGetSomeHelper(){}

        void Use( FUNC f )
        {
            PARENT::Use< FUNC >( f );
        }
    };

    //template< typename TGet > 
    //const TGet & _getFrom( const TVPipelineArgBase<> & arg  , const TGet * ) const 
    //{
    //    static_assert(0,"") ;
    //}

    //template< typename TGet > 
    //void _getFrom( const TGet ** pRtn , const TVPipelineArgBase<> & arg ) const 
    //{
    //    static_assert( 0 , "" ) ;
    //}

private :
    TVPipelineArgBase< TParams ... > m_imp ;
};

template < typename F , typename FE = F >
class TVLambdaInputPort : public TVLambdaInputPort< F , decltype(&FE::operator()) > 
{
public :
    TVLambdaInputPort( F f )
        : TVLambdaInputPort< F , decltype(&FE::operator()) >( f ){} 
} ;

template < typename F , typename ClassType, typename... Args >
class TVLambdaInputPort< F , void (ClassType::*)( IVSlot< Args > & ...) const > : public IVInputPort< Args ... >
{ 
public :
    TVLambdaInputPort( F f )
        : m_func( f ){}
public :
    virtual void Input ( IVSlot< Args > & ... args ) 
    {
        m_func( args ... ) ;
    }

private :
    F m_func ;
} ;

template< class F >
TVLambdaInputPort< F > VD_L2IP( F func )
{
    return TVLambdaInputPort< F >( func ) ;
}  


template< class T >
struct TVOutputPortParam
{
    typedef IVInputPort< T > type ;
} ;

template< class ... TINPUTs >
struct TVOutputPortParam< VI_AND< TINPUTs ... > >
{
    typedef IVInputPort< TINPUTs ... > type ;
} ;

template< unsigned COUNT >
class TVInputChngFlag
{
public :
    static_assert( COUNT < ( sizeof( unsigned ) * 8 )  , "Too big count !" ) ;

    TVInputChngFlag()
        : m_flag( 0 )
    {
    }

public :
    template< unsigned N >
    void Signal()
    {
        static_assert( N < COUNT , "Too big indx ! " ) ;
        m_flag |= ( unsigned(1) << N ) ;
    }

    void Reset()
    {
        m_flag = 0 ;
    }

    template< unsigned N >
    bool Check() const
    {
        return 0 != ( m_flag & ( unsigned(1) << N ) ) ;
    }

private :
    unsigned m_flag ; 
} ;

template< class T >
class TVDynVar
{
private :
    const T & m_src ;
    bool      m_chng ;

public :
    TVDynVar( const T & src , bool bChnged )
        : m_src( src ) , m_chng( bChnged )
    {}

    bool IsDirty() const 
    {
        return m_chng ;
    }

    operator const T & () const
    { 
        return m_src ; 
    }
} ;

template< typename TOUTPUT , typename ... TINPUTs >
VPUREINTERFACE IVConverter : IVInputPort< TINPUTs ... > , IVOutputPort< TOUTPUT >
{ 
} ; 

template< typename TINPUT , typename TOUTPUT = TINPUT > class TVData ;

//class TVData  : public IVSlot< TOUTPUT >
//              , public IVConverter< TOUTPUT , TINPUT >
//              , TVServer< IVTracer >
//{
//public:
//    TVData()
//    {
//    }
//    ~TVData()
//    { 
//        Close() ;
//    }
//
//private:
//    typedef TVData< TINPUT , TOUTPUT > my_type;
//    struct Prov : IVDataProv< TOUTPUT >
//    {
//        my_type & me;
//        Prov( my_type & m ) : me( m ) {}
//        virtual void Apply( VO< TVConstRef< TOUTPUT > > & usr ) { me._apply( usr ); }
//    };
//
//private:
//    void _apply( VO< TVConstRef< TOUTPUT > > & usr )
//    {  
//        m_proxySrc.UseServer( [ &usr ]( IVDataProv< TINPUT > & prov ){
//            prov.Apply( VD_L2U( [ &usr ]( VI< TVConstRef< TINPUT > > & spc ){
//                VI< TVConstRef< TOUTPUT > > & spcOut = static_cast< VI< TVConstRef< TOUTPUT > > & >( spc ) ;
//                usr.Visit( spcOut ) ;
//            } ) ) ;
//        } ) ;
//    }
//
//    void _notify()
//    {
//        m_listeners.TidyTravel( []( auto & t ){
//            t.OnChanged() ;
//        } ) ;
//    }
//
//public: 
//    void Close()
//    {
//        TVServer< IVTracer >::Close() ;
//        m_svcProv.Close() ;
//    }
//
//    void Output( IVInputPort< TOUTPUT > &  inp )
//    {
//        inp.Input( *this ) ;
//    }
//
//    void Input( IVSlot< TINPUT > & slot )
//    {
//        TVServer< IVTracer >::Run( [ this , & slot ]( VI< IVTracer > & spcTracer ){
//            slot.Trace( spcTracer ) ; 
//        } );
//
//        slot.GetData( VD_P2U( m_proxySrc ) ) ;
//
//        _notify() ;
//    };
//
//    // IVSlot< T >
//    void Trace( VI< IVTracer > & spc )
//    {
//        m_listeners.Add( spc ) ;
//    }
//
//    void GetData( VO< IVDataProv< TOUTPUT > > & usr )
//    {  
//        m_svcProv.RunNew( usr , *this ) ;
//    }
//
//    // IVTracer
//    void OnChanged()
//    {
//        _notify() ;
//    }
//
//    VD_DECLARE_NO_COPY_TEMPLATE_CLASS2( TVData , TINPUT , TOUTPUT );
//
//private:
//    TVServiceProxyArray< IVTracer        > m_listeners ; 
//    TVServiceProxy< IVDataProv< TINPUT > > m_proxySrc  ; 
//    TVServiceArray< IVDataProv< TOUTPUT > , Prov > m_svcProv;
//};

template< typename TOUTPUT >
class TVData< TOUTPUT , TOUTPUT >  : public IVSlot< TOUTPUT >
                                   , public IVInputPort< TOUTPUT >
                                   , TVServer< IVTracer >
{
public:
    TVData()
    {
    }
    ~TVData()
    { 
        Close() ;
    }

private:
    typedef TVData< TOUTPUT , TOUTPUT > my_type;
    struct Prov : IVDataProv< TOUTPUT >
    {
        my_type & me;
        Prov( my_type & m ) : me( m ) {}
        virtual void Apply( VO< TVConstRef< TOUTPUT > > & usr ) { me._apply( usr ); }
    };

private:
    void _apply( VO< TVConstRef< TOUTPUT > > & usr )
    {  
        m_proxySrc.UseServer( [ &usr ]( IVDataProv< TOUTPUT > & prov ){
            prov.Apply( usr ) ;
        } ) ;
    }

    void _notify()
    {
        m_listeners.TidyTravel( []( auto & t ){
            t.OnChanged() ;
        } ) ;
    }

public: 
    void Close()
    {
        TVServer< IVTracer >::Close() ;
        m_svcProv.Close() ;
    }

    void Output( IVInputPort< TOUTPUT > &  inp )
    {
        inp.Input( *this ) ;
    }

    void Input( IVSlot< TOUTPUT > & slot )
    {
        TVServer< IVTracer >::Run( [ this , & slot ]( VI< IVTracer > & spcTracer ){
            slot.Trace( spcTracer ) ; 
        } );

        auto VODate =VD_P2U( m_proxySrc );
        slot.GetData( VODate ) ;

        _notify() ;
    };

    // IVSlot< T >
    void Trace( VI< IVTracer > & spc )
    {
        m_listeners.Add( spc ) ;
    }

    void GetData( VO< IVDataProv< TOUTPUT > > & usr )
    {  
        m_svcProv.RunNew( usr , *this ) ;
    }

    // IVTracer
    void OnChanged()
    {
        _notify() ;
    }
    
    template< typename F >
    void Peer( F f )
    {
        TVServiceProxy< TVConstRef< TOUTPUT > > dtProxy ;

        m_proxySrc.UseServer( [ &dtProxy ]( IVDataProv< TOUTPUT > & prov ){
            prov.Apply( VD_P2U( dtProxy ) ) ;
        } ); 

        dtProxy.UseServer( [ f ]( const TVConstRef< TOUTPUT > & cb ){
            f( cb.m_ref );
        } ) ;
    }

private :
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS2( TVData , TOUTPUT , TOUTPUT ) ; 

private:
    TVServiceProxyArray< IVTracer         > m_listeners ; 
    TVServiceProxy< IVDataProv< TOUTPUT > > m_proxySrc  ; 
    TVServiceArray< IVDataProv< TOUTPUT > , Prov > m_svcProv;
};

template< typename TOU , typename TIN >
struct TVRTransDerivedTrait
{
    static const TOU & Translate( const TIN & v ) 
    {
        return static_cast< const TOU & >( v ) ; 
    }
} ;  

template< typename TOU , typename TIN , unsigned offset >
struct TVRTransMemberTrait
{
    static const TOU & Translate( const TIN & v ) 
    {
        return * reinterpret_cast< const TOU * >( reinterpret_cast< const char * >( & v ) + offset ) ; 
    }
} ;
 
template< typename TINPUT , typename TOUTPUT , typename TTRANS = TVRTransDerivedTrait< TOUTPUT , TINPUT > >
class TVAdaptor : public IVSlot< TOUTPUT >
                , public IVConverter< TOUTPUT , TINPUT >
                , TVServer< IVTracer >
{ 
public:
    TVAdaptor()
    { 
    }
    ~TVAdaptor()
    { 
        Close() ;
    }

    template< typename F >
    void Peer( F f )
    {
        TVServiceProxy< TVConstRef< TINPUT > > dtProxy ;

        m_proxySrc.UseServer( [ &dtProxy ]( IVDataProv< TINPUT > & prov ){
            prov.Apply( VD_P2U( dtProxy ) ) ;
        } ); 

        dtProxy.UseServer( [ f ]( const TVConstRef< TINPUT > & cb ){
            f( TTRANS::Translate( cb.m_ref ) );
        } ) ; 
    }

private:
    typedef TVAdaptor< TINPUT , TOUTPUT , TTRANS > my_type;
    
    struct Clnt ;

    struct AdpProv : IVDataProv< TOUTPUT >
    {
        Clnt & owner ;
        AdpProv( Clnt & c ) : owner( c ) {}
        virtual void Apply( VO< TVConstRef< TOUTPUT > > & usr ) 
        {
            owner._apply( usr ); 
        }
    };

    struct Clnt 
    {
    public :
        template< typename F >
        Clnt( TVConstRef< TINPUT > & cr , F f ):m_input(cr)
        {
            m_svcProv.RunNew( f , *this ) ;
        }
        ~Clnt()
        {
            m_svcProv.Close() ;
            m_svcData.Close() ;
        }
        void _apply( VO< TVConstRef< TOUTPUT > > & usr )
        { 
            m_svcData.RunNew( usr , TTRANS::Translate( m_input.m_ref ) ) ;
        }

    private :
        TVConstRef< TINPUT > & m_input ;
        TVServiceArray< IVDataProv< TOUTPUT > , AdpProv > m_svcProv ;
        TVServiceArray< TVConstRef< TOUTPUT > > m_svcData ;
    } ;

    struct Prov : IVDataProv< TOUTPUT >
    {
        my_type & me;
        Prov( my_type & m ) : me( m ) {}
        virtual void Apply( VO< TVConstRef< TOUTPUT > > & usr ) 
        {
            me._apply( usr ); 
        }
    };

private:
    void _apply( VO< TVConstRef< TOUTPUT > > & usr )
    {  
        if( m_proxyProv.IsClosed() )
        {
            m_proxySrc.UseServer( [ this ]( IVDataProv< TINPUT > & prov ){
                prov.Apply( VD_L2U( [ this ]( VI< TVConstRef< TINPUT > > & spc ){
                    m_clntInput.Create( spc , [ this ]( VI< IVDataProv< TOUTPUT > > & spc ){
                        m_proxyProv.Create( spc ) ;
                    } ) ;
                } ) ) ;
            } ) ;
        }

        m_proxyProv.UseServer( [&usr]( auto & prov ){
            prov.Apply( usr ) ;
        } ) ;
    }

    void _notify()
    {
        m_listeners.TidyTravel( []( auto & t ){
            t.OnChanged() ;
        } ) ;
    }

public: 
    void Close()
    {
        TVServer< IVTracer >::Close() ;
        m_svcProv.Close() ;
    }

    void Output( IVInputPort< TOUTPUT > &  inp )
    {
        inp.Input( *this ) ;
    }

    void Input( IVSlot< TINPUT > & slot )
    {
        TVServer< IVTracer >::Run( [ this , & slot ]( VI< IVTracer > & spcTracer ){
            slot.Trace( spcTracer ) ; 
        } );

        slot.GetData( VD_P2U( m_proxySrc ) ) ;

        _notify() ;
    };

    // IVSlot< T >
    void Trace( VI< IVTracer > & spc )
    {
        m_listeners.Add( spc ) ;
    }

    void GetData( VO< IVDataProv< TOUTPUT > > & usr )
    {  
        m_svcProv.RunNew( usr , *this ) ;
    }

    // IVTracer
    void OnChanged()
    {
        _notify() ;
    }

    VD_DECLARE_NO_COPY_TEMPLATE_CLASS3( TVAdaptor , TINPUT , TOUTPUT , TTRANS );

private:
    TVServiceProxyArray< IVTracer         > m_listeners ; 
    TVServiceProxy< IVDataProv< TINPUT  > > m_proxySrc  ;
    TVClient< Clnt , TVConstRef< TINPUT > > m_clntInput ;
    TVServiceProxy< IVDataProv< TOUTPUT > > m_proxyProv ;
    TVServiceArray< IVDataProv< TOUTPUT > , Prov > m_svcProv;
} ;

#define VD_ADP_MEM( name , c , m ) TVAdaptor< c , decltype(((c*)0)->m) , TVRTransMemberTrait< decltype(((c*)0)->m) , c , offsetof( c , m ) > > name
#define VD_ADP_DRV( name , c , b ) TVAdaptor< c , b > name ;

template< class T , class TImp = T >
class TVSource : public IVSlot< T >
{
public:
    TVSource( const T & src )
        : m_imp( src )
    {
    }
    ~TVSource()
    {
        m_svcDyn.Close() ;
        m_svcData.Close();
    }

private:
    typedef TVSource< T , TImp > my_type;

    struct Prov : IVDataProv< T >
    {
        my_type & me;
        Prov( my_type & m ) : me( m ) {}
        void Apply( VO< TVConstRef< T > > & usr ){ me._getData( usr ); }
    };

public:
    virtual void Trace( VI< IVTracer > & spc )
    {
        m_listener.Add( spc ) ;
    }
    void GetData( VO< IVDataProv< T > > & usr )
    {
        m_svcDyn.RunNew( usr , *this );
    }
    void Reset( const T & src )
    {
        m_imp = src ;
        m_listener.TidyTravel( []( auto & t ){ t.OnChanged() ; } ) ;
    }
    
    const TImp & operator*() const 
    {
        return _getValue() ;
    }

    const TImp * operator->() const 
    { 
        return &(_getValue());
    } 

    void Output( IVInputPort< T > &  inp )
    {
        inp.Input( *this ) ;
    }

private:
    void _getData( VO< TVConstRef< T > > & usr )
    {
        m_svcData.RunNew( usr , m_imp );
    }
    const TImp & _getValue() const
    {	// return wrapped pointer
        return ( m_imp );
    }
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS2( TVSource , T , TImp );

private:
    TImp m_imp ;
    TVServiceArray< TVConstRef< T > > m_svcData;
    TVServiceArray< IVDataProv< T > , Prov > m_svcDyn;
    TVServiceProxyArray< IVTracer >          m_listener ;
};


template< class T >
class TVSourceConstBuffer : public TVSource< VSConstBuffer< T > >
{
public:
    TVSourceConstBuffer( const T * data , unsigned len )
        :TVSource< VSConstBuffer< T > >( m_Proxy )
    {
        m_Data.resize( len );
        memcpy_s( &m_Data[ 0 ] , len , data , len );
        m_Proxy.len = len;
        m_Proxy.pData = &m_Data[ 0 ];
    }

    ~TVSourceConstBuffer(){}

private:
    std::vector< T > m_Data;
    VSConstBuffer< T > m_Proxy;
};

template<>
class TVSourceConstBuffer< char > : public TVSource< VSConstBuffer< char > >
{
public:
    TVSourceConstBuffer( const char * data )
        :TVSource< VSConstBuffer< char > >( m_Proxy )
    {
        m_Data = data;
        m_Proxy.len = m_Data.size();
        m_Proxy.pData = m_Data.c_str();
    }

    ~TVSourceConstBuffer(){}

private:
    std::string m_Data;
    VSConstBuffer< char > m_Proxy;
};

template<>
class TVSourceConstBuffer< void > : public TVSource< VSConstBuffer< char > >
{
public:
    TVSourceConstBuffer( const void * pData , unsigned len )
        :TVSource< VSConstBuffer< char > >( m_Proxy )
    {
        m_Proxy.len = len;
        m_Proxy.pData = static_cast< const char* >( pData );
    }
    ~TVSourceConstBuffer(){}

private:
    VSConstBuffer< char > m_Proxy;
};
 
template< typename TF > class TVHubCreator ;
 
template< typename ... TRs >
class TVHubCreator< IVSysDynamic< TRs ... > >
{
public :
    template< typename TTARGID , typename TARGOU , typename ... TTARGINPUTs >
    static void Create( IVSysDynamic< TRs ... > & sys , VO< IVRUniqHub< TTARGID , TARGOU , TTARGINPUTs ... > > & usr )
    {
        IVSysDynamicBase< TRs ... > & sysNxt = sys ;
        _create( sysNxt , usr ) ;
    }

private : 
    //template< typename TTARGID , typename TARGOU , typename ... TTARGINPUTs >
    //static void _create( IVSysDynamicBase<> & sys , VO< IVRUniqHub< TTARGID , TARGOU , TTARGINPUTs ... > > & usr )
    //{ 
    //    static_assert( 0 , "The type of created hub is not exist .... Error! " ) ;
    //} 
    template< typename TLID , typename ... TROTHERs , typename TTARGID , typename TARGOU , typename ... TTARGINPUTs >
    static void _create( IVSysDynamicBase< IVRLTN< TLID > , TROTHERs ... > & sys 
                       , VO< IVRUniqHub< TTARGID , TARGOU , TTARGINPUTs ... > > & usr )
    { 
        _create( static_cast< IVSysDynamicBase< TROTHERs ... > & >( sys ) , usr ) ;
    }

    template< typename TLID , typename ... TROTHERs , typename TARGOU , typename ... TTARGINPUTs >
    static void _create( IVSysDynamicBase< IVRLTN< TLID > , TROTHERs ... > & sys 
                       , VO< IVRUniqHub< TLID , TARGOU , TTARGINPUTs ... > > & usr )
    { 
        sys.Create( usr ) ;
    } 
} ;

template< class TR > class TVHub ; 
template< typename TID , typename TOU , typename TNAME > class TVHubBase ;

class TVHubBaseUtil
{
public :
    //template< typename TTARG >
    //static void _connect( IVRHub<> & hub , IVSlot< TTARG > & val )
    //{
    //    static_assert( 0 , "The type of TTARG is not composition of this hub ..... Error !" ) ;
    //} 
    template< typename TI , typename ... INPUTs , typename TTARG >
    static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TTARG > & val )
    {
        _connect( static_cast< IVRHub< INPUTs ... > & >( hub ) , val  ) ;
    } 
    template< typename TI , typename ... INPUTs >
    static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & val )
    { 
        hub.Connect( val ) ;
    }
    static void _reset( IVRHub< > & hub )
    {
    } 
    template< typename TI , typename ... INPUTs >
    static void _reset( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & s , IVSlot< INPUTs > & ... otherSlots )
    {
        hub.Connect( s ) ;
        _reset( static_cast< IVRHub< INPUTs ... > & >( hub ) , otherSlots ... ) ;
    }
     
    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    static void _createComp( IVRExp< IVRLTN< TCOMID > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        re.Create( usr ) ;
    }     
    template< typename TEXPOU , typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    static void _createComp( IVRExp< VI_AND< TEXPOU , TEXPOUTPUTs ... > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        _createComp( static_cast< IVRExp< VI_AND< TEXPOUTPUTs ... > > & >( re ) , usr ) ;
    }
    template< typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    static void _createComp( IVRExp< VI_AND< IVRLTN< TCOMID > , TEXPOUTPUTs ... > > & re
                           , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        (( IVRExp< IVRLTN< TCOMID > > & )re).Create( usr ) ;
    }
    //template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    //static void _createComp( IVRExp< VI_AND< > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    //{
    //    static_assert( 0 , "Error ! ") ;
    //} 

    template< typename TTARG >
    static void _export( IVRExp< TTARG > & re , IVInputPort< TTARG > & inp )
    {
        re.Output( inp ) ;
    }     
    template< typename TEXPOU , typename ... TEXPOUTPUTs , typename TTARG >
    static void _export( IVRExp< VI_AND< TEXPOU , TEXPOUTPUTs ... > > & re , IVInputPort< TTARG > & inp )
    {
        _export( static_cast< IVRExp< VI_AND< TEXPOUTPUTs ... > > & >( re ) , inp ) ;
    }
    template< typename ... TEXPOUTPUTs , typename TTARG >
    static void _export( IVRExp< VI_AND< TTARG , TEXPOUTPUTs ... > > & re , IVInputPort< TTARG > & inp )
    {
        static_cast< IVRExp< TTARG >& >(re).Output( inp ) ; 
    }
    //template< typename TTARG >
    //static void _export( IVRExp< VI_AND< > > & re , IVInputPort< TTARG > & inp )
    //{
    //    static_assert( 0 , "Error ! ") ;
    //} 
} ;

template< typename TID , typename TOU , typename ... INPUTs >
class TVHubBase< TID , TOU , VI_AND< INPUTs ... > > : public IVSlot< IVRLTN< TID  > > 
{
public :
    // Constructor
    TVHubBase()
    {
    }
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys ) ;
    }
    template< class TS >
    TVHubBase( TS & sys , IVSlot< INPUTs > & ... types )
    {
        Create( sys , types ... ) ;
    }
    ~TVHubBase()
    {
    }
    
private :
    TVHubBase( const TVHubBase< TID , TOU , VI_AND< INPUTs ... > >& ) ;            
    TVHubBase& operator=( const TVHubBase< TID , TOU , VI_AND< INPUTs ... > >& ) ;

public :
    // Creation
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) ) ;
    }  
    template< class TS >
    void Create( TS & sys , IVSlot< INPUTs > & ... slots )
    {
        Create( sys ) ;
        BatchConnect( slots ... ) ;
    } 

    // Connection 
    template< typename T >
    void Connect( IVSlot< T > & v )
    {
        m_proxy.UseServer( [ & v ]( auto & hub ){
            TVHubBaseUtil::_connect( hub ,  v ) ;
        } ) ;
    } ;
    void BatchConnect( IVSlot< INPUTs > & ... slots )
    {
        m_proxy.UseServer( [ this , & slots ... ]( auto & hub ){
            TVHubBaseUtil::_reset( static_cast< IVRHub< INPUTs ... > & >( hub ) , slots ... ) ;
        } ) ;
    } 

    template< typename TTARG >
    void  Output( IVInputPort< TTARG > & inp )
    {
        m_proxy.UseServer( [ &inp ]( IVRUniqHub< TID , TOU , INPUTs ... > & hub ){
            TVHubBaseUtil::_export( static_cast< IVRExp< TOU > & >( hub ) , inp ) ;
        } ) ;
    }

    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    void CreateComponents( VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        m_proxy.UseServer( [ this , &usr ]( IVRUniqHub< TID , TOU , INPUTs ... > & hub ){
            TVHubBaseUtil::_createComp( static_cast< IVRExp< TOU > & >( hub ) , usr ) ;
        } ) ;
    } 

    // Destory
    void Destory()
    {
        m_proxy.Destory() ;
    } 

private :
    virtual void Present()
    {
        m_proxy.UseServer( []( IVRUniqHub< TID , TOU , INPUTs ... > & im ){
            im.Participate() ;
        } , 
        []{
            VASSERT(0);
        }) ;
    }

//private :
    //template< typename TTARG >
    //static void _connect( IVRHub<> & hub , IVSlot< TTARG > & val )
    //{
    //    static_assert( 0 , "The type of TTARG is not composition of this hub ..... Error !" ) ;
    //} 
    //template< typename TI , typename ... INPUTs , typename TTARG >
    //static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TTARG > & val )
    //{
    //    _connect( static_cast< IVRHub< INPUTs ... > & >( hub ) , val  ) ;
    //} 
    //template< typename TI , typename ... INPUTs >
    //static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & val )
    //{ 
    //    hub.Connect( val ) ;
    //}
    //void _reset( IVRHub< > & hub )
    //{
    //} 
    //template< typename TI , typename ... INPUTs >
    //void _reset( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & s , IVSlot< INPUTs > & ... otherSlots )
    //{
    //    hub.Connect( s ) ;
    //    _reset( static_cast< IVRHub< INPUTs ... > & >( hub ) , otherSlots ... ) ;
    //}
    // 
    //template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    //void _createComp( IVRExp< IVRLTN< TCOMID > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    //{
    //    re.Create( usr ) ;
    //}     
    //template< typename TEXPOU , typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    //void _createComp( IVRExp< VI_AND< TEXPOU , TEXPOUTPUTs ... > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    //{
    //    _createComp( static_cast< IVRExp< VI_AND< TEXPOUTPUTs ... > > & >( re ) , usr ) ;
    //}
    //template< typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    //void _createComp( IVRExp< VI_AND< IVRLTN< TCOMID > , TEXPOUTPUTs ... > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    //{
    //    re.Create( usr ) ;
    //}
    //template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    //void _createComp( IVRExp< VI_AND< > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    //{
    //    static_assert( 0 , "Error ! ") ;
    //} 

private :
    TVServiceProxy< IVRUniqHub< TID , TOU , INPUTs ... > > m_proxy ;
} ;

// Evander added -- 20161223
template< typename TID , typename TOU >
class TVHubBase< TID , TOU , VI_AND<> > : public IVSlot< IVRLTN< TID  > >
{
public:
    // Constructor
    TVHubBase(){}
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys );
    }
    ~TVHubBase(){}

private:
    TVHubBase( const TVHubBase< TID , TOU , VI_AND<> >& );
    TVHubBase& operator=( const TVHubBase< TID , TOU , VI_AND<> >& );

public:
    // Creation
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) );
    }

    void BatchConnect( )
    {
    }

    // Connection 
    //template< typename T >
    //void Connect( IVSlot< T > & v )
    //{
    //    m_proxy.UseServer( [ &v ]( auto & hub ) {
    //        TVHubBaseUtil::_connect( hub , v );
    //    } );
    //};
    
    template< typename TTARG >
    void  Output( IVInputPort< TTARG > & inp )
    {
        m_proxy.UseServer( [ &inp ]( IVRUniqHub< TID , TOU > & hub ) {
            TVHubBaseUtil::_export( static_cast< IVRExp< TOU > & >( hub ) , inp );
        } );
    }

    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    void CreateComponents( VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        m_proxy.UseServer( [ this , &usr ]( IVRUniqHub< TID , TOU > & hub ) {
            TVHubBaseUtil::_createComp( static_cast< IVRExp< TOU > & >( hub ) , usr );
        } );
    }

    // Destory
    void Destory()
    {
        m_proxy.Destory();
    }

private:
    virtual void Present()
    {
        m_proxy.UseServer( []( IVRUniqHub< TID , TOU > & im ) {
            im.Participate();
        } ,
                           [] {
            VASSERT( 0 );
        } );
    }

    //private :
        //template< typename TTARG >
        //static void _connect( IVRHub<> & hub , IVSlot< TTARG > & val )
        //{
        //    static_assert( 0 , "The type of TTARG is not composition of this hub ..... Error !" ) ;
        //} 
        //template< typename TI , typename ... INPUTs , typename TTARG >
        //static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TTARG > & val )
        //{
        //    _connect( static_cast< IVRHub< INPUTs ... > & >( hub ) , val  ) ;
        //} 
        //template< typename TI , typename ... INPUTs >
        //static void _connect( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & val )
        //{ 
        //    hub.Connect( val ) ;
        //}
        //void _reset( IVRHub< > & hub )
        //{
        //} 
        //template< typename TI , typename ... INPUTs >
        //void _reset( IVRHub< TI , INPUTs ... > & hub , IVSlot< TI > & s , IVSlot< INPUTs > & ... otherSlots )
        //{
        //    hub.Connect( s ) ;
        //    _reset( static_cast< IVRHub< INPUTs ... > & >( hub ) , otherSlots ... ) ;
        //}
        // 
        //template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
        //void _createComp( IVRExp< IVRLTN< TCOMID > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
        //{
        //    re.Create( usr ) ;
        //}     
        //template< typename TEXPOU , typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
        //void _createComp( IVRExp< VI_AND< TEXPOU , TEXPOUTPUTs ... > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
        //{
        //    _createComp( static_cast< IVRExp< VI_AND< TEXPOUTPUTs ... > > & >( re ) , usr ) ;
        //}
        //template< typename ... TEXPOUTPUTs , typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
        //void _createComp( IVRExp< VI_AND< IVRLTN< TCOMID > , TEXPOUTPUTs ... > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
        //{
        //    re.Create( usr ) ;
        //}
        //template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
        //void _createComp( IVRExp< VI_AND< > > & re , VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
        //{
        //    static_assert( 0 , "Error ! ") ;
        //} 

private:
    TVServiceProxy< IVRUniqHub< TID , TOU > > m_proxy;
};

// Evander added -- 20161109
template< typename TID , typename TOU , typename ... INPUTs >
class TVHubBase< TID , TOU , VI_OR< INPUTs ... > > : public IVSlot< IVRLTN< TID  > >
{
public:
    // Constructor
    TVHubBase(){}
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys );
    }
    template< class TS , class TI >
    TVHubBase( TS & sys , IVSlot< TI > & slot )
    {
        Create( sys , slot );
    }
    ~TVHubBase()
    {}

private:
    TVHubBase( const TVHubBase< TID , TOU , VI_OR< INPUTs ... > >& );
    TVHubBase& operator=( const TVHubBase< TID , TOU , VI_OR< INPUTs ... > >& );

public:
    // Creation
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) );
    }
    template< class TS , class TI >
    void Create( TS & sys , IVSlot< TI > & slot )
    {
        Create( sys );
        Connect( slot );
    }

    // Connection 
    template< typename T >
    void Connect( IVSlot< T > & v )
    {
        m_proxy.UseServer( [ &v ]( auto & hub ) {
            TVHubBaseUtil::_connect( hub , v );
        } );
    };

    template< typename TTARG >
    void  Output( IVInputPort< TTARG > & inp )
    {
        m_proxy.UseServer( [ &inp ]( IVRUniqHub< TID , TOU , INPUTs ... > & hub ) {
            TVHubBaseUtil::_export( static_cast< IVRExp< TOU > & >( hub ) , inp );
        } );
    }

    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    void CreateComponents( VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        m_proxy.UseServer( [ this , &usr ]( IVRUniqHub< TID , TOU , INPUTs ... > & hub ) {
            TVHubBaseUtil::_createComp( static_cast< IVRExp< TOU > & >( hub ) , usr );
        } );
    }

    // Destory
    void Destory()
    {
        m_proxy.Destory();
    }

private:
    virtual void Present()
    {
        m_proxy.UseServer( []( IVRUniqHub< TID , TOU , INPUTs ... > & im ) {
            im.Participate();
        } ,
                           [] {
            VASSERT( 0 );
        } );
    }

private:
    TVServiceProxy< IVRUniqHub< TID , TOU , INPUTs ... > > m_proxy;
};
//////////////////////////////////////////////////////////////////////////

template< typename TID , typename TOU , typename TI >
class TVHubBase< TID , TOU , VI_ARR< TI > > : public IVSlot< IVRLTN< TID  > > 
{
public : 
    typedef TI              RITEM    ;  
    
public :
    // Constructor
    TVHubBase()
    {
    }
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys ) ;
    }
    ~TVHubBase()
    {
    }
         
private:                                             
    TVHubBase( const TVHubBase< TID , TOU , VI_ARR< TI > > & ) ;            
    TVHubBase& operator=( const TVHubBase< TID , TOU , VI_ARR< TI > > & ) ;

public :
    // Creation 
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) ); 
    }  

    // Connection
    void PushBack( IVSlot< TI > & v )
    {
        m_proxy.UseServer( [ & v ]( auto & hub ){
            hub.Connect( v ) ;
        } ) ;
    } ; 
     
    // Destory
    void Destory()
    {
        m_proxy.Destory() ;
    } 

private :
    virtual void Present()
    {
        m_proxy.UseServer( []( auto & im ){
            im.Participate() ;
        } , 
        []{
            VASSERT_MSG( 0 , "Using hub befor it's creation. error ! ");
        }) ;
    }

private :
    TVServiceProxy< IVRUniqHub< TID , TOU , RITEM > > m_proxy ; 
} ;

template< typename TID , typename TOU , typename TITEMID >
class TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR< IVRLTN< TITEMID > > > : public IVSlot< IVRLTN< TID  > > 
{
public : 
    typedef IVRLTN< TITEMID >     RITEM    ;  
    
public :
    // Constructor
    TVHubBase()
    {
    }
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys ) ;
    }
    ~TVHubBase()
    {
    }
         
private:                                             
    TVHubBase( const TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR< IVRLTN< TITEMID > > > & ) ;            
    TVHubBase& operator=( const TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR< IVRLTN< TITEMID > > > & ) ;

public :
    // Creation 
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) ); 
    }  

    // Connection
    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    void CreateComponents( VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        m_proxy.UseServer( [ this , &usr ]( IVRUniqHub< TID , VI_AND< RITEM , TOU > > & hub ){
            TVHubBaseUtil::_createComp( hub , usr );
        } );
    } 
     
    // Destory
    void Destory()
    {
        m_proxy.Destory() ;
    } 

private :
    virtual void Present()
    {
        m_proxy.UseServer( []( auto & im ){
            im.Participate() ;
        } , 
        []{
            VASSERT_MSG( 0 , "Using hub befor it's creation. error ! ");
        }) ;
    }

private :
     TVServiceProxy< IVRUniqHub< TID , VI_AND< RITEM , TOU > > > m_proxy ;  
} ;

template< typename TID , typename TOU , typename TI >
class TVHubBase< TID , TOU , VI_ARR_01< TI > > : public IVSlot< IVRLTN< TID  > >
{
public:
    typedef TI              RITEM;

public:
    // Constructor
    TVHubBase(){}
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys );
    }
    ~TVHubBase(){}

private:
    TVHubBase( const TVHubBase< TID , TOU , VI_ARR_01< TI > > & );
    TVHubBase& operator=( const TVHubBase< TID , TOU , VI_ARR_01< TI > > & );

public:
    // Creation 
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) );
    }

    // Connection
    void Connect( IVSlot< TI > & v )
    {
        m_proxy.UseServer( [ &v ]( auto & hub ) {
            hub.Connect( v );
        } );
    };

    // Destory
    void Destory()
    {
        m_proxy.Destory();
    }

private:
    virtual void Present()
    {
        m_proxy.UseServer( []( auto & im ) {
            im.Participate();
        } ,
                           [] {
            VASSERT_MSG( 0 , "Using hub befor it's creation. error ! " );
        } );
    }

private:
    TVServiceProxy< IVRUniqHub< TID , TOU , RITEM > > m_proxy;
};

template< typename TID , typename TOU , typename TITEMID >
class TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR_01< IVRLTN< TITEMID > > > : public IVSlot< IVRLTN< TID  > >
{
public:
    typedef IVRLTN< TITEMID >     RITEM;

public:
    // Constructor
    TVHubBase(){}
    template< class TS >
    TVHubBase( TS & sys )
    {
        Create( sys );
    }
    ~TVHubBase(){}

private:
    TVHubBase( const TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR_01< IVRLTN< TITEMID > > > & );
    TVHubBase& operator=( const TVHubBase< TID , VI_AND< IVRLTN< TITEMID > , TOU > , VI_ARR_01< IVRLTN< TITEMID > > > & );

public:
    // Creation 
    template< typename ... TRs >
    void Create( IVSysDynamic< TRs ... > & sys )
    {
        TVHubCreator< IVSysDynamic< TRs ... > >::Create( sys , VD_P2U( m_proxy ) );
    }
    template< typename TR >
    void Create( TVHub< TR > & hub )
    {
        hub.CreateComponents( VD_P2U( m_proxy ) );
    }

    // Connection
    template< typename TCOMID , typename TCOMVALUE , typename ... TCOMINPUTs >
    void CreateComponents( VO< IVRUniqHub< TCOMID , TCOMVALUE , TCOMINPUTs ... > > & usr )
    {
        m_proxy.UseServer( [ this , &usr ]( IVRUniqHub< TID , VI_AND< RITEM , TOU > > & hub ) {
            TVHubBaseUtil::_createComp( hub , usr );
        } );
    }

    // Destory
    void Destory()
    {
        m_proxy.Destory();
    }

private:
    virtual void Present()
    {
        m_proxy.UseServer( []( auto & im ) {
            im.Participate();
        } ,
                           [] {
            VASSERT_MSG( 0 , "Using hub befor it's creation. error ! " );
        } );
    }

private:
    TVServiceProxy< IVRUniqHub< TID , VI_AND< RITEM , TOU > > > m_proxy;
};

template< typename TID >
class TVHub< IVRLTN< TID > > : public TVHubBase< TID , typename IVRLTN<TID>::VALUE , typename IVRLTN< TID >::NAME >
{  
    using TVHubBase< TID , typename IVRLTN<TID>::VALUE , typename IVRLTN< TID >::NAME >::TVHubBase ;
}; 
//
/////////////////////////////////////////////////////////////////////////////////

// Converter 

/////////////////////////////////////////////////////////////////////////////////
template< typename TIMP , typename TEXP > 
class TVConverterData
{
public :
    TVConverterData()
#ifdef VD_NOT_CARE_PERFORMANCE
        : m_bDataValid( false )
#endif    
    {
    }
    ~TVConverterData()
    {  
        m_svcExp.Close() ;
        m_svcProv.Close() ;
    }

public : 
    void Close()
    {
        m_svcExp.Close() ;
        m_svcProv.Close() ;
    }

    void RunProvider( VO< IVDataProv< TEXP > > & usr )
    {
        m_svcProv.RunNew( usr , *this ) ;
    }
       
    template< typename ... TARGs >
    bool Update( const TARGs & ... args )
    {
        return _call( &TIMP::Build , m_instCnvtr , m_expValue , args ... ) ;
    }

    virtual void SignalDirty() = 0 ;
     
#ifdef VD_NOT_CARE_PERFORMANCE
    void Validate( bool bv )
    {
        m_bDataValid = bv ;
    }
    bool m_bDataValid ;
#endif

private :
    typedef TVConverterData< TIMP , TEXP > my_type ;
    struct ProvAdp : public IVDataProv< TEXP >
    { 
        my_type & me ; 
        ProvAdp( my_type & m ) :me( m ){}
        ~ProvAdp(){} 
        void Apply( VO< TVConstRef< TEXP > > & usr )
        {
            me._applyExpData( usr ) ;
        }
    };

    void _applyExpData( VO< TVConstRef< TEXP > > & usr )
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        VASSERT( m_bDataValid ) ;
        if( m_bDataValid )
        {
           m_svcExp.RunNew( usr , m_expValue ) ;
        }
#else
        m_svcExp.RunNew( usr , m_expValue ) ;
#endif
    }
     
private : 
    template< typename ... TARGs >
    bool _call( bool ( TIMP::* f )( TEXP & expVal , const TARGs & ... ) , TIMP & cnvtr , TEXP & expVal , const TARGs & ... args )
    {
        return (cnvtr.*f)( expVal , args ... ) ;
    }

    template< typename ... TARGs >
    bool _call( void ( TIMP::* f )( TEXP & expVal , const TARGs & ... ) , TIMP & cnvtr , TEXP & expVal , const TARGs & ... args ) 
    {  
        (cnvtr.*f)( expVal , args ... ) ;
        return true ;
    }

    TIMP m_instCnvtr  ;
    TEXP m_expValue   ; 
    TVServiceArray< TVConstRef< TEXP > >               m_svcExp        ; 
    TVServiceArray< IVDataProv< TEXP > , ProvAdp     > m_svcProv       ; 
} ;

template< typename TIMP > 
class TVConverterData< TIMP , void >
{
public :
    TVConverterData()
#ifdef VD_NOT_CARE_PERFORMANCE
        : m_bDataValid( false )
#endif    
    {
    }
    ~TVConverterData()
    {
    }

public : 
    void Close()
    {
    }

    void RunProvider( VO< IVDataProv< void > > & usr )
    {
    }
       
    template< typename ... TARGs >
    bool Update( const TARGs & ... args )
    {
        return _call( &TIMP::Build , m_instCnvtr , args ... ) ;
    }

    virtual void SignalDirty() = 0 ;
     
#ifdef VD_NOT_CARE_PERFORMANCE
    void Validate( bool bv )
    {
        m_bDataValid = bv ;
    }
    bool m_bDataValid ;
#endif
     
private : 
    template< typename ... TARGs >
    bool _call( bool ( TIMP::* f )( const TARGs & ... ) , TIMP & cnvtr , const TARGs & ... args )
    {
        return (cnvtr.*f)( args ... ) ;
    }

    template< typename ... TARGs >
    bool _call( void ( TIMP::* f )( const TARGs & ... ) , TIMP & cnvtr , const TARGs & ... args ) 
    {  
        (cnvtr.*f)( args ... ) ;
        return true ;
    }
    TIMP m_instCnvtr  ;
} ;

template< typename TIMP , typename TEXP , typename ... TINPUTs > struct TVConverterClntArg ;

template< typename TIMP , typename TEXP > struct TVConverterClntArg< TIMP , TEXP >
{
    TVConverterClntArg()
    {}
    ~TVConverterClntArg()
    {}

    template< typename ... TARGs >
    bool Update( TVConverterData< TIMP , TEXP > & expVal , const TARGs & ... args ) 
    {
        return expVal.Update( args ... ) ;
    } 
};

template< typename TIMP , typename TEXP , typename TI , typename ... TOTHERs > struct TVConverterClntArg< TIMP , TEXP , TI , TOTHERs ... >
{
    typedef TVConverterClntArg< TIMP , TEXP , TOTHERs ... > sub_type ;
    TVConstRef< TI > & m_refArg ; 
    sub_type           m_subValue ;

    TVConverterClntArg( TVConstRef< TI > & rt , TVConstRef< TOTHERs > & ... rothers )
        : m_refArg( rt )
        , m_subValue( rothers ... )
    {
    } ; 

    template< typename ... TARGs >
    bool Update( TVConverterData< TIMP , TEXP > & expVal , const TARGs & ... args ) 
    { 
        return m_subValue.template Update< TARGs ... , TI >( expVal , args ... , m_refArg.m_ref ) ;
    } 
} ; 

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverterClnt 
{
public :
    typedef TVConverterData< TIMP , TEXP > data_type ;
    typedef TVRef< data_type >             data_ref  ;

    template< typename FU >
    TVConverterClnt( data_ref & refData , TVConstRef< TINPUTs > & ... spcArgs , FU f )
        : m_refArgs( spcArgs ... )
        , m_refData( refData )
    {
        m_svcSelf.Run( f , *this ) ; 
    }
    ~TVConverterClnt()
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        m_refData.m_ref.Validate( false ) ;
#endif
        m_svcSelf.Close();
    }
   
public :
    typedef TVConverterClnt< TIMP , TEXP , TINPUTs ... > my_type ;
    typedef TVRef< my_type >                             my_ref  ; 

    bool UpdateData()
    {    
#ifdef VD_NOT_CARE_PERFORMANCE
        m_refData.m_ref.Validate( true ) ;
#endif
        return m_refArgs.template Update<>( m_refData.m_ref ) ;
    }

private : 
    data_ref                                        & m_refData ;
    TVService< my_ref >                               m_svcSelf ;
    TVConverterClntArg< TIMP , TEXP , TINPUTs ... >   m_refArgs ; 
} ;

template< typename TIMP , typename TEXP , typename ... TINPUTs > class TVConverterEntry ;

template< typename TIMP , typename TEXP >
class TVConverterEntry< TIMP , TEXP > 
{
public :
    typedef TVConverterData< TIMP , TEXP > data_type ;
    typedef TVRef< data_type >             data_ref  ;

    TVConverterEntry( TVConverterData< TIMP , TEXP > & tr ) 
        : m_notifier( tr )
    {
    }
    ~TVConverterEntry()
    {
    } 
    void Close() 
    {
        m_svcData.Close() ;
    }
    void OnChanged()
    {
        m_notifier.SignalDirty() ;
    }
    template< class F , typename ... SPCs >
    void AccessData( F f , SPCs & ... spcs )
    {
        m_svcData.Run( [ f , & spcs ... ]( VI< data_ref > & spcData ) {
            f( spcData , spcs ... ) ;
        }, m_notifier ) ;
    }

private :
    TVConverterData< TIMP , TEXP > & m_notifier ;
    TVService< TVRef< TVConverterData< TIMP , TEXP > > > m_svcData ;
} ;

template< typename TIMP , typename TEXP , typename TI , typename ... TIOTHERs >
class TVConverterEntry< TIMP , TEXP , TI , TIOTHERs ... > : TVServer< IVTracer >
{
public :
    TVConverterEntry( TVConverterData< TIMP , TEXP > & tr , IVSlot< TI > & slot , IVSlot< TIOTHERs > & ... otherSlots )
        :m_subInputs( tr , otherSlots ... ) 
    {
        TVServer< IVTracer >::Run( [ & slot ]( auto & spc ){
            slot.Trace( spc ) ;
        } ) ;

        slot.GetData( VD_P2U( m_prov ) ) ;
    }

    ~TVConverterEntry()
    {
        TVServer< IVTracer >::Close();
    }
     
    void Close() 
    {
        m_subInputs.Close() ;
    }

public :
    virtual void OnChanged()
    {
        m_subInputs.OnChanged() ;
    }

public :
    template< class F , typename ... SPCs >
    void AccessData( F f , SPCs & ... spcs )
    { 
        m_prov.UseServer( [ this , f, &spcs ... ]( auto & prov ){
            prov.Apply( VD_L2U( [ this , f , &spcs ...]( VI< TVConstRef< TI > > & spcVal ){
                m_subInputs.AccessData( f , spcs ... , spcVal ) ;
            } ) ) ;
        } ) ;
    }  

private :
    typedef TVConverterEntry< TIMP , TEXP , TIOTHERs ... > sub_type ;
    sub_type m_subInputs ;
    TVServiceProxy< IVDataProv< TI > > m_prov ;
} ;

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverterTracer 
{
public : 
    template< class FU >
    TVConverterTracer( TVConverterData< TIMP , TEXP > & tr , FU f , IVSlot< TINPUTs > & ... slots )
        : m_inputs( tr , slots ... ) 
    {
        m_svcSelf.Run( f , *this ) ;
    }
    ~TVConverterTracer()
    { 
        m_inputs.Close() ;
        m_svcSelf.Close() ;
    }

public : 
    bool UpdateData()
    {
        _check_and_build_data_prov() ;
        
        bool bRtn( false ) ;
        
        m_proxyClnt.UseServer( [ & bRtn ](auto & c ){
            bRtn = c.m_ref.UpdateData() ;
        } ) ;

        return bRtn ;
    }

private :
    typedef TVConverterData< TIMP , TEXP > data_type ;
    typedef TVRef< data_type >             data_ref  ;
    void _check_and_build_data_prov()
    { 
        if( m_proxyClnt.IsClosed() )
        {
            m_inputs.AccessData( [ this ]( VI< data_ref > & spcVal , VI< TVConstRef< TINPUTs > > & ... spcData ){
                m_clntData.Create( spcVal , spcData ... , VD_MP2L( m_proxyClnt ) ) ;
            } ) ; 
        }
    }

private :
    typedef TVConverterEntry< TIMP , TEXP , TINPUTs ... >  inputs_type ;
    typedef TVConverterClnt< TIMP , TEXP , TINPUTs ...  >  clnt_type   ;
    typedef TVRef< clnt_type >                             clnt_ref    ;
    typedef TVConverterTracer< TIMP , TEXP , TINPUTs ... > my_type ;
    typedef TVRef< my_type >                               my_ref  ; 

    inputs_type                                                   m_inputs     ; 
    TVClient< clnt_type , data_ref , TVConstRef< TINPUTs > ... > m_clntData   ; 
    TVServiceProxy< clnt_ref >                                    m_proxyClnt  ;
    TVService< my_ref >                                           m_svcSelf    ;
} ;
 
template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverter : public IVConverter< TEXP , TINPUTs ... >
                  , TVServer< IVDirtyObject >
                  , TVServer< TVConverterData< TIMP , TEXP > >

                  , IVSlot< TEXP >
{  
private :
    typedef TVConverterData< TIMP , TEXP >                 data_type ;
    typedef TVConverterTracer< TIMP , TEXP , TINPUTs ... > data_clnt_type ; 

public :
    TVConverter( VN_ELEMSYS::TVSysImpTimeline & tl )
        : m_timeline( tl ) 
    {
        _registerDirty() ;
    }
    ~TVConverter()
    {
        data_type::Close() ;
        TVServer< IVDirtyObject >::Close() ;
        TVServer< data_type >::Close() ; 
    }

public : 
    virtual void Input ( IVSlot< TINPUTs > & ... slots ) 
    {
        auto runner = [ this , &slots... ]( auto & spcTracer ){
            m_obsvr.Create( spcTracer , VD_MP2L( m_proxyObsvr ) , slots ... );
        };

        TVServer< data_type >::Run( runner ) ;
    }
    virtual void Output( typename TVOutputPortParam< TEXP >::type & ip )
    {
        ip.Input( *this ) ;
    }
 
private :
    virtual void Trace   ( VI< IVTracer > & spc ) 
    {
        m_listeners.Add( spc ) ;
    }
    virtual void GetData ( VO< IVDataProv< TEXP > > & usr ) 
    {
        data_type::RunProvider( usr ) ;
    }
    virtual void SignalDirty()
    {
        if( TVServer< IVDirtyObject >::IsClosed() )
        {
            _registerDirty() ;
        }
    }
    virtual void CleanAndDiffuse()
    {
        bool bInfect( false ) ;

        m_proxyObsvr.UseServer( [ this , & bInfect ](auto&t){
            bInfect = t.m_ref.UpdateData() ; 
        }) ;

        if( bInfect )
            m_listeners.TidyTravel( [](auto&t){t.OnChanged() ; } ) ;
    }
     
private :
    void _registerDirty()
    {
        TVServer< IVDirtyObject >::Run( [this]( auto & spcDirty ){   
            m_timeline.RegisterDirty( spcDirty ) ; 
        } ) ;
    }

private :
    VN_ELEMSYS::TVSysImpTimeline                     & m_timeline      ;
    TVServiceProxyArray< IVTracer >                    m_listeners     ; 
    TVClient< data_clnt_type , data_type   >           m_obsvr         ;
    TVServiceProxy< TVRef< data_clnt_type > >          m_proxyObsvr    ;
} ;
 
template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVSvcConverter
{
public :
    TVSvcConverter()
    {}
    ~TVSvcConverter()
    {}

public :
    typedef TEXP                                     exp_type    ;
    typedef IVConverter< TEXP , TINPUTs ...        > interf_type ;
    typedef TVConverter< TIMP , TEXP , TINPUTs ... > imp_type    ;

public : 
    void Close()
    {
        m_svc.Close() ;
    }
    void Elapse()
    {
        m_timeline.ClearDirty() ;
    } 
    void RunService( VO< interf_type > & pipe )
    {
        m_svc.RunNew( pipe , m_timeline ) ;
    } 
    template< typename T >
    void Init( const T & v )
    {
    }
private :
    VN_ELEMSYS::TVSysImpTimeline m_timeline ;
    TVServiceArray< interf_type , imp_type > m_svc ;
} ;

// 01Switcher
template< typename T >
class TVSwitcherData : public VSConstBuffer< const T * >
{
public:
    TVSwitcherData( VI< TVConstRef< T > > & spcValue , std::vector< const T * > & arr )
    {
        m_svcList.Run( [ this , &spcValue ]( VI< TVRef< std::vector< const T * > > > & spcArr ) {
            m_clnt.Create( spcValue , spcArr );
        } , arr );
    }

    ~TVSwitcherData()
    {
        m_svcList.Close();
    }

private:
    struct VKeeper
    {
    public:
#ifdef VD_NOT_CARE_PERFORMANCE
        VKeeper( const TVConstRef< T > & val , TVRef< std::vector< const T * > > & ctr )
            : m_container( ctr.m_ref )
        {
            idxInContainer = m_container.size();
            m_container.push_back( &val.m_ref );
        }
        ~VKeeper()
        {
            VASSERT( m_container.size() > idxInContainer );
            m_container[ idxInContainer ] = 0;
        }
        unsigned idxInContainer;
        std::vector< const T * > & m_container;
#else
        VKeeper( const TVConstRef< T > & val , TVRef< std::vector< const T * > > & ctr )
        {
            ctr.m_ref.push_back( &val.m_ref );
        }
        ~VKeeper()
        {
    }
#endif
};

private:
    TVClient< VKeeper , TVConstRef< T > , TVRef< std::vector< const T * > > > m_clnt;
    TVService< TVRef< std::vector< const T * > > > m_svcList;
};

template< typename T >
class TVSwitcher : public IVConverter< VS01Buffer< T > , T >
                 , IVSlot< VS01Buffer< T > >
                 , TVServer< IVDirtyObject >
{
public:
    typedef VS01Buffer< T > TOUTPUT;

    TVSwitcher( VN_ELEMSYS::TVSysImpTimeline & tl )
        :m_timeline( tl )
    {}
    ~TVSwitcher()
    {
        m_tracers.Close();
        m_svcData.Close();
        m_svcExp.Close();
        m_svcProv.Close();
        TVServer< IVDirtyObject > ::Close();
    }
private:
    virtual void Input( IVSlot< T > & inVal )
    {
        m_tracers.Run( [ &inVal ]( auto & spc ) { inVal.Trace( spc ); } , *this );
        inVal.GetData( VD_L2U( [ this ]( VI< IVDataProv< T > > & spc ) {
            m_itemProv.Create( spc );
        } ) );
        _notifyChange( true );
    }
    virtual void Output( IVInputPort< TOUTPUT > & outVal )
    {
        outVal.Input( *static_cast< IVSlot< TOUTPUT > * >( this ) );
    }

    // IVSlot< VSConstBuffer< const T * > >
    virtual void Trace( VI< IVTracer > & spc )
    {
        m_listeners.Add( spc );
    }
    virtual void GetData( VO< IVDataProv< TOUTPUT > > & usr )
    {
        m_svcProv.RunNew( usr , *this );
    }

    // IVDirtyObject
    virtual void CleanAndDiffuse()
    {
        if ( m_bLengthChanged )   
            _updateList();

        m_listeners.TidyTravel( []( auto&t ) {t.OnChanged(); } );
    }

private:
    typedef TVSwitcher< T > my_type;
    struct VItemTracer : public IVTracer
    {
        my_type & me;
        VItemTracer( my_type & m ) :me( m )
        {}
        ~VItemTracer()
        {
            me._notifyChange( true );
        }
        virtual void OnChanged()
        {
            me._notifyChange( false );
        }
    };
    struct ProvAdp : public IVDataProv< TOUTPUT >
    {
        my_type & me;
        ProvAdp( my_type & m ) :me( m ){}
        ~ProvAdp(){}
        void Apply( VO< TVConstRef< TOUTPUT > > & usr )
        {
            me._applyExpData( usr );
        }
    };

private:
    void _notifyChange( bool bLenChanged )
    {
        m_bLengthChanged |= bLenChanged;

        if ( TVServer< IVDirtyObject >::IsClosed() )
        {
            TVServer< IVDirtyObject >::Run( [ this ]( auto & spcDirty ) {
                m_timeline.RegisterDirty( spcDirty );
            } );
        }
    }
    void _applyExpData( VO< TVConstRef< TOUTPUT > > & usr )
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        _chechData();
#endif
        m_svcExp.RunNew( usr , m_expValue );
    }
    void _updateList()
    {
        m_svcData.Close();
        m_expContainer.clear();

        m_itemProv.UseServer( [ this ]( IVDataProv< T > & dp ) {
            dp.Apply( VD_L2U( [ this ]( VI< TVConstRef< T > > & spcItem ) {
                m_svcData.RunNew( [ this ]( VI< TVSwitcherData< T > >& spc ) {
                    m_svcDataHdlr.Add( spc );
                } , spcItem , m_expContainer );
            } ) );
        } );

        m_expValue.m_Vaild = !m_expContainer.empty();
        if ( !m_expContainer.empty() )
        {
            m_expValue.m_pData = *(&m_expContainer[ 0 ]);
        }

#ifdef VD_NOT_CARE_PERFORMANCE
        _chechData();
#endif
    };

#ifdef VD_NOT_CARE_PERFORMANCE
    void _chechData()
    {
        VASSERT( m_expContainer.size() < 2 );

        std::for_each( m_expContainer.begin() , m_expContainer.end() , []( const T * p ) {
            VASSERT( p != 0 );
        } );
    }
#endif

private:
    VN_ELEMSYS::TVSysImpTimeline                     & m_timeline;
    TVServiceProxyArray< IVTracer >                    m_listeners;
    TVService< IVTracer , VItemTracer >                m_tracers;
    bool                                               m_bLengthChanged;
    TVServiceProxy< IVDataProv< T > >                  m_itemProv;

    TOUTPUT                                            m_expValue;
    std::vector< const T * >                           m_expContainer;
    TVServiceArray< TVSwitcherData< T > >              m_svcData;
    TVServiceProxyArray< TVSwitcherData< T > >         m_svcDataHdlr;
    // 
    TVServiceArray< TVConstRef< TOUTPUT > >            m_svcExp;
    TVServiceArray< IVDataProv< TOUTPUT > , ProvAdp  > m_svcProv;
};

template< typename T >
class TVSvcSwitcher
{
public:
    TVSvcSwitcher(){}
    ~TVSvcSwitcher(){}

public:
    typedef VS01Buffer < T >         TEXP;
    typedef IVConverter< TEXP , T  > interf_type;
    typedef TVSwitcher < T         > imp_type;

public:
    void Close()
    {
        m_svc.Close();
    }
    void Elapse()
    {
        m_timeline.ClearDirty();
    }
    void RunService( VO< interf_type > & pipe )
    {
        m_svc.RunNew( pipe , m_timeline );
    }
    template< typename T >
    void Init( const T & v )
    {
    }

private:
    VN_ELEMSYS::TVSysImpTimeline             m_timeline;
    TVServiceArray< interf_type , imp_type > m_svc;
};

// Collectors 
template< typename T >
class TVCollectorData : public VSConstBuffer< const T * >
{
public :
    TVCollectorData( VI< TVConstRef< T > > & spcValue , std::vector< const T * > & arr )
    {
        m_svcList.Run( [ this , & spcValue ]( VI< TVRef< std::vector< const T * > > > & spcArr ){
            m_clnt.Create( spcValue , spcArr ) ;
        } , arr ) ;
    }  

    ~TVCollectorData()
    {
        m_svcList.Close() ;
    }

private :
    struct VKeeper
    {
    public :
#ifdef VD_NOT_CARE_PERFORMANCE
        VKeeper( const TVConstRef< T > & val , TVRef< std::vector< const T * > > & ctr )
            : m_container( ctr.m_ref ) 
        {
            idxInContainer = m_container.size() ;
            m_container.push_back( & val.m_ref ) ;
        }
        ~VKeeper()
        {
            VASSERT( m_container.size() > idxInContainer ) ;
            m_container[ idxInContainer ] = 0 ;
        }
        unsigned idxInContainer ;
        std::vector< const T * > & m_container ;
#else
        VKeeper( const TVConstRef< T > & val , TVRef< std::vector< const T * > > & ctr ) 
        { 
            ctr.m_ref.push_back( & val.m_ref ) ;
        }
        ~VKeeper()
        {
        } 
#endif
    } ;

private :
    TVClient< VKeeper , TVConstRef< T > , TVRef< std::vector< const T * > > > m_clnt ;
    TVService< TVRef< std::vector< const T * > > > m_svcList ;
} ;

template< typename T >
class TVCollector : public IVConverter< VSConstBuffer< const T * > , T > 
                  , IVSlot< VSConstBuffer< const T * > >
                  , TVServer< IVDirtyObject >  
{
public :
    typedef VSConstBuffer< const T * > TOUTPUT ;

    TVCollector( VN_ELEMSYS::TVSysImpTimeline & tl )
        :m_timeline(tl)
    {
        _notifyChange( true ) ;
    }
    ~TVCollector()
    {
        m_tracers.Close() ;
        m_svcData.Close() ;
        m_svcExp .Close() ;
        m_svcProv.Close() ; 
        TVServer< IVDirtyObject > ::Close() ;  
    }
private :
    virtual void Input ( IVSlot< T > & inVal )
    {
        m_tracers.RunNew( [ &inVal ]( auto & spc ){ inVal.Trace( spc ) ; } , *this ) ;
        inVal.GetData( VD_L2U( [ this ]( VI< IVDataProv< T > > & spc ){
            m_itemProvList.Add( spc ) ;
        } ) ) ;
        _notifyChange( true ) ;
    }
    virtual void Output( IVInputPort< TOUTPUT > & outVal )
    {
        outVal.Input( * static_cast< IVSlot< TOUTPUT > * >( this ) ) ;
    }

    // IVSlot< VSConstBuffer< const T * > >
    virtual void Trace   ( VI< IVTracer > & spc ) 
    {
        m_listeners.Add( spc ) ;
    }
    virtual void GetData ( VO< IVDataProv< TOUTPUT > > & usr ) 
    {
        m_svcProv.RunNew( usr , *this ) ;
    } 
      
    // IVDirtyObject
    virtual void CleanAndDiffuse()
    { 
        if( m_bLengthChanged ) // 数组长度发生变更   
            _updateList() ;  

        m_listeners.TidyTravel( [](auto&t){t.OnChanged() ; } ) ;
    }

private :
    typedef TVCollector< T > my_type;
    struct VItemTracer : public IVTracer
    { 
        my_type & me ;
        VItemTracer( my_type & m ) :me( m ){}
        ~VItemTracer(){ me._notifyChange( true )  ; } 
        virtual void OnChanged() 
        {
            me._notifyChange( false ) ;
        }
    };
    struct ProvAdp : public IVDataProv< TOUTPUT >
    {
        my_type & me;
        ProvAdp( my_type & m ) :me( m ){}
        ~ProvAdp(){}
        void Apply( VO< TVConstRef< TOUTPUT > > & usr )
        {
            me._applyExpData( usr );
        }
    };

private :
    void _notifyChange( bool bLenChanged )
    {
        m_bLengthChanged |= bLenChanged ;

        if( TVServer< IVDirtyObject >::IsClosed() )
        {
            TVServer< IVDirtyObject >::Run( [ this ]( auto & spcDirty ){
                m_timeline.RegisterDirty( spcDirty );
            } );
        }
    }  
    void _applyExpData( VO< TVConstRef< TOUTPUT > > & usr )
    { 
#ifdef VD_NOT_CARE_PERFORMANCE
        _chechData() ;
#endif
        m_svcExp.RunNew( usr , m_expValue );
    }
    void _updateList()
    {
        m_svcData.Close() ; 
        m_expContainer.clear() ;

        m_itemProvList.TidyTravel( [ this ]( IVDataProv< T > & dp ){
            dp.Apply( VD_L2U( [ this ]( VI< TVConstRef< T > > & spcItem ){
                m_svcData.RunNew( [ this ]( VI< TVCollectorData< T > >& spc ){
                    m_svcDataHdlr.Add( spc );
                } , spcItem , m_expContainer );
            } ) ) ;
        } ) ;

        m_expValue = VD_V2CB( m_expContainer ) ;
#ifdef VD_NOT_CARE_PERFORMANCE
        _chechData() ;
#endif
    } ;

#ifdef VD_NOT_CARE_PERFORMANCE
    void _chechData()
    {
        std::for_each( m_expContainer.begin() , m_expContainer.end() , []( const T * p ){
            VASSERT( p != 0 ) ;
        } ) ;
    }
#endif

private :
    VN_ELEMSYS::TVSysImpTimeline                     & m_timeline        ;
    TVServiceProxyArray< IVTracer >                    m_listeners       ; 
    TVServiceArray< IVTracer , VItemTracer >           m_tracers         ;
    bool                                               m_bLengthChanged  ;
    TVServiceProxyArray< IVDataProv< T > >             m_itemProvList    ;

    TOUTPUT                                            m_expValue        ;
    std::vector< const T * >                           m_expContainer    ;
    TVServiceArray< TVCollectorData< T > >             m_svcData         ;
    TVServiceProxyArray< TVCollectorData< T > >        m_svcDataHdlr     ;
    // 
    TVServiceArray< TVConstRef< TOUTPUT > >            m_svcExp          ; 
    TVServiceArray< IVDataProv< TOUTPUT > , ProvAdp  > m_svcProv         ;  
} ;

template< typename T >
class TVSvcCollector
{
public :
    TVSvcCollector()
    {}
    ~TVSvcCollector()
    {}

public :
    typedef VSConstBuffer< const T * > TEXP        ;
    typedef IVConverter< TEXP , T    > interf_type ;
    typedef TVCollector< T           > imp_type    ;

public : 
    void Close()
    {
        m_svc.Close() ;
    }
    void Elapse()
    {
        m_timeline.ClearDirty() ;
    } 
    void RunService( VO< interf_type > & pipe )
    {
        m_svc.RunNew( pipe , m_timeline ) ;
    } 
    template< typename T >
    void Init( const T & ) 
    {
    }
private :
    VN_ELEMSYS::TVSysImpTimeline             m_timeline ;
    TVServiceArray< interf_type , imp_type > m_svc      ;
} ;

// Duplicate
template< typename TImp , typename TEXP , typename ... TINPUT >  class TVDuplicateEntry ;

template< typename TImp , typename TEXP >  
class TVDuplicateEntry< TImp , TEXP >
{
public :
    TVDuplicateEntry( ) 
    {
    }
    ~TVDuplicateEntry()
    {
    }
    void Output( typename TVOutputPortParam< TEXP >::type & inPort )
    {
        m_imp.Output( inPort ) ;
    }

    template< unsigned N , typename ... TARGs >
    void Update( const TVInputChngFlag< N > & chngFlag , const TVDynVar< TARGs > & ... args )
    {  
        _call( &TImp::Update , m_imp , args ... ) ; 
    }

    void AttachData()
    {
    } 

private :
    template< typename ... TIAs , typename ... TARGs >
    void _call( void ( TImp::* f )( const TIAs & ... args ) , TImp & cnvtr , const TVDynVar< TARGs > & ... args )
    {
        (cnvtr.*f)( args ... ) ; 
    }
    //template< unsigned N , typename ... TARGs >
    //void _call( void ( TImp::* f )( const TVInputChngFlag< N > & , const TARGs & ... args ) , TImp & cnvtr , const TARGs & ... args )
    //{
    //    (cnvtr.*f)( chngFlag , args ... ) ; 
    //} 

private :
    TImp  m_imp ;
};

template< typename TImp , typename TEXP , typename TI , typename ... TIOthers >
class TVDuplicateEntry< TImp , TEXP ,  TI , TIOthers ... > : public TVDuplicateEntry< TImp , TEXP , TIOthers ... >   
{
private :
    typedef TVDuplicateEntry< TImp , TEXP , TIOthers ... > sub_type ;
    TVServiceProxy< IVDataProv< TI > >                     m_proxyProv ;
    TVServiceProxy< TVConstRef< TI > >                     m_proxyRef  ;   

public :
    TVDuplicateEntry()
    {
    }
    ~TVDuplicateEntry()
    {
    }

public :
    void AttachData( IVSlot< TI > & s , IVSlot< TIOthers > & ... sOthers )
    { 
        s.GetData( VD_P2U( m_proxyProv ) );
        sub_type::AttachData( sOthers ... ) ;
    }

    template< unsigned N , typename ... TARGs >
    void Update( const TVInputChngFlag< N > & chngFlag , const TVDynVar< TARGs > & ... args )
    {

        if( m_proxyRef.IsClosed() )
        {
            m_proxyProv.UseServer( [ this ]( IVDataProv< TI > & mc ){
                mc.Apply( VD_P2U( m_proxyRef ) );
            } );
        }

        m_proxyRef.UseServer( [ this , & chngFlag ,  & args ...]( const TVConstRef< TI > & taRef ){
            const unsigned IDX = N - ( sizeof ... ( TIOthers ) ) - 1 ;
            TVDynVar< TI > var( taRef.m_ref , chngFlag.template Check< IDX >() ) ;
            sub_type::Update( chngFlag , args ... , var ) ;
        } ) ;
    }
} ;

template< typename TImp , typename TEXP , typename ... TINPUT >
class TVDuplicate : public IVConverter< TEXP , TINPUT ... >
                  , public TVDuplicateEntry< TImp , TEXP , TINPUT ... >  
                  , public TVServer< IVDirtyObject >
{
public :
    typedef TVDuplicateEntry< TImp , TEXP , TINPUT ... > entry_type ;
    typedef TVDuplicate< TImp , TEXP , TINPUT ... > my_type ;
    typedef void ( my_type::* call_chng )() ;

    TVDuplicate( VN_ELEMSYS::TVSysImpTimeline & tl )
        :m_timeLine( tl )
    {
    }
    ~TVDuplicate()
    {
        TVServer< IVDirtyObject >::Close() ;
        for( unsigned i = 0 ; i < ARG_COUNT ; i ++ )
        {
            m_tracer[i].Close() ;
        }
    }

public :  
    virtual void Input ( IVSlot< TINPUT > & ... slot )
    {
        _traceSlot( slot ... ) ;
        entry_type::AttachData( slot ... );
    }
    virtual void Output( typename TVOutputPortParam< TEXP >::type & inPort )
    {
        entry_type::Output( inPort ) ;
    }
    virtual void CleanAndDiffuse()
    { 
        entry_type::Update( m_chngFlag ) ;
        m_chngFlag.Reset() ;
    }  
    
    template< unsigned >
    struct VPtr
    {
        my_type & me ;
        VPtr( my_type & m ):me(m){}
    } ;

    struct VTracer : IVTracer 
    {
        my_type & me ;
        call_chng fcall ; 
        template< unsigned IDX >
        VTracer( VPtr< IDX > & ptr ):me(ptr.me),fcall( &my_type::_notifyChange<IDX> )
        {
        } 
        void OnChanged()
        {
            ( me.*fcall )( );
        }
    } ;

private : 
    template< unsigned IDX >
    void _notifyChange()
    {
        m_chngFlag.template Signal<IDX>() ;
        _makeDirty() ;
    }

    void _makeDirty()
    { 
        if( TVServer< IVDirtyObject >::IsClosed() )
            TVServer< IVDirtyObject >::Run( [ this ]( auto &spc ){ m_timeLine.RegisterDirty( spc ) ; } ) ;
    } 

    void _traceSlot( ) 
    { 
    }
    template< typename TI , typename ... TNxts >
    void _traceSlot( IVSlot< TI > & si , IVSlot< TNxts > & ... slotOthers )
    { 
        const unsigned idx = ARG_COUNT - ( sizeof ... ( TNxts ) ) - 1 ;

        m_tracer[idx].Run( [ &si ]( VI< IVTracer > & spc ){
            si.Trace( spc ) ;
        } , VPtr<idx>( *this ) ) ;
    }

private :    
    VN_ELEMSYS::TVSysImpTimeline & m_timeLine ;
    static const unsigned ARG_COUNT = sizeof ... ( TINPUT ) ;
    TVService< IVTracer , VTracer > m_tracer[ ARG_COUNT ] ;
    TVInputChngFlag< ARG_COUNT    > m_chngFlag            ;
} ;

template< typename TImp , typename TEXP , typename ... TINPUT  >
class TVSvcDuplicate
{
public :
    TVSvcDuplicate()
    {}
    ~TVSvcDuplicate()
    {}

public : 
    typedef IVConverter< TEXP , TINPUT ...        > interf_type ;
    typedef TVDuplicate< TImp , TEXP , TINPUT ... > imp_type    ;

public : 
    void Close()
    {
        m_svc.Close() ;
    }
    void Elapse()
    {
        m_timeline.ClearDirty() ;
    } 
    void RunService( VO< interf_type > & pipe )
    {
        m_svc.RunNew( pipe , m_timeline ) ;
    } 
    template< typename T >
    void Init( const T & v )
    {
    }

private :
    VN_ELEMSYS::TVSysImpTimeline             m_timeline ;
    TVServiceArray< interf_type , imp_type > m_svc      ; 
} ;

////////////////////////////////////////////////////////////
template< typename TTARG > struct TVInputTrait
{
    static auto GetFunc() { return &TTARG::Input  ; } 
} ;
template< typename TTARG > struct TVOutputTrait
{
    static decltype( &TTARG::Output ) GetFunc() { return &TTARG::Output ; } 
} ;

template< typename TID > 
struct TVOutputTrait< TVHub< IVRLTN< TID > > >
{
    typedef typename IVRLTN< TID >::VALUE OUVALUE ;

    template< typename T = TID >
    static std::enable_if< std::is_void< typename IVRLTN<T>::VALUE >::value , decltype( TVHub< IVRLTN< TID > >::Output ) > 
        GetFunc()
    {
        return TVHub< IVRLTN< TID > >::Output ;
    } 
} ;


template< typename ... TINPUTs > 
class TVLinkerAdptorFilter 
{
public :
    template< typename F , typename TP , typename ... TPOthers >
    static void TravelArgs( F f , IVSlot< TINPUTs > & ... slots , TP & prov , TPOthers & ... otherProvs )
    {
        _travel( f , slots ... , prov , TVOutputTrait< TP >::GetFunc() , otherProvs ... ) ;
    }   

    template< typename F , typename TD , typename ... TPOthers >
    static void TravelArgs( F f , IVSlot< TINPUTs > & ... slots , IVSlot< TD > & td , TPOthers & ... otherProvs )
    {
        TVLinkerAdptorFilter< TINPUTs ... , TD >::TravelArgs( f , slots ... , td , otherProvs ... ) ;
    }  

    template< typename F >
    static void TravelArgs( F f , IVSlot< TINPUTs > & ... slots )
    { 
        f( slots ... ) ;
    }  

private :
    template< typename F , typename TP , typename TC , typename ... TARGs , typename ... TPOthers >
    static void _travel( F fuse , IVSlot< TINPUTs > & ... slots , TP & prov , void ( TC::*fout )( IVInputPort< TARGs ... > & ) , TPOthers & ... otherProvs )
    {
        static_assert( std::is_base_of< TC , TP >::value , "" ) ;

        auto fnxt =  [ fuse  , & slots ... , & otherProvs ... ]( IVSlot< TARGs > & ... ins ){
            TVLinkerAdptorFilter< TINPUTs ... , TARGs ... >::TravelArgs( fuse , slots ... , ins ... , otherProvs ... ) ;
        } ;  

        ( prov .*fout)( VD_L2IP( fnxt ) ) ; 
    }
} ; 

template< typename TTARG > 
class TVLinkerAdptor
{
public :
    template< typename ... TINPUTs , typename TC , typename ... TIs >
    static void AttachInput( TTARG & cvtr , void ( TC::*f )( IVSlot< TINPUTs > & ... ) , TIs & ... args )
    {
        auto fadp = [ & cvtr , f ]( IVSlot< TINPUTs > & ... ins ){
            (cvtr.*f)( ins ... ) ;
        } ;

        TVLinkerAdptorFilter<>::TravelArgs( fadp , args ... ) ; 
    }  
} ;

template< typename TCONV , typename ... TIs >
void VLNK( TCONV & cvtr ,  TIs & ... args )
{
    TVLinkerAdptor< TCONV >::AttachInput( cvtr , TVInputTrait< TCONV >::GetFunc() , args ... ) ;
} ;

template< typename TOUTPUT , typename ... TINPUTs >
class TVConverterWrapper : public VO< IVConverter< TOUTPUT , TINPUTs ... > > 
                         , public IVConverter< TOUTPUT , TINPUTs ... >
{
public :
    TVConverterWrapper() 
    {}
    ~TVConverterWrapper() 
    {}
public :
    typedef IVConverter< TOUTPUT , TINPUTs ... > interf_type ;  
     
    virtual void Input( IVSlot< TINPUTs > & ... args )
    {
        m_proxy.UseServer(  [ &args ... ]( auto & ds ){
            ds.Input( args ... ) ; 
        } ) ; 
    }

    virtual void Output( typename TVOutputPortParam< TOUTPUT >::type & inp ) 
    {
        m_proxy.UseServer( [ & inp ]( auto & ds ){ ds.Output( inp ) ; } ) ;
    }

private :
    void Visit( VI< interf_type >  & spc )
    {
        m_proxy.Create( spc ) ;
    }

private :
    TVServiceProxy< interf_type > m_proxy ;
} ;

////////////////////////////////////////////////////////////////////////////////////////////////

// System of dynamic relation implements

////////////////////////////////////////////////////////////////////////////////////////////////

template< typename TSysTrait , typename TR > class TVRHubImp ;
 
////////////////////////////////////////////////////////////////////////////////////////////////

// (1) Declaration of TVRHubImp 

////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ... TARGs > class TVRTupleData ;

template<>
class TVRTupleData<>
{
public :
    void Input() {}
    template< typename F , typename ... TARGs >
    void Output( F f , IVSlot< TARGs > & ... slts )
    {
        f( slts ... ) ;
    }

    template< typename F >
    void Output( F f )
    {
    }
} ;

template< typename T , typename ... TOTHERs >
class TVRTupleData< T , TOTHERs ... >
{
public :
    TVRTupleData(){}

public :
    void Input( IVSlot< T > & s , IVSlot< TOTHERs > & ... slts )
    {
        m_data.Input( s ) ;
        m_sub.Input( slts ... ) ;
    }
    
    template< typename F , typename ... TARGs >
    void Output( F f , IVSlot< TARGs > & ... slts )
    {
        m_sub.Output( f , slts ... , static_cast< IVSlot< T > & >( m_data ) ) ;
    } 

private :
    typedef TVRTupleData< TOTHERs ... > sub_type ;
    sub_type                            m_sub    ;
    TVData< T >                         m_data   ;
} ;

template< typename T , T > struct TVR_ComData_Args_From_Func ;
template< typename TIMP , typename ... TOUs , void ( TIMP::*mf )( IVInputPort< TOUs ... > & ) >
struct TVR_ComData_Args_From_Func< void ( TIMP::* )( IVInputPort< TOUs ... > & ) , mf >
{
    typedef TVRTupleData< TOUs ... > arg_type ;

    static void Output( TIMP & inst , arg_type & tpl )
    {
        (inst.*mf)( VD_L2IP( [ &tpl ]( IVSlot< TOUs > & ... slts ){
            tpl.Input( slts ... ) ; // TVR_ComData_Args_Accessor< TOUs ... >::Ouput( tpl , slts ... ) ;
        } ) ) ;
    }

    class ComAdp
    {
    public :
        ComAdp( TVRTupleData< TOUs ... > & td )
            : m_data( td )
        {
        }

    public : 
        void Output( IVInputPort< TOUs ... > & inp )
        { 
            m_data.Output( [ &inp ]( IVSlot< TOUs > & ... sins ){
                inp.Input( sins ... ) ;
            } ) ;
        }

    private :
        TVRTupleData< TOUs ... > & m_data ;
    } ; 
} ;

template< typename TIMP , typename ... TINs , void ( TIMP::*mf )( IVSlot< TINs > & ... ) >
struct TVR_ComData_Args_From_Func< void ( TIMP::* )( IVSlot< TINs > & ... ) , mf >
{
    typedef TVRTupleData< TINs ... > arg_type ;
    static void Input( TIMP & inst , arg_type & tpl )
    {
        tpl.Output( [ & inst ]( IVSlot< TINs > & ... slts ){
            ( inst.*mf )( slts ... ) ;
        } ) ; 
    }

    class ComAdp
    {
    public:
        ComAdp( TVRTupleData< TINs ... > & td )
            : m_data( td )
        {
        }

    public:
        void Input( IVSlot< TINs > & ... slots )
        { 
            m_data.Input( slots ... ) ;
        }

    private:
        TVRTupleData< TINs ... > & m_data;
    }; 
} ;
//
//
//template< typename TIMP , typename ... TOUs , typename ... TINs , void ( TIMP::*mf )( IVInputPort< TOUs ... > & , IVSlot< TINs > & ...  ) >
//struct TVR_ComData_Args_From_Func< void ( TIMP::* )( IVInputPort< TOUs ... > & , IVSlot< TINs > & ... ) , mf >
//{ 
//    typedef std::pair< TVRTupleData< TOUs ... > , TVRTupleData< TINs ... > > arg_type ; 
//
//    static void Output( TIMP & inst , arg_type & tpl )
//    {
//        TVRTupleData< TINs ... > & tdIn = tpl.second ;
//        TVRTupleData< TOUs ... > & tdOu = tpl.first  ;
//
//        tdIn.Output( [ &inst , & tdOu ]( IVSlot< TINs > & ... sins ){
//            ( inst.*mf )( VD_L2IP( [ &tdOu ]( IVSlot< TOUs > & ... souts ){
//                tdOu.Input( souts ... );
//            } ) , sins ... ) ;
//        } ) ;
//    }
//
//    class ComAdp
//    {
//    public:
//        ComAdp( arg_type & td )
//            : m_data( td )
//        {
//        }
//        void Input( IVSlot< TINs > & ... slots )
//        {
//            TVRTupleData< TINs ... > & tdIn = m_data.first  ;
//            tdIn.Input( slots ... )
//        }
//        void Output( IVInputPort< TOUs ... > & inp )
//        {
//            TVRTupleData< TOUs ... > & tdOu = m_data.first  ;
//            tdOu.Output( [ &inp ]( IVSlot< TOUs > & ... sins ){
//                inp.Input( sins ... ) ;
//            } ) ;
//        }
//
//    private:
//        arg_type & m_data;
//    } ;
//};
 
template <typename T>
class TVR_HasOutput
{
    typedef char one;
    typedef long two;

    template <typename C> static one tesetoutput( decltype(&C::Output) ) ;
    template <typename C> static two tesetoutput(...);    

public: 
    static const bool value = ( sizeof( tesetoutput< T >( 0 ) ) == sizeof( char ) );
}; 

template< typename T , bool bHasInput >
class TVOutputableComData
{
public :
    typedef TVR_ComData_Args_From_Func< decltype( &T::Output ) , &T::Output > parser_type ;
    typedef typename parser_type::arg_type                                    arg_type    ;
    arg_type m_args ;

public :
    void Output( T & inst  )
    { 
        parser_type::Output( inst , m_args ) ;
    }
    arg_type & GetArgs( )
    {
        return m_args ;
    } 
} ;

template< typename T >
class TVOutputableComData< T , false >
{
public :
    void Output( T & inst  )
    { 
    }
} ;
  
template <typename T>
class TVR_HasInput
{
    typedef char one;
    typedef long two;

    template <typename C> static one tesetinput( decltype(&C::Input) ) ;
    template <typename C> static two tesetinput(...);    

public: 
    static const bool value = ( sizeof( tesetinput< T >( 0 ) ) == sizeof( char ) );
}; 

template< typename T , bool bHasInput >
class TVInputableComData
{
public :
    typedef TVR_ComData_Args_From_Func< decltype( &T::Input ) , &T::Input > parser_type ;
    typedef typename parser_type::arg_type                                  arg_type    ;
    arg_type m_args ;

public :
    void Input( T & inst  )
    { 
        parser_type::Input( inst , m_args ) ;
    }
    arg_type & GetArgs( )
    {
        return m_args ;
    } 
} ;

template< typename T >
class TVInputableComData< T , false >
{
public :
    void Input( T & inst  )
    { 
    }
} ;
 
template< typename T , bool bHasOutput >
class TVCompBase : public TVR_ComData_Args_From_Func< decltype( &T::Output ) , &T::Output >::ComAdp
{
public :
    TVCompBase( TVOutputableComData< T , true > & co )
        : TVR_ComData_Args_From_Func< decltype( &T::Output ) , &T::Output >::ComAdp( co.GetArgs() )
    {
    }
} ;

template< typename T >
class TVCompBase< T , false >
{
public :
    TVCompBase( TVOutputableComData< T , false > & co ) 
    {
    }
} ;
 
template< typename T , bool bHasInput >
class TVCompBaseRev : public TVR_ComData_Args_From_Func< decltype( &T::Input ) , &T::Input >::ComAdp
{
public :
    TVCompBaseRev( TVInputableComData< T , true > & co )
        : TVR_ComData_Args_From_Func< decltype( &T::Input ) , &T::Input >::ComAdp( co.GetArgs() )
    {
    }
} ;

template< typename T >
class TVCompBaseRev< T , false >
{
public :
    TVCompBaseRev( TVInputableComData< T , false > & co ) 
    {
    }
} ;

template< typename T > 
class TVComp : public TVCompBase< T , TVR_HasOutput< T >::value >
             , public TVCompBaseRev< T , TVR_HasInput< T >::value >
{
public :
    typedef TVOutputableComData    < T , TVR_HasOutput< T >::value > ComDataOuType ; 
    typedef TVInputableComData < T , TVR_HasInput< T >::value  > ComDataInType ; 

    typedef TVCompBase< T , TVR_HasOutput< T >::value   > base_type     ;
    typedef TVCompBaseRev< T , TVR_HasInput< T >::value > base_rev_type ;

    TVComp( ComDataOuType & dt , ComDataInType & rev )
        : base_type( dt ) , base_rev_type( rev )
    {
    }   
} ;

template< typename T > 
using TVAggr = TVCompBase< T , TVR_HasOutput< T >::value > ;

////////////////////////////////////////////////////////////////////////////////////////////////

// (2) Hub linkage implementation

////////////////////////////////////////////////////////////////////////////////////////////////
template< typename TSysTrait , typename TR >
struct TVRHubNodeTrait
{
    typedef typename TSysTrait::template TRTrait< TR >::mngr_type  MNGR;
};

template< typename TSysTrait , typename T > class TVRHubLinkageFull ; 
template< typename TSysTrait , typename TID > 
class TVRHubLinkageFull< TSysTrait , IVRLTN< TID > >   
{
public : 
    typedef TVRHubImp< TSysTrait , IVRLTN< TID > >  hub_inst_type ;
    typedef typename hub_inst_type::MNGR             manager_type  ; 
    typedef TVOutputableComData   < manager_type , TVR_HasOutput< manager_type >::value > data_ou_type ;
    typedef TVInputableComData< manager_type , TVR_HasInput< manager_type >::value  > data_in_type ;

    void AttachProvider( IVSlot< IVRLTN< TID > > & si )
    { 
        hub_inst_type::Slot2Manager( si , [ this ]( manager_type & inst ){ 
            AttachManager( inst ) ;
        } ) ;
    } 

    // Attach Outter value 
    void AttachManager( manager_type & inst )
    {
        m_ou.Output( inst ) ; 
        m_in.Input( inst ) ;
    }

    // Provide Data for using by owner
    template< typename F >
    void UseAsComponent( F f )
    {
        auto Tmt =TVComp< manager_type >( m_ou , m_in );
        f( Tmt) ;
    } 

private :
    data_ou_type  m_ou ;
    data_in_type  m_in ;
 } ;

// ReadOnlyLinkage 
template< typename TSysTrait , typename T > 
class TVRHubLinkageReadOnly
{ 
public :
    TVRHubLinkageReadOnly(){}
    ~TVRHubLinkageReadOnly(){}
    void AttachProvider( IVSlot< T > & si )
    {
        m_data.Input( si ) ;
    }  
    template< typename F >
    void UseAsAggragation( F f )
    { 
        f( static_cast< IVSlot< T > & > ( m_data ) ) ;
    }  

    // Evander added -- 20161109
    void DetachProvider()
    {
        m_data.Close();
    }
    //////////////////////////////////////////////////////////////////////////

    TVData< T > m_data ;
} ;

template< typename TSysTrait , typename TID > 
class TVRHubLinkageReadOnly< TSysTrait , IVRLTN< TID > >   
{
public : 
    typedef TVRHubImp< TSysTrait , IVRLTN< TID > >  hub_inst_type ;
    typedef typename hub_inst_type::MNGR             manager_type  ; 
    typedef TVOutputableComData< manager_type , TVR_HasOutput< manager_type >::value > data_type ;

    void AttachProvider( IVSlot< IVRLTN< TID > > & si )
    { 
        hub_inst_type::Slot2Manager( si , [ this ]( manager_type & inst ){ 
            AttachManager( inst ) ;
        } ) ;
    } 

    void AttachManager( manager_type & inst )
    {
        m_data.Output( inst );
    }
    
    template< typename F >
    void UseAsAggragation( F f )
    {
        auto data=TVAggr< manager_type >( m_data ) ;
        f( data) ;
    }

    // Evander added -- 20161109
    void DetachProvider()
    {
        VASSERT( 0 && "需要实现DetachProvider" );
    }
    //////////////////////////////////////////////////////////////////////////

private :
    data_type m_data ;
 } ;
////////////////////////////////////////////////////////////////////////////////////////////////

// (3) Hub implementation

////////////////////////////////////////////////////////////////////////////////////////////////

// root 
template< typename TSysTrait , typename TINTERF >
class TVRHubImpRoot : public TINTERF
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    TVRHubImpRoot( TPIPELINE & pl )
        : m_pipeline( pl )
    {
    }
    
    TPIPELINE &  GetPipeLine()
    {
        return m_pipeline ;
    }
    
    void Close()
    {  
    }

private :
    TPIPELINE &   m_pipeline ;
}  ;

template< typename TSysTrait , typename TVALUE , typename TBASE >
class TVRHubImpRootBase : public TBASE
{
public :
    using TBASE::TBASE ;
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {
        f( args ... , static_cast< IVInputPort< TVALUE > & >( m_expData ) ) ;
    }
    void Output( IVInputPort< TVALUE > & inp )
    {
        inp.Input( m_expData ) ;
    }
    
    void Close()
    {  
    }
private :
    TVData< TVALUE > m_expData ;
} ;

template< typename TSysTrait , typename TEXPID , typename TBASE >
class TVRHubImpRootBase< TSysTrait , IVRLTN< TEXPID > , TBASE  > : public TBASE
{
public :
    using TBASE::TBASE ;

    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {     
        m_linkage.UseAsComponent( [ this , f , & args ... ]( auto & v ){
            f( args ... ,  v );
        } ) ; 
    }
    virtual void Create( VO< typename IVRLTN< TEXPID >::HUB > & sys )
    {
        auto fuse = [ this ]( auto & mngr ){
            m_linkage.AttachManager( mngr ) ;
        } ;

        m_svc.Run( sys , this->GetPipeLine() , fuse ) ;
    }
    void Close()
    {  
        m_svc.Close() ;
        TBASE::Close() ;
    }

private :   
    typedef typename IVRLTN< TEXPID >::HUB                interf_type  ;  
    typedef TVRHubImp< TSysTrait , IVRLTN< TEXPID >  >   svr_type     ; 
    typedef TVRHubLinkageFull< TSysTrait , IVRLTN< TEXPID > > linkage_type ;
    TVService< interf_type , svr_type > m_svc ;
    linkage_type                        m_linkage ;
} ;

template< typename TSysTrait , typename TBASE >
class TVRHubImpRootBase< TSysTrait , void , TBASE > : public TBASE
{
public :
    using TBASE::TBASE ;
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {
        f( args ... ) ;
    }
    void Close()
    {  
    }
} ;

template< typename TSysTrait , typename TBASE >
class TVRHubImpRootBase< TSysTrait , VI_AND<> , TBASE  > : public TBASE 
{
public :
    using TBASE::TBASE ;
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {
        f( args ... ) ;
    }
    void Close()
    {  
    }
} ;

template< typename TSysTrait , typename TEXPID , typename ... TOTHEREXPs , typename TBASE >
class TVRHubImpRootBase< TSysTrait , VI_AND< IVRLTN< TEXPID > , TOTHEREXPs ... > , TBASE  >
    : public TVRHubImpRootBase< TSysTrait , VI_AND< TOTHEREXPs ... > , TBASE  >
{
public :
    typedef TVRHubImpRootBase< TSysTrait , VI_AND< TOTHEREXPs ... > , TBASE  > sub_base_type ;
    using sub_base_type::sub_base_type ;

    ~TVRHubImpRootBase()
    { 
    }

public : 
    void Close()
    {  
        m_svc.Close() ;
        sub_base_type::Close() ;
    }

    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {       
        m_linkage.UseAsComponent( [ this , f , & args ... ]( auto & v ){
            sub_base_type::AccessLinkage( f , args ... , v ) ; 
        } ) ; 
    }
    virtual void Create( VO< typename IVRLTN< TEXPID >::HUB > & sys )
    {
        auto fuse = [ this ]( auto & mngr ){
            m_linkage.AttachManager( mngr ) ;
        } ;

        m_svc.Run( sys , this->GetPipeLine() , fuse ) ;
    }

private :   
    typedef typename IVRLTN< TEXPID >::HUB                interf_type  ;  
    typedef TVRHubImp< TSysTrait , IVRLTN< TEXPID >  >   svr_type     ; 
    typedef TVRHubLinkageFull< TSysTrait , IVRLTN< TEXPID > > linkage_type ;
    TVService< interf_type , svr_type > m_svc     ;
    linkage_type                        m_linkage ;
} ;

template< typename TSysTrait , typename TEXP , typename ... TOTHEREXPs , typename TBASE >
class TVRHubImpRootBase< TSysTrait , VI_AND< TEXP , TOTHEREXPs ... > , TBASE  >
    : public TVRHubImpRootBase< TSysTrait , VI_AND< TOTHEREXPs ... > , TBASE  >
{
public :
    typedef TVRHubImpRootBase< TSysTrait , VI_AND< TOTHEREXPs ... > , TBASE  > sub_base_type ;
    using sub_base_type::sub_base_type ;

    ~TVRHubImpRootBase()
    {
    }

public : 
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    { 
       sub_base_type::AccessLinkage( f ,  args ...  , static_cast< IVInputPort< TEXP > & >( m_expData )  ) ;
    }
    void Output( IVInputPort< TEXP > & inp )
    {
        inp.Input( m_expData ) ;
    }
    
    void Close()
    { 
        sub_base_type::Close() ;
    }
private :
    TVData< TEXP > m_expData ;
} ;

template< typename TSysTrait , typename TID , typename TVALUE , typename ... TINPUTs >
using TVRHubImpExp = TVRHubImpRootBase< TSysTrait , TVALUE , TVRHubImpRoot< TSysTrait , IVRUniqHub< TID , TVALUE , TINPUTs ... > > > ;

// And  
template< typename TSysTrait , typename TBASE , typename TVALUE , typename ... TINPUTs > class TVRHubImpBase_And ;
template< typename TSysTrait , typename TBASE , typename TVALUE >
class TVRHubImpBase_And< TSysTrait , TBASE , TVALUE > : public TBASE
{
public :
    using TBASE::TBASE ;

    ~TVRHubImpBase_And()
    {
    }

    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {  
        TBASE::AccessLinkage( f , args ... ) ;
    }
    
    void Close()
    { 
        TBASE::Close() ;
    }
} ;

template< typename TSysTrait , typename TBASE , typename TVALUE , typename TI , typename ... TOTHERs >
class TVRHubImpBase_And< TSysTrait , TBASE , TVALUE , TI , TOTHERs ... > : public TVRHubImpBase_And< TSysTrait , TBASE , TVALUE , TOTHERs ... >
{  
public :
    typedef TVRHubImpBase_And< TSysTrait , TBASE , TVALUE , TOTHERs ... > BASE_TYPE ;
    using TVRHubImpBase_And< TSysTrait , TBASE , TVALUE , TOTHERs ... >::TVRHubImpBase_And ;

    ~TVRHubImpBase_And()
    {
    }

private :
    typedef TVRHubImpBase_And< TSysTrait , TBASE , TVALUE , TOTHERs ... > sub_type ;  
    typedef TVRHubLinkageReadOnly< TSysTrait , TI > linkage_type ;

    virtual void Connect( IVSlot< TI > & si )
    {
        m_linkage.AttachProvider( si ) ;
    }

public : 
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {  
        m_linkage.UseAsAggragation( [ this , f , & args ... ]( auto & v ){
            sub_type::AccessLinkage( f , args ... , v ) ; 
        } ) ; 
    }
    
    void Close()
    { 
        BASE_TYPE::Close() ;
    }

private : 
    linkage_type m_linkage ;
} ; 

template< typename TSysTrait , typename TID , typename TVALUE , typename TNAME > class TVRHubImpAdp ; 
template< typename TSysTrait , typename TID , typename TVALUE , typename ... TINPUTs > 
class TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_AND< TINPUTs ... > > 
    : public TVRHubImpBase_And< TSysTrait , TVRHubImpExp< TSysTrait , TID , TVALUE , TINPUTs ... > , TVALUE , TINPUTs ... >
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef TVRHubImpBase_And< TSysTrait , TVRHubImpExp< TSysTrait , TID , TVALUE , TINPUTs ... > , TVALUE , TINPUTs ... > BASE_TYPE ;
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR  MNGR ;

public :
    TVRHubImpAdp( TPIPELINE & pl )
        : BASE_TYPE( pl )
    { 
        BASE_TYPE::AccessLinkage( [ this , & pl ]( auto & ... vin ){
            m_node.Renew( pl , vin ... ) ;
        } ) ;
    }
    ~TVRHubImpAdp()
    {
    }
 
    template< typename F >
    void AccessManager( F f )
    {
        m_node.Use( f ) ;
    } ;
    
    void Close()
    {
        m_node.Clear() ; 
        BASE_TYPE::Close() ;
    }

private :
    TVSafeScopedObj< MNGR > m_node ;
} ;

// Or Evander added -- 20161109
template< typename TSysTrait , typename TDERIVE , typename TBASE , typename TVALUE , typename ... TINPUTs > class TVRHubImpBase_Or ;
template< typename TSysTrait , typename TDERIVE , typename TBASE , typename TVALUE >
class TVRHubImpBase_Or< TSysTrait , TDERIVE , TBASE , TVALUE > : public TBASE
{
public:
    using TBASE::TBASE;

    ~TVRHubImpBase_Or(){}

    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {
        VASSERT( sizeof...( ARGs ) == 1 && "Error." );
    }

    template< typename F >
    void AccessLinkage( F f ){}

    template< typename F  , typename ARG >
    void AccessLinkage( F f , ARG & arg )
    {
        TBASE::AccessLinkage( f , arg );
    }

    void Close()
    {
        TBASE::Close();
    }

    void ResetBack(){}
};

template< typename TSysTrait , typename TDERIVE , typename TBASE , typename TVALUE , typename TI , typename ... TOTHERs >
class TVRHubImpBase_Or< TSysTrait , TDERIVE , TBASE , TVALUE , TI , TOTHERs ... > 
    : public TVRHubImpBase_Or< TSysTrait , TVRHubImpBase_Or< TSysTrait , TDERIVE , TBASE , TVALUE , TI , TOTHERs ... > , TBASE , TVALUE , TOTHERs ... >
{
public:
    typedef TVRHubImpBase_Or< TSysTrait , TVRHubImpBase_Or< TSysTrait , TDERIVE , TBASE , TVALUE , TI , TOTHERs ... > , TBASE , TVALUE , TOTHERs ... > BASE_TYPE;
    using BASE_TYPE::BASE_TYPE;

    ~TVRHubImpBase_Or(){}

private:
    typedef BASE_TYPE sub_type;
    typedef TVRHubLinkageReadOnly< TSysTrait , TI > linkage_type;

    virtual void Connect( IVSlot< TI > & si )
    {
        m_bUsing = true;
        m_linkage.AttachProvider( si );
        
        BASE_TYPE::ResetBack();
        static_cast< TDERIVE& >( *this ).ResetFront();
    }

public:
    template< typename F , typename ... ARGs >
    void AccessLinkage( F f , ARGs & ... args )
    {
        if ( m_bUsing )
        {
            m_linkage.UseAsAggragation( [ this , f , &args ... ]( auto & v ){
                sub_type::AccessLinkage( f , args ... , v );
            } );
        }
        else
        {
            sub_type::AccessLinkage( f , args ... );
        }
    }

    void Close()
    {
        BASE_TYPE::Close();
    }

    void ResetFront()
    {
        m_bUsing = false;
        m_linkage.Close();
        static_cast< TDERIVE& >( *this ).ResetFront();
    }

    void ResetBack()
    {
        m_bUsing = false;
        m_linkage.Close();
        BASE_TYPE::ResetBack();
    }

private:
    linkage_type m_linkage;
    bool         m_bUsing = false;
};

template< typename TSysTrait , typename TID , typename TVALUE , typename ... TINPUTs >
class TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_OR< TINPUTs ... > >
    : public TVRHubImpBase_Or< TSysTrait 
                             , TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_OR< TINPUTs ... > > 
                             , TVRHubImpExp< TSysTrait , TID , TVALUE , TINPUTs ... >
                             , TVALUE , TINPUTs ...
                             >
{

public:
    typedef typename TSysTrait::PIPELINE TPIPELINE;
    typedef TVRHubImpBase_Or< TSysTrait , TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_OR< TINPUTs ... > > 
                            , TVRHubImpExp< TSysTrait , TID , TVALUE , TINPUTs ... > , TVALUE , TINPUTs ... > BASE_TYPE;
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR  MNGR;

    TVRHubImpAdp( TPIPELINE & pl )
        : BASE_TYPE( pl )
    {}
    ~TVRHubImpAdp(){}

    template< typename F >
    void AccessManager( F f )
    {
        m_node.Use( f );
    };

    void Close()
    {
        m_node.Clear();
        BASE_TYPE::Close();
    }

    void ResetFront()
    {
        BASE_TYPE::AccessLinkage( [ this ]( auto & ... vin ) {
            m_node.Renew( this->GetPipeLine() , vin ... );
        } );
    }

private:
    TVSafeScopedObj< MNGR > m_node;
};
//////////////////////////////////////////////////////////////////////////

// Arr  
template< typename TSysTrait , typename TID , typename TVALUE , typename TD > 
class TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_ARR< TD > > : public TVRHubImpExp< TSysTrait , TID , TVALUE , TD >
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR MNGR ; 
    typedef  TVRHubImpExp< TSysTrait , TID , TVALUE , TD > BASE_TYPE ;

public :
    TVRHubImpAdp( TPIPELINE & pl ) 
        : BASE_TYPE( pl )
    {
        BASE_TYPE::AccessLinkage( [ this , & pl ]( auto & ... vin ){
            m_node.Renew( pl , vin ... ) ;
        } ) ; 
    }
    ~TVRHubImpAdp()
    {
        m_node.Clear() ;
    }

    //typedef unsigned  MNGR ;
    template< typename F >
    void AccessManager( F f )
    {  
        m_node.Use( f ) ;
    } ; 
    
    void Close()
    {
        m_node.Clear() ; 
        BASE_TYPE::Close() ;
    }

public :  
    void Connect( IVSlot< TD > & si )
    {  
        m_node.Use( [ & si ]( auto & mngr){
            mngr.AddItem( si ) ;
        } ) ;
    }

private :
    TVSafeScopedObj< MNGR > m_node ; 
} ;
 
template< typename TSysTrait , typename TID , typename TVALUE , typename TITEMID > 
class TVRHubImpAdp< TSysTrait , TID , VI_AND< IVRLTN< TITEMID > , TVALUE > , VI_ARR< IVRLTN< TITEMID > > >
    : public TVRHubImpRootBase< TSysTrait , TVALUE , TVRHubImpRoot< TSysTrait , IVRUniqHub< TID , VI_AND< IVRLTN< TITEMID > , TVALUE > > > >
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR MNGR ;
    typedef TVRHubImpRootBase< TSysTrait , TVALUE , TVRHubImpRoot< TSysTrait , IVRUniqHub< TID , VI_AND< IVRLTN< TITEMID > , TVALUE > > > > BASE_TYPE ;

private :
    typedef IVRLTN< TITEMID >                              item_rltn_type     ;
    typedef typename item_rltn_type::HUB                   item_interf_type   ;
    typedef TVRHubImp< TSysTrait , item_rltn_type    >    item_hub_inst_type ;
    typedef typename item_hub_inst_type::MNGR              item_manger_type   ;
    typedef TVRHubLinkageReadOnly< TSysTrait , item_rltn_type >    item_linkage_type  ;

public :
    TVRHubImpAdp( TPIPELINE & pl ) 
        : BASE_TYPE( pl )
    {
        BASE_TYPE::AccessLinkage( [ this , & pl ]( auto & ... vin ){
            m_node.Renew( pl , vin ... ) ;
        } ) ; 
    }
    ~TVRHubImpAdp()
    { 
    }

    void Close()
    {
        m_node.Clear() ;
        m_svc.Close() ;

        BASE_TYPE::Close() ;
    }

    //typedef unsigned  MNGR ;
    template< typename F >
    void AccessManager( F f )
    {  
        m_node.Use( f ) ; 
    } ; 

public : 
    void Create( VO< typename IVRLTN< TITEMID >::HUB > & usr )
    {   
        m_svc.RunNew( usr , this->GetPipeLine() , [ this ]( item_manger_type & mngrItem ){
            m_node.Use( [ & mngrItem ]( MNGR & mngrArr ){
                mngrArr.AddItem( mngrItem ) ;
            } ) ;
        } ) ; 
    }

private :
    TVSafeScopedObj< MNGR > m_node ;  
    TVServiceArray< item_interf_type , item_hub_inst_type > m_svc ; 
} ;

// Arr 01
template< typename TSysTrait , typename TID , typename TVALUE , typename TD > 
class TVRHubImpAdp< TSysTrait , TID , TVALUE , VI_ARR_01< TD > > : public TVRHubImpExp< TSysTrait , TID , TVALUE , TD >
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR MNGR ; 
    typedef  TVRHubImpExp< TSysTrait , TID , TVALUE , TD > BASE_TYPE ;

public :
    TVRHubImpAdp( TPIPELINE & pl ) 
        : BASE_TYPE( pl )
    {
        BASE_TYPE::AccessLinkage( [ this , & pl ]( auto & ... vin ){
            m_node.Renew( pl , vin ... ) ;
        } ) ; 
    }
    ~TVRHubImpAdp()
    {
        m_node.Clear() ;
    }

    //typedef unsigned  MNGR ;
    template< typename F >
    void AccessManager( F f )
    {  
        m_node.Use( f ) ;
    } ; 
    
    void Close()
    {
        m_node.Clear() ; 
        BASE_TYPE::Close() ;
    }

public :  
    void Connect( IVSlot< TD > & si )
    {  
        m_node.Use( [ & si ]( auto & mngr){
            mngr.SetItem( si ) ;
        } ) ;
    }

private :
    TVSafeScopedObj< MNGR > m_node ; 
} ;
 
template< typename TSysTrait , typename TID , typename TVALUE , typename TITEMID > 
class TVRHubImpAdp< TSysTrait , TID , VI_AND< IVRLTN< TITEMID > , TVALUE > , VI_ARR_01< IVRLTN< TITEMID > > >
    : public TVRHubImpRootBase< TSysTrait , TVALUE , TVRHubImpRoot< TSysTrait , IVRUniqHub< TID , VI_AND< IVRLTN< TITEMID > , TVALUE > > > >
{
public :
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef typename TVRHubNodeTrait< TSysTrait , IVRLTN< TID > >::MNGR MNGR ;
    typedef TVRHubImpRootBase< TSysTrait , TVALUE , TVRHubImpRoot< TSysTrait , IVRUniqHub< TID , VI_AND< IVRLTN< TITEMID > , TVALUE > > > > BASE_TYPE ;

private :
    typedef IVRLTN< TITEMID >                              item_rltn_type     ;
    typedef typename item_rltn_type::HUB                   item_interf_type   ;
    typedef TVRHubImp< TSysTrait , item_rltn_type    >    item_hub_inst_type ;
    typedef typename item_hub_inst_type::MNGR              item_manger_type   ;
    typedef TVRHubLinkageReadOnly< TSysTrait , item_rltn_type >    item_linkage_type  ;

public :
    TVRHubImpAdp( TPIPELINE & pl ) 
        : BASE_TYPE( pl )
    {
        BASE_TYPE::AccessLinkage( [ this , & pl ]( auto & ... vin ){
            m_node.Renew( pl , vin ... ) ;
        } ) ; 
    }
    ~TVRHubImpAdp()
    { 
    }

    void Close()
    {
        m_node.Clear() ;
        m_svc.Close() ;

        BASE_TYPE::Close() ;
    }

    //typedef unsigned  MNGR ;
    template< typename F >
    void AccessManager( F f )
    {  
        m_node.Use( f ) ; 
    } ; 

public : 
    void Create( VO< typename IVRLTN< TITEMID >::HUB > & usr )
    {   
        m_svc.RunNew( usr , this->GetPipeLine() , [ this ]( item_manger_type & mngrItem ){
            m_node.Use( [ & mngrItem ]( MNGR & mngrArr ){
                mngrArr.SetItem( mngrItem ) ;
            } ) ;
        } ) ; 
    }

private :
    TVSafeScopedObj< MNGR > m_node ;  
    TVServiceArray< item_interf_type , item_hub_inst_type > m_svc ; 
} ;

// HubImp defination
template< typename TSysTrait , typename TR >
class TVRHubImp : public TVRHubImpAdp< TSysTrait , typename TR::ID , typename TR::VALUE , typename TR::NAME  > 
{ 
public : 
    typedef typename TSysTrait::PIPELINE TPIPELINE ; 
    typedef TVRHubImpAdp< TSysTrait , typename TR::ID , typename TR::VALUE , typename TR::NAME  > base_type ;
    typedef TVRHubImp< TSysTrait , TR > my_type ;

    TVRHubImp( TPIPELINE & pl )
        : base_type( pl )
    {
    }

    template< typename FUSE >
    TVRHubImp( TPIPELINE & pl , FUSE f )
        : base_type( pl )
    {
        this->AccessManager( f ) ; 
    }

    ~TVRHubImp()
    {
        base_type::Close() ;
    }

public : 
    template< typename F >
    static void Slot2Manager( IVSlot< TR > & slot , F f )
    { 
        Slot2Object( slot , [ f ]( my_type & inst ){
            inst.AccessManager( f ) ;
        } ) ;
    } 
     
    template< typename F >
    static void Slot2Object( IVSlot< TR > & slot , F f )
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        bool bHasVal = false ;
        auto FAdp = [ f , & bHasVal ]( my_type & v ){
            f( v ) ;
            bHasVal = true ;
        } ;
        VCMN::TVLambdaUser< decltype( FAdp ) , my_type , my_type & > usr( FAdp ) ;
#else
        VCMN::TVLambdaUser< F , my_type , my_type & > usr( f ) ;
#endif
        IVUser< my_type , my_type & > ** p = _getDock() ;
        VASSERT( * p == 0 ) ;
         *p = & usr ;
         slot.Present() ;
         *p = 0 ;
#ifdef VD_NOT_CARE_PERFORMANCE
         VASSERT_MSG( bHasVal , "No Instantiate object ! " ) ;
#endif
    } 

private :
    virtual void Participate()
    {
        IVUser< my_type , my_type & > ** p = _getDock() ;
        VASSERT( * p != 0 ) ; 
        (*p)->Visit( * this ) ;
    }

private :
    static IVUser< my_type , my_type & > ** _getDock()
    {
        static IVUser< my_type , my_type & > * p(0) ;
        return &p ;
    } 
} ;

////////////////////////////////////////////////////////////////////////////////////////////////

// (4) system implements 

////////////////////////////////////////////////////////////////////////////////////////////////
template< typename TSysTrait , typename TINTF , typename ... TRs > class TVDynamicSystemImpBase2 ;

template< typename TSysTrait , typename TINTF >
class TVDynamicSystemImpBase2< TSysTrait , TINTF > : public TINTF 
{
public : 
    template< typename ... TARGs >
    TVDynamicSystemImpBase2( const TARGs & ... args )
        : m_pipeLine( args ... ) 
    {
    }
    ~TVDynamicSystemImpBase2(){} 
    void Close(){ }
protected :
    typedef typename TSysTrait::PIPELINE    TPIPELINE ; 
    TPIPELINE & GetPipeLine()  { return m_pipeLine ; }

public :
    virtual void Elapse()
    {  
       m_pipeLine.Elapse() ;
    } 

private:
    TPIPELINE m_pipeLine;
} ;  

template< typename TSysTrait , typename TINTF , typename TR ,  typename ... TROTHERs >
class TVDynamicSystemImpBase2< TSysTrait , TINTF , TR , TROTHERs ... >
    : public TVDynamicSystemImpBase2< TSysTrait , TINTF , TROTHERs ... >
{
public :
    typedef TVDynamicSystemImpBase2< TSysTrait , TINTF , TROTHERs ... > base_type    ;
    typedef typename TR::HUB                                            interf_type  ;  
    typedef TVRHubImp< TSysTrait , TR  >                               svr_type     ; 

public : 
    template< typename ... TARGs >
    TVDynamicSystemImpBase2( const TARGs & ... args )
        : base_type( args ... ) 
    {
    }
    ~TVDynamicSystemImpBase2()
    {
        m_svc.Close();
    }  

public :
    virtual void Close ( )
    {
        base_type::Close() ;
    }  
    virtual void Create ( VO< interf_type > & usr )
    {
        m_svc.RunNew( usr , this->GetPipeLine() ) ;
    } 

private :  
    TVServiceArray< interf_type , svr_type > m_svc ; 
} ;

template< typename TSysTrait , typename TSYS > class TVDynamicSystemImp2 ;
template< typename TSysTrait , typename ... TRs >
class TVDynamicSystemImp2< TSysTrait , IVSysDynamic< TRs ... > >
    : public TVDynamicSystemImpBase2< TSysTrait , IVSysDynamic< TRs ... > , TRs ... >
{
public :
    typedef TVDynamicSystemImpBase2< TSysTrait , IVSysDynamic< TRs ... > , TRs ... > IMP_BASE ;

public :
    template< typename ... TARGs >
    TVDynamicSystemImp2( const TARGs & ... args )
        : IMP_BASE( args ... )
    {    
    }
    ~TVDynamicSystemImp2()
    {
        IMP_BASE::Close() ;
    } 
} ;

// Systematic Converter wrapper classes

////////////////////////////////////////////////////////////

template< typename TID , typename DESC , typename TOUTPUT > class TVRSystematicConverter ;

template< typename TID , typename ... TINPUTs , typename TOUTPUT > 
class TVRSystematicConverter< TID , VI_AND< TINPUTs ... > , TOUTPUT > : public IVConverter< TOUTPUT , TINPUTs ... >
{ 
public :
    typedef IVConverter< TOUTPUT , TINPUTs ... > TCONVTR ;
    typedef IVRUniqHub < TID     , TINPUTs ... > TIMPROT ;  

    template< typename TSYS >
    TVRSystematicConverter( TSYS & sys )
        : m_hub( sys )
    { 
    }
    ~TVRSystematicConverter()
    {
    }

public :
    virtual void Input ( IVSlot< TINPUTs > & ... inputs ) 
    {
        m_hub.BatchConnect( inputs ... ) ;
    }

    virtual void Output( IVInputPort< TOUTPUT > & inp ) 
    {
        m_hub.Output( inp ) ;
    }
     
private : 
    TVHub< IVRLTN< TID    > >   m_hub ;   
} ;

template< typename TSYS > class TVRSvcSystematicConverter ; 
template< typename TID >
class TVRSvcSystematicConverter< IVSysDynamic< IVRLTN< TID > > >
{
public :
    typedef IVRLTN< TID >                                                                            ROOT     ; 
    typedef TVRSystematicConverter< typename ROOT::ID , typename ROOT::NAME , typename ROOT::VALUE > ROOTPIPE ;
    typedef typename ROOTPIPE::TCONVTR                                                               INTERF   ;
//
public :
    template< typename TLIB >
    TVRSvcSystematicConverter( TLIB & lib )
    {
        lib.Create( VD_P2U( m_proxySys ) ) ;
    }
    ~TVRSvcSystematicConverter()
    {
        m_svcPipe.Close() ;
    }
//public : 
    void Elapse()
    {
         m_proxySys.UseServer( []( auto & ds ){ ds.Elapse() ; } ) ;
    }
    void Close()
    {
        m_svcPipe.Close() ;
    } 
    void RunService( VO< INTERF > & usr )
    { 
        m_proxySys.UseServer( [ this , &usr ]( auto & sys ){
            m_svcPipe.RunNew( usr , sys ) ;   
        }) ; 
    }

private :
   TVServiceProxy< IVSysDynamic< IVRLTN< TID > > > m_proxySys ;
   TVServiceArray< INTERF , ROOTPIPE >             m_svcPipe  ;
} ;

template< typename TLIB , typename TSYS , void FNAME( IVUser< TLIB > & usr ) > class TVRSvcAsynSystematicConverter ; 
template< typename TLIB , typename TID  , void FNAME( IVUser< TLIB > & usr ) >
class TVRSvcAsynSystematicConverter< TLIB , IVSysDynamic< IVRLTN< TID > > , FNAME >
{
public :
    typedef TVRSvcSystematicConverter< IVSysDynamic< IVRLTN< TID > > > sys_imp_type ; 
    typedef typename sys_imp_type::INTERF                              interf_type  ;

    TVRSvcAsynSystematicConverter() 
        : m_pImp( 0 )
    {
        mutexEnd.lock() ;

        std::thread trd( TVRSvcAsynSystematicConverter::_run , FNAME , this ) ;
        std::unique_lock<std::mutex> ulock( mutexBgn ) ;
        condbgn.wait( ulock, [ this ]{  return m_pImp != 0 ; } ) ;

        m_thread.swap( trd ) ;
    } 
    ~TVRSvcAsynSystematicConverter()
    {
        mutexEnd.unlock() ;   
        std::unique_lock<std::mutex> ulock( mutexBgn ) ;
        condbgn.wait( ulock, [ this ]{  return m_pImp == 0 ; } ) ;  
        m_thread.join() ;
    } ;
    
public :
    void RunService( VO< interf_type > & usr )
    {
        VASSERT( m_pImp != 0 ) ;
        m_pImp->RunService( usr ) ;
    }

    void Elapse()
    {
        VASSERT( m_pImp != 0 ) ;
        m_pImp->Elapse() ;
    }

    void Close() 
    {
        VASSERT( m_pImp != 0 ) ;
        m_pImp->Close() ;
    }
    
    template< typename T >
    void Init( const T & v )
    {
    }

private :
    static void _run( void f( IVUser< TLIB > & usr ) , TVRSvcAsynSystematicConverter * pObj )
    {  
        f( VD_L2U( [ pObj ]( TLIB & lib ){  
            _runWithLib( lib , pObj ) ; 
            _waitforusing( pObj ) ;
            _release( pObj ) ;
        } ) ) ; 
    } 

    static void _runWithLib( TLIB & lib , TVRSvcAsynSystematicConverter * pObj )
    {
        std::lock_guard<std::mutex> guard( pObj->mutexBgn ) ; 
        pObj->m_pImp = new sys_imp_type( lib ) ; 
        pObj->condbgn.notify_all() ;
    }
    
    static void _waitforusing( TVRSvcAsynSystematicConverter * pObj )
    {
        std::lock_guard<std::mutex> guard( pObj->mutexEnd ) ;  
    }

    static void _release( TVRSvcAsynSystematicConverter * pObj ) 
    {         
        std::lock_guard<std::mutex> guard( pObj->mutexBgn ) ; 

        delete pObj->m_pImp ;
        pObj->m_pImp = 0 ; 
        pObj->condbgn.notify_all() ; 
    } 

private :
     
    std::mutex              mutexBgn    ;
    std::mutex              mutexEnd    ;
    std::condition_variable condbgn     ;
    std::condition_variable condend     ;
    sys_imp_type   *        m_pImp      ;
    std::thread             m_thread    ;
} ; 

/////////////////////////////////////////////////////////////

// Macros of system implementation 

////////////////////////////////////////////////////////////

template< typename T , T , typename TCONSTRUCT > struct TVConverterTraitBase ;

template< typename TIMP , typename R , typename TOUTPUT , typename ... TINPUTs , R (TIMP::*mf)( TOUTPUT & , const TINPUTs & ... ) , typename TCONSTRUCT >
struct TVConverterTraitBase< R ( TIMP::* )( TOUTPUT & , const TINPUTs & ... ) , mf , typename TCONSTRUCT > 
{
    typedef TVSvcConverter< TIMP , TOUTPUT , TINPUTs ... > server_type ;
    typedef IVConverter< TOUTPUT , TINPUTs ... >           node_type   ;
} ; 

template< typename TIMP , typename R , typename ... TINPUTs , R (TIMP::*mf)( const TINPUTs & ... ) , typename TCONSTRUCT >
struct TVConverterTraitBase< R ( TIMP::* )( const TINPUTs & ... ) , mf , typename TCONSTRUCT > 
{
    typedef TVSvcConverter< TIMP , void , TINPUTs ... > server_type ;
    typedef IVConverter< void , TINPUTs ... >           node_type   ;
} ; 

template< typename TR , typename TOU , typename TIN > struct TVRelation2ConvertBase ;
template< typename TRID , typename TOU , typename ... TINPUTS > 
struct TVRelation2ConvertBase< IVRLTN< TRID > , TOU , VI_AND< TINPUTS ... > >
{ 
    typedef IVConverter< TOU , TINPUTS ... > type   ; 
} ;

template< typename TRID >
using TVRelation2Convert = TVRelation2ConvertBase< IVRLTN< TRID > , typename IVRLTN< TRID >::VALUE , typename IVRLTN< TRID >::NAME > ;
// 
//template< typename T , typename ... TSUBs > class TVExternalSystemExpandServerBase ;
//
//template< typename TRID , typename TOU , typename ... TINPUTS >  
//class TVExternalSystemExpandServerBase< IVRUniqHub< TRID , TOU , TINPUTS ... > >
//{
//public :
//    typedef IVConverter< TOU , TINPUTS ... > node_type   ;  
//
//private :
//    class VNode : public node_type
//    {
//    public :
//        VNode( IVSysDynamic< IVRLTN< TRID > > & sys )
//        {
//            m_hub.Create( sys ) ; 
//        }
//        ~VNode ()
//        { 
//        }
//    public :
//        virtual void Input ( IVSlot< TINPUTS > & ... inputs ) 
//        {
//            m_hub.BatchConnect( inputs ... ) ;
//        }
//        virtual void Output( IVInputPort< TOU > & op )
//        {
//            m_hub.Output( op ) ;
//        }
//
//    private :
//        TVHub< IVRLTN< TRID > > m_hub ;
//    } ;
//
//public :
//    void Elapse()
//    {
//        m_extSysProxy.UseServer( []( auto & sys ){
//            sys.Elapse() ;
//        }) ;
//    }
//    void Close()
//    {
//        m_svcNode.Close() ;
//    } 
//    //template< void >
//    template< typename ... TAs >
//    void Init( const TVPipelineArg< TAs ... > & v )
//    {   
//        const VSESS< IVRLTN< TRID > > & es = v.Get< VSESS< IVRLTN< TRID > > >( ) ; 
//        es.pSysProv->Apply( VD_P2U( m_extSysProxy ) ) ;
//    }
//    void RunService( VO< node_type > & pipe )
//    {
//        m_extSysProxy.UseServer( [ this , & pipe ]( auto & sys ){
//            m_svcNode.RunNew( pipe , sys ) ;
//        }) ; 
//    }
//private : 
//    TVServiceProxy< IVSysDynamic< IVRLTN< TRID > > > m_extSysProxy ;
//    TVServiceArray< node_type , VNode > m_svcNode ;
//} ;
//
//template< typename TRID , typename TOU , typename ... TINPUTS >  
//class TVExternalSystemExpandServerBase< IVRUniqHub< TRID , VI_AND< TOU > , TINPUTS ... > > : public TVExternalSystemExpandServerBase< IVRUniqHub< TRID , TOU , TINPUTS ... > >
//{
//public :
//    typedef TVExternalSystemExpandServerBase< IVRUniqHub< TRID , TOU , TINPUTS ... > > base_type ;
//    typedef IVConverter< TOU , TINPUTS ... > node_type   ;  
//    using  base_type::base_type ;
//} ;
//
//template< typename TRID , typename TOU , typename ... TINPUTS , typename ... TSUBs >  
//class TVExternalSystemExpandServerBase< IVRUniqHub< TRID , VI_AND< TOU > , TINPUTS ... > , TSUBs ... >
//{
//public :
//    typedef IVConverter< TOU , TINPUTS ... > node_type   ;  
//
//private :
//    class VNode : public node_type
//    {
//    public :
//        VNode( IVSysDynamic< IVRLTN< TRID > > & sys )
//        {
//            m_hub.Create( sys ) ; 
//        }
//        ~VNode ()
//        { 
//        }
//    public :
//        virtual void Input ( IVSlot< TINPUTS > & ... inputs ) 
//        {
//            m_hub.BatchConnect( inputs ... ) ;
//        }
//        virtual void Output( IVInputPort< TOU > & op )
//        {
//            m_hub.Output( op ) ;
//        }
//
//    private :
//        TVHub< IVRLTN< TRID > > m_hub ;
//    } ;
//
//public :
//    void Elapse()
//    {
//        static_assert(0,"") ;
//        m_extSysProxy.UseServer( []( auto & sys ){
//            sys.Elapse() ;
//        }) ;
//    }
//    void Close()
//    {
//        m_svcNode.Close() ;
//    } 
//    //template< void >
//    template< typename ... TAs >
//    void Init( const TVPipelineArg< TAs ... > & v )
//    {   
//        const VSESS< IVRLTN< TRID > > & es = v.Get< VSESS< IVRLTN< TRID > > >( ) ; 
//        es.pSysProv->Apply( VD_P2U( m_extSysProxy ) ) ;
//    }
//    void RunService( VO< node_type > & pipe )
//    {
//        m_extSysProxy.UseServer( [ this , & pipe ]( auto & sys ){
//            m_svcNode.RunNew( pipe , sys ) ;
//        }) ; 
//    }
//private : 
//    TVServiceProxy< IVSysDynamic< IVRLTN< TRID > > > m_extSysProxy ;
//    TVServiceArray< node_type , VNode > m_svcNode ;
//} ;
//
//template< typename TES , typename ... T > class TVExternalSystemNodeServer ;
//
//template< typename TRID > class TVExternalSystemNodeServer< IVRLTN< TRID > >
//    : public TVExternalSystemExpandServerBase< typename IVRLTN< TRID >::HUB >
//{
//public :
//    typedef TVExternalSystemExpandServerBase< typename IVRLTN< TRID >::HUB > base_type ;
//    using base_type::base_type ;
//    typedef typename base_type::node_type node_type ;
//} ;
// 
//template< typename TRID , typename TRSUBID , typename ... TSUBs >
//class TVExternalSystemNodeServer< IVRLTN< TRID > , IVRLTN< TRSUBID > , TSUBs ... > 
//{
//public :
//    typedef TVExternalSystemNodeServer< IVRLTN< TRSUBID > , TSUBs ... > sub_type ;
//    typedef TVExternalSystemExpandServerBase< typename IVRLTN< TRID >::HUB > bridge_type ;
//    typedef typename sub_type::node_type node_type ;
//    typedef typename bridge_type::node_type bridge_node_type ;
//
//public : 
//    TVExternalSystemNodeServer()
//    {
//        //static_assert( 0 , "" ) ;
//    }
//    ~TVExternalSystemNodeServer()
//    {}
//
//public :
//    template< typename ... TAs >
//    void Init( const TVPipelineArg< TAs ... > & v )
//    {
//        m_bridge.Init( v ) ;
//        
//        TVServiceProxy< IVConverter< VSESS< IVRLTN< TRSUBID > > > > extProxy ;
//        m_bridge.RunService( extProxy ) ;
//
//    } 
//    void Elapse()
//    {  
//    } 
//    void RunService( VO< node_type > & pipe ){}
//    template< typename ... TAs >
//    void Close()
//    {}
//private :
//    bridge_type  m_bridge ;
//} ;

template< typename TRID , typename TRSRCID , typename TI , typename TO > class TVRelationNodeServerBase ;

template< typename TRID , typename TRSRCID , typename ... TINPUTs , typename ... TOUTPUTs  >  
class TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , VI_AND< TINPUTs ... > , VI_AND< TOUTPUTs ... > >
{
public :
    TVRelationNodeServerBase( const VSESS< IVRLTN< TRSRCID > > & src )
        : m_extSystem( src )
    {
        m_extSystem.pSysProv->Apply( VD_P2U( m_sysProxy )) ;
    }

public :
    void Build( TOUTPUTs & ... vOuts , const TINPUTs & ... vIns ) 
    {
    }

private :
    const VSESS< IVRLTN< TRSRCID > > & m_extSystem ;
    TVServiceProxy< IVSysDynamic< IVRLTN< TRSRCID > > > m_sysProxy ;
} ; 

template< typename TRID , typename TRSRCID , typename ... TINPUTs , typename TOUs  >  
class TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , VI_AND< TINPUTs ... > , TOUs >
    : public TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , VI_AND< TINPUTs ... > , VI_AND< TOUs > >
{
public :
    typedef TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , VI_AND< TINPUTs ... > , VI_AND< TOUs > > base_type ;
    using base_type::base_type ;
} ; 

template< typename ... T >  class TVRelationNodeServer ;

template< typename TRID , typename TRSRCID >  
class TVRelationNodeServer< IVRLTN< TRID > , IVRLTN< TRSRCID > >
    : public TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , typename IVRLTN< TRID >::NAME , typename IVRLTN< TRID >::VALUE >
{
public :
    typedef TVRelationNodeServerBase< IVRLTN< TRID > , IVRLTN< TRSRCID > , typename IVRLTN< TRID >::NAME , typename IVRLTN< TRID >::VALUE > base_type ;
    using base_type::base_type ;
} ;

template< typename ... TDEPs >
class TVConverterDep
{
} ;

template< typename ... T > struct TVConverterTrait ;

template< typename TIMP , typename ... TRDEPIDs >
struct TVConverterTrait< TIMP , TRDEPIDs ... >  
     : TVConverterTraitBase< decltype( &TIMP::Build ) , &TIMP::Build , TVConverterDep< TRDEPIDs ... > > 
{} ;
 

template< typename TRID , typename ... TRDEPIDs >
struct TVConverterTrait< IVRLTN< TRID > , TRDEPIDs ... > : TVConverterTrait< TVRelationNodeServer< IVRLTN< TRID > , TRDEPIDs ... > >
{} ;

// For External system node
//template< typename TRID , typename ... TSUBs >
//struct TVConverterTrait< IVRLTN< TRID > , TSUBs ... >
//{
//    typedef TVExternalSystemNodeServer< IVRLTN< TRID > , TSUBs ... > server_type ;
//    typedef typename server_type::node_type node_type ; 
//} ;

template< typename TITEM >
struct TVConverterArrTrait
{
    typedef TVSvcCollector< TITEM >             server_type ;
    typedef typename server_type::interf_type   node_type   ;
} ;

template< typename TITEM >
struct TVConverterSwitchTrait
{
    typedef TVSvcSwitcher< TITEM >              server_type ;
    typedef typename server_type::interf_type   node_type   ;
};

template< typename TLIB , typename TSYS , void FNAME( IVUser< TLIB > & libUsr ) > 
struct TVConverterSysTrait
{
    typedef TVRSvcAsynSystematicConverter< TLIB , TSYS , FNAME > server_type ; 
    typedef typename server_type::interf_type   node_type   ;
} ;

template< typename TI , typename TO , TI , TO > struct TVConverterCtrlTraitBase ;

template< typename TIMP , typename R , typename ... TINPUTs , typename ... TOUTINs 
        , R (TIMP::* mfin)( const TINPUTs & ... ) 
        , void (TIMP::* mfout)( IVInputPort< TOUTINs ... > & ) >
struct TVConverterCtrlTraitBase< R ( TIMP::* )( const TINPUTs & ... ) 
                              ,  void ( TIMP::* )( IVInputPort< TOUTINs ... > & ) 
                              , mfin 
                              , mfout > 
{
    typedef TVSvcDuplicate< TIMP , VI_AND< TOUTINs ... > , TINPUTs ... > server_type ;
    typedef IVConverter< VI_AND< TOUTINs ... > , TINPUTs ... >           node_type   ;
} ;  

template< typename TIMP >
using TVConverterCtrlTrait = TVConverterCtrlTraitBase< decltype( &TIMP::Update ) 
                                                    , decltype( &TIMP::Output ) 
                                                    , &TIMP::Update , &TIMP::Output > ;


template< unsigned id > struct VPipeNodeID {} ;

template< typename TNID , typename TCNVR > struct TVDeduceConverterWrapper ;

template< unsigned TNID , typename TOUTPUT , typename ... TINPUTs >
struct TVDeduceConverterWrapper< VPipeNodeID< TNID > , IVConverter< TOUTPUT , TINPUTs ... > >
{
    typedef TVConverterWrapper< TOUTPUT , TINPUTs ... > type ;
} ;


template< unsigned > struct TVHelperCounter
{
};

#define VDDP_NODE_IMP_WITHID( nID , name , trait , ... )   typename trait::server_type m_sysSvc##name ;\
    typedef typename TVDeduceConverterWrapper< VPipeNodeID< nID > , typename trait::node_type >::type name ;\
    void Create##name( VO< typename trait::node_type > & usr )\
    { m_sysSvc##name.RunService( usr ); }\
    template< typename TFST , typename ... TINs >\
    void Create##name( name & usr , TFST & fstInput , TINs & ... inputs )\
    {  m_sysSvc##name.RunService( usr );  VLNK( usr , fstInput , inputs ... ) ; }\
    VD_DECLAREEC_IMP( m_sysSvc##name , nID )

#define VDDP_NODE_IMP( name , trait , ... ) VDDP_NODE_IMP_WITHID( VD_EXPAND( __COUNTER__ ) , name , trait , __VA_ARGS__ )


#define VD_DECLAREEC_IMP( sysname , c ) void LoopElapse( const TVHelperCounter< c > * ) \
                                                { LoopElapse( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) ) ; sysname.Elapse() ; } \
                                         void LoopClose( const TVHelperCounter< c > * ) \
                                                { LoopClose( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) ) ; sysname.Close() ; } \
                                         template< typename T > void LoopInit( const TVHelperCounter< c > * , const T & v ) \
                                                { LoopInit<T>( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) , v ) ; sysname.Init( v ) ; } 

#define VD_BEGIN_PIPELINE_IMP( c )      void LoopElapse( const TVHelperCounter< c > * ) {}  \
                                        void LoopClose( const TVHelperCounter< c > * ) {} \
                                        template< typename T > void LoopInit( const TVHelperCounter< c > * , const T & v ){}

#define VD_END_PIPELINE_IMP( c )       public : void Elapse() { LoopElapse( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) ) ; } \
                                                void Close() { LoopClose( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) ) ; } \
                                       private : template< typename T > void _initialize( const T & v ){ LoopInit<T>( reinterpret_cast< TVHelperCounter< c - 1 > * >( 0 ) , v ) ; } 

#define VD_BEGIN_PIPELINE2( plname , envname ) class plname { \
    public : typedef envname DEPENDENCE ; plname( const envname & env ):m_Env(env){ _initialize(env) ; } ~plname(){ Close() ; } \
        const envname & GetEnv() { return m_Env; }\
    private : const envname & m_Env; \
    public : \
        VD_BEGIN_PIPELINE_IMP( VD_EXPAND( __COUNTER__ ) ) 


#define VD_BEGIN_PIPELINE( plname , ... ) class plname { \
    public : typedef TVPipelineArg< __VA_ARGS__ > DEPENDENCE ; \
            template< typename ... TARGs > \
            plname( const TARGs & ... args ):m_Env( args ... ){ _initialize( m_Env ) ; } ~plname(){ Close() ; } \
        const DEPENDENCE & GetEnv() { return m_Env; }\
        private : const DEPENDENCE m_Env; \
        public :\
        VD_BEGIN_PIPELINE_IMP( VD_EXPAND( __COUNTER__ ) )
#define VD_END_PIPELINE()   VD_END_PIPELINE_IMP( VD_EXPAND( __COUNTER__ ) ) } ;


#define VDDP_ARR( name , type )  VDDP_NODE_IMP( name , TVConverterArrTrait< type > )  
#define VDDP_SWT( name , type )  VDDP_NODE_IMP( name , TVConverterSwitchTrait< type > )  
#define VDDP_NOD( name , ...  )  VDDP_NODE_IMP( name , TVConverterTrait< __VA_ARGS__ > , __VA_ARGS__ )
#define VDDP_SYS( name , lib , factory , sys )  static void _get_lib_name_of##name( IVUser< factory > & usr ) \
                                        { VD_USESTDLIBERRARY( lib , [ & usr ]( factory & lib ){ usr.Visit( lib ) ; } ) ;}\
                                        typedef TVConverterSysTrait< factory , sys , _get_lib_name_of##name > sys_svc_type_of##name ;\
                                        VDDP_NODE_IMP( name , sys_svc_type_of##name ) 
#define VDDP_CTR( name , type )  typedef TVConverterCtrlTrait< type > sys_svc_type_of##name ;\
                                VDDP_NODE_IMP( name , sys_svc_type_of##name )  

#define VDDP_USR( name , trait ) VDDP_NODE_IMP( name , trait )



////////////////////////////////////////////////////////////////////////////////////////////////

// (4-2) another system implements 

////////////////////////////////////////////////////////////////////////////////////////////////
template< typename TSysTrait , typename TEnv , typename TINTF , typename ... TRs > class TVDynamicSystemImpBase3;

template< typename TSysTrait , typename TINTF >
class TVDynamicSystemImpBase3< TSysTrait , std::tuple<> , TINTF > : public TINTF
{
public:
    TVDynamicSystemImpBase3( const std::tuple<> & env )
    {}
    ~TVDynamicSystemImpBase3()
    {}
    void Close()
    {}
protected:
    typedef typename TSysTrait::PIPELINE    TPIPELINE;
    TPIPELINE & GetPipeLine()
    {
        return m_pipeLine;
    }

    //TEnv & GetEnv()
    //{
    //    return m_Env;
    //}

public:
    virtual void Elapse()
    {
        m_pipeLine.Elapse();
    }

private:
    TPIPELINE   m_pipeLine ;
};

template< typename TSysTrait , typename TEnv , typename TINTF >
class TVDynamicSystemImpBase3< TSysTrait , TEnv , TINTF > : public TINTF
{
public:
    TVDynamicSystemImpBase3( const TEnv & env ) :m_pipeLine( env )
    {}
    ~TVDynamicSystemImpBase3()
    {}
    void Close()
    {}
protected:
    typedef typename TSysTrait::PIPELINE    TPIPELINE;
    TPIPELINE & GetPipeLine()
    {
        return m_pipeLine;
    }

    //TEnv & GetEnv()
    //{
    //    return m_Env;
    //}

public:
    virtual void Elapse()
    {
        m_pipeLine.Elapse();
    }

private:
    TPIPELINE   m_pipeLine;
    //TEnv &      m_Env;
};

template< typename TSysTrait , typename TEnv , typename TINTF , typename TR , typename ... TROTHERs >
class TVDynamicSystemImpBase3< TSysTrait , TEnv , TINTF , TR , TROTHERs ... >
    : public TVDynamicSystemImpBase3< TSysTrait , TEnv , TINTF , TROTHERs ... >
{
public:
    typedef TVDynamicSystemImpBase3< TSysTrait , TEnv , TINTF , TROTHERs ... > base_type;
    typedef typename TR::HUB                                            interf_type;
    typedef TVRHubImp< TSysTrait , TR  >                               svr_type;

public:
    TVDynamicSystemImpBase3( const TEnv & env ) :base_type( env )
    {}
    ~TVDynamicSystemImpBase3()
    {
        m_svc.Close();
    }

public:
    virtual void Close()
    {
        base_type::Close();
    }
    virtual void Create( VO< interf_type > & usr )
    {
        m_svc.RunNew( usr , this->GetPipeLine() );
    }

private:
    TVServiceArray< interf_type , svr_type > m_svc;
};

template< typename TSysTrait , typename TEnv , typename TSYS > class TVDynamicSystemImp3;

template< typename TSysTrait , typename TEnv , typename ... TRs >
class TVDynamicSystemImp3< TSysTrait , TEnv , IVSysDynamic< TRs ... > >
    : public TVDynamicSystemImpBase3< TSysTrait , TEnv , IVSysDynamic< TRs ... > , TRs ... >
{
public:
    typedef TVDynamicSystemImpBase3< TSysTrait , TEnv , IVSysDynamic< TRs ... > , TRs ... > IMP_BASE;

public:
    TVDynamicSystemImp3( const TEnv & env ) :IMP_BASE( env )
    {}
    ~TVDynamicSystemImp3()
    {
        IMP_BASE::Close();
    }
};

#define VD_BEGIN_PIPELINE2( plname , envname ) class plname { \
    public : typedef envname DEPENDENCE ; plname( const envname & env ):m_Env(env){ _initialize(env) ; } ~plname(){ Close() ; } \
        const envname & GetEnv() { return m_Env; }\
    private : const envname & m_Env; \
    public : \
        VD_BEGIN_PIPELINE_IMP( VD_EXPAND( __COUNTER__ ) ) 

#define VD_END_PIPELINE2()   VD_END_PIPELINE_IMP( VD_EXPAND( __COUNTER__ ) ) } ;

template< typename TTuple , size_t val , typename ... Ts > class TVTupleDataGetterHelper;

template< typename TTuple , size_t val , typename TFst , typename ... TOthers >
class TVTupleDataGetterHelper< TTuple , val , TFst , TOthers ... > : public TVTupleDataGetterHelper< TTuple , val + 1 , TOthers ... >
{
public:
    TVTupleDataGetterHelper(){}
    ~TVTupleDataGetterHelper(){}

    template< typename FUNC , typename ... TDatas >
    void Use( TTuple & tup , FUNC f , TDatas & ... dats )
    {
        TVTupleDataGetterHelper< TTuple , val + 1 , TOthers ... >::Use( tup , f , dats ... , std::get<val>( tup ) );
    }
};

template< typename TTuple , size_t val >
class TVTupleDataGetterHelper< TTuple , val >
{
public:
    TVTupleDataGetterHelper(){}
    ~TVTupleDataGetterHelper(){}

    template< typename FUNC , typename ... TDatas >
    void Use( TTuple & tup , FUNC f , TDatas & ... dats )
    {
        f( dats ... );
    }
};

template< typename ... Ts > class TVTupleDataGetterHelperGen;

template< typename ... TTuples >
class TVTupleDataGetterHelperGen< std::tuple< TTuples ... > >
    : public TVTupleDataGetterHelper< std::tuple< TTuples ... > , 0 , TTuples ... >
{};

template< typename TTuple > 
class TVTupleDataGetter : public TVTupleDataGetterHelperGen< TTuple >
{
public:
    TVTupleDataGetter(){}
    ~TVTupleDataGetter(){}

    template< typename TFunc >
    void Use( TTuple & tup , TFunc func )
    {
        TVTupleDataGetterHelperGen< TTuple >::Use( tup , func );
    }
};

template<>
class TVTupleDataGetter< std::tuple<> >
{
public:
    TVTupleDataGetter(){}
    ~TVTupleDataGetter(){}

    template< typename Func >
    void Use( std::tuple<> & tup , Func func )
    {
        func();
    }
};

template< typename T > class TVLNKObj : public std::false_type{};

template< typename TLNK > class TVLNKObj< IVSlot     < TLNK > > : public std::true_type{};
template< typename TLNK > class TVLNKObj< TVSource   < TLNK > > : public std::true_type{};
template< typename TLNK > class TVLNKObj< TVData     < TLNK > > : public std::true_type{};
template< typename TLNK > class TVLNKObj< IVInputPort< TLNK > > : public std::true_type{};
template< typename TLNK > class TVLNKObj< TVComp     < TLNK > > : public std::true_type{};
template< typename TLNK > class TVLNKObj< TVAggr     < TLNK > > : public std::true_type{};

template< typename TTuple , typename ... TParams > class TVCreateSpliterFinish;

template< typename ... TCons , typename ... TParams >
class TVCreateSpliterFinish< std::tuple< TCons & ... > , TParams ... >
{
public:
    template< typename CreateFunc , typename LinkFunc >
    TVCreateSpliterFinish( CreateFunc cf , LinkFunc lf , std::tuple< TCons & ... > & tup , TParams & ... params )
    {
        TVTupleDataGetter< std::tuple< TCons & ... > > tdg;
        tdg.Use( tup , std::function< void( TCons & ...  ) >( [&cf]( TCons & ... cons ){
            cf.Build( cons... );
        } ) );

        lf.Build( params ... );
    }
    ~TVCreateSpliterFinish(){}
};

template< typename TTuple , typename ... TParams > class TVCreateSpliter;

template< bool lnk , typename TTuple , typename TParam , typename ... TParams > class TVCreateSpliterHelper;

template< typename ... TCons , typename TParam , typename ... TParams >
class TVCreateSpliterHelper< false , std::tuple<TCons & ...> , TParam , TParams ... > 
    : public TVCreateSpliter< std::tuple<TCons & ... , TParam & > , TParams ... >
{
public:
    typedef TVCreateSpliter< std::tuple<TCons & ... , TParam & > , TParams ... > PARENT;
    
    template< typename CreateFunc , typename LinkFunc >
    TVCreateSpliterHelper( CreateFunc cf , LinkFunc lf , std::tuple< TCons & ... > & tup , TParam & param , TParams & ... params )
        :PARENT( cf , lf , std::tuple_cat( tup , std::tuple< TParam & >(param) ) , params ... )
    {}
    ~TVCreateSpliterHelper(){}
};

template< typename ... TCons , typename TParam , typename ... TParams >
class TVCreateSpliterHelper< true , std::tuple<TCons & ...> , TParam , TParams ... >
    : public TVCreateSpliterFinish< std::tuple<TCons & ... > , TParam , TParams ... >
{
public:
    typedef TVCreateSpliterFinish< std::tuple<TCons & ... > , TParam , TParams ... > PARENT;
    using PARENT::PARENT;
};

template< typename ... TCons , typename TParam , typename ... TParams >
class TVCreateSpliter< std::tuple< TCons & ... > , TParam , TParams ... > 
    : public TVCreateSpliterHelper< TVLNKObj<TParam>::value , std::tuple< TCons & ... > , TParam , TParams ... >
{
public:
    typedef TVCreateSpliterHelper< TVLNKObj<TParam>::value , std::tuple< TCons & ... > , TParam , TParams ... > PARENT;

    template< typename CreateFunc , typename LinkFunc >
    TVCreateSpliter( CreateFunc cf , LinkFunc lf , std::tuple< TCons & ... > & tup , TParam & param , TParams & ... params )
        :PARENT( cf , lf , tup , param , params ... )
    {}
    ~TVCreateSpliter(){}
};

template< typename ... TCons >
class TVCreateSpliter< std::tuple< TCons & ... > >
{
public:
    template< typename CreateFunc , typename LinkFunc >
    TVCreateSpliter( CreateFunc cf , LinkFunc lf , std::tuple< TCons & ... > & tup )
    {
        TVTupleDataGetter< std::tuple< TCons & ... > > tdg;
        tdg.Use( tup , std::function< void( TCons & ...  ) >( [ &cf ]( TCons & ... cons ) {
            cf.Build( cons... );
        } ) );
    }
    ~TVCreateSpliter(){}
};

template< typename TServerType , typename TUsr >
class TVCreateFunc
{
public:
    TVCreateFunc( TServerType & sys , TUsr & usr ):m_Sys(sys),m_Usr(usr){}
    ~TVCreateFunc(){}

    template< typename ... TCons >
    void Build( TCons & ... cons )
    {
        m_Sys.RunService( m_Usr , cons ... );
    }

private:
    TServerType & m_Sys;
    TUsr        & m_Usr;
};

template< typename TUsr >
class TVLinkFunc
{
public:
    TVLinkFunc(TUsr & usr):m_Usr(usr){}
    ~TVLinkFunc(){}

    template< typename ... TLnks >
    void Build( TLnks & ... lnks )
    {
        VLNK( m_Usr , lnks... );
    }

private:
    TUsr        & m_Usr;
};

#define VDDP_NODE_IMP2_WITHID( nID , name , trait , ... ) typename trait::server_type m_sysSvc##name ;\
    typedef typename TVDeduceConverterWrapper< VPipeNodeID< nID > , typename trait::node_type >::type name ;\
    template< typename ... TParams >\
    void Create##name( name & usr , TParams & ... params )\
    { TVCreateSpliter<std::tuple<> , TParams ... > cs( TVCreateFunc<typename trait::server_type , name >( m_sysSvc##name , usr ), TVLinkFunc< name >( usr ) , std::tuple<>() , params ... ); }\
    VD_DECLAREEC_IMP( m_sysSvc##name , nID )
 
#define VDDP_NODE_IMP2( name , trait ) VDDP_NODE_IMP2_WITHID( VD_EXPAND( __COUNTER__ ) , name , trait )

/////////////////////////////////////////////////////////////////////////////////
// Converter 2
/////////////////////////////////////////////////////////////////////////////////

template< typename TIMP , typename TEXP >
class TVConverterData2
{
public:
    template< typename ... TEnvs >
    TVConverterData2( TEnvs & ... envs )
        : m_instCnvtr( envs ... )
#ifdef VD_NOT_CARE_PERFORMANCE
        , m_bDataValid( false )
#endif    
    {}
    ~TVConverterData2()
    {
        m_svcExp.Close();
        m_svcProv.Close();
    }

public:
    void Close()
    {
        m_svcExp.Close();
        m_svcProv.Close();
    }

    void RunProvider( VO< IVDataProv< TEXP > > & usr )
    {
        m_svcProv.RunNew( usr , *this );
    }

    template< typename ... TARGs >
    bool Update( const TARGs & ... args )
    {
        return _call( &TIMP::Build , m_instCnvtr , m_expValue , args ... );
    }

    virtual void SignalDirty() = 0;

#ifdef VD_NOT_CARE_PERFORMANCE
    void Validate( bool bv )
    {
        m_bDataValid = bv;
    }
    bool m_bDataValid;
#endif

private:
    typedef TVConverterData2< TIMP , TEXP > my_type;
    struct ProvAdp : public IVDataProv< TEXP >
    {
        my_type & me;
        ProvAdp( my_type & m ) :me( m )
        {}
        ~ProvAdp()
        {}
        void Apply( VO< TVConstRef< TEXP > > & usr )
        {
            me._applyExpData( usr );
        }
    };

    void _applyExpData( VO< TVConstRef< TEXP > > & usr )
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        VASSERT( m_bDataValid );
        if ( m_bDataValid )
        {
            m_svcExp.RunNew( usr , m_expValue );
        }
#else
        m_svcExp.RunNew( usr , m_expValue );
#endif
    }

private:
    template< typename ... TARGs >
    bool _call( bool ( TIMP::* f )( TEXP & expVal , const TARGs & ... ) , TIMP & cnvtr , TEXP & expVal , const TARGs & ... args )
    {
        return ( cnvtr.*f )( expVal , args ... );
    }

    template< typename ... TARGs >
    bool _call( void ( TIMP::* f )( TEXP & expVal , const TARGs & ... ) , TIMP & cnvtr , TEXP & expVal , const TARGs & ... args )
    {
        ( cnvtr.*f )( expVal , args ... );
        return true;
    }

    TIMP m_instCnvtr;
    TEXP m_expValue;
    TVServiceArray< TVConstRef< TEXP > >               m_svcExp;
    TVServiceArray< IVDataProv< TEXP > , ProvAdp     > m_svcProv;
};

template< typename TIMP >
class TVConverterData2< TIMP , void >
{
public:
    template< typename ... TEnvs >
    TVConverterData2( TEnvs & ... envs )
        : m_instCnvtr( envs ... )
#ifdef VD_NOT_CARE_PERFORMANCE
        , m_bDataValid( false )
#endif    
    {}

    ~TVConverterData2()
    { 
    }

public:
    void Close(){}

    void RunProvider( VO< IVDataProv< void > > & usr ){}

    template< typename ... TARGs >
    bool Update( const TARGs & ... args )
    {
        return _call( &TIMP::Build , m_instCnvtr , args ... );
    }

    virtual void SignalDirty() = 0;

#ifdef VD_NOT_CARE_PERFORMANCE
    void Validate( bool bv )
    {
        m_bDataValid = bv;
    }
    bool m_bDataValid;
#endif

private:
    template< typename ... TARGs >
    bool _call( bool ( TIMP::* f )( const TARGs & ... ) , TIMP & cnvtr , const TARGs & ... args )
    {
        return ( cnvtr.*f )( args ... );
    }

    template< typename ... TARGs >
    bool _call( void ( TIMP::* f )( const TARGs & ... ) , TIMP & cnvtr , const TARGs & ... args )
    {
        ( cnvtr.*f )( args ... );
        return true;
    }
    TIMP m_instCnvtr;
};

template< typename TIMP , typename TEXP , typename ... TINPUTs > struct TVConverterClntArg2;

template< typename TIMP , typename TEXP > struct TVConverterClntArg2< TIMP , TEXP >
{
    TVConverterClntArg2()
    {}
    ~TVConverterClntArg2()
    {}

    template< typename ... TARGs >
    bool Update( TVConverterData2< TIMP , TEXP > & expVal , const TARGs & ... args )
    {
        return expVal.Update( args ... );
    }
};

template< typename TIMP , typename TEXP , typename TI , typename ... TOTHERs > struct TVConverterClntArg2< TIMP , TEXP , TI , TOTHERs ... >
{
    typedef TVConverterClntArg2< TIMP , TEXP , TOTHERs ... > sub_type;
    TVConstRef< TI > & m_refArg;
    sub_type           m_subValue;

    TVConverterClntArg2( TVConstRef< TI > & rt , TVConstRef< TOTHERs > & ... rothers )
        : m_refArg( rt )
        , m_subValue( rothers ... )
    {};

    template< typename ... TARGs >
    bool Update( TVConverterData2< TIMP , TEXP > & expVal , const TARGs & ... args )
    {
        return m_subValue.template Update< TARGs ... , TI >( expVal , args ... , m_refArg.m_ref );
    }
};

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverterClnt2
{
public:
    typedef TVConverterData2< TIMP , TEXP > data_type;
    typedef TVRef< data_type >              data_ref;

    template< typename FU >
    TVConverterClnt2( data_ref & refData , TVConstRef< TINPUTs > & ... spcArgs , FU f )
        : m_refArgs( spcArgs ... )
        , m_refData( refData )
    {
        m_svcSelf.Run( f , *this );
    }
    ~TVConverterClnt2()
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        m_refData.m_ref.Validate( false );
#endif
        m_svcSelf.Close();
    }

public:
    typedef TVConverterClnt2< TIMP , TEXP , TINPUTs ... > my_type;
    typedef TVRef< my_type >                              my_ref;

    bool UpdateData()
    {
#ifdef VD_NOT_CARE_PERFORMANCE
        m_refData.m_ref.Validate( true );
#endif
        return m_refArgs.template Update<>( m_refData.m_ref );
    }

private:
    data_ref                                        & m_refData;
    TVService< my_ref >                               m_svcSelf;
    TVConverterClntArg2< TIMP , TEXP , TINPUTs ... >  m_refArgs;
};

template< typename TIMP , typename TEXP , typename ... TINPUTs > class TVConverterEntry2;

template< typename TIMP , typename TEXP >
class TVConverterEntry2< TIMP , TEXP >
{
public:
    typedef TVConverterData2< TIMP , TEXP > data_type;
    typedef TVRef< data_type >              data_ref;

    TVConverterEntry2( TVConverterData2< TIMP , TEXP > & tr )
        : m_notifier( tr )
    {}
    ~TVConverterEntry2(){}
    void Close()
    {
        m_svcData.Close();
    }
    void OnChanged()
    {
        m_notifier.SignalDirty();
    }
    template< class F , typename ... SPCs >
    void AccessData( F f , SPCs & ... spcs )
    {
        m_svcData.Run( [ f , &spcs ... ]( VI< data_ref > & spcData ) {
            f( spcData , spcs ... );
        } , m_notifier );
    }

private:
    TVConverterData2< TIMP , TEXP > & m_notifier;
    TVService< TVRef< TVConverterData2< TIMP , TEXP > > > m_svcData;
};

template< typename TIMP , typename TEXP , typename TI , typename ... TIOTHERs >
class TVConverterEntry2< TIMP , TEXP , TI , TIOTHERs ...  > : TVServer< IVTracer >
{
public:
    TVConverterEntry2( TVConverterData2< TIMP , TEXP > & tr , IVSlot< TI > & slot , IVSlot< TIOTHERs > & ... otherSlots )
        :m_subInputs( tr , otherSlots ... )
    {
        TVServer< IVTracer >::Run( [ &slot ]( auto & spc ) {
            slot.Trace( spc );
        } );

        slot.GetData( VD_P2U( m_prov ) );
    }

    ~TVConverterEntry2()
    {
        TVServer< IVTracer >::Close();
    }

    void Close()
    {
        m_subInputs.Close();
    }

public:
    virtual void OnChanged()
    {
        m_subInputs.OnChanged();
    }

public:
    template< class F , typename ... SPCs >
    void AccessData( F f , SPCs & ... spcs )
    {
        m_prov.UseServer( [ this , f , &spcs ... ]( auto & prov ){
            prov.Apply( VD_L2U( [ this , f , &spcs ... ]( VI< TVConstRef< TI > > & spcVal ){
                m_subInputs.AccessData( f , spcs ... , spcVal );
            } ) );
        } );
    }

private:
    typedef TVConverterEntry2< TIMP , TEXP , TIOTHERs ... > sub_type;
    sub_type m_subInputs;
    TVServiceProxy< IVDataProv< TI > > m_prov;
};

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverterTracer2
{
public:
    template< class FU >
    TVConverterTracer2( TVConverterData2< TIMP , TEXP > & tr , FU f , IVSlot< TINPUTs > & ... slots )
        : m_inputs( tr , slots ... )
    {
        m_svcSelf.Run( f , *this );
    }
    ~TVConverterTracer2()
    {
        m_inputs.Close();
        m_svcSelf.Close();
    }

public:
    bool UpdateData()
    {
        _check_and_build_data_prov();

        bool bRtn( false );

        m_proxyClnt.UseServer( [ &bRtn ]( auto & c ) {
            bRtn = c.m_ref.UpdateData();
        } );

        return bRtn;
    }

private:
    typedef TVConverterData2< TIMP , TEXP > data_type;
    typedef TVRef< data_type >              data_ref;
    void _check_and_build_data_prov()
    {
        if ( m_proxyClnt.IsClosed() )
        {
            m_inputs.AccessData( [ this ]( VI< data_ref > & spcVal , VI< TVConstRef< TINPUTs > > & ... spcData ) {
                m_clntData.Create( spcVal , spcData ... , VD_MP2L( m_proxyClnt ) );
            } );
        }
    }

private:
    typedef TVConverterEntry2< TIMP , TEXP , TINPUTs ... >          inputs_type;
    typedef TVConverterClnt2< TIMP , TEXP , TINPUTs ... >           clnt_type;
    typedef TVRef< clnt_type >                                      clnt_ref;
    typedef TVConverterTracer2< TIMP , TEXP , TINPUTs ... >         my_type;
    typedef TVRef< my_type >                                        my_ref;

    inputs_type                                                   m_inputs;
    TVClient< clnt_type , data_ref , TVConstRef< TINPUTs > ... > m_clntData;
    TVServiceProxy< clnt_ref >                                    m_proxyClnt;
    TVService< my_ref >                                           m_svcSelf;
};

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVConverter2 : public IVConverter< TEXP , TINPUTs ... >
    , TVServer< IVDirtyObject >
    , TVServer< TVConverterData2< TIMP , TEXP > >

    , IVSlot< TEXP >
{
private:
    typedef TVConverterData2< TIMP , TEXP >                     data_type;
    typedef TVConverterTracer2< TIMP , TEXP , TINPUTs ... >     data_clnt_type;

public:
    template< typename ... TEnvs >
    TVConverter2( VN_ELEMSYS::TVSysImpTimeline & tl , TEnvs & ... envs )
        : m_timeline( tl )
        , TVServer< TVConverterData2< TIMP , TEXP > >( envs ... )
    {
        _registerDirty();
    }
    ~TVConverter2()
    {
        data_type::Close();
        TVServer< IVDirtyObject >::Close();
        TVServer< data_type >::Close();
    }

public:
    virtual void Input( IVSlot< TINPUTs > & ... slots )
    {
        auto runner = [ this , &slots... ]( auto & spcTracer ){
            m_obsvr.Create( spcTracer , VD_MP2L( m_proxyObsvr ) , slots ... );
        };

        TVServer< data_type >::Run( runner );
    }
    virtual void Output( typename TVOutputPortParam< TEXP >::type & ip )
    {
        ip.Input( *this );
    }

private:
    virtual void Trace( VI< IVTracer > & spc )
    {
        m_listeners.Add( spc );
    }
    virtual void GetData( VO< IVDataProv< TEXP > > & usr )
    {
        data_type::RunProvider( usr );
    }
    virtual void SignalDirty()
    {
        if ( TVServer< IVDirtyObject >::IsClosed() )
        {
            _registerDirty();
        }
    }
    virtual void CleanAndDiffuse()
    {
        bool bInfect( false );

        m_proxyObsvr.UseServer( [ this , &bInfect ]( auto&t ) {
            bInfect = t.m_ref.UpdateData();
        } );

        if ( bInfect )
            m_listeners.TidyTravel( []( auto&t ) {t.OnChanged(); } );
    }

private:
    void _registerDirty()
    {
        TVServer< IVDirtyObject >::Run( [ this ]( auto & spcDirty ) {
            m_timeline.RegisterDirty( spcDirty );
        } );
    }

private:
    VN_ELEMSYS::TVSysImpTimeline                     & m_timeline;
    TVServiceProxyArray< IVTracer >                    m_listeners;
    TVClient< data_clnt_type , data_type   >           m_obsvr;
    TVServiceProxy< TVRef< data_clnt_type > >          m_proxyObsvr;
};

template< typename TIMP , typename TEXP , typename ... TINPUTs >
class TVSvcConverter2
{
public:
    TVSvcConverter2(){}
    ~TVSvcConverter2(){}

public:
    typedef TEXP                                     exp_type;
    typedef IVConverter< TEXP , TINPUTs ...        > interf_type;
    typedef TVConverter2< TIMP , TEXP , TINPUTs ... > imp_type;

public:
    void Close()
    {
        m_svc.Close();
    }
    void Elapse()
    {
        m_timeline.ClearDirty();
    }

    template< typename ... TEnvs >
    void RunService( VO< interf_type > & pipe , TEnvs & ... envs )
    {
        m_svc.RunNew( pipe , m_timeline , envs ... );
    }
        
    template< typename T >
    void Init( const T & v )
    {
    }

private:
    VN_ELEMSYS::TVSysImpTimeline m_timeline;
    TVServiceArray< interf_type , imp_type > m_svc;
};

template< typename T , T > struct TVConverterTraitBase2;

template< typename TIMP , typename R , typename TOUTPUT , typename ... TINPUTs , R( TIMP::*mf )( TOUTPUT & , const TINPUTs & ... ) >
struct TVConverterTraitBase2< R( TIMP::* )( TOUTPUT & , const TINPUTs & ... ) , mf >
{
    typedef TVSvcConverter2< TIMP , TOUTPUT , TINPUTs ... >  server_type;
    typedef IVConverter< TOUTPUT , TINPUTs ... >             node_type;
};

template< typename TIMP , typename R , typename ... TINPUTs , R( TIMP::*mf )( const TINPUTs & ... ) >
struct TVConverterTraitBase2< R( TIMP::* )( const TINPUTs & ... ) , mf >
{
    typedef TVSvcConverter2< TIMP , void , TINPUTs ... > server_type;
    typedef IVConverter< void , TINPUTs ... >            node_type;
};

//
//template< typename TIMP >
//using TVConverterTrait2 = TVConverterTraitBase2< decltype( &TIMP::Build ) , &TIMP::Build >;

template< typename ... T > struct TVConverterTrait2 ;

template< typename TIMP , typename ... TRDEPIDs >
struct TVConverterTrait2< TIMP , TRDEPIDs ... >  
     : TVConverterTraitBase2< decltype( &TIMP::Build ) , &TIMP::Build > 
{} ;
 

template< typename TRID , typename ... TRDEPIDs >
struct TVConverterTrait2< IVRLTN< TRID > , TRDEPIDs ... > : TVConverterTrait2< TVRelationNodeServer< IVRLTN< TRID > , TRDEPIDs ... > >
{} ;

#define VDDP_NOD2( name , ... )  VDDP_NODE_IMP2( name , TVConverterTrait2< __VA_ARGS__ > ) 

/////////////////////////////////////////////////////////////

// templates of simple system implementation

////////////////////////////////////////////////////////////
template< typename TR , typename TIMP  , typename ... TDEPs > class TVRSimpPipeLine ;

template< typename TID , typename TIMP  , typename ... TDEPs > 
class TVRSimpPipeLine< IVRLTN< TID > , TIMP , TDEPs ... >
    : public TVRSimpPipeLine< typename IVRLTN< TID >::HUB , TIMP , TDEPs ... > 
{
public :
    using TVRSimpPipeLine< typename IVRLTN< TID >::HUB , TIMP , TDEPs ... >::TVRSimpPipeLine ;
} ;

template< typename TID , typename TOUT , typename ... TINPUTs , typename TIMP , typename ... TDEPs >
class TVRSimpPipeLine< IVRUniqHub< TID , TOUT , TINPUTs ... > , TIMP , TDEPs ... >
{
public :
    typedef TVPipelineArg< TDEPs ... > DEPENDENCE ;
    TVRSimpPipeLine( const TDEPs & ... deps )
        : m_args( deps ... )
    {
    }
    ~TVRSimpPipeLine()
    {
    }
public :
    void Create( VO< IVConverter< TOUT , TINPUTs ... > > & usr )
    { 
        //m_sysPipe.RunService( usr , m_args  ) ;
        m_args.GetSome( [ this , &usr ]( const TDEPs & ... deps ){
            m_sysPipe.RunService( usr , deps ... ) ;
        } ) ;
    }
public :
    void Elapse()
    {
        m_sysPipe.Elapse() ;
    }

private :
    TVSvcConverter2< TIMP , TOUT , TINPUTs ... > m_sysPipe ;
    TVPipelineArg< TDEPs ... >  m_args ;
} ;

template< typename TID , typename TOUT , typename ... TINPUTs , typename TIMP >
class TVRSimpPipeLine< IVRUniqHub< TID , TOUT , TINPUTs ... > , TIMP >
{
public :
    typedef TVPipelineArg<> DEPENDENCE ;

    TVRSimpPipeLine( ) 
    {
    }
    ~TVRSimpPipeLine()
    {
    }
public :
    void Create( VO< IVConverter< TOUT , TINPUTs ... > > & usr )
    { 
        m_sysPipe.RunService( usr ) ;
        //m_sysPipe.RunService( usr , m_args  ) ;
        //m_args.GetSome( [ this , &usr ]( const TDEPs & ... deps ){
        //    m_sysPipe.RunService( usr , deps ... ) ;
        //} ) ;
    }
public :
    void Elapse()
    {
        m_sysPipe.Elapse() ;
    }

private :
    TVSvcConverter2< TIMP , TOUT , TINPUTs ... > m_sysPipe ; 
} ;

template< typename TID , typename TOUT , typename ... TINPUTs , typename TIMP , typename ... TDEPs >
class TVRSimpPipeLine< IVRUniqHub< TID , VI_AND< TOUT > , TINPUTs ... > , TIMP , TDEPs ... >
    : public TVRSimpPipeLine< IVRUniqHub< TID , TOUT , TINPUTs ... > , TIMP , TDEPs ... >
{
public :
    using TVRSimpPipeLine< IVRUniqHub< TID , TOUT , TINPUTs ... > , TIMP , TDEPs ... >::TVRSimpPipeLine ;
} ;

template< typename TIMP , typename TPRT , typename ... TDEPs > class TVRSimpManager ;

template< typename TIMP , typename TID , typename TOUT , typename ... TI  , typename ... TDEPs > 
class TVRSimpManager< TIMP , IVRUniqHub< TID , TOUT , TI ... >  , TDEPs ... > 
{
public :
    TVRSimpManager( TVRSimpPipeLine< IVRLTN< TID > , TIMP , TDEPs ... > & pl , IVSlot< TI > & ... locs  , IVInputPort< TOUT > & inp )
    { 
        pl.Create( m_node ) ;
        VLNK( inp , m_node ) ;
        VLNK( m_node , locs ... ) ;
    }

private :
    TVConverterWrapper< TOUT , TI ... > m_node ; 
} ;

template< typename TIMP , typename TID , typename TOUT , typename ... TI  , typename ... TDEPs > 
class TVRSimpManager< TIMP , IVRUniqHub< TID , VI_AND<TOUT> , TI ... >  , TDEPs ... > 
    : TVRSimpManager< TIMP , IVRUniqHub< TID , TOUT , TI ... >  , TDEPs ... >
{
public :
    using TVRSimpManager< TIMP , IVRUniqHub< TID , TOUT , TI ... >  , TDEPs ... >::TVRSimpManager ;
} ;

template< typename TIMP , typename TID , typename ... TI  , typename ... TDEPs > 
class TVRSimpManager< TIMP , IVRUniqHub< TID , void , TI ... >  , TDEPs ... > 
{
public :
    TVRSimpManager( TVRSimpPipeLine< IVRLTN< TID > , TIMP , TDEPs ... > & pl , IVSlot< TI > & ... locs )
    { 
        pl.Create( m_node ) ;
        VLNK( m_node , locs ... ) ;
    }

private :
    TVConverterWrapper< void , TI ... > m_node ; 
} ;

template< class TRROOT , class TIMP , typename ... TDEP >
struct TVRSimpSysTrait
{
    typedef IVSysDynamic< typename TRROOT > SYSTYPE ;
    typedef TVRSimpPipeLine< TRROOT , TIMP , TDEP ... >  PIPELINE ;

    template< typename TR > struct TRTrait
    {
        typedef TVRSimpManager< TIMP , typename TRROOT::HUB , TDEP ... > mngr_type ;
    };
} ;



template< class TRROOT , class TIMP >
using TVRSimpSystem = TVDynamicSystemImp2< TVRSimpSysTrait< TRROOT , TIMP > , IVSysDynamic< TRROOT > > ;

/////////////////////////////////////////////////////////////
