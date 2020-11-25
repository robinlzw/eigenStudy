#pragma once
#include "vfvector2.h"

struct VFRect
{
	float X;
	float Y;
	float Width;
	float Height;

	VFRect()
	{
		X = Y = Width = Height = 0.0f;
	}

	VFRect(float x,float y,float width,float height)
	{
		X = x;
		Y = y;
		Width = width;
		Height = height;
	}

	VFRect(const VFVector2& location,const VFVector2& size)
	{
		X = location.x;
		Y = location.y;
		Width = size.x;
		Height = size.y;
	}

	VFRect* Clone() const
	{
		return new VFRect(X, Y, Width, Height);
	}

	void GetLocation(VFVector2* pt) const
	{
		pt->x = X;
		pt->y = Y;
	}

	void GetSize(VFVector2* size) const
	{
		size->x = Width;
		size->y = Height;
	}

	void GetBounds(VFRect* rect) const
	{
		rect->X = X;
		rect->Y = Y;
		rect->Width = Width;
		rect->Height = Height;
	}

	float GetLeft() const
	{
		return X;
	}

	float GetTop() const
	{
		return Y;
	}

	float GetRight() const
	{
		return X+Width;
	}

	float GetBottom() const
	{
		return Y+Height;
	}

    VFVector2 GetCenter() const
    {
        return VFVector2( X + Width * 0.5f , Y + Height * 0.5f ) ;
    }

	float IsEmptyArea() const
	{
		return (Width <= VF_EPS_2) || (Height <= VF_EPS_2);
	}

	bool Equals(const VFRect & rect) const
	{
		return X == rect.X &&
			Y == rect.Y &&
			Width == rect.Width &&
			Height == rect.Height;
	}

	bool Contains(float x,float y) const
	{
		return x >= X && x < X+Width &&
			y >= Y && y < Y+Height;
	}

	bool Contains(const VFVector2& pt) const
	{
		return Contains(pt.x, pt.y);
	}

	bool Contains(const VFRect& rect) const
	{
		return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
			(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
	}

	void Inflate(float dx,float dy)
	{
		X -= dx;
		Y -= dy;
		Width += 2*dx;
		Height += 2*dy;
	}

	void Inflate(const VFVector2& pt)
	{
		Inflate(pt.x, pt.y);
	}

	bool Intersect(const VFRect& rect)
	{
		return Intersect(*this, *this, rect);
	}

    void merge( const VFRect a )
    {
		float right  = max(a.GetRight(), GetRight());
		float bottom = max(a.GetBottom(), GetBottom());
		float left   = min(a.GetLeft(), GetLeft());
		float top    = min(a.GetTop(), GetTop());

		X = left;
		Y = top;
		Width = right - left  ;
		Height = bottom - top ;
    }

    void merge( const VFVector2 & pt )
    {
        if( pt.x < X )
        {
            Width += X - pt.x ;
            X = pt.x ;
        }
        else if( pt.x > X + Width )
            Width = pt.x - X ;

        if( pt.y < Y )
        {
            Height += Y - pt.y ;
            Y = pt.y ;
        }
        else if( pt.y > Y + Height )
            Height = pt.y - Y ;
    }

	static bool Intersect(VFRect& c,
		const VFRect& a,
		const VFRect& b)
	{
		float right = min(a.GetRight(), b.GetRight());
		float bottom = min(a.GetBottom(), b.GetBottom());
		float left = max(a.GetLeft(), b.GetLeft());
		float top = max(a.GetTop(), b.GetTop());

		c.X = left;
		c.Y = top;
		c.Width = right - left;
		c.Height = bottom - top;
		return !c.IsEmptyArea();
	}

	bool IntersectsWith(const VFRect& rect) const
	{
		return (GetLeft() < rect.GetRight() &&
			GetTop() < rect.GetBottom() &&
			GetRight() > rect.GetLeft() &&
			GetBottom() > rect.GetTop());
	} 

	static bool Union(VFRect& c,
		const VFRect& a,
		const VFRect& b)
	{
		float right = max(a.GetRight(), b.GetRight());
		float bottom = max(a.GetBottom(), b.GetBottom());
		float left = min(a.GetLeft(), b.GetLeft());
		float top = min(a.GetTop(), b.GetTop());

		c.X = left;
		c.Y = top;
		c.Width = right - left;
		c.Height = bottom - top;
		return !c.IsEmptyArea();
	}

	void Offset(const VFVector2& pt)
	{
		Offset(pt.x, pt.y);
	}

	void Offset(float dx,float dy)
	{
		X += dx;
		Y += dy;
	}
};
 