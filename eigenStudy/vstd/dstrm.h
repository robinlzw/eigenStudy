#pragma once

#include "strm.h"
#include "vstring.h"
#include <vector>

class VDataInput
{
public:
    VDataInput( IVInputStream & s ) ; 
    ~VDataInput();

    unsigned long   Read32()     ;
    unsigned short  Read16()     ;
    unsigned char   Read8()      ;
    float           ReadFloat()  ;
    double          ReadDouble() ;
    void     ReadString( std::string& s ) ;
    void     ReadWString( std::wstring& ws );
    
    template< typename VT , int sz> void ReadArray( VT (&Dest)[sz] ) ;

    void Read32(unsigned long   * buffer, size_t size );
    void Read32(unsigned int    * buffer, size_t size );
    void Read16(unsigned short  * buffer, size_t size );
    void Read8(unsigned char    * buffer, size_t size );
    void ReadFloat(float        * buffer, size_t size );
    void ReadDouble(double      * buffer, size_t size );
    void ReadPersist( IVPersist & p ) ;

    VDataInput & operator >> ( std::string & s   ) ;
	VDataInput & operator >> ( std::wstring & s   ) ;
    VDataInput & operator >> ( bool & c          ) ;
    VDataInput & operator >> ( char & c          ) ;
    VDataInput & operator >> ( short& i          ) ;
    VDataInput & operator >> ( long& i           ) ;
    VDataInput & operator >> ( int& i            ) ;
    VDataInput & operator >> ( unsigned char& c  ) ;
    VDataInput & operator >> ( unsigned short& i ) ;
    VDataInput & operator >> ( unsigned long& i  ) ;
    VDataInput & operator >> ( unsigned int& i   ) ;
    VDataInput & operator >> ( float        & i  ) ;
    VDataInput & operator >> ( double       & i  ) ;


protected:
    IVInputStream * m_input ;
};

class VDataOutput
{
public:
    VDataOutput(IVOutputStream& s) ;
    ~VDataOutput() ;

    void Write32(unsigned long i);
    void Write16(unsigned short i);
    void Write8(unsigned char i);
    void WriteFloat(float d);
    void WriteDouble(double d);
    void WriteString(const std::string& string);
    void WriteWString(const std::wstring& wstring);
    
    template< typename VT , int sz> void WriteArray( VT (&Dest)[sz] ) ;

    void Write32( const unsigned long *buffer, size_t size);
    void Write32( const unsigned int  *buffer, size_t size );
    void Write16(const unsigned short *buffer, size_t size);
    void Write8(const unsigned char *buffer, size_t size);
    void WriteFloat(const float *buffer, size_t size);
    void WriteDouble(const double *buffer, size_t size);
    void WritePersist( IVPersist & p ) ;

    VDataOutput& operator << (const std::string& string);
	VDataOutput& operator << (const std::wstring& string);
    VDataOutput& operator << (bool c  ) ;
    VDataOutput& operator << (char c);
    VDataOutput& operator << (short i);
    VDataOutput& operator << (long i);
    VDataOutput& operator << (int i);
    VDataOutput& operator << (unsigned char c) ;
    VDataOutput& operator << (unsigned short i);
    VDataOutput& operator << (unsigned long i) ; 
    VDataOutput& operator << (unsigned int i) ; 
    VDataOutput& operator << (double f);
    VDataOutput& operator << (float f);

protected:
    IVOutputStream *m_output ; 
};

template< typename VT , int sz> 
inline 
    void VDataInput::ReadArray( VT (&Dest)[sz] )
{
    m_input->Read( Dest , sz * sizeof( VT ) ) ;
}

template< typename VT , int sz> 
inline 
    void VDataOutput::WriteArray( VT (&Dest)[sz] )
{
    m_output->Write( Dest , sz * sizeof( VT ) ) ;
}

template< class T >
inline
VDataOutput & operator <<( VDataOutput & stm , const std::vector< T > & data )
{
    unsigned count = data.size() ;

    stm << count ;

    for( unsigned i = 0 ; i < count ; i ++ )
    {
        stm << data[i] ;
    }

    return stm ;
}



template< class T >
inline
VDataInput & operator >>( VDataInput & stm , std::vector< T > & data )
{
    unsigned count ;

    stm >> count ;
    data.resize( count ) ;

    for( unsigned i = 0 ; i < count ; i ++ )
    {
        stm >> data[i] ;
    }

    return stm ;
}

#include "dstrm.inl"
