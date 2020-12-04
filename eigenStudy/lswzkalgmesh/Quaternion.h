#pragma once

// Quaternion represents an element of the quaternions, along with all the usual
// vectors space operations (addition, multiplication by scalars, etc.).  The
// Hamilton product is expressed using the * operator:
//
//    Quaternion p, q, r;
//    r = q * p;
//
// and conjugation is expressed using the method Quaternion::conj():
//
//    Quaternion q;
//    double normQSquared = -q.conj()*q;
//
// Individual components can be accessed in several ways: the real and imaginary
// parts can be accessed using the methods Quaternion::re() and Quaternion::im():
//
//   Quaternion q;
//   double a = q.re();
//   Vector b = q.im();
//
// or by index:
//
//   Quaternion q;
//   double a  = q[0];
//   double bi = q[1];
//   double bj = q[2];
//   double bk = q[3];
//

#include <vmath/vfvector3.h>
#include "Complex.h"

class Quaternion
{
public:
	Quaternion();                                   
	// initializes all components to zero

	Quaternion(const Quaternion & p);               
	// initializes from existing quaternion

	Quaternion(float s, float vi, float vj, float vk);   
	// initializes with specified real (s) and imaginary (v) components

	Quaternion(float s, const VFVector3 & v);       
	// initializes with specified real (s) and imaginary (v) components

	Quaternion(float s);                          
	// initializes purely real quaternion with specified real (s) component (imaginary part is zero)

	Quaternion(const VFVector3 & v);               
	// initializes purely imaginary quaternion with specified imaginary (v) component (real part is zero)

	Quaternion(const Complex & z);                 
	// for a complex number z=a+bi, initializes quaternion to a+bi+0j+0k

	const Quaternion & operator = (float _s);       
	// assigns a purely real quaternion with real value s

	const Quaternion & operator = (const VFVector3 & _v);   
	// assigns a purely real quaternion with imaginary value v

	float & operator [] (unsigned idx);              
	// returns reference to the specified component (0-based indexing: r, i, j, k)

	const float & operator [] (unsigned idx) const;  
	// returns const reference to the specified component (0-based indexing: r, i, j, k)

	void toMatrix(float Q[4][4]) const;              
	// builds 4x4 matrix Q representing (left) quaternion multiplication

	float & re(void);
	// returns reference to double part

	const float & re(void) const;
	// returns const reference to double part

	VFVector3 & im(void);
	// returns reference to imaginary part

	const VFVector3 & im(void) const;
	// returns const reference to imaginary part

	Quaternion operator+(const Quaternion& q) const;
	// addition

	Quaternion operator-(const Quaternion& q) const;
	// subtraction

	Quaternion operator-(void) const;
	// negation

	Quaternion operator*(float c) const;
	// right scalar multiplication

	Quaternion operator/(float c) const;
	// scalar division

	void operator+=(const Quaternion& q);
	// addition / assignment

	void operator+=(float c);
	// addition / assignment of pure real

	void operator-=(const Quaternion& q);
	// subtraction / assignment

	void operator-=(float c);
	// subtraction / assignment of pure real

	void operator*=(float c);
	// scalar multiplication / assignment

	void operator/=(float c);
	// scalar division / assignment

	Quaternion operator*(const Quaternion& q) const;
	// Hamilton product

	void operator*=(const Quaternion& q);
	// Hamilton product / assignment

	Quaternion conj(void) const;
	// conjugation

	Quaternion inv(void) const;
	// inverse

	float norm(void) const;
	// returns Euclidean length

	float norm2(void) const;
	// returns Euclidean length squared

	Quaternion unit(void) const;
	// returns unit quaternion

	void normalize(void);
	// divides by Euclidean length

	friend Quaternion operator*(float c, const Quaternion& q);
	// left scalar multiplication

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& q);
	// prints components

protected:
	float s;        // scalar (double) part
	VFVector3 v;    // vector (imaginary) part	
};
