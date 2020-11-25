#pragma once

#include "vnvector.h"
#include "vfdef.h"

template< class T >
struct VTNRect
{
    T X ;
    T Y ;
    T Width ;
    T Height ;
      
    VTNRect()
    {
        X = Y = Width = Height = 0;
    }

    VTNRect( T x , T y, T width, T height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    VTNRect( const VNVector2< T > & location , const VNVector2< T > & size)
    {
        X = location.x ;
        Y = location.y ;
        Width = size.x ;
        Height = size.y ;
    }

    VTNRect * Clone() const
    {
        return new VTNRect<T>( X, Y, Width, Height ) ;
    }

    VNVector2< T > GetLocation( ) const
    {
        return VNVector2< T >( X , Y ) ;
    }

    VNVector2< T > GetSize() const
    {
        return VNVector2< T >( Width , Height ) ;  
    }

    VTNRect< T > GetBounds() const
    {
        rect->X = X;
        rect->Y = Y;
        rect->Width = Width;
        rect->Height = Height;
    }

    T GetLeft() const
    {
        return X;
    }

    T GetTop() const
    {
        return Y;
    }

    T GetRight() const
    {
        return X+Width;
    }

    T GetBottom() const
    {
        return Y+Height;
    }

    bool IsEmptyArea() const
    {
        return (Width <= 0) || (Height <= 0);
    }

    bool Equals(  const VTNRect< T > & rect) const
    {
        return X == rect.X &&
               Y == rect.Y &&
               Width == rect.Width &&
               Height == rect.Height;
    }

    bool Contains(T x,T y) const
    {
        return x >= X && x < X+Width &&
               y >= Y && y < Y+Height;
    }

    bool Contains(  const VNVector2<T> & pt) const
    {
        return Contains(pt.x, pt.y);
    }

    bool Contains(  VTNRect< T > & rect ) const
    {
        return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
               (Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
    }

	void Inflate(T dx,T dy)
	{
		X -= dx;
		Y -= dy;
		Width += 2*dx;
		Height += 2*dy;
	}

	void Inflate(const VNVector2<T> & pt)
	{
		Inflate(pt.x, pt.y);
	}
	
	bool Intersect(const VTNRect<T>& rect)
	{
		return Intersect(*this, *this, rect);
	}

    void merge( const VTNRect<T>& a )
    {
		int right  = vf_max(a.GetRight(), GetRight());
		int bottom = vf_max(a.GetBottom(), GetBottom());
		int left   = vf_min(a.GetLeft(), GetLeft());
		int top    = vf_min(a.GetTop(), GetTop());

		X = left;
		Y = top;
		Width = right - left  ;
		Height = bottom - top ;
    }

    void merge( const VNVector2<T> & pt )
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

	static bool Intersect(VTNRect<T>& c,
		const VTNRect<T>& a,
		const VTNRect<T>& b)
	{
		int right = vf_min(a.GetRight(), b.GetRight());
		int bottom = vf_min(a.GetBottom(), b.GetBottom());
		int left = vf_max(a.GetLeft(), b.GetLeft());
		int top = vf_max(a.GetTop(), b.GetTop());

		c.X = left;
		c.Y = top;
		c.Width = right - left;
		c.Height = bottom - top;
		return !c.IsEmptyArea();
	}

	bool IntersectsWith(const VTNRect<T>& rect) const
	{
		return (GetLeft() < rect.GetRight() &&
			GetTop() < rect.GetBottom() &&
			GetRight() > rect.GetLeft() &&
			GetBottom() > rect.GetTop());
	}

	static bool Union(VTNRect<T>& c,
		const VTNRect<T>& a,
		const VTNRect<T>& b)
	{
		int right = vf_max(a.GetRight(), b.GetRight());
		int bottom = vf_max(a.GetBottom(), b.GetBottom());
		int left = vf_min(a.GetLeft(), b.GetLeft());
		int top = vf_min(a.GetTop(), b.GetTop());

		c.X = left;
		c.Y = top;
		c.Width = right - left;
		c.Height = bottom - top;
		return !c.IsEmptyArea();
	}

	void Offset(const VNVector2<T> pt)
	{
		Offset(pt.x, pt.y);
	}

	void Offset(T dx,T dy)
	{
		X += dx;
		Y += dy;
	}
} ;

typedef VTNRect< int      > VNRECTI ;
typedef VTNRect< char     > VNRECTB ;
typedef VTNRect< short    > VNRECTS ;
typedef VTNRect< long     > VNRECTL ;
