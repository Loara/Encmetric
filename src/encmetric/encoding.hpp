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
/*
    Encoding Metrics classes, with some useful base classes.

    Any implementation of an Encoding Metric must have a typedef 'ctype' represents the type of data you want to encode/decode
    (for character encodings you can use Unicode). Also it must have these function member declared as 
    static:

     - constexpr int unity() noexcept  => minimum number of bytes needed to detect the length of a character
     - constexpr bool has_max() noexcept => the encoding fixes the maximum number of bytes per-character
     - constexpr int max_bytes() noexcept => maximum number of bytes needed to store an entire character, undefined
       if has_max is false
     - int chLen(const byte *)  => the length in bytes of the first character pointed by (can throw
        an encoding_error if the length can't be recognized). The first purpouse of this function
        is only to calculate le length of a character, not to verify it.
     - bool validChar(const byte *, int &) noexcept  => Test is the first character is valid with 
        respect to this encoding. This function sets also the character length in the second argument if
        it is valid (if the character is not valid the status of this argument is undefined). 
     - int decode(T *, const byte *, int)  => sets the Unicode code of the first encoded characters
        and returns the number of bytes read. If there aren't enough bytes it must return 0
     - int encode(const T &, byte *, int)  => encode the Unicode character and writes it in the memory pointed
        and returns the number of bytes written. If there isn't enough space it must return 0

     Both have a integer parameter that represents the size of the string/buffer, and returns the nunber of bytes
     read/written. If the string parameter in to_unicode is too small return 0 without throw an exception 
*/
#include <encmetric/base.hpp>
#include <typeindex>
#include <type_traits>
#include <cstring>

namespace adv{

inline void copyN(const byte *src, byte *des, size_t l) {std::memcpy(des, src, l);}

inline bool is_all_zero(const byte *data, size_t len){
	for(size_t i=0; i<len; i++){
		if(data[i] != byte{0})
			return false;
	}
	return true;
}

class encoding_error : public std::exception{
	private:
		const char *c;
	public:
		encoding_error(const char *ms) : c{ms} {}
		encoding_error() : encoding_error{"EncMetric error"} {}
		const char *what() const noexcept override {return c;}
};
template<typename T>
class EncMetric{
	public:
		using ctype=T;
		virtual ~EncMetric() {}
		virtual int d_unity() const noexcept=0;
		virtual int d_max_bytes() const=0;
		virtual int d_chLen(const byte *) const=0;
		virtual bool d_validChar(const byte *, int &chlen) const noexcept =0;
		virtual int d_decode(ctype *, const byte *, size_t) const =0;
		virtual int d_encode(const ctype &, byte *, size_t) const =0;
		virtual std::type_index index() const noexcept=0;
};

/*
    Placeholder: the encoding is determined at runtime
*/
template<typename T>
class WIDE{
	public:
		using ctype=T;
};

//using WIDENC=WIDE<unicode>;

/*
    No encoding provided
*/
template<typename T>
class RAW{
	public:
		using ctype=T;
		static constexpr int unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr int max_bytes() {return 1;}
		static int chLen(const byte *) {return 1;}
		static bool validChar(const byte *, int &i) noexcept{
			i=1;
			return true;
		}
		static int decode(T *, const byte *, size_t) {throw encoding_error{"RAW encoding can't be converted to Unicode"};}
		static int encode(const T &, byte *, size_t) {throw encoding_error{"RAW encoding can't be converted to Unicode"};}
};

using RAWchr=RAW<unicode>;

template<typename S, typename T>
struct same_data : public std::is_same<typename S::ctype, typename T::ctype> {};
template<typename S, typename T>
inline constexpr bool same_data_v = same_data<S, T>::value;

template<typename U>
struct is_wide : public std::false_type {};
template<typename tt>
struct is_wide<WIDE<tt>> : public std::true_type {};

template<typename T>
inline constexpr bool is_wide_v = is_wide<T>::value;

template<typename U>
struct is_raw : public std::false_type {};
template<typename tt>
struct is_raw<RAW<tt>> : public std::true_type {};

template<typename T>
inline constexpr bool is_raw_v = is_raw<T>::value;

/*
    Template packs are used when you need SFINAE
*/
template<typename S, typename T, typename U, typename...>
struct enable_same_data : public std::enable_if<same_data_v<S, T>, U>{};

template<typename S, typename T, typename U, typename... Args>
using enable_same_data_t = typename enable_same_data<S, T, U, Args...>::type;

template<typename T, typename U, typename...>
struct enable_wide : public std::enable_if<is_wide_v<T>, U> {};
template<typename T, typename U, typename...>
struct enable_not_wide : public std::enable_if<!is_wide_v<T>, U>{};

template<typename T, typename U, typename... Args>
using enable_wide_t = typename enable_wide<T, U, Args...>::type;
template<typename T, typename U, typename... Args>
using enable_not_wide_t = typename enable_not_wide<T, U, Args...>::type;

template<typename T>
inline constexpr bool fixed_size = T::has_max() && (T::unity() == T::max_bytes());
template<typename T>
inline constexpr bool fixed_size<WIDE<T>> = false;

/*
    index_traits control if encoding class ovverides the index
*/
template<typename T, enable_not_wide_t<T, int>, typename = void>
struct index_traits_0{
	static std::type_index index() noexcept {return std::type_index{typeid(T)};}
};

template<typename T>
struct index_traits_0<T, 0, std::void_t<decltype(T::enc_index)>>{
	static std::type_index index() noexcept {return T::enc_index();}
};

template<typename T>
struct index_traits : public index_traits_0<T, 0> {};

/*
    Wrapper of an encoding T in order to save it in a class field of WIDENC classes
*/
template<typename T, enable_not_wide_t<T, int> =0>
class DynEncoding : public EncMetric<typename T::ctype>{
	private:		
		DynEncoding() {}
	public:
		using static_enc = T;

		~DynEncoding() {}

		int d_unity() const noexcept {return static_enc::unity();}
		int d_max_bytes() const {return static_enc::max_bytes();}
		int d_chLen(const byte *b) const {return static_enc::chLen(b);}
		bool d_validChar(const byte *b, int &chlen) const noexcept {return static_enc::validChar(b, chlen);}
		std::type_index index() const noexcept {return index_traits<T>::index();}

		int d_decode(typename T::ctype *uni, const byte *by, size_t l) const {return static_enc::decode(uni, by, l);}
		int d_encode(const typename T::ctype &uni, byte *by, size_t l) const {return static_enc::encode(uni, by, l);}

		constexpr bool d_fixed_size() const noexcept {return fixed_size<T>;}

		static const EncMetric<typename T::ctype> &instance() noexcept{
			static DynEncoding<T> t{};
			return t;
		}
};

template<typename T>
constexpr void assert_raw(){static_assert(!is_raw_v<T>, "Using RAW format");}

template<typename T>
void assert_raw(const EncMetric<T> &f){
	if(f.index() == index_traits<RAW<T>>::index())
		throw encoding_error("Using RAW format");
}

/*
    Some basic encodings
*/
class ASCII{
	public:
		using ctype=unicode;
		static constexpr int unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr int max_bytes() noexcept {return 1;}
		static int chLen(const byte *);
		static bool validChar(const byte *, int &) noexcept;
		static int decode(unicode *uni, const byte *by, size_t l);
		static int encode(const unicode &uni, byte *by, size_t l);
};

class Latin1{
	public:
		using ctype=unicode;
		static constexpr int unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr int max_bytes() noexcept {return 1;}
		static int chLen(const byte *);
		static bool validChar(const byte *, int &) noexcept;
		static int decode(unicode *uni, const byte *by, size_t l);
		static int encode(const unicode &uni, byte *by, size_t l);
};

}


