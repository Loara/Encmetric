#pragma once

#include <encmetric/encoding.hpp>
#include <encmetric/enc_string.hpp>

namespace adv{

struct three_byte{
	byte *bytes;
	uint nbyte;
};

class Base64_padding{
	public:
		using ctype=three_byte;
		static constexpr unsigned char recode[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
		static int find(byte b) noexcept{
			if(std::to_integer<unsigned char>(b) == '=')
				return 64;		
			for(int i=0; i<64; i++){
				if(std::to_integer<unsigned char>(b) == recode[i])
					return i;
			}
			return -1;
		}
		static constexpr uint unity() noexcept {return 4;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr uint max_bytes() noexcept {return 4;}
		static constexpr uint chLen(const byte *) {return 4;}
		static constexpr uint encLen(const three_byte &) {return 4;}
		static bool validChar(const byte *, uint &) noexcept;
		static uint decode(three_byte *uni, const byte *by, size_t l);
		static uint encode(const three_byte &uni, byte *by, size_t l);
};

void base64_encode(const byte *from, byte *to, size_t siz);
void base64_decode(const byte *from, byte *to, size_t siz);

}
