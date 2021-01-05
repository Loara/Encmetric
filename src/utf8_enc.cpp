
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
#include <encmetric/utf8_enc.hpp>

using namespace adv;

int UTF8::chLen(const byte *data){
	//non è necessario fare tutti i controlli, poiché si suppone che la stringa sia corretta
	//usare validChar per effettuare tutti i controlli
	byte b = *data;
	if(bit_zero(b, 7))
		return 1;
	else if(bit_zero(b, 6))
		throw encoding_error("Invalid utf8 character");
	else if(bit_zero(b, 5))
		return 2;
	else if(bit_zero(b, 4))
		return 3;
	else if(bit_zero(b, 3))
		return 4;
	else{
		throw encoding_error("Invalid utf8 character");
	}
}

bool UTF8::validChar(const byte *data, int &add) noexcept{
	byte b = *data;

	if(bit_zero(b, 7))
		add = 1;
	else if(bit_zero(b, 6))
		return false;
	else if(bit_zero(b, 5))
		add = 2;
	else if(bit_zero(b, 4))
		add = 3;
	else if(bit_zero(b, 3))
		add = 4;
	else
		return false;
	for(int i=1; i<add; i++){
		b=data[i];
		if(bit_zero(b, 7) || bit_one(b, 6))
			return false;
	}
	return true;
}

int UTF8::to_unicode(unicode &uni, const byte *by, int l){
	int y_byte = 0;
	byte b = *by;
	uni = 0;
	
	if(bit_zero(b, 7)){
		y_byte = 1;
	}
	else if(bit_zero(b, 6))
		throw encoding_error("Invalid utf8 character");
	else if(bit_zero(b, 5)){
		y_byte = 2;
		reset_bits(b, 7, 6);
	}
	else if(bit_zero(b, 4)){
		y_byte = 3;
		reset_bits(b, 7, 6, 5);
	}
	else if(bit_zero(b, 3)){
		y_byte = 4;
		reset_bits(b, 7, 6, 5, 4);
	}
	else
		throw encoding_error("Invalid utf8 character");

	if(l < y_byte)
		throw encoding_error("Not enough bytes");
	uni = std::to_integer<unicode>(b);
	for(int i = 1; i < y_byte; i++){
		byte temp = by[i];
		reset_bits(temp, 6, 7);
		uni = (uni << 6) + std::to_integer<unicode>(temp);
	}
	return y_byte;
}

int UTF8::from_unicode(unicode uni, byte *by, int l){
	int y_byte;
	byte set_mask{0};
	//byte reset_mask{0}; Non è necessario
	if(uni >= 0 && uni < 0x80){
		y_byte = 1;
	}
	else if(uni >= 0x80 && uni < 0x800){
		y_byte = 2;
		set_mask = compose_bit_mask<byte>(7, 6);
	}
	else if(uni >= 0x800 && uni < 0x10000){
		y_byte = 3;
		set_mask = compose_bit_mask<byte>(7, 6, 5);
	}
	else if(uni >= 0x10000 && uni < 0x110000){
		y_byte = 4;
		set_mask = compose_bit_mask<byte>(7, 6, 5, 4);
	}
	else throw encoding_error("Not Unicode character");

	if(l < y_byte)
		return 0;
	for(int i = y_byte-1; i>=1; i--){
		by[i] = byte{static_cast<unsigned char>(uni & 0x3f)};
		uni >>= 6;
		set_bits(by[i], 7);
	}
	by[0] = byte{static_cast<unsigned char>(uni)};
	by[0] |= set_mask;
	return y_byte;
}


