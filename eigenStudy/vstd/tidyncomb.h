#pragma once

#include "tidyn.h"

template< class TSys1 , class TSys2 > struct TVSYSBND_OR {} ;

namespace VN_ELEMSYS
{ 
    template< class TSys1 , class TSys2 > 
    struct TVDeduceExport< TVBind< TVSYSBND_OR< TSys1 , TSys2 > > >
    {
        typedef TVBind< TVSYSBND_OR< TSys1 , TSys2 > >  ElemType    ;
        typedef typename TSys1::ExpType                 ExpType     ;  
        typedef typename TSys1::ExpType                 ExpUnitType ;  
    } ;

    template< class TDerived , class TIDElem , class ExpType >
    class TVBindElementOR : public IVElement< TIDElem >
    {
        typedef void ( TDerived::* AdpFunc )( IVExtElement< VIDUnknown > & inval , VO<  IVDynamicNew< TVBind< ExpType > >  >& usr ) ; 
        TDerived & m_owner ;
        TVElement< VIDUnknown > m_element ;
        AdpFunc                fbuild    ;

    public :
        TVBindElementOR( TDerived & me , VI<  IVElement< VIDUnknown >  >& spc , AdpFunc f ) 
            : m_owner( me ) , fbuild( f )
        {
            m_element.Visit( spc ) ;
        }

    private :
        virtual void Participate()
        {
            m_owner.ParticipateTranslation( [ this ]( VO<  IVDynamicNew< TVBind< ExpType > >  >& usr ){
                ( m_owner.*fbuild )( m_element , usr ) ;
            } ) ;
        }
    } ;

    template< class TDerived , class TIDElem , class TSys1 , class TSys2 , class TElement , class TExp > 
    class TVSysImpElement< TDerived , TIDElem , TVSYSBND_OR< TSys1 , TSys2 > , TElement , TExp >
    {
    public :
        typedef TExp ExpType ;
        typedef TDerived DerivedType ;
        typedef TVSysImpElement< TDerived , TIDElem , TVSYSBND_OR< TSys1 , TSys2 > , TElement , TExp > MyType ; 
        typedef TVBindElementOR< TDerived , TIDElem , TExp > Element ; 
        typedef void ( TDerived::* AdpFunc )( IVExtElement< VIDUnknown > & inval , VO<  IVDynamicNew< TVBind< ExpType > >  >& usr ) ;

    public : 
        template< class TChk >
        struct CheckInput
        {
            static const bool value = TSys1::CheckInput< TChk >::value || TSys2::CheckInput< TChk >::value ;
        } ; 

    private :
        TSys1 m_sysFirst   ;
        TSys2 m_sysSecond   ;  
        TVServiceArray< IVElement< TIDElem > , Element > m_svc ;

    private :
        void _export1( IVExtElement< VIDUnknown > & inval , VO<  IVDynamicNew< TVBind< ExpType > >  >& usr )
        { 
            m_sysFirst.Export( inval , usr ) ;
        } 
        void _export2( IVExtElement< VIDUnknown > & inval , VO<  IVDynamicNew< TVBind< ExpType > >  >& usr )
        {
            m_sysSecond.Export( inval , usr ) ;
        } 

        template< class TSysIn , class TEleIn , bool b1 , bool b2 > struct Executor ;
        template< class TSysIn , class TEleIn > struct Executor< TSysIn , TEleIn , true , false >
        { 
            static void Create( MyType & inst , TSysIn & sysInput , TEleIn & inval , VO<  IVElement< TIDElem >  >& outval )
            {
                inst._create1( sysInput , inval , outval ) ; 
            }
        } ;  

        template< class TSysIn , class TEleIn , bool b1 , bool b2 > struct Executor ;
        template< class TSysIn , class TEleIn > struct Executor< TSysIn , TEleIn , false , true >
        { 
            static void Create( MyType & inst , TSysIn & sysInput , TEleIn & inval , VO<  IVElement< TIDElem >  >& outval )
            {
                inst._create2( sysInput , inval , outval ) ; 
            }
        } ;

        template< class TSysIn , class TEleIn >
        void _create1( TSysIn & sysInput , TEleIn & inval , VO<  IVElement< TIDElem >  >& outval )
        {
            m_sysFirst.Create( sysInput , inval , VD_L2U( [ this , &outval ]( VI<  IVElement< VIDUnknown >  >& spc ){
                _create1( spc , outval ) ;
            } ) ) ;
        }

        template< class TSysIn , class TEleIn >
        void _create2( TSysIn & sysInput , TEleIn & inval , VO<  IVElement< TIDElem >  >& outval )
        {
            m_sysSecond.Create( sysInput , inval , VD_L2U( [ this , &outval ]( VI<  IVElement< VIDUnknown >  >& spc ){
                _create2( spc , outval ) ;
            } ) ) ;
        } 

        void _create1( VI<  IVElement< VIDUnknown >  >& spc , VO<  IVElement< TIDElem >  >& outval )
        { 
            m_svc.RunNew( outval , * static_cast< TDerived * >( this ) , spc , &DerivedType::_export1 ) ;
        }

        void _create2( VI<  IVElement< VIDUnknown >  >& spc , VO<  IVElement< TIDElem >  >& outval )
        { 
            m_svc.RunNew( outval , * static_cast< TDerived * >( this ) , spc , &DerivedType::_export2 ) ;
        } 

    public :
        TVSysImpElement()
        {
            static_assert( std::is_same< TSys1::ExpType , TSys2::ExpType >::value , "Is not same export type! " ) ;
        }
        ~TVSysImpElement()
        {
        }

        void Close() 
        {
            m_sysFirst.Close() ;
            m_sysSecond.Close() ;
            m_svc.Close() ;
        }

        template< class TSysIn , class TEleIn >
        void Create( TSysIn & sysInput , TEleIn & inval , VO<  IVElement< TIDElem >  >& outval )
        { 
            Executor< TSysIn , TEleIn , TSys1::CheckInput< TSysIn >::value , TSys2::CheckInput< TSysIn >::value >::Create( *this , sysInput , inval , outval ) ;
        }
        
        void PrepareInput() 
        {
            m_sysFirst.Elapse() ;
            m_sysSecond.Elapse() ; 
        }  
    } ;
} ; 
 