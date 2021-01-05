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
#include <encmetric/chite.hpp>
#include <encmetric/byte_tools.hpp>

namespace adv{

template<bool be>
class UTF32{
	public:
		static constexpr int unity() noexcept {return 4;}
		static int chLen(const byte *){ return 4;}
		static bool validChar(const byte *, int &chlen) noexcept;
		static int to_unicode(unicode &uni, const byte *by, int l);
		static int from_unicode(unicode uni, byte *by, int l);
};
/*
namespace u16_c{

template<bool be>
inline constexpr int LB = be ? 1 : 0;
template<bool be>
inline constexpr int HB = be ? 0 : 1;

}
*/
using UTF32LE = UTF32<false>;
using UTF32BE = UTF32<true>;

#include <encmetric/utf32_enc.tpp>
}