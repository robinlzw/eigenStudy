#pragma once

#include<iosfwd>
#include<math.h>
#include<iostream>

class Complex
{
public:
	Complex(float a = 0, float b = 0) : im(a), re(b) {}  // constructs number a+bi

	void operator += (const Complex & z);  // add z

	void operator -= (const Complex & z); // subtract z

	void operator *= (const Complex & z);  // Complex multiply by z

	void operator *= (float r);             // scalar multiply by r

	void operator /= (float r);             // scalar divide by r

	void operator /= (const Complex & z);   // complex divide by z

	Complex operator - (void) const;            // returns the additive inverse

	Complex conj(void) const;            // returns Complex conjugate

	Complex inv(void) const;              // returns inverse

	float arg(void)  const;             // returns argument

	float norm(void) const;             // returns norm

	float norm2(void) const;             // returns norm squared

	Complex unit(void) const;             // returns complex number with unit norm and same modulus

	Complex exponential(void) const;   // complex exponentiation

	float dot(const Complex & z1, const Complex & z2);    // inner product

	float cross(const Complex & z1, const Complex & z2);    // cross product

	friend Complex operator - (const Complex & z1, const Complex & z2);   // binary subtraction

	friend Complex operator * (const Complex & z1, const Complex & z2);   // binary Complex multiplication

	friend Complex operator * (const Complex & z, float r);		// right scalar multiplication

	friend Complex operator * (float r, const Complex & z);     // left scalar multiplication

	friend Complex operator / (const Complex & z, float r);     // scalar division

	friend Complex operator / (const Complex & z1, const Complex & z2);   // complex division

	friend std::ostream & operator << (std::ostream & os, const Complex & z);     // prints components

	float re;          // real part
	float im;          // imaginary part
};
