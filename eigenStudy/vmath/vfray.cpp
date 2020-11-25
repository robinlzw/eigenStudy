#include "vfray.h"
#include "vfplane.h"

std::pair< bool, float > VFRay::Intersects( const VFPlane & plane ) const
{ 
    float denom = plane.normal.Dot( GetDirection() ) ;
    if ( fabs( denom ) < VF_EPS_2 )
    {
        // Parallel
        return std::pair<bool, float >( false, 0.0f );
    }
    else
    {
        float nom = plane.normal.Dot( GetOrigin() ) + plane.d ;
        float t = -(nom/denom);
        return std::pair<bool, float>( t >= 0 , t ) ;
    }
}
