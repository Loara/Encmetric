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
#include <encmetric/enc_string.hpp>
#include <encmetric/all_enc.hpp>
#include <encmetric/config.hpp>
#include <type_traits>
//#include <iostream>

namespace adv{

//C compiler default encoding for string literals
using CENC = UTF8;
inline constexpr bool bend = is_be();


//explicit declaration of template - for compilation improvment
#ifndef encmetric_library
extern template class tchar_pt<CENC>;
extern template class const_tchar_pt<CENC>;
extern template class adv_string_view<CENC>;
extern template class adv_string<CENC>;
#endif

//better names
using achar_pt = tchar_pt<CENC>;
using c_achar_pt = const_tchar_pt<CENC>;
using astr_view = adv_string_view<CENC>;
template<typename U = std::allocator<byte>>
using astr_d = adv_string<CENC, U>;
using astr = astr_d<>;

inline astr_view getstring(const char *c){
	return astr_view{c_achar_pt{c}};
}

inline astr_view getstring(const char *c, size_t len){
	return astr_view{c_achar_pt{c}, len, false};
}

inline adv_string_view<UTF16<bend>> getstring_16(const char16_t *c){
	return adv_string_view<UTF16<bend>>{const_tchar_pt<UTF16<bend>>{(const byte *)c}};
}

inline adv_string_view<UTF16<bend>> getstring_16(const char16_t *c, size_t len){
	return adv_string_view<UTF16<bend>>{const_tchar_pt<UTF16<bend>>{(const byte *)c}, len*2, false};


}

/*
    Detect utf8, utf16 encoding from BOM, if haven't BOM then throw exception
*/
inline const EncMetric<unicode> *detect_bom(adv_string_view<RAW<unicode>> t){
	const_tchar_pt<RAW<unicode>> ptr = t.begin();
	if(t.size() < 2)
		throw encoding_error{"No BOM"};
	if(ptr[0] == byte{0xfe} && ptr[1] == byte{0xff})
		return DynEncoding<UTF16<true>>::instance();
	if(ptr[0] == byte{0xff} && ptr[1] == byte{0xfe})
		return DynEncoding<UTF16<false>>::instance();
	if(t.size() < 3)
		throw encoding_error{"No BOM"};
	if(ptr[0] == byte{0xef} && ptr[1] == byte{0xbb} && ptr[2] == byte{0xbf})
		return DynEncoding<UTF8>::instance();
	throw encoding_error{"No BOM"};
	
}

inline namespace literals{
inline namespace astr_literals{
inline const byte * operator"" _raw(const char *c, std::size_t){
	return (const byte *)c;
}

inline astr_view operator"" _asv(const char *b, std::size_t st){
	return astr_view{b, st, meas::size};
}
inline adv_string_view<UTF16<bend>> operator"" _asv(const char16_t *b, std::size_t st){
	return adv_string_view<UTF16<bend>>{(const byte *)b, st * 2, meas::size};
}
inline adv_string_view<UTF32<bend>> operator"" _asv(const char32_t *b, std::size_t st){
	return adv_string_view<UTF32<bend>>{(const byte *)b, st * 4, meas::size};
}

}
}

}
