 
inline
VDataInput::VDataInput(IVInputStream& s)
  : m_input(&s) 
{
}

inline VDataInput::~VDataInput()
{ 
}
 
inline 
unsigned long VDataInput::Read32()
{
  unsigned long i32;

  m_input->Read(&i32, 4);

  return i32 ;
}

inline 
unsigned short VDataInput::Read16()
{
  unsigned short i16;

  m_input->Read(&i16, 2);

  return i16 ;
}

inline 
unsigned char VDataInput::Read8()
{
  unsigned char buf;

  m_input->Read(&buf, 1);

  return (unsigned char)buf;
}

inline 
float VDataInput::ReadFloat()
{ 
    float d ;
    m_input->Read( &d , sizeof(float) ) ;
    return d ;
}

inline 
double VDataInput::ReadDouble()
{ 
    double d ;
    m_input->Read( &d , sizeof(double) ) ;
    return d ;
}

inline 
void VDataInput::ReadString( std::string& s )
{
    const size_t len = Read32() ;

    if( len == 0 )
    {
        s.clear();
    }
    else
    {
		s.resize( len );
        m_input->Read( &s[0] , len ) ;
    }
}

inline
void VDataInput::ReadWString( std::wstring& ws )
{
    const size_t len = Read32() ;

    if ( len == 0 )
    {
		ws.clear();
    }
    else
    {
		ws.resize( len );
        m_input->Read( &ws[0] , sizeof(wchar_t)*len );
    }
}

inline 
void VDataInput::Read32( unsigned long *buffer , size_t size )
{
    m_input->Read( buffer, size * sizeof( unsigned long ) ) ;
}

inline 
void VDataInput::Read32( unsigned int *buffer , size_t size )
{
    m_input->Read( buffer, size * sizeof( unsigned int ) ) ;
}

inline 
void VDataInput::Read16(unsigned short *buffer, size_t size)
{
    m_input->Read( buffer, size * 2 ) ;
}

inline 
void VDataInput::Read8(unsigned char *buffer, size_t size)
{
  m_input->Read(buffer, size);
}

inline 
void VDataInput::ReadFloat( float *buffer, size_t size)
{
  m_input->Read( buffer, size * sizeof( float ) );
}

inline 
void VDataInput::ReadDouble(double *buffer, size_t size)
{
  m_input->Read( buffer, size * sizeof( double ) );
}

inline
    void VDataInput::ReadPersist( IVPersist & p ) 
{
    p.LoadFromStream( * m_input ) ;
}

inline 
VDataInput& VDataInput::operator>>(std::string& s)
{
	ReadString( s );
	return *this;
}

inline
VDataInput & VDataInput::operator >> ( std::wstring & s   ) 
{
	ReadWString( s );
	return *this;
}

inline 
VDataInput& VDataInput::operator>>( bool& c )
{
    if( Read8() )
        c = true ;
    else
        c = false ;

    return *this;
}

inline 
VDataInput& VDataInput::operator>>(char& c)
{
  c = (char)Read8();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(short& i)
{
  i = (short)Read16();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(long& i)
{
  i = (long)Read32();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(int& i)
{
  i = (int)Read32();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(unsigned char& c)
{
  c = Read8();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(unsigned short& i)
{
  i = Read16();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(unsigned long& i)
{
  i = Read32();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(unsigned int& i)
{
  i = ( unsigned int)Read32();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(double& i)
{
  i = ReadDouble();
  return *this;
}

inline 
VDataInput& VDataInput::operator>>(float& f)
{
  f = ReadFloat();
  return *this;
}


//////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// VDataOutput
// ---------------------------------------------------------------------------
 
inline 
VDataOutput::VDataOutput(IVOutputStream& s)
  : m_output(&s) 
{
}

inline 
VDataOutput::~VDataOutput()
{ 
} 

inline 
void VDataOutput::Write32(unsigned long i32 )
{ 
    m_output->Write(&i32, 4);
}

inline 
void VDataOutput::Write16(unsigned short i16 )
{ 
  m_output->Write(&i16, 2);
}

inline 
void VDataOutput::Write8(unsigned char i)
{
  m_output->Write(&i, 1);
}

inline 
void VDataOutput::WriteString(const std::string & str )
{
    const char * buff = str.c_str() ;
    size_t len = strlen( buff );
    
    Write32(len);

    if (len > 0)
        m_output->Write( buff , len );
}

inline
void VDataOutput::WriteWString(const std::wstring& wstr)
{
    const wchar_t * buff = wstr.c_str() ;
    size_t len = wcslen( buff );
    
    Write32(len);
    
    if ( len > 0 )
        m_output->Write( buff , sizeof(wchar_t)*len );
}

inline 
void VDataOutput::WriteFloat( float d)
{ 
   m_output->Write( & d, sizeof( float ) );
}

inline 
void VDataOutput::WriteDouble( double d)
{ 
   m_output->Write( & d, sizeof( double ) );
}
   
inline 
void VDataOutput::Write32( const unsigned long *buffer, size_t size)
{
    m_output->Write( buffer , size * sizeof( unsigned long ) ) ;
}
   
inline 
void VDataOutput::Write32( const unsigned int  *buffer, size_t size)
{
    m_output->Write( buffer , size * sizeof( unsigned int ) ) ;
}

inline 
void VDataOutput::Write16(const unsigned short *buffer, size_t size)
{
    m_output->Write( buffer , size * sizeof( unsigned short ) ) ;
}

inline 
void VDataOutput::Write8(const unsigned char *buffer, size_t size)
{
   m_output->Write( buffer, size ) ;
}

inline 
void VDataOutput::WriteFloat(const float *buffer, size_t size)
{
    m_output->Write( buffer , size * sizeof( float ) ) ;
}

inline 
void VDataOutput::WriteDouble(const double *buffer, size_t size)
{
    m_output->Write( buffer , size * sizeof( double ) ) ;
}

inline 
    void VDataOutput::WritePersist( IVPersist & p ) 
{
    p.SaveToStream( * m_output ) ;
}

inline 
VDataOutput& VDataOutput::operator<<(const std::string& string)
{
      WriteString(string);
      return *this;
}

inline
VDataOutput& VDataOutput::operator << (const std::wstring& string)
{
	WriteWString(string);
	return *this;
}

inline VDataOutput& VDataOutput::operator<<(bool c)
{
    if( c )
        Write8( (unsigned char)1);
    else
        Write8((unsigned char)0 );

    return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(char c)
{
      Write8((unsigned char)c);
      return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(short i)
{
  Write16((unsigned short)i);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(long i)
{
  Write32((unsigned long)i);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(int i)
{
  Write32((unsigned long)i);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(unsigned char c)
{
  Write8(c);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(unsigned short i)
{
  Write16(i);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(unsigned long i)
{
  Write32(i);
  return *this;
}
inline 
VDataOutput& VDataOutput::operator<<(unsigned int i)
{
  Write32((unsigned int)i);
  return *this;
}


inline 
VDataOutput& VDataOutput::operator<<(double f)
{
  WriteDouble(f);
  return *this;
}

inline 
VDataOutput& VDataOutput::operator<<(float f)
{
  WriteFloat( f ) ;

  return *this;
}
