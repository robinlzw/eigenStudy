#include "stdafx.h"
#include"Quaternion.h"

// CONSTRUCTORS ----------------------------------------------------------
Quaternion::Quaternion(void)
	:s(0.f), v(0.f, 0.f, 0.f) {}

Quaternion::Quaternion(const Quaternion & q)
	: s(q.s), v(q.v) {}

Quaternion::Quaternion(float s_, float vi, float vj, float vk)
	: s(s_), v(vi, vj, vk) {}

Quaternion::Quaternion(float s_, const VFVector3 & q)
	: s(s_), v(q) {}

Quaternion::Quaternion(float s_)
	:s(s_), v(0.f, 0.f, 0.f) {}

Quaternion::Quaternion(const VFVector3 & q)
	:s(0.f), v(q) {}

Quaternion::Quaternion(const Complex & z)
	:s(z.re), v(z.im, 0.f, 0.f) {}

const Quaternion & Quaternion::operator=(float _s)
{
	s = _s;
	v = VFVector3(0.f, 0.f, 0.f);

	return *this;
}

const Quaternion & Quaternion::operator=(const VFVector3 & _v)
{
	s = 0.f;
	v = _v;

	return *this;
}


// ACCESSORS -------------------------------------------------------------
float & Quaternion::operator[](unsigned idx)
{
	return ( &s )[ idx ];
}

const float & Quaternion::operator[](unsigned idx) const
{
	return ( &s )[ idx ];
}

void Quaternion::toMatrix(float Q[4][4]) const
{
	Q[0][0] = s; Q[0][1] = -v.x; Q[0][2] = -v.y; Q[0][3] = -v.z;
	Q[1][0] = v.x; Q[1][1] = s; Q[1][2] = -v.z; Q[1][3] = v.y;
	Q[2][0] = v.y; Q[2][1] = v.z; Q[2][2] = s; Q[2][3] = -v.x;
	Q[3][0] = v.z; Q[3][1] = -v.y; Q[3][2] = v.x; Q[3][3] = s;
}

float & Quaternion::re(void)
{
	return s;
}

const float & Quaternion::re(void) const
{
	return s;
}

VFVector3 & Quaternion::im(void)
{
	return v;
}

const VFVector3 & Quaternion::im(void) const
{
	return v;
}

// VECTOR SPACE OPERATIONS -----------------------------------------------
Quaternion Quaternion::operator+(const Quaternion& q) const
{
	return Quaternion(s + q.s, v + q.v);
}

Quaternion Quaternion::operator-(const Quaternion& q) const
{
	return Quaternion(s - q.s, v - q.v);
}

Quaternion Quaternion::operator-(void) const
{
	return Quaternion(-s, -v);
}

Quaternion Quaternion::operator*(float c) const
{
	return Quaternion(s*c, v*c);
}

Quaternion operator*(float c, const Quaternion& q)
{
	return q*c;
}

Quaternion Quaternion::operator/(float c) const
{
	return Quaternion(s / c, v / c);
}

void Quaternion::operator+=(const Quaternion& q)
{
	s += q.s;
	v += q.v;
}

void Quaternion::operator+=(float c)
{
	s += c;
}

void Quaternion::operator-=(const Quaternion& q)
{
	s -= q.s;
	v -= q.v;
}

void Quaternion::operator-=(float c)
{
	s -= c;
}

void Quaternion::operator*=(float c)
{
	s *= c;
	v *= c;
}

void Quaternion::operator/=(float c)
{
	s /= c;
	v /= c;
}

// ALGEBRAIC OPERATIONS --------------------------------------------------
Quaternion Quaternion::operator*(const Quaternion& q) const
{
	const float& s1(s);
	const float& s2(q.s);
	const VFVector3 & v1(v);
	const VFVector3 & v2(q.v);

	return Quaternion(s1*s2 - v1.Dot(v2), s1*v2 + s2*v1 + v1.Cross(v2));
}

void Quaternion::operator*=(const Quaternion& q)
{
	*this = (*this * q);
}

Quaternion Quaternion::conj(void) const
{
	return Quaternion(s, -v);
}

Quaternion Quaternion::inv(void) const
{
	return (this->conj()) / this->norm2();
}

// NORMS -----------------------------------------------------------------
float Quaternion::norm(void) const
{
	return sqrtf(s*s + v.x*v.x + v.y*v.y + v.z*v.z);
}

float Quaternion::norm2(void) const
{
	return s*s + v.Dot(v);
}

Quaternion Quaternion::unit(void) const
{
	return *this / norm();
}

void Quaternion::normalize(void)
{
	*this /= norm();
}

// GEOMETRIC OPERATIONS --------------------------------------------------
Quaternion slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	// interpolate length
	float m0 = q0.norm();
	float m1 = q1.norm();
	float m = (1 - t)*m0 + t*m1;

	// interpolate direction
	Quaternion p0 = q0 / m0;
	Quaternion p1 = q1 / m1;
	float theta = acos((p0.conj()*p1).re());
	Quaternion p = (sin((1 - t)*theta)*p0 + sin(t*theta)*p1) / sin(theta);

	return m*p;
}

// I/O -------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
	os << "( " << q.re() << ", " << q.im() << " )";

	return os;
}
