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
#include <encmetric/utf16_enc.hpp>

namespace adv{

template<bool be>
int UTF16<be>::chLen(const byte *data){
	//non è necessario fare tutti i controlli, poiché si suppone che la stringa sia corretta
	//usare validChar per effettuare tutti i controlli
	if(utf16_range(data, be))
		return 4;
	else
		return 2;
}

template<bool be>
bool UTF16<be>::validChar(const byte *data, int &add) noexcept{
	if(utf16_H_range(data, be)){
		add = 4;
		if(!utf16_L_range(data+2, false))
			return false;
	}
	else if(utf16_L_range(data, false))
		return false;
	else
		add = 2;
	return true;
}

template<bool be>
int UTF16<be>::decode(unicode *uni, const byte *by, size_t l){
	int y_byte = 0;
	uni = 0;
	
	if(utf16_range(by, be))
		y_byte = 4;
	else
		y_byte = 2;

	if(l < y_byte)
		return 0;

	if(y_byte == 4){
		byte buf[4];
		copy_end(by, 4, be, buf, 2);
		
		leave_bits(buf[0], 0, 1);
		leave_bits(buf[2], 0, 1);
		
		unicode p_word{(read_unicode(buf[0]) << 8) + read_unicode(buf[1])};
		unicode s_word{(read_unicode(buf[2]) << 8) + read_unicode(buf[3])};
		*uni = unicode{(p_word << 10) + s_word + 0x10000};
	}
	else{
		byte buf[2];
		copy_end(by, 2, be, buf, 2);		
		*uni = unicode{(read_unicode(buf[0]) << 8) + read_unicode(buf[1])};
	}
	return y_byte;
}

template<bool be>
int UTF16<be>::encode(const unicode &unin, byte *by, size_t l){
	int y_byte;
	if(unin >= 0 && unin < 0xffff){
		y_byte = 2;
	}
	else if(unin >= 0x10000 && unin < 0x110000){
		y_byte = 4;
	}
	else throw encoding_error("Not Unicode character");

	if(l < y_byte)
		return 0;
	
	if(y_byte == 4){
		unicode uni{unin - 0x10000};
		byte temp[4];
		temp[3] = byte{static_cast<uint8_t>(uni & 0xff)};
		uni=unicode{uni >> 8};
		temp[2] = byte{static_cast<uint8_t>(uni & 0x03)};
		uni=unicode{uni >> 2};
		temp[1] = byte{static_cast<uint8_t>(uni & 0xff)};
		uni=unicode{uni >> 8};
		temp[0] = byte{static_cast<uint8_t>(uni & 0x03)};

		set_bits(temp[2], 7, 6, 4, 3, 2);
		set_bits(temp[0], 7, 6, 4, 3);
		copy_end(temp, 4, be, by, 2);
	}
	else{
		unicode uni = unin;
		byte temp[2];
		temp[1] = byte{static_cast<uint8_t>(uni & 0xff)};
		uni=unicode{uni >> 8};
		temp[0] = byte{static_cast<uint8_t>(uni & 0xff)};
		copy_end(temp, 2, be, by, 2);
	}
	return y_byte;
}

	template class UTF16<true>;
	template class UTF16<false>;
}


