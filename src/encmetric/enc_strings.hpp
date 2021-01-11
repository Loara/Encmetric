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
#include <encmetric/utf_string.hpp>
#include <encmetric/all_enc.hpp>
#include <encmetric/config.hpp>
#include <type_traits>

namespace adv{

//Encoding predefinito del compilatore
using CENC = UTF8;
inline constexpr bool bend = false;

//encoding per IO del sistema
using IOenc = std::conditional_t<is_windows(), UTF16<bend>, UTF8>;

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
using astr = adv_string<CENC>;
using iochar_pt = tchar_pt<IOenc>;
using c_iochar_pt = const_tchar_pt<IOenc>;
using iostr_view = adv_string_view<IOenc>;
using iostr = adv_string<IOenc>;

inline astr_view getstring(const char *c){
	return astr_view{c_achar_pt{c}};
}

inline astr_view getstring(const char *c, int len){
	return astr_view{c_achar_pt{c}, len, false};
}

inline adv_string_view<UTF16<bend>> getstring_16(const char16_t *c){
	return adv_string_view<UTF16<bend>>{const_tchar_pt<UTF16<bend>>{(const byte *)c}};
}

inline adv_string_view<UTF16<bend>> getstring_16(const char16_t *c, int len){
	return adv_string_view<UTF16<bend>>{const_tchar_pt<UTF16<bend>>{(const byte *)c}, len*2, false};
}

}
