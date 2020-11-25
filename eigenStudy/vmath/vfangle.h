#pragma once

#include "vfdef.h"

class VFDegree ;

class VFRadian
{
    float mRad;

public:
    explicit VFRadian ( float r=0 ) : mRad(r) {}
    VFRadian ( const VFDegree & d );
    VFRadian& operator = ( const float& f ) { mRad = f; return *this; }
    VFRadian& operator = ( const VFRadian& r ) { mRad = r.mRad; return *this; }
    VFRadian& operator = ( const VFDegree& d );

    float valueDegrees() const; // see bottom of this file
    float valueRadians() const { return mRad; }
    float valueAngleUnits() const;

    const VFRadian& operator + () const { return *this; }
    VFRadian operator + ( const VFRadian& r ) const { return VFRadian ( mRad + r.mRad ); }
    VFRadian operator + ( const VFDegree& d ) const;
    VFRadian& operator += ( const VFRadian& r ) { mRad += r.mRad; return *this; }
    VFRadian& operator += ( const VFDegree& d );
    VFRadian operator - () const { return VFRadian(-mRad); }
    VFRadian operator - ( const VFRadian& r ) const { return VFRadian ( mRad - r.mRad ); }
    VFRadian operator - ( const VFDegree& d ) const;
    VFRadian& operator -= ( const VFRadian& r ) { mRad -= r.mRad; return *this; }
    VFRadian& operator -= ( const VFDegree& d );
    VFRadian operator * ( float f ) const { return VFRadian ( mRad * f ); }
    VFRadian operator * ( const VFRadian& f ) const { return VFRadian ( mRad * f.mRad ); }
    VFRadian& operator *= ( float f ) { mRad *= f; return *this; }
    VFRadian operator / ( float f ) const { return VFRadian ( mRad / f ); }
    VFRadian& operator /= ( float f ) { mRad /= f; return *this; }

    bool operator <  ( const VFRadian& r ) const { return mRad <  r.mRad; }
    bool operator <= ( const VFRadian& r ) const { return mRad <= r.mRad; }
    bool operator == ( const VFRadian& r ) const { return mRad == r.mRad; }
    bool operator != ( const VFRadian& r ) const { return mRad != r.mRad; }
    bool operator >= ( const VFRadian& r ) const { return mRad >= r.mRad; }
    bool operator >  ( const VFRadian& r ) const { return mRad >  r.mRad; } 
};

/** Wrapper class which indicates a given angle value is in Degrees.
@remarks
VFDegree values are interchangeable with VFRadian values, and conversions
will be done automatically between them.
*/
class VFDegree
{
    float mDeg; // if you get an error here - make sure to define/typedef 'float' first

public:
    explicit VFDegree ( float d=0 ) : mDeg(d) {}
    VFDegree ( const VFRadian& r ) : mDeg(r.valueDegrees()) {}
    VFDegree& operator = ( const float& f ) { mDeg = f; return *this; }
    VFDegree& operator = ( const VFDegree& d ) { mDeg = d.mDeg; return *this; }
    VFDegree& operator = ( const VFRadian& r ) { mDeg = r.valueDegrees(); return *this; }

    float valueDegrees() const { return mDeg; }
    float valueRadians() const; // see bottom of this file
    float valueAngleUnits() const;

    const VFDegree& operator + () const { return *this; }
    VFDegree operator + ( const VFDegree& d ) const { return VFDegree ( mDeg + d.mDeg ); }
    VFDegree operator + ( const VFRadian& r ) const { return VFDegree ( mDeg + r.valueDegrees() ); }
    VFDegree& operator += ( const VFDegree& d ) { mDeg += d.mDeg; return *this; }
    VFDegree& operator += ( const VFRadian& r ) { mDeg += r.valueDegrees(); return *this; }
    VFDegree operator - () const { return VFDegree(-mDeg); }
    VFDegree operator - ( const VFDegree& d ) const { return VFDegree ( mDeg - d.mDeg ); }
    VFDegree operator - ( const VFRadian& r ) const { return VFDegree ( mDeg - r.valueDegrees() ); }
    VFDegree& operator -= ( const VFDegree& d ) { mDeg -= d.mDeg; return *this; }
    VFDegree& operator -= ( const VFRadian& r ) { mDeg -= r.valueDegrees(); return *this; }
    VFDegree operator * ( float f ) const { return VFDegree ( mDeg * f ); }
    VFDegree operator * ( const VFDegree& f ) const { return VFDegree ( mDeg * f.mDeg ); }
    VFDegree& operator *= ( float f ) { mDeg *= f; return *this; }
    VFDegree operator / ( float f ) const { return VFDegree ( mDeg / f ); }
    VFDegree& operator /= ( float f ) { mDeg /= f; return *this; }

    bool operator <  ( const VFDegree& d ) const { return mDeg <  d.mDeg; }
    bool operator <= ( const VFDegree& d ) const { return mDeg <= d.mDeg; }
    bool operator == ( const VFDegree& d ) const { return mDeg == d.mDeg; }
    bool operator != ( const VFDegree& d ) const { return mDeg != d.mDeg; }
    bool operator >= ( const VFDegree& d ) const { return mDeg >= d.mDeg; }
    bool operator >  ( const VFDegree& d ) const { return mDeg >  d.mDeg; }
};

/** Wrapper class which identifies a value as the currently default angle 
type, as defined by Math::setAngleUnit.
@remarks
VFAngle values will be automatically converted between radians and degrees,
as appropriate.
*/
class VFAngle
{
    float mAngle;
public:
    explicit VFAngle ( float angle ) : mAngle(angle) {}
    operator VFRadian() const;
    operator VFDegree() const;
};

// these functions could not be defined within the class definition of class
// VFRadian because they required class VFDegree to be defined
inline VFRadian::VFRadian ( const VFDegree& d ) : mRad(d.valueRadians()) {
}
inline VFRadian& VFRadian::operator = ( const VFDegree& d ) {
    mRad = d.valueRadians(); return *this;
}
inline VFRadian VFRadian::operator + ( const VFDegree& d ) const {
    return VFRadian ( mRad + d.valueRadians() );
}
inline VFRadian& VFRadian::operator += ( const VFDegree& d ) {
    mRad += d.valueRadians();
    return *this;
}
inline VFRadian VFRadian::operator - ( const VFDegree& d ) const {
    return VFRadian ( mRad - d.valueRadians() );
}
inline VFRadian& VFRadian::operator -= ( const VFDegree& d ) {
    mRad -= d.valueRadians();
    return *this;
} 
inline float VFRadian::valueDegrees() const
{
    return mRad * ( 180.f / VF_PI ) ; 
}

inline float VFDegree::valueRadians() const
{
    return mDeg * ( VF_PI / 180.f ) ;
} 

inline VFRadian operator * ( float a, const VFRadian& b )
{
    return VFRadian ( a * b.valueRadians() );
}

inline VFRadian operator / ( float a, const VFRadian& b )
{
    return VFRadian ( a / b.valueRadians() );
}

inline VFDegree operator * ( float a, const VFDegree& b )
{
    return VFDegree ( a * b.valueDegrees() );
}

inline VFDegree operator / ( float a, const VFDegree& b )
{
    return VFDegree ( a / b.valueDegrees() );
}

inline float Cos ( const VFRadian& fValue ) {
    return  cos( fValue.valueRadians() ) ;
}

inline float Sin ( const VFRadian& fValue ) {
    return  sin( fValue.valueRadians() ) ;
}

inline VFRadian ATan2 ( float fY, float fX) { 
    return VFRadian( atan2( fY , fX ) ) ; 
}