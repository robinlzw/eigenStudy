#pragma once

#include <string>
#ifdef WIN32
#include <tchar.h>
#endif
#include "obj.h"

typedef IVUser< const wchar_t * > IVStringUser ;

//
//VINTERFACE IVStringUser : public IVUser< const wchar_t * > 
//{
//    virtual void            SetContent( const wchar_t * pWStr )  = 0 ;
//    virtual void            FromANSI  ( const char    * pAStr )  = 0 ;
//    virtual void            FromUTF8  ( const char    * pUStr )  = 0 ;
//} ;