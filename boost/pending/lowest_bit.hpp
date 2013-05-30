// -----------------------------------------------------------
// lowest_bit.hpp
//
//           Position of the lowest bit 'on'
//
//         Copyright (c) 2003-2004, 2008 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_LOWEST_BIT_HPP_GP_20030301
#define BOOST_LOWEST_BIT_HPP_GP_20030301


// tests do pass with or without:
#define BOOST_LOWEST_BIT_USE_INTRINSICS

// but it's not yet verified whether these are faster for current CPUs (or maybe
// faster for 32-bit but slower than 64-bit) - may be linear time instead of
// logarithmic, but more icache friendly for sure

#include <assert.h>
#include <boost/config.hpp>
#include <boost/pending/integer_log2.hpp>

#ifdef _MSC_VER
# include <intrin.h>
#endif

namespace boost {

/// return index of lowest set bit
    template <typename T>
    int lowest_bit(T x) {

        assert(x >= 1); // PRE

        // clear all bits on except the rightmost one,
        // then calculate the logarithm base 2
        //
        return boost::integer_log2<T>( x - ( x & (x-1) ) );

    }

/// return index of highest set bit
template <typename T>
int highest_bit(T x) {
  assert(x >= 1); // PRE
  return boost::integer_log2(x);
}

#ifdef BOOST_LOWEST_BIT_USE_INTRINSICS

#ifdef _MSC_VER

int lowest_bit(unsigned long x) {
  assert(x); // PRE
  unsigned long index;
  _BitScanForward(&index, x);
  return (int)index;
}

int lowest_bit(__int64 x) {
  assert(x); // PRE
  unsigned long index;
  _BitScanForward64(&index, x);
  return (int)index;
}

int highest_bit(unsigned long x) {
  assert(x); // PRE
  unsigned long index;
  _BitScanReverse(&index, x);
  return (int)index;
}

int highest_bit(__int64 x) {
  assert(x); // PRE
  unsigned long index;
  _BitScanReverse64(&index, x);
  return (int)index;
}

#elif BOOST_BUILTIN_CTZ

int lowest_bit(unsigned x) {
  assert(x); // PRE
  return __builtin_ctz(x);
}

int lowest_bit(unsigned long x) {
  assert(x); // PRE
  return __builtin_ctzl(x);
}

int lowest_bit(unsigned long long x) {
  assert(x); // PRE
  return __builtin_ctzll(x);
}

int highest_bit(unsigned x) {
  assert(x); // PRE
  return __builtin_clz(x);
}

int highest_bit(unsigned long x) {
  assert(x); // PRE
  return __builtin_clzl(x);
}

int highest_bit(unsigned long long x) {
  assert(x); // PRE
  return __builtin_clzll(x);
}
#endif

#endif
}


#endif // include guard
