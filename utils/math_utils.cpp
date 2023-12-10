#include <math_utils.hpp>


long lcm( long lhs, long rhs )
{
    return lhs * rhs / gcd( lhs, rhs );
}

long gcd( long lhs, long rhs )
{
    if( rhs == 0L )
    {
        return lhs;
    }

    return gcd( rhs, lhs % rhs );
}
