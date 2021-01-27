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
#include <encmetric/encoding.hpp>

namespace adv{

/*
    Basic class for every ISO-8859-n encoding based on a table search.

    Enc is the class specialization, must have a public static array of unicode
    member table for each character from A0 to FF
*/
template<typename Enc>
class ISO_8859_basic{
	public:
		static constexpr int unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr int max_bytes() noexcept {return 1;}
		static int chLen(const byte *) {return 1;}
		static bool validChar(const byte *, int &chlen) noexcept {chlen=1; return true;}
		static int to_unicode(unicode &uni, const byte *by, int l){
			if(l <= 0)
				throw encoding_error("Not enough bytes");
			if(bit_zero(*by, 7) || bit_zero(*by, 5, 6)){
				uni = std::to_integer<unicode>(by[0]);
				return 1;
			}
			else{
				int idx = std::to_integer<int>(by[0]);
				idx -= 0xa0;
				uni = Enc::table[idx];
				return 1;
			}
		}
		static int from_unicode(unicode uni, byte *by, int l){
			if(l <= 0)
				throw encoding_error("Not enough bytes");
			if(uni < 0xa0){
				*by = byte{static_cast<unsigned char>(uni & 0xff)};
				return 1;
			}
			else{
				int idx = -1;
				for(int i=0; i < (6*16); i++){
					if(Enc::table[i] == uni){
						idx = i;
						break;
					}
				}
				if(idx == -1)
					throw encoding_error("Cannot convert to a ISO-8859 character");
				*by = byte{static_cast<unsigned char>(idx + 0xa0)};
				return 1;
			}
		}
};

using ISO_8859_1 = Latin1;

class ISO_8859_2 : public ISO_8859_basic<ISO_8859_2>{
	public:
		static const unicode table[];
};

}
