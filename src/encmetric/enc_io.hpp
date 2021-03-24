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
#include <type_traits>
#include <iostream>
#include <string>

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

/*
	encoding-aware IO stream, BEWARE not fully tested
*/
template<typename stream, typename T, enable_not_wide_t<T, int> v>
class stream_wrapper{
	private:
		stream &val;
	public:
		stream_wrapper(stream &str) : val{str} {}
		stream &basic_stream() noexcept{return val;}
		/*
		template<typename U>
		stream_wrapper<stream, T, v> &operator>>(U&& ty){
			val >> std::forward<U>(vty);
			return *this;
		}
		*/
		template<typename U>
		stream_wrapper<stream, T, v> &operator<<(U&& ty){
			val << std::forward<U>(ty);
			return *this;
		}
		stream_wrapper<stream, T, v> & operator<<(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&) ){
			val << func;
			return *this;
		}
		stream_wrapper<stream, T, v> &operator<<(iostr_view ty){
			val.write(ty.raw(), ty.size() * sizeof(byte));
			return *this;
		}
		template<typename U>
		stream_wrapper<stream, T, v> &operator>>(U&& ty){
			val >> std::forward<U>(ty);
			return *this;
		}
		stream_wrapper<stream, T, v> & operator>>(std::basic_ostream<char, std::char_traits<char>>& (*func)(std::basic_ostream<char, std::char_traits<char>>&) ){
			val >> func;
			return *this;
		}
		template<typename U = std::allocator<byte>>
		iostr_d<U> getline(const U &alloc = U{}){
			std::string afk{};
			getline(val, afk);
			//std::cout << "Ok" << std::endl;
			const char *data = afk.c_str();
			size_t size = afk.size();
			iostr_view view{data, size, true};
			return iostr_d<U>{view};
		}
};
/*
template<typename stream, typename T, enable_not_wide_t<T, int> v>
stream_wrapper<stream, T, v> &endl(stream_wrapper<stream, T, v> &str){
	str.basic_stream() << std::endl;
	return str;
}
*/

inline stream_wrapper<std::istream, IOenc, 0> ain{std::cin};
inline stream_wrapper<std::ostream, IOenc, 0> aout{std::cout};
inline stream_wrapper<std::ostream, IOenc, 0> aerr{std::cerr};

}




