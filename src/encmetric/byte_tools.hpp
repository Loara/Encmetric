#pragma once
/*
    This file is part of Encmetric.
    Copyright (C) 2021 Paolo De Donato.

    Encmetric is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Encmetric is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Encmetric. If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <stdexcept>

namespace adv{
using std::byte;

inline bool compare(const byte *a, const byte *b, int nsiz) noexcept{
	return std::memcmp(a, b, nsiz) == 0;
}

template<typename RET, typename IntegerType>
constexpr RET compose_bit_mask(IntegerType bit){
	static_assert(std::is_integral_v<IntegerType>, "Not integral type");
	if(bit < 0 || bit >= (sizeof(RET)*8))
		throw std::out_of_range("Invalid bit position");
	return RET{1} << bit;
}

template<typename RET, typename IntegerType, typename... T>
constexpr RET compose_bit_mask(IntegerType bit, T... oth){
	return compose_bit_mask<RET>(bit) | compose_bit_mask<RET>(oth...);
}

/*
true if all the selected bits are 0
*/
template<typename B, typename... T>
constexpr bool bit_zero(B test, T... all){
	return (test & compose_bit_mask<B>(all...)) == B{0};
}

/*
true if all the selected bits are 1
*/
template<typename B, typename... T>
constexpr bool bit_one(B test, T... all){
	B mask = compose_bit_mask<B>(all...);
	return (test & mask) == mask;
}

/*
sets these bits
*/
template<typename B, typename... T>
constexpr void set_bits(B &mask, T... all){
	mask |= compose_bit_mask<B>(all...);
}

/*
resets these bits
*/
template<typename B, typename... T>
constexpr void reset_bits(B &mask, T... all){
	mask &= ~compose_bit_mask<B>(all...);
}

/*
resets all other bits
*/
template<typename B, typename... T>
constexpr void leave_bits(B &mask, T... all){
	mask &= compose_bit_mask<B>(all...);
}

/*
access an endianess-dependend arrays as a big endian array
*/

inline constexpr int acc(bool be, int dim, int i){
	return be ? i : dim-1-i;
}

template<typename T>
constexpr T& access(T *by, bool be, int dim, int i){ return by[acc(be, dim, i)];}

// trasforma un array di len byte che memorizza (len/dim) numeri con endianess be in un altro con endianess big endian. 
//Viceversa applicato ad un array big endian lo trasforma in un array con endianess be
inline constexpr void copy_end(const byte *in, int len, bool be, byte *out, int dim){
	if((len % dim) != 0)
		return;
	int steps = len / dim;
	for(int i=0; i<steps; i++){
		for(int j=0; j<dim; j++)
			out[i*dim + j] = access(in+(i*dim), be, dim, j);
	}
}

}


