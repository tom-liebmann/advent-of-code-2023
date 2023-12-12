#include <math_utils.hpp>

#include <cmath>


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

std::unordered_map< long, long > computePrimeFactors( long n )
{
    auto factors = std::unordered_map< long, long >{};

    while( n % 2 == 0 )
    {
        ++factors[ 2 ];
        n = n / 2;
    }

    for( auto i = 3L; i <= std::sqrt( n ); i = i + 2 )
    {
        while( n % i == 0 )
        {
            ++factors[ i ];
            n = n / i;
        }
    }

    if( n > 2 )
    {
        ++factors[ n ];
    }

    return factors;
}
