#pragma once

#include <unordered_map>


// Least common multiple
long lcm( long lhs, long rhs );

// Greatest common divisor
long gcd( long lhs, long rhs );


std::unordered_map< long, long > computePrimeFactors( long n );
