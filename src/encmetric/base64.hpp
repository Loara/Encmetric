#pragma once

#include <encmetric/encoding.hpp>

namespace adv{

struct byte_data{
	size_t len;
	byte *data;
};

struct Base64_padding{
	using ctype=byte_data;
	static constexpr uint unity() noexcept {return 1;}
	static constexpr bool has_max() noexcept {return false;}
	static constexpr uint max_bytes() noexcept {return 0;}
	static uint chLen(const byte *);
	static bool validChar(const byte *, uint &) noexcept;
	static uint decode(byte_data *uni, const byte *by, size_t l);
	static uint encode(const byte_data &uni, byte *by, size_t l);
};

}
