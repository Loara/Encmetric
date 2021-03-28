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
#include <encmetric/enc_io_core.hpp>
#include <type_traits>

namespace adv{

//System encoding for IO operations
using IOenc = std::conditional_t<is_windows(), UTF16<false>, UTF8>;

//explicit declaration of template - for compilation improvment
using iochar_pt = tchar_pt<IOenc>;
using c_iochar_pt = const_tchar_pt<IOenc>;
using iostr_view = adv_string_view<IOenc>;
template<typename U = std::allocator<byte>>
using iostr_d = adv_string<IOenc, U>;
using iostr = iostr_d<>;

size_t stdin_getChrs(iochar_pt, size_t);
size_t stdout_putChrs(c_iochar_pt, size_t);
size_t stderr_putChrs(c_iochar_pt, size_t);

}




