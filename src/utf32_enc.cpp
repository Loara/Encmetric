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
#define encmetric_library
#include <encmetric/utf32_enc.hpp>

namespace adv{
template<bool be>
bool UTF32<be>::validChar(const byte *data, int &add) noexcept{
	if(access(data, be, 4, 0) != byte{0})
		return false;
	byte rew = access(data, be, 4, 1);
	if(!bit_zero(rew, 7, 6, 5))
		return false;
	if(bit_one(rew, 4) && !bit_zero(rew, 3, 2, 1, 0))
		return false;
	return true;
}

template<bool be>
int UTF32<be>::to_unicode(unicode &uni, const byte *by, size_t l){
	if(l < 4)
		throw encoding_error("Not enough bytes");
	uni = 0;
	for(int i=0; i<4; i++){
		uni = (uni << 8) + std::to_integer<unicode>(access(by, be, 4, i));
	}
	return 4;
}

template<bool be>
int UTF32<be>::from_unicode(unicode uni, byte *by, size_t l){
	byte temp[4];
	for(int i=0; i<4; i++){
		temp[3-i] = byte{static_cast<unsigned char>(uni & 0xff)};
		uni >>= 8;
	}
	copy_end(temp, 4, be, by, 4);
	return 4;
}

	template class UTF32<true>;
	template class UTF32<false>;
}


