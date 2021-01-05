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
class UTF16{
	public:
		static constexpr int unity() noexcept {return 2;}
		static int chLen(const byte *);
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
using UTF16LE = UTF16<false>;
using UTF16BE = UTF16<true>;

//w_st_pt utf16_endianess(const byte *, int len, const EncMetric &def = dyn_encoding<UTF16BE>::instance());

/*
controlla se la coppia di byte che contiene le cifre più significative (nella prima coppia) è valida
*/
inline constexpr bool utf16_H_range(const byte *datas, bool be) noexcept{
	byte data = access(datas, be, 2, 0);//accede al byte con le cifre più significative
	return bit_one(data, 7, 6, 4, 3) && bit_zero(data, 5, 2);
}

/*
controlla se la coppia di byte che contiene le cifre meno significative (nella seconda) è valido
*/
inline constexpr bool utf16_L_range(const byte *datas, bool be) noexcept{
	byte data = access(datas, be, 2, 0);
	return bit_one(data, 7, 6, 4, 3, 2) && bit_zero(data, 5);
}

/*
controlla se la coppia di byte fa parte di un carattere a 4 bytes
*/
inline constexpr bool utf16_range(const byte *datas, bool be) noexcept{
	byte data = access(datas, be, 2, 0);
	return bit_one(data, 7, 6, 4, 3) && bit_zero(data, 5);
}

/*
Trasforma una word con endianess be in una word big endian
e trasforma una word big endian in una word con endianess be

i puntatori NON possono puntare alla medesima locazione di memoria
inline constexpr void utf16_switch(byte *b, const byte *in, bool be) noexcept{
	if(be){
		b[0] = in[0];
		b[1] = in[1];
	}
	else{
		b[0] = in[1];
		b[1] = in[0];
	}
}
*/

#include <encmetric/utf16_enc.tpp>
}
