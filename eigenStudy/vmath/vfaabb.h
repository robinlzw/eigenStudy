#ifndef __EASPRT_VFAABB_H__
#define __EASPRT_VFAABB_H__

#include "vmathpredef.h"
#include <assert.h>
#include "vfmatrix4.h"
#include <algorithm>

// Axis Aligned Bound Box
struct VFAABB 
{
    VFVECTOR3  vMinimize ;
    VFVECTOR3  vMaximize ;
} ;

class VFAxisAlignedBox
{
public:
    enum Extent
    {
        EXTENT_NULL,
        EXTENT_FINITE,
        EXTENT_INFINITE
    };
protected:

    VFVector3 mMinimum;
    VFVector3 mMaximum;
    Extent mExtent;
    mutable VFVector3* mCorners;

public:
    /*
    1-----2
    /|    /|
    / |   / |
    5-----4  |
    |  0--|--3
    | /   | /
    |/    |/
    6-----7
    */
    typedef enum {
        FAR_LEFT_BOTTOM = 0,
        FAR_LEFT_TOP = 1,
        FAR_RIGHT_TOP = 2,
        FAR_RIGHT_BOTTOM = 3,
        NEAR_RIGHT_BOTTOM = 7,
        NEAR_LEFT_BOTTOM = 6,
        NEAR_LEFT_TOP = 5,
        NEAR_RIGHT_TOP = 4
    } CornerEnum;
    inline VFAxisAlignedBox() : mMinimum(VFVector3::ZERO), mMaximum(VFVector3::UNIT_SCALE), mCorners(0)
    {
        // Default to a null box 
        setMinimum( -0.5, -0.5, -0.5 );
        setMaximum( 0.5, 0.5, 0.5 );
        mExtent = EXTENT_NULL;
    }
    inline VFAxisAlignedBox(Extent e) : mMinimum(VFVector3::ZERO), mMaximum(VFVector3::UNIT_SCALE), mCorners(0)
    {
        setMinimum( -0.5, -0.5, -0.5 );
        setMaximum( 0.5, 0.5, 0.5 );
        mExtent = e;
    }

    inline VFAxisAlignedBox(const VFAxisAlignedBox & rkBox) : mMinimum(VFVector3::ZERO), mMaximum(VFVector3::UNIT_SCALE), mCorners(0)

    {
        if (rkBox.isNull())
            setNull();
        else if (rkBox.isInfinite())
            setInfinite();
        else
            setExtents( rkBox.mMinimum, rkBox.mMaximum );
    }

    inline VFAxisAlignedBox( const VFVector3& min, const VFVector3& max ) : mMinimum(VFVector3::ZERO), mMaximum(VFVector3::UNIT_SCALE), mCorners(0)
    {
        setExtents( min, max );
    }

    inline VFAxisAlignedBox(
        float mx, float my, float mz,
        float Mx, float My, float Mz ) : mMinimum(VFVector3::ZERO), mMaximum(VFVector3::UNIT_SCALE), mCorners(0)
    {
        setExtents( mx, my, mz, Mx, My, Mz );
    }

    VFAxisAlignedBox& operator=(const VFAxisAlignedBox& rhs)
    {
        // Specifically override to avoid copying mCorners
        if (rhs.isNull())
            setNull();
        else if (rhs.isInfinite())
            setInfinite();
        else
            setExtents(rhs.mMinimum, rhs.mMaximum);

        return *this;
    }

    ~VFAxisAlignedBox()
    {
        if (mCorners)
            delete[] mCorners ;
    }


    inline const VFVector3& getMinimum(void) const
    { 
        return mMinimum; 
    }

    inline VFVector3& getMinimum(void)
    { 
        return mMinimum; 
    }

    inline const VFVector3& getMaximum(void) const
    { 
        return mMaximum;
    }

    inline VFVector3& getMaximum(void)
    { 
        return mMaximum;
    }


    inline void setMinimum( const VFVector3& vec )
    {
        mExtent = EXTENT_FINITE;
        mMinimum = vec;
    }

    inline void setMinimum( float x, float y, float z )
    {
        mExtent = EXTENT_FINITE;
        mMinimum.x = x;
        mMinimum.y = y;
        mMinimum.z = z;
    }

    inline void setMinimumX(float x)
    {
        mMinimum.x = x;
    }

    inline void setMinimumY(float y)
    {
        mMinimum.y = y;
    }

    inline void setMinimumZ(float z)
    {
        mMinimum.z = z;
    }

    inline void setMaximum( const VFVector3& vec )
    {
        mExtent = EXTENT_FINITE;
        mMaximum = vec;
    }

    inline void setMaximum( float x, float y, float z )
    {
        mExtent = EXTENT_FINITE;
        mMaximum.x = x;
        mMaximum.y = y;
        mMaximum.z = z;
    }

    inline void setMaximumX( float x )
    {
        mMaximum.x = x;
    }

    inline void setMaximumY( float y )
    {
        mMaximum.y = y;
    }

    inline void setMaximumZ( float z )
    {
        mMaximum.z = z;
    }

    inline void setExtents( const VFVector3& min, const VFVector3& max )
    {
        assert( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
            "The minimum corner of the box must be less than or equal to maximum corner" );

        mExtent = EXTENT_FINITE;
        mMinimum = min;
        mMaximum = max;
    }

    inline void setExtents(
        float mx, float my, float mz,
        float Mx, float My, float Mz )
    {
        assert( (mx <= Mx && my <= My && mz <= Mz) &&
            "The minimum corner of the box must be less than or equal to maximum corner" );

        mExtent = EXTENT_FINITE;

        mMinimum.x = mx;
        mMinimum.y = my;
        mMinimum.z = mz;

        mMaximum.x = Mx;
        mMaximum.y = My;
        mMaximum.z = Mz;

    }

    inline const VFVector3* getAllCorners(void) const
    {
        assert( (mExtent == EXTENT_FINITE) && "Can't get corners of a null or infinite AAB" );

        // The order of these items is, using right-handed co-ordinates:
        // Minimum Z face, starting with Min(all), then anticlockwise
        //   around face (looking onto the face)
        // Maximum Z face, starting with Max(all), then anticlockwise
        //   around face (looking onto the face)
        // Only for optimization/compatibility.
        if (!mCorners)
            mCorners = new VFVector3[8] ;

        mCorners[0] = mMinimum;
        mCorners[1].x = mMinimum.x; mCorners[1].y = mMaximum.y; mCorners[1].z = mMinimum.z;
        mCorners[2].x = mMaximum.x; mCorners[2].y = mMaximum.y; mCorners[2].z = mMinimum.z;
        mCorners[3].x = mMaximum.x; mCorners[3].y = mMinimum.y; mCorners[3].z = mMinimum.z;            

        mCorners[4] = mMaximum;
        mCorners[5].x = mMinimum.x; mCorners[5].y = mMaximum.y; mCorners[5].z = mMaximum.z;
        mCorners[6].x = mMinimum.x; mCorners[6].y = mMinimum.y; mCorners[6].z = mMaximum.z;
        mCorners[7].x = mMaximum.x; mCorners[7].y = mMinimum.y; mCorners[7].z = mMaximum.z;

        return mCorners;
    }

    VFVector3 getCorner(CornerEnum cornerToGet) const
    {
        switch(cornerToGet)
        {
        case FAR_LEFT_BOTTOM:
            return mMinimum;
        case FAR_LEFT_TOP:
            return VFVector3(mMinimum.x, mMaximum.y, mMinimum.z);
        case FAR_RIGHT_TOP:
            return VFVector3(mMaximum.x, mMaximum.y, mMinimum.z);
        case FAR_RIGHT_BOTTOM:
            return VFVector3(mMaximum.x, mMinimum.y, mMinimum.z);
        case NEAR_RIGHT_BOTTOM:
            return VFVector3(mMaximum.x, mMinimum.y, mMaximum.z);
        case NEAR_LEFT_BOTTOM:
            return VFVector3(mMinimum.x, mMinimum.y, mMaximum.z);
        case NEAR_LEFT_TOP:
            return VFVector3(mMinimum.x, mMaximum.y, mMaximum.z);
        case NEAR_RIGHT_TOP:
            return mMaximum;
        default:
            return VFVector3();
        }
    }

    // friend std::ostream& operator<<( std::ostream& o, const VFAxisAlignedBox aab )
    //{
    //    switch (aab.mExtent)
    //    {
    //    case EXTENT_NULL:
    //        o << "VFAxisAlignedBox(null)";
    //        return o;

    //    case EXTENT_FINITE:
    //        o << "VFAxisAlignedBox(min=" << aab.mMinimum << ", max=" << aab.mMaximum << ")";
    //        return o;

    //    case EXTENT_INFINITE:
    //        o << "VFAxisAlignedBox(infinite)";
    //        return o;

    //    default: // shut up compiler
    //        assert( false && "Never reached" );
    //        return o;
    //    }
    //}

    void merge( const VFAxisAlignedBox& rhs )
    {
        // Do nothing if rhs null, or this is infinite
        if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
        {
            return;
        }
        // Otherwise if rhs is infinite, make this infinite, too
        else if (rhs.mExtent == EXTENT_INFINITE)
        {
            mExtent = EXTENT_INFINITE;
        }
        // Otherwise if current null, just take rhs
        else if (mExtent == EXTENT_NULL)
        {
            setExtents(rhs.mMinimum, rhs.mMaximum);
        }
        // Otherwise merge
        else
        {
            VFVector3 min = mMinimum;
            VFVector3 max = mMaximum;
            max.MakeCeil(rhs.mMaximum);
            min.MakeFloor(rhs.mMinimum);

            setExtents(min, max);
        }

    }

    inline void merge( const VFVector3& point )
    {
        switch (mExtent)
        {
        case EXTENT_NULL: // if null, use this point
            setExtents(point, point);
            return;

        case EXTENT_FINITE:
            mMaximum.MakeCeil(point);
            mMinimum.MakeFloor(point);
            return;

        case EXTENT_INFINITE: // if infinite, makes no difference
            return;
        }

        assert( false && "Never reached" );
    }

    inline void transform( const VFMatrix4& matrix )
    {
        // Do nothing if current null or infinite
        if( mExtent != EXTENT_FINITE )
            return;

        VFVector3 oldMin, oldMax, currentCorner;

        // Getting the old values so that we can use the existing merge method.
        oldMin = mMinimum;
        oldMax = mMaximum;

        // reset
        setNull();

        // We sequentially compute the corners in the following order :
        // 0, 6, 5, 1, 2, 4 ,7 , 3
        // This sequence allows us to only change one member at a time to get at all corners.

        // For each one, we transform it using the matrix
        // Which gives the resulting point and merge the resulting point.

        // First corner 
        // min min min
        currentCorner = oldMin;
        merge( matrix * currentCorner );

        // min,min,max
        currentCorner.z = oldMax.z;
        merge( matrix * currentCorner );

        // min max max
        currentCorner.y = oldMax.y;
        merge( matrix * currentCorner );

        // min max min
        currentCorner.z = oldMin.z;
        merge( matrix * currentCorner );

        // max max min
        currentCorner.x = oldMax.x;
        merge( matrix * currentCorner );

        // max max max
        currentCorner.z = oldMax.z;
        merge( matrix * currentCorner );

        // max min max
        currentCorner.y = oldMin.y;
        merge( matrix * currentCorner );

        // max min min
        currentCorner.z = oldMin.z;
        merge( matrix * currentCorner ); 
    }

    void TransformAffine(const VFMatrix4& m)
    {
        assert(m.IsAffine());

        // Do nothing if current null or infinite
        if ( mExtent != EXTENT_FINITE )
            return;

        VFVector3 centre = getCenter();
        VFVector3 halfSize = getHalfSize();

        VFVector3 newCentre = m.TransformAffine(centre);
        VFVector3 newHalfSize(
            VFMath::Abs(m[0][0]) * halfSize.x + VFMath::Abs(m[0][1]) * halfSize.y + VFMath::Abs(m[0][2]) * halfSize.z, 
            VFMath::Abs(m[1][0]) * halfSize.x + VFMath::Abs(m[1][1]) * halfSize.y + VFMath::Abs(m[1][2]) * halfSize.z,
            VFMath::Abs(m[2][0]) * halfSize.x + VFMath::Abs(m[2][1]) * halfSize.y + VFMath::Abs(m[2][2]) * halfSize.z);

        setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
    }

    inline void setNull()
    {
        mExtent = EXTENT_NULL;
    }

    inline bool isNull(void) const
    {
        return (mExtent == EXTENT_NULL);
    }

    bool isFinite(void) const
    {
        return (mExtent == EXTENT_FINITE);
    }

    inline void setInfinite()
    {
        mExtent = EXTENT_INFINITE;
    }

    bool isInfinite(void) const
    {
        return (mExtent == EXTENT_INFINITE);
    }

    inline bool intersects(const VFAxisAlignedBox& b2) const
    {
        // Early-fail for nulls
        if (this->isNull() || b2.isNull())
            return false;

        // Early-success for infinites
        if (this->isInfinite() || b2.isInfinite())
            return true;

        // Use up to 6 separating planes
        if (mMaximum.x < b2.mMinimum.x)
            return false;
        if (mMaximum.y < b2.mMinimum.y)
            return false;
        if (mMaximum.z < b2.mMinimum.z)
            return false;

        if (mMinimum.x > b2.mMaximum.x)
            return false;
        if (mMinimum.y > b2.mMaximum.y)
            return false;
        if (mMinimum.z > b2.mMaximum.z)
            return false;

        // otherwise, must be intersecting
        return true;

    }

    inline VFAxisAlignedBox intersection(const VFAxisAlignedBox& b2) const
    {
        if (this->isNull() || b2.isNull())
        {
            return VFAxisAlignedBox();
        }
        else if (this->isInfinite())
        {
            return b2;
        }
        else if (b2.isInfinite())
        {
            return *this;
        }

        VFVector3 intMin = mMinimum;
        VFVector3 intMax = mMaximum;

        intMin.MakeCeil(b2.getMinimum());
        intMax.MakeFloor(b2.getMaximum());

        // Check intersection isn't null
        if (intMin.x < intMax.x &&
            intMin.y < intMax.y &&
            intMin.z < intMax.z)
        {
            return VFAxisAlignedBox(intMin, intMax);
        }

        return VFAxisAlignedBox();
    }

    float volume(void) const
    {
        switch (mExtent)
        {
        case EXTENT_NULL:
            return 0.0f;

        case EXTENT_FINITE:
            {
                VFVector3 diff = mMaximum - mMinimum;
                return diff.x * diff.y * diff.z;
            }

        case EXTENT_INFINITE:
            return VFMath::POS_INFINITY;

        default: // shut up compiler
            assert( false && "Never reached" );
            return 0.0f;
        }
    }

    inline void scale(const VFVector3& s)
    {
        // Do nothing if current null or infinite
        if (mExtent != EXTENT_FINITE)
            return;

        // NB assumes centered on origin
        VFVector3 min = mMinimum * s;
        VFVector3 max = mMaximum * s;
        setExtents(min, max);
    }

    bool intersects(const VFSphere& s) const
    {
        return VFMath::intersects(s, *this); 
    }
    
    bool intersects(const VFPlane & p) const
    {
        return VFMath::intersects(p, *this);
    }

    bool intersects(const VFVector3& v) const
    {
        switch (mExtent)
        {
        case EXTENT_NULL:
            return false;

        case EXTENT_FINITE:
            return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  && 
                v.y >= mMinimum.y  &&  v.y <= mMaximum.y  && 
                v.z >= mMinimum.z  &&  v.z <= mMaximum.z);

        case EXTENT_INFINITE:
            return true;

        default: // shut up compiler
            assert( false && "Never reached" );
            return false;
        }
    }
    VFVector3 getCenter(void) const
    {
        assert( (mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB" );

        return VFVector3(
            (mMaximum.x + mMinimum.x) * 0.5f,
            (mMaximum.y + mMinimum.y) * 0.5f,
            (mMaximum.z + mMinimum.z) * 0.5f);
    }
    VFVector3 getSize(void) const
    {
        switch (mExtent)
        {
        case EXTENT_NULL:
            return VFVector3::ZERO;

        case EXTENT_FINITE:
            return mMaximum - mMinimum;

        case EXTENT_INFINITE:
            return VFVector3(
                VFMath::POS_INFINITY,
                VFMath::POS_INFINITY,
                VFMath::POS_INFINITY);

        default: // shut up compiler
            assert( false && "Never reached" );
            return VFVector3::ZERO;
        }
    }
    VFVector3 getHalfSize(void) const
    {
        switch (mExtent)
        {
        case EXTENT_NULL:
            return VFVector3::ZERO;

        case EXTENT_FINITE:
            return (mMaximum - mMinimum) * 0.5;

        case EXTENT_INFINITE:
            return VFVector3(
                VFMath::POS_INFINITY,
                VFMath::POS_INFINITY,
                VFMath::POS_INFINITY);

        default: // shut up compiler
            assert( false && "Never reached" );
            return VFVector3::ZERO;
        }
    }

    bool contains(const VFVector3& v) const
    {
        if (isNull())
            return false;
        if (isInfinite())
            return true;

        return mMinimum.x <= v.x && v.x <= mMaximum.x &&
            mMinimum.y <= v.y && v.y <= mMaximum.y &&
            mMinimum.z <= v.z && v.z <= mMaximum.z;
    }

    float distance(const VFVector3& v) const ; 

    bool contains(const VFAxisAlignedBox& other) const
    {
        if (other.isNull() || this->isInfinite())
            return true;

        if (this->isNull() || other.isInfinite())
            return false;

        return this->mMinimum.x <= other.mMinimum.x &&
            this->mMinimum.y <= other.mMinimum.y &&
            this->mMinimum.z <= other.mMinimum.z &&
            other.mMaximum.x <= this->mMaximum.x &&
            other.mMaximum.y <= this->mMaximum.y &&
            other.mMaximum.z <= this->mMaximum.z;
    }

    bool operator== (const VFAxisAlignedBox& rhs) const
    {
        if (this->mExtent != rhs.mExtent)
            return false;

        if (!this->isFinite())
            return true;

        return this->mMinimum == rhs.mMinimum &&
            this->mMaximum == rhs.mMaximum;
    }

    bool operator!= (const VFAxisAlignedBox& rhs) const
    {
        return !(*this == rhs);
    }

    // special values
    static const VFAxisAlignedBox BOX_NULL;
    static const VFAxisAlignedBox BOX_INFINITE;


};

#endif //__EASPRT_VFAABB_H__